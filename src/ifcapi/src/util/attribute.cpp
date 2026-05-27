// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/attribute.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/relationship.h"
#include "../api/pset/attribute_props.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <stdexcept>

namespace {

const IfcParse::parameter_type* leaf_type(const IfcParse::parameter_type* pt) {
    while (pt) {
        if (auto* nt = pt->as_named_type()) {
            auto* decl = nt->declared_type();
            if (!decl) return pt;
            if (auto* td = decl->as_type_declaration()) {
                pt = td->declared_type();
                continue;
            }
            return pt;
        }
        return pt;
    }
    return nullptr;
}

const char* primitive_type_for(const IfcParse::parameter_type* pt) {
    pt = leaf_type(pt);
    if (!pt) return "unknown";

    if (pt->as_aggregation_type()) return "aggregate";

    if (auto* nt = pt->as_named_type()) {
        auto* decl = nt->declared_type();
        if (!decl) return "unknown";
        if (decl->as_entity()) return "entity";
        if (decl->as_enumeration_type()) return "enum";
        if (decl->as_select_type()) return "select";
        return "unknown";
    }

    if (auto* st = pt->as_simple_type()) {
        switch (st->declared_type()) {
            case IfcParse::simple_type::string_type:  return "string";
            case IfcParse::simple_type::real_type:    return "float";
            case IfcParse::simple_type::number_type:
            case IfcParse::simple_type::integer_type: return "integer";
            case IfcParse::simple_type::boolean_type: return "boolean";
            case IfcParse::simple_type::logical_type: return "enum";
            case IfcParse::simple_type::binary_type:  return "binary";
            default:                                  return "unknown";
        }
    }
    return "unknown";
}

const IfcParse::enumeration_type* enum_type_for(const IfcParse::attribute* attr) {
    if (!attr) return nullptr;
    auto* pt = leaf_type(attr->type_of_attribute());
    auto* nt = pt ? pt->as_named_type() : nullptr;
    auto* decl = nt ? nt->declared_type() : nullptr;
    return decl ? decl->as_enumeration_type() : nullptr;
}

bool has_attr(IfcUtil::IfcBaseClass* entity, const char* name) {
    return ifcapi::detail::attr_index_of(entity, name) >= 0;
}

bool optional_truthy(const ifcapi::detail::OptionalString& value) {
    return value.has_value && !value.value.empty();
}

void unset_attr(IfcUtil::IfcBaseClass* entity, const char* name) {
    int idx = ifcapi::detail::attr_index_of(entity, name);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

void sync_predefined_type(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* product) {
    if (!file || !product || !has_attr(product, "PredefinedType")) {
        return;
    }
    auto predefined_type = ifcapi::detail::read_optional_string_attr(product, "PredefinedType");
    if (has_attr(product, "ElementType")) {
        auto element_type = ifcapi::detail::read_optional_string_attr(product, "ElementType");
        if (!element_type.has_value && predefined_type.has_value && predefined_type.value == "USERDEFINED") {
            ifcapi::detail::write_string_attr(product, "PredefinedType", "NOTDEFINED");
        } else if (optional_truthy(element_type) && (!predefined_type.has_value || predefined_type.value != "USERDEFINED")) {
            ifcapi::detail::write_string_attr(product, "PredefinedType", "USERDEFINED");
        }
        return;
    }
    if (!has_attr(product, "ObjectType")) {
        return;
    }
    auto object_type = ifcapi::detail::read_optional_string_attr(product, "ObjectType");
    auto* relating_type = ifcapi::bindings::element_get_type(product);
    auto type_predefined = relating_type
        ? ifcapi::detail::read_optional_string_attr(relating_type, "PredefinedType")
        : ifcapi::detail::OptionalString{};
    if (type_predefined.has_value && type_predefined.value != "NOTDEFINED") {
        unset_attr(product, "ObjectType");
        unset_attr(product, "PredefinedType");
    } else if (!object_type.has_value && predefined_type.has_value && predefined_type.value == "USERDEFINED") {
        ifcapi::detail::write_string_attr(product, "PredefinedType", "NOTDEFINED");
    } else if (optional_truthy(object_type) && (!predefined_type.has_value || predefined_type.value != "USERDEFINED")) {
        ifcapi::detail::write_string_attr(product, "PredefinedType", "USERDEFINED");
    }
}

}  // namespace

namespace ifcapi {
namespace bindings {

const char* attribute_get_primitive_type(const IfcParse::attribute* attribute) {
    if (!attribute) return "unknown";
    return primitive_type_for(attribute->type_of_attribute());
}

std::vector<std::string> attribute_get_enum_items(const IfcParse::attribute* attribute) {
    std::vector<std::string> result;
    auto* en = enum_type_for(attribute);
    if (!en) return result;
    const auto& items = en->enumeration_items();
    result.reserve(items.size());
    for (const auto& item : items) result.push_back(item);
    return result;
}

void attribute_edit_attributes(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    ifcopenshell_pset_props_t* attributes,
    bool should_sync_predefined_type,
    bool should_update_owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        if (!product) {
            throw std::runtime_error("attribute_edit_attributes requires a product");
        }
        ifcapi::detail::apply_attribute_props(product, attributes);
        if (should_sync_predefined_type) {
            sync_predefined_type(file, product);
        }
        if (should_update_owner_history && has_attr(product, "OwnerHistory")) {
            ifcapi::detail::update_owner_history(file, product, user, application);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
