// This file was generated with the assistance of an AI coding tool.

#include "ifcparse_api.h"

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

#include "ifc_parse_api.h"
#include "IfcParse.h"
#include "IfcSIPrefix.h"
#include "IfcLogger.h"
#include "Header_section_schema.h"
#include "IfcSpfHeader.h"
#include "IfcFile.h"
#include "file_open_status.h"
#include "IfcBaseClass.h"
#include "IfcSchema.h"
#include "aggregate_of_instance.h"
#include "IfcEntityInstanceData.h"
#include "utils.h"

namespace {
thread_local std::string g_last_error;
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

void set_last_error(const std::string& message) {
    g_last_error = message;
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

ifcopenshell_string_list_t make_string_list(const std::vector<std::string>& values) {
    auto* items = new ifcopenshell_string_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_string(values[i]);
    }
    return ifcopenshell_string_list_t{items, values.size()};
}

std::vector<std::string> to_cpp_string_list(const ifcopenshell_string_list_t* value) {
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

ifcopenshell_bool_list_t make_bool_list(const std::vector<bool>& values) {
    bool* items = values.empty() ? nullptr : new bool[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_bool_list_t{items, values.size()};
}

std::vector<bool> to_cpp_bool_list(const ifcopenshell_bool_list_t* value) {
    validate_list_items("bool_list", value->items, value->size);
    std::vector<bool> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(value->items[i]);
    }
    return result;
}

ifcopenshell_int32_list_t make_int32_list(const std::vector<int>& values) {
    int32_t* items = values.empty() ? nullptr : new int32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = static_cast<int32_t>(values[i]);
    }
    return ifcopenshell_int32_list_t{items, values.size()};
}

std::vector<int> to_cpp_int32_list(const ifcopenshell_int32_list_t* value) {
    validate_list_items("int32_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<int>(value->items, value->items + value->size);
}

ifcopenshell_uint32_list_t make_uint32_list(const std::vector<unsigned int>& values) {
    uint32_t* items = values.empty() ? nullptr : new uint32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = static_cast<uint32_t>(values[i]);
    }
    return ifcopenshell_uint32_list_t{items, values.size()};
}

std::vector<unsigned int> to_cpp_uint32_list(const ifcopenshell_uint32_list_t* value) {
    validate_list_items("uint32_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<unsigned int>(value->items, value->items + value->size);
}

ifcopenshell_int32_list_list_t make_int32_list_list(const std::vector<std::vector<int>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_int32_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_int32_list(values[i]);
    }
    return ifcopenshell_int32_list_list_t{items, values.size()};
}

std::vector<std::vector<int>> to_cpp_int32_list_list(const ifcopenshell_int32_list_list_t* value) {
    validate_list_items("int32_list_list", value->items, value->size);
    std::vector<std::vector<int>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(to_cpp_int32_list(&value->items[i]));
    }
    return result;
}

ifcopenshell_double_list_t make_double_list(const std::vector<double>& values) {
    double* items = values.empty() ? nullptr : new double[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = values[i];
    }
    return ifcopenshell_double_list_t{items, values.size()};
}

std::vector<double> to_cpp_double_list(const ifcopenshell_double_list_t* value) {
    validate_list_items("double_list", value->items, value->size);
    if (value->size == 0) {
        return {};
    }
    return std::vector<double>(value->items, value->items + value->size);
}

ifcopenshell_double_list_list_t make_double_list_list(const std::vector<std::vector<double>>& values) {
    auto* items = values.empty() ? nullptr : new ifcopenshell_double_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = make_double_list(values[i]);
    }
    return ifcopenshell_double_list_list_t{items, values.size()};
}

std::vector<std::vector<double>> to_cpp_double_list_list(const ifcopenshell_double_list_list_t* value) {
    validate_list_items("double_list_list", value->items, value->size);
    std::vector<std::vector<double>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {
        result.push_back(to_cpp_double_list(&value->items[i]));
    }
    return result;
}

template <typename T>
void set_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index, const T& value) {
    instance->set_attribute_value(index, value);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {
        entity->populate_derived();
    }
}

void unset_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index) {
    instance->unset_attribute_value(index);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {
        entity->populate_derived();
    }
}

void set_instance_attribute_from_attribute_value(IfcUtil::IfcBaseClass* instance, size_t index, const AttributeValue& value) {
    value.apply_visitor([&](const auto& inner) -> void {
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, Derived>) {
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        } else {
            set_instance_argument(instance, index, inner);
        }
    });
}
} // namespace

struct ifcopenshell_ifc_file_t {
    IfcParse::IfcFile* ptr;
    bool owned;
};

struct ifcopenshell_ifc_instance_streamer_t {
    IfcParse::InstanceStreamer* ptr;
    bool owned;
};

struct ifcopenshell_ifc_instance_t {
    IfcUtil::IfcBaseClass* ptr;
    bool owned;
};

struct ifcopenshell_ifc_header_t {
    IfcParse::IfcSpfHeader* ptr;
    bool owned;
};

struct ifcopenshell_ifc_file_description_t {
    Header_section_schema::file_description* ptr;
    bool owned;
};

struct ifcopenshell_ifc_file_name_t {
    Header_section_schema::file_name* ptr;
    bool owned;
};

struct ifcopenshell_ifc_file_schema_t {
    Header_section_schema::file_schema* ptr;
    bool owned;
};

struct ifcopenshell_ifc_declaration_t {
    IfcParse::declaration* ptr;
    bool owned;
};

struct ifcopenshell_ifc_type_declaration_t {
    IfcParse::type_declaration* ptr;
    bool owned;
};

struct ifcopenshell_ifc_select_type_t {
    IfcParse::select_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_schema_t {
    IfcParse::schema_definition* ptr;
    bool owned;
};

struct ifcopenshell_ifc_enumeration_t {
    IfcParse::enumeration_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_parameter_type_t {
    IfcParse::parameter_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_named_type_t {
    IfcParse::named_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_simple_type_t {
    IfcParse::simple_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_aggregation_type_t {
    IfcParse::aggregation_type* ptr;
    bool owned;
};

struct ifcopenshell_ifc_entity_t {
    IfcParse::entity* ptr;
    bool owned;
};

struct ifcopenshell_ifc_attribute_t {
    IfcParse::attribute* ptr;
    bool owned;
};

struct ifcopenshell_ifc_inverse_attribute_t {
    IfcParse::inverse_attribute* ptr;
    bool owned;
};

struct ifcopenshell_ifcparse_attribute_value_t {
    AttributeValue value;
};

struct ifcopenshell_ifcparse_instance_list_t {
    aggregate_of_instance::ptr value;
};

static ifcopenshell_ifc_declaration_list_t make_ifc_declaration_list(const std::vector<const IfcParse::declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_declaration_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(values[i]), false};
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

static ifcopenshell_ifc_entity_list_t make_ifc_entity_list(const std::vector<const IfcParse::entity*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_entity_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_entity_t{const_cast<IfcParse::entity*>(values[i]), false};
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

static ifcopenshell_ifc_enumeration_list_t make_ifc_enumeration_list(const std::vector<const IfcParse::enumeration_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_enumeration_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_enumeration_t{const_cast<IfcParse::enumeration_type*>(values[i]), false};
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

static ifcopenshell_ifc_select_type_list_t make_ifc_select_type_list(const std::vector<const IfcParse::select_type*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_select_type_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_select_type_t{const_cast<IfcParse::select_type*>(values[i]), false};
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

static ifcopenshell_ifc_type_declaration_list_t make_ifc_type_declaration_list(const std::vector<const IfcParse::type_declaration*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_type_declaration_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_type_declaration_t{const_cast<IfcParse::type_declaration*>(values[i]), false};
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

static ifcopenshell_ifc_attribute_list_t make_ifc_attribute_list(const std::vector<const IfcParse::attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_attribute_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_attribute_t{const_cast<IfcParse::attribute*>(values[i]), false};
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

static ifcopenshell_ifc_inverse_attribute_list_t make_ifc_inverse_attribute_list(const std::vector<const IfcParse::inverse_attribute*>& values) {
    auto** items = values.empty() ? nullptr : new ifcopenshell_ifc_inverse_attribute_t*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {
        items[i] = new ifcopenshell_ifc_inverse_attribute_t{const_cast<IfcParse::inverse_attribute*>(values[i]), false};
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

void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
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

void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
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

void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value) {
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

void ifcopenshell_ifcparse_clear_error(void) {
    g_last_error.clear();
}

const char* ifcopenshell_ifcparse_last_error_message(void) {
    return g_last_error.c_str();
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
void ifcopenshell_ifc_declaration_list_destroy(ifcopenshell_ifc_declaration_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_ifc_declaration_destroy(value->items[i]);
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
        ifcopenshell_ifc_entity_destroy(value->items[i]);
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
        ifcopenshell_ifc_enumeration_destroy(value->items[i]);
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
        ifcopenshell_ifc_select_type_destroy(value->items[i]);
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
        ifcopenshell_ifc_type_declaration_destroy(value->items[i]);
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
        ifcopenshell_ifc_attribute_destroy(value->items[i]);
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
        ifcopenshell_ifc_inverse_attribute_destroy(value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

bool ifcopenshell_ifcparse_clear_schemas(void) {
    try {
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_register_schema(ifcopenshell_ifc_schema_t* arg_0) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{IfcParse::traverse(instance_cpp, max_level)};
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{IfcParse::traverse_breadth_first(instance_cpp, max_level)};
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
return new IfcParse::IfcFile(path, IfcParse::FT_AUTODETECT, readonly);
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_read_memory(const char* data, ifcopenshell_ifc_file_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (data == nullptr) { throw std::runtime_error("Parameter \"data\" must not be null"); }
    std::string data_cpp(data);
        auto generated_result = [&]() {
char* copied_data = new char[data_cpp.size()];
memcpy(copied_data, data_cpp.c_str(), data_cpp.size());
return new IfcParse::IfcFile((void*)copied_data, static_cast<int>(data_cpp.size()));
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

bool ifcopenshell_ifcparse_stream_from_string(const char* data, ifcopenshell_ifc_instance_streamer_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (data == nullptr) { throw std::runtime_error("Parameter \"data\" must not be null"); }
    std::string data_cpp(data);
        auto generated_result = [&]() {
char* copied_data = new char[data_cpp.size()];
memcpy(copied_data, data_cpp.c_str(), data_cpp.size());
return new IfcParse::InstanceStreamer((void*)copied_data, static_cast<int>(data_cpp.size()));
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_guess_file_type(const char* path, int32_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
return static_cast<int>(IfcParse::guess_file_type(path_cpp));
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

bool ifcopenshell_ifcparse_set_feature(const char* name, bool value) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_set_log_format_json(void) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_file_create(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t* decl, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type == nullptr || type->ptr == nullptr) { throw std::runtime_error("Handle parameter \"type\" is invalid"); }
    auto type_cpp = type->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->getInverse(instance_id, type_cpp, attribute_index)};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (entities == nullptr) { throw std::runtime_error("Handle parameter \"entities\" is invalid"); }
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (entity == nullptr || entity->ptr == nullptr) { throw std::runtime_error("Handle parameter \"entity\" is invalid"); }
    auto entity_cpp = entity->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->addEntity(entity_cpp, id), false};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = static_cast<size_t>(self_cpp->getTotalInverses(instance_id));
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_int32_list(self_cpp->get_inverse_indices(instance_id));
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_file_by_guid(ifcopenshell_ifc_file_t* self, const char* guid, ifcopenshell_ifc_instance_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_instance_t{self_cpp->instance_by_id(id), false};
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->instances_by_reference(id)};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->traverse(instance_cpp, max_level)};
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (instance == nullptr || instance->ptr == nullptr) { throw std::runtime_error("Handle parameter \"instance\" is invalid"); }
    auto instance_cpp = instance->ptr;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->traverse_breadth_first(instance_cpp, max_level)};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifcparse_attribute_value_t{self_cpp->get_attribute_value(index)};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_declaration_t{const_cast<IfcParse::declaration*>(self_cpp->declaration_by_name(name)), false};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = make_string(self_cpp->lookup_enum_value(i));
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_aggregation_type_as_aggregation_type(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        *out_result = new ifcopenshell_ifc_attribute_t{const_cast<IfcParse::attribute*>(self_cpp->attribute_by_index(index)), false};
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_instance_list_contains(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifc_instance_t* arg_0, bool* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        *out_result = self_cpp->contains(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_instance_list_remove(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifc_instance_t* arg_0) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
    if (arg_0 == nullptr || arg_0->ptr == nullptr) { throw std::runtime_error("Handle parameter \"arg_0\" is invalid"); }
    auto arg_0_cpp = arg_0->ptr;
        self_cpp->remove(arg_0_cpp);
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifcparse_instance_list_reserve(ifcopenshell_ifcparse_instance_list_t* self, size_t capacity) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
        self_cpp->reserve(capacity);
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_instance_list_unique(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifcparse_instance_list_t** out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || !self->value) { throw std::runtime_error("Receiver handle is invalid"); }
    auto self_cpp = self->value;
        *out_result = new ifcopenshell_ifcparse_instance_list_t{self_cpp->unique()};
        return true;
    } catch (const std::exception& e) {
        set_last_error(e.what());
        return false;
    } catch (...) {
        set_last_error("Unknown C++ exception");
        return false;
    }
}

bool ifcopenshell_ifc_file_initialize(ifcopenshell_ifc_file_t* self, const char* path, int32_t file_type, bool readonly, bool* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (path == nullptr) { throw std::runtime_error("Parameter \"path\" must not be null"); }
    std::string path_cpp(path);
        auto generated_result = [&]() {
return self_cpp->initialize(path_cpp, static_cast<IfcParse::filetype>(file_type), readonly);
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

bool ifcopenshell_ifc_file_write(ifcopenshell_ifc_file_t* self, const char* path) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_file_schema_name(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (name == nullptr) { throw std::runtime_error("Parameter \"name\" must not be null"); }
    std::string name_cpp(name);
    if (value == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_instance_get_argument_type(ifcopenshell_ifc_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_instance_unset_argument(ifcopenshell_ifc_instance_t* self, size_t index) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (value == nullptr) { throw std::runtime_error("Handle parameter \"value\" is invalid"); }
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_attribute_value_is_null(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifcparse_attribute_value_as_double_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_header_write(ifcopenshell_ifc_header_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_simple_type_declared_type(ifcopenshell_ifc_simple_type_t* self, int32_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp->declared_type());
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

bool ifcopenshell_ifc_simple_type_kind(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_string_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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

bool ifcopenshell_ifc_instance_streamer_bypass_types(ifcopenshell_ifc_instance_streamer_t* self, const ifcopenshell_string_list_t* type_names) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
    if (type_names == nullptr) { throw std::runtime_error("Parameter \"type_names\" must not be null"); }
    auto type_names_cpp = to_cpp_string_list(type_names);
        [&]() {
std::set<std::string> names(type_names_cpp.begin(), type_names_cpp.end());
self_cpp->bypassTypes(names);
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

bool ifcopenshell_ifc_instance_streamer_status(ifcopenshell_ifc_instance_streamer_t* self, int32_t* out_result) {
    try {
        ifcopenshell_ifcparse_clear_error();
    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }
    if (self == nullptr || self->ptr == nullptr) { throw std::runtime_error("Receiver handle is invalid"); }
    auto* self_cpp = self->ptr;
        auto generated_result = [&]() {
return static_cast<int>(self_cpp->status().value());
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
        ifcopenshell_ifcparse_clear_error();
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
