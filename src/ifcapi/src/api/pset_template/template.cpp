// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/pset_template.h"
#include "ifcapi/detail/attribute.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
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

void pset_template_remove_pset_template(express::Base* pset_template)
{
    entity_remove_deep2(pset_template);
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
    entity_remove_deep2(&prop_template);
}

} // namespace bindings
} // namespace ifcapi
