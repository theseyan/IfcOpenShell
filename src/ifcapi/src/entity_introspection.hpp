// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Lightweight entity introspection helpers shared across translation units.
// Header-only (inline). Pulls in only ifcparse — no selector/value machinery.

#ifndef IFCAPI_ENTITY_INTROSPECTION_HPP
#define IFCAPI_ENTITY_INTROSPECTION_HPP

#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {

inline bool entity_is_a(IfcUtil::IfcBaseClass* e, const char* type) {
    return e && e->declaration().is(type);
}

inline int find_attr_idx(IfcUtil::IfcBaseClass* e, const char* name) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return -1;
    auto* d = be->declaration().as_entity();
    if (!d) return -1;
    return static_cast<int>(d->attribute_index(name));
}

inline bool has_attr(IfcUtil::IfcBaseClass* e, const char* name) {
    return find_attr_idx(e, name) >= 0;
}

inline IfcUtil::IfcBaseClass* get_entity_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return nullptr;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull() || v.type() != IfcUtil::Argument_ENTITY_INSTANCE) return nullptr;
        return (IfcUtil::IfcBaseClass*)v;
    } catch (...) { return nullptr; }
}

inline std::vector<IfcUtil::IfcBaseClass*> get_entity_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return out;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return out;
        auto agg = (aggregate_of_instance::ptr)v;
        if (agg) for (auto& it : *agg) if (it) out.push_back(it);
    } catch (...) {}
    return out;
}

inline std::string get_string_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        if (v.type() == IfcUtil::Argument_STRING || v.type() == IfcUtil::Argument_ENUMERATION)
            return (std::string)v;
    } catch (...) {}
    return {};
}

inline bool get_double_attr(IfcUtil::IfcBaseClass* e, const char* attr, double* out) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        *out = (double)v;
        return true;
    } catch (...) { return false; }
}

inline bool read_double_vec3(IfcUtil::IfcBaseClass* e, const char* attr, double out[3]) {
    out[0] = out[1] = out[2] = 0.0;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        auto vec = (std::vector<double>)v;
        for (size_t i = 0; i < std::min(vec.size(), size_t(3)); ++i) out[i] = vec[i];
        return true;
    } catch (...) { return false; }
}

inline void set_entity_ref(IfcUtil::IfcBaseClass* e, const char* attr, IfcUtil::IfcBaseClass* v) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return;
    if (v) {
        e->set_attribute_value(static_cast<size_t>(idx), v);
        return;
    }
    bool was_set = false;
    try {
        auto cur = e->get_attribute_value(static_cast<size_t>(idx));
        was_set = !cur.isNull();
    } catch (...) {}
    if (was_set) {
        try { e->set_attribute_value(static_cast<size_t>(idx), Blank{}); } catch (...) {}
    }
}

inline std::vector<IfcUtil::IfcBaseClass*> get_all_inverses(
    IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e)
{
    std::vector<IfcUtil::IfcBaseClass*> out;
    if (!file || !e) return out;
    try {
        auto inv = file->getInverse(e->id(), nullptr, -1);
        if (inv) for (auto& it : *inv) if (it) out.push_back(it);
    } catch (...) {}
    return out;
}

}  // namespace ifcapi

#endif
