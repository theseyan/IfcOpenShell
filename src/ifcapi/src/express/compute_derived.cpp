// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// Implementation of `ifcopenshell_compute_derived` — the C ABI that the
// Python wrapper calls to evaluate a DERIVE attribute using the native
// rule dispatch table populated by the generated per-schema translation
// units.

#include "selector_internal.h"

#include "ifcapi/express_rules.h"
#include "ifcapi/express/runtime.h"
#include "ifcapi/express/value.h"

#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcBaseClass.h"

namespace {

using ifcapi::express::Value;

ifcopenshell_value_t* convert(const Value& v) {
    switch (v.tag()) {
        case Value::Tag::Indeterminate:
            return nullptr;
        case Value::Tag::Bool:
            return make_bool(v.as_bool());
        case Value::Tag::Int:
            return make_int(static_cast<int64_t>(v.as_int()));
        case Value::Tag::Real:
            return make_double(v.as_double());
        case Value::Tag::Str:
            return make_string(v.as_string());
        case Value::Tag::Entity: {
            auto* e = static_cast<IfcUtil::IfcBaseClass*>(v.as_entity().ptr);
            return e ? make_instance(e) : make_none();
        }
        case Value::Tag::List: {
            auto* out = make_list();
            for (const auto& item : v.as_list()) {
                out->list_val.push_back(convert(item));
            }
            return out;
        }
        case Value::Tag::Set: {
            // Represent sets as lists over the ABI — Python side treats
            // aggregate DERIVE results as tuples regardless.
            auto* out = make_list();
            for (const auto& item : v.as_set()) {
                out->list_val.push_back(convert(item));
            }
            return out;
        }
        case Value::Tag::EntityProxy:
            // Scratch in-memory proxies have no stable identity across the
            // ABI; surface them as INDETERMINATE. Rules in shipping schemas
            // never return bare proxies as top-level results.
            return nullptr;
    }
    return nullptr;
}

}  // namespace

extern "C" {

IFCAPI_EXPORT ifcopenshell_value_t*
ifcopenshell_compute_derived(ifcopenshell_ifc_instance_t* instance,
                             const char* attr_name) {
    if (!instance || !instance->ptr || !attr_name) return nullptr;
    auto* e = instance->ptr;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    const auto* decl = be->declaration().as_entity();
    if (!decl) return nullptr;

    auto fn = ifcapi::express::lookup_derived(decl, attr_name);
    if (!fn) return nullptr;

    ifcapi::express::EntityRef self_ref;
    self_ref.ptr = static_cast<void*>(e);

    Value result;
    try {
        result = fn(self_ref);
    } catch (...) {
        return nullptr;
    }
    return convert(result);
}

}  // extern "C"
