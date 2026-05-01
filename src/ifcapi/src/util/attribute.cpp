// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/attribute.h"

#include "ifcparse/IfcSchema.h"

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

} // namespace bindings
} // namespace ifcapi

