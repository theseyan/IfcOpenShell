// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// Native DERIVE evaluator used by the generated high-level binding facade.

#include "selector/internal.h"

#include "ifcapi/bindings/value.h"
#include "ifcapi/value.h"
#include "ifcapi/express/runtime.h"
#include "ifcapi/express/value.h"

#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcBaseClass.h"

namespace {

using ifcapi::express::Value;

ifcopenshell_value_t* convert(const Value& v) {
    Value abi_value = ifcapi::express::materialize_for_abi(v);
    switch (abi_value.tag()) {
        case Value::Tag::Indeterminate:
            return nullptr;
        case Value::Tag::Bool:
            return make_bool(abi_value.as_bool());
        case Value::Tag::Int:
            return make_int(static_cast<int64_t>(abi_value.as_int()));
        case Value::Tag::Real:
            return make_double(abi_value.as_double());
        case Value::Tag::Str:
            return make_string(abi_value.as_string());
        case Value::Tag::Entity: {
            auto* e = static_cast<IfcUtil::IfcBaseClass*>(abi_value.as_entity().ptr);
            return e ? make_instance(e) : make_none();
        }
        case Value::Tag::List: {
            auto* out = make_list();
            for (const auto& item : abi_value.as_list()) {
                out->list_val.push_back(convert(item));
            }
            return out;
        }
        case Value::Tag::Set: {
            // Represent sets as lists over the ABI — Python side treats
            // aggregate DERIVE results as tuples regardless.
            auto* out = make_list();
            for (const auto& item : abi_value.as_set()) {
                out->list_val.push_back(convert(item));
            }
            return out;
        }
        case Value::Tag::EntityProxy:
            // materialize_for_abi() should already have converted proxies.
            return nullptr;
    }
    return nullptr;
}

}  // namespace

namespace ifcapi {
namespace bindings {

ifcopenshell_value_t* compute_derived(IfcUtil::IfcBaseClass* instance, const std::string& attr_name) {
    if (!instance) return nullptr;
    auto* e = instance;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    const auto* decl = be->declaration().as_entity();
    if (!decl) return nullptr;

    auto fn = ifcapi::express::lookup_derived(decl, attr_name.c_str());
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

} // namespace bindings
} // namespace ifcapi
