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

inline bool entity_is_a(express::Base e, const char* type) {
    return e && e.declaration().is(type);
}

inline int find_attr_idx(express::Base e, const char* name) {
    auto* d = e ? e.declaration().as_entity() : nullptr;
    if (!d) return -1;
    return static_cast<int>(d->attribute_index(name));
}

inline bool has_attr(express::Base e, const char* name) {
    return find_attr_idx(e, name) >= 0;
}

inline express::Base get_entity_ref(express::Base e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        return static_cast<express::Base>(v);
    } catch (...) { return {}; }
}

inline std::vector<express::Base> get_entity_list(express::Base e, const char* attr) {
    std::vector<express::Base> out;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return out;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return out;
        return static_cast<std::vector<express::Base>>(v);
    } catch (...) {}
    return out;
}

inline std::string get_string_attr(express::Base e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        return static_cast<std::string>(v);
    } catch (...) {}
    return {};
}

inline bool get_double_attr(express::Base e, const char* attr, double* out) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        *out = (double)v;
        return true;
    } catch (...) { return false; }
}

inline bool read_double_vec3(express::Base e, const char* attr, double out[3]) {
    out[0] = out[1] = out[2] = 0.0;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        auto vec = (std::vector<double>)v;
        for (size_t i = 0; i < std::min(vec.size(), size_t(3)); ++i) out[i] = vec[i];
        return true;
    } catch (...) { return false; }
}

inline void set_entity_ref(express::Base e, const char* attr, express::Base v) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return;
    if (v) {
        e.set_attribute_value(static_cast<size_t>(idx), v);
        return;
    }
    bool was_set = false;
    try {
        auto cur = e.get_attribute_value(static_cast<size_t>(idx));
        was_set = !cur.isNull();
    } catch (...) {}
    if (was_set) {
        try { e.unset_attribute_value(static_cast<size_t>(idx)); } catch (...) {}
    }
}

inline std::vector<express::Base> get_all_inverses(
    ifcopenshell::file* file, express::Base e)
{
    std::vector<express::Base> out;
    if (!file || !e) return out;
    try {
        for (auto it : file->instances_by_reference(static_cast<int>(e.id()))) if (it) out.push_back(it);
    } catch (...) {}
    return out;
}

}  // namespace ifcapi

#endif
