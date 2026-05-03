#include "ifcopenshell_api.h"
#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Note: project-specific headers (geometry, serializers, schema, etc.) are
// pulled in transitively via ifcopenshell_api_internal.hpp; do not re-include them
// here to avoid header-guard-less redefinitions in third-party headers.
#include "utils.h"

// Error reporting state. Defined in the named ifcopenshell::capi namespace so
// that external translation units can participate via the internal header.
namespace ifcopenshell {
namespace capi {
thread_local std::string g_last_error;
thread_local int g_last_error_kind = 0;

void set_last_error(const std::string& message) {
    g_last_error_kind = 1;
    g_last_error = message;
}

void set_last_error(int kind, const std::string& message) {
    g_last_error_kind = kind;
    g_last_error = message;
}
} // namespace capi
} // namespace ifcopenshell

namespace {
using ifcopenshell::capi::g_last_error;
using ifcopenshell::capi::set_last_error;

bool feature_use_attribute_value_derived = false;
std::stringstream ifcopenshell_log_stream;
bool g_log_stream_initialized = false;

void ensure_log_stream_initialized() {
    if (!g_log_stream_initialized) {
        Logger::SetOutput(nullptr, &ifcopenshell_log_stream);
        g_log_stream_initialized = true;
    }
}

IfcUtil::ArgumentType helper_fn_attribute_type(const IfcUtil::IfcBaseClass* inst, unsigned index) {
    const IfcParse::parameter_type* parameter_type = nullptr;
    if (inst->declaration().as_entity()) {
        parameter_type = inst->declaration().as_entity()->attribute_by_index(index)->type_of_attribute();
        if (inst->declaration().as_entity()->derived()[index]) {
            return IfcUtil::Argument_DERIVED;
        }
    } else if (inst->declaration().as_type_declaration() && index == 0) {
        parameter_type = inst->declaration().as_type_declaration()->declared_type();
    } else if (inst->declaration().as_enumeration_type() && index == 0) {
        return IfcUtil::Argument_STRING;
    }

    if (parameter_type == nullptr) {
        return IfcUtil::Argument_UNKNOWN;
    }
    return IfcUtil::from_parameter_type(parameter_type);
}

void validate_list_items(const char* name, const void* items, size_t size) {
    if (size > 0 && items == nullptr) {
        throw std::runtime_error(std::string("Parameter '") + name + "' has a null items pointer");
    }
}

template <typename T>
struct is_std_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

std::string json_escape_string(const std::string& value) {
    std::ostringstream out;
    for (unsigned char ch : value) {
        switch (ch) {
        case '\\': out << "\\\\"; break;
        case '"': out << "\\\""; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if (ch < 0x20) {
                out << "\\u"
                    << "00"
                    << "0123456789abcdef"[ch >> 4]
                    << "0123456789abcdef"[ch & 0x0f];
            } else {
                out << static_cast<char>(ch);
            }
        }
    }
    return out.str();
}

std::string json_quote(const std::string& value) {
    return std::string(1, '"') + json_escape_string(value) + '"';
}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier);

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier);

template <typename T>
std::string vector_to_json_string(const std::vector<T>& values, bool include_identifier) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            out << ",";
        }
        out << value_to_json_string(values[i], include_identifier);
    }
    out << "]";
    return out.str();
}

std::string reference_or_simple_type_to_json_string(const IfcParse::reference_or_simple_type& value, bool include_identifier) {
    if (auto* instance = std::get_if<IfcUtil::IfcBaseClass*>(&value)) {
        return *instance ? instance_to_info_json_string(*instance, include_identifier) : "null";
    }
    auto reference = std::get<IfcParse::InstanceReference>(value);
    return std::string(R"({"ref":)") + std::to_string(reference.v) + "}";
}

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier) {
    if constexpr (is_std_vector_v<T>) {
        return vector_to_json_string(value, include_identifier);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return json_quote(value);
    } else if constexpr (std::is_same_v<T, const char*>) {
        return value ? json_quote(value) : "null";
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else if constexpr (std::is_same_v<T, boost::logic::tribool>) {
        if (boost::logic::indeterminate(value)) {
            return "null";
        }
        return value ? "true" : "false";
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        std::ostringstream out;
        out << value;
        return out.str();
    } else if constexpr (std::is_same_v<T, EnumerationReference>) {
        return json_quote(std::string(value.value()));
    } else if constexpr (std::is_same_v<T, IfcParse::reference_or_simple_type>) {
        return reference_or_simple_type_to_json_string(value, include_identifier);
    } else if constexpr (std::is_same_v<T, IfcUtil::IfcBaseClass*>) {
        return value ? instance_to_info_json_string(value, include_identifier) : "null";
    } else if constexpr (std::is_same_v<T, aggregate_of_instance::ptr>) {
        if (!value) {
            return "null";
        }
        std::ostringstream out;
        out << "[";
        for (size_t i = 0; i < value->size(); ++i) {
            if (i != 0) {
                out << ",";
            }
            out << ((*value)[static_cast<int>(i)] ? instance_to_info_json_string((*value)[static_cast<int>(i)], include_identifier) : "null");
        }
        out << "]";
        return out.str();
    } else if constexpr (std::is_same_v<T, aggregate_of_aggregate_of_instance::ptr>) {
        if (!value) {
            return "null";
        }
        std::ostringstream out;
        out << "[";
        size_t outer_index = 0;
        for (auto outer = value->begin(); outer != value->end(); ++outer, ++outer_index) {
            if (outer_index != 0) {
                out << ",";
            }
            out << "[";
            for (size_t inner_index = 0; inner_index < outer->size(); ++inner_index) {
                if (inner_index != 0) {
                    out << ",";
                }
                auto* instance = (*outer)[inner_index];
                out << (instance ? instance_to_info_json_string(instance, include_identifier) : "null");
            }
            out << "]";
        }
        out << "]";
        return out.str();
    } else if constexpr (
        std::is_same_v<T, empty_aggregate_t> ||
        std::is_same_v<T, empty_aggregate_of_aggregate_t> ||
        std::is_same_v<T, Blank> ||
        std::is_same_v<T, Derived>) {
        return "null";
    } else {
        return json_quote("<unsupported>");
    }
}

std::string attribute_value_to_json_string(const AttributeValue& value, bool include_identifier) {
    return value.apply_visitor([include_identifier](const auto& inner) -> std::string {
        return value_to_json_string(inner, include_identifier);
    });
}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier) {
    if (instance == nullptr) {
        return "null";
    }
    std::ostringstream out;
    out << "{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {
        if (!first) {
            out << ",";
        }
        first = false;
        out << json_quote(key) << ":" << json_value;
    };

    if (instance->declaration().as_entity()) {
        const auto attributes = instance->declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {
            emit_field(attributes[i]->name(), attribute_value_to_json_string(instance->get_attribute_value(i), include_identifier));
        }
        if (include_identifier) {
            emit_field("id", std::to_string(instance->as<IfcUtil::IfcBaseEntity>()->id()));
        }
    } else {
        emit_field("wrappedValue", attribute_value_to_json_string(instance->get_attribute_value(0), include_identifier));
    }

    emit_field("type", json_quote(instance->declaration().name()));
    out << "}";
    return out.str();
}

std::string unresolved_reference_variant_to_json_string(
    const std::variant<
        IfcParse::reference_or_simple_type,
        std::vector<IfcParse::reference_or_simple_type>,
        std::vector<std::vector<IfcParse::reference_or_simple_type>>>& value,
    bool include_identifier) {
    return std::visit(
        [include_identifier](const auto& inner) -> std::string {
            return value_to_json_string(inner, include_identifier);
        },
        value);
}

std::string unresolved_references_to_json_string(
    const IfcParse::unresolved_references& references,
    const IfcParse::declaration* declaration) {
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : references) {
        if (!first) {
            out << ",";
        }
        first = false;
        const auto& mutable_value = entry.first;
        out << "{"
            << R"("entity_name":)" << mutable_value.name_
            << R"(,"attribute_index":)" << static_cast<int>(mutable_value.index_);
        if (declaration && declaration->as_entity() && mutable_value.index_ < declaration->as_entity()->attribute_count()) {
            out << R"(,"attribute_name":)" << json_quote(declaration->as_entity()->attribute_by_index(mutable_value.index_)->name());
        }
        out << R"(,"value":)" << unresolved_reference_variant_to_json_string(entry.second, true) << "}";
    }
    out << "]";
    return out.str();
}

template <typename Inverses>
std::string inverses_to_json_string(const Inverses& inverses) {
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : inverses) {
        if (!first) {
            out << ",";
        }
        first = false;
        out << "{"
            << R"("instance_id":)" << std::get<0>(entry.first)
            << R"(,"instance_type":)" << std::get<1>(entry.first)
            << R"(,"attribute_index":)" << std::get<2>(entry.first)
            << R"(,"referencing_ids":)" << vector_to_json_string(entry.second, true)
            << "}";
    }
    out << "]";
    return out.str();
}

std::string instance_stream_read_instance_json(IfcParse::InstanceStreamer* streamer, bool type_as_declaration_instance) {
    if (!(*streamer)) {
        return "null";
    }
    auto inst = streamer->readInstance();
    if (!inst) {
        return "null";
    }

    std::ostringstream out;
    out << "{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {
        if (!first) {
            out << ",";
        }
        first = false;
        out << json_quote(key) << ":" << json_value;
    };

    emit_field("id", std::to_string(std::get<0>(*inst)));
    if (type_as_declaration_instance) {
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    } else {
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }

    const auto* declaration = std::get<1>(*inst);
    const auto& data = std::get<2>(*inst);
    if (declaration->as_entity()) {
        for (size_t i = 0; i < declaration->as_entity()->attribute_count(); ++i) {
            emit_field(
                declaration->as_entity()->attribute_by_index(i)->name(),
                attribute_value_to_json_string(data.get_attribute_value(nullptr, declaration, 0, i), true));
        }
    }

    for (const auto& reference : streamer->references()) {
        std::string key = std::to_string(reference.first.index_);
        if (declaration->as_entity() && reference.first.index_ < declaration->as_entity()->attribute_count()) {
            key = declaration->as_entity()->attribute_by_index(reference.first.index_)->name();
        }
        emit_field(key, unresolved_reference_variant_to_json_string(reference.second, true));
    }

    streamer->references().clear();
    streamer->inverses().clear();
    out << "}";
    return out.str();
}

ifcopenshell_string_t make_string(const std::string& value) {
    char* data = new char[value.size() + 1];
    std::memcpy(data, value.c_str(), value.size() + 1);
    return ifcopenshell_string_t{data, value.size(), true};
}

ifcopenshell_string_t make_static_string(const char* value) {
    if (value == nullptr) {
        return ifcopenshell_string_t{nullptr, 0, false};
    }
    return ifcopenshell_string_t{const_cast<char*>(value), std::strlen(value), false};
}

template <typename T>
void set_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index, const T& value) {
    instance->set_attribute_value(index, value);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {
        entity->populate_derived();
    }
}

void unset_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index) {
    set_instance_argument(instance, index, Blank{});
}

void set_instance_attribute_from_attribute_value(IfcUtil::IfcBaseClass* instance, size_t index, const AttributeValue& value) {
    value.apply_visitor([&](const auto& inner) -> void {
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, Derived>) {
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        } else if constexpr (std::is_same_v<T, empty_aggregate_t> ||
                              std::is_same_v<T, empty_aggregate_of_aggregate_t>) {
            // Empty-aggregate sentinels arise when reading absent aggregate
            // values; assigning them is equivalent to unsetting the attribute.
            unset_instance_argument(instance, index);
        } else {
            set_instance_argument(instance, index, inner);
        }
    });
}
} // namespace

static ifcopenshell_ifc_instance_list_t make_ifc_instance_list(const std::vector<IfcUtil::IfcBaseClass*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_instance_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_instance_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_instance_list_t{items, values.size()};
}

static ifcopenshell_ifc_instance_list_t make_ifc_instance_list(const std::vector<const IfcUtil::IfcBaseClass*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_instance_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_instance_t{const_cast<IfcUtil::IfcBaseClass*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_instance_list_t{items, values.size()};
}

static ifcopenshell_ifc_instance_list_t make_ifc_instance_list(std::vector<std::unique_ptr<IfcUtil::IfcBaseClass>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_instance_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_instance_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_instance_list_t{items, values.size()};
}

static std::vector<const IfcUtil::IfcBaseClass*> to_cpp_ifc_instance_list(const ifcopenshell_ifc_instance_list_t* values) {
    validate_list_items("ifc_instance_list", values->items, values->size);
    std::vector<const IfcUtil::IfcBaseClass*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifcgeom_conversion_result_shape_list_t make_ifcgeom_conversion_result_shape_list(const std::vector<IfcGeom::ConversionResultShape*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_conversion_result_shape_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_conversion_result_shape_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_conversion_result_shape_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_conversion_result_shape_list_t make_ifcgeom_conversion_result_shape_list(const std::vector<const IfcGeom::ConversionResultShape*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_conversion_result_shape_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_conversion_result_shape_t{const_cast<IfcGeom::ConversionResultShape*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_conversion_result_shape_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_conversion_result_shape_list_t make_ifcgeom_conversion_result_shape_list(std::vector<std::unique_ptr<IfcGeom::ConversionResultShape>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_conversion_result_shape_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_conversion_result_shape_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_conversion_result_shape_list_t{items, values.size()};
}

static std::vector<const IfcGeom::ConversionResultShape*> to_cpp_ifcgeom_conversion_result_shape_list(const ifcopenshell_ifcgeom_conversion_result_shape_list_t* values) {
    validate_list_items("ifcgeom_conversion_result_shape_list", values->items, values->size);
    std::vector<const IfcGeom::ConversionResultShape*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifcgeom_svgfill_polygon_list_t make_ifcgeom_svgfill_polygon_list(const std::vector<svgfill::polygon_2*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_svgfill_polygon_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_svgfill_polygon_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_svgfill_polygon_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_svgfill_polygon_list_t make_ifcgeom_svgfill_polygon_list(const std::vector<const svgfill::polygon_2*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_svgfill_polygon_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_svgfill_polygon_t{const_cast<svgfill::polygon_2*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_svgfill_polygon_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_svgfill_polygon_list_t make_ifcgeom_svgfill_polygon_list(std::vector<std::unique_ptr<svgfill::polygon_2>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_svgfill_polygon_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_svgfill_polygon_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_svgfill_polygon_list_t{items, values.size()};
}

static std::vector<const svgfill::polygon_2*> to_cpp_ifcgeom_svgfill_polygon_list(const ifcopenshell_ifcgeom_svgfill_polygon_list_t* values) {
    validate_list_items("ifcgeom_svgfill_polygon_list", values->items, values->size);
    std::vector<const svgfill::polygon_2*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_declaration_list_t make_ifc_declaration_list(const std::vector<IfcParse::declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_declaration_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_declaration_list_t{items, values.size()};
}

static ifcopenshell_ifc_declaration_list_t make_ifc_declaration_list(const std::vector<const IfcParse::declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_declaration_list_t{items, values.size()};
}

static ifcopenshell_ifc_declaration_list_t make_ifc_declaration_list(std::vector<std::unique_ptr<IfcParse::declaration>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_declaration_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_declaration_list_t{items, values.size()};
}

static std::vector<const IfcParse::declaration*> to_cpp_ifc_declaration_list(const ifcopenshell_ifc_declaration_list_t* values) {
    validate_list_items("ifc_declaration_list", values->items, values->size);
    std::vector<const IfcParse::declaration*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_entity_list_t make_ifc_entity_list(const std::vector<IfcParse::entity*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_entity_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_entity_list_t{items, values.size()};
}

static ifcopenshell_ifc_entity_list_t make_ifc_entity_list(const std::vector<const IfcParse::entity*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_entity_list_t{items, values.size()};
}

static ifcopenshell_ifc_entity_list_t make_ifc_entity_list(std::vector<std::unique_ptr<IfcParse::entity>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_entity_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_entity_list_t{items, values.size()};
}

static std::vector<const IfcParse::entity*> to_cpp_ifc_entity_list(const ifcopenshell_ifc_entity_list_t* values) {
    validate_list_items("ifc_entity_list", values->items, values->size);
    std::vector<const IfcParse::entity*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_enumeration_list_t make_ifc_enumeration_list(const std::vector<IfcParse::enumeration_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_enumeration_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_enumeration_list_t{items, values.size()};
}

static ifcopenshell_ifc_enumeration_list_t make_ifc_enumeration_list(const std::vector<const IfcParse::enumeration_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_enumeration_t{const_cast<IfcParse::enumeration_type*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_enumeration_list_t{items, values.size()};
}

static ifcopenshell_ifc_enumeration_list_t make_ifc_enumeration_list(std::vector<std::unique_ptr<IfcParse::enumeration_type>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_enumeration_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_enumeration_list_t{items, values.size()};
}

static std::vector<const IfcParse::enumeration_type*> to_cpp_ifc_enumeration_list(const ifcopenshell_ifc_enumeration_list_t* values) {
    validate_list_items("ifc_enumeration_list", values->items, values->size);
    std::vector<const IfcParse::enumeration_type*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_select_type_list_t make_ifc_select_type_list(const std::vector<IfcParse::select_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_select_type_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_select_type_list_t{items, values.size()};
}

static ifcopenshell_ifc_select_type_list_t make_ifc_select_type_list(const std::vector<const IfcParse::select_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_select_type_t{const_cast<IfcParse::select_type*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_select_type_list_t{items, values.size()};
}

static ifcopenshell_ifc_select_type_list_t make_ifc_select_type_list(std::vector<std::unique_ptr<IfcParse::select_type>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_select_type_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_select_type_list_t{items, values.size()};
}

static std::vector<const IfcParse::select_type*> to_cpp_ifc_select_type_list(const ifcopenshell_ifc_select_type_list_t* values) {
    validate_list_items("ifc_select_type_list", values->items, values->size);
    std::vector<const IfcParse::select_type*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_type_declaration_list_t make_ifc_type_declaration_list(const std::vector<IfcParse::type_declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_type_declaration_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_type_declaration_list_t{items, values.size()};
}

static ifcopenshell_ifc_type_declaration_list_t make_ifc_type_declaration_list(const std::vector<const IfcParse::type_declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_type_declaration_t{const_cast<IfcParse::type_declaration*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_type_declaration_list_t{items, values.size()};
}

static ifcopenshell_ifc_type_declaration_list_t make_ifc_type_declaration_list(std::vector<std::unique_ptr<IfcParse::type_declaration>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_type_declaration_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_type_declaration_list_t{items, values.size()};
}

static std::vector<const IfcParse::type_declaration*> to_cpp_ifc_type_declaration_list(const ifcopenshell_ifc_type_declaration_list_t* values) {
    validate_list_items("ifc_type_declaration_list", values->items, values->size);
    std::vector<const IfcParse::type_declaration*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_attribute_list_t make_ifc_attribute_list(const std::vector<IfcParse::attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_attribute_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_attribute_list_t{items, values.size()};
}

static ifcopenshell_ifc_attribute_list_t make_ifc_attribute_list(const std::vector<const IfcParse::attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_attribute_t{const_cast<IfcParse::attribute*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_attribute_list_t{items, values.size()};
}

static ifcopenshell_ifc_attribute_list_t make_ifc_attribute_list(std::vector<std::unique_ptr<IfcParse::attribute>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_attribute_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_attribute_list_t{items, values.size()};
}

static std::vector<const IfcParse::attribute*> to_cpp_ifc_attribute_list(const ifcopenshell_ifc_attribute_list_t* values) {
    validate_list_items("ifc_attribute_list", values->items, values->size);
    std::vector<const IfcParse::attribute*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifc_inverse_attribute_list_t make_ifc_inverse_attribute_list(const std::vector<IfcParse::inverse_attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_inverse_attribute_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_inverse_attribute_list_t{items, values.size()};
}

static ifcopenshell_ifc_inverse_attribute_list_t make_ifc_inverse_attribute_list(const std::vector<const IfcParse::inverse_attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_inverse_attribute_t{const_cast<IfcParse::inverse_attribute*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_inverse_attribute_list_t{items, values.size()};
}

static ifcopenshell_ifc_inverse_attribute_list_t make_ifc_inverse_attribute_list(std::vector<std::unique_ptr<IfcParse::inverse_attribute>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifc_inverse_attribute_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifc_inverse_attribute_list_t{items, values.size()};
}

static std::vector<const IfcParse::inverse_attribute*> to_cpp_ifc_inverse_attribute_list(const ifcopenshell_ifc_inverse_attribute_list_t* values) {
    validate_list_items("ifc_inverse_attribute_list", values->items, values->size);
    std::vector<const IfcParse::inverse_attribute*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifcgeom_taxonomy_style_list_t make_ifcgeom_taxonomy_style_list(const std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_taxonomy_style_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_taxonomy_style_t{values[i]};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_taxonomy_style_list_t{items, values.size()};
}

static std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>> to_cpp_ifcgeom_taxonomy_style_list(const ifcopenshell_ifcgeom_taxonomy_style_list_t* values) {
    validate_list_items("ifcgeom_taxonomy_style_list", values->items, values->size);
    std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifcgeom_taxonomy_item_list_t make_ifcgeom_taxonomy_item_list(const std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_taxonomy_item_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_taxonomy_item_t{values[i]};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_taxonomy_item_list_t{items, values.size()};
}

static std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>> to_cpp_ifcgeom_taxonomy_item_list(const ifcopenshell_ifcgeom_taxonomy_item_list_t* values) {
    validate_list_items("ifcgeom_taxonomy_item_list", values->items, values->size);
    std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}

static ifcopenshell_ifcgeom_element_list_t make_ifcgeom_element_list(const std::vector<IfcGeom::Element*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_element_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_element_t{values[i], false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_element_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_element_list_t make_ifcgeom_element_list(const std::vector<const IfcGeom::Element*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_element_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_element_t{const_cast<IfcGeom::Element*>(values[i]), false};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_element_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_element_list_t make_ifcgeom_element_list(std::vector<std::unique_ptr<IfcGeom::Element>> values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_element_t*[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
            items[i] = new ifcopenshell_ifcgeom_element_t{values[i].release(), true};
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) { delete items[j]; }
        delete[] items;
        throw;
    }
    return ifcopenshell_ifcgeom_element_list_t{items, values.size()};
}

static std::vector<const IfcGeom::Element*> to_cpp_ifcgeom_element_list(const ifcopenshell_ifcgeom_element_list_t* values) {
    validate_list_items("ifcgeom_element_list", values->items, values->size);
    std::vector<const IfcGeom::Element*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {
            throw std::runtime_error("handle_list contains an invalid handle");
        }
        result.push_back(item->ptr);
    }
    return result;
}
static ifcopenshell_ifc_instance_list_list_t make_ifc_instance_list_list(const std::vector<std::vector<IfcUtil::IfcBaseClass*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_instance_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_instance_list(values[i]);
    }
    return ifcopenshell_ifc_instance_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_instance_list_list_t make_ifc_instance_list_list(const std::vector<std::vector<const IfcUtil::IfcBaseClass*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_instance_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_instance_list(values[i]);
    }
    return ifcopenshell_ifc_instance_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcUtil::IfcBaseClass*>> to_cpp_ifc_instance_list_list(const ifcopenshell_ifc_instance_list_list_t* values) {
    validate_list_items("ifc_instance_list_list", values->items, values->size);
    std::vector<std::vector<const IfcUtil::IfcBaseClass*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_instance_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifcgeom_conversion_result_shape_list_list_t make_ifcgeom_conversion_result_shape_list_list(const std::vector<std::vector<IfcGeom::ConversionResultShape*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_conversion_result_shape_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_conversion_result_shape_list(values[i]);
    }
    return ifcopenshell_ifcgeom_conversion_result_shape_list_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_conversion_result_shape_list_list_t make_ifcgeom_conversion_result_shape_list_list(const std::vector<std::vector<const IfcGeom::ConversionResultShape*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_conversion_result_shape_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_conversion_result_shape_list(values[i]);
    }
    return ifcopenshell_ifcgeom_conversion_result_shape_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcGeom::ConversionResultShape*>> to_cpp_ifcgeom_conversion_result_shape_list_list(const ifcopenshell_ifcgeom_conversion_result_shape_list_list_t* values) {
    validate_list_items("ifcgeom_conversion_result_shape_list_list", values->items, values->size);
    std::vector<std::vector<const IfcGeom::ConversionResultShape*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifcgeom_conversion_result_shape_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifcgeom_svgfill_polygon_list_list_t make_ifcgeom_svgfill_polygon_list_list(const std::vector<std::vector<svgfill::polygon_2*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_svgfill_polygon_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_svgfill_polygon_list(values[i]);
    }
    return ifcopenshell_ifcgeom_svgfill_polygon_list_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_svgfill_polygon_list_list_t make_ifcgeom_svgfill_polygon_list_list(const std::vector<std::vector<const svgfill::polygon_2*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_svgfill_polygon_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_svgfill_polygon_list(values[i]);
    }
    return ifcopenshell_ifcgeom_svgfill_polygon_list_list_t{items, values.size()};
}

static std::vector<std::vector<const svgfill::polygon_2*>> to_cpp_ifcgeom_svgfill_polygon_list_list(const ifcopenshell_ifcgeom_svgfill_polygon_list_list_t* values) {
    validate_list_items("ifcgeom_svgfill_polygon_list_list", values->items, values->size);
    std::vector<std::vector<const svgfill::polygon_2*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifcgeom_svgfill_polygon_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_declaration_list_list_t make_ifc_declaration_list_list(const std::vector<std::vector<IfcParse::declaration*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_declaration_list(values[i]);
    }
    return ifcopenshell_ifc_declaration_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_declaration_list_list_t make_ifc_declaration_list_list(const std::vector<std::vector<const IfcParse::declaration*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_declaration_list(values[i]);
    }
    return ifcopenshell_ifc_declaration_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::declaration*>> to_cpp_ifc_declaration_list_list(const ifcopenshell_ifc_declaration_list_list_t* values) {
    validate_list_items("ifc_declaration_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::declaration*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_declaration_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_entity_list_list_t make_ifc_entity_list_list(const std::vector<std::vector<IfcParse::entity*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_entity_list(values[i]);
    }
    return ifcopenshell_ifc_entity_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_entity_list_list_t make_ifc_entity_list_list(const std::vector<std::vector<const IfcParse::entity*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_entity_list(values[i]);
    }
    return ifcopenshell_ifc_entity_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::entity*>> to_cpp_ifc_entity_list_list(const ifcopenshell_ifc_entity_list_list_t* values) {
    validate_list_items("ifc_entity_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::entity*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_entity_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_enumeration_list_list_t make_ifc_enumeration_list_list(const std::vector<std::vector<IfcParse::enumeration_type*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_enumeration_list(values[i]);
    }
    return ifcopenshell_ifc_enumeration_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_enumeration_list_list_t make_ifc_enumeration_list_list(const std::vector<std::vector<const IfcParse::enumeration_type*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_enumeration_list(values[i]);
    }
    return ifcopenshell_ifc_enumeration_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::enumeration_type*>> to_cpp_ifc_enumeration_list_list(const ifcopenshell_ifc_enumeration_list_list_t* values) {
    validate_list_items("ifc_enumeration_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::enumeration_type*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_enumeration_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_select_type_list_list_t make_ifc_select_type_list_list(const std::vector<std::vector<IfcParse::select_type*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_select_type_list(values[i]);
    }
    return ifcopenshell_ifc_select_type_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_select_type_list_list_t make_ifc_select_type_list_list(const std::vector<std::vector<const IfcParse::select_type*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_select_type_list(values[i]);
    }
    return ifcopenshell_ifc_select_type_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::select_type*>> to_cpp_ifc_select_type_list_list(const ifcopenshell_ifc_select_type_list_list_t* values) {
    validate_list_items("ifc_select_type_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::select_type*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_select_type_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_type_declaration_list_list_t make_ifc_type_declaration_list_list(const std::vector<std::vector<IfcParse::type_declaration*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_type_declaration_list(values[i]);
    }
    return ifcopenshell_ifc_type_declaration_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_type_declaration_list_list_t make_ifc_type_declaration_list_list(const std::vector<std::vector<const IfcParse::type_declaration*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_type_declaration_list(values[i]);
    }
    return ifcopenshell_ifc_type_declaration_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::type_declaration*>> to_cpp_ifc_type_declaration_list_list(const ifcopenshell_ifc_type_declaration_list_list_t* values) {
    validate_list_items("ifc_type_declaration_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::type_declaration*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_type_declaration_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_attribute_list_list_t make_ifc_attribute_list_list(const std::vector<std::vector<IfcParse::attribute*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_attribute_list(values[i]);
    }
    return ifcopenshell_ifc_attribute_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_attribute_list_list_t make_ifc_attribute_list_list(const std::vector<std::vector<const IfcParse::attribute*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_attribute_list(values[i]);
    }
    return ifcopenshell_ifc_attribute_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::attribute*>> to_cpp_ifc_attribute_list_list(const ifcopenshell_ifc_attribute_list_list_t* values) {
    validate_list_items("ifc_attribute_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::attribute*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_attribute_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifc_inverse_attribute_list_list_t make_ifc_inverse_attribute_list_list(const std::vector<std::vector<IfcParse::inverse_attribute*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_inverse_attribute_list(values[i]);
    }
    return ifcopenshell_ifc_inverse_attribute_list_list_t{items, values.size()};
}

static ifcopenshell_ifc_inverse_attribute_list_list_t make_ifc_inverse_attribute_list_list(const std::vector<std::vector<const IfcParse::inverse_attribute*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifc_inverse_attribute_list(values[i]);
    }
    return ifcopenshell_ifc_inverse_attribute_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcParse::inverse_attribute*>> to_cpp_ifc_inverse_attribute_list_list(const ifcopenshell_ifc_inverse_attribute_list_list_t* values) {
    validate_list_items("ifc_inverse_attribute_list_list", values->items, values->size);
    std::vector<std::vector<const IfcParse::inverse_attribute*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifc_inverse_attribute_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifcgeom_taxonomy_style_list_list_t make_ifcgeom_taxonomy_style_list_list(const std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_taxonomy_style_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_taxonomy_style_list(values[i]);
    }
    return ifcopenshell_ifcgeom_taxonomy_style_list_list_t{items, values.size()};
}

static std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>>> to_cpp_ifcgeom_taxonomy_style_list_list(const ifcopenshell_ifcgeom_taxonomy_style_list_list_t* values) {
    validate_list_items("ifcgeom_taxonomy_style_list_list", values->items, values->size);
    std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::style>>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifcgeom_taxonomy_style_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifcgeom_taxonomy_item_list_list_t make_ifcgeom_taxonomy_item_list_list(const std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_taxonomy_item_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_taxonomy_item_list(values[i]);
    }
    return ifcopenshell_ifcgeom_taxonomy_item_list_list_t{items, values.size()};
}

static std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>>> to_cpp_ifcgeom_taxonomy_item_list_list(const ifcopenshell_ifcgeom_taxonomy_item_list_list_t* values) {
    validate_list_items("ifcgeom_taxonomy_item_list_list", values->items, values->size);
    std::vector<std::vector<std::shared_ptr<ifcopenshell::geometry::taxonomy::item>>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifcgeom_taxonomy_item_list(&values->items[i]));
    }
    return result;
}

static ifcopenshell_ifcgeom_element_list_list_t make_ifcgeom_element_list_list(const std::vector<std::vector<IfcGeom::Element*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_element_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_element_list(values[i]);
    }
    return ifcopenshell_ifcgeom_element_list_list_t{items, values.size()};
}

static ifcopenshell_ifcgeom_element_list_list_t make_ifcgeom_element_list_list(const std::vector<std::vector<const IfcGeom::Element*>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_ifcgeom_element_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_ifcgeom_element_list(values[i]);
    }
    return ifcopenshell_ifcgeom_element_list_list_t{items, values.size()};
}

static std::vector<std::vector<const IfcGeom::Element*>> to_cpp_ifcgeom_element_list_list(const ifcopenshell_ifcgeom_element_list_list_t* values) {
    validate_list_items("ifcgeom_element_list_list", values->items, values->size);
    std::vector<std::vector<const IfcGeom::Element*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {
        result.push_back(to_cpp_ifcgeom_element_list(&values->items[i]));
    }
    return result;
}

void ifcopenshell_string_destroy(ifcopenshell_string_t* value) {
    if (value == nullptr) {
        return;
    }
    if (value->owned && value->data != nullptr) {
        delete[] value->data;
    }
    value->data = nullptr;
    value->size = 0;
    value->owned = false;
}

void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_string_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_double_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_int32_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int32_list_list_list_destroy(ifcopenshell_int32_list_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_int32_list_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int64_list_destroy(ifcopenshell_int64_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_uint8_list_destroy(ifcopenshell_uint8_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

static ifcopenshell_string_list_t make_string_list(const std::vector<std::string>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_string_t[values.size()];
    size_t initialized = 0;
    try {
        for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_string(values[i]);
            ++initialized;
        }
    } catch (...) {
        for (size_t j = 0; j < initialized; ++j) {
            delete[] items[j].data;
        }
        delete[] items;
        throw;
    }
    return ifcopenshell_string_list_t{items, values.size()};
}

static std::vector<std::string> to_cpp_string_list(const ifcopenshell_string_list_t* value) {
    validate_list_items("string_list", value->items, value->size);
    std::vector<std::string> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        const auto& item = value->items[i];
        if (item.data == nullptr && item.size > 0) {
            throw std::runtime_error("string_list contains a null string buffer");
        }
        result.emplace_back(item.data == nullptr ? "" : item.data, item.size);
    }
    return result;
}

static ifcopenshell_int32_list_t make_int32_list(const std::vector<int>& values) {
    auto* items = values.empty() ? nullptr : new int32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = static_cast<int32_t>(values[i]);
    }
    return ifcopenshell_int32_list_t{items, values.size()};
}

static std::vector<int> to_cpp_int32_list(const ifcopenshell_int32_list_t* value) {
    validate_list_items("int32_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<int>(value->items, value->items + value->size);
}

static ifcopenshell_double_list_t make_double_list(const std::vector<double>& values) {
    auto* items = values.empty() ? nullptr : new double[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_double_list_t{items, values.size()};
}

static std::vector<double> to_cpp_double_list(const ifcopenshell_double_list_t* value) {
    validate_list_items("double_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<double>(value->items, value->items + value->size);
}

static ifcopenshell_bool_list_t make_bool_list(const std::vector<bool>& values) {
    auto* items = values.empty() ? nullptr : new bool[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_bool_list_t{items, values.size()};
}

static std::vector<bool> to_cpp_bool_list(const ifcopenshell_bool_list_t* value) {
    validate_list_items("bool_list", value->items, value->size);
    std::vector<bool> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(value->items[i]);
    }
    return result;
}

static ifcopenshell_double_list_list_t make_double_list_list(const std::vector<std::vector<double>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_double_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_double_list(values[i]);
    }
    return ifcopenshell_double_list_list_t{items, values.size()};
}

static std::vector<std::vector<double>> to_cpp_double_list_list(const ifcopenshell_double_list_list_t* value) {
    validate_list_items("double_list_list", value->items, value->size);
    std::vector<std::vector<double>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(to_cpp_double_list(&value->items[i]));
    }
    return result;
}

static ifcopenshell_int32_list_list_t make_int32_list_list(const std::vector<std::vector<int>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_int32_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_int32_list(values[i]);
    }
    return ifcopenshell_int32_list_list_t{items, values.size()};
}

static std::vector<std::vector<int>> to_cpp_int32_list_list(const ifcopenshell_int32_list_list_t* value) {
    validate_list_items("int32_list_list", value->items, value->size);
    std::vector<std::vector<int>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(to_cpp_int32_list(&value->items[i]));
    }
    return result;
}

static ifcopenshell_int32_list_list_list_t make_int32_list_list_list(const std::vector<std::vector<std::vector<int>>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_int32_list_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_int32_list_list(values[i]);
    }
    return ifcopenshell_int32_list_list_list_t{items, values.size()};
}

static std::vector<std::vector<std::vector<int>>> to_cpp_int32_list_list_list(const ifcopenshell_int32_list_list_list_t* value) {
    validate_list_items("int32_list_list_list", value->items, value->size);
    std::vector<std::vector<std::vector<int>>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(to_cpp_int32_list_list(&value->items[i]));
    }
    return result;
}

static ifcopenshell_int64_list_t make_int64_list(const std::vector<int64_t>& values) {
    auto* items = values.empty() ? nullptr : new int64_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_int64_list_t{items, values.size()};
}

static std::vector<int64_t> to_cpp_int64_list(const ifcopenshell_int64_list_t* value) {
    validate_list_items("int64_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<int64_t>(value->items, value->items + value->size);
}

static ifcopenshell_uint32_list_t make_uint32_list(const std::vector<unsigned int>& values) {
    auto* items = values.empty() ? nullptr : new uint32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = static_cast<uint32_t>(values[i]);
    }
    return ifcopenshell_uint32_list_t{items, values.size()};
}

static std::vector<unsigned int> to_cpp_uint32_list(const ifcopenshell_uint32_list_t* value) {
    validate_list_items("uint32_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<unsigned int>(value->items, value->items + value->size);
}

static ifcopenshell_uint8_list_t make_uint8_list(const std::vector<uint8_t>& values) {
    auto* items = values.empty() ? nullptr : new uint8_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_uint8_list_t{items, values.size()};
}

static std::vector<uint8_t> to_cpp_uint8_list(const ifcopenshell_uint8_list_t* value) {
    validate_list_items("uint8_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<uint8_t>(value->items, value->items + value->size);
}

void ifcopenshell_clear_error(void) {
    ifcopenshell::capi::g_last_error.clear();
    ifcopenshell::capi::g_last_error_kind = 0;
}

const char* ifcopenshell_last_error_message(void) {
    return ifcopenshell::capi::g_last_error.c_str();
}

int ifcopenshell_last_error_kind(void) {
    return ifcopenshell::capi::g_last_error_kind;
}

void ifcopenshell_ifc_file_destroy(ifcopenshell_ifc_file_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifc_instance_streamer_destroy(ifcopenshell_ifc_instance_streamer_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifc_instance_destroy(ifcopenshell_ifc_instance_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifc_header_destroy(ifcopenshell_ifc_header_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_file_description_destroy(ifcopenshell_ifc_file_description_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_file_name_destroy(ifcopenshell_ifc_file_name_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_file_schema_destroy(ifcopenshell_ifc_file_schema_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_declaration_destroy(ifcopenshell_ifc_declaration_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_type_declaration_destroy(ifcopenshell_ifc_type_declaration_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_select_type_destroy(ifcopenshell_ifc_select_type_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_schema_destroy(ifcopenshell_ifc_schema_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_enumeration_destroy(ifcopenshell_ifc_enumeration_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_parameter_type_destroy(ifcopenshell_ifc_parameter_type_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_named_type_destroy(ifcopenshell_ifc_named_type_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_simple_type_destroy(ifcopenshell_ifc_simple_type_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_aggregation_type_destroy(ifcopenshell_ifc_aggregation_type_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_entity_destroy(ifcopenshell_ifc_entity_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_attribute_destroy(ifcopenshell_ifc_attribute_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifc_inverse_attribute_destroy(ifcopenshell_ifc_inverse_attribute_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcparse_attribute_value_destroy(ifcopenshell_ifcparse_attribute_value_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcparse_instance_list_destroy(ifcopenshell_ifcparse_instance_list_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_item_destroy(ifcopenshell_ifcgeom_taxonomy_item_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_matrix4_destroy(ifcopenshell_ifcgeom_taxonomy_matrix4_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_point3_destroy(ifcopenshell_ifcgeom_taxonomy_point3_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_direction3_destroy(ifcopenshell_ifcgeom_taxonomy_direction3_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_style_destroy(ifcopenshell_ifcgeom_taxonomy_style_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_colour_destroy(ifcopenshell_ifcgeom_taxonomy_colour_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_line_destroy(ifcopenshell_ifcgeom_taxonomy_line_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_circle_destroy(ifcopenshell_ifcgeom_taxonomy_circle_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_ellipse_destroy(ifcopenshell_ifcgeom_taxonomy_ellipse_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_bspline_curve_destroy(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_offset_curve_destroy(ifcopenshell_ifcgeom_taxonomy_offset_curve_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_edge_destroy(ifcopenshell_ifcgeom_taxonomy_edge_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_loop_destroy(ifcopenshell_ifcgeom_taxonomy_loop_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_face_destroy(ifcopenshell_ifcgeom_taxonomy_face_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_shell_destroy(ifcopenshell_ifcgeom_taxonomy_shell_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_solid_destroy(ifcopenshell_ifcgeom_taxonomy_solid_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_plane_destroy(ifcopenshell_ifcgeom_taxonomy_plane_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_cylinder_destroy(ifcopenshell_ifcgeom_taxonomy_cylinder_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_sphere_destroy(ifcopenshell_ifcgeom_taxonomy_sphere_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_torus_destroy(ifcopenshell_ifcgeom_taxonomy_torus_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_bspline_surface_destroy(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_collection_destroy(ifcopenshell_ifcgeom_taxonomy_collection_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_loft_destroy(ifcopenshell_ifcgeom_taxonomy_loft_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_extrusion_destroy(ifcopenshell_ifcgeom_taxonomy_extrusion_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_revolve_destroy(ifcopenshell_ifcgeom_taxonomy_revolve_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_destroy(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_node_destroy(ifcopenshell_ifcgeom_taxonomy_node_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_taxonomy_boolean_result_destroy(ifcopenshell_ifcgeom_taxonomy_boolean_result_t* handle) {
    if (handle == nullptr) {
        return;
    }
    handle->ptr.reset();
    delete handle;
}

void ifcopenshell_ifcgeom_iterator_destroy(ifcopenshell_ifcgeom_iterator_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_settings_destroy(ifcopenshell_ifcgeom_settings_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_serializer_settings_destroy(ifcopenshell_ifcgeom_serializer_settings_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_geometry_serializer_destroy(ifcopenshell_ifcgeom_geometry_serializer_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_serializer_destroy(ifcopenshell_ifcgeom_serializer_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_buffer_destroy(ifcopenshell_ifcgeom_buffer_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_tree_destroy(ifcopenshell_ifcgeom_tree_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_tree_clash_list_destroy(ifcopenshell_ifcgeom_tree_clash_list_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_tree_clash_destroy(ifcopenshell_ifcgeom_tree_clash_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_tree_ray_intersection_list_destroy(ifcopenshell_ifcgeom_tree_ray_intersection_list_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_tree_ray_intersection_destroy(ifcopenshell_ifcgeom_tree_ray_intersection_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_transformation_destroy(ifcopenshell_ifcgeom_transformation_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_element_destroy(ifcopenshell_ifcgeom_element_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_brep_element_destroy(ifcopenshell_ifcgeom_brep_element_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_triangulation_element_destroy(ifcopenshell_ifcgeom_triangulation_element_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_serialized_element_destroy(ifcopenshell_ifcgeom_serialized_element_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_triangulation_destroy(ifcopenshell_ifcgeom_triangulation_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcgeom_brep_representation_destroy(ifcopenshell_ifcgeom_brep_representation_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcgeom_serialization_destroy(ifcopenshell_ifcgeom_serialization_t* handle) {
    if (handle == nullptr) {
        return;
    }
    delete handle;
}

void ifcopenshell_ifcgeom_conversion_result_shape_destroy(ifcopenshell_ifcgeom_conversion_result_shape_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_opaque_number_destroy(ifcopenshell_ifcgeom_opaque_number_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_svgfill_polygon_destroy(ifcopenshell_ifcgeom_svgfill_polygon_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcgeom_function_item_evaluator_destroy(ifcopenshell_ifcgeom_function_item_evaluator_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { delete handle->ptr; }
    delete handle;
}

void ifcopenshell_ifcapi_value_destroy(ifcopenshell_ifcapi_value_t* handle) {
    if (handle == nullptr) {
        return;
    }
    if (handle->owned && handle->ptr) { ifcapi::bindings::value_free(handle->ptr); }
    delete handle;
}
void ifcopenshell_ifc_instance_list_destroy(ifcopenshell_ifc_instance_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_instance_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_conversion_result_shape_list_destroy(ifcopenshell_ifcgeom_conversion_result_shape_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifcgeom_conversion_result_shape_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_svgfill_polygon_list_destroy(ifcopenshell_ifcgeom_svgfill_polygon_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifcgeom_svgfill_polygon_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_declaration_list_destroy(ifcopenshell_ifc_declaration_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_declaration_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_entity_list_destroy(ifcopenshell_ifc_entity_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_entity_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_enumeration_list_destroy(ifcopenshell_ifc_enumeration_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_enumeration_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_select_type_list_destroy(ifcopenshell_ifc_select_type_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_select_type_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_type_declaration_list_destroy(ifcopenshell_ifc_type_declaration_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_type_declaration_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_attribute_list_destroy(ifcopenshell_ifc_attribute_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_attribute_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_inverse_attribute_list_destroy(ifcopenshell_ifc_inverse_attribute_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifc_inverse_attribute_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_taxonomy_style_list_destroy(ifcopenshell_ifcgeom_taxonomy_style_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifcgeom_taxonomy_style_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_taxonomy_item_list_destroy(ifcopenshell_ifcgeom_taxonomy_item_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifcgeom_taxonomy_item_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_element_list_destroy(ifcopenshell_ifcgeom_element_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        if (value->items[i] != nullptr) {
            ifcopenshell_ifcgeom_element_destroy(value->items[i]);
        }
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}
void ifcopenshell_ifc_instance_list_list_destroy(ifcopenshell_ifc_instance_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_instance_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_conversion_result_shape_list_list_destroy(ifcopenshell_ifcgeom_conversion_result_shape_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifcgeom_conversion_result_shape_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_svgfill_polygon_list_list_destroy(ifcopenshell_ifcgeom_svgfill_polygon_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifcgeom_svgfill_polygon_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_declaration_list_list_destroy(ifcopenshell_ifc_declaration_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_declaration_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_entity_list_list_destroy(ifcopenshell_ifc_entity_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_entity_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_enumeration_list_list_destroy(ifcopenshell_ifc_enumeration_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_enumeration_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_select_type_list_list_destroy(ifcopenshell_ifc_select_type_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_select_type_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_type_declaration_list_list_destroy(ifcopenshell_ifc_type_declaration_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_type_declaration_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_attribute_list_list_destroy(ifcopenshell_ifc_attribute_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_attribute_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifc_inverse_attribute_list_list_destroy(ifcopenshell_ifc_inverse_attribute_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_inverse_attribute_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_taxonomy_style_list_list_destroy(ifcopenshell_ifcgeom_taxonomy_style_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifcgeom_taxonomy_style_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_taxonomy_item_list_list_destroy(ifcopenshell_ifcgeom_taxonomy_item_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifcgeom_taxonomy_item_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_ifcgeom_element_list_list_destroy(ifcopenshell_ifcgeom_element_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifcgeom_element_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

bool ifcopenshell_ifcparse_clear_schemas(void) {
    try {
        ifcopenshell_clear_error();
        IfcParse::clear_schemas();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_guess_file_type(const char* fn, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (fn == nullptr) { throw std::runtime_error("Parameter \"fn\" must not be null"); }
    std::string fn_cpp(fn);
        *out_result = static_cast<int32_t>(IfcParse::guess_file_type(fn_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_parse_ifcxml(const char* filename, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
        *out_result = new ifcopenshell_ifc_file_t{IfcParse::parse_ifcxml(filename_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_register_schema(ifcopenshell_ifc_schema_t* arg_0) {
    try {
        ifcopenshell_clear_error();
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        IfcParse::register_schema(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_schema_by_name(const char* arg_0, ifcopenshell_ifc_schema_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (arg_0 == nullptr) { throw std::runtime_error("Parameter \"arg_0\" must not be null"); }
    std::string arg_0_cpp(arg_0);
        *out_result = new ifcopenshell_ifc_schema_t{const_cast<IfcParse::schema_definition*>(IfcParse::schema_by_name(arg_0_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_schema_names(ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = make_string_list(IfcParse::schema_names());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto max_level_cpp = static_cast<int>(max_level);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{IfcParse::traverse(instance_cpp, max_level_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_traverse_breadth_first(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto max_level_cpp = static_cast<int>(max_level);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{IfcParse::traverse_breadth_first(instance_cpp, max_level_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_open(const char* path, bool readonly, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
{
    std::ifstream probe(path_cpp.c_str());
    if (!probe.good()) {
        throw std::runtime_error(std::string("File does not exist or is not readable: ") + path_cpp);
    }
}
auto file = std::make_unique<IfcParse::IfcFile>(path_cpp, IfcParse::FT_AUTODETECT, readonly);
if (!file->good()) {
    throw std::runtime_error(std::string("Failed to open IFC file: ") + path_cpp);
}
return file.release();
        }();
        *out_result = new ifcopenshell_ifc_file_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_open_bypass(const char* path, const ifcopenshell_string_list_t* type_names, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
    if (type_names == nullptr) { throw std::runtime_error("Parameter \"type_names\" must not be null"); }
    auto type_names_cpp = to_cpp_string_list(type_names);
        auto generated_result = [&]() {
auto file = std::make_unique<IfcParse::IfcFile>(IfcParse::uninitialized_tag{});
for (const auto& type_name : type_names_cpp) {
    file->bypass_type(type_name);
}
if (!file->initialize(path_cpp)) {
    throw std::runtime_error(std::string("Failed to open IFC file: ") + path_cpp);
}
return file.release();
        }();
        *out_result = new ifcopenshell_ifc_file_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_new_file(const char* schema_identifier, int32_t file_type, const char* path, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (schema_identifier == nullptr) { throw std::runtime_error("Parameter \"schema_identifier\" must not be null"); }
    std::string schema_identifier_cpp(schema_identifier);
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
const IfcParse::schema_definition* schema = IfcParse::schema_by_name(schema_identifier);
return new IfcParse::IfcFile(schema, static_cast<IfcParse::filetype>(file_type), path);
        }();
        *out_result = new ifcopenshell_ifc_file_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_read_memory(void* data, int32_t length, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (data == nullptr) { throw std::runtime_error("Parameter \"data\" must not be null"); }
    auto data_cpp = static_cast<const void*>(data);
        auto generated_result = [&]() {
if (length < 0) {
    throw std::runtime_error("length is negative");
}
auto file = std::make_unique<IfcParse::IfcFile>(const_cast<void*>(data_cpp), static_cast<int>(length));
if (!file->good()) {
    throw std::runtime_error("Failed to parse IFC data from string");
}
return file.release();
        }();
        *out_result = new ifcopenshell_ifc_file_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_stream(ifcopenshell_ifc_instance_streamer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return new IfcParse::InstanceStreamer();
        }();
        *out_result = new ifcopenshell_ifc_instance_streamer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_stream_from_path(const char* path, bool mmap, ifcopenshell_ifc_instance_streamer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
#ifdef USE_MMAP
return new IfcParse::InstanceStreamer(path_cpp, mmap);
#else
(void)mmap;
return new IfcParse::InstanceStreamer(path_cpp, false);
#endif
        }();
        *out_result = new ifcopenshell_ifc_instance_streamer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_stream_from_string(const char* data, ifcopenshell_ifc_instance_streamer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (data == nullptr) { throw std::runtime_error("Parameter \"data\" must not be null"); }
    std::string data_cpp(data);
        auto generated_result = [&]() {
return new IfcParse::InstanceStreamer((void*)data_cpp.data(), static_cast<int>(data_cpp.size()));
        }();
        *out_result = new ifcopenshell_ifc_instance_streamer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_version(ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return IFCOPENSHELL_VERSION;
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_set_feature(const char* name, bool value) {
    try {
        ifcopenshell_clear_error();
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        [&]() {
if (name_cpp == "use_attribute_value_derived") {
    feature_use_attribute_value_derived = value;
    return;
}
throw std::runtime_error("Invalid feature specification");
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_get_feature(const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
if (name_cpp == "use_attribute_value_derived") {
    return feature_use_attribute_value_derived;
}
throw std::runtime_error("Invalid feature specification");
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_get_log(ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
ensure_log_stream_initialized();
std::string log = ifcopenshell_log_stream.str();
ifcopenshell_log_stream.str("");
ifcopenshell_log_stream.clear();
return log;
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_turn_on_detailed_logging(void) {
    try {
        ifcopenshell_clear_error();
        [&]() {
Logger::SetOutput(&std::cout, &std::cout);
Logger::Verbosity(Logger::LOG_DEBUG);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_turn_off_detailed_logging(void) {
    try {
        ifcopenshell_clear_error();
        [&]() {
ensure_log_stream_initialized();
Logger::SetOutput(nullptr, &ifcopenshell_log_stream);
Logger::Verbosity(Logger::LOG_WARNING);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_instance_list_create_from_handles(const ifcopenshell_ifc_instance_list_t* instances, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instances == nullptr) { throw std::runtime_error("Parameter \"instances\" must not be null"); }
    auto instances_cpp = to_cpp_ifc_instance_list(instances);
        auto generated_result = [&]() {
boost::shared_ptr<aggregate_of_instance> agg(new aggregate_of_instance());
agg->reserve(instances_cpp.size());
for (const auto* inst : instances_cpp) {
    if (inst != nullptr) {
        agg->push(const_cast<IfcUtil::IfcBaseClass*>(inst));
    }
}
return agg;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_set_log_format_json(void) {
    try {
        ifcopenshell_clear_error();
        [&]() {
ensure_log_stream_initialized();
ifcopenshell_log_stream.str("");
ifcopenshell_log_stream.clear();
Logger::OutputFormat(Logger::FMT_JSON);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_set_log_format_text(void) {
    try {
        ifcopenshell_clear_error();
        [&]() {
ensure_log_stream_initialized();
ifcopenshell_log_stream.str("");
ifcopenshell_log_stream.clear();
Logger::OutputFormat(Logger::FMT_PLAIN);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_ifc_si_prefix_to_value(const char* prefix, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prefix == nullptr) { throw std::runtime_error("Parameter \"prefix\" must not be null"); }
    std::string prefix_cpp(prefix);
        auto generated_result = [&]() {
return IfcParse::IfcSIPrefixToValue(prefix_cpp);
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_get_si_equivalent(ifcopenshell_ifc_instance_t* named_unit, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (named_unit == nullptr || named_unit->ptr == nullptr) { throw std::runtime_error("Handle parameter \"named_unit\" is invalid"); }
    auto named_unit_cpp = named_unit->ptr;
        auto generated_result = [&]() {
if (!named_unit_cpp->declaration().is("IfcNamedUnit")) {
    throw IfcParse::IfcException("Instance is not an IfcNamedUnit.");
}
double scale = 1.0;
IfcUtil::IfcBaseClass* si_unit = nullptr;
if (named_unit_cpp->declaration().is("IfcConversionBasedUnit")) {
    auto* factor = static_cast<IfcUtil::IfcBaseClass*>(named_unit_cpp->get_attribute_value(
        named_unit_cpp->declaration().as_entity()->attribute_index("ConversionFactor")));
    auto* value_component = static_cast<IfcUtil::IfcBaseClass*>(factor->get_attribute_value(
        factor->declaration().as_entity()->attribute_index("ValueComponent")));
    auto* unit_component = static_cast<IfcUtil::IfcBaseClass*>(factor->get_attribute_value(
        factor->declaration().as_entity()->attribute_index("UnitComponent")));
    scale = static_cast<double>(value_component->get_attribute_value(0));
    if (unit_component->declaration().is("IfcSIUnit")) {
        si_unit = unit_component;
    }
} else if (named_unit_cpp->declaration().is("IfcSIUnit")) {
    si_unit = named_unit_cpp;
}
if (si_unit != nullptr) {
    AttributeValue prefix = si_unit->get_attribute_value(
        si_unit->declaration().as_entity()->attribute_index("Prefix"));
    if (!prefix.isNull()) {
        scale *= IfcParse::IfcSIPrefixToValue(static_cast<std::string>(prefix));
    }
} else {
    scale = 0.0;
}
return scale;
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_get_info_cpp(ifcopenshell_ifc_instance_t* instance, bool include_identifier, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
return instance_to_info_json_string(instance_cpp, include_identifier);
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_operator_token_ptr(size_t start, const char* data, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (data == nullptr) { throw std::runtime_error("Parameter \"data\" must not be null"); }
    std::string data_cpp(data);
        auto generated_result = [&]() {
const char value = data_cpp.empty() ? '$' : data_cpp.front();
return static_cast<int>(IfcParse::OperatorTokenPtr(nullptr, start, value).type);
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_general_token_ptr(size_t start, const char* token, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (token == nullptr) { throw std::runtime_error("Parameter \"token\" must not be null"); }
    std::string token_cpp(token);
        auto generated_result = [&]() {
return static_cast<int>(IfcParse::GeneralTokenPtr(nullptr, start, token_cpp).type);
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_new_instance(const char* schema_identifier, const char* declaration_name, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (schema_identifier == nullptr) { throw std::runtime_error("Parameter \"schema_identifier\" must not be null"); }
    std::string schema_identifier_cpp(schema_identifier);
    if (declaration_name == nullptr) { throw std::runtime_error("Parameter \"declaration_name\" must not be null"); }
    std::string declaration_name_cpp(declaration_name);
        auto generated_result = [&]() {
const IfcParse::schema_definition* schema = IfcParse::schema_by_name(schema_identifier_cpp);
const IfcParse::declaration* decl = schema->declaration_by_name(declaration_name_cpp);
IfcEntityInstanceData data(in_memory_attribute_storage(decl->as_entity() ? decl->as_entity()->attribute_count() : 1));
auto* inst = schema->instantiate(decl, std::move(data));
if (auto* entinst = inst->as<IfcUtil::IfcBaseEntity>()) {
    entinst->populate_derived();
}
return inst;
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_settings(ifcopenshell_ifcgeom_settings_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcgeom_settings_t{new ifcopenshell::geometry::Settings(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_serializer_settings(ifcopenshell_ifcgeom_serializer_settings_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcgeom_serializer_settings_t{new ifcopenshell::geometry::SerializerSettings(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_buffer(ifcopenshell_ifcgeom_buffer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcgeom_buffer_t{new stream_or_filename(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_buffer_from_filename(const char* filename, ifcopenshell_ifcgeom_buffer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
        *out_result = new ifcopenshell_ifcgeom_buffer_t{new stream_or_filename(filename_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_obj_serializer(ifcopenshell_ifcgeom_buffer_t* obj_output, ifcopenshell_ifcgeom_buffer_t* mtl_output, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (obj_output == nullptr || obj_output->ptr == nullptr) { throw std::runtime_error("Handle parameter \"obj_output\" is invalid"); }
    auto obj_output_cpp = obj_output->ptr;
    if (mtl_output == nullptr || mtl_output->ptr == nullptr) { throw std::runtime_error("Handle parameter \"mtl_output\" is invalid"); }
    auto mtl_output_cpp = mtl_output->ptr;
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new WaveFrontOBJSerializer(*obj_output_cpp, *mtl_output_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_ttl_serializer(ifcopenshell_ifcgeom_buffer_t* output, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (output == nullptr || output->ptr == nullptr) { throw std::runtime_error("Handle parameter \"output\" is invalid"); }
    auto output_cpp = output->ptr;
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new TtlWktSerializer(*output_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_svg_serializer(ifcopenshell_ifcgeom_buffer_t* output, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (output == nullptr || output->ptr == nullptr) { throw std::runtime_error("Handle parameter \"output\" is invalid"); }
    auto output_cpp = output->ptr;
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new SvgSerializer(*output_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_gltf_serializer(const char* filename, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        #if defined(WITH_GLTF)
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new GltfSerializer(filename_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_gltf_serializer requires WITH_GLTF");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_iges_serializer(const char* filename, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        #if defined(IFOPSH_WITH_OPENCASCADE)
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new IgesSerializer(filename_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_iges_serializer requires IFOPSH_WITH_OPENCASCADE");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_step_serializer(const char* filename, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        #if defined(IFOPSH_WITH_OPENCASCADE)
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new StepSerializer(filename_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_step_serializer requires IFOPSH_WITH_OPENCASCADE");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_collada_serializer(const char* filename, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        #if defined(WITH_OPENCOLLADA)
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new ColladaSerializer(filename_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_collada_serializer requires WITH_OPENCOLLADA");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_hdf_serializer(const char* filename, ifcopenshell_ifcgeom_settings_t* geometry_settings, ifcopenshell_ifcgeom_serializer_settings_t* serializer_settings, ifcopenshell_ifcgeom_geometry_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
    if (geometry_settings == nullptr || geometry_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"geometry_settings\" is invalid"); }
    auto geometry_settings_cpp = geometry_settings->ptr;
    if (serializer_settings == nullptr || serializer_settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"serializer_settings\" is invalid"); }
    auto serializer_settings_cpp = serializer_settings->ptr;
        #if defined(WITH_HDF5) && defined(IFOPSH_WITH_OPENCASCADE)
        *out_result = new ifcopenshell_ifcgeom_geometry_serializer_t{static_cast<GeometrySerializer*>(new HdfSerializer(filename_cpp, *geometry_settings_cpp, *serializer_settings_cpp)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_hdf_serializer requires defined(WITH_HDF5) && defined(IFOPSH_WITH_OPENCASCADE)");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_json_serializer(ifcopenshell_ifc_file_t* file, const char* filename, ifcopenshell_ifcgeom_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
        auto generated_result = [&]() {
#ifdef WITH_GLTF
return new JsonSerializer(file_cpp, std::string(filename));
#else
throw std::runtime_error("JSON serializer requires GLTF support (nlohmann_json)");
#endif
        }();
        *out_result = new ifcopenshell_ifcgeom_serializer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_xml_serializer(ifcopenshell_ifc_file_t* file, const char* filename, ifcopenshell_ifcgeom_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (filename == nullptr) { throw std::runtime_error("Parameter \"filename\" must not be null"); }
    std::string filename_cpp(filename);
        auto generated_result = [&]() {
return new XmlSerializer(file_cpp, std::string(filename));
        }();
        *out_result = new ifcopenshell_ifcgeom_serializer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_rocksdb_serializer(ifcopenshell_ifc_file_t* file, const char* rocksdb_filename, ifcopenshell_ifcgeom_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (rocksdb_filename == nullptr) { throw std::runtime_error("Parameter \"rocksdb_filename\" must not be null"); }
    std::string rocksdb_filename_cpp(rocksdb_filename);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_ROCKSDB
return new RocksDbSerializer(file_cpp, std::string(rocksdb_filename));
#else
throw std::runtime_error("RocksDB serializer requires WITH_ROCKSDB support");
#endif
        }();
        *out_result = new ifcopenshell_ifcgeom_serializer_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_rocksdb_serializer_streaming(const char* input_filename, const char* rocksdb_filename, bool stream, ifcopenshell_ifcgeom_serializer_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (input_filename == nullptr) { throw std::runtime_error("Parameter \"input_filename\" must not be null"); }
    std::string input_filename_cpp(input_filename);
    if (rocksdb_filename == nullptr) { throw std::runtime_error("Parameter \"rocksdb_filename\" must not be null"); }
    std::string rocksdb_filename_cpp(rocksdb_filename);
        #if defined(IFOPSH_WITH_ROCKSDB)
        *out_result = new ifcopenshell_ifcgeom_serializer_t{static_cast<Serializer*>(new RocksDbSerializer(input_filename_cpp, rocksdb_filename_cpp, stream)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_rocksdb_serializer_streaming requires IFOPSH_WITH_ROCKSDB");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_tree(ifcopenshell_ifcgeom_tree_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcgeom_tree_t{new IfcGeom::tree(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_tree_from_file(ifcopenshell_ifc_file_t* file, ifcopenshell_ifcgeom_tree_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
        *out_result = new ifcopenshell_ifcgeom_tree_t{new IfcGeom::tree(*file_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_tree_from_file_with_settings(ifcopenshell_ifc_file_t* file, ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifcgeom_tree_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
        *out_result = new ifcopenshell_ifcgeom_tree_t{new IfcGeom::tree(*file_cpp, *settings_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_tree_from_iterator(ifcopenshell_ifcgeom_iterator_t* iterator, ifcopenshell_ifcgeom_tree_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (iterator == nullptr || iterator->ptr == nullptr) { throw std::runtime_error("Handle parameter \"iterator\" is invalid"); }
    auto iterator_cpp = iterator->ptr;
        *out_result = new ifcopenshell_ifcgeom_tree_t{new IfcGeom::tree(*iterator_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_iterator(const char* geometry_library, ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_file_t* file, int32_t num_threads, ifcopenshell_ifcgeom_iterator_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (geometry_library == nullptr) { throw std::runtime_error("Parameter \"geometry_library\" must not be null"); }
    std::string geometry_library_cpp(geometry_library);
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
        auto generated_result = [&]() {
auto* settings_ptr = settings_cpp;
auto kernel = ifcopenshell::geometry::kernels::construct(file_cpp, geometry_library_cpp, *settings_ptr);
return new IfcGeom::Iterator(std::move(kernel), *settings_ptr, file_cpp, num_threads);
        }();
        *out_result = new ifcopenshell_ifcgeom_iterator_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_iterator_with_include_exclude(const char* geometry_library, ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_file_t* file, const ifcopenshell_string_list_t* elems, bool include, int32_t num_threads, ifcopenshell_ifcgeom_iterator_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (geometry_library == nullptr) { throw std::runtime_error("Parameter \"geometry_library\" must not be null"); }
    std::string geometry_library_cpp(geometry_library);
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (elems == nullptr) { throw std::runtime_error("Parameter \"elems\" must not be null"); }
    auto elems_cpp = to_cpp_string_list(elems);
        auto generated_result = [&]() {
auto* settings_ptr = settings_cpp;
auto kernel = ifcopenshell::geometry::kernels::construct(file_cpp, geometry_library_cpp, *settings_ptr);
std::set<std::string> elems_set(elems_cpp.begin(), elems_cpp.end());
IfcGeom::entity_filter ef{include, false, elems_set};
return new IfcGeom::Iterator(std::move(kernel), *settings_ptr, file_cpp, {ef}, num_threads);
        }();
        *out_result = new ifcopenshell_ifcgeom_iterator_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_iterator_with_include_exclude_globalid(const char* geometry_library, ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_file_t* file, const ifcopenshell_string_list_t* elems, bool include, int32_t num_threads, ifcopenshell_ifcgeom_iterator_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (geometry_library == nullptr) { throw std::runtime_error("Parameter \"geometry_library\" must not be null"); }
    std::string geometry_library_cpp(geometry_library);
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (elems == nullptr) { throw std::runtime_error("Parameter \"elems\" must not be null"); }
    auto elems_cpp = to_cpp_string_list(elems);
        auto generated_result = [&]() {
auto* settings_ptr = settings_cpp;
auto kernel = ifcopenshell::geometry::kernels::construct(file_cpp, geometry_library_cpp, *settings_ptr);
std::set<std::string> elems_set(elems_cpp.begin(), elems_cpp.end());
IfcGeom::attribute_filter af;
af.attribute_name = "GlobalId";
af.populate(elems_set);
af.include = include;
return new IfcGeom::Iterator(std::move(kernel), *settings_ptr, file_cpp, {af}, num_threads);
        }();
        *out_result = new ifcopenshell_ifcgeom_iterator_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_iterator_with_include_exclude_id(const char* geometry_library, ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_file_t* file, const ifcopenshell_int32_list_t* elems, bool include, int32_t num_threads, ifcopenshell_ifcgeom_iterator_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (geometry_library == nullptr) { throw std::runtime_error("Parameter \"geometry_library\" must not be null"); }
    std::string geometry_library_cpp(geometry_library);
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (elems == nullptr) { throw std::runtime_error("Parameter \"elems\" must not be null"); }
    auto elems_cpp = to_cpp_int32_list(elems);
        auto generated_result = [&]() {
auto* settings_ptr = settings_cpp;
auto kernel = ifcopenshell::geometry::kernels::construct(file_cpp, geometry_library_cpp, *settings_ptr);
std::set<int> elems_set(elems_cpp.begin(), elems_cpp.end());
IfcGeom::instance_id_filter af(include, false, elems_set);
return new IfcGeom::Iterator(std::move(kernel), *settings_ptr, file_cpp, {af}, num_threads);
        }();
        *out_result = new ifcopenshell_ifcgeom_iterator_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_point3(double x, double y, double z, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::point3>(x, y, z);
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_direction3(double x, double y, double z, ifcopenshell_ifcgeom_taxonomy_direction3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
Eigen::Vector3d direction(x, y, z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
return ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::direction3>(direction);
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_direction3_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_line(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, ifcopenshell_ifcgeom_taxonomy_line_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::line>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_line_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_circle(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_ifcgeom_taxonomy_circle_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (radius <= 0.) {
    throw std::runtime_error("Radius must be > 0");
}
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::circle>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
value->radius = radius;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_circle_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_ellipse(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius1, double radius2, ifcopenshell_ifcgeom_taxonomy_ellipse_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (radius1 <= 0. || radius2 <= 0.) {
    throw std::runtime_error("Radii must be > 0");
}
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::ellipse>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
value->radius = radius1;
value->radius2 = radius2;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_ellipse_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_plane(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, ifcopenshell_ifcgeom_taxonomy_plane_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::plane>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_plane_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_cylinder(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_ifcgeom_taxonomy_cylinder_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (radius <= 0.) {
    throw std::runtime_error("Radius must be > 0");
}
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::cylinder>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
value->radius = radius;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_cylinder_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_sphere(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_ifcgeom_taxonomy_sphere_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (radius <= 0.) {
    throw std::runtime_error("Radius must be > 0");
}
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::sphere>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
value->radius = radius;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_sphere_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_torus(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius1, double radius2, ifcopenshell_ifcgeom_taxonomy_torus_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (radius1 <= 0. || radius2 <= 0.) {
    throw std::runtime_error("Radii must be > 0");
}
Eigen::Vector3d origin(origin_x, origin_y, origin_z);
Eigen::Vector3d direction(dir_x, dir_y, dir_z);
if (direction.squaredNorm() <= 1.e-12) {
    throw std::runtime_error("Direction vector must be non-zero");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::torus>();
value->matrix = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(origin, direction);
value->radius1 = radius1;
value->radius2 = radius2;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_torus_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_bspline_curve(int32_t degree, ifcopenshell_ifcgeom_taxonomy_bspline_curve_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (degree < 1) {
    throw std::runtime_error("B-spline curve degree must be >= 1");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::bspline_curve>();
value->degree = degree;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_bspline_curve_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_offset_curve(ifcopenshell_ifcgeom_taxonomy_item_t* basis, ifcopenshell_ifcgeom_taxonomy_direction3_t* reference, double offset, ifcopenshell_ifcgeom_taxonomy_offset_curve_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (basis == nullptr || basis->ptr == nullptr) { throw std::runtime_error("Handle parameter \"basis\" is invalid"); }
    auto basis_cpp = basis->ptr;
    if (reference == nullptr || reference->ptr == nullptr) { throw std::runtime_error("Handle parameter \"reference\" is invalid"); }
    auto reference_cpp = reference->ptr;
        auto generated_result = [&]() {
auto basis_value = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::item>(basis_cpp);
if (!basis_value) {
    throw std::runtime_error("Offset-curve basis is invalid");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::offset_curve>();
value->basis = basis_value;
value->reference = reference_cpp;
value->offset = offset;
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_offset_curve_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_bspline_surface(int32_t degree_u, int32_t degree_v, ifcopenshell_ifcgeom_taxonomy_bspline_surface_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (degree_u < 1 || degree_v < 1) {
    throw std::runtime_error("B-spline surface degrees must be >= 1");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::bspline_surface>();
value->degree = { degree_u, degree_v };
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_bspline_surface_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_box(double dx, double dy, double dz, ifcopenshell_ifcgeom_taxonomy_solid_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (dx <= 0. || dy <= 0. || dz <= 0.) {
    throw std::runtime_error("Box dimensions must be > 0");
}
return ifcopenshell::geometry::create_box(dx, dy, dz);
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_solid_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_collection(ifcopenshell_ifcgeom_taxonomy_collection_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::collection>();
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_collection_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_loft(ifcopenshell_ifcgeom_taxonomy_loft_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::loft>();
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_loft_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_node(ifcopenshell_ifcgeom_taxonomy_node_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::node>();
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_node_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_boolean_result(int32_t operation, ifcopenshell_ifcgeom_taxonomy_boolean_result_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
if (operation < 0 || operation > 2) {
    throw std::runtime_error("Boolean operation must be 0 (UNION), 1 (SUBTRACTION), or 2 (INTERSECTION)");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::boolean_result>();
value->operation = static_cast<ifcopenshell::geometry::taxonomy::boolean_result::operation_t>(operation);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_boolean_result_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_extrusion(ifcopenshell_ifcgeom_taxonomy_item_t* basis, ifcopenshell_ifcgeom_taxonomy_direction3_t* direction, double depth, ifcopenshell_ifcgeom_taxonomy_extrusion_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (basis == nullptr || basis->ptr == nullptr) { throw std::runtime_error("Handle parameter \"basis\" is invalid"); }
    auto basis_cpp = basis->ptr;
    if (direction == nullptr || direction->ptr == nullptr) { throw std::runtime_error("Handle parameter \"direction\" is invalid"); }
    auto direction_cpp = direction->ptr;
        auto generated_result = [&]() {
auto basis_value = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::item>(basis_cpp);
if (!basis_value) {
    throw std::runtime_error("Basis item is invalid");
}
if (depth <= 0.) {
    throw std::runtime_error("Extrusion depth must be > 0");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::extrusion>(
    ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(),
    basis_value,
    direction_cpp,
    depth
);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_extrusion_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_revolve(ifcopenshell_ifcgeom_taxonomy_item_t* basis, ifcopenshell_ifcgeom_taxonomy_point3_t* axis_origin, ifcopenshell_ifcgeom_taxonomy_direction3_t* direction, double angle, ifcopenshell_ifcgeom_taxonomy_revolve_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (basis == nullptr || basis->ptr == nullptr) { throw std::runtime_error("Handle parameter \"basis\" is invalid"); }
    auto basis_cpp = basis->ptr;
    if (axis_origin == nullptr || axis_origin->ptr == nullptr) { throw std::runtime_error("Handle parameter \"axis_origin\" is invalid"); }
    auto axis_origin_cpp = axis_origin->ptr;
    if (direction == nullptr || direction->ptr == nullptr) { throw std::runtime_error("Handle parameter \"direction\" is invalid"); }
    auto direction_cpp = direction->ptr;
        auto generated_result = [&]() {
auto basis_value = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::item>(basis_cpp);
if (!basis_value) {
    throw std::runtime_error("Basis item is invalid");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::revolve>(
    ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(),
    basis_value,
    axis_origin_cpp,
    direction_cpp,
    boost::optional<double>(angle)
);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_revolve_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_create_sweep_along_curve(ifcopenshell_ifcgeom_taxonomy_face_t* basis_face, ifcopenshell_ifcgeom_taxonomy_item_t* directrix, ifcopenshell_ifcgeom_taxonomy_direction3_t* reference_direction, ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (basis_face == nullptr || basis_face->ptr == nullptr) { throw std::runtime_error("Handle parameter \"basis_face\" is invalid"); }
    auto basis_face_cpp = basis_face->ptr;
    if (directrix == nullptr || directrix->ptr == nullptr) { throw std::runtime_error("Handle parameter \"directrix\" is invalid"); }
    auto directrix_cpp = directrix->ptr;
    if (reference_direction == nullptr || reference_direction->ptr == nullptr) { throw std::runtime_error("Handle parameter \"reference_direction\" is invalid"); }
    auto reference_direction_cpp = reference_direction->ptr;
        auto generated_result = [&]() {
auto directrix_value = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::item>(directrix_cpp);
if (!directrix_value) {
    throw std::runtime_error("Directrix item is invalid");
}
auto value = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::sweep_along_curve>(
    ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(),
    basis_face_cpp,
    directrix_value,
    reference_direction_cpp
);
return value;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_shape(ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifc_instance_t* representation, const char* geometry_library, ifcopenshell_ifcgeom_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto representation_cpp = (representation != nullptr && representation->ptr != nullptr) ? representation->ptr : nullptr;
    const char* geometry_library_str = geometry_library;
        auto generated_result = [&]() {
IfcParse::IfcFile* file = instance_cpp->file_;
if (!file) {
    throw std::runtime_error("Instance has no associated file");
}
std::string geom_lib = geometry_library_str ? std::string(geometry_library_str) : "opencascade";
ifcopenshell::geometry::Converter kernel(
    ifcopenshell::geometry::kernels::construct(file, geom_lib, *settings_cpp),
    file, *settings_cpp);

auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>();
if (!entity) {
    throw std::runtime_error("Instance is not an entity");
}

// Handle IfcProduct instances
if (entity->declaration().is("IfcProduct")) {
    IfcUtil::IfcBaseClass* ifc_representation = representation_cpp;

    if (!ifc_representation) {
        // Find representation from product
        auto prod_rep_attr = entity->get("Representation");
        if (prod_rep_attr.isNull()) {
            throw IfcParse::IfcException("Representation is NULL");
        }
        auto* prod_rep = (IfcUtil::IfcBaseClass*)prod_rep_attr;
        auto* prod_rep_entity = prod_rep->as<IfcUtil::IfcBaseEntity>();
        auto reps_attr = prod_rep_entity->get("Representations");
        auto reps = (boost::shared_ptr<aggregate_of_instance>)reps_attr;

        if (!reps || reps->size() == 0) {
            throw IfcParse::IfcException("No suitable IfcRepresentation found");
        }

        bool is_curves = settings_cpp->get<ifcopenshell::geometry::settings::OutputDimensionality>().get()
            == ifcopenshell::geometry::settings::CURVES;

        // Strategy 1: Find by RepresentationIdentifier
        for (auto it = reps->begin(); it != reps->end(); ++it) {
            auto* rep_entity = (*it)->as<IfcUtil::IfcBaseEntity>();
            if (!rep_entity) continue;
            auto id_attr = rep_entity->get("RepresentationIdentifier");
            if (id_attr.isNull()) continue;
            std::string rep_id = (std::string)id_attr;
            if (!is_curves) {
                if (rep_id == "Body" || rep_id == "Facetation") {
                    ifc_representation = *it;
                    break;
                }
            } else {
                if (rep_id == "Plan" || rep_id == "Axis") {
                    ifc_representation = *it;
                    break;
                }
            }
        }

        // Strategy 2: Find by ContextType
        if (!ifc_representation) {
            for (auto it = reps->begin(); it != reps->end(); ++it) {
                auto* rep_entity = (*it)->as<IfcUtil::IfcBaseEntity>();
                if (!rep_entity) continue;
                auto ctx_attr = rep_entity->get("ContextOfItems");
                if (ctx_attr.isNull()) continue;
                auto* ctx = ((IfcUtil::IfcBaseClass*)ctx_attr)->as<IfcUtil::IfcBaseEntity>();
                if (!ctx) continue;
                auto ct_attr = ctx->get("ContextType");
                if (ct_attr.isNull()) continue;
                std::string context_type = (std::string)ct_attr;
                std::transform(context_type.begin(), context_type.end(), context_type.begin(), ::tolower);
                std::set<std::string> valid_types;
                if (!is_curves) {
                    valid_types = {"model", "design", "model view", "detail view"};
                } else {
                    valid_types = {"plan"};
                }
                if (valid_types.count(context_type)) {
                    ifc_representation = *it;
                }
            }
        }

        // Strategy 3: Use first available
        if (!ifc_representation) {
            ifc_representation = *reps->begin();
        }
    } else {
        if (!ifc_representation->declaration().is("IfcRepresentation")) {
            throw IfcParse::IfcException("Supplied representation not of type IfcRepresentation");
        }
    }

    auto* rep_entity = ifc_representation->as<IfcUtil::IfcBaseEntity>();
    IfcGeom::BRepElement* brep = kernel.create_brep_for_representation_and_product(rep_entity, entity);
    if (!brep) {
        throw IfcParse::IfcException("Failed to process shape");
    }

    auto output_type = settings_cpp->get<ifcopenshell::geometry::settings::IteratorOutput>().get();
    if (output_type == ifcopenshell::geometry::settings::SERIALIZED) {
        IfcGeom::Element* result = new IfcGeom::SerializedElement(*brep);
        delete brep;
        return result;
    } else if (output_type == ifcopenshell::geometry::settings::TRIANGULATED) {
        IfcGeom::Element* result = new IfcGeom::TriangulationElement(*brep);
        delete brep;
        return result;
    }
    return static_cast<IfcGeom::Element*>(brep);
}
// Handle representation items directly (mirrors SWIG's behaviour
// for IfcRepresentationItem / IfcRepresentation / IfcProfileDef).
else if (entity->declaration().is("IfcRepresentationItem") ||
         entity->declaration().is("IfcRepresentation") ||
         entity->declaration().is("IfcProfileDef")) {
    IfcGeom::ConversionResults shapes;
    try {
        shapes = kernel.convert(instance_cpp);
    } catch (...) {
        throw IfcParse::IfcException("Failed to process representation item");
    }
    if (shapes.empty()) {
        throw IfcParse::IfcException(std::string("kernel.convert produced no shapes for ") + entity->declaration().name() + " #" + std::to_string(entity->id()));
    }
    auto brep_rep = boost::shared_ptr<IfcGeom::Representation::BRep>(
        new IfcGeom::Representation::BRep(
            kernel.settings(), entity->declaration().name(),
            std::to_string(entity->id()), shapes));

    auto output_type = settings_cpp->get<ifcopenshell::geometry::settings::IteratorOutput>().get();
    // Build a lightweight BRepElement that wraps the bare
    // representation so the resulting handle can flow through
    // ``element_geometry`` accessors uniformly.
    auto identity = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>();
    const IfcUtil::IfcBaseEntity* null_product = nullptr;
    auto* brep_elem = new IfcGeom::BRepElement(
        entity->id(), -1,
        entity->declaration().name(),
        entity->declaration().name(),
        std::string(),
        std::string(),
        identity, brep_rep,
        null_product);
    if (output_type == ifcopenshell::geometry::settings::SERIALIZED) {
        IfcGeom::Element* result = new IfcGeom::SerializedElement(*brep_elem);
        delete brep_elem;
        return result;
    }
    if (output_type == ifcopenshell::geometry::settings::TRIANGULATED) {
        IfcGeom::Element* result = new IfcGeom::TriangulationElement(*brep_elem);
        delete brep_elem;
        return result;
    }
    return static_cast<IfcGeom::Element*>(brep_elem);
}
throw IfcParse::IfcException("Unsupported instance type for create_shape. Use map_shape for placements.");
        }();
        *out_result = new ifcopenshell_ifcgeom_element_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_map_shape(ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
if (instance_cpp->file_ == nullptr) {
    throw std::runtime_error("Unable to map instance without file");
}
std::unique_ptr<ifcopenshell::geometry::abstract_mapping> mapping(
    ifcopenshell::geometry::impl::mapping_implementations().construct(instance_cpp->file_, *settings_cpp));
return mapping->map(instance_cpp);
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serialise(const char* schema_name, const char* shape_str, bool advanced, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (schema_name == nullptr) { throw std::runtime_error("Parameter \"schema_name\" must not be null"); }
    std::string schema_name_cpp(schema_name);
    if (shape_str == nullptr) { throw std::runtime_error("Parameter \"shape_str\" must not be null"); }
    std::string shape_str_cpp(shape_str);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_OPENCASCADE
std::string shape_string(shape_str_cpp);
std::stringstream stream{shape_string};
BRepTools_ShapeSet shapes;
shapes.Read(stream);
const TopoDS_Shape& shp = shapes.Shape(shapes.NbShapes());
return IfcGeom::serialise(std::string(schema_name_cpp), shp, advanced);
#else
(void)schema_name_cpp; (void)shape_str_cpp; (void)advanced;
throw std::runtime_error("serialise requires IFOPSH_WITH_OPENCASCADE");
return (IfcUtil::IfcBaseClass*)nullptr;
#endif
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tesselate(const char* schema_name, const char* shape_str, double deflection, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (schema_name == nullptr) { throw std::runtime_error("Parameter \"schema_name\" must not be null"); }
    std::string schema_name_cpp(schema_name);
    if (shape_str == nullptr) { throw std::runtime_error("Parameter \"shape_str\" must not be null"); }
    std::string shape_str_cpp(shape_str);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_OPENCASCADE
std::string shape_string(shape_str_cpp);
std::stringstream stream{shape_string};
BRepTools_ShapeSet shapes;
shapes.Read(stream);
const TopoDS_Shape& shp = shapes.Shape(shapes.NbShapes());
return IfcGeom::tesselate(std::string(schema_name_cpp), shp, deflection);
#else
(void)schema_name_cpp; (void)shape_str_cpp; (void)deflection;
throw std::runtime_error("tesselate requires IFOPSH_WITH_OPENCASCADE");
return (IfcUtil::IfcBaseClass*)nullptr;
#endif
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_epeck_from_int(int32_t value, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        #if defined(IFOPSH_WITH_CGAL)
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{static_cast<IfcGeom::OpaqueNumber*>(new ifcopenshell::geometry::NumberEpeck(value)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_epeck_from_int requires IFOPSH_WITH_CGAL");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_epeck_from_double(double value, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        #if defined(IFOPSH_WITH_CGAL)
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{static_cast<IfcGeom::OpaqueNumber*>(new ifcopenshell::geometry::NumberEpeck(value)), true};
#else
        throw std::runtime_error("ifcopenshell_ifcgeom_create_epeck_from_double requires IFOPSH_WITH_CGAL");
#endif
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_epeck_from_string(const char* value, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
return new ifcopenshell::geometry::NumberEpeck(typename CGAL::Epeck::FT::ET(std::string(value_cpp)));
#else
(void)value_cpp;
throw std::runtime_error("create_epeck requires IFOPSH_WITH_CGAL");
return (IfcGeom::OpaqueNumber*)nullptr;
#endif
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_nary_union(const ifcopenshell_ifcgeom_conversion_result_shape_list_t* shapes, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (shapes == nullptr) { throw std::runtime_error("Parameter \"shapes\" must not be null"); }
    auto shapes_cpp = to_cpp_ifcgeom_conversion_result_shape_list(shapes);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
std::vector<const CGAL::Nef_polyhedron_3<CGAL::Epeck>*> nefs;
for (auto* shape : shapes_cpp) {
    auto* cgs = dynamic_cast<ifcopenshell::geometry::CgalShape*>(const_cast<IfcGeom::ConversionResultShape*>(shape));
    if (cgs) {
        nefs.push_back(&cgs->nef());
    }
}
CGAL::Nef_nary_union_3<CGAL::Nef_polyhedron_3<CGAL::Epeck>> accum;
for (auto* n : nefs) {
    accum.add_polyhedron(*n);
}
return new ifcopenshell::geometry::CgalShape(accum.get_union());
#else
(void)shapes_cpp;
throw std::runtime_error("nary_union requires IFOPSH_WITH_CGAL");
return (IfcGeom::ConversionResultShape*)nullptr;
#endif
        }();
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svg_to_line_segments(const char* svg_data, const char* class_name, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (svg_data == nullptr) { throw std::runtime_error("Parameter \"svg_data\" must not be null"); }
    std::string svg_data_cpp(svg_data);
    const char* class_name_str = class_name;
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
std::vector<std::vector<svgfill::line_segment_2>> segments;
boost::optional<std::string> cn;
if (class_name_str) cn = std::string(class_name_str);
if (!svgfill::svg_to_line_segments(std::string(svg_data_cpp), cn, segments)) {
    throw std::runtime_error("Failed to read SVG");
}
// Serialize as JSON-like string: [[[[x1,y1],[x2,y2]], ...], ...]
std::ostringstream oss;
oss << "[";
for (size_t g = 0; g < segments.size(); ++g) {
    if (g > 0) oss << ",";
    oss << "[";
    for (size_t s = 0; s < segments[g].size(); ++s) {
        if (s > 0) oss << ",";
        oss << "[[" << segments[g][s][0][0] << "," << segments[g][s][0][1]
            << "],[" << segments[g][s][1][0] << "," << segments[g][s][1][1] << "]]";
    }
    oss << "]";
}
oss << "]";
return oss.str();
#else
(void)svg_data_cpp; (void)class_name_str;
throw std::runtime_error("svg_to_line_segments requires IFOPSH_WITH_CGAL");
return std::string();
#endif
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svg_to_polygons(const char* svg_data, const char* class_name, ifcopenshell_ifcgeom_svgfill_polygon_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (svg_data == nullptr) { throw std::runtime_error("Parameter \"svg_data\" must not be null"); }
    std::string svg_data_cpp(svg_data);
    const char* class_name_str = class_name;
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
std::vector<svgfill::polygon_2> polygons;
boost::optional<std::string> cn;
if (class_name_str) cn = std::string(class_name_str);
if (!svgfill::svg_to_polygons(std::string(svg_data_cpp), cn, polygons)) {
    throw std::runtime_error("Failed to read SVG");
}
std::vector<const svgfill::polygon_2*> result;
for (auto& p : polygons) {
    result.push_back(new svgfill::polygon_2(p));
}
return result;
#else
(void)svg_data_cpp; (void)class_name_str;
throw std::runtime_error("svg_to_polygons requires IFOPSH_WITH_CGAL");
return std::vector<const svgfill::polygon_2*>();
#endif
        }();
        *out_result = make_ifcgeom_svgfill_polygon_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_arrange_polygons(const ifcopenshell_ifcgeom_svgfill_polygon_list_t* polygons, ifcopenshell_ifcgeom_svgfill_polygon_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (polygons == nullptr) { throw std::runtime_error("Parameter \"polygons\" must not be null"); }
    auto polygons_cpp = to_cpp_ifcgeom_svgfill_polygon_list(polygons);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
std::vector<svgfill::polygon_2> input;
for (auto* p : polygons_cpp) {
    input.push_back(*p);
}
std::vector<svgfill::polygon_2> arranged;
if (!svgfill::arrange_polygons(input, arranged)) {
    throw std::runtime_error("Failed to arrange polygons");
}
std::vector<const svgfill::polygon_2*> result;
for (auto& p : arranged) {
    result.push_back(new svgfill::polygon_2(p));
}
return result;
#else
(void)polygons_cpp;
throw std::runtime_error("arrange_polygons requires IFOPSH_WITH_CGAL");
return std::vector<const svgfill::polygon_2*>();
#endif
        }();
        *out_result = make_ifcgeom_svgfill_polygon_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_line_segments_to_polygons(int32_t solver, double eps, const char* segments_json, ifcopenshell_ifcgeom_svgfill_polygon_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (segments_json == nullptr) { throw std::runtime_error("Parameter \"segments_json\" must not be null"); }
    std::string segments_json_cpp(segments_json);
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
// Parse segments from the JSON-like string produced by svg_to_line_segments
// Format: [[[[x1,y1],[x2,y2]], ...], ...]
std::vector<std::vector<svgfill::line_segment_2>> segments;
// Simple parser for the nested array format
std::string s(segments_json_cpp);
size_t pos = 0;
auto skip_ws = [&]() { while (pos < s.size() && isspace(s[pos])) pos++; };
auto expect = [&](char c) { skip_ws(); if (pos < s.size() && s[pos] == c) { pos++; return true; } return false; };
auto parse_double = [&]() -> double {
    skip_ws();
    size_t end;
    double v = std::stod(s.substr(pos), &end);
    pos += end;
    return v;
};
if (expect('[')) {
    do {
        skip_ws();
        if (s[pos] == ']') break;
        std::vector<svgfill::line_segment_2> group;
        if (expect('[')) {
            do {
                skip_ws();
                if (s[pos] == ']') break;
                svgfill::line_segment_2 seg;
                expect('['); expect('[');
                seg[0][0] = parse_double(); expect(',');
                seg[0][1] = parse_double();
                expect(']'); expect(','); expect('[');
                seg[1][0] = parse_double(); expect(',');
                seg[1][1] = parse_double();
                expect(']'); expect(']');
            group.push_back(seg);
            } while (expect(','));
            expect(']');
        }
        segments.push_back(group);
    } while (expect(','));
    expect(']');
}
std::vector<std::vector<svgfill::polygon_2>> polygon_groups;
if (!svgfill::line_segments_to_polygons(static_cast<svgfill::solver>(solver), eps, segments, polygon_groups)) {
    throw std::runtime_error("Failed to process line segments");
}
// Flatten all groups into a single list
std::vector<const svgfill::polygon_2*> result;
for (auto& group : polygon_groups) {
    for (auto& p : group) {
        result.push_back(new svgfill::polygon_2(p));
    }
}
return result;
#else
(void)solver; (void)eps; (void)segments_json_cpp;
throw std::runtime_error("line_segments_to_polygons requires IFOPSH_WITH_CGAL");
return std::vector<const svgfill::polygon_2*>();
#endif
        }();
        *out_result = make_ifcgeom_svgfill_polygon_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_helmert_curve_point(double A0, double A1, double A2, double s, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        auto generated_result = [&]() {
return ifcopenshell::geometry::helmert_curve_point(A0, A1, A2, s);
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_convert_loop_to_function_item(ifcopenshell_ifcgeom_taxonomy_item_t* loop_item, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (loop_item == nullptr || loop_item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"loop_item\" is invalid"); }
    auto loop_item_cpp = loop_item->ptr;
        auto generated_result = [&]() {
auto loop = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::loop>(loop_item_cpp);
if (!loop) {
    throw std::runtime_error("Input is not a taxonomy loop");
}
auto result = ifcopenshell::geometry::convert_loop_to_function_item(loop);
if (!result) {
    throw std::runtime_error("Failed to convert loop to function_item");
}
return result;
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_create_function_item_evaluator(ifcopenshell_ifcgeom_settings_t* settings, ifcopenshell_ifcgeom_taxonomy_item_t* fn_item, ifcopenshell_ifcgeom_function_item_evaluator_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = settings->ptr;
    if (fn_item == nullptr || fn_item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"fn_item\" is invalid"); }
    auto fn_item_cpp = fn_item->ptr;
        auto generated_result = [&]() {
auto fn = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(fn_item_cpp);
if (!fn) {
    throw std::runtime_error("Input is not a taxonomy function_item");
}
return new ifcopenshell::geometry::function_item_evaluator(*settings_cpp, fn);
        }();
        *out_result = new ifcopenshell_ifcgeom_function_item_evaluator_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_function_item_start(ifcopenshell_ifcgeom_taxonomy_item_t* item, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
        auto generated_result = [&]() {
auto fn = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(item_cpp);
if (!fn) {
    throw std::runtime_error("Input is not a taxonomy function_item");
}
return fn->start();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_function_item_end(ifcopenshell_ifcgeom_taxonomy_item_t* item, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
        auto generated_result = [&]() {
auto fn = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(item_cpp);
if (!fn) {
    throw std::runtime_error("Input is not a taxonomy function_item");
}
return fn->end();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_compute_derived(ifcopenshell_ifc_instance_t* instance, const char* attribute_name, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    if (attribute_name == nullptr) { throw std::runtime_error("Parameter \"attribute_name\" must not be null"); }
    std::string attribute_name_cpp(attribute_name);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::compute_derived(instance_cpp, attribute_name_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_aggregate(ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_aggregate(instance_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_contained(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_contained(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_container(ifcopenshell_ifc_instance_t* instance, bool direct_only, const char* ifc_class, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto direct_only_cpp = static_cast<bool>(direct_only);
    const char* ifc_class_str = ifc_class;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_container(instance_cpp, direct_only_cpp, ifc_class), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_controls(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_controls(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_decomposition(ifcopenshell_ifc_instance_t* element, bool is_recursive, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    auto is_recursive_cpp = static_cast<bool>(is_recursive);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_decomposition(element_cpp, is_recursive_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_elements_by_layer(ifcopenshell_ifc_instance_t* layer, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (layer == nullptr || layer->ptr == nullptr) { throw std::runtime_error("Handle parameter \"layer\" is invalid"); }
    auto layer_cpp = layer->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_elements_by_layer(layer_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_elements_by_material(ifcopenshell_ifc_instance_t* material, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (material == nullptr || material->ptr == nullptr) { throw std::runtime_error("Handle parameter \"material\" is invalid"); }
    auto material_cpp = material->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_elements_by_material(material_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_elements_by_profile(ifcopenshell_ifc_instance_t* profile, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (profile == nullptr || profile->ptr == nullptr) { throw std::runtime_error("Handle parameter \"profile\" is invalid"); }
    auto profile_cpp = profile->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_elements_by_profile(profile_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_elements_by_representation(ifcopenshell_ifc_instance_t* representation, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_elements_by_representation(representation_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_elements_by_style(ifcopenshell_ifc_instance_t* style, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (style == nullptr || style->ptr == nullptr) { throw std::runtime_error("Handle parameter \"style\" is invalid"); }
    auto style_cpp = style->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_elements_by_style(style_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_filled_void(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_filled_void(element_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_groups(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_groups(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_layers(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_layers(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_material(ifcopenshell_ifc_instance_t* instance, bool should_skip_usage, bool should_inherit, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto should_skip_usage_cpp = static_cast<bool>(should_skip_usage);
    auto should_inherit_cpp = static_cast<bool>(should_inherit);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_material(instance_cpp, should_skip_usage_cpp, should_inherit_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_nest(ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_nest(instance_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_openings(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_openings(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_parent(ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_parent(instance_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_parts(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_parts(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_pset_ids(ifcopenshell_ifc_instance_t* element, bool psets_only, bool qtos_only, bool should_inherit, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    auto psets_only_cpp = static_cast<bool>(psets_only);
    auto qtos_only_cpp = static_cast<bool>(qtos_only);
    auto should_inherit_cpp = static_cast<bool>(should_inherit);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_pset_ids(element_cpp, psets_only_cpp, qtos_only_cpp, should_inherit_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_referenced_elements(ifcopenshell_ifc_instance_t* reference, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (reference == nullptr || reference->ptr == nullptr) { throw std::runtime_error("Handle parameter \"reference\" is invalid"); }
    auto reference_cpp = reference->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_referenced_elements(reference_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_referenced_structures(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_referenced_structures(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_shape_aspects(ifcopenshell_ifc_instance_t* element, bool should_inherit, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    auto should_inherit_cpp = static_cast<bool>(should_inherit);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_shape_aspects(element_cpp, should_inherit_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_structure_referenced_elements(ifcopenshell_ifc_instance_t* structure, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (structure == nullptr || structure->ptr == nullptr) { throw std::runtime_error("Handle parameter \"structure\" is invalid"); }
    auto structure_cpp = structure->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_structure_referenced_elements(structure_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_styles(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_styles(element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_type(ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_type(instance_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_types(ifcopenshell_ifc_instance_t* type_element, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (type_element == nullptr || type_element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"type_element\" is invalid"); }
    auto type_element_cpp = type_element->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::element_get_types(type_element_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_get_voided_element(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::element_get_voided_element(element_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_is_userdefined_type(ifcopenshell_ifc_instance_t* element, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = ifcapi::bindings::element_is_userdefined_type(element_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_remove_deep(ifcopenshell_ifc_instance_t* element) {
    try {
        ifcopenshell_clear_error();
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        ifcapi::bindings::element_remove_deep(element_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_element_replace_element(ifcopenshell_ifc_instance_t* old_element, ifcopenshell_ifc_instance_t* new_element) {
    try {
        ifcopenshell_clear_error();
    if (old_element == nullptr || old_element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"old_element\" is invalid"); }
    auto old_element_cpp = old_element->ptr;
    if (new_element == nullptr || new_element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"new_element\" is invalid"); }
    auto new_element_cpp = new_element->ptr;
        ifcapi::bindings::element_replace_element(old_element_cpp, new_element_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_entity_remove_deep2(ifcopenshell_ifc_instance_t* instance) {
    try {
        ifcopenshell_clear_error();
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        ifcapi::bindings::entity_remove_deep2(instance_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_filter_all(ifcopenshell_ifc_file_t* file, const char* query, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::selector_filter_all(file_cpp, query_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_filter_elements(ifcopenshell_ifc_file_t* file, const char* query, const ifcopenshell_ifc_instance_list_t* elements, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
    if (elements == nullptr) { throw std::runtime_error("Parameter \"elements\" must not be null"); }
    auto elements_cpp = to_cpp_ifc_instance_list(elements);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::selector_filter_elements(file_cpp, query_cpp, elements_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_get_element_value(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* element, const char* query, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto file_cpp = (file != nullptr && file->ptr != nullptr) ? file->ptr : nullptr;
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::selector_get_element_value(file_cpp, element_cpp, query_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_set_element_value(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* element, const ifcopenshell_string_list_t* keys, const ifcopenshell_bool_list_t* regex_flags, ifcopenshell_ifcapi_value_t* value, const char* concat, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    auto element_cpp = (element != nullptr && element->ptr != nullptr) ? element->ptr : nullptr;
    if (keys == nullptr) { throw std::runtime_error("Parameter \"keys\" must not be null"); }
    auto keys_cpp = to_cpp_string_list(keys);
    if (regex_flags == nullptr) { throw std::runtime_error("Parameter \"regex_flags\" must not be null"); }
    auto regex_flags_cpp = to_cpp_bool_list(regex_flags);
    auto value_cpp = (value != nullptr && value->ptr != nullptr) ? value->ptr : nullptr;
    const char* concat_str = concat;
        *out_result = ifcapi::bindings::selector_set_element_value(file_cpp, element_cpp, keys_cpp, regex_flags_cpp, value_cpp, concat);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_is_x(double value, double x, double tolerance, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<double>(value);
    auto x_cpp = static_cast<double>(x);
    auto tolerance_cpp = static_cast<double>(tolerance);
        *out_result = ifcapi::bindings::shape_is_x(value_cpp, x_cpp, tolerance_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_calculate_unit_scale(ifcopenshell_ifc_file_t* file, const char* unit_type, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (unit_type == nullptr) { throw std::runtime_error("Parameter \"unit_type\" must not be null"); }
    std::string unit_type_cpp(unit_type);
        *out_result = static_cast<double>(ifcapi::bindings::unit_calculate_unit_scale(file_cpp, unit_type_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_convert(double value, const char* from_prefix, const char* from_unit, const char* to_prefix, const char* to_unit, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<double>(value);
    if (from_prefix == nullptr) { throw std::runtime_error("Parameter \"from_prefix\" must not be null"); }
    std::string from_prefix_cpp(from_prefix);
    if (from_unit == nullptr) { throw std::runtime_error("Parameter \"from_unit\" must not be null"); }
    std::string from_unit_cpp(from_unit);
    if (to_prefix == nullptr) { throw std::runtime_error("Parameter \"to_prefix\" must not be null"); }
    std::string to_prefix_cpp(to_prefix);
    if (to_unit == nullptr) { throw std::runtime_error("Parameter \"to_unit\" must not be null"); }
    std::string to_unit_cpp(to_unit);
        *out_result = static_cast<double>(ifcapi::bindings::unit_convert(value_cpp, from_prefix_cpp, from_unit_cpp, to_prefix_cpp, to_unit_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_convert_unit(double value, ifcopenshell_ifc_instance_t* from_unit, ifcopenshell_ifc_instance_t* to_unit, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<double>(value);
    if (from_unit == nullptr || from_unit->ptr == nullptr) { throw std::runtime_error("Handle parameter \"from_unit\" is invalid"); }
    auto from_unit_cpp = from_unit->ptr;
    if (to_unit == nullptr || to_unit->ptr == nullptr) { throw std::runtime_error("Handle parameter \"to_unit\" is invalid"); }
    auto to_unit_cpp = to_unit->ptr;
        *out_result = static_cast<double>(ifcapi::bindings::unit_convert_unit(value_cpp, from_unit_cpp, to_unit_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_format_length(double value, double precision, int32_t decimal_places, bool suppress_zero_inches, const char* unit_system, const char* input_unit, const char* output_unit, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<double>(value);
    auto precision_cpp = static_cast<double>(precision);
    auto decimal_places_cpp = static_cast<int>(decimal_places);
    auto suppress_zero_inches_cpp = static_cast<bool>(suppress_zero_inches);
    if (unit_system == nullptr) { throw std::runtime_error("Parameter \"unit_system\" must not be null"); }
    std::string unit_system_cpp(unit_system);
    if (input_unit == nullptr) { throw std::runtime_error("Parameter \"input_unit\" must not be null"); }
    std::string input_unit_cpp(input_unit);
    if (output_unit == nullptr) { throw std::runtime_error("Parameter \"output_unit\" must not be null"); }
    std::string output_unit_cpp(output_unit);
        *out_result = make_string(ifcapi::bindings::unit_format_length(value_cpp, precision_cpp, decimal_places_cpp, suppress_zero_inches_cpp, unit_system_cpp, input_unit_cpp, output_unit_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_full_unit_name(ifcopenshell_ifc_instance_t* unit, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (unit == nullptr || unit->ptr == nullptr) { throw std::runtime_error("Handle parameter \"unit\" is invalid"); }
    auto unit_cpp = unit->ptr;
        *out_result = make_string(ifcapi::bindings::unit_get_full_unit_name(unit_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_measure_class(const char* unit_type, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (unit_type == nullptr) { throw std::runtime_error("Parameter \"unit_type\" must not be null"); }
    std::string unit_type_cpp(unit_type);
        *out_result = make_string(ifcapi::bindings::unit_get_measure_class(unit_type_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_measure_unit_type(const char* measure_class, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (measure_class == nullptr) { throw std::runtime_error("Parameter \"measure_class\" must not be null"); }
    std::string measure_class_cpp(measure_class);
        *out_result = make_string(ifcapi::bindings::unit_get_measure_unit_type(measure_class_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_named_dimensions(const char* name, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = make_int32_list(ifcapi::bindings::unit_get_named_dimensions(name_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_prefix(const char* text, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (text == nullptr) { throw std::runtime_error("Parameter \"text\" must not be null"); }
    std::string text_cpp(text);
        *out_result = make_string(ifcapi::bindings::unit_get_prefix(text_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_prefix_multiplier(const char* text, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (text == nullptr) { throw std::runtime_error("Parameter \"text\" must not be null"); }
    std::string text_cpp(text);
        *out_result = static_cast<double>(ifcapi::bindings::unit_get_prefix_multiplier(text_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_project_unit(ifcopenshell_ifc_file_t* file, const char* unit_type, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (unit_type == nullptr) { throw std::runtime_error("Parameter \"unit_type\" must not be null"); }
    std::string unit_type_cpp(unit_type);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::unit_get_project_unit(file_cpp, unit_type_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_si_dimensions(const char* name, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = make_int32_list(ifcapi::bindings::unit_get_si_dimensions(name_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_symbol_measure_class(const char* symbol, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (symbol == nullptr) { throw std::runtime_error("Parameter \"symbol\" must not be null"); }
    std::string symbol_cpp(symbol);
        *out_result = make_string(ifcapi::bindings::unit_get_symbol_measure_class(symbol_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_symbol_quantity_class(const char* symbol, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (symbol == nullptr) { throw std::runtime_error("Parameter \"symbol\" must not be null"); }
    std::string symbol_cpp(symbol);
        *out_result = make_string(ifcapi::bindings::unit_get_symbol_quantity_class(symbol_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_unit_assignment(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::unit_get_unit_assignment(file_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_unit_name(const char* text, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (text == nullptr) { throw std::runtime_error("Parameter \"text\" must not be null"); }
    std::string text_cpp(text);
        *out_result = make_string(ifcapi::bindings::unit_get_unit_name(text_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_unit_name_universal(const char* text, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (text == nullptr) { throw std::runtime_error("Parameter \"text\" must not be null"); }
    std::string text_cpp(text);
        *out_result = make_string(ifcapi::bindings::unit_get_unit_name_universal(text_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_get_unit_symbol(ifcopenshell_ifc_instance_t* unit, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (unit == nullptr || unit->ptr == nullptr) { throw std::runtime_error("Handle parameter \"unit\" is invalid"); }
    auto unit_cpp = unit->ptr;
        *out_result = make_string(ifcapi::bindings::unit_get_unit_symbol(unit_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_measure_class(ifcopenshell_ifc_instance_t* prop, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = make_string(ifcapi::bindings::unit_resolve_property_measure_class(prop_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_table_defined_measure_class(ifcopenshell_ifc_instance_t* prop, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = make_string(ifcapi::bindings::unit_resolve_property_table_defined_measure_class(prop_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_table_defined_unit(ifcopenshell_ifc_instance_t* prop, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::unit_resolve_property_table_defined_unit(prop_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_table_defining_measure_class(ifcopenshell_ifc_instance_t* prop, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = make_string(ifcapi::bindings::unit_resolve_property_table_defining_measure_class(prop_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_table_defining_unit(ifcopenshell_ifc_instance_t* prop, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::unit_resolve_property_table_defining_unit(prop_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_unit_resolve_property_unit(ifcopenshell_ifc_instance_t* prop, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (prop == nullptr || prop->ptr == nullptr) { throw std::runtime_error("Handle parameter \"prop\" is invalid"); }
    auto prop_cpp = prop->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::unit_resolve_property_unit(prop_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_as_bool(ifcopenshell_ifcapi_value_t* value, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = ifcapi::bindings::value_as_bool(value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_as_double(ifcopenshell_ifcapi_value_t* value, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = static_cast<double>(ifcapi::bindings::value_as_double(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_as_instance(ifcopenshell_ifcapi_value_t* value, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::value_as_instance(value_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_as_int64(ifcopenshell_ifcapi_value_t* value, int64_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = static_cast<int64_t>(ifcapi::bindings::value_as_int64(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_as_string(ifcopenshell_ifcapi_value_t* value, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = make_string(ifcapi::bindings::value_as_string(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_dict_key_at(ifcopenshell_ifcapi_value_t* value, size_t index, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = make_string(ifcapi::bindings::value_dict_key_at(value_cpp, index_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_dict_size(ifcopenshell_ifcapi_value_t* value, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = static_cast<size_t>(ifcapi::bindings::value_dict_size(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_dict_value_at(ifcopenshell_ifcapi_value_t* value, size_t index, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = new ifcopenshell_ifcapi_value_t{const_cast<ifcopenshell_value_t*>(ifcapi::bindings::value_dict_value_at(value_cpp, index_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_kind(ifcopenshell_ifcapi_value_t* value, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = static_cast<int32_t>(ifcapi::bindings::value_kind(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_list_append(ifcopenshell_ifcapi_value_t* list, ifcopenshell_ifcapi_value_t* item, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (list == nullptr || list->ptr == nullptr) { throw std::runtime_error("Handle parameter \"list\" is invalid"); }
    auto list_cpp = list->ptr;
    auto item_cpp = (item != nullptr && item->ptr != nullptr) ? item->ptr : nullptr;
        *out_result = ifcapi::bindings::value_list_append(list_cpp, item_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_list_at(ifcopenshell_ifcapi_value_t* value, size_t index, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = new ifcopenshell_ifcapi_value_t{const_cast<ifcopenshell_value_t*>(ifcapi::bindings::value_list_at(value_cpp, index_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_list_size(ifcopenshell_ifcapi_value_t* value, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        *out_result = static_cast<size_t>(ifcapi::bindings::value_list_size(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_bool(bool value, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<bool>(value);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_bool(value_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_double(double value, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<double>(value);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_double(value_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_instance(ifcopenshell_ifc_instance_t* value, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = (value != nullptr && value->ptr != nullptr) ? value->ptr : nullptr;
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_instance(value_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_int(int64_t value, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto value_cpp = static_cast<long long>(value);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_int(value_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_list(ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_list(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_none(ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_none(), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_value_new_string(const char* value, ifcopenshell_ifcapi_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        *out_result = new ifcopenshell_ifcapi_value_t{ifcapi::bindings::value_new_string(value_cpp), true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_classification_get_references(ifcopenshell_ifc_instance_t* element, bool should_inherit, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    auto should_inherit_cpp = static_cast<bool>(should_inherit);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::classification_get_references(element_cpp, should_inherit_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_representation_get_context(ifcopenshell_ifc_file_t* file, const char* context_type, const char* subcontext, const char* target_view, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    const char* context_type_str = context_type;
    const char* subcontext_str = subcontext;
    const char* target_view_str = target_view;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::representation_get_context(file_cpp, context_type, subcontext, target_view), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_representation_get_prioritised_contexts(ifcopenshell_ifc_file_t* file, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::representation_get_prioritised_contexts(file_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_representation_get_product_representation(ifcopenshell_ifc_instance_t* element, ifcopenshell_ifc_instance_t* context, const char* context_type, const char* subcontext, const char* target_view, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    auto context_cpp = (context != nullptr && context->ptr != nullptr) ? context->ptr : nullptr;
    const char* context_type_str = context_type;
    const char* subcontext_str = subcontext;
    const char* target_view_str = target_view;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::representation_get_product_representation(element_cpp, context_cpp, context_type, subcontext, target_view), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_representation_resolve(ifcopenshell_ifc_instance_t* representation, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::representation_resolve(representation_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_representation_resolve_base_items(ifcopenshell_ifc_instance_t* representation, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{ifcapi::bindings::representation_resolve_base_items(representation_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_a2p(const ifcopenshell_double_list_t* origin, const ifcopenshell_double_list_t* z_axis, const ifcopenshell_double_list_t* x_axis, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (origin == nullptr) { throw std::runtime_error("Parameter \"origin\" must not be null"); }
    auto origin_cpp = to_cpp_double_list(origin);
    if (z_axis == nullptr) { throw std::runtime_error("Parameter \"z_axis\" must not be null"); }
    auto z_axis_cpp = to_cpp_double_list(z_axis);
    if (x_axis == nullptr) { throw std::runtime_error("Parameter \"x_axis\" must not be null"); }
    auto x_axis_cpp = to_cpp_double_list(x_axis);
        *out_result = make_double_list(ifcapi::bindings::placement_a2p(origin_cpp, z_axis_cpp, x_axis_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_get_axis2placement(ifcopenshell_ifc_instance_t* instance, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = make_double_list(ifcapi::bindings::placement_get_axis2placement(instance_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_get_cartesian_xform_3d(ifcopenshell_ifc_instance_t* instance, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = make_double_list(ifcapi::bindings::placement_get_cartesian_xform_3d(instance_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_get_local_placement(ifcopenshell_ifc_instance_t* instance, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto instance_cpp = (instance != nullptr && instance->ptr != nullptr) ? instance->ptr : nullptr;
        *out_result = make_double_list(ifcapi::bindings::placement_get_local_placement(instance_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_get_mappeditem_xform(ifcopenshell_ifc_instance_t* instance, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = make_double_list(ifcapi::bindings::placement_get_mappeditem_xform(instance_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_get_storey_elevation(ifcopenshell_ifc_instance_t* instance, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = static_cast<double>(ifcapi::bindings::placement_get_storey_elevation(instance_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_placement_rotation(double angle_rad, const char* axis, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto angle_rad_cpp = static_cast<double>(angle_rad);
    if (axis == nullptr) { throw std::runtime_error("Parameter \"axis\" must not be null"); }
    std::string axis_cpp(axis);
        *out_result = make_double_list(ifcapi::bindings::placement_rotation(angle_rad_cpp, axis_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_aggregate_assign_object(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* relating_object, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    if (relating_object == nullptr || relating_object->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_object\" is invalid"); }
    auto relating_object_cpp = relating_object->ptr;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::aggregate_assign_object(file_cpp, products_cpp, relating_object_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_aggregate_unassign_object(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        ifcapi::bindings::aggregate_unassign_object(file_cpp, products_cpp, user_cpp, application_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_spatial_assign_container(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* relating_structure, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    if (relating_structure == nullptr || relating_structure->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_structure\" is invalid"); }
    auto relating_structure_cpp = relating_structure->ptr;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::spatial_assign_container(file_cpp, products_cpp, relating_structure_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_spatial_unassign_container(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        ifcapi::bindings::spatial_unassign_container(file_cpp, products_cpp, user_cpp, application_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_nest_assign_object(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* objects, ifcopenshell_ifc_instance_t* relating_object, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (objects == nullptr) { throw std::runtime_error("Parameter \"objects\" must not be null"); }
    auto objects_cpp = to_cpp_ifc_instance_list(objects);
    if (relating_object == nullptr || relating_object->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_object\" is invalid"); }
    auto relating_object_cpp = relating_object->ptr;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::nest_assign_object(file_cpp, objects_cpp, relating_object_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_nest_unassign_object(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* objects, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (objects == nullptr) { throw std::runtime_error("Parameter \"objects\" must not be null"); }
    auto objects_cpp = to_cpp_ifc_instance_list(objects);
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        ifcapi::bindings::nest_unassign_object(file_cpp, objects_cpp, user_cpp, application_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_type_assign_type(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* objects, ifcopenshell_ifc_instance_t* relating_type, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (objects == nullptr) { throw std::runtime_error("Parameter \"objects\" must not be null"); }
    auto objects_cpp = to_cpp_ifc_instance_list(objects);
    if (relating_type == nullptr || relating_type->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_type\" is invalid"); }
    auto relating_type_cpp = relating_type->ptr;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::type_assign_type(file_cpp, objects_cpp, relating_type_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_type_assign_type_ex(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* objects, ifcopenshell_ifc_instance_t* relating_type, bool should_map_representations, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (objects == nullptr) { throw std::runtime_error("Parameter \"objects\" must not be null"); }
    auto objects_cpp = to_cpp_ifc_instance_list(objects);
    if (relating_type == nullptr || relating_type->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_type\" is invalid"); }
    auto relating_type_cpp = relating_type->ptr;
    auto should_map_representations_cpp = static_cast<bool>(should_map_representations);
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::type_assign_type_ex(file_cpp, objects_cpp, relating_type_cpp, should_map_representations_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_type_unassign_type(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* objects, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (objects == nullptr) { throw std::runtime_error("Parameter \"objects\" must not be null"); }
    auto objects_cpp = to_cpp_ifc_instance_list(objects);
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        ifcapi::bindings::type_unassign_type(file_cpp, objects_cpp, user_cpp, application_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_assign_representation(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* product, ifcopenshell_ifc_instance_t* representation, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (product == nullptr || product->ptr == nullptr) { throw std::runtime_error("Handle parameter \"product\" is invalid"); }
    auto product_cpp = product->ptr;
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::geometry_assign_representation(file_cpp, product_cpp, representation_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_copy_representation(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* source, ifcopenshell_ifc_instance_t* target, const char* context_identifier, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (source == nullptr || source->ptr == nullptr) { throw std::runtime_error("Handle parameter \"source\" is invalid"); }
    auto source_cpp = source->ptr;
    if (target == nullptr || target->ptr == nullptr) { throw std::runtime_error("Handle parameter \"target\" is invalid"); }
    auto target_cpp = target->ptr;
    const char* context_identifier_str = context_identifier;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::geometry_copy_representation(file_cpp, source_cpp, target_cpp, context_identifier), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_map_representation(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* representation, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::geometry_map_representation(file_cpp, representation_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_profile_extents(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* profile, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (profile == nullptr || profile->ptr == nullptr) { throw std::runtime_error("Handle parameter \"profile\" is invalid"); }
    auto profile_cpp = profile->ptr;
        *out_result = make_double_list(ifcapi::bindings::geometry_profile_extents(file_cpp, profile_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_unassign_representation(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* product, ifcopenshell_ifc_instance_t* representation) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (product == nullptr || product->ptr == nullptr) { throw std::runtime_error("Handle parameter \"product\" is invalid"); }
    auto product_cpp = product->ptr;
    if (representation == nullptr || representation->ptr == nullptr) { throw std::runtime_error("Handle parameter \"representation\" is invalid"); }
    auto representation_cpp = representation->ptr;
        ifcapi::bindings::geometry_unassign_representation(file_cpp, product_cpp, representation_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_type_map_type_representations(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* related_object, ifcopenshell_ifc_instance_t* relating_type, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (related_object == nullptr || related_object->ptr == nullptr) { throw std::runtime_error("Handle parameter \"related_object\" is invalid"); }
    auto related_object_cpp = related_object->ptr;
    if (relating_type == nullptr || relating_type->ptr == nullptr) { throw std::runtime_error("Handle parameter \"relating_type\" is invalid"); }
    auto relating_type_cpp = relating_type->ptr;
        *out_result = ifcapi::bindings::type_map_type_representations(file_cpp, related_object_cpp, relating_type_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_geometry_edit_object_placement(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* product, const ifcopenshell_double_list_t* matrix, bool is_si, bool should_transform_children, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (product == nullptr || product->ptr == nullptr) { throw std::runtime_error("Handle parameter \"product\" is invalid"); }
    auto product_cpp = product->ptr;
    if (matrix == nullptr) { throw std::runtime_error("Parameter \"matrix\" must not be null"); }
    auto matrix_cpp = to_cpp_double_list(matrix);
    auto is_si_cpp = static_cast<bool>(is_si);
    auto should_transform_children_cpp = static_cast<bool>(should_transform_children);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::geometry_edit_object_placement(file_cpp, product_cpp, matrix_cpp, is_si_cpp, should_transform_children_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_axis2_placement_2d(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* position, const ifcopenshell_double_list_t* x_direction, bool has_x_direction, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
    if (x_direction == nullptr) { throw std::runtime_error("Parameter \"x_direction\" must not be null"); }
    auto x_direction_cpp = to_cpp_double_list(x_direction);
    auto has_x_direction_cpp = static_cast<bool>(has_x_direction);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_axis2_placement_2d(file_cpp, position_cpp, x_direction_cpp, has_x_direction_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_axis2_placement_3d(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* position, const ifcopenshell_double_list_t* z_axis, const ifcopenshell_double_list_t* x_axis, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
    if (z_axis == nullptr) { throw std::runtime_error("Parameter \"z_axis\" must not be null"); }
    auto z_axis_cpp = to_cpp_double_list(z_axis);
    if (x_axis == nullptr) { throw std::runtime_error("Parameter \"x_axis\" must not be null"); }
    auto x_axis_cpp = to_cpp_double_list(x_axis);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_axis2_placement_3d(file_cpp, position_cpp, z_axis_cpp, x_axis_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_block(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* position, double x_length, double y_length, double z_length, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
    auto x_length_cpp = static_cast<double>(x_length);
    auto y_length_cpp = static_cast<double>(y_length);
    auto z_length_cpp = static_cast<double>(z_length);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_block(file_cpp, position_cpp, x_length_cpp, y_length_cpp, z_length_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_circle(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* center, double radius, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (center == nullptr) { throw std::runtime_error("Parameter \"center\" must not be null"); }
    auto center_cpp = to_cpp_double_list(center);
    auto radius_cpp = static_cast<double>(radius);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_circle(file_cpp, center_cpp, radius_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_curve_between_two_points(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_curve_between_two_points(file_cpp, points_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_deep_copy(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* element, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_deep_copy(file_cpp, element_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_edge(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* start, const ifcopenshell_double_list_t* end, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (start == nullptr) { throw std::runtime_error("Parameter \"start\" must not be null"); }
    auto start_cpp = to_cpp_double_list(start);
    if (end == nullptr) { throw std::runtime_error("Parameter \"end\" must not be null"); }
    auto end_cpp = to_cpp_double_list(end);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_edge(file_cpp, start_cpp, end_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_ellipse_curve(ifcopenshell_ifc_file_t* file, double x_axis_radius, double y_axis_radius, const ifcopenshell_double_list_t* position, const ifcopenshell_double_list_list_t* trim_points, const ifcopenshell_double_list_t* ref_x_direction, const ifcopenshell_int32_list_t* trim_points_mask, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    auto x_axis_radius_cpp = static_cast<double>(x_axis_radius);
    auto y_axis_radius_cpp = static_cast<double>(y_axis_radius);
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
    if (trim_points == nullptr) { throw std::runtime_error("Parameter \"trim_points\" must not be null"); }
    auto trim_points_cpp = to_cpp_double_list_list(trim_points);
    if (ref_x_direction == nullptr) { throw std::runtime_error("Parameter \"ref_x_direction\" must not be null"); }
    auto ref_x_direction_cpp = to_cpp_double_list(ref_x_direction);
    if (trim_points_mask == nullptr) { throw std::runtime_error("Parameter \"trim_points_mask\" must not be null"); }
    auto trim_points_mask_cpp = to_cpp_int32_list(trim_points_mask);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_ellipse_curve(file_cpp, x_axis_radius_cpp, y_axis_radius_cpp, position_cpp, trim_points_cpp, ref_x_direction_cpp, trim_points_mask_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_extrude(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* profile_or_curve, double magnitude, const ifcopenshell_double_list_t* position, const ifcopenshell_double_list_t* extrusion_vector, const ifcopenshell_double_list_t* position_z_axis, const ifcopenshell_double_list_t* position_x_axis, const ifcopenshell_double_list_t* position_y_axis, bool has_position_y_axis, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (profile_or_curve == nullptr || profile_or_curve->ptr == nullptr) { throw std::runtime_error("Handle parameter \"profile_or_curve\" is invalid"); }
    auto profile_or_curve_cpp = profile_or_curve->ptr;
    auto magnitude_cpp = static_cast<double>(magnitude);
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
    if (extrusion_vector == nullptr) { throw std::runtime_error("Parameter \"extrusion_vector\" must not be null"); }
    auto extrusion_vector_cpp = to_cpp_double_list(extrusion_vector);
    if (position_z_axis == nullptr) { throw std::runtime_error("Parameter \"position_z_axis\" must not be null"); }
    auto position_z_axis_cpp = to_cpp_double_list(position_z_axis);
    if (position_x_axis == nullptr) { throw std::runtime_error("Parameter \"position_x_axis\" must not be null"); }
    auto position_x_axis_cpp = to_cpp_double_list(position_x_axis);
    if (position_y_axis == nullptr) { throw std::runtime_error("Parameter \"position_y_axis\" must not be null"); }
    auto position_y_axis_cpp = to_cpp_double_list(position_y_axis);
    auto has_position_y_axis_cpp = static_cast<bool>(has_position_y_axis);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_extrude(file_cpp, profile_or_curve_cpp, magnitude_cpp, position_cpp, extrusion_vector_cpp, position_z_axis_cpp, position_x_axis_cpp, position_y_axis_cpp, has_position_y_axis_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_face(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_face(file_cpp, points_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_faceted_brep(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    if (faces == nullptr) { throw std::runtime_error("Parameter \"faces\" must not be null"); }
    auto faces_cpp = to_cpp_int32_list_list(faces);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_faceted_brep(file_cpp, points_cpp, faces_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_get_polyline_coords(ifcopenshell_ifc_instance_t* polyline, ifcopenshell_double_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (polyline == nullptr || polyline->ptr == nullptr) { throw std::runtime_error("Handle parameter \"polyline\" is invalid"); }
    auto polyline_cpp = polyline->ptr;
        *out_result = make_double_list_list(ifcapi::bindings::shape_builder_get_polyline_coords(polyline_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_half_space_solid(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* plane, bool agreement_flag, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (plane == nullptr || plane->ptr == nullptr) { throw std::runtime_error("Handle parameter \"plane\" is invalid"); }
    auto plane_cpp = plane->ptr;
    auto agreement_flag_cpp = static_cast<bool>(agreement_flag);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_half_space_solid(file_cpp, plane_cpp, agreement_flag_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_indexed_polycurve_2d(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* segments, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    if (segments == nullptr) { throw std::runtime_error("Parameter \"segments\" must not be null"); }
    auto segments_cpp = to_cpp_int32_list_list(segments);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_indexed_polycurve_2d(file_cpp, points_cpp, segments_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mep_bend_shape(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* segment, double start_length, double end_length, double angle, double radius, const ifcopenshell_double_list_t* bend_vector, bool flip_z_axis, ifcopenshell_shape_builder_mep_bend_shape_result_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (segment == nullptr || segment->ptr == nullptr) { throw std::runtime_error("Handle parameter \"segment\" is invalid"); }
    auto segment_cpp = segment->ptr;
    auto start_length_cpp = static_cast<double>(start_length);
    auto end_length_cpp = static_cast<double>(end_length);
    auto angle_cpp = static_cast<double>(angle);
    auto radius_cpp = static_cast<double>(radius);
    if (bend_vector == nullptr) { throw std::runtime_error("Parameter \"bend_vector\" must not be null"); }
    auto bend_vector_cpp = to_cpp_double_list(bend_vector);
    auto flip_z_axis_cpp = static_cast<bool>(flip_z_axis);
        auto result_value = ifcapi::bindings::shape_builder_mep_bend_shape(file_cpp, segment_cpp, start_length_cpp, end_length_cpp, angle_cpp, radius_cpp, bend_vector_cpp, flip_z_axis_cpp);
        out_result->representation = new ifcopenshell_ifc_instance_t{result_value.representation, false};
        out_result->start_length = static_cast<double>(result_value.start_length);
        out_result->end_length = static_cast<double>(result_value.end_length);
        out_result->radius = static_cast<double>(result_value.radius);
        out_result->angle = static_cast<double>(result_value.angle);
        out_result->lateral_axis = static_cast<int32_t>(result_value.lateral_axis);
        out_result->lateral_sign = static_cast<double>(result_value.lateral_sign);
        out_result->z_axis_sign = static_cast<int32_t>(result_value.z_axis_sign);
        out_result->main_profile_dimension = static_cast<double>(result_value.main_profile_dimension);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mep_transition_calculate(const ifcopenshell_double_list_t* start_half_dim, const ifcopenshell_double_list_t* end_half_dim, const ifcopenshell_double_list_t* offset, const ifcopenshell_double_list_t* diff, bool has_diff, bool end_profile, double length, bool has_length, double angle, bool has_angle, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (start_half_dim == nullptr) { throw std::runtime_error("Parameter \"start_half_dim\" must not be null"); }
    auto start_half_dim_cpp = to_cpp_double_list(start_half_dim);
    if (end_half_dim == nullptr) { throw std::runtime_error("Parameter \"end_half_dim\" must not be null"); }
    auto end_half_dim_cpp = to_cpp_double_list(end_half_dim);
    if (offset == nullptr) { throw std::runtime_error("Parameter \"offset\" must not be null"); }
    auto offset_cpp = to_cpp_double_list(offset);
    if (diff == nullptr) { throw std::runtime_error("Parameter \"diff\" must not be null"); }
    auto diff_cpp = to_cpp_double_list(diff);
    auto has_diff_cpp = static_cast<bool>(has_diff);
    auto end_profile_cpp = static_cast<bool>(end_profile);
    auto length_cpp = static_cast<double>(length);
    auto has_length_cpp = static_cast<bool>(has_length);
    auto angle_cpp = static_cast<double>(angle);
    auto has_angle_cpp = static_cast<bool>(has_angle);
        *out_result = static_cast<double>(ifcapi::bindings::shape_builder_mep_transition_calculate(start_half_dim_cpp, end_half_dim_cpp, offset_cpp, diff_cpp, has_diff_cpp, end_profile_cpp, length_cpp, has_length_cpp, angle_cpp, has_angle_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mep_transition_length(const ifcopenshell_double_list_t* start_half_dim, const ifcopenshell_double_list_t* end_half_dim, double angle, const ifcopenshell_double_list_t* profile_offset, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (start_half_dim == nullptr) { throw std::runtime_error("Parameter \"start_half_dim\" must not be null"); }
    auto start_half_dim_cpp = to_cpp_double_list(start_half_dim);
    if (end_half_dim == nullptr) { throw std::runtime_error("Parameter \"end_half_dim\" must not be null"); }
    auto end_half_dim_cpp = to_cpp_double_list(end_half_dim);
    auto angle_cpp = static_cast<double>(angle);
    if (profile_offset == nullptr) { throw std::runtime_error("Parameter \"profile_offset\" must not be null"); }
    auto profile_offset_cpp = to_cpp_double_list(profile_offset);
        *out_result = static_cast<double>(ifcapi::bindings::shape_builder_mep_transition_length(start_half_dim_cpp, end_half_dim_cpp, angle_cpp, profile_offset_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mep_transition_shape(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* start_segment, ifcopenshell_ifc_instance_t* end_segment, double start_length, double end_length, double angle, const ifcopenshell_double_list_t* profile_offset, ifcopenshell_shape_builder_mep_transition_shape_result_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (start_segment == nullptr || start_segment->ptr == nullptr) { throw std::runtime_error("Handle parameter \"start_segment\" is invalid"); }
    auto start_segment_cpp = start_segment->ptr;
    if (end_segment == nullptr || end_segment->ptr == nullptr) { throw std::runtime_error("Handle parameter \"end_segment\" is invalid"); }
    auto end_segment_cpp = end_segment->ptr;
    auto start_length_cpp = static_cast<double>(start_length);
    auto end_length_cpp = static_cast<double>(end_length);
    auto angle_cpp = static_cast<double>(angle);
    if (profile_offset == nullptr) { throw std::runtime_error("Parameter \"profile_offset\" must not be null"); }
    auto profile_offset_cpp = to_cpp_double_list(profile_offset);
        auto result_value = ifcapi::bindings::shape_builder_mep_transition_shape(file_cpp, start_segment_cpp, end_segment_cpp, start_length_cpp, end_length_cpp, angle_cpp, profile_offset_cpp);
        out_result->representation = new ifcopenshell_ifc_instance_t{result_value.representation, false};
        out_result->has_result = static_cast<bool>(result_value.has_result);
        out_result->start_length = static_cast<double>(result_value.start_length);
        out_result->end_length = static_cast<double>(result_value.end_length);
        out_result->angle = static_cast<double>(result_value.angle);
        out_result->profile_offset = make_double_list(result_value.profile_offset);
        out_result->transition_length = static_cast<double>(result_value.transition_length);
        out_result->full_transition_length = static_cast<double>(result_value.full_transition_length);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mesh(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    if (faces == nullptr) { throw std::runtime_error("Parameter \"faces\" must not be null"); }
    auto faces_cpp = to_cpp_int32_list_list(faces);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_mesh(file_cpp, points_cpp, faces_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_mirror(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* item, const ifcopenshell_double_list_t* mirror_axes, const ifcopenshell_double_list_t* mirror_point, bool create_copy, const ifcopenshell_double_list_t* placement_matrix, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
    if (mirror_axes == nullptr) { throw std::runtime_error("Parameter \"mirror_axes\" must not be null"); }
    auto mirror_axes_cpp = to_cpp_double_list(mirror_axes);
    if (mirror_point == nullptr) { throw std::runtime_error("Parameter \"mirror_point\" must not be null"); }
    auto mirror_point_cpp = to_cpp_double_list(mirror_point);
    auto create_copy_cpp = static_cast<bool>(create_copy);
    if (placement_matrix == nullptr) { throw std::runtime_error("Parameter \"placement_matrix\" must not be null"); }
    auto placement_matrix_cpp = to_cpp_double_list(placement_matrix);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_mirror(file_cpp, item_cpp, mirror_axes_cpp, mirror_point_cpp, create_copy_cpp, placement_matrix_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_plane(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* location, const ifcopenshell_double_list_t* normal, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (location == nullptr) { throw std::runtime_error("Parameter \"location\" must not be null"); }
    auto location_cpp = to_cpp_double_list(location);
    if (normal == nullptr) { throw std::runtime_error("Parameter \"normal\" must not be null"); }
    auto normal_cpp = to_cpp_double_list(normal);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_plane(file_cpp, location_cpp, normal_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_polygonal_face_set(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_list_t* faces, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    if (faces == nullptr) { throw std::runtime_error("Parameter \"faces\" must not be null"); }
    auto faces_cpp = to_cpp_int32_list_list_list(faces);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_polygonal_face_set(file_cpp, points_cpp, faces_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_polyline(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, bool closed, const ifcopenshell_double_list_t* position_offset, bool has_position_offset, const ifcopenshell_int32_list_t* arc_points, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    auto closed_cpp = static_cast<bool>(closed);
    if (position_offset == nullptr) { throw std::runtime_error("Parameter \"position_offset\" must not be null"); }
    auto position_offset_cpp = to_cpp_double_list(position_offset);
    auto has_position_offset_cpp = static_cast<bool>(has_position_offset);
    if (arc_points == nullptr) { throw std::runtime_error("Parameter \"arc_points\" must not be null"); }
    auto arc_points_cpp = to_cpp_int32_list(arc_points);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_polyline(file_cpp, points_cpp, closed_cpp, position_offset_cpp, has_position_offset_cpp, arc_points_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_profile(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* outer_curve, const char* name, const ifcopenshell_ifc_instance_list_t* inner_curves, const char* profile_type, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (outer_curve == nullptr || outer_curve->ptr == nullptr) { throw std::runtime_error("Handle parameter \"outer_curve\" is invalid"); }
    auto outer_curve_cpp = outer_curve->ptr;
    const char* name_str = name;
    if (inner_curves == nullptr) { throw std::runtime_error("Parameter \"inner_curves\" must not be null"); }
    auto inner_curves_cpp = to_cpp_ifc_instance_list(inner_curves);
    const char* profile_type_str = profile_type;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_profile(file_cpp, outer_curve_cpp, name, inner_curves_cpp, profile_type), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_representation(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* context, const ifcopenshell_ifc_instance_list_t* items, const char* representation_type, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (context == nullptr || context->ptr == nullptr) { throw std::runtime_error("Handle parameter \"context\" is invalid"); }
    auto context_cpp = context->ptr;
    if (items == nullptr) { throw std::runtime_error("Parameter \"items\" must not be null"); }
    auto items_cpp = to_cpp_ifc_instance_list(items);
    const char* representation_type_str = representation_type;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_representation(file_cpp, context_cpp, items_cpp, representation_type), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_rotate(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* item, double angle, const ifcopenshell_double_list_t* pivot_point, bool counter_clockwise, bool create_copy, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
    auto angle_cpp = static_cast<double>(angle);
    if (pivot_point == nullptr) { throw std::runtime_error("Parameter \"pivot_point\" must not be null"); }
    auto pivot_point_cpp = to_cpp_double_list(pivot_point);
    auto counter_clockwise_cpp = static_cast<bool>(counter_clockwise);
    auto create_copy_cpp = static_cast<bool>(create_copy);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_rotate(file_cpp, item_cpp, angle_cpp, pivot_point_cpp, counter_clockwise_cpp, create_copy_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_set_polyline_coords(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* polyline, const ifcopenshell_double_list_list_t* coords, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (polyline == nullptr || polyline->ptr == nullptr) { throw std::runtime_error("Handle parameter \"polyline\" is invalid"); }
    auto polyline_cpp = polyline->ptr;
    if (coords == nullptr) { throw std::runtime_error("Parameter \"coords\" must not be null"); }
    auto coords_cpp = to_cpp_double_list_list(coords);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_set_polyline_coords(file_cpp, polyline_cpp, coords_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_sphere(ifcopenshell_ifc_file_t* file, double radius, const ifcopenshell_double_list_t* center, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    auto radius_cpp = static_cast<double>(radius);
    if (center == nullptr) { throw std::runtime_error("Parameter \"center\" must not be null"); }
    auto center_cpp = to_cpp_double_list(center);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_sphere(file_cpp, radius_cpp, center_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_swept_disk_solid(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* path_curve, double radius, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (path_curve == nullptr || path_curve->ptr == nullptr) { throw std::runtime_error("Handle parameter \"path_curve\" is invalid"); }
    auto path_curve_cpp = path_curve->ptr;
    auto radius_cpp = static_cast<double>(radius);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_swept_disk_solid(file_cpp, path_curve_cpp, radius_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_translate(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* item, const ifcopenshell_double_list_t* translation, bool create_copy, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
    if (translation == nullptr) { throw std::runtime_error("Parameter \"translation\" must not be null"); }
    auto translation_cpp = to_cpp_double_list(translation);
    auto create_copy_cpp = static_cast<bool>(create_copy);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_translate(file_cpp, item_cpp, translation_cpp, create_copy_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_triangulated_face_set(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (points == nullptr) { throw std::runtime_error("Parameter \"points\" must not be null"); }
    auto points_cpp = to_cpp_double_list_list(points);
    if (faces == nullptr) { throw std::runtime_error("Parameter \"faces\" must not be null"); }
    auto faces_cpp = to_cpp_int32_list_list(faces);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_triangulated_face_set(file_cpp, points_cpp, faces_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_shape_builder_vertex(ifcopenshell_ifc_file_t* file, const ifcopenshell_double_list_t* position, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (position == nullptr) { throw std::runtime_error("Parameter \"position\" must not be null"); }
    auto position_cpp = to_cpp_double_list(position);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::shape_builder_vertex(file_cpp, position_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_add_pset(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* product, const char* name, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, const char* ifc2x3_subclass, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (product == nullptr || product->ptr == nullptr) { throw std::runtime_error("Handle parameter \"product\" is invalid"); }
    auto product_cpp = product->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
    const char* ifc2x3_subclass_str = ifc2x3_subclass;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::pset_add_pset(file_cpp, product_cpp, name_cpp, owner_history_cpp, user_cpp, application_cpp, ifc2x3_subclass), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_add_qto(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* product, const char* name, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (product == nullptr || product->ptr == nullptr) { throw std::runtime_error("Handle parameter \"product\" is invalid"); }
    auto product_cpp = product->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::pset_add_qto(file_cpp, product_cpp, name_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_edit_pset(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* pset, const char* name, void* properties, ifcopenshell_ifc_instance_t* pset_template, bool should_purge, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (pset == nullptr || pset->ptr == nullptr) { throw std::runtime_error("Handle parameter \"pset\" is invalid"); }
    auto pset_cpp = pset->ptr;
    const char* name_str = name;
    if (properties == nullptr) { throw std::runtime_error("Parameter \"properties\" must not be null"); }
    auto properties_cpp = static_cast<ifcopenshell_pset_props_t*>(properties);
    auto pset_template_cpp = (pset_template != nullptr && pset_template->ptr != nullptr) ? pset_template->ptr : nullptr;
    auto should_purge_cpp = static_cast<bool>(should_purge);
        *out_result = ifcapi::bindings::pset_edit_pset(file_cpp, pset_cpp, name, properties_cpp, pset_template_cpp, should_purge_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_edit_qto(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* qto, const char* name, void* properties, ifcopenshell_ifc_instance_t* qto_template, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (qto == nullptr || qto->ptr == nullptr) { throw std::runtime_error("Handle parameter \"qto\" is invalid"); }
    auto qto_cpp = qto->ptr;
    const char* name_str = name;
    if (properties == nullptr) { throw std::runtime_error("Parameter \"properties\" must not be null"); }
    auto properties_cpp = static_cast<ifcopenshell_pset_props_t*>(properties);
    auto qto_template_cpp = (qto_template != nullptr && qto_template->ptr != nullptr) ? qto_template->ptr : nullptr;
        *out_result = ifcapi::bindings::pset_edit_qto(file_cpp, qto_cpp, name, properties_cpp, qto_template_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_free(void* props) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
        ifcapi::bindings::pset_props_free(props_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_new(void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = static_cast<void*>(ifcapi::bindings::pset_props_new());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_bool(void* props, const char* key, bool value) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<bool>(value);
        ifcapi::bindings::pset_props_set_bool(props_cpp, key_cpp, value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_dict(void* outer, const char* key, void* inner) {
    try {
        ifcopenshell_clear_error();
    if (outer == nullptr) { throw std::runtime_error("Parameter \"outer\" must not be null"); }
    auto outer_cpp = static_cast<ifcopenshell_pset_props_t*>(outer);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (inner == nullptr) { throw std::runtime_error("Parameter \"inner\" must not be null"); }
    auto inner_cpp = static_cast<ifcopenshell_pset_props_t*>(inner);
        ifcapi::bindings::pset_props_set_dict(outer_cpp, key_cpp, inner_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_double(void* props, const char* key, double value) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<double>(value);
        ifcapi::bindings::pset_props_set_double(props_cpp, key_cpp, value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_double_list(void* props, const char* key, const ifcopenshell_double_list_t* values) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (values == nullptr) { throw std::runtime_error("Parameter \"values\" must not be null"); }
    auto values_cpp = to_cpp_double_list(values);
        ifcapi::bindings::pset_props_set_double_list(props_cpp, key_cpp, values_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_instance(void* props, const char* key, ifcopenshell_ifc_instance_t* value) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = (value != nullptr && value->ptr != nullptr) ? value->ptr : nullptr;
        ifcapi::bindings::pset_props_set_instance(props_cpp, key_cpp, value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_int(void* props, const char* key, int64_t value) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<long long>(value);
        ifcapi::bindings::pset_props_set_int(props_cpp, key_cpp, value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_int_list(void* props, const char* key, const ifcopenshell_int64_list_t* values) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (values == nullptr) { throw std::runtime_error("Parameter \"values\" must not be null"); }
    auto values_cpp = to_cpp_int64_list(values);
        ifcapi::bindings::pset_props_set_int_list(props_cpp, key_cpp, values_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_null(void* props, const char* key) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
        ifcapi::bindings::pset_props_set_null(props_cpp, key_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_string(void* props, const char* key, const char* value) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        ifcapi::bindings::pset_props_set_string(props_cpp, key_cpp, value_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_string_list(void* props, const char* key, const ifcopenshell_string_list_t* values) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (values == nullptr) { throw std::runtime_error("Parameter \"values\" must not be null"); }
    auto values_cpp = to_cpp_string_list(values);
        ifcapi::bindings::pset_props_set_string_list(props_cpp, key_cpp, values_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_typed_bool(void* props, const char* key, bool value, const char* ifc_type) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<bool>(value);
    if (ifc_type == nullptr) { throw std::runtime_error("Parameter \"ifc_type\" must not be null"); }
    std::string ifc_type_cpp(ifc_type);
        ifcapi::bindings::pset_props_set_typed_bool(props_cpp, key_cpp, value_cpp, ifc_type_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_typed_double(void* props, const char* key, double value, const char* ifc_type) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<double>(value);
    if (ifc_type == nullptr) { throw std::runtime_error("Parameter \"ifc_type\" must not be null"); }
    std::string ifc_type_cpp(ifc_type);
        ifcapi::bindings::pset_props_set_typed_double(props_cpp, key_cpp, value_cpp, ifc_type_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_typed_int(void* props, const char* key, int64_t value, const char* ifc_type) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    auto value_cpp = static_cast<long long>(value);
    if (ifc_type == nullptr) { throw std::runtime_error("Parameter \"ifc_type\" must not be null"); }
    std::string ifc_type_cpp(ifc_type);
        ifcapi::bindings::pset_props_set_typed_int(props_cpp, key_cpp, value_cpp, ifc_type_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_typed_string(void* props, const char* key, const char* value, const char* ifc_type) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
    if (ifc_type == nullptr) { throw std::runtime_error("Parameter \"ifc_type\" must not be null"); }
    std::string ifc_type_cpp(ifc_type);
        ifcapi::bindings::pset_props_set_typed_string(props_cpp, key_cpp, value_cpp, ifc_type_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_props_set_unit_for_last(void* props, ifcopenshell_ifc_instance_t* unit) {
    try {
        ifcopenshell_clear_error();
    if (props == nullptr) { throw std::runtime_error("Parameter \"props\" must not be null"); }
    auto props_cpp = static_cast<ifcopenshell_pset_props_t*>(props);
    auto unit_cpp = (unit != nullptr && unit->ptr != nullptr) ? unit->ptr : nullptr;
        ifcapi::bindings::pset_props_set_unit_for_last(props_cpp, unit_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_get_applicable(void* pqt, const char* ifc_class, const char* predefined_type, bool pset_only, bool qto_only, const char* schema_name, ifcopenshell_ifc_instance_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (pqt == nullptr) { throw std::runtime_error("Parameter \"pqt\" must not be null"); }
    auto pqt_cpp = static_cast<ifcopenshell_pset_template_t*>(pqt);
    const char* ifc_class_str = ifc_class;
    const char* predefined_type_str = predefined_type;
    auto pset_only_cpp = static_cast<bool>(pset_only);
    auto qto_only_cpp = static_cast<bool>(qto_only);
    const char* schema_name_str = schema_name;
        *out_result = make_ifc_instance_list(ifcapi::bindings::pset_template_get_applicable(pqt_cpp, ifc_class, predefined_type, pset_only_cpp, qto_only_cpp, schema_name));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_get_applicable_names(void* pqt, const char* ifc_class, const char* predefined_type, bool pset_only, bool qto_only, const char* schema_name, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (pqt == nullptr) { throw std::runtime_error("Parameter \"pqt\" must not be null"); }
    auto pqt_cpp = static_cast<ifcopenshell_pset_template_t*>(pqt);
    const char* ifc_class_str = ifc_class;
    const char* predefined_type_str = predefined_type;
    auto pset_only_cpp = static_cast<bool>(pset_only);
    auto qto_only_cpp = static_cast<bool>(qto_only);
    const char* schema_name_str = schema_name;
        *out_result = make_string_list(ifcapi::bindings::pset_template_get_applicable_names(pqt_cpp, ifc_class, predefined_type, pset_only_cpp, qto_only_cpp, schema_name));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_get_by_name(void* pqt, const char* name, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (pqt == nullptr) { throw std::runtime_error("Parameter \"pqt\" must not be null"); }
    auto pqt_cpp = static_cast<ifcopenshell_pset_template_t*>(pqt);
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::pset_template_get_by_name(pqt_cpp, name_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_get_template(const char* schema_identifier, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (schema_identifier == nullptr) { throw std::runtime_error("Parameter \"schema_identifier\" must not be null"); }
    std::string schema_identifier_cpp(schema_identifier);
        *out_result = static_cast<void*>(ifcapi::bindings::pset_template_get_template(schema_identifier_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_is_templated(void* pqt, const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (pqt == nullptr) { throw std::runtime_error("Parameter \"pqt\" must not be null"); }
    auto pqt_cpp = static_cast<ifcopenshell_pset_template_t*>(pqt);
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = ifcapi::bindings::pset_template_is_templated(pqt_cpp, name_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_pset_type(ifcopenshell_ifc_instance_t* pset_template, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (pset_template == nullptr || pset_template->ptr == nullptr) { throw std::runtime_error("Handle parameter \"pset_template\" is invalid"); }
    auto pset_template_cpp = pset_template->ptr;
        *out_result = make_string(ifcapi::bindings::pset_template_pset_type(pset_template_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_pset_template_set_template_dir(const char* dir) {
    try {
        ifcopenshell_clear_error();
    if (dir == nullptr) { throw std::runtime_error("Parameter \"dir\" must not be null"); }
    std::string dir_cpp(dir);
        ifcapi::bindings::pset_template_set_template_dir(dir_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_node_child(void* node, size_t index, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (node == nullptr) { throw std::runtime_error("Parameter \"node\" must not be null"); }
    auto node_cpp = static_cast<ifcopenshell_selector_node_t*>(node);
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = static_cast<void*>(ifcapi::bindings::selector_node_child(node_cpp, index_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_node_child_count(void* node, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (node == nullptr) { throw std::runtime_error("Parameter \"node\" must not be null"); }
    auto node_cpp = static_cast<ifcopenshell_selector_node_t*>(node);
        *out_result = static_cast<size_t>(ifcapi::bindings::selector_node_child_count(node_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_node_free(void* root) {
    try {
        ifcopenshell_clear_error();
    if (root == nullptr) { throw std::runtime_error("Parameter \"root\" must not be null"); }
    auto root_cpp = static_cast<ifcopenshell_selector_node_t*>(root);
        ifcapi::bindings::selector_node_free(root_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_node_kind(void* node, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (node == nullptr) { throw std::runtime_error("Parameter \"node\" must not be null"); }
    auto node_cpp = static_cast<ifcopenshell_selector_node_t*>(node);
        *out_result = static_cast<int32_t>(ifcapi::bindings::selector_node_kind(node_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_node_text(void* node, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (node == nullptr) { throw std::runtime_error("Parameter \"node\" must not be null"); }
    auto node_cpp = static_cast<ifcopenshell_selector_node_t*>(node);
        *out_result = make_string(ifcapi::bindings::selector_node_text(node_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_parse_filter(const char* query, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = static_cast<void*>(ifcapi::bindings::selector_parse_filter(query_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_parse_format(const char* query, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = static_cast<void*>(ifcapi::bindings::selector_parse_format(query_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_parse_get_element(const char* query, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = static_cast<void*>(ifcapi::bindings::selector_parse_get_element(query_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_format(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* instance, const char* query, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto file_cpp = (file != nullptr && file->ptr != nullptr) ? file->ptr : nullptr;
    auto instance_cpp = (instance != nullptr && instance->ptr != nullptr) ? instance->ptr : nullptr;
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = make_string(ifcapi::bindings::selector_format(file_cpp, instance_cpp, query_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_keys_count(void* keys, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (keys == nullptr) { throw std::runtime_error("Parameter \"keys\" must not be null"); }
    auto keys_cpp = static_cast<ifcopenshell_selector_keys_t*>(keys);
        *out_result = static_cast<size_t>(ifcapi::bindings::selector_keys_count(keys_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_keys_free(void* keys) {
    try {
        ifcopenshell_clear_error();
    if (keys == nullptr) { throw std::runtime_error("Parameter \"keys\" must not be null"); }
    auto keys_cpp = static_cast<ifcopenshell_selector_keys_t*>(keys);
        ifcapi::bindings::selector_keys_free(keys_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_keys_get(void* keys, size_t index, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (keys == nullptr) { throw std::runtime_error("Parameter \"keys\" must not be null"); }
    auto keys_cpp = static_cast<ifcopenshell_selector_keys_t*>(keys);
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = make_string(ifcapi::bindings::selector_keys_get(keys_cpp, index_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_keys_is_regex(void* keys, size_t index, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (keys == nullptr) { throw std::runtime_error("Parameter \"keys\" must not be null"); }
    auto keys_cpp = static_cast<ifcopenshell_selector_keys_t*>(keys);
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = ifcapi::bindings::selector_keys_is_regex(keys_cpp, index_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_selector_parse_keys(const char* query, void** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (query == nullptr) { throw std::runtime_error("Parameter \"query\" must not be null"); }
    std::string query_cpp(query);
        *out_result = static_cast<void*>(ifcapi::bindings::selector_parse_keys(query_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_root_create_entity(ifcopenshell_ifc_file_t* file, const char* ifc_class, const char* predefined_type, const char* name, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (ifc_class == nullptr) { throw std::runtime_error("Parameter \"ifc_class\" must not be null"); }
    std::string ifc_class_cpp(ifc_class);
    const char* predefined_type_str = predefined_type;
    const char* name_str = name;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::root_create_entity(file_cpp, ifc_class_cpp, predefined_type, name, owner_history_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_owner_create_owner_history(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::owner_create_owner_history(file_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_owner_update_owner_history(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* element, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    auto element_cpp = (element != nullptr && element->ptr != nullptr) ? element->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::owner_update_owner_history(file_cpp, element_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_group_add_group(ifcopenshell_ifc_file_t* file, const char* name, const char* description, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    const char* description_str = description;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::group_add_group(file_cpp, name_cpp, description, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_group_assign_group(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* group, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    if (group == nullptr || group->ptr == nullptr) { throw std::runtime_error("Handle parameter \"group\" is invalid"); }
    auto group_cpp = group->ptr;
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::group_assign_group(file_cpp, products_cpp, group_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_group_unassign_group(ifcopenshell_ifc_file_t* file, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* group, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application) {
    try {
        ifcopenshell_clear_error();
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    if (group == nullptr || group->ptr == nullptr) { throw std::runtime_error("Handle parameter \"group\" is invalid"); }
    auto group_cpp = group->ptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        ifcapi::bindings::group_unassign_group(file_cpp, products_cpp, group_cpp, user_cpp, application_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_group_update_group_products(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* group, const ifcopenshell_ifc_instance_list_t* products, ifcopenshell_ifc_instance_t* owner_history, ifcopenshell_ifc_instance_t* user, ifcopenshell_ifc_instance_t* application, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (file == nullptr || file->ptr == nullptr) { throw std::runtime_error("Handle parameter \"file\" is invalid"); }
    auto file_cpp = file->ptr;
    if (group == nullptr || group->ptr == nullptr) { throw std::runtime_error("Handle parameter \"group\" is invalid"); }
    auto group_cpp = group->ptr;
    if (products == nullptr) { throw std::runtime_error("Parameter \"products\" must not be null"); }
    auto products_cpp = to_cpp_ifc_instance_list(products);
    auto owner_history_cpp = (owner_history != nullptr && owner_history->ptr != nullptr) ? owner_history->ptr : nullptr;
    auto user_cpp = (user != nullptr && user->ptr != nullptr) ? user->ptr : nullptr;
    auto application_cpp = (application != nullptr && application->ptr != nullptr) ? application->ptr : nullptr;
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::group_update_group_products(file_cpp, group_cpp, products_cpp, owner_history_cpp, user_cpp, application_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_schema_reassign_class(ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* element, const char* new_class, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    auto file_cpp = (file != nullptr && file->ptr != nullptr) ? file->ptr : nullptr;
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
    if (new_class == nullptr) { throw std::runtime_error("Parameter \"new_class\" must not be null"); }
    std::string new_class_cpp(new_class);
        *out_result = new ifcopenshell_ifc_instance_t{ifcapi::bindings::schema_reassign_class(file_cpp, element_cpp, new_class_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_guid_new(ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
        *out_result = make_string(ifcapi::guid_new());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_guid_compress(const char* uuid_hex, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (uuid_hex == nullptr) { throw std::runtime_error("Parameter \"uuid_hex\" must not be null"); }
    std::string uuid_hex_cpp(uuid_hex);
        *out_result = make_string(ifcapi::guid_compress(uuid_hex_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcapi_guid_expand(const char* guid, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (guid == nullptr) { throw std::runtime_error("Parameter \"guid\" must not be null"); }
    std::string guid_cpp(guid);
        *out_result = make_string(ifcapi::guid_expand(guid_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_create(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t* decl, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (decl == nullptr || decl->ptr == nullptr) { throw std::runtime_error("Handle parameter \"decl\" is invalid"); }
    auto decl_cpp = decl->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->create(decl_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_inverses_by_declaration(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_ifc_declaration_t* type, int32_t attribute_index, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto instance_id_cpp = static_cast<int>(instance_id);
    if (type == nullptr || type->ptr == nullptr) { throw std::runtime_error("Handle parameter \"type\" is invalid"); }
    auto type_cpp = type->ptr;
    auto attribute_index_cpp = static_cast<int>(attribute_index);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->getInverse(instance_id_cpp, type_cpp, attribute_index_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_by_type(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type == nullptr) { throw std::runtime_error("Parameter \"type\" must not be null"); }
    std::string type_cpp(type);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->instances_by_type(type_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_by_type_excl_subtypes(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type == nullptr) { throw std::runtime_error("Parameter \"type\" must not be null"); }
    std::string type_cpp(type);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->instances_by_type_excl_subtypes(type_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_fresh_id(ifcopenshell_ifc_file_t* self, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<uint32_t>(self_cpp->FreshId());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_add_entities(ifcopenshell_ifc_file_t* self, ifcopenshell_ifcparse_instance_list_t* entities) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (entities == nullptr) { throw std::runtime_error("Handle parameter \"entities\" must not be null"); }
    auto entities_cpp = entities->value;
        self_cpp->addEntities(entities_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_add(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity, int32_t id, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (entity == nullptr || entity->ptr == nullptr) { throw std::runtime_error("Handle parameter \"entity\" is invalid"); }
    auto entity_cpp = entity->ptr;
    auto id_cpp = static_cast<int>(id);
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->addEntity(entity_cpp, id_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_add_type_ref(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* new_entity) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (new_entity == nullptr || new_entity->ptr == nullptr) { throw std::runtime_error("Handle parameter \"new_entity\" is invalid"); }
    auto new_entity_cpp = new_entity->ptr;
        self_cpp->add_type_ref(new_entity_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_batch(ifcopenshell_ifc_file_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->batch();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_build_inverses(ifcopenshell_ifc_file_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->build_inverses();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_build_inverses_(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* arg_0) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        self_cpp->build_inverses_(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_bypass_type(ifcopenshell_ifc_file_t* self, const char* type_name) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type_name == nullptr) { throw std::runtime_error("Parameter \"type_name\" must not be null"); }
    std::string type_name_cpp(type_name);
        self_cpp->bypass_type(type_name_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_create_timestamp(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->createTimestamp());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_max_id(ifcopenshell_ifc_file_t* self, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<uint32_t>(self_cpp->getMaxId());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_total_inverses_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto instance_id_cpp = static_cast<int>(instance_id);
        *out_result = static_cast<size_t>(self_cpp->getTotalInverses(instance_id_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_inverse_indices_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto instance_id_cpp = static_cast<int>(instance_id);
        *out_result = make_int32_list(self_cpp->get_inverse_indices(instance_id_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_ifcroot_type(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(self_cpp->ifcroot_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_initialize(ifcopenshell_ifc_file_t* self, const char* path, int32_t ty, bool readonly, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
    auto ty_cpp = static_cast<IfcParse::filetype>(ty);
    auto readonly_cpp = static_cast<bool>(readonly);
        *out_result = self_cpp->initialize(path_cpp, ty_cpp, readonly_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_by_guid(ifcopenshell_ifc_file_t* self, const char* guid, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (guid == nullptr) { throw std::runtime_error("Parameter \"guid\" must not be null"); }
    std::string guid_cpp(guid);
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->instance_by_guid(guid_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto id_cpp = static_cast<int>(id);
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->instance_by_id(id_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_instances_by_reference(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto id_cpp = static_cast<int>(id);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->instances_by_reference(id_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_process_deletion_inverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* inst) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (inst == nullptr || inst->ptr == nullptr) { throw std::runtime_error("Handle parameter \"inst\" is invalid"); }
    auto inst_cpp = inst->ptr;
        self_cpp->process_deletion_inverse(inst_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_recalculate_id_counter(ifcopenshell_ifc_file_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->recalculate_id_counter();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_remove(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (entity == nullptr || entity->ptr == nullptr) { throw std::runtime_error("Handle parameter \"entity\" is invalid"); }
    auto entity_cpp = entity->ptr;
        self_cpp->removeEntity(entity_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_remove_type_ref(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* new_entity) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (new_entity == nullptr || new_entity->ptr == nullptr) { throw std::runtime_error("Handle parameter \"new_entity\" is invalid"); }
    auto new_entity_cpp = new_entity->ptr;
        self_cpp->remove_type_ref(new_entity_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_reset_identity_cache(ifcopenshell_ifc_file_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->reset_identity_cache();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_schema_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_schema_t{const_cast<IfcParse::schema_definition*>(self_cpp->schema()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_traverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto max_level_cpp = static_cast<int>(max_level);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->traverse(instance_cpp, max_level_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_traverse_breadth_first(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
    auto max_level_cpp = static_cast<int>(max_level);
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->traverse_breadth_first(instance_cpp, max_level_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_unbatch(ifcopenshell_ifc_file_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->unbatch();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_declaration(ifcopenshell_ifc_instance_t* self, ifcopenshell_ifc_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(&(self_cpp->declaration())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_argument(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_attribute_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = new ifcopenshell_ifcparse_attribute_value_t{self_cpp->get_attribute_value(index_cpp)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_id(ifcopenshell_ifc_instance_t* self, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<uint32_t>(self_cpp->id());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_identity(ifcopenshell_ifc_instance_t* self, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<uint32_t>(self_cpp->identity());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_declaration_by_name(ifcopenshell_ifc_schema_t* self, const char* name, ifcopenshell_ifc_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(self_cpp->declaration_by_name(name_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_declaration_by_index(ifcopenshell_ifc_schema_t* self, size_t name, ifcopenshell_ifc_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto name_cpp = static_cast<unsigned long>(name);
        *out_result = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(self_cpp->declaration_by_name(name_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_declaration_list(self_cpp->declarations());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_entities(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_entity_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_entity_list(self_cpp->entities());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_enumeration_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_enumeration_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_enumeration_list(self_cpp->enumeration_types());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_name(ifcopenshell_ifc_schema_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_select_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_select_type_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_select_type_list(self_cpp->select_types());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_type_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_type_declaration_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_type_declaration_list(self_cpp->type_declarations());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_is_a(ifcopenshell_ifc_declaration_t* self, const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = self_cpp->is(name_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_as_entity(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(self_cpp->as_entity()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_as_enumeration_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_enumeration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_enumeration_t{const_cast<IfcParse::enumeration_type*>(self_cpp->as_enumeration_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_as_select_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_select_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_select_type_t{const_cast<IfcParse::select_type*>(self_cpp->as_select_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_as_type_declaration(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_type_declaration_t{const_cast<IfcParse::type_declaration*>(self_cpp->as_type_declaration()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_index_in_schema(ifcopenshell_ifc_declaration_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->index_in_schema());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_name(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_name_uc(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name_uc());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_schema(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_schema_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_schema_t{const_cast<IfcParse::schema_definition*>(self_cpp->schema()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_declaration_type(ifcopenshell_ifc_declaration_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->type());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_type_declaration_as_type_declaration(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_type_declaration_t{const_cast<IfcParse::type_declaration*>(self_cpp->as_type_declaration()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_type_declaration_declared_type(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_parameter_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_parameter_type_t{const_cast<IfcParse::parameter_type*>(self_cpp->declared_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_select_type_as_select_type(ifcopenshell_ifc_select_type_t* self, ifcopenshell_ifc_select_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_select_type_t{const_cast<IfcParse::select_type*>(self_cpp->as_select_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_select_type_select_list(ifcopenshell_ifc_select_type_t* self, ifcopenshell_ifc_declaration_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_declaration_list(self_cpp->select_list());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_enumeration_as_enumeration_type(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_ifc_enumeration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_enumeration_t{const_cast<IfcParse::enumeration_type*>(self_cpp->as_enumeration_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_enumeration_enumeration_items(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->enumeration_items());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_enumeration_lookup_enum_offset(ifcopenshell_ifc_enumeration_t* self, const char* string, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (string == nullptr) { throw std::runtime_error("Parameter \"string\" must not be null"); }
    std::string string_cpp(string);
        *out_result = static_cast<size_t>(self_cpp->lookup_enum_offset(string_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_enumeration_lookup_enum_value(ifcopenshell_ifc_enumeration_t* self, size_t i, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto i_cpp = static_cast<unsigned long>(i);
        *out_result = make_string(self_cpp->lookup_enum_value(i_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_parameter_type_as_aggregation_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_aggregation_type_t{const_cast<IfcParse::aggregation_type*>(self_cpp->as_aggregation_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_parameter_type_as_named_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_named_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_named_type_t{const_cast<IfcParse::named_type*>(self_cpp->as_named_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_parameter_type_as_simple_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_simple_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_simple_type_t{const_cast<IfcParse::simple_type*>(self_cpp->as_simple_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_named_type_is_a(ifcopenshell_ifc_named_type_t* self, const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = self_cpp->is(name_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_named_type_as_named_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_named_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_named_type_t{const_cast<IfcParse::named_type*>(self_cpp->as_named_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_named_type_declared_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_declaration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_declaration_t{self_cpp->declared_type(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_simple_type_as_simple_type(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_ifc_simple_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_simple_type_t{const_cast<IfcParse::simple_type*>(self_cpp->as_simple_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_simple_type_declared_type(ifcopenshell_ifc_simple_type_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->declared_type());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_as_aggregation_type(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_aggregation_type_t{const_cast<IfcParse::aggregation_type*>(self_cpp->as_aggregation_type()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_bound1(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->bound1());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_bound2(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->bound2());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_type_of_element(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_parameter_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_parameter_type_t{self_cpp->type_of_element(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_file(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_file_t{self_cpp->file(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_file_description(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_description_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_file_description_t{const_cast<Header_section_schema::file_description*>(self_cpp->file_description()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_file_name(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_name_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_file_name_t{const_cast<Header_section_schema::file_name*>(self_cpp->file_name()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_file_schema(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_schema_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_file_schema_t{const_cast<Header_section_schema::file_schema*>(self_cpp->file_schema()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_read(ifcopenshell_ifc_header_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->read();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_try_read(ifcopenshell_ifc_header_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->tryRead();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_class(ifcopenshell_ifc_file_description_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->Class())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_declaration(ifcopenshell_ifc_file_description_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->declaration())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_description(ifcopenshell_ifc_file_description_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->description());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_implementation_level(ifcopenshell_ifc_file_description_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->implementation_level());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_setdescription(ifcopenshell_ifc_file_description_t* self, const ifcopenshell_string_list_t* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    auto v_cpp = to_cpp_string_list(v);
        self_cpp->setdescription(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_description_setimplementation_level(ifcopenshell_ifc_file_description_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->setimplementation_level(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_class(ifcopenshell_ifc_file_name_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->Class())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_author(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->author());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_authorization(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->authorization());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_declaration(ifcopenshell_ifc_file_name_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->declaration())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_name(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_organization(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->organization());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_originating_system(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->originating_system());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_preprocessor_version(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->preprocessor_version());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setauthor(ifcopenshell_ifc_file_name_t* self, const ifcopenshell_string_list_t* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    auto v_cpp = to_cpp_string_list(v);
        self_cpp->setauthor(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setauthorization(ifcopenshell_ifc_file_name_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->setauthorization(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setname(ifcopenshell_ifc_file_name_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->setname(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setorganization(ifcopenshell_ifc_file_name_t* self, const ifcopenshell_string_list_t* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    auto v_cpp = to_cpp_string_list(v);
        self_cpp->setorganization(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setoriginating_system(ifcopenshell_ifc_file_name_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->setoriginating_system(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_setpreprocessor_version(ifcopenshell_ifc_file_name_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->setpreprocessor_version(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_settime_stamp(ifcopenshell_ifc_file_name_t* self, const char* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    std::string v_cpp(v);
        self_cpp->settime_stamp(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_name_time_stamp(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->time_stamp());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema_class(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->Class())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema_declaration(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(&(self_cpp->declaration())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema_schema_identifiers(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->schema_identifiers());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema_setschema_identifiers(ifcopenshell_ifc_file_schema_t* self, const ifcopenshell_string_list_t* v) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (v == nullptr) { throw std::runtime_error("Parameter \"v\" must not be null"); }
    auto v_cpp = to_cpp_string_list(v);
        self_cpp->setschema_identifiers(v_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_attribute_index(ifcopenshell_ifc_entity_t* self, const char* attr_name, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (attr_name == nullptr) { throw std::runtime_error("Parameter \"attr_name\" must not be null"); }
    std::string attr_name_cpp(attr_name);
        *out_result = static_cast<int32_t>(self_cpp->attribute_index(attr_name_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_all_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_attribute_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_attribute_list(self_cpp->all_attributes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_all_inverse_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_inverse_attribute_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_inverse_attribute_list(self_cpp->all_inverse_attributes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_as_entity(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(self_cpp->as_entity()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_attribute_by_index(ifcopenshell_ifc_entity_t* self, size_t index, ifcopenshell_ifc_attribute_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto index_cpp = static_cast<unsigned long>(index);
        *out_result = new ifcopenshell_ifc_attribute_t{const_cast<IfcParse::attribute*>(self_cpp->attribute_by_index(index_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_attribute_count(ifcopenshell_ifc_entity_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<size_t>(self_cpp->attribute_count());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_attribute_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_attribute_list(self_cpp->attributes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_derived(ifcopenshell_ifc_entity_t* self, ifcopenshell_bool_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_bool_list(self_cpp->derived());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_is_abstract(ifcopenshell_ifc_entity_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->is_abstract();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_set_attributes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_attribute_list_t* attributes, const ifcopenshell_bool_list_t* derived) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (attributes == nullptr) { throw std::runtime_error("Parameter \"attributes\" must not be null"); }
    auto attributes_cpp = to_cpp_ifc_attribute_list(attributes);
    if (derived == nullptr) { throw std::runtime_error("Parameter \"derived\" must not be null"); }
    auto derived_cpp = to_cpp_bool_list(derived);
        self_cpp->set_attributes(attributes_cpp, derived_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_set_inverse_attributes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_inverse_attribute_list_t* inverse_attributes) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (inverse_attributes == nullptr) { throw std::runtime_error("Parameter \"inverse_attributes\" must not be null"); }
    auto inverse_attributes_cpp = to_cpp_ifc_inverse_attribute_list(inverse_attributes);
        self_cpp->set_inverse_attributes(inverse_attributes_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_set_subtypes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_entity_list_t* subtypes) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (subtypes == nullptr) { throw std::runtime_error("Parameter \"subtypes\" must not be null"); }
    auto subtypes_cpp = to_cpp_ifc_entity_list(subtypes);
        self_cpp->set_subtypes(subtypes_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_subtypes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifc_entity_list(self_cpp->subtypes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_supertype(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(self_cpp->supertype()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_attribute_name(ifcopenshell_ifc_attribute_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_attribute_optional(ifcopenshell_ifc_attribute_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->optional();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_attribute_type_of_attribute(ifcopenshell_ifc_attribute_t* self, ifcopenshell_ifc_parameter_type_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_parameter_type_t{const_cast<IfcParse::parameter_type*>(self_cpp->type_of_attribute()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_attribute_reference(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_ifc_attribute_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_attribute_t{const_cast<IfcParse::attribute*>(self_cpp->attribute_reference()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_bound1(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->bound1());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_bound2(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->bound2());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_entity_reference(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_ifc_entity_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(self_cpp->entity_reference()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_name(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_bypassed_instances(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_uint32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_uint32_list(self_cpp->bypassed_instances());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_has_semicolon(ifcopenshell_ifc_instance_streamer_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->hasSemicolon();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_push_page(ifcopenshell_ifc_instance_streamer_t* self, const char* page) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (page == nullptr) { throw std::runtime_error("Parameter \"page\" must not be null"); }
    std::string page_cpp(page);
        self_cpp->pushPage(page_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_semicolon_count(ifcopenshell_ifc_instance_streamer_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<size_t>(self_cpp->semicolonCount());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_instance_list_size(ifcopenshell_ifcparse_instance_list_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
        *out_result = static_cast<size_t>(self_cpp->size());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_write(ifcopenshell_ifc_file_t* self, const char* path) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        [&]() {
std::ofstream stream(IfcUtil::path::from_utf8(path_cpp).c_str());
if (!stream.good()) {
    throw std::runtime_error("Failed to write to path: '" + path_cpp + "', check folder and file permissions.");
}
stream << (*self_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_storage_mode(ifcopenshell_ifc_file_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return std::visit([](auto& storage) -> int {
    using T = std::decay_t<decltype(storage)>;
    if constexpr (std::is_same_v<T, IfcParse::impl::in_memory_file_storage>) {
        return 0;
    } else if constexpr (std::is_same_v<T, IfcParse::impl::rocks_db_file_storage>) {
        return 1;
    }
    return -1;
}, self_cpp->storage_);
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_create_entity_by_name(ifcopenshell_ifc_file_t* self, const char* type_name, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type_name == nullptr) { throw std::runtime_error("Parameter \"type_name\" must not be null"); }
    std::string type_name_cpp(type_name);
        auto generated_result = [&]() {
const auto* schema = self_cpp->schema();
const auto* decl = schema->declaration_by_name(type_name_cpp);
if (!decl || (!decl->as_entity() && !decl->as_type_declaration() && !decl->as_enumeration_type())) {
    throw std::runtime_error("Declaration is not creatable");
}
auto* entity = self_cpp->create(decl);
if (!entity) {
    throw std::runtime_error("Failed to create entity");
}
return entity;
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_create_entity_by_name_with_id(ifcopenshell_ifc_file_t* self, const char* type_name, uint32_t id, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type_name == nullptr) { throw std::runtime_error("Parameter \"type_name\" must not be null"); }
    std::string type_name_cpp(type_name);
        auto generated_result = [&]() {
const auto* schema = self_cpp->schema();
const auto* decl = schema->declaration_by_name(type_name_cpp);
if (!decl || !decl->as_entity()) {
    throw std::runtime_error("Type declaration is not an entity");
}
auto* inst = schema->instantiate(
    decl, in_memory_attribute_storage(decl->as_entity()->attribute_count()));
inst->file_ = nullptr;
auto* entity = self_cpp->addEntity(inst, static_cast<int>(id));
if (!entity) {
    throw std::runtime_error("Failed to create entity with id");
}
return entity;
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_add_entity(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, uint32_t id, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
auto* added = self_cpp->addEntity(instance_cpp, id == 0 ? -1 : static_cast<int>(id));
if (!added) {
    throw std::runtime_error("Failed to add entity");
}
return added;
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_schema_name(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (self_cpp->schema() == nullptr) {
    return std::string();
}
return self_cpp->schema()->name();
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_inverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
if (auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>()) {
    return self_cpp->getInverse(entity->id(), 0, -1);
}
throw IfcParse::IfcException("Only entities with ids are supported for get_inverse.");
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_total_inverses(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
if (auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>()) {
    return self_cpp->getTotalInverses(entity->id());
}
throw IfcParse::IfcException("Only entities with ids are supported for get_total_inverses.");
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_types(ifcopenshell_ifc_file_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
const size_t n = std::distance(self_cpp->types_begin(), self_cpp->types_end());
std::vector<std::string> types;
types.reserve(n);
std::transform(self_cpp->types_begin(), self_cpp->types_end(), std::back_inserter(types), [](const IfcParse::declaration* decl) {
    return decl->name();
});
return types;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_to_string(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::ostringstream stream;
stream << (*self_cpp);
return stream.str();
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_entity_names(ifcopenshell_ifc_file_t* self, ifcopenshell_uint32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<unsigned int> ids;
ids.reserve(std::distance(self_cpp->begin(), self_cpp->end()));
for (auto it = self_cpp->begin(); it != self_cpp->end(); ++it) {
    ids.push_back(it->first);
}
return ids;
        }();
        *out_result = make_uint32_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_file_pointer(ifcopenshell_ifc_file_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return reinterpret_cast<size_t>(self_cpp);
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_inverse_indices(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
if (auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>()) {
    return self_cpp->get_inverse_indices(entity->id());
}
throw IfcParse::IfcException("Only entities with ids are supported for get_inverse_indices.");
        }();
        *out_result = make_int32_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_good(ifcopenshell_ifc_file_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp->good().value());
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_get_unit(ifcopenshell_ifc_file_t* self, const char* unit_type, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (unit_type == nullptr) { throw std::runtime_error("Parameter \"unit_type\" must not be null"); }
    std::string unit_type_cpp(unit_type);
        auto generated_result = [&]() {
return self_cpp->getUnit(unit_type_cpp).second;
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_key_value_store_query(ifcopenshell_ifc_file_t* self, const char* key, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (key == nullptr) { throw std::runtime_error("Parameter \"key\" must not be null"); }
    std::string key_cpp(key);
        auto generated_result = [&]() {
auto* storage = std::visit([](auto& value) -> const IfcParse::impl::rocks_db_file_storage* {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, IfcParse::impl::rocks_db_file_storage>) {
        return &value;
    }
    return nullptr;
}, self_cpp->storage_);
if (!storage) {
    return std::string();
}
#ifdef IFOPSH_WITH_ROCKSDB
std::string value;
if (storage->db->Get(storage->ropts, key_cpp, &value) != rocksdb::Status::OK()) {
    return std::string();
}
return value;
#else
return std::string();
#endif
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_key_value_store_iter(ifcopenshell_ifc_file_t* self, const char* prefix, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (prefix == nullptr) { throw std::runtime_error("Parameter \"prefix\" must not be null"); }
    std::string prefix_cpp(prefix);
        auto generated_result = [&]() {
std::vector<std::string> values;
auto* storage = std::visit([](auto& value) -> const IfcParse::impl::rocks_db_file_storage* {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, IfcParse::impl::rocks_db_file_storage>) {
        return &value;
    }
    return nullptr;
}, self_cpp->storage_);
if (!storage) {
    return values;
}
#ifdef IFOPSH_WITH_ROCKSDB
std::unique_ptr<rocksdb::Iterator> iterator(storage->db->NewIterator(storage->ropts));
const rocksdb::Slice prefix_slice(prefix_cpp);
for (iterator->Seek(prefix_cpp); iterator->Valid() && iterator->key().starts_with(prefix_slice); iterator->Next()) {
    values.push_back(iterator->key().ToString());
}
#endif
return values;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_file_pointer(ifcopenshell_ifc_instance_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return reinterpret_cast<size_t>(self_cpp->file_);
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_data(ifcopenshell_ifc_instance_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return reinterpret_cast<size_t>(&self_cpp->data());
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_argument_index(ifcopenshell_ifc_instance_t* self, const char* name, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
if (self_cpp->declaration().as_entity()) {
    return static_cast<unsigned int>(self_cpp->declaration().as_entity()->attribute_index(name_cpp));
}
if (name_cpp == "wrappedValue") {
    return 0u;
}
throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
        }();
        *out_result = static_cast<uint32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_argument_name(ifcopenshell_ifc_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (self_cpp->declaration().as_entity()) {
    return self_cpp->declaration().as_entity()->attribute_by_index(index)->name();
}
if (index == 0u) {
    return std::string("wrappedValue");
}
throw IfcParse::IfcException(std::to_string(index) + " out of bounds on " + self_cpp->declaration().name());
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_attribute_category(ifcopenshell_ifc_instance_t* self, const char* name, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
if (!self_cpp->declaration().as_entity()) {
    return name_cpp == "wrappedValue" ? 1 : 0;
}
for (const auto* attr : self_cpp->declaration().as_entity()->all_attributes()) {
    if (attr->name() == name_cpp) {
        return 1;
    }
}
for (const auto* attr : self_cpp->declaration().as_entity()->all_inverse_attributes()) {
    if (attr->name() == name_cpp) {
        return 2;
    }
}
return 0;
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_attribute_value(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_attribute_value_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Handle parameter \"value\" must not be null"); }
    auto value_cpp = value->value;
        [&]() {
if (!self_cpp->declaration().as_entity()) {
    if (name_cpp != "wrappedValue") {
        throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
    }
    set_instance_attribute_from_attribute_value(self_cpp, 0u, value_cpp);
    return;
}
const auto index = self_cpp->declaration().as_entity()->attribute_index(name_cpp);
if (index < 0) {
    throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
}
set_instance_attribute_from_attribute_value(self_cpp, static_cast<size_t>(index), value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_unset_attribute_value(ifcopenshell_ifc_instance_t* self, const char* name) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        [&]() {
if (!self_cpp->declaration().as_entity()) {
    if (name_cpp != "wrappedValue") {
        throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
    }
    unset_instance_argument(self_cpp, 0u);
    return;
}
const auto index = self_cpp->declaration().as_entity()->attribute_index(name_cpp);
if (index < 0) {
    throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
}
unset_instance_argument(self_cpp, static_cast<size_t>(index));
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_inverse(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
if (self_cpp->declaration().as_entity()) {
    return static_cast<IfcUtil::IfcBaseEntity*>(self_cpp)->get_inverse(name_cpp);
}
throw IfcParse::IfcException(name_cpp + " not found on " + self_cpp->declaration().name());
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_attribute_value(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_attribute_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->get_attribute_value(index);
        }();
        *out_result = new ifcopenshell_ifcparse_attribute_value_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_argument_by_name(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_attribute_value_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
auto* entity = self_cpp->declaration().as_entity();
if (!entity) {
    throw std::runtime_error("Attribute '" + name_cpp + "' not found on entity named " + self_cpp->declaration().name());
}
auto index = entity->attribute_index(name_cpp);
if (index == -1) {
    throw std::runtime_error("Attribute '" + name_cpp + "' not found on entity named " + self_cpp->declaration().name());
}
return self_cpp->get_attribute_value(static_cast<unsigned>(index));
        }();
        *out_result = new ifcopenshell_ifcparse_attribute_value_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_argument_type(ifcopenshell_ifc_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return IfcUtil::ArgumentTypeToString(helper_fn_attribute_type(self_cpp, index));
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_to_string(ifcopenshell_ifc_instance_t* self, bool valid_spf, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::ostringstream oss;
self_cpp->toString(oss, valid_spf);
return oss.str();
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_class_name(ifcopenshell_ifc_instance_t* self, bool with_schema, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
auto name = self_cpp->declaration().name();
if (with_schema) {
    name = self_cpp->declaration().schema()->name() + "." + name;
}
return name;
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_is_a(ifcopenshell_ifc_instance_t* self, const char* declaration_name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (declaration_name == nullptr) { throw std::runtime_error("Parameter \"declaration_name\" must not be null"); }
    std::string declaration_name_cpp(declaration_name);
        auto generated_result = [&]() {
return self_cpp->declaration().is(declaration_name_cpp);
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_attribute_names(ifcopenshell_ifc_instance_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (!self_cpp->declaration().as_entity()) {
    return std::vector<std::string>(1, "wrappedValue");
}
const auto attrs = self_cpp->declaration().as_entity()->all_attributes();
std::vector<std::string> names;
names.reserve(attrs.size());
for (const auto* attr : attrs) {
    names.push_back(attr->name());
}
return names;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_inverse_attribute_names(ifcopenshell_ifc_instance_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (!self_cpp->declaration().as_entity()) {
    return std::vector<std::string>();
}
const auto attrs = self_cpp->declaration().as_entity()->all_inverse_attributes();
std::vector<std::string> names;
names.reserve(attrs.size());
for (const auto* attr : attrs) {
    names.push_back(attr->name());
}
return names;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_get_inverse_attribute_by_name(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto generated_result = [&]() {
if (auto* entity = self_cpp->as<IfcUtil::IfcBaseEntity>()) {
    return entity->get_inverse(name_cpp);
}
throw IfcParse::IfcException("Only entities with ids are supported for inverse attributes.");
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_unset_argument(ifcopenshell_ifc_instance_t* self, size_t index) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
unset_instance_argument(self_cpp, index);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_bool(ifcopenshell_ifc_instance_t* self, size_t index, bool value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
set_instance_argument(self_cpp, index, value);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_int32(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
set_instance_argument(self_cpp, index, static_cast<int>(value));
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_double(ifcopenshell_ifc_instance_t* self, size_t index, double value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
set_instance_argument(self_cpp, index, value);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_string(ifcopenshell_ifc_instance_t* self, size_t index, const char* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_instance(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_instance_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_instance_list(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_instance_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Handle parameter \"value\" must not be null"); }
    auto value_cpp = value->value;
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_int32_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_int32_list(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_double_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_double_list(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_string_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_string_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_string_list(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_int32_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_int32_list_list(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_double_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_double_list_list(value);
        [&]() {
set_instance_argument(self_cpp, index, value_cpp);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_logical(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
IfcUtil::ArgumentType arg_type = helper_fn_attribute_type(self_cpp, static_cast<unsigned>(index));
if (arg_type != IfcUtil::Argument_LOGICAL) {
    throw IfcParse::IfcException("Attribute not set");
}
boost::logic::tribool logical_value;
if (value == 0) {
    logical_value = false;
} else if (value == 1) {
    logical_value = true;
} else if (value == -1) {
    logical_value = boost::logic::indeterminate;
} else {
    throw IfcParse::IfcException("Logical value must be -1, 0, or 1");
}
set_instance_argument(self_cpp, index, logical_value);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_int32_list_list(value);
        [&]() {
IfcUtil::ArgumentType arg_type = helper_fn_attribute_type(self_cpp, static_cast<unsigned>(index));
if (arg_type != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
    throw IfcParse::IfcException("Attribute not set");
}
if (self_cpp->file_ == nullptr) {
    throw IfcParse::IfcException("Instance is not attached to a file.");
}
aggregate_of_aggregate_of_instance::ptr aggregate(new aggregate_of_aggregate_of_instance());
for (const auto& group : value_cpp) {
    std::vector<IfcUtil::IfcBaseClass*> instances;
    instances.reserve(group.size());
    for (int identifier : group) {
        auto* instance = self_cpp->file_->instance_by_id(identifier);
        if (instance == nullptr) {
            throw IfcParse::IfcException("Unable to resolve instance id " + std::to_string(identifier));
        }
        instances.push_back(instance);
    }
    aggregate->push(instances);
}
set_instance_argument(self_cpp, index, aggregate);
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_enumeration(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_enumeration_t* enumeration, size_t enumeration_index) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (enumeration == nullptr || enumeration->ptr == nullptr) { throw std::runtime_error("Handle parameter \"enumeration\" is invalid"); }
    auto enumeration_cpp = enumeration->ptr;
        [&]() {
set_instance_argument(self_cpp, index, EnumerationReference(enumeration_cpp, enumeration_index));
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_set_argument_enumeration_by_name(ifcopenshell_ifc_instance_t* self, size_t index, const char* value, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        auto generated_result = [&]() {
auto* entity_decl = self_cpp->declaration().as_entity();
if (entity_decl == nullptr) {
    return false;
}
const auto attrs = entity_decl->all_attributes();
if (index >= attrs.size()) {
    return false;
}
const IfcParse::parameter_type* parameter_type = attrs[index]->type_of_attribute();
const IfcParse::enumeration_type* enumeration = nullptr;
while (parameter_type != nullptr) {
    auto* named = parameter_type->as_named_type();
    if (named == nullptr) {
        break;
    }
    auto* declaration = named->declared_type();
    if ((enumeration = declaration->as_enumeration_type()) != nullptr) {
        break;
    }
    auto* type_declaration = declaration->as_type_declaration();
    if (type_declaration == nullptr) {
        break;
    }
    parameter_type = type_declaration->declared_type();
}
if (enumeration == nullptr) {
    return false;
}
const auto& items = enumeration->enumeration_items();
auto it = std::find(items.begin(), items.end(), value_cpp);
if (it == items.end()) {
    throw IfcParse::IfcException("'" + value_cpp + "' is not a valid value for enumeration " + enumeration->name());
}
set_instance_argument(self_cpp, index, EnumerationReference(enumeration, static_cast<size_t>(std::distance(items.begin(), it))));
return true;
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_is_null(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return self_cpp.isNull();
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return IfcUtil::ArgumentTypeToString(self_cpp.type());
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_size(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return self_cpp.size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_int32(ifcopenshell_ifcparse_attribute_value_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp);
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_bool(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<bool>(self_cpp);
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_double(ifcopenshell_ifcparse_attribute_value_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<double>(self_cpp);
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_int32_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::vector<int>>(self_cpp);
        }();
        *out_result = make_int32_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_double_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::vector<double>>(self_cpp);
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_int32_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::vector<std::vector<int>>>(self_cpp);
        }();
        *out_result = make_int32_list_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_instance_id_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
if (self_cpp.isNull() || self_cpp.type() != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
    throw IfcParse::IfcException("Attribute is not an aggregate of aggregate of entity instance");
}
auto aggregate = static_cast<aggregate_of_aggregate_of_instance::ptr>(self_cpp);
std::vector<std::vector<int>> result;
result.reserve(aggregate->size());
for (const auto& group : *aggregate) {
    std::vector<int> row;
    row.reserve(group.size());
    for (const auto* instance : group) {
        if (instance == nullptr) {
            throw IfcParse::IfcException("Aggregate contains a null entity instance");
        }
        row.push_back(static_cast<int>(instance->id()));
    }
    result.push_back(std::move(row));
}
return result;
        }();
        *out_result = make_int32_list_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_double_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::vector<std::vector<double>>>(self_cpp);
        }();
        *out_result = make_double_list_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_string_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::vector<std::string>>(self_cpp);
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_string(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<std::string>(self_cpp);
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_instance(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<IfcUtil::IfcBaseClass*>(self_cpp);
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_instance_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<boost::shared_ptr<aggregate_of_instance>>(self_cpp);
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_enumeration_value(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return std::string(static_cast<EnumerationReference>(self_cpp).value());
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_enumeration_index(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return static_cast<EnumerationReference>(self_cpp).index();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_attribute_value_as_enumeration_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_enumeration_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto& self_cpp = self->value;
        auto generated_result = [&]() {
return const_cast<IfcParse::enumeration_type*>(static_cast<EnumerationReference>(self_cpp).enumeration());
        }();
        *out_result = new ifcopenshell_ifc_enumeration_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_header(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_header_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return &self_cpp->header();
        }();
        *out_result = new ifcopenshell_ifc_header_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_header_file_description(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return const_cast<IfcUtil::IfcBaseClass*>(
    static_cast<const IfcUtil::IfcBaseClass*>(self_cpp->header().file_description()));
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_header_file_name(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return const_cast<IfcUtil::IfcBaseClass*>(
    static_cast<const IfcUtil::IfcBaseClass*>(self_cpp->header().file_name()));
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_header_file_schema(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return const_cast<IfcUtil::IfcBaseClass*>(
    static_cast<const IfcUtil::IfcBaseClass*>(self_cpp->header().file_schema()));
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_header_write(ifcopenshell_ifc_header_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::ostringstream stream;
self_cpp->write(stream);
return stream.str();
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_schema_instantiate(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_t* declaration, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (declaration == nullptr || declaration->ptr == nullptr) { throw std::runtime_error("Handle parameter \"declaration\" is invalid"); }
    auto declaration_cpp = declaration->ptr;
        auto generated_result = [&]() {
IfcEntityInstanceData data(in_memory_attribute_storage(declaration_cpp->as_entity() ? declaration_cpp->as_entity()->attribute_count() : 1));
auto* instance = self_cpp->instantiate(declaration_cpp, std::move(data));
if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {
    entity->populate_derived();
}
return instance;
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_select_type_select_list_names(ifcopenshell_ifc_select_type_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<std::string> names;
names.reserve(self_cpp->select_list().size());
for (const auto* decl : self_cpp->select_list()) {
    names.push_back(decl->name());
}
return names;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_type_declaration_argument_types(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<std::string> result;
auto argument_type = IfcUtil::Argument_UNKNOWN;
auto* declared_type = self_cpp->declared_type();
if (declared_type != nullptr) {
    argument_type = IfcUtil::from_parameter_type(declared_type);
}
result.push_back(IfcUtil::ArgumentTypeToString(argument_type));
return result;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_enumeration_argument_types(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return std::vector<std::string>{IfcUtil::ArgumentTypeToString(IfcUtil::Argument_STRING)};
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_parameter_type_kind(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (self_cpp->as_named_type()) {
    return "NAMED";
}
if (self_cpp->as_simple_type()) {
    return "SIMPLE";
}
if (self_cpp->as_aggregation_type()) {
    return "AGGREGATION";
}
throw std::runtime_error("Unknown parameter type.");
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_simple_type_kind(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
switch (self_cpp->declared_type()) {
case IfcParse::simple_type::binary_type:
    return "BINARY";
case IfcParse::simple_type::boolean_type:
    return "BOOLEAN";
case IfcParse::simple_type::integer_type:
    return "INTEGER";
case IfcParse::simple_type::logical_type:
    return "LOGICAL";
case IfcParse::simple_type::number_type:
    return "NUMBER";
case IfcParse::simple_type::real_type:
    return "REAL";
case IfcParse::simple_type::string_type:
    return "STRING";
case IfcParse::simple_type::datatype_COUNT:
    break;
}
throw std::runtime_error("Unknown simple type.");
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_type_of_aggregation(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp->type_of_aggregation());
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_type_of_aggregation_string(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
switch (self_cpp->type_of_aggregation()) {
case IfcParse::aggregation_type::array_type:
    return "array";
case IfcParse::aggregation_type::bag_type:
    return "bag";
case IfcParse::aggregation_type::list_type:
    return "list";
case IfcParse::aggregation_type::set_type:
    return "set";
}
throw std::runtime_error("Unknown aggregation type.");
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_type_of_aggregation(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp->type_of_aggregation());
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_inverse_attribute_type_of_aggregation_string(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
switch (self_cpp->type_of_aggregation()) {
case IfcParse::inverse_attribute::bag_type:
    return "bag";
case IfcParse::inverse_attribute::set_type:
    return "set";
case IfcParse::inverse_attribute::unspecified_type:
    return "";
}
throw std::runtime_error("Unknown inverse aggregation type.");
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_entity_argument_types(ifcopenshell_ifc_entity_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<std::string> result;
size_t index = 0;
for (const auto* attr : self_cpp->all_attributes()) {
    auto argument_type = IfcUtil::Argument_UNKNOWN;
    auto* parameter_type = attr->type_of_attribute();
    if (self_cpp->derived()[index++]) {
        argument_type = IfcUtil::Argument_DERIVED;
    } else if (parameter_type != nullptr) {
        argument_type = IfcUtil::from_parameter_type(parameter_type);
    }
    result.push_back(IfcUtil::ArgumentTypeToString(argument_type));
}
return result;
        }();
        *out_result = make_string_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_status(ifcopenshell_ifc_instance_streamer_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(static_cast<IfcParse::file_open_status::file_open_enum>(self_cpp->status()));
        }();
        *out_result = static_cast<int32_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_references(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return unresolved_references_to_json_string(self_cpp->references(), nullptr);
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_inverses(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return inverses_to_json_string(self_cpp->inverses());
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_instance_streamer_read_instance_py(ifcopenshell_ifc_instance_streamer_t* self, bool type_as_declaration_instance, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return instance_stream_read_instance_json(self_cpp, type_as_declaration_instance);
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_aggregation_type_kind(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
switch (self_cpp->type_of_aggregation()) {
case IfcParse::aggregation_type::array_type:
    return "ARRAY";
case IfcParse::aggregation_type::bag_type:
    return "BAG";
case IfcParse::aggregation_type::list_type:
    return "LIST";
case IfcParse::aggregation_type::set_type:
    return "SET";
}
throw std::runtime_error("Unknown aggregation type.");
        }();
        *out_result = make_static_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_instance_list_get(ifcopenshell_ifcparse_instance_list_t* self, size_t index, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
        auto generated_result = [&]() {
if (index >= self_cpp->size()) {
    throw std::out_of_range("Instance list index out of range.");
}
return (*self_cpp)[static_cast<int>(index)];
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->edges());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges_item_ids(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->edges_item_ids());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_faces(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->faces());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_item_ids(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->item_ids());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_material_ids(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->material_ids());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_materials(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_ifcgeom_taxonomy_style_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_taxonomy_style_list(self_cpp->materials());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_normals(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->normals());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_polyhedral_faces_with_holes(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list_list_list(self_cpp->polyhedral_faces_with_holes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_polyhedral_faces_without_holes(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_int32_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list_list(self_cpp->polyhedral_faces_without_holes());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_uvs(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->uvs());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_verts(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->verts());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_verts_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->verts().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_faces_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->faces().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_normals_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->normals().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->edges().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_material_ids_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->material_ids().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_item_ids_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->item_ids().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges_item_ids_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->edges_item_ids().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_uvs_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->uvs().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_material_count(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->materials().size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_compute_bounds(ifcopenshell_ifcgeom_iterator_t* self, bool with_geometry) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto with_geometry_cpp = static_cast<bool>(with_geometry);
        self_cpp->compute_bounds(with_geometry_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_create(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{const_cast<IfcUtil::IfcBaseClass*>(self_cpp->create()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_file(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_file_t{self_cpp->file(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_element_t{self_cpp->get(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_log(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->getLog());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_native(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_brep_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_brep_element_t{self_cpp->get_native(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_object(ifcopenshell_ifcgeom_iterator_t* self, int32_t id, ifcopenshell_ifcgeom_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto id_cpp = static_cast<int>(id);
        *out_result = new ifcopenshell_ifcgeom_element_t{const_cast<IfcGeom::Element*>(self_cpp->get_object(id_cpp)), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_task_items(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_taxonomy_item_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_taxonomy_item_list(self_cpp->get_task_items());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_had_error_processing_elements(ifcopenshell_ifcgeom_iterator_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->had_error_processing_elements();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_initialize(ifcopenshell_ifcgeom_iterator_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->initialize();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_progress(ifcopenshell_ifcgeom_iterator_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->progress());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_unit_magnitude(ifcopenshell_ifcgeom_iterator_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->unit_magnitude());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_unit_name(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->unit_name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_calculate_projected_surface_area(ifcopenshell_ifcgeom_brep_representation_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t* ax, double along_x, double along_y, double along_z, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (ax == nullptr || ax->ptr == nullptr) { throw std::runtime_error("Handle parameter \"ax\" is invalid"); }
    const auto& ax_cpp = ax->ptr;
    auto along_x_cpp = static_cast<double&>(along_x);
    auto along_y_cpp = static_cast<double&>(along_y);
    auto along_z_cpp = static_cast<double&>(along_z);
        *out_result = self_cpp->calculate_projected_surface_area(ax_cpp, along_x_cpp, along_y_cpp, along_z_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_entity(ifcopenshell_ifcgeom_brep_representation_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->entity());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_id(ifcopenshell_ifcgeom_brep_representation_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->id());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_item_id(ifcopenshell_ifcgeom_brep_representation_t* self, int32_t i, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto i_cpp = static_cast<int>(i);
        *out_result = static_cast<int32_t>(self_cpp->item_id(i_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_settings(ifcopenshell_ifcgeom_brep_representation_t* self, ifcopenshell_ifcgeom_settings_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_settings_t{const_cast<ifcopenshell::geometry::Settings*>(&(self_cpp->settings())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_size(ifcopenshell_ifcgeom_brep_representation_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->size());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_context(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->context());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_guid(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->guid());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_id(ifcopenshell_ifcgeom_element_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->id());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_name(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->name());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_parent_id(ifcopenshell_ifcgeom_element_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->parent_id());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_parents(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_ifcgeom_element_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_element_list(self_cpp->parents());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_product(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{const_cast<IfcUtil::IfcBaseEntity*>(self_cpp->product()), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_transformation(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_ifcgeom_transformation_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_transformation_t{const_cast<IfcGeom::Transformation*>(&(self_cpp->transformation())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_type(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->type());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_unique_id(ifcopenshell_ifcgeom_element_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->unique_id());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_element_calculate_projected_surface_area(ifcopenshell_ifcgeom_brep_element_t* self, double along_x, double along_y, double along_z, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto along_x_cpp = static_cast<double&>(along_x);
    auto along_y_cpp = static_cast<double&>(along_y);
    auto along_z_cpp = static_cast<double&>(along_z);
        *out_result = self_cpp->calculate_projected_surface_area(along_x_cpp, along_y_cpp, along_z_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_element_geometry(ifcopenshell_ifcgeom_brep_element_t* self, ifcopenshell_ifcgeom_brep_representation_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_brep_representation_t{const_cast<IfcGeom::Representation::BRep*>(&(self_cpp->geometry())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_element_geometry(ifcopenshell_ifcgeom_triangulation_element_t* self, ifcopenshell_ifcgeom_triangulation_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_triangulation_t{const_cast<IfcGeom::Representation::Triangulation*>(&(self_cpp->geometry())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serialized_element_geometry(ifcopenshell_ifcgeom_serialized_element_t* self, ifcopenshell_ifcgeom_serialization_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_serialization_t{const_cast<IfcGeom::Representation::Serialization*>(&(self_cpp->geometry())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_solid_mt(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->solid(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_add(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t* arg_0, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->add(arg_0_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_box(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->box(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_concat(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t* arg_0, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->concat(arg_0_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_edges(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_conversion_result_shape_list(self_cpp->edges());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_facets(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_conversion_result_shape_list(self_cpp->facets());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_halfspaces(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->halfspaces(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_intersect(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t* arg_0, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->intersect(arg_0_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_is_manifold(ifcopenshell_ifcgeom_conversion_result_shape_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->is_manifold();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_moved(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t* arg_0, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->moved(arg_0_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_num_edges(ifcopenshell_ifcgeom_conversion_result_shape_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->num_edges());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_num_faces(ifcopenshell_ifcgeom_conversion_result_shape_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->num_faces());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_num_vertices(ifcopenshell_ifcgeom_conversion_result_shape_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->num_vertices());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_solid(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->solid(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_subtract(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t* arg_0, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->subtract(arg_0_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_surface_area_along_direction(ifcopenshell_ifcgeom_conversion_result_shape_t* self, double tol, ifcopenshell_ifcgeom_taxonomy_matrix4_t* arg_1, double along_x, double along_y, double along_z, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto tol_cpp = static_cast<double>(tol);
    if (arg_1 == nullptr || arg_1->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_1\" is invalid"); }
    const auto& arg_1_cpp = arg_1->ptr;
    auto along_x_cpp = static_cast<double&>(along_x);
    auto along_y_cpp = static_cast<double&>(along_y);
    auto along_z_cpp = static_cast<double&>(along_z);
        *out_result = self_cpp->surface_area_along_direction(tol_cpp, arg_1_cpp, along_x_cpp, along_y_cpp, along_z_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_surface_genus(ifcopenshell_ifcgeom_conversion_result_shape_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->surface_genus());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_vertices(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_conversion_result_shape_list(self_cpp->vertices());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_wrap_in_compound(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{self_cpp->wrap_in_compound(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_function_item_evaluator_evaluation_points(ifcopenshell_ifcgeom_function_item_evaluator_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->evaluation_points());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_function_item_evaluator_evaluate(ifcopenshell_ifcgeom_function_item_evaluator_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->evaluate()};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serialization_brep_data(ifcopenshell_ifcgeom_serialization_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->brep_data());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serialization_surface_style_ids(ifcopenshell_ifcgeom_serialization_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->surface_style_ids());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serialization_surface_styles(ifcopenshell_ifcgeom_serialization_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->surface_styles());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_geometry_settings(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifcgeom_settings_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_settings_t{const_cast<ifcopenshell::geometry::Settings*>(&(self_cpp->geometry_settings())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_settings(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifcgeom_serializer_settings_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_serializer_settings_t{const_cast<ifcopenshell::geometry::SerializerSettings*>(&(self_cpp->settings())), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_write_triangulation_element(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifcgeom_triangulation_element_t* o) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (o == nullptr || o->ptr == nullptr) { throw std::runtime_error("Handle parameter \"o\" is invalid"); }
    auto o_cpp = o->ptr;
        self_cpp->write(o_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_write_brep_element(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifcgeom_brep_element_t* o) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (o == nullptr || o->ptr == nullptr) { throw std::runtime_error("Handle parameter \"o\" is invalid"); }
    auto o_cpp = o->ptr;
        self_cpp->write(o_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_finalize(ifcopenshell_ifcgeom_geometry_serializer_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->finalize();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_is_tesselated(ifcopenshell_ifcgeom_geometry_serializer_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->isTesselated();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_read(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifc_file_t* f, const char* guid, const char* representation_id, int32_t rt, ifcopenshell_ifcgeom_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (f == nullptr || f->ptr == nullptr) { throw std::runtime_error("Handle parameter \"f\" is invalid"); }
    auto& f_cpp = *f->ptr;
    if (guid == nullptr) { throw std::runtime_error("Parameter \"guid\" must not be null"); }
    std::string guid_cpp(guid);
    if (representation_id == nullptr) { throw std::runtime_error("Parameter \"representation_id\" must not be null"); }
    std::string representation_id_cpp(representation_id);
    auto rt_cpp = static_cast<GeometrySerializer::read_type>(rt);
        *out_result = new ifcopenshell_ifcgeom_element_t{self_cpp->read(f_cpp, guid_cpp, representation_id_cpp, rt_cpp), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_ready(ifcopenshell_ifcgeom_geometry_serializer_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->ready();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_set_file(ifcopenshell_ifcgeom_geometry_serializer_t* self, ifcopenshell_ifc_file_t* arg_0) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        self_cpp->setFile(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_set_unit_name_and_magnitude(ifcopenshell_ifcgeom_geometry_serializer_t* self, const char* name, double magnitude) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    auto magnitude_cpp = static_cast<float>(magnitude);
        self_cpp->setUnitNameAndMagnitude(name_cpp, magnitude_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_geometry_serializer_write_header(ifcopenshell_ifcgeom_geometry_serializer_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->writeHeader();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_circle_matrix(ifcopenshell_ifcgeom_taxonomy_circle_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_circle_radius(ifcopenshell_ifcgeom_taxonomy_circle_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_line_matrix(ifcopenshell_ifcgeom_taxonomy_line_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_line_as_item(ifcopenshell_ifcgeom_taxonomy_line_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{std::static_pointer_cast<ifcopenshell::geometry::taxonomy::item>(self->ptr)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_plane_matrix(ifcopenshell_ifcgeom_taxonomy_plane_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_ellipse_matrix(ifcopenshell_ifcgeom_taxonomy_ellipse_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_ellipse_radius1(ifcopenshell_ifcgeom_taxonomy_ellipse_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_ellipse_radius2(ifcopenshell_ifcgeom_taxonomy_ellipse_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius2);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_has_specularity(ifcopenshell_ifcgeom_taxonomy_style_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->has_specularity();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_has_transparency(ifcopenshell_ifcgeom_taxonomy_style_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->has_transparency();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_diffuse(ifcopenshell_ifcgeom_taxonomy_style_t* self, ifcopenshell_ifcgeom_taxonomy_colour_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_colour_t{std::make_shared<ifcopenshell::geometry::taxonomy::colour>(self_cpp->diffuse)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_name(ifcopenshell_ifcgeom_taxonomy_style_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_string(self_cpp->name);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_specular(ifcopenshell_ifcgeom_taxonomy_style_t* self, ifcopenshell_ifcgeom_taxonomy_colour_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_colour_t{std::make_shared<ifcopenshell::geometry::taxonomy::colour>(self_cpp->specular)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_specularity(ifcopenshell_ifcgeom_taxonomy_style_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->specularity);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_surface(ifcopenshell_ifcgeom_taxonomy_style_t* self, ifcopenshell_ifcgeom_taxonomy_colour_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_colour_t{std::make_shared<ifcopenshell::geometry::taxonomy::colour>(self_cpp->surface)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_transparency(ifcopenshell_ifcgeom_taxonomy_style_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->transparency);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_use_surface_color(ifcopenshell_ifcgeom_taxonomy_style_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->use_surface_color;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sphere_matrix(ifcopenshell_ifcgeom_taxonomy_sphere_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sphere_radius(ifcopenshell_ifcgeom_taxonomy_sphere_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_torus_matrix(ifcopenshell_ifcgeom_taxonomy_torus_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_torus_radius1(ifcopenshell_ifcgeom_taxonomy_torus_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius1);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_torus_radius2(ifcopenshell_ifcgeom_taxonomy_torus_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius2);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_cylinder_matrix(ifcopenshell_ifcgeom_taxonomy_cylinder_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_cylinder_radius(ifcopenshell_ifcgeom_taxonomy_cylinder_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->radius);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_extrusion_basis(ifcopenshell_ifcgeom_taxonomy_extrusion_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->basis) { throw std::runtime_error("basis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->basis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_extrusion_depth(ifcopenshell_ifcgeom_taxonomy_extrusion_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->depth);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_extrusion_direction(ifcopenshell_ifcgeom_taxonomy_extrusion_t* self, ifcopenshell_ifcgeom_taxonomy_direction3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->direction) { throw std::runtime_error("direction is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_direction3_t{self_cpp->direction};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_extrusion_matrix(ifcopenshell_ifcgeom_taxonomy_extrusion_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_offset_curve_basis(ifcopenshell_ifcgeom_taxonomy_offset_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->basis) { throw std::runtime_error("basis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->basis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_offset_curve_offset(ifcopenshell_ifcgeom_taxonomy_offset_curve_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<double>(self_cpp->offset);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_offset_curve_reference(ifcopenshell_ifcgeom_taxonomy_offset_curve_t* self, ifcopenshell_ifcgeom_taxonomy_direction3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->reference) { throw std::runtime_error("reference is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_direction3_t{self_cpp->reference};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_offset_curve_as_item(ifcopenshell_ifcgeom_taxonomy_offset_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{std::static_pointer_cast<ifcopenshell::geometry::taxonomy::item>(self->ptr)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_has_angle(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->angle.is_initialized();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_angle(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->angle.is_initialized()) { throw std::runtime_error("angle is not set"); }
        *out_result = static_cast<double>(*self_cpp->angle);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_axis_origin(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->axis_origin) { throw std::runtime_error("axis_origin is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{self_cpp->axis_origin};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_basis(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->basis) { throw std::runtime_error("basis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->basis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_direction(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, ifcopenshell_ifcgeom_taxonomy_direction3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->direction) { throw std::runtime_error("direction is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_direction3_t{self_cpp->direction};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_revolve_matrix(ifcopenshell_ifcgeom_taxonomy_revolve_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_degree(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<int32_t>(self_cpp->degree);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_knots(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_double_list(self_cpp->knots);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_multiplicities(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_int32_list(self_cpp->multiplicities);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_has_weights(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->weights.is_initialized();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_weights(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->weights.is_initialized()) { throw std::runtime_error("weights is not set"); }
        *out_result = make_double_list(*self_cpp->weights);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_control_point_count(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->control_points.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_control_point_at(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->control_points.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{self_cpp->control_points[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_curve_as_item(ifcopenshell_ifcgeom_taxonomy_bspline_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{std::static_pointer_cast<ifcopenshell::geometry::taxonomy::item>(self->ptr)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_basis(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->basis) { throw std::runtime_error("basis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->basis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_has_basis(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->basis != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_curve(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->curve) { throw std::runtime_error("curve is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->curve};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_has_curve(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->curve != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_direction(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, ifcopenshell_ifcgeom_taxonomy_direction3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->direction) { throw std::runtime_error("direction is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_direction3_t{self_cpp->direction};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_has_direction(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->direction != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_matrix(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_has_matrix(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->matrix != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_surface(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->surface) { throw std::runtime_error("surface is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->surface};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_has_surface(ifcopenshell_ifcgeom_taxonomy_sweep_along_curve_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->surface != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_face_basis(ifcopenshell_ifcgeom_taxonomy_face_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->basis) { throw std::runtime_error("basis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->basis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_face_loop_count(ifcopenshell_ifcgeom_taxonomy_face_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_face_loop_at(ifcopenshell_ifcgeom_taxonomy_face_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_loop_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_loop_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_face_matrix(ifcopenshell_ifcgeom_taxonomy_face_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_face_as_item(ifcopenshell_ifcgeom_taxonomy_face_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{std::static_pointer_cast<ifcopenshell::geometry::taxonomy::item>(self->ptr)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_axis(ifcopenshell_ifcgeom_taxonomy_loft_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->axis) { throw std::runtime_error("axis is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->axis};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_has_axis(ifcopenshell_ifcgeom_taxonomy_loft_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = (self_cpp->axis != nullptr);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_item_count(ifcopenshell_ifcgeom_taxonomy_loft_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_item_at(ifcopenshell_ifcgeom_taxonomy_loft_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_add_item(ifcopenshell_ifcgeom_taxonomy_loft_t* self, ifcopenshell_ifcgeom_taxonomy_item_t* item) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
        auto cast_item = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::geom_item>(item_cpp);
        if (!cast_item) { throw std::runtime_error("Invalid item type"); }
        self_cpp->children.push_back(cast_item);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loft_set_axis(ifcopenshell_ifcgeom_taxonomy_loft_t* self, ifcopenshell_ifcgeom_taxonomy_item_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
    if (value == nullptr || value->ptr == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
    auto value_cpp = value->ptr;
        self_cpp->axis = value_cpp;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loop_edge_count(ifcopenshell_ifcgeom_taxonomy_loop_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_loop_edge_at(ifcopenshell_ifcgeom_taxonomy_loop_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_edge_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_edge_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_shell_face_count(ifcopenshell_ifcgeom_taxonomy_shell_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_shell_face_at(ifcopenshell_ifcgeom_taxonomy_shell_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_face_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_face_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_solid_shell_count(ifcopenshell_ifcgeom_taxonomy_solid_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_solid_shell_at(ifcopenshell_ifcgeom_taxonomy_solid_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_shell_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_shell_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_solid_matrix(ifcopenshell_ifcgeom_taxonomy_solid_t* self, ifcopenshell_ifcgeom_taxonomy_matrix4_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (!self_cpp->matrix) { throw std::runtime_error("matrix is not set"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_matrix4_t{self_cpp->matrix};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_collection_item_count(ifcopenshell_ifcgeom_taxonomy_collection_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_collection_item_at(ifcopenshell_ifcgeom_taxonomy_collection_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_collection_add_item(ifcopenshell_ifcgeom_taxonomy_collection_t* self, ifcopenshell_ifcgeom_taxonomy_item_t* item) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
        auto cast_item = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::geom_item>(item_cpp);
        if (!cast_item) { throw std::runtime_error("Invalid item type"); }
        self_cpp->children.push_back(cast_item);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_boolean_result_operation(ifcopenshell_ifcgeom_taxonomy_boolean_result_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<int32_t>(self_cpp->operation);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_boolean_result_item_count(ifcopenshell_ifcgeom_taxonomy_boolean_result_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = self_cpp->children.size();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_boolean_result_item_at(ifcopenshell_ifcgeom_taxonomy_boolean_result_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        if (index >= self_cpp->children.size()) { throw std::runtime_error("Index out of bounds"); }
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{self_cpp->children[index]};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_boolean_result_add_item(ifcopenshell_ifcgeom_taxonomy_boolean_result_t* self, ifcopenshell_ifcgeom_taxonomy_item_t* item) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
    if (item == nullptr || item->ptr == nullptr) { throw std::runtime_error("Handle parameter \"item\" is invalid"); }
    auto item_cpp = item->ptr;
        auto cast_item = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::geom_item>(item_cpp);
        if (!cast_item) { throw std::runtime_error("Invalid item type"); }
        self_cpp->children.push_back(cast_item);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_degree_u(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<int32_t>(self_cpp->degree[0]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_degree_v(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<int32_t>(self_cpp->degree[1]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_multiplicities_u(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_int32_list(self_cpp->multiplicities[0]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_multiplicities_v(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_int32_list(self_cpp->multiplicities[1]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_knots_u(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_double_list(self_cpp->knots[0]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_knots_v(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = make_double_list(self_cpp->knots[1]);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_as_item(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{std::static_pointer_cast<ifcopenshell::geometry::taxonomy::item>(self->ptr)};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_clone(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{self_cpp->clone(), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_to_double(ifcopenshell_ifcgeom_opaque_number_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->to_double());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_to_string(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->to_string());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_add(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) + other_cpp;
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_subtract(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) - other_cpp;
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_multiply(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) * other_cpp;
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_divide(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) / other_cpp;
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_negate(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return -(*self_cpp);
        }();
        *out_result = new ifcopenshell_ifcgeom_opaque_number_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_equals(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) == other_cpp;
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_opaque_number_less_than(ifcopenshell_ifcgeom_opaque_number_t* self, ifcopenshell_ifcgeom_opaque_number_t* other, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (other == nullptr || other->ptr == nullptr) { throw std::runtime_error("Handle parameter \"other\" is invalid"); }
    auto other_cpp = other->ptr;
        auto generated_result = [&]() {
return (*self_cpp) < other_cpp;
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_finalize(ifcopenshell_ifcgeom_serializer_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->finalize();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_ready(ifcopenshell_ifcgeom_serializer_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->ready();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_set_file(ifcopenshell_ifcgeom_serializer_t* self, ifcopenshell_ifc_file_t* arg_0) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        self_cpp->setFile(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_write_header(ifcopenshell_ifcgeom_serializer_t* self) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        self_cpp->writeHeader();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_type(ifcopenshell_ifcgeom_settings_t* self, const char* name, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = make_string(self_cpp->get_type(name_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_setting_names(ifcopenshell_ifcgeom_settings_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->setting_names());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_bool(ifcopenshell_ifcgeom_settings_t* self, const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<bool>(&val)) { *out_result = *p; matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_bool(ifcopenshell_ifcgeom_settings_t* self, const char* name, bool value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_int(ifcopenshell_ifcgeom_settings_t* self, const char* name, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<int>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (auto* p = boost::get<ifcopenshell::geometry::settings::IteratorOutputOptions>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (auto* p = boost::get<ifcopenshell::geometry::settings::FunctionStepMethod>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (auto* p = boost::get<ifcopenshell::geometry::settings::OutputDimensionalityTypes>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (auto* p = boost::get<ifcopenshell::geometry::settings::TriangulationMethod>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_int(ifcopenshell_ifcgeom_settings_t* self, const char* name, int32_t value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(int(value)));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_double(ifcopenshell_ifcgeom_settings_t* self, const char* name, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<double>(&val)) { *out_result = static_cast<double>(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_double(ifcopenshell_ifcgeom_settings_t* self, const char* name, double value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_string(ifcopenshell_ifcgeom_settings_t* self, const char* name, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::string>(&val)) { *out_result = make_string(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_string(ifcopenshell_ifcgeom_settings_t* self, const char* name, const char* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_int_set(ifcopenshell_ifcgeom_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::set<int>>(&val)) { *out_result = make_int32_list(([&]() { auto tmp = *p; return std::vector(tmp.begin(), tmp.end()); })()); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_int_set(ifcopenshell_ifcgeom_settings_t* self, const char* name, const ifcopenshell_int32_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_vec = to_cpp_int32_list(value);
    std::set<int> value_cpp(value_vec.begin(), value_vec.end());
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_string_set(ifcopenshell_ifcgeom_settings_t* self, const char* name, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::set<std::string>>(&val)) { *out_result = make_string_list(([&]() { auto tmp = *p; return std::vector(tmp.begin(), tmp.end()); })()); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_string_set(ifcopenshell_ifcgeom_settings_t* self, const char* name, const ifcopenshell_string_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_vec = to_cpp_string_list(value);
    std::set<std::string> value_cpp(value_vec.begin(), value_vec.end());
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_get_double_list(ifcopenshell_ifcgeom_settings_t* self, const char* name, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::vector<double>>(&val)) { *out_result = make_double_list(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_settings_set_double_list(ifcopenshell_ifcgeom_settings_t* self, const char* name, const ifcopenshell_double_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_cpp = to_cpp_double_list(value);
        self_cpp->set(name_cpp, ifcopenshell::geometry::Settings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_type(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        *out_result = make_string(self_cpp->get_type(name_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_setting_names(ifcopenshell_ifcgeom_serializer_settings_t* self, ifcopenshell_string_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string_list(self_cpp->setting_names());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_bool(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<bool>(&val)) { *out_result = *p; matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_set_bool(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, bool value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::SerializerSettings::value_variant_t(value));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_int(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<int>(&val)) { *out_result = static_cast<int32_t>(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_set_int(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, int32_t value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::SerializerSettings::value_variant_t(int(value)));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_double(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<double>(&val)) { *out_result = static_cast<double>(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_set_double(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, double value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        self_cpp->set(name_cpp, ifcopenshell::geometry::SerializerSettings::value_variant_t(value));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_string(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::string>(&val)) { *out_result = make_string(*p); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_set_string(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, const char* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    std::string value_cpp(value);
        self_cpp->set(name_cpp, ifcopenshell::geometry::SerializerSettings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_get_int_set(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
        auto val = self_cpp->get(name_cpp);
        bool matched = false;
        if (auto* p = boost::get<std::set<int>>(&val)) { *out_result = make_int32_list(([&]() { auto tmp = *p; return std::vector(tmp.begin(), tmp.end()); })()); matched = true; }
        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_serializer_settings_set_int_set(ifcopenshell_ifcgeom_serializer_settings_t* self, const char* name, const ifcopenshell_int32_list_t* value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Parameter \"value\" must not be null"); }
    auto value_vec = to_cpp_int32_list(value);
    std::set<int> value_cpp(value_vec.begin(), value_vec.end());
        self_cpp->set(name_cpp, ifcopenshell::geometry::SerializerSettings::value_variant_t(value_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_buffer_get_value(ifcopenshell_ifcgeom_buffer_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->get_value());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_buffer_is_ready(ifcopenshell_ifcgeom_buffer_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->is_ready();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_item_hash(ifcopenshell_ifcgeom_taxonomy_item_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<size_t>(self_cpp->hash());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_item_identity(ifcopenshell_ifcgeom_taxonomy_item_t* self, uint32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<uint32_t>(self_cpp->identity());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_item_kind(ifcopenshell_ifcgeom_taxonomy_item_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        *out_result = static_cast<int32_t>(self_cpp->kind());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_enable_face_styles(ifcopenshell_ifcgeom_tree_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = self_cpp->enable_face_styles();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_set_enable_face_styles(ifcopenshell_ifcgeom_tree_t* self, bool b) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    auto b_cpp = static_cast<bool>(b);
        self_cpp->enable_face_styles(b_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_add_file(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifc_file_t* f, ifcopenshell_ifcgeom_settings_t* settings) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (f == nullptr || f->ptr == nullptr) { throw std::runtime_error("Handle parameter \"f\" is invalid"); }
    auto& f_cpp = *f->ptr;
    if (settings == nullptr || settings->ptr == nullptr) { throw std::runtime_error("Handle parameter \"settings\" is invalid"); }
    auto settings_cpp = *settings->ptr;
        self_cpp->add_file(f_cpp, settings_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_add_iterator(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_iterator_t* it) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (it == nullptr || it->ptr == nullptr) { throw std::runtime_error("Handle parameter \"it\" is invalid"); }
    auto& it_cpp = *it->ptr;
        self_cpp->add_file(it_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_distances(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->distances());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_is_manifold(ifcopenshell_ifcgeom_tree_t* self, const ifcopenshell_int32_list_t* fs, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (fs == nullptr) { throw std::runtime_error("Parameter \"fs\" must not be null"); }
    auto fs_cpp = to_cpp_int32_list(fs);
        *out_result = self_cpp->is_manifold(fs_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_protrusion_distances(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(self_cpp->protrusion_distances());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_styles(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_taxonomy_style_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_ifcgeom_taxonomy_style_list(self_cpp->styles());
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_uint8_to_b64(ifcopenshell_ifcgeom_tree_t* self, const ifcopenshell_uint8_list_t* uuids_array, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (uuids_array == nullptr) { throw std::runtime_error("Parameter \"uuids_array\" must not be null"); }
    auto uuids_array_cpp = to_cpp_uint8_list(uuids_array);
        *out_result = make_string(self_cpp->uint8_to_b64(uuids_array_cpp));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_a(ifcopenshell_ifcgeom_tree_clash_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        if (!self_cpp->a) { throw std::runtime_error("a is not set"); }
        *out_result = new ifcopenshell_ifc_instance_t{const_cast<IfcUtil::IfcBaseClass*>(self_cpp->a), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_b(ifcopenshell_ifcgeom_tree_clash_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        if (!self_cpp->b) { throw std::runtime_error("b is not set"); }
        *out_result = new ifcopenshell_ifc_instance_t{const_cast<IfcUtil::IfcBaseClass*>(self_cpp->b), false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_type(ifcopenshell_ifcgeom_tree_clash_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->clash_type);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_distance(ifcopenshell_ifcgeom_tree_clash_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->distance);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_p1(ifcopenshell_ifcgeom_tree_clash_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(std::vector<double>(self_cpp->p1.begin(), self_cpp->p1.end()));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_p2(ifcopenshell_ifcgeom_tree_clash_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(std::vector<double>(self_cpp->p2.begin(), self_cpp->p2.end()));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_distance(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->distance);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_dot_product(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->dot_product);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_normal(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(std::vector<double>(self_cpp->normal.begin(), self_cpp->normal.end()));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_position(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_double_list(std::vector<double>(self_cpp->position.begin(), self_cpp->position.end()));
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_ray_distance(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<double>(self_cpp->ray_distance);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_style_index(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, int32_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<int32_t>(self_cpp->style_index);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_point3_get_data(ifcopenshell_ifcgeom_taxonomy_point3_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        const auto& v = self_cpp->ccomponents();
        *out_result = make_double_list(std::vector<double>{{v(0), v(1), v(2)}});
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_direction3_get_data(ifcopenshell_ifcgeom_taxonomy_direction3_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        const auto& v = self_cpp->ccomponents();
        *out_result = make_double_list(std::vector<double>{{v(0), v(1), v(2)}});
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_matrix4_get_data(ifcopenshell_ifcgeom_taxonomy_matrix4_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        std::vector<double> data(16);
        const auto& mat = self_cpp->ccomponents();
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                data[i * 4 + j] = mat(i, j);
            }
        }
        *out_result = make_double_list(data);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_colour_get_data(ifcopenshell_ifcgeom_taxonomy_colour_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        const auto& v = self_cpp->ccomponents();
        *out_result = make_double_list(std::vector<double>{{v(0), v(1), v(2)}});
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_transformation_matrix(ifcopenshell_ifcgeom_transformation_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        std::vector<double> data(16);
        const auto& mat = self_cpp->data()->ccomponents();
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                data[i * 4 + j] = mat(i, j);
            }
        }
        *out_result = make_double_list(data);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_element(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifc_instance_t* instance, bool completely_within, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>();
if (!entity) {
    throw std::runtime_error("Instance should be an IfcProduct entity");
}
auto selected = self_cpp->select(entity, completely_within, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_point(ifcopenshell_ifcgeom_tree_t* self, double x, double y, double z, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
gp_Pnt p(x, y, z);
auto selected = self_cpp->select(p, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_brep_element(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_brep_element_t* element, bool completely_within, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (element == nullptr || element->ptr == nullptr) { throw std::runtime_error("Handle parameter \"element\" is invalid"); }
    auto element_cpp = element->ptr;
        auto generated_result = [&]() {
auto selected = self_cpp->select(element_cpp, completely_within, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_shape_serialization(ifcopenshell_ifcgeom_tree_t* self, const char* shape_serialization, bool completely_within, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (shape_serialization == nullptr) { throw std::runtime_error("Parameter \"shape_serialization\" must not be null"); }
    std::string shape_serialization_cpp(shape_serialization);
        auto generated_result = [&]() {
std::stringstream stream(shape_serialization);
BRepTools_ShapeSet shapes;
shapes.Read(stream);
const TopoDS_Shape& shape = shapes.Shape(shapes.NbShapes());
auto selected = self_cpp->select(shape, completely_within, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_box_point(ifcopenshell_ifcgeom_tree_t* self, double x, double y, double z, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
gp_Pnt p(x, y, z);
auto selected = self_cpp->select_box(p, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_box_element(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifc_instance_t* instance, bool completely_within, double extend, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        auto generated_result = [&]() {
auto* entity = instance_cpp->as<IfcUtil::IfcBaseEntity>();
if (!entity) {
    throw std::runtime_error("Instance should be an IfcProduct entity");
}
auto selected = self_cpp->select_box(entity, completely_within, extend);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_box_bounds(ifcopenshell_ifcgeom_tree_t* self, double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, bool completely_within, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
Bnd_Box box;
box.Update(xmin, ymin, zmin, xmax, ymax, zmax);
auto selected = self_cpp->select_box(box, completely_within);
auto result = aggregate_of_instance::ptr(new aggregate_of_instance());
result->reserve(selected.size());
for (auto* item : selected) {
    result->push(const_cast<IfcUtil::IfcBaseEntity*>(item));
}
return result;
        }();
        *out_result = new ifcopenshell_ifcparse_instance_list_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_style_count(ifcopenshell_ifcgeom_tree_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->styles().size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_style_at(ifcopenshell_ifcgeom_tree_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_style_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
const auto& styles = self_cpp->styles();
if (index >= styles.size()) {
    throw std::out_of_range("Style index out of range");
}
return styles[index];
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_style_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_intersection_many(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcparse_instance_list_t* set_a, ifcopenshell_ifcparse_instance_list_t* set_b, double tolerance, bool check_all, ifcopenshell_ifcgeom_tree_clash_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (set_a == nullptr) { throw std::runtime_error("Handle parameter \"set_a\" must not be null"); }
    auto set_a_cpp = set_a->value;
    if (set_b == nullptr) { throw std::runtime_error("Handle parameter \"set_b\" must not be null"); }
    auto set_b_cpp = set_b->value;
        auto generated_result = [&]() {
std::vector<const IfcUtil::IfcBaseEntity*> entities_a;
entities_a.reserve(set_a_cpp->size());
for (auto it = set_a_cpp->begin(); it != set_a_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_a must contain IfcProduct entities");
    }
    entities_a.push_back(entity);
}

std::vector<const IfcUtil::IfcBaseEntity*> entities_b;
entities_b.reserve(set_b_cpp->size());
for (auto it = set_b_cpp->begin(); it != set_b_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_b must contain IfcProduct entities");
    }
    entities_b.push_back(entity);
}

return new std::vector<IfcGeom::clash>(self_cpp->clash_intersection_many(entities_a, entities_b, tolerance, check_all));
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_clash_list_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_collision_many(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcparse_instance_list_t* set_a, ifcopenshell_ifcparse_instance_list_t* set_b, bool allow_touching, ifcopenshell_ifcgeom_tree_clash_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (set_a == nullptr) { throw std::runtime_error("Handle parameter \"set_a\" must not be null"); }
    auto set_a_cpp = set_a->value;
    if (set_b == nullptr) { throw std::runtime_error("Handle parameter \"set_b\" must not be null"); }
    auto set_b_cpp = set_b->value;
        auto generated_result = [&]() {
std::vector<const IfcUtil::IfcBaseEntity*> entities_a;
entities_a.reserve(set_a_cpp->size());
for (auto it = set_a_cpp->begin(); it != set_a_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_a must contain IfcProduct entities");
    }
    entities_a.push_back(entity);
}

std::vector<const IfcUtil::IfcBaseEntity*> entities_b;
entities_b.reserve(set_b_cpp->size());
for (auto it = set_b_cpp->begin(); it != set_b_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_b must contain IfcProduct entities");
    }
    entities_b.push_back(entity);
}

return new std::vector<IfcGeom::clash>(self_cpp->clash_collision_many(entities_a, entities_b, allow_touching));
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_clash_list_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_clearance_many(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcparse_instance_list_t* set_a, ifcopenshell_ifcparse_instance_list_t* set_b, double clearance, bool check_all, ifcopenshell_ifcgeom_tree_clash_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (set_a == nullptr) { throw std::runtime_error("Handle parameter \"set_a\" must not be null"); }
    auto set_a_cpp = set_a->value;
    if (set_b == nullptr) { throw std::runtime_error("Handle parameter \"set_b\" must not be null"); }
    auto set_b_cpp = set_b->value;
        auto generated_result = [&]() {
std::vector<const IfcUtil::IfcBaseEntity*> entities_a;
entities_a.reserve(set_a_cpp->size());
for (auto it = set_a_cpp->begin(); it != set_a_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_a must contain IfcProduct entities");
    }
    entities_a.push_back(entity);
}

std::vector<const IfcUtil::IfcBaseEntity*> entities_b;
entities_b.reserve(set_b_cpp->size());
for (auto it = set_b_cpp->begin(); it != set_b_cpp->end(); ++it) {
    auto* entity = (*it) ? (*it)->as<IfcUtil::IfcBaseEntity>() : nullptr;
    if (!entity) {
        throw std::runtime_error("set_b must contain IfcProduct entities");
    }
    entities_b.push_back(entity);
}

return new std::vector<IfcGeom::clash>(self_cpp->clash_clearance_many(entities_a, entities_b, clearance, check_all));
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_clash_list_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_count(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_tree_clash_list_t* clashes, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (clashes == nullptr || clashes->ptr == nullptr) { throw std::runtime_error("Handle parameter \"clashes\" is invalid"); }
    auto clashes_cpp = clashes->ptr;
        auto generated_result = [&]() {
return clashes_cpp->size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_clash_at(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_tree_clash_list_t* clashes, size_t index, ifcopenshell_ifcgeom_tree_clash_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (clashes == nullptr || clashes->ptr == nullptr) { throw std::runtime_error("Handle parameter \"clashes\" is invalid"); }
    auto clashes_cpp = clashes->ptr;
        auto generated_result = [&]() {
if (index >= clashes_cpp->size()) {
    throw std::out_of_range("Clash index out of range");
}
return new IfcGeom::clash((*clashes_cpp)[index]);
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_clash_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_select_ray(ifcopenshell_ifcgeom_tree_t* self, double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double length, ifcopenshell_ifcgeom_tree_ray_intersection_list_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
gp_Pnt p0(origin_x, origin_y, origin_z);
gp_Dir d(dir_x, dir_y, dir_z);
return new std::vector<IfcGeom::ray_intersection_result>(self_cpp->select_ray(p0, d, length));
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_ray_intersection_list_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_count(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_tree_ray_intersection_list_t* intersections, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (intersections == nullptr || intersections->ptr == nullptr) { throw std::runtime_error("Handle parameter \"intersections\" is invalid"); }
    auto intersections_cpp = intersections->ptr;
        auto generated_result = [&]() {
return intersections_cpp->size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_at(ifcopenshell_ifcgeom_tree_t* self, ifcopenshell_ifcgeom_tree_ray_intersection_list_t* intersections, size_t index, ifcopenshell_ifcgeom_tree_ray_intersection_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (intersections == nullptr || intersections->ptr == nullptr) { throw std::runtime_error("Handle parameter \"intersections\" is invalid"); }
    auto intersections_cpp = intersections->ptr;
        auto generated_result = [&]() {
if (index >= intersections_cpp->size()) {
    throw std::out_of_range("Ray intersection index out of range");
}
return new IfcGeom::ray_intersection_result((*intersections_cpp)[index]);
        }();
        *out_result = new ifcopenshell_ifcgeom_tree_ray_intersection_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_tree_ray_intersection_instance(ifcopenshell_ifcgeom_tree_ray_intersection_t* self, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return const_cast<IfcUtil::IfcBaseEntity*>(self_cpp->instance);
        }();
        *out_result = new ifcopenshell_ifc_instance_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_next(ifcopenshell_ifcgeom_iterator_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->next() != nullptr;
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_as_triangulation_element(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_triangulation_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
IfcGeom::Element* elem = self_cpp->get();
IfcGeom::TriangulationElement* tri = dynamic_cast<IfcGeom::TriangulationElement*>(elem);
if (!tri) {
    throw std::runtime_error("Current element is not a TriangulationElement");
}
return tri;
        }();
        *out_result = new ifcopenshell_ifcgeom_triangulation_element_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_as_brep_element(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_brep_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
IfcGeom::Element* elem = self_cpp->get();
IfcGeom::BRepElement* brep = dynamic_cast<IfcGeom::BRepElement*>(elem);
if (!brep) {
    throw std::runtime_error("Current element is not a BRepElement");
}
return brep;
        }();
        *out_result = new ifcopenshell_ifcgeom_brep_element_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_as_serialized_element(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_serialized_element_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
IfcGeom::Element* elem = self_cpp->get();
IfcGeom::SerializedElement* serialized = dynamic_cast<IfcGeom::SerializedElement*>(elem);
if (!serialized) {
    throw std::runtime_error("Current element is not a SerializedElement");
}
return serialized;
        }();
        *out_result = new ifcopenshell_ifcgeom_serialized_element_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_bounds_min(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return std::make_shared<ifcopenshell::geometry::taxonomy::point3>(
    static_cast<const ifcopenshell::geometry::taxonomy::point3&>(self_cpp->bounds_min()));
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_bounds_max(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return std::make_shared<ifcopenshell::geometry::taxonomy::point3>(
    static_cast<const ifcopenshell::geometry::taxonomy::point3&>(self_cpp->bounds_max()));
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_iterator_get_task_products(ifcopenshell_ifcgeom_iterator_t* self, ifcopenshell_ifc_instance_list_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<std::vector<const IfcUtil::IfcBaseClass*>> result;
auto tasks = self_cpp->get_task_products();
if (!tasks) {
    return result;
}
for (auto outer = tasks->begin(); outer != tasks->end(); ++outer) {
    std::vector<const IfcUtil::IfcBaseClass*> row;
    row.reserve(outer->size());
    for (size_t i = 0; i < outer->size(); ++i) {
        row.push_back((*outer)[static_cast<int>(i)]);
    }
    result.push_back(std::move(row));
}
return result;
        }();
        *out_result = make_ifc_instance_list_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_transformation_buffer(ifcopenshell_ifcgeom_element_t* self, const double** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->transformation().data()->ccomponents();
        }();
        *out_result = (generated_result).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_element_transformation_buffer_size(ifcopenshell_ifcgeom_element_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return 16; // 4x4 matrix
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_element_calc_volume(ifcopenshell_ifcgeom_brep_element_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
double v;
if (self_cpp->geometry().calculate_volume(v)) {
    return v;
}
return std::numeric_limits<double>::quiet_NaN();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_element_calc_surface_area(ifcopenshell_ifcgeom_brep_element_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
double v;
if (self_cpp->geometry().calculate_surface_area(v)) {
    return v;
}
return std::numeric_limits<double>::quiet_NaN();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_as_compound(ifcopenshell_ifcgeom_brep_representation_t* self, bool force_meters, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->as_compound(force_meters);
        }();
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{generated_result, true};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_item(ifcopenshell_ifcgeom_brep_representation_t* self, int32_t index, ifcopenshell_ifcgeom_conversion_result_shape_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return const_cast<IfcGeom::ConversionResultShape*>(self_cpp->item(index));
        }();
        *out_result = new ifcopenshell_ifcgeom_conversion_result_shape_t{generated_result, false};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_calculate_volume(ifcopenshell_ifcgeom_brep_representation_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
double value = 0.0;
if (self_cpp->calculate_volume(value)) {
    return value;
}
return std::numeric_limits<double>::quiet_NaN();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_brep_representation_calculate_surface_area(ifcopenshell_ifcgeom_brep_representation_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
double value = 0.0;
if (self_cpp->calculate_surface_area(value)) {
    return value;
}
return std::numeric_limits<double>::quiet_NaN();
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_serialize(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
ifcopenshell::geometry::taxonomy::matrix4 identity;
std::string result;
self_cpp->Serialize(identity, result);
return result;
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_serialize_obj(ifcopenshell_ifcgeom_conversion_result_shape_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
#ifdef IFOPSH_WITH_CGAL
std::ostringstream result;
auto* cgs = dynamic_cast<ifcopenshell::geometry::CgalShape*>(self_cpp);
if (!cgs) {
    throw std::runtime_error("serialize_obj is only available for CGAL conversion result shapes");
}
write_to_obj(cgs->nef(), result, std::numeric_limits<size_t>::max());
return result.str();
#else
throw std::runtime_error("serialize_obj requires IFOPSH_WITH_CGAL");
return std::string();
#endif
        }();
        *out_result = make_string(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_convex_tag(ifcopenshell_ifcgeom_conversion_result_shape_t* self, bool value) {
    try {
        ifcopenshell_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        [&]() {
#ifdef IFOPSH_WITH_CGAL
auto* cgs = dynamic_cast<ifcopenshell::geometry::CgalShape*>(self_cpp);
if (!cgs) {
    throw std::runtime_error("convex_tag is only available for CGAL conversion result shapes");
}
cgs->convex_tag() = value;
#else
(void)value;
throw std::runtime_error("convex_tag requires IFOPSH_WITH_CGAL");
#endif
        }();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_area(ifcopenshell_ifcgeom_conversion_result_shape_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
auto* result = self_cpp->area();
double val = result->to_double();
delete result;
return val;
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_volume(ifcopenshell_ifcgeom_conversion_result_shape_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
auto* result = self_cpp->volume();
double val = result->to_double();
delete result;
return val;
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_conversion_result_shape_length(ifcopenshell_ifcgeom_conversion_result_shape_t* self, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
auto* result = self_cpp->length();
double val = result->to_double();
delete result;
return val;
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_material_at(ifcopenshell_ifcgeom_triangulation_t* self, size_t index, ifcopenshell_ifcgeom_taxonomy_style_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (index >= self_cpp->materials().size()) {
    throw std::runtime_error("Material index out of bounds");
}
return self_cpp->materials()[index];
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_style_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_verts_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const double** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->verts()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_faces_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const int32_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->faces()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_normals_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const double** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->normals()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const int32_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->edges()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_material_ids_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const int32_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->material_ids()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_item_ids_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const int32_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->item_ids()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_edges_item_ids_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const int32_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->edges_item_ids()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_uvs_buffer(ifcopenshell_ifcgeom_triangulation_t* self, const double** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = (self_cpp->uvs()).data();
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_colors_buffer(ifcopenshell_ifcgeom_triangulation_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<double> clrs;
clrs.reserve(self_cpp->materials().size() * 4);
for (auto& mptr : self_cpp->materials()) {
    auto& m = *mptr;
    if (m.diffuse) {
        clrs.push_back(m.diffuse.ccomponents()[0]);
        clrs.push_back(m.diffuse.ccomponents()[1]);
        clrs.push_back(m.diffuse.ccomponents()[2]);
    } else {
        clrs.push_back(0.);
        clrs.push_back(0.);
        clrs.push_back(0.);
    }
    if (m.has_transparency()) {
        clrs.push_back(1. - m.transparency);
    } else {
        clrs.push_back(1.);
    }
}
return clrs;
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_triangulation_colors_buffer_size(ifcopenshell_ifcgeom_triangulation_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->materials().size() * 4;
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_style_instance_id(ifcopenshell_ifcgeom_taxonomy_style_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (self_cpp->instance == nullptr) {
    return static_cast<size_t>(0);
}
const auto* entity = self_cpp->instance->as<IfcUtil::IfcBaseEntity>();
if (entity == nullptr) {
    return static_cast<size_t>(0);
}
return static_cast<size_t>(entity->id());
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_control_point_row_count(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
return self_cpp->control_points.size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_control_point_col_count_at(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t row, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (row >= self_cpp->control_points.size()) {
    throw std::runtime_error("B-spline surface control-point row out of bounds");
}
return self_cpp->control_points[row].size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_control_point_at(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t row, size_t col, ifcopenshell_ifcgeom_taxonomy_point3_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (row >= self_cpp->control_points.size()) {
    throw std::runtime_error("B-spline surface control-point row out of bounds");
}
if (col >= self_cpp->control_points[row].size()) {
    throw std::runtime_error("B-spline surface control-point column out of bounds");
}
return self_cpp->control_points[row][col];
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_point3_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_has_weights(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, bool* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
return self_cpp->weights.is_initialized();
        }();
        *out_result = generated_result;
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_weight_row_count(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (!self_cpp->weights.is_initialized()) {
    throw std::runtime_error("B-spline surface weights are not set");
}
return self_cpp->weights->size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_weight_col_count_at(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t row, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (!self_cpp->weights.is_initialized()) {
    throw std::runtime_error("B-spline surface weights are not set");
}
if (row >= self_cpp->weights->size()) {
    throw std::runtime_error("B-spline surface weight row out of bounds");
}
return (*self_cpp->weights)[row].size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_taxonomy_bspline_surface_weight_at(ifcopenshell_ifcgeom_taxonomy_bspline_surface_t* self, size_t row, size_t col, double* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr.get();
        auto generated_result = [&]() {
if (!self_cpp->weights.is_initialized()) {
    throw std::runtime_error("B-spline surface weights are not set");
}
if (row >= self_cpp->weights->size()) {
    throw std::runtime_error("B-spline surface weight row out of bounds");
}
if (col >= (*self_cpp->weights)[row].size()) {
    throw std::runtime_error("B-spline surface weight column out of bounds");
}
return (*self_cpp->weights)[row][col];
        }();
        *out_result = static_cast<double>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_boundary_size(ifcopenshell_ifcgeom_svgfill_polygon_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->boundary.size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_boundary_point(ifcopenshell_ifcgeom_svgfill_polygon_t* self, size_t index, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (index >= self_cpp->boundary.size()) {
    throw std::runtime_error("Index out of bounds");
}
std::vector<double> result = {self_cpp->boundary[index][0], self_cpp->boundary[index][1]};
return result;
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_inner_boundary_count(ifcopenshell_ifcgeom_svgfill_polygon_t* self, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->inner_boundaries.size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_inner_boundary_size(ifcopenshell_ifcgeom_svgfill_polygon_t* self, size_t boundary_index, size_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (boundary_index >= self_cpp->inner_boundaries.size()) {
    throw std::runtime_error("Index out of bounds");
}
return self_cpp->inner_boundaries[boundary_index].size();
        }();
        *out_result = static_cast<size_t>(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_inner_boundary_point(ifcopenshell_ifcgeom_svgfill_polygon_t* self, size_t boundary_index, size_t point_index, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
if (boundary_index >= self_cpp->inner_boundaries.size()) {
    throw std::runtime_error("Boundary index out of bounds");
}
if (point_index >= self_cpp->inner_boundaries[boundary_index].size()) {
    throw std::runtime_error("Point index out of bounds");
}
auto& pt = self_cpp->inner_boundaries[boundary_index][point_index];
std::vector<double> result = {pt[0], pt[1]};
return result;
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_svgfill_polygon_point_inside(ifcopenshell_ifcgeom_svgfill_polygon_t* self, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
std::vector<double> result = {self_cpp->point_inside[0], self_cpp->point_inside[1]};
return result;
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_function_item_evaluator_evaluation_points_range(ifcopenshell_ifcgeom_function_item_evaluator_t* self, double ustart, double uend, int32_t nsteps, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->evaluation_points(ustart, uend, static_cast<unsigned>(nsteps));
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_function_item_evaluator_evaluate_range(ifcopenshell_ifcgeom_function_item_evaluator_t* self, double ustart, double uend, int32_t nsteps, ifcopenshell_ifcgeom_taxonomy_item_t** out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return self_cpp->evaluate(ustart, uend, static_cast<unsigned>(nsteps));
        }();
        *out_result = new ifcopenshell_ifcgeom_taxonomy_item_t{generated_result};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcgeom_function_item_evaluator_evaluate_at(ifcopenshell_ifcgeom_function_item_evaluator_t* self, double u, ifcopenshell_double_list_t* out_result) {
    try {
        ifcopenshell_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
Eigen::Matrix4d mat = self_cpp->evaluate(u);
std::vector<double> result(16);
for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
        result[i * 4 + j] = mat(i, j);
return result;
        }();
        *out_result = make_double_list(generated_result);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}
