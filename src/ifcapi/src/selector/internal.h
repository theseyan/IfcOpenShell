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

#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcFile.h"

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
    IfcUtil::IfcBaseClass* inst_val = nullptr;

    std::vector<ifcopenshell_value_t*> list_val;
    std::vector<std::pair<std::string, ifcopenshell_value_t*>> dict_val;

    ~ifcopenshell_value_t() {
        for (auto* v : list_val) delete v;
        for (auto& kv : dict_val) delete kv.second;
    }

    ifcopenshell_value_t() = default;
    ifcopenshell_value_t(const ifcopenshell_value_t&) = delete;
    ifcopenshell_value_t& operator=(const ifcopenshell_value_t&) = delete;
};

/* ---- Type alias ---- */
using Val = ifcopenshell_value_t;

/* ====================================================================
 *  Value factories
 * ==================================================================== */

inline Val* make_none() { return new Val(); }

inline Val* make_bool(bool b) {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_BOOL; v->b_val = b; return v;
}
inline Val* make_int(int64_t i) {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_INT; v->i_val = i; return v;
}
inline Val* make_double(double d) {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_DOUBLE; v->d_val = d; return v;
}
inline Val* make_string(std::string s) {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_STRING; v->s_val = std::move(s); return v;
}
inline Val* make_instance(IfcUtil::IfcBaseClass* e) {
    if (!e) return make_none();
    auto* v = new Val(); v->kind = IFCSEL_VALUE_INSTANCE; v->inst_val = e; return v;
}
inline Val* make_list() {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_LIST; return v;
}
inline Val* make_dict() {
    auto* v = new Val(); v->kind = IFCSEL_VALUE_DICT; return v;
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
            auto* l = make_list();
            for (auto* c : v->list_val) l->list_val.push_back(clone_val(c));
            return l;
        }
        case IFCSEL_VALUE_DICT: {
            auto* d = make_dict();
            for (auto& kv : v->dict_val) d->dict_val.push_back({kv.first, clone_val(kv.second)});
            return d;
        }
    }
    return make_none();
}

/* ====================================================================
 *  RAII scoped handle
 * ==================================================================== */

struct ScopedHandle {
    ifcopenshell_ifc_instance_t h;
    explicit ScopedHandle(IfcUtil::IfcBaseClass* e) { h.ptr = e; h.owned = false; }
    ifcopenshell_ifc_instance_t* get() { return &h; }
};

/* ====================================================================
 *  Entity introspection helpers
 * ==================================================================== */

inline bool entity_is_a(IfcUtil::IfcBaseClass* e, const char* type) {
    return e && e->declaration().is(type);
}

inline int find_attr_idx(IfcUtil::IfcBaseClass* e, const char* name) {
    auto* decl = e->declaration().as_entity();
    if (!decl) return -1;
    return static_cast<int>(decl->attribute_index(name));
}

inline std::string get_string_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return "";
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return "";
        if (v.type() == IfcUtil::Argument_STRING || v.type() == IfcUtil::Argument_ENUMERATION)
            return (std::string)v;
    } catch (...) {}
    return "";
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
    std::vector<IfcUtil::IfcBaseClass*> result;
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return result;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return result;
        auto agg = (aggregate_of_instance::ptr)v;
        if (agg) for (auto& item : *agg) { if (item) result.push_back(item); }
    } catch (...) {}
    return result;
}

inline std::vector<IfcUtil::IfcBaseClass*> get_inverse_list(IfcUtil::IfcBaseClass* e, const char* name) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return result;
    try {
        auto inv = be->get_inverse(name);
        if (inv) for (auto& item : *inv) { if (item) result.push_back(item); }
    } catch (...) {}
    return result;
}

/* ====================================================================
 *  Value unwrapping
 * ==================================================================== */

inline Val* unwrap_typed_value(IfcUtil::IfcBaseClass* typed) {
    if (!typed) return make_none();
    try {
        auto inner = typed->get_attribute_value(0);
        if (inner.isNull()) return make_none();
        switch (inner.type()) {
            case IfcUtil::Argument_STRING:      return make_string((std::string)inner);
            case IfcUtil::Argument_DOUBLE:      return make_double((double)inner);
            case IfcUtil::Argument_INT:         return make_int((int)inner);
            case IfcUtil::Argument_BOOL:        return make_bool((bool)inner);
            case IfcUtil::Argument_ENUMERATION: return make_string((std::string)inner);
            default:                            return make_none();
        }
    } catch (...) { return make_none(); }
}

inline Val* attr_to_val(const AttributeValue& v) {
    if (v.isNull()) return make_none();
    switch (v.type()) {
        case IfcUtil::Argument_STRING:      return make_string((std::string)v);
        case IfcUtil::Argument_INT:         return make_int((int)v);
        case IfcUtil::Argument_DOUBLE:      return make_double((double)v);
        case IfcUtil::Argument_BOOL:        return make_bool((bool)v);
        case IfcUtil::Argument_ENUMERATION: return make_string((std::string)v);
        case IfcUtil::Argument_ENTITY_INSTANCE: {
            auto* ref = (IfcUtil::IfcBaseClass*)v;
            if (!ref) return make_none();
            if (ref->declaration().as_type_declaration()) return unwrap_typed_value(ref);
            return make_instance(ref);
        }
        case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
            auto agg = (aggregate_of_instance::ptr)v;
            if (!agg) return make_none();
            auto* list = make_list();
            for (auto& item : *agg) {
                if (!item) { list->list_val.push_back(make_none()); continue; }
                if (item->declaration().as_type_declaration())
                    list->list_val.push_back(unwrap_typed_value(item));
                else
                    list->list_val.push_back(make_instance(item));
            }
            return list;
        }
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: {
            auto vec = (std::vector<double>)v;
            auto* l = make_list();
            for (double d : vec) l->list_val.push_back(make_double(d));
            return l;
        }
        case IfcUtil::Argument_AGGREGATE_OF_INT: {
            auto vec = (std::vector<int>)v;
            auto* l = make_list();
            for (int i : vec) l->list_val.push_back(make_int(i));
            return l;
        }
        case IfcUtil::Argument_AGGREGATE_OF_STRING: {
            auto vec = (std::vector<std::string>)v;
            auto* l = make_list();
            for (auto& s : vec) l->list_val.push_back(make_string(s));
            return l;
        }
        default: return make_none();
    }
}

inline Val* read_attr(IfcUtil::IfcBaseClass* e, const std::string& attr_name) {
    auto* decl = e->declaration().as_entity();
    if (!decl) return nullptr;
    int idx_i = static_cast<int>(decl->attribute_index(attr_name));
    if (idx_i < 0) return nullptr;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx_i));
        return attr_to_val(v);
    } catch (...) { return make_none(); }
}

/* ====================================================================
 *  Pset extraction
 * ==================================================================== */

inline Val* extract_prop_value(IfcUtil::IfcBaseClass* prop) {
    if (!prop) return make_none();
    const auto& tname = prop->declaration().name();

    if (tname == "IfcPropertySingleValue") {
        int nv = find_attr_idx(prop, "NominalValue");
        if (nv < 0) return make_none();
        try {
            auto outer = prop->get_attribute_value(static_cast<size_t>(nv));
            if (outer.isNull()) return make_none();
            if (outer.type() != IfcUtil::Argument_ENTITY_INSTANCE) return make_none();
            auto* typed = (IfcUtil::IfcBaseClass*)outer;
            if (!typed || !typed->declaration().as_type_declaration()) return make_none();
            return unwrap_typed_value(typed);
        } catch (...) { return make_none(); }
    }

    if (tname == "IfcPropertyEnumeratedValue") {
        int ev = find_attr_idx(prop, "EnumerationValues");
        if (ev < 0) return make_none();
        try {
            auto outer = prop->get_attribute_value(static_cast<size_t>(ev));
            if (outer.isNull()) return make_none();
            auto agg = (aggregate_of_instance::ptr)outer;
            if (!agg) return make_none();
            auto* list = make_list();
            for (auto& item : *agg) {
                if (item && item->declaration().as_type_declaration())
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
            auto outer = prop->get_attribute_value(static_cast<size_t>(lv));
            if (outer.isNull()) return make_none();
            auto agg = (aggregate_of_instance::ptr)outer;
            if (!agg) return make_none();
            auto* list = make_list();
            for (auto& item : *agg) {
                if (item && item->declaration().as_type_declaration())
                    list->list_val.push_back(unwrap_typed_value(item));
                else
                    list->list_val.push_back(make_none());
            }
            return list;
        } catch (...) { return make_none(); }
    }

    return make_none();
}

inline Val* extract_pset_props(IfcUtil::IfcBaseClass* defn) {
    if (!defn) return make_none();
    auto* dict = make_dict();
    const auto& tname = defn->declaration().name();

    if (tname == "IfcPropertySet" || tname == "IfcMaterialProperties" ||
        tname == "IfcProfileProperties" || tname == "IfcExtendedMaterialProperties") {

        const char* pa = (tname == "IfcPropertySet") ? "HasProperties"
                       : (tname == "IfcExtendedMaterialProperties") ? "ExtendedProperties"
                       : "Properties";
        for (auto* prop : get_entity_list(defn, pa)) {
            if (!prop) continue;
            dict->dict_val.push_back({get_string_attr(prop, "Name"), extract_prop_value(prop)});
        }
    } else if (tname == "IfcElementQuantity") {
        for (auto* q : get_entity_list(defn, "Quantities")) {
            if (!q || !entity_is_a(q, "IfcPhysicalSimpleQuantity")) continue;
            auto* edecl = q->declaration().as_entity();
            if (!edecl || edecl->all_attributes().size() < 4) continue;
            try {
                auto v = q->get_attribute_value(3);
                dict->dict_val.push_back({get_string_attr(q, "Name"), attr_to_val(v)});
            } catch (...) {
                dict->dict_val.push_back({get_string_attr(q, "Name"), make_none()});
            }
        }
    }
    return dict;
}

inline std::vector<std::pair<std::string, Val*>>
get_all_psets(IfcUtil::IfcBaseClass* entity) {
    std::vector<std::pair<std::string, Val*>> result;
    ScopedHandle sh(entity);
    uint32_t count = 0;
    ifcopenshell_ifc_instance_t** handles =
        ifcopenshell_element_get_pset_ids(sh.get(), false, false, true, &count);
    if (!handles || count == 0) return result;

    for (uint32_t i = 0; i < count; ++i) {
        auto* h = handles[i];
        if (!h || !h->ptr) continue;
        std::string nm = get_string_attr(h->ptr, "Name");
        Val* props = extract_pset_props(h->ptr);

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
    ifcopenshell_free_instance_array(handles, count);
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
        auto* key_node = ifcopenshell_selector_node_child(kl, i);
        if (!key_node || ifcopenshell_selector_node_kind(key_node) != IFCSEL_NODE_KEY) continue;
        if (ifcopenshell_selector_node_child_count(key_node) == 0) continue;

        auto* val_node = ifcopenshell_selector_node_child(key_node, 0);
        if (!val_node) continue;

        KeyEntry ke;
        auto vk = ifcopenshell_selector_node_kind(val_node);

        if (vk == IFCSEL_NODE_REGEX_STRING) {
            ke.is_regex = true;
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto* tok = ifcopenshell_selector_node_child(val_node, 0);
                const char* txt = ifcopenshell_selector_node_text(tok);
                ke.text = txt ? txt : "";
            }
            try { ke.pattern = std::regex(ke.text); }
            catch (...) { continue; }
        } else if (vk == IFCSEL_NODE_QUOTED_STRING) {
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto* tok = ifcopenshell_selector_node_child(val_node, 0);
                const char* txt = ifcopenshell_selector_node_text(tok);
                ke.text = unescape_quoted(txt ? txt : "");
            }
        } else {
            if (ifcopenshell_selector_node_child_count(val_node) > 0) {
                auto* tok = ifcopenshell_selector_node_child(val_node, 0);
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

inline Val* resolve_predefined_type(IfcUtil::IfcBaseClass* e) {
    ScopedHandle sh(e);
    auto* type_h = ifcopenshell_element_get_type(sh.get());
    IfcUtil::IfcBaseClass* type_e = nullptr;
    if (type_h) { type_e = type_h->ptr; ifcopenshell_ifc_instance_destroy(type_h); }

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

inline Val* resolve_classification(IfcUtil::IfcBaseClass* e) {
    auto* list = make_list();
    for (auto* rel : get_inverse_list(e, "HasAssociations")) {
        if (!entity_is_a(rel, "IfcRelAssociatesClassification")) continue;
        auto* ref = get_entity_ref(rel, "RelatingClassification");
        if (ref) list->list_val.push_back(make_instance(ref));
    }
    return list;
}

inline Val* resolve_groups(IfcUtil::IfcBaseClass* e) {
    auto* list = make_list();
    for (auto* rel : get_inverse_list(e, "HasAssignments")) {
        if (!entity_is_a(rel, "IfcRelAssignsToGroup")) continue;
        auto* grp = get_entity_ref(rel, "RelatingGroup");
        if (grp) list->list_val.push_back(make_instance(grp));
    }
    return list;
}

inline Val* resolve_occurrences(IfcUtil::IfcBaseClass* e) {
    auto* list = make_list();
    for (auto* rel : get_inverse_list(e, "ObjectTypeOf")) {
        if (!entity_is_a(rel, "IfcRelDefinesByType")) continue;
        for (auto* obj : get_entity_list(rel, "RelatedObjects"))
            list->list_val.push_back(make_instance(obj));
    }
    return list;
}

inline Val* resolve_styles(IfcUtil::IfcBaseClass* e) {
    auto* list = make_list();
    if (!e) return list;
    ScopedHandle sh(e);
    uint32_t n = 0;
    auto** arr = ifcopenshell_util_element_get_styles(sh.get(), &n);
    if (!arr) return list;
    for (uint32_t i = 0; i < n; ++i) {
        if (arr[i] && arr[i]->ptr) list->list_val.push_back(make_instance(arr[i]->ptr));
    }
    ifcopenshell_free_instance_array(arr, n);
    return list;
}

inline Val* resolve_systems(IfcUtil::IfcBaseClass* e, bool zones_only) {
    auto* list = make_list();
    for (auto* rel : get_inverse_list(e, "HasAssignments")) {
        if (!entity_is_a(rel, "IfcRelAssignsToGroup")) continue;
        auto* grp = get_entity_ref(rel, "RelatingGroup");
        if (!grp) continue;
        bool is_zone = entity_is_a(grp, "IfcZone");
        bool is_system = entity_is_a(grp, "IfcSystem") && !entity_is_a(grp, "IfcStructuralAnalysisModel");
        if ((zones_only && is_zone) || (!zones_only && is_system && !is_zone)) {
            list->list_val.push_back(make_instance(grp));
        }
    }
    return list;
}

inline Val* resolve_profiles(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    auto* list = make_list();
    if (!e) return list;

    ScopedHandle sh(e);
    auto* mat_h = ifcopenshell_element_get_material(sh.get(), true, true);
    auto* mat = mat_h ? mat_h->ptr : nullptr;
    if (mat_h) ifcopenshell_ifc_instance_destroy(mat_h);
    if (mat && entity_is_a(mat, "IfcMaterialProfileSet")) {
        for (auto* mp : get_entity_list(mat, "MaterialProfiles")) {
            auto* profile = get_entity_ref(mp, "Profile");
            if (profile) list->list_val.push_back(make_instance(profile));
        }
        return list;
    }

    if (!file) return list;
    ifcopenshell_ifc_file_t fh{file, false};
    auto* rep_h = ifcopenshell_representation_get_product_representation(
        &fh, sh.get(), nullptr, "Model", "Body", "MODEL_VIEW");
    if (!rep_h) return list;

    uint32_t n = 0;
    auto** items = ifcopenshell_representation_resolve_base_items(&fh, rep_h, &n);
    ifcopenshell_ifc_instance_destroy(rep_h);
    if (!items) return list;
    for (uint32_t i = 0; i < n; ++i) {
        auto* item = items[i] ? items[i]->ptr : nullptr;
        if (!item || !entity_is_a(item, "IfcExtrudedAreaSolid")) continue;
        auto* swept = get_entity_ref(item, "SweptArea");
        if (swept) list->list_val.push_back(make_instance(swept));
    }
    ifcopenshell_free_instance_array(items, n);
    return list;
}

inline Val* resolve_xyz(IfcUtil::IfcBaseClass* e, const std::string& k) {
    auto* placement_e = get_entity_ref(e, "ObjectPlacement");
    if (!placement_e) return make_none();
    double matrix[16];
    ScopedHandle sh(placement_e);
    if (!ifcopenshell_placement_get_local_placement(sh.get(), matrix)) return make_none();
    int ci = (k == "x") ? 0 : (k == "y") ? 1 : 2;
    return make_double(matrix[ci * 4 + 3]);
}

inline bool get_numeric_attr(IfcUtil::IfcBaseClass* e, const char* attr, double* out) {
    int idx = find_attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return false;
        if (v.type() == IfcUtil::Argument_DOUBLE) {
            *out = (double)v;
            return true;
        }
        if (v.type() == IfcUtil::Argument_INT) {
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

inline bool get_typed_numeric_value(IfcUtil::IfcBaseClass* e, double* out) {
    if (!e || !out) return false;
    try {
        auto inner = e->get_attribute_value(0);
        if (inner.isNull()) return false;
        if (inner.type() == IfcUtil::Argument_DOUBLE) {
            *out = (double)inner;
            return true;
        }
        if (inner.type() == IfcUtil::Argument_INT) {
            *out = (double)(int)inner;
            return true;
        }
    } catch (...) {}
    return false;
}

inline bool apply_wcs_inverse(IfcParse::IfcFile* file, double& x, double& y, double& z) {
    if (!file) return false;
    auto ctxs = file->instances_by_type("IfcGeometricRepresentationContext");
    if (!ctxs) return false;
    IfcUtil::IfcBaseClass* chosen = nullptr;
    for (auto& ctx : *ctxs) {
        if (!ctx) continue;
        chosen = ctx;
        if (get_string_attr(ctx, "ContextType") == "Model") break;
    }
    if (!chosen) return false;
    auto* wcs = get_entity_ref(chosen, "WorldCoordinateSystem");
    if (!wcs) return false;
    ScopedHandle sh(wcs);
    double m[16];
    if (!ifcopenshell_placement_get_axis2placement(sh.get(), m)) return false;
    double inv[16];
    if (!invert_rigid4(m, inv)) return false;
    transform_point4(inv, x, y, z);
    return true;
}

inline bool get_map_conversion(IfcParse::IfcFile* file, double& eastings, double& northings,
                               double& orthogonal_height, double& xaa, double& xao,
                               double& scale, double& factor_x, double& factor_y, double& factor_z) {
    eastings = northings = orthogonal_height = 0.0;
    xaa = 1.0;
    xao = 0.0;
    scale = factor_x = factor_y = factor_z = 1.0;
    if (!file) return false;

    auto conversions = file->instances_by_type("IfcCoordinateOperation");
    if (!conversions || conversions->size() == 0) return false;
    auto* conversion = (*conversions)[0];
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
        auto* first = get_entity_ref(conversion, "FirstCoordinate");
        auto* second = get_entity_ref(conversion, "SecondCoordinate");
        get_typed_numeric_value(first, &eastings);
        get_typed_numeric_value(second, &northings);
        get_numeric_attr(conversion, "Height", &orthogonal_height);
        return true;
    }

    return false;
}

inline Val* resolve_map_coordinate(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e, const std::string& k) {
    auto* placement_e = get_entity_ref(e, "ObjectPlacement");
    if (!placement_e) return make_none();
    double matrix[16];
    ScopedHandle sh(placement_e);
    if (!ifcopenshell_placement_get_local_placement(sh.get(), matrix)) return make_none();
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

inline Val* apply_key(IfcParse::IfcFile* file, const Val* cur, const KeyEntry& key);

inline Val* apply_key_to_list(IfcParse::IfcFile* file, const Val* cur, const KeyEntry& key) {
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

    auto* results = make_list();
    for (auto* item : cur->list_val) {
        Val* sub = apply_key(file, item, key);
        if (!sub) { results->list_val.push_back(make_none()); continue; }
        if (sub->kind == IFCSEL_VALUE_LIST) {
            for (auto* sv : sub->list_val) results->list_val.push_back(sv);
            sub->list_val.clear();
            delete sub;
        } else {
            results->list_val.push_back(sub);
        }
    }
    return results;
}

inline Val* apply_key(IfcParse::IfcFile* file, const Val* cur, const KeyEntry& key) {
    if (!cur || cur->kind == IFCSEL_VALUE_NONE) return make_none();

    if (cur->kind == IFCSEL_VALUE_LIST)
        return apply_key_to_list(file, cur, key);

    if (cur->kind == IFCSEL_VALUE_DICT) {
        if (key.is_regex) {
            std::vector<Val*> matches;
            for (auto& kv : cur->dict_val) {
                if (!regex_match_start(kv.first, key.pattern)) continue;
                if (kv.second->kind == IFCSEL_VALUE_LIST) {
                    for (auto* sv : kv.second->list_val) matches.push_back(clone_val(sv));
                } else {
                    matches.push_back(clone_val(kv.second));
                }
            }
            if (matches.empty()) return make_none();
            if (matches.size() == 1) return matches[0];
            auto* l = make_list();
            for (auto* v : matches) l->list_val.push_back(v);
            return l;
        } else {
            for (auto& kv : cur->dict_val)
                if (kv.first == key.text) return clone_val(kv.second);
            return make_none();
        }
    }

    if (cur->kind != IFCSEL_VALUE_INSTANCE) return make_none();
    auto* e = cur->inst_val;
    if (!e) return make_none();

    const std::string& k = key.text;

    if (!key.is_regex) {
        if (k == "type") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_type(sh.get());
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "material" || k == "mat") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_material(sh.get(), true, true);
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "materials" || k == "mats") {
            ScopedHandle sh(e);
            auto* mh = ifcopenshell_element_get_material(sh.get(), true, true);
            if (!mh) return make_none();
            auto* mat = mh->ptr;
            ifcopenshell_ifc_instance_destroy(mh);
            if (!mat) return make_none();
            auto* list = make_list();
            if (entity_is_a(mat, "IfcMaterial")) {
                list->list_val.push_back(make_instance(mat));
            } else if (entity_is_a(mat, "IfcMaterialLayerSet")) {
                for (auto* lay : get_entity_list(mat, "MaterialLayers")) {
                    auto* m = get_entity_ref(lay, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialProfileSet")) {
                for (auto* pr : get_entity_list(mat, "MaterialProfiles")) {
                    auto* m = get_entity_ref(pr, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialConstituentSet")) {
                for (auto* co : get_entity_list(mat, "MaterialConstituents")) {
                    auto* m = get_entity_ref(co, "Material");
                    if (m) list->list_val.push_back(make_instance(m));
                }
            } else if (entity_is_a(mat, "IfcMaterialList")) {
                for (auto* m : get_entity_list(mat, "Materials"))
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
            auto* list = make_list();
            for (auto* item : get_entity_list(e, attr)) list->list_val.push_back(make_instance(item));
            return list;
        }
        if (k == "container") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_container(sh.get(), false, nullptr);
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "space") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_container(sh.get(), false, "IfcSpace");
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "storey") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_container(sh.get(), false, "IfcBuildingStorey");
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "building") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_container(sh.get(), false, "IfcBuilding");
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "site") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_container(sh.get(), false, "IfcSite");
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "parent") {
            ScopedHandle sh(e);
            auto* h = ifcopenshell_element_get_parent(sh.get());
            if (!h) return make_none();
            Val* r = make_instance(h->ptr);
            ifcopenshell_ifc_instance_destroy(h);
            return r;
        }
        if (k == "types" || k == "occurrences") return resolve_occurrences(e);
        if (k == "count")           return make_int(1);
        if (k == "class")           return make_string(e->declaration().name());
        if (k == "predefined_type") return resolve_predefined_type(e);
        if (k == "id")              return make_int(static_cast<int64_t>(e->id()));
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
        auto* list = make_list();
        for (auto* v : matching) list->list_val.push_back(v);
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
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* entity,
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
