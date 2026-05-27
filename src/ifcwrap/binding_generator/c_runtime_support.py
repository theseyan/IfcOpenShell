# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations


def _render_cpp_support_runtime() -> str:
    """Render C++ runtime helpers emitted before generated call bodies.

    This intentionally remains an f-string even though it has no substitutions:
    the generated C++ contains many literal braces, and keeping the original
    f-string escaping avoids hand-unescaping JSON/C++ brace literals.
    """
    return f"""// Error reporting state. Defined in the named ifcopenshell::capi namespace so
// that external translation units can participate via the internal header.
namespace ifcopenshell {{
namespace capi {{
thread_local std::string g_last_error;
thread_local int g_last_error_kind = 0;

void set_last_error(const std::string& message) {{
    g_last_error_kind = 1;
    g_last_error = message;
}}

void set_last_error(int kind, const std::string& message) {{
    g_last_error_kind = kind;
    g_last_error = message;
}}
}} // namespace capi
}} // namespace ifcopenshell

namespace {{
using ifcopenshell::capi::g_last_error;
using ifcopenshell::capi::set_last_error;

bool feature_use_attribute_value_derived = false;
std::stringstream ifcopenshell_log_stream;
bool g_log_stream_initialized = false;

void ensure_log_stream_initialized() {{
    if (!g_log_stream_initialized) {{
        logger::set_output(nullptr, &ifcopenshell_log_stream);
        g_log_stream_initialized = true;
    }}
}}

ifcopenshell::argument_type helper_fn_attribute_type(const express::Base* inst, unsigned index) {{
    const ifcopenshell::parameter_type* parameter_type = nullptr;
    if (inst->declaration().as_entity()) {{
        parameter_type = inst->declaration().as_entity()->attribute_by_index(index)->type_of_attribute();
        if (inst->declaration().as_entity()->derived()[index]) {{
            return ifcopenshell::Argument_DERIVED;
        }}
    }} else if (inst->declaration().as_type_declaration() && index == 0) {{
        parameter_type = inst->declaration().as_type_declaration()->declared_type();
    }} else if (inst->declaration().as_enumeration_type() && index == 0) {{
        return ifcopenshell::Argument_STRING;
    }}

    if (parameter_type == nullptr) {{
        return ifcopenshell::Argument_UNKNOWN;
    }}
    return ifcopenshell::from_parameter_type(parameter_type);
}}

void validate_list_items(const char* name, const void* items, size_t size) {{
    if (size > 0 && items == nullptr) {{
        throw std::runtime_error(std::string("Parameter '") + name + "' has a null items pointer");
    }}
}}

template <typename T>
struct capi_is_std_vector : std::false_type {{}};

template <typename T, typename Alloc>
struct capi_is_std_vector<std::vector<T, Alloc>> : std::true_type {{}};

template <typename T>
inline constexpr bool capi_is_std_vector_v = capi_is_std_vector<T>::value;

std::string json_escape_string(const std::string& value) {{
    std::ostringstream out;
    for (unsigned char ch : value) {{
        switch (ch) {{
        case '\\\\': out << "\\\\\\\\"; break;
        case '"': out << "\\\\\\""; break;
        case '\\b': out << "\\\\b"; break;
        case '\\f': out << "\\\\f"; break;
        case '\\n': out << "\\\\n"; break;
        case '\\r': out << "\\\\r"; break;
        case '\\t': out << "\\\\t"; break;
        default:
            if (ch < 0x20) {{
                out << "\\\\u"
                    << "00"
                    << "0123456789abcdef"[ch >> 4]
                    << "0123456789abcdef"[ch & 0x0f];
            }} else {{
                out << static_cast<char>(ch);
            }}
        }}
    }}
    return out.str();
}}

std::string json_quote(const std::string& value) {{
    return std::string(1, '"') + json_escape_string(value) + '"';
}}

std::string instance_to_info_json_string(const express::Base& instance, bool include_identifier);

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier);

template <typename T>
std::string vector_to_json_string(const std::vector<T>& values, bool include_identifier) {{
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {{
        if (i != 0) {{
            out << ",";
        }}
        out << value_to_json_string(values[i], include_identifier);
    }}
    out << "]";
    return out.str();
}}

std::string reference_or_simple_type_to_json_string(const ifcopenshell::reference_or_simple_type& value, bool include_identifier) {{
    if (auto* instance = std::get_if<express::Base>(&value)) {{
        return *instance ? instance_to_info_json_string(*instance, include_identifier) : "null";
    }}
    auto reference = std::get<ifcopenshell::instance_reference>(value);
    return std::string(R"({{"ref":)") + std::to_string(reference.v) + "}}";
}}

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier) {{
    if constexpr (capi_is_std_vector_v<T>) {{
        return vector_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, std::string>) {{
        return json_quote(value);
    }} else if constexpr (std::is_same_v<T, const char*>) {{
        return value ? json_quote(value) : "null";
    }} else if constexpr (std::is_same_v<T, bool>) {{
        return value ? "true" : "false";
    }} else if constexpr (std::is_same_v<T, boost::logic::tribool>) {{
        if (boost::logic::indeterminate(value)) {{
            return "null";
        }}
        return value ? "true" : "false";
    }} else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {{
        std::ostringstream out;
        out << value;
        return out.str();
    }} else if constexpr (std::is_same_v<T, enumeration_reference>) {{
        return json_quote(std::string(value.value()));
    }} else if constexpr (std::is_same_v<T, ifcopenshell::reference_or_simple_type>) {{
        return reference_or_simple_type_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, express::Base>) {{
        return value ? instance_to_info_json_string(value, include_identifier) : "null";
    }} else if constexpr (
        std::is_same_v<T, empty_aggregate_t> ||
        std::is_same_v<T, empty_aggregate_of_aggregate_t> ||
        std::is_same_v<T, blank> ||
        std::is_same_v<T, derived>) {{
        return "null";
    }} else {{
        return json_quote("<unsupported>");
    }}
}}

std::string attribute_value_to_json_string(const attribute_value& value, bool include_identifier) {{
    return value.apply_visitor([include_identifier](const auto& inner) -> std::string {{
        return value_to_json_string(inner, include_identifier);
    }});
}}

std::string instance_to_info_json_string(const express::Base& instance, bool include_identifier) {{
    if (!instance) {{
        return "null";
    }}
    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    if (instance.declaration().as_entity()) {{
        const auto attributes = instance.declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {{
            emit_field(attributes[i]->name(), attribute_value_to_json_string(instance.get_attribute_value(i), include_identifier));
        }}
        if (include_identifier) {{
            emit_field("id", std::to_string(instance.id()));
        }}
    }} else {{
        emit_field("wrappedValue", attribute_value_to_json_string(instance.get_attribute_value(0), include_identifier));
    }}

    emit_field("type", json_quote(instance.declaration().name()));
    out << "}}";
    return out.str();
}}

std::string unresolved_reference_variant_to_json_string(
    const std::variant<
        ifcopenshell::reference_or_simple_type,
        std::vector<ifcopenshell::reference_or_simple_type>,
        std::vector<std::vector<ifcopenshell::reference_or_simple_type>>>& value,
    bool include_identifier) {{
    return std::visit(
        [include_identifier](const auto& inner) -> std::string {{
            return value_to_json_string(inner, include_identifier);
        }},
        value);
}}

std::string unresolved_references_to_json_string(
    const ifcopenshell::unresolved_references& references,
    const ifcopenshell::declaration* declaration) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : references) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        const auto& mutable_value = entry.first;
        out << "{{"
            << R"("entity_name":)" << mutable_value.name_
            << R"(,"attribute_index":)" << static_cast<int>(mutable_value.index_);
        if (declaration && declaration->as_entity() && mutable_value.index_ < declaration->as_entity()->attribute_count()) {{
            out << R"(,"attribute_name":)" << json_quote(declaration->as_entity()->attribute_by_index(mutable_value.index_)->name());
        }}
        out << R"(,"value":)" << unresolved_reference_variant_to_json_string(entry.second, true) << "}}";
    }}
    out << "]";
    return out.str();
}}

std::string inverses_to_json_string(const ifcopenshell::impl::in_memory_file_storage::entities_by_ref_t& inverses) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : inverses) {{
        for (const auto& inverse : entry.second) {{
            if (!first) {{
                out << ",";
            }}
            first = false;
            out << "{{"
                << R"("instance_id":)" << entry.first
                << R"(,"instance_type":)" << std::get<0>(inverse.first)
                << R"(,"attribute_index":)" << std::get<1>(inverse.first)
                << R"(,"referencing_ids":)" << vector_to_json_string(inverse.second, true)
                << "}}";
        }}
    }}
    out << "]";
    return out.str();
}}

std::string instance_stream_read_instance_json(ifcopenshell::instance_streamer<>* streamer, bool type_as_declaration_instance) {{
    if (!(*streamer)) {{
        return "null";
    }}
    auto inst = streamer->read_instance();
    if (!inst) {{
        return "null";
    }}

    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    emit_field("id", std::to_string(std::get<0>(*inst)));
    if (type_as_declaration_instance) {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }} else {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }}

    const auto* declaration = std::get<1>(*inst);
    const auto& data = std::get<2>(*inst);
    if (declaration->as_entity()) {{
        for (size_t i = 0; i < declaration->as_entity()->attribute_count(); ++i) {{
            emit_field(
                declaration->as_entity()->attribute_by_index(i)->name(),
                attribute_value_to_json_string(data->get_attribute_value(i), true));
        }}
    }}

    for (const auto& reference : streamer->references()) {{
        std::string key = std::to_string(reference.first.index_);
        if (declaration->as_entity() && reference.first.index_ < declaration->as_entity()->attribute_count()) {{
            key = declaration->as_entity()->attribute_by_index(reference.first.index_)->name();
        }}
        emit_field(key, unresolved_reference_variant_to_json_string(reference.second, true));
    }}

    streamer->references().clear();
    streamer->inverses().clear();
    out << "}}";
    return out.str();
}}

ifcopenshell_string_t make_string(const std::string& value) {{
    char* data = new char[value.size() + 1];
    std::memcpy(data, value.c_str(), value.size() + 1);
    return ifcopenshell_string_t{{data, value.size(), true}};
}}

ifcopenshell_string_t make_static_string(const char* value) {{
    if (value == nullptr) {{
        return ifcopenshell_string_t{{nullptr, 0, false}};
    }}
    return ifcopenshell_string_t{{const_cast<char*>(value), std::strlen(value), false}};
}}

template <typename T>
void set_instance_argument(express::Base* instance, size_t index, const T& value) {{
    instance->set_attribute_value(index, value);
}}

void set_instance_argument(express::Base* instance, size_t index, express::Base* value) {{
    if (value == nullptr) {{
        instance->unset_attribute_value(index);
    }} else {{
        instance->set_attribute_value(index, *value);
    }}
}}

void set_instance_argument(express::Base* instance, size_t index, std::vector<express::Base>* value) {{
    if (value == nullptr) {{
        instance->unset_attribute_value(index);
    }} else {{
        instance->set_attribute_value(index, *value);
    }}
}}

void unset_instance_argument(express::Base* instance, size_t index) {{
    instance->unset_attribute_value(index);
}}

void set_instance_attribute_from_attribute_value(express::Base* instance, size_t index, const attribute_value& value) {{
    value.apply_visitor([&](const auto& inner) -> void {{
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, derived>) {{
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        }} else if constexpr (std::is_same_v<T, empty_aggregate_t> ||
                              std::is_same_v<T, empty_aggregate_of_aggregate_t>) {{
            // Empty-aggregate sentinels arise when reading absent aggregate
            // values; assigning them is equivalent to unsetting the attribute.
            unset_instance_argument(instance, index);
        }} else {{
            set_instance_argument(instance, index, inner);
        }}
    }});
}}
}} // namespace"""
