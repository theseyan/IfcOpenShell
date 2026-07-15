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

const ifcopenshell::parameter_type* leaf_type(const ifcopenshell::parameter_type* pt) {
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

const char* primitive_type_for(const ifcopenshell::parameter_type* pt) {
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
            case ifcopenshell::simple_type::string_type:  return "string";
            case ifcopenshell::simple_type::real_type:    return "float";
            case ifcopenshell::simple_type::number_type:
            case ifcopenshell::simple_type::integer_type: return "integer";
            case ifcopenshell::simple_type::boolean_type: return "boolean";
            case ifcopenshell::simple_type::logical_type: return "enum";
            case ifcopenshell::simple_type::binary_type:  return "binary";
            default:                                  return "unknown";
        }
    }
    return "unknown";
}

const ifcopenshell::enumeration_type* enum_type_for(const ifcopenshell::attribute* attr) {
    if (!attr) return nullptr;
    auto* pt = leaf_type(attr->type_of_attribute());
    auto* nt = pt ? pt->as_named_type() : nullptr;
    auto* decl = nt ? nt->declared_type() : nullptr;
    return decl ? decl->as_enumeration_type() : nullptr;
}

bool has_attr(express::Base entity, const char* name) {
    return ifcapi::detail::attr_index_of(entity, name) >= 0;
}

bool optional_truthy(const ifcapi::detail::OptionalString& value) {
    return value.has_value && !value.value.empty();
}

void unset_attr(express::Base entity, const char* name) {
    int idx = ifcapi::detail::attr_index_of(entity, name);
    if (idx >= 0) {
        entity.set_attribute_value(static_cast<size_t>(idx), blank{});
    }
}

void sync_predefined_type(ifcopenshell::file* file, express::Base product) {
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
    auto relating_type = ifcapi::bindings::element_get_type(&product);
    auto type_predefined = relating_type
        ? ifcapi::detail::read_optional_string_attr(*relating_type, "PredefinedType")
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

const char* attribute_get_primitive_type(const ifcopenshell::attribute* attribute) {
    if (!attribute) return "unknown";
    return primitive_type_for(attribute->type_of_attribute());
}

std::vector<std::string> attribute_get_enum_items(const ifcopenshell::attribute* attribute) {
    std::vector<std::string> result;
    auto* en = enum_type_for(attribute);
    if (!en) return result;
    const auto& items = en->enumeration_items();
    result.reserve(items.size());
    for (const auto& item : items) result.push_back(item);
    return result;
}

void attribute_edit_attributes(
    ifcopenshell::file* file,
    const AttributeEditAttributesOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto product_value = options.product;
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        ifcapi::detail::apply_named_attributes(product_value, options.attributes);
        if (options.sync_predefined_type) {
            sync_predefined_type(file, product_value);
        }
        if (options.update_owner_history && has_attr(product_value, "OwnerHistory")) {
            ifcapi::detail::update_owner_history(file, product_value, user_value, application_value);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

} // namespace bindings

namespace detail {

void edit_named_attributes(
    ifcopenshell::file* file,
    express::Base target,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    try {
        apply_named_attributes(target, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void apply_named_attributes(express::Base target, ifcopenshell_pset_props_t* attributes)
{
    if (!target) {
        throw std::runtime_error("Named attribute edit requires a target");
    }
    apply_attribute_props(target, attributes);
}

} // namespace detail
} // namespace ifcapi
