// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcSchema.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

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

extern "C" {

IFCAPI_EXPORT const char* ifcopenshell_util_attribute_get_primitive_type(const void* attribute) {
    auto* attr = reinterpret_cast<const IfcParse::attribute*>(attribute);
    if (!attr) return "unknown";
    return primitive_type_for(attr->type_of_attribute());
}

IFCAPI_EXPORT char** ifcopenshell_util_attribute_get_enum_items(const void* attribute, uint32_t* out_count) {
    if (out_count) *out_count = 0;
    auto* attr = reinterpret_cast<const IfcParse::attribute*>(attribute);
    auto* en = enum_type_for(attr);
    if (!en) return nullptr;
    const auto& items = en->enumeration_items();
    if (items.empty()) return nullptr;
    auto** buf = static_cast<char**>(std::malloc(items.size() * sizeof(char*)));
    if (!buf) return nullptr;
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& s = items[i];
        char* dup = static_cast<char*>(std::malloc(s.size() + 1));
        if (!dup) {
            for (size_t j = 0; j < i; ++j) std::free(buf[j]);
            std::free(buf);
            return nullptr;
        }
        std::memcpy(dup, s.data(), s.size());
        dup[s.size()] = '\0';
        buf[i] = dup;
    }
    if (out_count) *out_count = static_cast<uint32_t>(items.size());
    return buf;
}

}  // extern "C"
