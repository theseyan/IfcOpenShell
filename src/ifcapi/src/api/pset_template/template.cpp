// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/pset_template.h"
#include "ifcapi/detail/attribute.h"
#include "guid.h"
#include "../pset/attribute_props.hpp"
#include "../pset/props.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

void write_string_or_blank(express::Base entity, const char* attr, const char* value) {
    if (value) {
        ifcapi::detail::write_string_attr(entity, attr, value);
    } else {
        ifcapi::detail::write_blank_attr(entity, attr);
    }
}

std::string pset_type_or_default(express::Base pset_template) {
    auto result = ifcapi::bindings::pset_template_pset_type(&pset_template);
    return result.empty() ? "PSET" : result;
}

const ifcapi_pset::Entry* find_prop(ifcopenshell_pset_props_t* props, const char* key) {
    if (!props) return nullptr;
    for (auto it = props->entries.rbegin(); it != props->entries.rend(); ++it) {
        if (it->key == key) return &*it;
    }
    return nullptr;
}

std::string entry_string_or(const ifcapi_pset::Entry* entry, const std::string& fallback) {
    if (!entry || entry->kind == ifcapi_pset::Kind::NONE) return fallback;
    if (entry->kind != ifcapi_pset::Kind::STRING && entry->kind != ifcapi_pset::Kind::TYPED_STRING) {
        throw std::invalid_argument("Expected a string attribute value");
    }
    return entry->s_val.empty() ? fallback : entry->s_val;
}

const ifcopenshell::simple_type* underlying_simple_type(const ifcopenshell::type_declaration* declaration) {
    auto* parameter = declaration ? declaration->declared_type() : nullptr;
    while (parameter) {
        if (auto* simple = parameter->as_simple_type()) return simple;
        auto* named = parameter->as_named_type();
        auto* nested = named ? named->declared_type() : nullptr;
        auto* nested_type = nested ? nested->as_type_declaration() : nullptr;
        if (!nested_type) return nullptr;
        parameter = nested_type->declared_type();
    }
    return nullptr;
}

size_t list_size(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
        case ifcapi_pset::Kind::NONE: return 0;
        case ifcapi_pset::Kind::BOOL_LIST: return entry.bool_list.size();
        case ifcapi_pset::Kind::INT_LIST: return entry.int_list.size();
        case ifcapi_pset::Kind::DOUBLE_LIST: return entry.dbl_list.size();
        case ifcapi_pset::Kind::STRING_LIST: return entry.str_list.size();
        default: throw std::invalid_argument("Enumerators must be a primitive sequence");
    }
}

std::vector<express::Base> make_enumerator_values(
    ifcopenshell::file* file,
    const std::string& measure_type,
    const ifcapi_pset::Entry& entry)
{
    const auto* declaration = ifcapi::detail::declaration_by_name(file, measure_type.c_str());
    const auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    const auto* simple = underlying_simple_type(type_declaration);
    if (!type_declaration || !simple) {
        throw std::invalid_argument("Invalid PrimaryMeasureType: " + measure_type);
    }

    const auto primitive = simple->declared_type();
    const bool compatible =
        (primitive == ifcopenshell::simple_type::string_type && entry.kind == ifcapi_pset::Kind::STRING_LIST)
        || (primitive == ifcopenshell::simple_type::integer_type && entry.kind == ifcapi_pset::Kind::INT_LIST)
        || ((primitive == ifcopenshell::simple_type::real_type || primitive == ifcopenshell::simple_type::number_type)
            && (entry.kind == ifcapi_pset::Kind::DOUBLE_LIST || entry.kind == ifcapi_pset::Kind::INT_LIST))
        || ((primitive == ifcopenshell::simple_type::boolean_type || primitive == ifcopenshell::simple_type::logical_type)
            && entry.kind == ifcapi_pset::Kind::BOOL_LIST);
    if (!compatible) {
        throw std::invalid_argument("Enumerator values are incompatible with " + measure_type);
    }

    if (entry.kind == ifcapi_pset::Kind::DOUBLE_LIST) {
        for (double value : entry.dbl_list) {
            if (!std::isfinite(value)) {
                throw std::invalid_argument("Enumerator values are incompatible with " + measure_type);
            }
        }
    }
    if (entry.kind == ifcapi_pset::Kind::INT_LIST) {
        for (int64_t value : entry.int_list) {
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                throw std::invalid_argument("Enumerator values are incompatible with " + measure_type);
            }
        }
    }

    std::vector<express::Base> values;
    values.reserve(list_size(entry));
    for (size_t i = 0; i < list_size(entry); ++i) {
        auto wrapped = file->create(type_declaration);
        switch (entry.kind) {
            case ifcapi_pset::Kind::BOOL_LIST:
                wrapped.set_attribute_value(0, static_cast<bool>(entry.bool_list[i]));
                break;
            case ifcapi_pset::Kind::INT_LIST:
                if (primitive == ifcopenshell::simple_type::real_type
                    || primitive == ifcopenshell::simple_type::number_type) {
                    wrapped.set_attribute_value(0, static_cast<double>(entry.int_list[i]));
                } else {
                    wrapped.set_attribute_value(0, static_cast<int>(entry.int_list[i]));
                }
                break;
            case ifcapi_pset::Kind::DOUBLE_LIST:
                wrapped.set_attribute_value(0, entry.dbl_list[i]);
                break;
            case ifcapi_pset::Kind::STRING_LIST:
                wrapped.set_attribute_value(0, entry.str_list[i]);
                break;
            default:
                break;
        }
        values.push_back(wrapped);
    }
    return values;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base pset_template_add_pset_template(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& template_type,
    const std::string& applicable_entity)
{
    const auto* decl = file->schema()->declaration_by_name("IfcPropertySetTemplate");
    auto result = file->create(decl);
    detail::write_string_attr(result, "GlobalId", guid_new());
    detail::write_string_attr(result, "Name", name);
    detail::write_string_attr(result, "TemplateType", template_type);
    detail::write_string_attr(result, "ApplicableEntity", applicable_entity);
    return result;
}

express::Base pset_template_add_prop_template(
    ifcopenshell::file* file,
    express::Base* pset_template_ptr,
    const std::string& name,
    const char* description,
    const char* template_type,
    const char* primary_measure_type)
{
    auto pset_template = detail::deref_or_empty(pset_template_ptr);
    const std::string assumed_pset_type = pset_type_or_default(pset_template);
    std::string resolved_template_type = template_type
        ? std::string(template_type)
        : (assumed_pset_type == "QTO" ? "Q_LENGTH" : "P_SINGLEVALUE");

    const char* resolved_measure_type = primary_measure_type;
    std::string default_measure_type;
    if (assumed_pset_type == "PSET" && !resolved_measure_type) {
        default_measure_type = "IfcLabel";
        resolved_measure_type = default_measure_type.c_str();
    } else if (assumed_pset_type != "PSET") {
        resolved_measure_type = nullptr;
    }

    const auto* decl = file->schema()->declaration_by_name("IfcSimplePropertyTemplate");
    auto result = file->create(decl);
    detail::write_string_attr(result, "GlobalId", guid_new());
    detail::write_string_attr(result, "Name", name);
    write_string_or_blank(result, "Description", description);
    write_string_or_blank(result, "PrimaryMeasureType", resolved_measure_type);
    detail::write_string_attr(result, "TemplateType", resolved_template_type);
    detail::write_string_attr(result, "AccessState", "READWRITE");
    detail::write_blank_attr(result, "Enumerators");

    auto templates = detail::read_ref_aggregate(pset_template, "HasPropertyTemplates");
    templates.push_back(result);
    std::sort(templates.begin(), templates.end(), [](express::Base lhs, express::Base rhs) {
        return detail::read_string_attr(lhs, "Name") < detail::read_string_attr(rhs, "Name");
    });
    detail::write_ref_aggregate(pset_template, "HasPropertyTemplates", templates);
    return result;
}

void pset_template_edit_prop_template(
    ifcopenshell::file* file,
    const PsetTemplateEditPropTemplateOptions& options)
{
    if (!file || !options.prop_template || !options.prop_template.declaration().is("IfcSimplePropertyTemplate")
        || !detail::exists_in_file(file, options.prop_template)) {
        throw std::invalid_argument("Invalid IfcSimplePropertyTemplate");
    }

    const auto* enumerator_entry = find_prop(options.attributes, "Enumerators");
    const size_t enumerator_count = enumerator_entry ? list_size(*enumerator_entry) : 0;
    if (enumerator_entry && enumerator_count > 0) {
        const auto existing_name = detail::read_string_attr(options.prop_template, "Name");
        const auto existing_measure_type = detail::read_string_attr(options.prop_template, "PrimaryMeasureType");
        const auto name = entry_string_or(find_prop(options.attributes, "Name"),
            existing_name.empty() ? std::string("Unnamed") : existing_name);
        const auto measure_type = entry_string_or(find_prop(options.attributes, "PrimaryMeasureType"),
            existing_measure_type.empty() ? std::string("IfcLabel") : existing_measure_type);
        auto values = make_enumerator_values(file, measure_type, *enumerator_entry);

        auto enumeration = detail::read_ref_attr(options.prop_template, "Enumerators");
        if (enumeration) {
            if (!enumeration.declaration().is("IfcPropertyEnumeration") || !detail::exists_in_file(file, enumeration)) {
                throw std::invalid_argument("Invalid IfcPropertyEnumeration");
            }
        } else {
            const auto* declaration = detail::declaration_by_name(file, "IfcPropertyEnumeration");
            if (!declaration) throw std::invalid_argument("IfcPropertyEnumeration is not supported by this schema");
            enumeration = file->create(declaration);
        }
        detail::write_string_attr(enumeration, "Name", name);
        detail::write_ref_aggregate(enumeration, "EnumerationValues", values);
        detail::write_ref_attr(options.prop_template, "Enumerators", enumeration);
    }

    if (options.attributes) {
        for (const auto& entry : options.attributes->entries) {
            if (entry.key != "Enumerators") detail::apply_attribute_prop(options.prop_template, entry);
        }
    }
}

void pset_template_remove_pset_template(express::Base* pset_template)
{
    entity_remove_deep(pset_template);
}

void pset_template_remove_prop_template(
    ifcopenshell::file* file,
    express::Base* prop_template_ptr)
{
    auto prop_template = detail::deref_or_empty(prop_template_ptr);
    if (file && prop_template && prop_template.id() > 0) {
        auto inverses = file->instances_by_reference(static_cast<int>(prop_template.id()));
        for (auto inverse : inverses) {
            auto templates = detail::read_ref_aggregate(inverse, "HasPropertyTemplates");
            if (templates.size() <= 1) continue;
            auto it = std::find(templates.begin(), templates.end(), prop_template);
            if (it != templates.end()) {
                templates.erase(it);
                detail::write_ref_aggregate(inverse, "HasPropertyTemplates", templates);
            }
        }
    }
    entity_remove_deep(&prop_template);
}

} // namespace bindings
} // namespace ifcapi
