// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Internal helpers shared between selector_get.cpp and selector_filter.cpp.
// Included (not linked) — all functions are inline/static-inline.
// Do NOT include from public headers.

#ifndef IFCAPI_SELECTOR_INTERNAL_H
#define IFCAPI_SELECTOR_INTERNAL_H

#include "ifcapi/value.h"
#include "ifcapi/selector/ast.h"
#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "ifcapi/bindings/selector.h"

#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/file.h"

#include <cmath>
#include <cstring>
#include <regex>
#include <string>
#include <vector>

/* ====================================================================
 *  Full definition of the opaque value handle.
 * ==================================================================== */

struct ifcopenshell_value_t {
    ifcopenshell_value_kind_t kind = IFCSEL_VALUE_NONE;
    bool    b_val  = false;
    int64_t i_val  = 0;
    double  d_val  = 0.0;
    std::string s_val;
    express::Base inst_val;

    std::vector<ifcopenshell_value_t*> list_val;
    std::vector<std::pair<std::string, ifcopenshell_value_t*>> dict_val;

    ~ifcopenshell_value_t() {
        for (auto v : list_val) delete v;
        for (auto& kv : dict_val) delete kv.second;
    }

    ifcopenshell_value_t() = default;
    ifcopenshell_value_t(const ifcopenshell_value_t&) = delete;
    ifcopenshell_value_t& operator=(const ifcopenshell_value_t&) = delete;
};

/* ---- Type alias ---- */
using Val = ifcopenshell_value_t;

inline ifcopenshell_selector_node_t* ifcopenshell_selector_parse_filter(const char* query) {
    return ifcapi::bindings::selector_parse_filter(query ? std::string(query) : std::string());
}

inline ifcopenshell_selector_node_t* ifcopenshell_selector_parse_get_element(const char* query) {
    return ifcapi::bindings::selector_parse_get_element(query ? std::string(query) : std::string());
}

inline ifcopenshell_selector_node_t* ifcopenshell_selector_parse_format(const char* query) {
    return ifcapi::bindings::selector_parse_format(query ? std::string(query) : std::string());
}

inline ifcsel_node_kind ifcopenshell_selector_node_kind(const ifcopenshell_selector_node_t* node) {
    return static_cast<ifcsel_node_kind>(ifcapi::bindings::selector_node_kind(node));
}

inline size_t ifcopenshell_selector_node_child_count(const ifcopenshell_selector_node_t* node) {
    return ifcapi::bindings::selector_node_child_count(node);
}

inline ifcopenshell_selector_node_t* ifcopenshell_selector_node_child(
    const ifcopenshell_selector_node_t* node, size_t index)
{
    return ifcapi::bindings::selector_node_child(node, index);
}

inline const char* ifcopenshell_selector_node_text(const ifcopenshell_selector_node_t* node) {
    static thread_local std::string text;
    text = ifcapi::bindings::selector_node_text(node);
    return text.empty() ? nullptr : text.c_str();
}

inline void ifcopenshell_selector_node_free(ifcopenshell_selector_node_t* root) {
    ifcapi::bindings::selector_node_free(root);
}

/* ====================================================================
 *  Value factories
 * ==================================================================== */

inline Val* make_none() { return new Val(); }

inline Val* make_bool(bool b) {
    auto v = new Val(); v->kind = IFCSEL_VALUE_BOOL; v->b_val = b; return v;
}
inline Val* make_int(int64_t i) {
    auto v = new Val(); v->kind = IFCSEL_VALUE_INT; v->i_val = i; return v;
}
inline Val* make_double(double d) {
    auto v = new Val(); v->kind = IFCSEL_VALUE_DOUBLE; v->d_val = d; return v;
}
inline Val* make_string(std::string s) {
    auto v = new Val(); v->kind = IFCSEL_VALUE_STRING; v->s_val = std::move(s); return v;
}
inline Val* make_instance(express::Base e) {
    if (!e) return make_none();
    auto v = new Val(); v->kind = IFCSEL_VALUE_INSTANCE; v->inst_val = e; return v;
}
inline Val* make_list() {
    auto v = new Val(); v->kind = IFCSEL_VALUE_LIST; return v;
}
inline Val* make_dict() {
    auto v = new Val(); v->kind = IFCSEL_VALUE_DICT; return v;
}

inline bool placement_matrix_to_array(const std::vector<double>& values, double out[16]) {
    if (values.size() != 16) return false;
    std::copy(values.begin(), values.end(), out);
    return true;
}

inline Val* clone_val(const Val* v) {
    if (!v) return make_none();
    switch (v->kind) {
        case IFCSEL_VALUE_NONE:     return make_none();
        case IFCSEL_VALUE_BOOL:     return make_bool(v->b_val);
        case IFCSEL_VALUE_INT:      return make_int(v->i_val);
        case IFCSEL_VALUE_DOUBLE:   return make_double(v->d_val);
        case IFCSEL_VALUE_STRING:   return make_string(v->s_val);
        case IFCSEL_VALUE_INSTANCE: return make_instance(v->inst_val);
        case IFCSEL_VALUE_LIST: {
            auto l = make_list();
            for (auto c : v->list_val) l->list_val.push_back(clone_val(c));
            return l;
        }
        case IFCSEL_VALUE_DICT: {
            auto d = make_dict();
            for (auto& kv : v->dict_val) d->dict_val.push_back({kv.first, clone_val(kv.second)});
            return d;
        }
    }
    return make_none();
}

/* ====================================================================
 *  RAII scoped handle
 * ==================================================================== */

struct ScopedHandle_DISABLED {
    ifcopenshell_ifc_instance_t h;
    explicit ScopedHandle_DISABLED(express::Base e) { h.value = e; }
    ifcopenshell_ifc_instance_t* get() { return &h; }
};

/* ====================================================================
 *  Entity introspection helpers
 * ==================================================================== */

inline bool entity_is_a(express::Base e, const char* type) {
    return e && e.declaration().is(type);
}

inline int find_attr_idx(express::Base e, const char* name) {
    auto decl = e.declaration().as_entity();
    if (!decl) return -1;
    return static_cast<int>(decl->attribute_index(name));
}

inline std::string get_string_attr(express::Base e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return "";
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return "";
        if (v.type() == ifcopenshell::Argument_STRING || v.type() == ifcopenshell::Argument_ENUMERATION)
            return (std::string)v;
    } catch (...) {}
    return "";
}

inline express::Base get_entity_ref(express::Base e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull() || v.type() != ifcopenshell::Argument_ENTITY_INSTANCE) return {};
        return (express::Base)v;
    } catch (...) { return {}; }
}

inline std::vector<express::Base> get_entity_list(express::Base e, const char* attr) {
    std::vector<express::Base> result;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return result;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return result;
        auto agg = (std::vector<express::Base>)v;
        for (auto item : agg) { if (item) result.push_back(item); }
    } catch (...) {}
    return result;
}

inline std::vector<express::Base> get_inverse_list(express::Base e, const char* name) {
    return ifcapi::detail::read_inverse_aggregate(e, name);
}

/* ====================================================================
 *  Value unwrapping
 * ==================================================================== */

inline Val* unwrap_typed_value(express::Base typed) {
    if (!typed) return make_none();
    try {
        auto inner = typed.get_attribute_value(0);
        if (inner.isNull()) return make_none();
        switch (inner.type()) {
            case ifcopenshell::Argument_STRING:      return make_string((std::string)inner);
            case ifcopenshell::Argument_DOUBLE:      return make_double((double)inner);
            case ifcopenshell::Argument_INT:         return make_int((int)inner);
            case ifcopenshell::Argument_BOOL:        return make_bool((bool)inner);
            case ifcopenshell::Argument_ENUMERATION: return make_string((std::string)inner);
            default:                            return make_none();
        }
    } catch (...) { return make_none(); }
}

inline Val* attr_to_val(const attribute_value& v) {
    if (v.isNull()) return make_none();
    switch (v.type()) {
        case ifcopenshell::Argument_STRING:      return make_string((std::string)v);
        case ifcopenshell::Argument_INT:         return make_int((int)v);
        case ifcopenshell::Argument_DOUBLE:      return make_double((double)v);
        case ifcopenshell::Argument_BOOL:        return make_bool((bool)v);
        case ifcopenshell::Argument_ENUMERATION: return make_string((std::string)v);
        case ifcopenshell::Argument_ENTITY_INSTANCE: {
            auto ref = (express::Base)v;
            if (!ref) return make_none();
            if (ref.declaration().as_type_declaration()) return unwrap_typed_value(ref);
            return make_instance(ref);
        }
        case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
            auto agg = (std::vector<express::Base>)v;
            if (agg.empty()) return make_none();
            auto list = make_list();
            for (auto item : agg) {
                if (!item) { list->list_val.push_back(make_none()); continue; }
                if (item.declaration().as_type_declaration())
                    list->list_val.push_back(unwrap_typed_value(item));
                else
                    list->list_val.push_back(make_instance(item));
            }
            return list;
        }
        case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE: {
            auto vec = (std::vector<double>)v;
            auto l = make_list();
            for (double d : vec) l->list_val.push_back(make_double(d));
            return l;
        }
        case ifcopenshell::Argument_AGGREGATE_OF_INT: {
            auto vec = (std::vector<int>)v;
            auto l = make_list();
            for (int i : vec) l->list_val.push_back(make_int(i));
            return l;
        }
        case ifcopenshell::Argument_AGGREGATE_OF_STRING: {
            auto vec = (std::vector<std::string>)v;
            auto l = make_list();
            for (auto& s : vec) l->list_val.push_back(make_string(s));
            return l;
        }
        default: return make_none();
    }
}

inline Val* read_attr(express::Base e, const std::string& attr_name) {
    auto decl = e.declaration().as_entity();
    if (!decl) return {};
    int idx_i = static_cast<int>(decl->attribute_index(attr_name));
    if (idx_i < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx_i));
        return attr_to_val(v);
    } catch (...) { return make_none(); }
}

/* ====================================================================
 *  Pset extraction
 * ==================================================================== */

inline Val* extract_prop_value(express::Base prop) {
    if (!prop) return make_none();
    const auto& tname = prop.declaration().name();

    if (tname == "IfcPropertySingleValue") {
        int nv = find_attr_idx(prop, "NominalValue");
        if (nv < 0) return make_none();
        try {
            auto outer = prop.get_attribute_value(static_cast<size_t>(nv));
            if (outer.isNull()) return make_none();
            if (outer.type() != ifcopenshell::Argument_ENTITY_INSTANCE) return make_none();
            auto typed = (express::Base)outer;
            if (!typed || !typed.declaration().as_type_declaration()) return make_none();
            return unwrap_typed_value(typed);
        } catch (...) { return make_none(); }
    }

    if (tname == "IfcPropertyEnumeratedValue") {
        int ev = find_attr_idx(prop, "EnumerationValues");
        if (ev < 0) return make_none();
        try {
            auto outer = prop.get_attribute_value(static_cast<size_t>(ev));
            if (outer.isNull()) return make_none();
            auto agg = (std::vector<express::Base>)outer;
            if (agg.empty()) return make_none();
            auto list = make_list();
            for (auto item : agg) {
                if (item && item.declaration().as_type_declaration())
                    list->list_val.push_back(unwrap_typed_value(item));
                else
                    list->list_val.push_back(make_none());
            }
            return list;
        } catch (...) { return make_none(); }
    }

    if (tname == "IfcPropertyListValue") {
        int lv = find_attr_idx(prop, "ListValues");
        if (lv < 0) return make_none();
        try {
            auto outer = prop.get_attribute_value(static_cast<size_t>(lv));
            if (outer.isNull()) return make_none();
            auto agg = (std::vector<express::Base>)outer;
            if (agg.empty()) return make_none();
            auto list = make_list();
            for (auto item : agg) {
                if (item && item.declaration().as_type_declaration())
                    list->list_val.push_back(unwrap_typed_value(item));
                else
                    list->list_val.push_back(make_none());
            }
            return list;
        } catch (...) { return make_none(); }
    }

    return make_none();
}

inline Val* extract_pset_props(express::Base defn) {
    if (!defn) return make_none();
    auto dict = make_dict();
    const auto& tname = defn.declaration().name();

    if (tname == "IfcPropertySet" || tname == "IfcMaterialProperties" ||
        tname == "IfcProfileProperties" || tname == "IfcExtendedMaterialProperties") {

        const char* pa = (tname == "IfcPropertySet") ? "HasProperties"
                       : (tname == "IfcExtendedMaterialProperties") ? "ExtendedProperties"
                       : "Properties";
        for (auto prop : get_entity_list(defn, pa)) {
            if (!prop) continue;
            dict->dict_val.push_back({get_string_attr(prop, "Name"), extract_prop_value(prop)});
        }
    } else if (tname == "IfcElementQuantity") {
        for (auto q : get_entity_list(defn, "Quantities")) {
            if (!q || !entity_is_a(q, "IfcPhysicalSimpleQuantity")) continue;
            auto edecl = q.declaration().as_entity();
            if (!edecl || edecl->all_attributes().size() < 4) continue;
            try {
                auto v = q.get_attribute_value(3);
                dict->dict_val.push_back({get_string_attr(q, "Name"), attr_to_val(v)});
            } catch (...) {
                dict->dict_val.push_back({get_string_attr(q, "Name"), make_none()});
            }
        }
    }
    return dict;
}

inline std::vector<std::pair<std::string, Val*>>
get_all_psets(express::Base entity) {
    std::vector<std::pair<std::string, Val*>> result;
    auto psets = ifcapi::bindings::element_get_pset_ids(&entity, false, false, true);
    if (psets.empty()) return result;

    for (auto pset : psets) {
        if (!pset) continue;
        std::string nm = get_string_attr(pset, "Name");
        Val* props = extract_pset_props(pset);

        bool found = false;
        for (auto& kv : result) {
            if (kv.first == nm) {
                for (auto& p : props->dict_val)
                    kv.second->dict_val.push_back({p.first, p.second});
                props->dict_val.clear();
                delete props;
                found = true;
                break;
            }
        }
        if (!found) result.push_back({nm, props});
    }
    return result;
}

/* ====================================================================
 *  Key parsing (used by selector_get)
 * ==================================================================== */

struct KeyEntry {
    bool is_regex = false;
    std::string text;
    std::regex pattern;
};

inline std::string unescape_quoted(const std::string& raw) {
    if (raw.size() < 2) return "";
    std::string out;
    for (size_t i = 1; i + 1 < raw.size(); ++i) {
        if (raw[i] == '\\') continue;
        out += raw[i];
    }
    return out;
}

inline std::vector<KeyEntry> extract_keys(const ifcopenshell_selector_node_t* root) {
    std::vector<KeyEntry> keys;
    if (!root) return keys;

    const ifcopenshell_selector_node_t* kl = root;
    if (ifcopenshell_selector_node_kind(root) == IFCSEL_NODE_START) {
        if (ifcopenshell_selector_node_child_count(root) == 0) return keys;
        kl = ifcopenshell_selector_node_child(root, 0);
    }
    if (ifcopenshell_selector_node_kind(kl) != IFCSEL_NODE_KEYS_LIST) return keys;

    size_t n = ifcopenshell_selector_node_child_count(kl);
    for (size_t i = 0; i < n; ++i) {
        auto key_node = ifcopenshell_selector_node_child(kl, i);
        if (!key_node || ifcopenshell_selector_node_kind(key_node) != IFCSEL_NODE_KEY) continue;
        if (ifcopenshell_selector_node_child_count(key_node) == 0) continue;

        auto val_node = ifcopenshell_selector_node_child(key_node, 0);
        if (!val_node) continue;

        KeyEntry ke;
        auto vk = ifcopenshell_selector_node_kind(val_node);

        if (vk == IFCSEL_NODE_REGEX_STRING) {
            ke.is_regex = true;
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto tok = ifcopenshell_selector_node_child(val_node, 0);
                const char* txt = ifcopenshell_selector_node_text(tok);
                ke.text = txt ? txt : "";
            }
            try { ke.pattern = std::regex(ke.text); }
            catch (...) { continue; }
        } else if (vk == IFCSEL_NODE_QUOTED_STRING) {
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto tok = ifcopenshell_selector_node_child(val_node, 0);
                const char* txt = ifcopenshell_selector_node_text(tok);
                ke.text = unescape_quoted(txt ? txt : "");
            }
        } else {
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto tok = ifcopenshell_selector_node_child(val_node, 0);
                const char* txt = ifcopenshell_selector_node_text(tok);
                ke.text = txt ? txt : "";
            }
        }
        keys.push_back(std::move(ke));
    }
    return keys;
}

/* ====================================================================
 *  Semantic helpers
 * ==================================================================== */

inline Val* resolve_predefined_type(express::Base e) {
    auto type_e = ifcapi::bindings::element_get_type(&e);

    if (type_e) {
        std::string pt = get_string_attr(type_e, "PredefinedType");
        if (pt.empty() || pt == "USERDEFINED") {
            int et_idx = find_attr_idx(type_e, "ElementType");
            if (et_idx < 0) {
                std::string prt = get_string_attr(type_e, "ProcessType");
                if (!prt.empty() && prt != "NOTDEFINED") return make_string(prt);
            } else {
                std::string et = get_string_attr(type_e, "ElementType");
                if (!et.empty() && et != "NOTDEFINED") return make_string(et);
            }
        } else if (pt != "NOTDEFINED") {
            return make_string(pt);
        }
    }

    std::string pt = get_string_attr(e, "PredefinedType");
    if (pt.empty() || pt == "USERDEFINED") {
        std::string ot = get_string_attr(e, "ObjectType");
        return ot.empty() ? make_none() : make_string(ot);
    }
    return (pt == "NOTDEFINED") ? make_none() : make_string(pt);
}

inline Val* resolve_classification(express::Base e) {
    auto list = make_list();
    for (auto rel : get_inverse_list(e, "HasAssociations")) {
        if (!entity_is_a(rel, "IfcRelAssociatesClassification")) continue;
        auto ref = get_entity_ref(rel, "RelatingClassification");
        if (ref) list->list_val.push_back(make_instance(ref));
    }
    return list;
}

inline Val* resolve_groups(express::Base e) {
    auto list = make_list();
    for (auto rel : get_inverse_list(e, "HasAssignments")) {
        if (!entity_is_a(rel, "IfcRelAssignsToGroup")) continue;
        auto grp = get_entity_ref(rel, "RelatingGroup");
        if (grp) list->list_val.push_back(make_instance(grp));
    }
    return list;
}

inline Val* resolve_occurrences(express::Base e) {
    auto list = make_list();
    for (auto rel : get_inverse_list(e, "ObjectTypeOf")) {
        if (!entity_is_a(rel, "IfcRelDefinesByType")) continue;
        for (auto obj : get_entity_list(rel, "RelatedObjects"))
            list->list_val.push_back(make_instance(obj));
    }
    return list;
}

inline Val* resolve_styles(express::Base e) {
    auto list = make_list();
    if (!e) return list;
    auto styles = ifcapi::bindings::element_get_styles(&e);
    if (styles.empty()) return list;
    for (auto style : styles) {
        if (style) list->list_val.push_back(make_instance(style));
    }
    return list;
}

inline Val* resolve_systems(express::Base e, bool zones_only) {
    auto list = make_list();
    for (auto rel : get_inverse_list(e, "HasAssignments")) {
        if (!entity_is_a(rel, "IfcRelAssignsToGroup")) continue;
        auto grp = get_entity_ref(rel, "RelatingGroup");
        if (!grp) continue;
        bool is_zone = entity_is_a(grp, "IfcZone");
        bool is_system = entity_is_a(grp, "IfcSystem") && !entity_is_a(grp, "IfcStructuralAnalysisModel");
        if ((zones_only && is_zone) || (!zones_only && is_system && !is_zone)) {
            list->list_val.push_back(make_instance(grp));
        }
    }
    return list;
}

inline Val* resolve_profiles(ifcopenshell::file* file, express::Base e) {
    auto list = make_list();
    if (!e) return list;

    auto mat = ifcapi::bindings::element_get_material(&e, true, true);
    if (mat && entity_is_a(mat, "IfcMaterialProfileSet")) {
        for (auto mp : get_entity_list(mat, "MaterialProfiles")) {
            auto profile = get_entity_ref(mp, "Profile");
            if (profile) list->list_val.push_back(make_instance(profile));
        }
        return list;
    }

    if (!file) return list;
    auto rep = ifcapi::bindings::representation_get_product_representation(
        &e, nullptr, "Model", "Body", "MODEL_VIEW");
    if (!rep) return list;

    auto items = ifcapi::bindings::representation_resolve_base_items(&rep);
    if (items.empty()) return list;
    for (auto item : items) {
        if (!item || !entity_is_a(item, "IfcExtrudedAreaSolid")) continue;
        auto swept = get_entity_ref(item, "SweptArea");
        if (swept) list->list_val.push_back(make_instance(swept));
    }
    return list;
}

inline Val* resolve_xyz(express::Base e, const std::string& k) {
    auto placement_e = get_entity_ref(e, "ObjectPlacement");
    if (!placement_e) return make_none();
    double matrix[16];
    if (!placement_matrix_to_array(ifcapi::bindings::placement_get_local_placement(&placement_e), matrix)) return make_none();
    int ci = (k == "x") ? 0 : (k == "y") ? 1 : 2;
    return make_double(matrix[ci * 4 + 3]);
}

inline bool get_numeric_attr(express::Base e, const char* attr, double* out) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        if (v.type() == ifcopenshell::Argument_DOUBLE) {
            *out = (double)v;
            return true;
        }
        if (v.type() == ifcopenshell::Argument_INT) {
            *out = (double)(int)v;
            return true;
        }
    } catch (...) {}
    return false;
}

inline bool invert_rigid4(const double* m, double* out) {
    if (!m || !out) return false;
    out[0] = m[0]; out[1] = m[4]; out[2] = m[8];  out[3] = 0.0;
    out[4] = m[1]; out[5] = m[5]; out[6] = m[9];  out[7] = 0.0;
    out[8] = m[2]; out[9] = m[6]; out[10] = m[10]; out[11] = 0.0;
    out[12] = 0.0; out[13] = 0.0; out[14] = 0.0; out[15] = 1.0;
    out[3] = -(out[0] * m[3] + out[1] * m[7] + out[2] * m[11]);
    out[7] = -(out[4] * m[3] + out[5] * m[7] + out[6] * m[11]);
    out[11] = -(out[8] * m[3] + out[9] * m[7] + out[10] * m[11]);
    return true;
}

inline void transform_point4(const double* m, double& x, double& y, double& z) {
    double ox = x;
    double oy = y;
    double oz = z;
    x = (m[0] * ox) + (m[1] * oy) + (m[2] * oz) + m[3];
    y = (m[4] * ox) + (m[5] * oy) + (m[6] * oz) + m[7];
    z = (m[8] * ox) + (m[9] * oy) + (m[10] * oz) + m[11];
}

inline bool get_typed_numeric_value(express::Base e, double* out) {
    if (!e || !out) return false;
    try {
        auto inner = e.get_attribute_value(0);
        if (inner.isNull()) return false;
        if (inner.type() == ifcopenshell::Argument_DOUBLE) {
            *out = (double)inner;
            return true;
        }
        if (inner.type() == ifcopenshell::Argument_INT) {
            *out = (double)(int)inner;
            return true;
        }
    } catch (...) {}
    return false;
}

inline bool apply_wcs_inverse(ifcopenshell::file* file, double& x, double& y, double& z) {
    if (!file) return false;
    auto ctxs = file->instances_by_type("IfcGeometricRepresentationContext");
    if (ctxs.empty()) return false;
    express::Base chosen;
    for (auto ctx : ctxs) {
        if (!ctx) continue;
        chosen = ctx;
        if (get_string_attr(ctx, "ContextType") == "Model") break;
    }
    if (!chosen) return false;
    auto wcs = get_entity_ref(chosen, "WorldCoordinateSystem");
    if (!wcs) return false;
    double m[16];
    if (!placement_matrix_to_array(ifcapi::bindings::placement_get_axis2placement(&wcs), m)) return false;
    double inv[16];
    if (!invert_rigid4(m, inv)) return false;
    transform_point4(inv, x, y, z);
    return true;
}

inline bool get_map_conversion(ifcopenshell::file* file, double& eastings, double& northings,
                               double& orthogonal_height, double& xaa, double& xao,
                               double& scale, double& factor_x, double& factor_y, double& factor_z) {
    eastings = northings = orthogonal_height = 0.0;
    xaa = 1.0;
    xao = 0.0;
    scale = factor_x = factor_y = factor_z = 1.0;
    if (!file) return false;

    auto conversions = file->instances_by_type("IfcCoordinateOperation");
    if (conversions.empty()) return false;
    auto conversion = conversions[0];
    if (!conversion) return false;

    if (entity_is_a(conversion, "IfcMapConversion")) {
        get_numeric_attr(conversion, "Eastings", &eastings);
        get_numeric_attr(conversion, "Northings", &northings);
        get_numeric_attr(conversion, "OrthogonalHeight", &orthogonal_height);
        get_numeric_attr(conversion, "XAxisAbscissa", &xaa);
        get_numeric_attr(conversion, "XAxisOrdinate", &xao);
        get_numeric_attr(conversion, "Scale", &scale);
        if (entity_is_a(conversion, "IfcMapConversionScaled")) {
            get_numeric_attr(conversion, "FactorX", &factor_x);
            get_numeric_attr(conversion, "FactorY", &factor_y);
            get_numeric_attr(conversion, "FactorZ", &factor_z);
        }
        if (xaa == 0.0 && xao == 0.0) {
            xaa = 1.0;
            xao = 0.0;
        }
        return true;
    }

    if (entity_is_a(conversion, "IfcRigidOperation")) {
        auto first = get_entity_ref(conversion, "FirstCoordinate");
        auto second = get_entity_ref(conversion, "SecondCoordinate");
        get_typed_numeric_value(first, &eastings);
        get_typed_numeric_value(second, &northings);
        get_numeric_attr(conversion, "Height", &orthogonal_height);
        return true;
    }

    return false;
}

inline Val* resolve_map_coordinate(ifcopenshell::file* file, express::Base e, const std::string& k) {
    auto placement_e = get_entity_ref(e, "ObjectPlacement");
    if (!placement_e) return make_none();
    double matrix[16];
    if (!placement_matrix_to_array(ifcapi::bindings::placement_get_local_placement(&placement_e), matrix)) return make_none();
    double x = matrix[3];
    double y = matrix[7];
    double z = matrix[11];
    apply_wcs_inverse(file, x, y, z);

    double eastings, northings, orthogonal_height, xaa, xao, scale, factor_x, factor_y, factor_z;
    if (get_map_conversion(file, eastings, northings, orthogonal_height, xaa, xao, scale, factor_x, factor_y, factor_z)) {
        double theta = std::atan2(xao, xaa);
        double easting = (scale * factor_x * std::cos(theta) * x) - (scale * factor_y * std::sin(theta) * y) + eastings;
        double northing = (scale * factor_x * std::sin(theta) * x) + (scale * factor_y * std::cos(theta) * y) + northings;
        double elevation = (scale * factor_z * z) + orthogonal_height;
        if (k == "easting") return make_double(easting);
        if (k == "northing") return make_double(northing);
        return make_double(elevation);
    }

    if (k == "easting") return make_double(x);
    if (k == "northing") return make_double(y);
    return make_double(z);
}

/* ====================================================================
 *  Regex helper
 * ==================================================================== */

inline bool regex_match_start(const std::string& str, const std::regex& pattern) {
    std::smatch m;
    return std::regex_search(str, m, pattern, std::regex_constants::match_continuous);
}

/* ====================================================================
 *  apply_key  (get_element evaluator)
 * ==================================================================== */

inline Val* apply_key(ifcopenshell::file* file, const Val* cur, const KeyEntry& key);

inline Val* apply_key_to_list(ifcopenshell::file* file, const Val* cur, const KeyEntry& key) {
    const std::string& k = key.text;

    if (!key.is_regex) {
        bool all_digits = !k.empty();
        for (char c : k) if (!std::isdigit((unsigned char)c)) { all_digits = false; break; }
        if (all_digits) {
            size_t idx = static_cast<size_t>(std::stoul(k));
            if (idx >= cur->list_val.size()) return make_none();
            return clone_val(cur->list_val[idx]);
        }
    }

    auto results = make_list();
    for (auto item : cur->list_val) {
        Val* sub = apply_key(file, item, key);
        if (!sub) { results->list_val.push_back(make_none()); continue; }
        if (sub->kind == IFCSEL_VALUE_LIST) {
            for (auto sv : sub->list_val) results->list_val.push_back(sv);
            sub->list_val.clear();
            delete sub;
        } else {
            results->list_val.push_back(sub);
        }
    }
    return results;
}

inline Val* apply_key(ifcopenshell::file* file, const Val* cur, const KeyEntry& key) {
    if (!cur || cur->kind == IFCSEL_VALUE_NONE) return make_none();

    if (cur->kind == IFCSEL_VALUE_LIST)
        return apply_key_to_list(file, cur, key);

    if (cur->kind == IFCSEL_VALUE_DICT) {
        if (key.is_regex) {
            std::vector<Val*> matches;
            for (auto& kv : cur->dict_val) {
                if (!regex_match_start(kv.first, key.pattern)) continue;
                if (kv.second->kind == IFCSEL_VALUE_LIST) {
                    for (auto sv : kv.second->list_val) matches.push_back(clone_val(sv));
                } else {
                    matches.push_back(clone_val(kv.second));
                }
            }
            if (matches.empty()) return make_none();
            if (matches.size() == 1) return matches[0];
            auto l = make_list();
            for (auto v : matches) l->list_val.push_back(v);
            return l;
        } else {
            for (auto& kv : cur->dict_val)
                if (kv.first == key.text) return clone_val(kv.second);
            return make_none();
        }
    }

    if (cur->kind != IFCSEL_VALUE_INSTANCE) return make_none();
    auto e = cur->inst_val;
    if (!e) return make_none();

    const std::string& k = key.text;

    if (!key.is_regex) {
        if (k == "type") {
            auto type = ifcapi::bindings::element_get_type(&e);
            return type ? make_instance(type) : make_none();
        }
        if (k == "material" || k == "mat") {
            auto material = ifcapi::bindings::element_get_material(&e, true, true);
            return material ? make_instance(material) : make_none();
        }
        if (k == "materials" || k == "mats") {
            auto mat = ifcapi::bindings::element_get_material(&e, true, true);
            if (!mat) return make_none();
            auto list = make_list();
            if (entity_is_a(mat, "IfcMaterial")) {
                list->list_val.push_back(make_instance(mat));
            } else if (entity_is_a(mat, "IfcMaterialLayerSet")) {
                for (auto lay : get_entity_list(mat, "MaterialLayers")) {
                    auto m = get_entity_ref(lay, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialProfileSet")) {
                for (auto pr : get_entity_list(mat, "MaterialProfiles")) {
                    auto m = get_entity_ref(pr, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialConstituentSet")) {
                for (auto co : get_entity_list(mat, "MaterialConstituents")) {
                    auto m = get_entity_ref(co, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialList")) {
                for (auto m : get_entity_list(mat, "Materials"))
                    list->list_val.push_back(make_instance(m));
            }
            return list;
        }
        if (k == "profiles")        return resolve_profiles(file, e);
        if (k == "styles")          return resolve_styles(e);
        if (k == "system")          return resolve_systems(e, false);
        if (k == "zone")            return resolve_systems(e, true);

        if (k == "item" || k == "i") {
            const char* attr = entity_is_a(e, "IfcMaterialLayerSet")  ? "MaterialLayers"
                             : entity_is_a(e, "IfcMaterialProfileSet") ? "MaterialProfiles"
                             : entity_is_a(e, "IfcMaterialConstituentSet") ? "MaterialConstituents"
                             : nullptr;
            if (!attr) return make_none();
            auto list = make_list();
            for (auto item : get_entity_list(e, attr)) list->list_val.push_back(make_instance(item));
            return list;
        }
        if (k == "container") {
            auto container = ifcapi::bindings::element_get_container(&e, false, nullptr);
            return container ? make_instance(container) : make_none();
        }
        if (k == "space") {
            auto space = ifcapi::bindings::element_get_container(&e, false, "IfcSpace");
            return space ? make_instance(space) : make_none();
        }
        if (k == "storey") {
            auto storey = ifcapi::bindings::element_get_container(&e, false, "IfcBuildingStorey");
            return storey ? make_instance(storey) : make_none();
        }
        if (k == "building") {
            auto building = ifcapi::bindings::element_get_container(&e, false, "IfcBuilding");
            return building ? make_instance(building) : make_none();
        }
        if (k == "site") {
            auto site = ifcapi::bindings::element_get_container(&e, false, "IfcSite");
            return site ? make_instance(site) : make_none();
        }
        if (k == "parent") {
            auto parent = ifcapi::bindings::element_get_parent(&e);
            return parent ? make_instance(parent) : make_none();
        }
        if (k == "types" || k == "occurrences") return resolve_occurrences(e);
        if (k == "count")           return make_int(1);
        if (k == "class")           return make_string(e.declaration().name());
        if (k == "predefined_type") return resolve_predefined_type(e);
        if (k == "id")              return make_int(static_cast<int64_t>(e.id()));
        if (k == "classification")  return resolve_classification(e);
        if (k == "group")           return resolve_groups(e);

        if ((k == "x" || k == "y" || k == "z") &&
            find_attr_idx(e, "ObjectPlacement") >= 0) {
            return resolve_xyz(e, k);
        }
        if ((k == "easting" || k == "northing" || k == "elevation") &&
            find_attr_idx(e, "ObjectPlacement") >= 0) {
            return resolve_map_coordinate(file, e, k);
        }

        std::string attr_key = k;
        if (k == "Name" && entity_is_a(e, "IfcMaterialLayerSet"))
            attr_key = "LayerSetName";

        Val* attr_val = read_attr(e, attr_key);
        if (attr_val) return attr_val;
    }

    auto psets = get_all_psets(e);

    if (key.is_regex) {
        std::vector<Val*> matching;
        for (auto& kv : psets) {
            if (regex_match_start(kv.first, key.pattern))
                matching.push_back(kv.second);
            else
                delete kv.second;
        }
        if (matching.empty()) return make_none();
        if (matching.size() == 1) return matching[0];
        auto list = make_list();
        for (auto v : matching) list->list_val.push_back(v);
        return list;
    }

    Val* found_pset = nullptr;
    for (auto& kv : psets) {
        if (kv.first == k)
            found_pset = kv.second;
        else
            delete kv.second;
    }
    if (!found_pset) return make_none();
    return found_pset;
}

inline Val* get_element_value_impl(
    ifcopenshell::file* file,
    express::Base entity,
    const std::vector<KeyEntry>& keys)
{
    Val* cur = make_instance(entity);
    for (const auto& key : keys) {
        if (cur->kind == IFCSEL_VALUE_NONE) break;
        Val* next = apply_key(file, cur, key);
        delete cur;
        cur = next ? next : make_none();
    }
    return cur;
}

#endif /* IFCAPI_SELECTOR_INTERNAL_H */
