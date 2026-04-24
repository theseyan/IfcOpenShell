// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/value.h"
#include "ifcopenshell_api_internal.hpp"
#include "selector_internal.h"
#include "entity_introspection.hpp"
#include "placement_helpers.hpp"
#include "api_pset_props.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <regex>
#include <string>
#include <vector>

/* ------------------------------------------------------------------ */
/*  Public opaque keylist                                               */
/* ------------------------------------------------------------------ */

struct ifcopenshell_selector_keylist_t {
    std::vector<KeyEntry> keys;
};

/* ------------------------------------------------------------------ */
/*  Implementation                                                      */
/* ------------------------------------------------------------------ */

namespace {

inline void set_error(const std::string& m) { ifcopenshell::capi::set_last_error(m); }

inline IfcUtil::IfcBaseEntity* as_entity(IfcUtil::IfcBaseClass* e) {
    return e ? dynamic_cast<IfcUtil::IfcBaseEntity*>(e) : nullptr;
}

inline std::string lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

bool icontains(const std::string& s, const std::string& sub) {
    auto a = lower(s), b = lower(sub);
    return a.find(b) != std::string::npos;
}

/* ---------- Value (None|Bool|Int|Double|String|Instance|List) helpers ---------- */

bool val_is_none(const ifcopenshell_value_t* v) {
    return !v || v->kind == IFCSEL_VALUE_NONE;
}

std::string val_to_string(const ifcopenshell_value_t* v) {
    if (val_is_none(v)) return "";
    switch (v->kind) {
        case IFCSEL_VALUE_STRING: return v->s_val;
        case IFCSEL_VALUE_INT:    return std::to_string(v->i_val);
        case IFCSEL_VALUE_DOUBLE: {
            std::string s = std::to_string(v->d_val);
            return s;
        }
        case IFCSEL_VALUE_BOOL:   return v->b_val ? "True" : "False";
        default: return "";
    }
}

bool val_truthy(const ifcopenshell_value_t* v) {
    if (val_is_none(v)) return false;
    switch (v->kind) {
        case IFCSEL_VALUE_BOOL:   return v->b_val;
        case IFCSEL_VALUE_INT:    return v->i_val != 0;
        case IFCSEL_VALUE_DOUBLE: return v->d_val != 0.0;
        case IFCSEL_VALUE_STRING: return !v->s_val.empty();
        case IFCSEL_VALUE_INSTANCE: return v->inst_val != nullptr;
        case IFCSEL_VALUE_LIST:   return !v->list_val.empty();
        case IFCSEL_VALUE_DICT:   return !v->dict_val.empty();
        default: return false;
    }
}

/* Compare a Val* (dict-extracted current value) with the user-supplied value
   for the “return early on equality” shortcut. Falls back to false when the
   shapes don't match — over-conservative is safe (we just do a redundant
   write). */
bool vals_equal(const Val* a, const ifcopenshell_value_t* b) {
    if (!a && val_is_none(b)) return true;
    if (!a) return false;
    if (a->kind == IFCSEL_VALUE_NONE && val_is_none(b)) return true;
    if (val_is_none(b)) return a->kind == IFCSEL_VALUE_NONE;
    if (a->kind != b->kind) {
        if ((a->kind == IFCSEL_VALUE_INT && b->kind == IFCSEL_VALUE_DOUBLE) ||
            (a->kind == IFCSEL_VALUE_DOUBLE && b->kind == IFCSEL_VALUE_INT)) {
            double da = (a->kind == IFCSEL_VALUE_INT) ? (double)a->i_val : a->d_val;
            double db = (b->kind == IFCSEL_VALUE_INT) ? (double)b->i_val : b->d_val;
            return da == db;
        }
        return false;
    }
    switch (a->kind) {
        case IFCSEL_VALUE_BOOL:   return a->b_val == b->b_val;
        case IFCSEL_VALUE_INT:    return a->i_val == b->i_val;
        case IFCSEL_VALUE_DOUBLE: return a->d_val == b->d_val;
        case IFCSEL_VALUE_STRING: return a->s_val == b->s_val;
        case IFCSEL_VALUE_INSTANCE: return a->inst_val == b->inst_val;
        case IFCSEL_VALUE_LIST: {
            if (a->list_val.size() != b->list_val.size()) return false;
            for (size_t i = 0; i < a->list_val.size(); ++i)
                if (!vals_equal(a->list_val[i], b->list_val[i])) return false;
            return true;
        }
        default: return false;
    }
}

/* Helpers around the existing C ABI ------------------------------------ */

IfcUtil::IfcBaseClass* call_get_type(IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    ScopedHandle sh(e);
    auto* h = ifcopenshell_element_get_type(sh.get());
    IfcUtil::IfcBaseClass* r = h ? h->ptr : nullptr;
    if (h) ifcopenshell_ifc_instance_destroy(h);
    return r;
}

IfcUtil::IfcBaseClass* call_get_material(IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    ScopedHandle sh(e);
    auto* h = ifcopenshell_element_get_material(sh.get(), true, true);
    IfcUtil::IfcBaseClass* r = h ? h->ptr : nullptr;
    if (h) ifcopenshell_ifc_instance_destroy(h);
    return r;
}

std::vector<IfcUtil::IfcBaseClass*> call_get_materials(IfcUtil::IfcBaseClass* e) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    auto* mat = call_get_material(e);
    if (!mat) return out;
    if (entity_is_a(mat, "IfcMaterial")) {
        out.push_back(mat);
    } else if (entity_is_a(mat, "IfcMaterialLayerSet")) {
        for (auto* lay : get_entity_list(mat, "MaterialLayers")) {
            auto* m = ifcapi::get_entity_ref(lay, "Material");
            if (m) out.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialProfileSet")) {
        for (auto* pr : get_entity_list(mat, "MaterialProfiles")) {
            auto* m = ifcapi::get_entity_ref(pr, "Material");
            if (m) out.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialConstituentSet")) {
        for (auto* co : get_entity_list(mat, "MaterialConstituents")) {
            auto* m = ifcapi::get_entity_ref(co, "Material");
            if (m) out.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialList")) {
        for (auto* m : get_entity_list(mat, "Materials")) out.push_back(m);
    }
    return out;
}

std::vector<IfcUtil::IfcBaseClass*> call_get_styles(IfcUtil::IfcBaseClass* e) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    if (!e) return out;
    ScopedHandle sh(e);
    uint32_t n = 0;
    auto** arr = ifcopenshell_util_element_get_styles(sh.get(), &n);
    if (!arr) return out;
    for (uint32_t i = 0; i < n; ++i) if (arr[i] && arr[i]->ptr) out.push_back(arr[i]->ptr);
    ifcopenshell_free_instance_array(arr, n);
    return out;
}

std::vector<IfcUtil::IfcBaseClass*> call_get_classification(IfcUtil::IfcBaseClass* e) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    if (!e) return out;
    ScopedHandle sh(e);
    uint32_t n = 0;
    auto** arr = ifcopenshell_util_classification_get_references(sh.get(), true, &n);
    if (!arr) return out;
    for (uint32_t i = 0; i < n; ++i) if (arr[i] && arr[i]->ptr) out.push_back(arr[i]->ptr);
    ifcopenshell_free_instance_array(arr, n);
    return out;
}

IfcUtil::IfcBaseClass* call_get_container(IfcUtil::IfcBaseClass* e, const char* cls) {
    if (!e) return nullptr;
    ScopedHandle sh(e);
    auto* h = ifcopenshell_element_get_container(sh.get(), false, cls);
    IfcUtil::IfcBaseClass* r = h ? h->ptr : nullptr;
    if (h) ifcopenshell_ifc_instance_destroy(h);
    return r;
}

IfcUtil::IfcBaseClass* call_get_parent(IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    ScopedHandle sh(e);
    auto* h = ifcopenshell_element_get_parent(sh.get());
    IfcUtil::IfcBaseClass* r = h ? h->ptr : nullptr;
    if (h) ifcopenshell_ifc_instance_destroy(h);
    return r;
}

/* Get all psets (de-duplicated by name) of e — name → property-dict-Val. */
std::vector<std::pair<std::string, Val*>> all_psets(IfcUtil::IfcBaseClass* e) {
    return get_all_psets(e);
}

/* Find the pset_instance (IfcPropertySet/IfcElementQuantity) with the given
   Name on element. Returns nullptr if none. */
IfcUtil::IfcBaseClass* find_pset_instance(IfcUtil::IfcBaseClass* e, const std::string& name) {
    if (!e) return nullptr;
    ScopedHandle sh(e);
    uint32_t n = 0;
    auto** arr = ifcopenshell_element_get_pset_ids(sh.get(), false, false, true, &n);
    IfcUtil::IfcBaseClass* result = nullptr;
    if (arr) {
        for (uint32_t i = 0; i < n; ++i) {
            auto* h = arr[i];
            if (!result && h && h->ptr && get_string_attr(h->ptr, "Name") == name) {
                result = h->ptr;
            }
        }
        ifcopenshell_free_instance_array(arr, n);
    }
    return result;
}

/* Build an ifcopenshell_pset_props_t containing a single key/value entry
   for use with edit_pset / edit_qto. value is interpreted analogously to
   the Python {key: value} mapping. */
void props_set_one(ifcopenshell_pset_props_t* props, const char* key,
                   const ifcopenshell_value_t* v, bool force_double) {
    if (!props) return;
    if (force_double) {
        double d = 0.0;
        if (!val_is_none(v)) {
            if (v->kind == IFCSEL_VALUE_DOUBLE) d = v->d_val;
            else if (v->kind == IFCSEL_VALUE_INT) d = (double)v->i_val;
            else if (v->kind == IFCSEL_VALUE_STRING) {
                try { d = std::stod(v->s_val); } catch (...) { d = 0.0; }
            } else if (v->kind == IFCSEL_VALUE_BOOL) d = v->b_val ? 1.0 : 0.0;
        }
        ifcopenshell_pset_props_set_double(props, key, d);
        return;
    }
    if (val_is_none(v)) { ifcopenshell_pset_props_set_null(props, key); return; }
    switch (v->kind) {
        case IFCSEL_VALUE_BOOL:   ifcopenshell_pset_props_set_bool(props, key, v->b_val); break;
        case IFCSEL_VALUE_INT:    ifcopenshell_pset_props_set_int(props, key, v->i_val); break;
        case IFCSEL_VALUE_DOUBLE: ifcopenshell_pset_props_set_double(props, key, v->d_val); break;
        case IFCSEL_VALUE_STRING: ifcopenshell_pset_props_set_string(props, key, v->s_val.c_str()); break;
        case IFCSEL_VALUE_INSTANCE: {
            ScopedHandle sh(v->inst_val);
            ifcopenshell_pset_props_set_instance(props, key, sh.get());
            break;
        }
        case IFCSEL_VALUE_LIST: {
            // Distinguish string-list / int-list / double-list.
            bool all_str = true, all_int = true, all_dbl = true;
            for (auto* it : v->list_val) {
                if (!it || it->kind != IFCSEL_VALUE_STRING) all_str = false;
                if (!it || it->kind != IFCSEL_VALUE_INT) all_int = false;
                if (!it || (it->kind != IFCSEL_VALUE_DOUBLE && it->kind != IFCSEL_VALUE_INT)) all_dbl = false;
            }
            if (all_str) {
                std::vector<const char*> ptrs;
                std::vector<std::string> hold;
                hold.reserve(v->list_val.size());
                for (auto* it : v->list_val) hold.push_back(it->s_val);
                for (auto& s : hold) ptrs.push_back(s.c_str());
                ifcopenshell_pset_props_set_string_list(props, key, ptrs.data(), (uint32_t)ptrs.size());
            } else if (all_int) {
                std::vector<int64_t> vs;
                for (auto* it : v->list_val) vs.push_back(it->i_val);
                ifcopenshell_pset_props_set_int_list(props, key, vs.data(), (uint32_t)vs.size());
            } else if (all_dbl) {
                std::vector<double> vs;
                for (auto* it : v->list_val) vs.push_back(it->kind == IFCSEL_VALUE_INT ? (double)it->i_val : it->d_val);
                ifcopenshell_pset_props_set_double_list(props, key, vs.data(), (uint32_t)vs.size());
            } else {
                std::vector<const char*> ptrs;
                std::vector<std::string> hold;
                hold.reserve(v->list_val.size());
                for (auto* it : v->list_val) hold.push_back(it ? it->s_val : "");
                for (auto& s : hold) ptrs.push_back(s.c_str());
                ifcopenshell_pset_props_set_string_list(props, key, ptrs.data(), (uint32_t)ptrs.size());
            }
            break;
        }
        default:
            ifcopenshell_pset_props_set_null(props, key);
            break;
    }
}

void edit_pset_one(ifcopenshell_ifc_file_t* fh, IfcUtil::IfcBaseClass* pset,
                   const char* key, const ifcopenshell_value_t* v) {
    auto* props = ifcopenshell_pset_props_new();
    props_set_one(props, key, v, false);
    ScopedHandle sh(pset);
    ifcopenshell_api_pset_edit_pset(fh, sh.get(), nullptr, props, nullptr, false);
    ifcopenshell_pset_props_free(props);
}

void edit_qto_one(ifcopenshell_ifc_file_t* fh, IfcUtil::IfcBaseClass* qto,
                  const char* key, const ifcopenshell_value_t* v, bool force_double = true) {
    auto* props = ifcopenshell_pset_props_new();
    props_set_one(props, key, v, force_double);
    ScopedHandle sh(qto);
    ifcopenshell_api_pset_edit_qto(fh, sh.get(), nullptr, props, nullptr);
    ifcopenshell_pset_props_free(props);
}

/* ---------- regex match helper (anchored start, like re.match) */
bool re_match(const std::regex& p, const std::string& s) {
    std::smatch m;
    return std::regex_search(s, m, p, std::regex_constants::match_continuous);
}

/* ====================================================================
 *  predefined_type closure                                              */

void apply_set_predefined_type(IfcParse::IfcFile* /*file*/, IfcUtil::IfcBaseClass* element,
                               const ifcopenshell_value_t* value, bool is_type) {
    auto* be = as_entity(element);
    if (!be) return;
    auto* decl = be->declaration().as_entity();
    if (!decl) return;
    int idx = static_cast<int>(decl->attribute_index("PredefinedType"));
    if (idx < 0) return;
    const auto& attrs = decl->all_attributes();
    if ((size_t)idx >= attrs.size()) return;
    const IfcParse::attribute* attr = attrs[(size_t)idx];

    uint32_t enum_n = 0;
    char** enum_arr = ifcopenshell_util_attribute_get_enum_items(
        reinterpret_cast<const void*>(attr), &enum_n);
    std::vector<std::string> enum_items;
    if (enum_arr) {
        for (uint32_t i = 0; i < enum_n; ++i) enum_items.emplace_back(enum_arr[i] ? enum_arr[i] : "");
        ifcopenshell_free_string_array(enum_arr, enum_n);
    }

    std::string current_pt = get_string_attr(element, "PredefinedType");
    const char* type_attr_name = is_type ? "ElementType" : "ObjectType";

    bool value_is_none = val_is_none(value);
    std::string value_str = val_to_string(value);

    if (value_is_none || value_str == "NOTDEFINED" || value_str == "USERDEFINED") {
        try { element->set_attribute_value((size_t)idx, std::string("NOTDEFINED")); } catch (...) {}
        int t_idx = ifcapi::find_attr_idx(element, type_attr_name);
        if (t_idx >= 0) {
            try { element->unset_attribute_value((size_t)t_idx); } catch (...) {}
        }
        return;
    }

    bool in_enum = std::find(enum_items.begin(), enum_items.end(), value_str) != enum_items.end();
    if (in_enum) {
        if (current_pt == value_str) return;
        try { element->set_attribute_value((size_t)idx, value_str); } catch (...) {}
        return;
    }

    if (current_pt != "USERDEFINED") {
        try { element->set_attribute_value((size_t)idx, std::string("USERDEFINED")); } catch (...) {}
    }
    int t_idx = ifcapi::find_attr_idx(element, type_attr_name);
    if (t_idx >= 0) {
        try { element->set_attribute_value((size_t)t_idx, value_str); } catch (...) {}
    }
}

/* ====================================================================
 *  process_pset_prop_value closure                                       */

/* Returns one of:
 *   PSET_VAL_USE_VALUE: write *value* unchanged
 *   PSET_VAL_USE_LIST:  write the assembled string list (out_list)
 *   PSET_VAL_SKIP:      no-op (current value already matches)
 *   PSET_VAL_ERROR:     raise (error text in last_error)
 */
enum PsetPVResult { PV_USE_VALUE, PV_USE_LIST, PV_SKIP, PV_ERROR };

PsetPVResult process_pset_prop_value(
    IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset,
    const std::string& prop, const ifcopenshell_value_t* value,
    const Val* current_value /* may be nullptr */, const std::string& concat,
    std::vector<std::string>& out_list)
{
    if (val_is_none(value) || value->kind != IFCSEL_VALUE_STRING) return PV_USE_VALUE;
    const std::string& vs = value->s_val;

    bool current_is_list = current_value && current_value->kind == IFCSEL_VALUE_LIST;
    bool current_is_missing_or_list = !current_value ||
        current_value->kind == IFCSEL_VALUE_NONE || current_is_list;
    if (!current_is_missing_or_list) return PV_USE_VALUE;

    if (current_is_list) {
        std::vector<std::string> enum_values;
        std::string buf;
        size_t pos = 0;
        while (pos <= vs.size()) {
            size_t found = vs.find(concat, pos);
            if (found == std::string::npos) {
                enum_values.push_back(vs.substr(pos));
                break;
            }
            enum_values.push_back(vs.substr(pos, found - pos));
            pos = found + concat.size();
        }
        if (enum_values.size() == current_value->list_val.size()) {
            std::vector<std::string> a = enum_values;
            std::vector<std::string> b;
            for (auto* it : current_value->list_val)
                b.push_back(it && it->kind == IFCSEL_VALUE_STRING ? it->s_val : "");
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            if (a == b) return PV_SKIP;
        }
    }

    auto* tmpl = ifcopenshell_util_pset_get_template(file->schema()->name().c_str());
    if (!tmpl) return PV_USE_VALUE;
    std::string pset_name = get_string_attr(pset, "Name");
    auto* psh = ifcopenshell_util_pset_template_get_by_name(tmpl, pset_name.c_str());
    if (!psh) return PV_USE_VALUE;
    auto* pset_template = psh->ptr;
    IfcUtil::IfcBaseClass* prop_template = nullptr;
    for (auto* pt : get_entity_list(pset_template, "HasPropertyTemplates")) {
        if (!pt) continue;
        std::string pname;
        try {
            auto av = pt->get_attribute_value(2);
            if (!av.isNull()) pname = (std::string)av;
        } catch (...) {}
        if (pname == prop) { prop_template = pt; break; }
    }
    if (!prop_template) {
        ifcopenshell_ifc_instance_destroy(psh);
        return PV_USE_VALUE;
    }

    std::string template_type;
    try {
        auto av = prop_template->get_attribute_value(4);
        if (!av.isNull()) template_type = (std::string)av;
    } catch (...) {}

    if (template_type != "P_ENUMERATEDVALUE") {
        ifcopenshell_ifc_instance_destroy(psh);
        return PV_USE_VALUE;
    }

    IfcUtil::IfcBaseClass* enumeration = nullptr;
    try {
        auto av = prop_template->get_attribute_value(7);
        if (!av.isNull() && av.type() == IfcUtil::Argument_ENTITY_INSTANCE)
            enumeration = (IfcUtil::IfcBaseClass*)av;
    } catch (...) {}

    if (!enumeration) {
        ifcopenshell_ifc_instance_destroy(psh);
        out_list = { vs };
        return PV_USE_LIST;
    }

    std::vector<std::string> available;
    try {
        auto av = enumeration->get_attribute_value(1);
        if (!av.isNull()) {
            auto agg = (aggregate_of_instance::ptr)av;
            if (agg) for (auto& it : *agg) {
                if (!it) continue;
                try {
                    auto inner = it->get_attribute_value(0);
                    if (!inner.isNull()) available.push_back((std::string)inner);
                } catch (...) {}
            }
        }
    } catch (...) {}

    ifcopenshell_ifc_instance_destroy(psh);

    if (std::find(available.begin(), available.end(), vs) != available.end()) {
        out_list = { vs };
        return PV_USE_LIST;
    }

    std::vector<std::string> enum_values;
    {
        size_t pos = 0;
        while (pos <= vs.size()) {
            size_t found = vs.find(concat, pos);
            if (found == std::string::npos) {
                enum_values.push_back(vs.substr(pos));
                break;
            }
            enum_values.push_back(vs.substr(pos, found - pos));
            pos = found + concat.size();
        }
    }
    for (const auto& ev : enum_values) {
        if (std::find(available.begin(), available.end(), ev) == available.end()) {
            std::string msg = "Error setting pset enum property.\nInvalid enum values for property '" +
                              prop + "' in pset '" + pset_name + "': '";
            for (size_t i = 0; i < enum_values.size(); ++i) {
                if (i) msg += ", ";
                msg += enum_values[i];
            }
            msg += "'.\nPossible enum values for this property: ";
            for (size_t i = 0; i < available.size(); ++i) {
                if (i) msg += ", ";
                msg += available[i];
            }
            msg += ".";
            set_error(msg);
            return PV_ERROR;
        }
    }
    out_list = enum_values;
    return PV_USE_LIST;
}

/* ====================================================================
 *  Cursor                                                                */

struct DictView {
    IfcUtil::IfcBaseClass* pset = nullptr;     // owning pset/qto entity
    Val* dict = nullptr;                       // owned DICT
    DictView() = default;
    DictView(DictView&& o) noexcept { pset = o.pset; dict = o.dict; o.dict = nullptr; }
    DictView& operator=(DictView&& o) noexcept { delete dict; pset = o.pset; dict = o.dict; o.dict = nullptr; return *this; }
    DictView(const DictView&) = delete;
    DictView& operator=(const DictView&) = delete;
    ~DictView() { delete dict; }
};

struct Cursor {
    enum K { K_NONE, K_INSTANCE, K_DICT, K_LIST } kind = K_NONE;
    IfcUtil::IfcBaseClass* inst = nullptr;
    DictView dict;
    std::vector<Cursor> items;

    Cursor() = default;
    Cursor(Cursor&&) noexcept = default;
    Cursor& operator=(Cursor&&) noexcept = default;
    Cursor(const Cursor&) = delete;
    Cursor& operator=(const Cursor&) = delete;

    static Cursor instance(IfcUtil::IfcBaseClass* e) {
        Cursor c; c.kind = K_INSTANCE; c.inst = e; return c;
    }
    static Cursor none() { Cursor c; c.kind = K_NONE; return c; }
    static Cursor dictv(IfcUtil::IfcBaseClass* pset, Val* dict_val) {
        Cursor c; c.kind = K_DICT; c.dict.pset = pset; c.dict.dict = dict_val; return c;
    }
    static Cursor listv(std::vector<Cursor>&& it) {
        Cursor c; c.kind = K_LIST; c.items = std::move(it); return c;
    }
};

/* Look up a key inside a dict-pset and return the cloned Val*, or nullptr if missing. */
Val* dict_get(const DictView& d, const std::string& key) {
    if (!d.dict) return nullptr;
    for (auto& kv : d.dict->dict_val)
        if (kv.first == key) return clone_val(kv.second);
    return nullptr;
}

/* ====================================================================
 *  Forward declaration                                                   */

int do_set(IfcParse::IfcFile* file,
           ifcopenshell_ifc_file_t* file_h,
           Cursor cur,
           const std::vector<KeyEntry>& keys,
           size_t start_idx,
           const ifcopenshell_value_t* value,
           const std::string& concat);

/* ====================================================================
 *  Helper: try to set an attribute on `e`, with type-cast fallback        */

void setattr_with_cast(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e,
                       const std::string& key, const ifcopenshell_value_t* value)
{
    auto* be = as_entity(e);
    if (!be) return;
    auto* decl = be->declaration().as_entity();
    if (!decl) return;
    int idx = (int)decl->attribute_index(key);
    if (idx < 0) return;

    const auto& attrs = decl->all_attributes();
    if ((size_t)idx >= attrs.size()) return;
    const IfcParse::attribute* attr = attrs[(size_t)idx];
    const char* dt = ifcopenshell_util_attribute_get_primitive_type(
        reinterpret_cast<const void*>(attr));
    std::string dts = dt ? dt : "";

    auto try_set_none = [&]() {
        try { e->unset_attribute_value((size_t)idx); } catch (...) {}
    };

    if (val_is_none(value)) {
        try_set_none();
        return;
    }

    try {
        if (dts == "string") {
            std::string s;
            switch (value->kind) {
                case IFCSEL_VALUE_STRING: s = value->s_val; break;
                case IFCSEL_VALUE_INT:    s = std::to_string(value->i_val); break;
                case IFCSEL_VALUE_DOUBLE: s = std::to_string(value->d_val); break;
                case IFCSEL_VALUE_BOOL:   s = value->b_val ? "True" : "False"; break;
                default: s = val_to_string(value); break;
            }
            e->set_attribute_value((size_t)idx, s);
        } else if (dts == "float") {
            double d = 0.0;
            if (value->kind == IFCSEL_VALUE_DOUBLE) d = value->d_val;
            else if (value->kind == IFCSEL_VALUE_INT) d = (double)value->i_val;
            else if (value->kind == IFCSEL_VALUE_BOOL) d = value->b_val ? 1.0 : 0.0;
            else if (value->kind == IFCSEL_VALUE_STRING) {
                try { d = std::stod(value->s_val); } catch (...) { d = 0.0; }
            }
            e->set_attribute_value((size_t)idx, d);
        } else if (dts == "integer") {
            int iv = 0;
            if (value->kind == IFCSEL_VALUE_INT) iv = (int)value->i_val;
            else if (value->kind == IFCSEL_VALUE_DOUBLE) iv = (int)value->d_val;
            else if (value->kind == IFCSEL_VALUE_BOOL) iv = value->b_val ? 1 : 0;
            else if (value->kind == IFCSEL_VALUE_STRING) {
                try { iv = std::stoi(value->s_val); } catch (...) { iv = 0; }
            }
            e->set_attribute_value((size_t)idx, iv);
        } else if (dts == "boolean") {
            bool b = false;
            if (value->kind == IFCSEL_VALUE_BOOL) b = value->b_val;
            else if (value->kind == IFCSEL_VALUE_INT) b = value->i_val != 0;
            else if (value->kind == IFCSEL_VALUE_DOUBLE) b = value->d_val != 0.0;
            else if (value->kind == IFCSEL_VALUE_STRING) {
                const std::string& s = value->s_val;
                if (s == "True" || s == "true" || s == "TRUE" || s == "Yes" || s == "1") b = true;
                else if (s == "False" || s == "false" || s == "FALSE" || s == "No" || s == "0") b = false;
                else b = !s.empty();
            }
            e->set_attribute_value((size_t)idx, b);
        } else if (dts == "entity") {
            IfcUtil::IfcBaseClass* ref = nullptr;
            if (value->kind == IFCSEL_VALUE_INSTANCE) ref = value->inst_val;
            else if (value->kind == IFCSEL_VALUE_STRING) {
                try { ref = file->instance_by_guid(value->s_val); }
                catch (...) { ref = nullptr; }
            }
            e->set_attribute_value((size_t)idx, ref);
        } else if (dts == "enum") {
            std::string s = (value->kind == IFCSEL_VALUE_STRING) ? value->s_val : val_to_string(value);
            e->set_attribute_value((size_t)idx, s);
        } else {
            /* Unknown primitive — fall back to raw value of matching kind. */
            switch (value->kind) {
                case IFCSEL_VALUE_STRING: e->set_attribute_value((size_t)idx, value->s_val); break;
                case IFCSEL_VALUE_INT:    e->set_attribute_value((size_t)idx, (int)value->i_val); break;
                case IFCSEL_VALUE_DOUBLE: e->set_attribute_value((size_t)idx, value->d_val); break;
                case IFCSEL_VALUE_BOOL:   e->set_attribute_value((size_t)idx, value->b_val); break;
                case IFCSEL_VALUE_INSTANCE: e->set_attribute_value((size_t)idx, value->inst_val); break;
                default: break;
            }
        }
    } catch (...) {
        /* Fallback: try the raw value type. */
        try {
            switch (value->kind) {
                case IFCSEL_VALUE_STRING: e->set_attribute_value((size_t)idx, value->s_val); break;
                case IFCSEL_VALUE_INT:    e->set_attribute_value((size_t)idx, (int)value->i_val); break;
                case IFCSEL_VALUE_DOUBLE: e->set_attribute_value((size_t)idx, value->d_val); break;
                case IFCSEL_VALUE_BOOL:   e->set_attribute_value((size_t)idx, value->b_val); break;
                case IFCSEL_VALUE_INSTANCE: e->set_attribute_value((size_t)idx, value->inst_val); break;
                default: break;
            }
        } catch (...) {}
    }
}

/* ====================================================================
 *  do_set — iterative key application                                    */

int do_set(IfcParse::IfcFile* file,
           ifcopenshell_ifc_file_t* file_h,
           Cursor cur,
           const std::vector<KeyEntry>& keys,
           size_t start_idx,
           const ifcopenshell_value_t* value,
           const std::string& concat)
{
    for (size_t i = start_idx; i < keys.size(); ++i) {
        const KeyEntry& key = keys[i];
        const std::string& k = key.text;

        if (cur.kind == Cursor::K_NONE) return 0;

        /* ----- INSTANCE branch ----- */
        if (cur.kind == Cursor::K_INSTANCE && cur.inst) {
            if (!key.is_regex) {
                /* Navigation keys */
                if (k == "type") { cur = Cursor::instance(call_get_type(cur.inst)); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "material" || k == "mat") { cur = Cursor::instance(call_get_material(cur.inst)); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "materials" || k == "mats") {
                    std::vector<Cursor> it;
                    for (auto* m : call_get_materials(cur.inst)) it.push_back(Cursor::instance(m));
                    cur = Cursor::listv(std::move(it));
                    continue;
                }
                if (k == "styles") {
                    std::vector<Cursor> it;
                    for (auto* s : call_get_styles(cur.inst)) it.push_back(Cursor::instance(s));
                    cur = Cursor::listv(std::move(it));
                    continue;
                }
                if (k == "item" || k == "i") {
                    const char* attr = nullptr;
                    if (entity_is_a(cur.inst, "IfcMaterialLayerSet")) attr = "MaterialLayers";
                    else if (entity_is_a(cur.inst, "IfcMaterialProfileSet")) attr = "MaterialProfiles";
                    else if (entity_is_a(cur.inst, "IfcMaterialConstituentSet")) attr = "MaterialConstituents";
                    if (!attr) { /* unchanged — Python would leave element as-is */ continue; }
                    std::vector<Cursor> it;
                    for (auto* x : get_entity_list(cur.inst, attr)) it.push_back(Cursor::instance(x));
                    cur = Cursor::listv(std::move(it));
                    continue;
                }
                if (k == "container") { cur = Cursor::instance(call_get_container(cur.inst, nullptr)); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "space")     { cur = Cursor::instance(call_get_container(cur.inst, "IfcSpace")); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "storey")    { cur = Cursor::instance(call_get_container(cur.inst, "IfcBuildingStorey")); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "building")  { cur = Cursor::instance(call_get_container(cur.inst, "IfcBuilding")); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "site")      { cur = Cursor::instance(call_get_container(cur.inst, "IfcSite")); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "parent")    { cur = Cursor::instance(call_get_parent(cur.inst)); if (!cur.inst) cur.kind = Cursor::K_NONE; continue; }
                if (k == "class") {
                    std::string cur_cls = cur.inst->declaration().name();
                    std::string val_str = val_to_string(value);
                    if (lower(cur_cls) == lower(val_str)) return 0;
                    ScopedHandle eh(cur.inst);
                    auto* fh_local = file_h;
                    auto* h = ifcopenshell_util_schema_reassign_class(fh_local, eh.get(), val_str.c_str());
                    if (h) ifcopenshell_ifc_instance_destroy(h);
                    return 0;
                }
                if (k == "id") return 0;
                if (k == "predefined_type") {
                    /* current_value compare for early return */
                    Val* current_val = resolve_predefined_type(cur.inst);
                    bool equal = vals_equal(current_val, value);
                    delete current_val;
                    if (equal) return 0;

                    auto* type_e = call_get_type(cur.inst);
                    if (type_e) apply_set_predefined_type(file, type_e, value, true);
                    else        apply_set_predefined_type(file, cur.inst, value, false);
                    return 0;
                }
                if (k == "classification") {
                    std::vector<Cursor> it;
                    for (auto* r : call_get_classification(cur.inst)) it.push_back(Cursor::instance(r));
                    cur = Cursor::listv(std::move(it));
                    continue;
                }
                if ((k == "x" || k == "y" || k == "z" ||
                     k == "easting" || k == "northing" || k == "elevation") &&
                    ifcapi::has_attr(cur.inst, "ObjectPlacement")) {
                    if (k == "easting" || k == "northing" || k == "elevation") return 0;

                    std::array<double, 16> matrix;
                    auto* placement_e = ifcapi::get_entity_ref(cur.inst, "ObjectPlacement");
                    if (!placement_e) {
                        ifcapi::identity4(matrix.data());
                    } else {
                        ScopedHandle ph(placement_e);
                        if (!ifcopenshell_placement_get_local_placement(ph.get(), matrix.data())) {
                            ifcapi::identity4(matrix.data());
                        }
                    }
                    int ci = (k == "x") ? 0 : (k == "y") ? 1 : 2;
                    double prev = matrix[(size_t)ci * 4 + 3];
                    double newv = 0.0;
                    if (val_truthy(value)) {
                        if (value->kind == IFCSEL_VALUE_DOUBLE) newv = value->d_val;
                        else if (value->kind == IFCSEL_VALUE_INT) newv = (double)value->i_val;
                        else if (value->kind == IFCSEL_VALUE_STRING) {
                            try { newv = std::stod(value->s_val); } catch (...) { newv = 0.0; }
                        } else if (value->kind == IFCSEL_VALUE_BOOL) newv = value->b_val ? 1.0 : 0.0;
                    }
                    if (ifcopenshell_util_shape_is_x(newv, prev, 0.0)) return 0;

                    matrix[(size_t)ci * 4 + 3] = newv;
                    ScopedHandle eh(cur.inst);
                    auto* h = ifcopenshell_api_geometry_edit_object_placement(
                        file_h, eh.get(), matrix.data(), false, true);
                    if (h) ifcopenshell_ifc_instance_destroy(h);
                    return 0;
                }
            }

            /* General attribute branch (entity_instance) */
            std::string attr_key = k;
            if (!key.is_regex && k == "Name" && entity_is_a(cur.inst, "IfcMaterialLayerSet"))
                attr_key = "LayerSetName";

            int aidx = key.is_regex ? -1 : ifcapi::find_attr_idx(cur.inst, attr_key.c_str());
            if (aidx >= 0) {
                /* Read current value via attr_to_val. */
                Val* current_val = nullptr;
                try {
                    auto av = cur.inst->get_attribute_value((size_t)aidx);
                    current_val = attr_to_val(av);
                } catch (...) { current_val = make_none(); }

                bool is_last = (i + 1 == keys.size());
                if (!is_last) {
                    /* Descend into current_val. Convert Val* → Cursor. */
                    if (current_val->kind == IFCSEL_VALUE_INSTANCE && current_val->inst_val) {
                        cur = Cursor::instance(current_val->inst_val);
                    } else if (current_val->kind == IFCSEL_VALUE_LIST) {
                        std::vector<Cursor> items;
                        for (auto* it : current_val->list_val) {
                            if (it && it->kind == IFCSEL_VALUE_INSTANCE && it->inst_val)
                                items.push_back(Cursor::instance(it->inst_val));
                            else
                                items.push_back(Cursor::none());
                        }
                        cur = Cursor::listv(std::move(items));
                    } else if (current_val->kind == IFCSEL_VALUE_NONE) {
                        cur = Cursor::none();
                    } else {
                        /* primitive — next iteration falls through and raises */
                        cur = Cursor::none();
                    }
                    delete current_val;
                    continue;
                }

                bool equal = vals_equal(current_val, value);
                delete current_val;
                if (equal) return 0;

                setattr_with_cast(file, cur.inst, attr_key, value);
                return 0;
            }

            /* Pset extraction branch */
            auto psets = all_psets(cur.inst);
            if (key.is_regex) {
                std::vector<Cursor> matching;
                for (auto& kv : psets) {
                    if (re_match(key.pattern, kv.first)) {
                        IfcUtil::IfcBaseClass* p = find_pset_instance(cur.inst, kv.first);
                        matching.push_back(Cursor::dictv(p, kv.second));
                    } else {
                        delete kv.second;
                    }
                }
                if (matching.empty()) {
                    cur = Cursor::none();
                } else if (matching.size() == 1) {
                    cur = std::move(matching[0]);
                } else {
                    cur = Cursor::listv(std::move(matching));
                }
                continue;
            }

            /* Scalar pset key: get_pset, possibly auto-create */
            Val* pset_dict = nullptr;
            for (auto& kv : psets) {
                if (kv.first == k && !pset_dict) pset_dict = kv.second;
                else delete kv.second;
            }

            IfcUtil::IfcBaseClass* pset_inst = nullptr;
            if (pset_dict) {
                pset_inst = find_pset_instance(cur.inst, k);
            } else {
                /* Auto-create when value is truthy and next key is the prop name. */
                if (val_truthy(value) && (i + 2 == keys.size())) {
                    bool is_qto = icontains(k, "qto") || icontains(k, "quantity") || icontains(k, "quantities");
                    ScopedHandle eh(cur.inst);
                    ifcopenshell_ifc_instance_t* h = nullptr;
                    if (is_qto) {
                        h = ifcopenshell_api_pset_add_qto(file_h, eh.get(), k.c_str(), nullptr);
                    } else {
                        h = ifcopenshell_api_pset_add_pset(file_h, eh.get(), k.c_str(), nullptr, nullptr);
                    }
                    if (h) {
                        pset_inst = h->ptr;
                        ifcopenshell_ifc_instance_destroy(h);
                        pset_dict = make_dict();  /* empty */
                    }
                }
            }

            if (!pset_dict) { cur = Cursor::none(); continue; }
            cur = Cursor::dictv(pset_inst, pset_dict);
            continue;
        }

        /* ----- DICT branch (a pset-property dict) ----- */
        if (cur.kind == Cursor::K_DICT) {
            IfcUtil::IfcBaseClass* pset = cur.dict.pset;
            if (!pset) return 0;

            if (key.is_regex) {
                if (!cur.dict.dict) return 0;
                for (auto& kv : cur.dict.dict->dict_val) {
                    if (!re_match(key.pattern, kv.first)) continue;
                    if (entity_is_a(pset, "IfcPropertySet") && !vals_equal(kv.second, value)) {
                        edit_pset_one(file_h, pset, kv.first.c_str(), value);
                    } else if (entity_is_a(pset, "IfcElementQuantity")) {
                        /* Convert value to double for Qto comparison */
                        Val tmp;
                        tmp.kind = IFCSEL_VALUE_DOUBLE;
                        if (val_is_none(value)) tmp.d_val = 0.0;
                        else if (value->kind == IFCSEL_VALUE_DOUBLE) tmp.d_val = value->d_val;
                        else if (value->kind == IFCSEL_VALUE_INT) tmp.d_val = (double)value->i_val;
                        else if (value->kind == IFCSEL_VALUE_STRING) {
                            try { tmp.d_val = std::stod(value->s_val); } catch (...) { tmp.d_val = 0.0; }
                        } else if (value->kind == IFCSEL_VALUE_BOOL) tmp.d_val = value->b_val ? 1.0 : 0.0;
                        if (!vals_equal(kv.second, &tmp)) {
                            edit_qto_one(file_h, pset, kv.first.c_str(), value, true);
                        }
                    }
                }
                return 0;
            }

            /* Scalar key on dict */
            Val* current = dict_get(cur.dict, k);
            if (entity_is_a(pset, "IfcPropertySet")) {
                if (current && vals_equal(current, value)) { delete current; return 0; }
                std::vector<std::string> out_list;
                PsetPVResult pr = process_pset_prop_value(file, pset, k, value, current, concat, out_list);
                delete current;
                if (pr == PV_ERROR) return -1;
                if (pr == PV_SKIP) return 0;
                if (pr == PV_USE_LIST) {
                    auto* props = ifcopenshell_pset_props_new();
                    std::vector<const char*> ptrs;
                    for (auto& s : out_list) ptrs.push_back(s.c_str());
                    ifcopenshell_pset_props_set_string_list(props, k.c_str(), ptrs.data(), (uint32_t)ptrs.size());
                    ScopedHandle ph(pset);
                    ifcopenshell_api_pset_edit_pset(file_h, ph.get(), nullptr, props, nullptr, false);
                    ifcopenshell_pset_props_free(props);
                    return 0;
                }
                edit_pset_one(file_h, pset, k.c_str(), value);
                return 0;
            }
            if (entity_is_a(pset, "IfcElementQuantity")) {
                /* try float(value) … */
                bool castable = false;
                double dv = 0.0;
                if (val_is_none(value)) { delete current; return 0; }
                switch (value->kind) {
                    case IFCSEL_VALUE_DOUBLE: dv = value->d_val; castable = true; break;
                    case IFCSEL_VALUE_INT:    dv = (double)value->i_val; castable = true; break;
                    case IFCSEL_VALUE_BOOL:   dv = value->b_val ? 1.0 : 0.0; castable = true; break;
                    case IFCSEL_VALUE_STRING:
                        try { dv = std::stod(value->s_val); castable = true; }
                        catch (...) { castable = false; }
                        break;
                    default: castable = false; break;
                }
                if (!castable) { delete current; return 0; }
                Val tmp; tmp.kind = IFCSEL_VALUE_DOUBLE; tmp.d_val = dv;
                if (current && vals_equal(current, &tmp)) { delete current; return 0; }
                delete current;
                Val vd; vd.kind = IFCSEL_VALUE_DOUBLE; vd.d_val = dv;
                edit_qto_one(file_h, pset, k.c_str(), &vd, true);
                return 0;
            }
            delete current;
            return 0;
        }

        /* ----- LIST branch ----- */
        if (cur.kind == Cursor::K_LIST) {
            if (!key.is_regex) {
                bool all_digits = !k.empty();
                for (char c : k) if (!std::isdigit((unsigned char)c)) { all_digits = false; break; }
                if (all_digits) {
                    size_t idx = 0;
                    try { idx = (size_t)std::stoul(k); } catch (...) { return 0; }
                    if (idx >= cur.items.size()) return 0;
                    cur = std::move(cur.items[idx]);
                    continue;
                }
            }
            for (auto& item : cur.items) {
                int rc = do_set(file, file_h, std::move(item), keys, i, value, concat);
                if (rc != 0) return rc;
            }
            return 0;
        }

        break;
    }

    set_error("Failed to set value (invalid or unsupported query).");
    return -1;
}

}  // namespace

/* ====================================================================
 *  Public C entry points                                                 */

extern "C" {

IFCAPI_EXPORT ifcopenshell_selector_keylist_t* ifcopenshell_selector_keylist_create(void) {
    return new ifcopenshell_selector_keylist_t();
}

IFCAPI_EXPORT void ifcopenshell_selector_keylist_destroy(ifcopenshell_selector_keylist_t* h) {
    delete h;
}

IFCAPI_EXPORT void ifcopenshell_selector_keylist_append_string(
    ifcopenshell_selector_keylist_t* h, const char* str)
{
    if (!h) return;
    KeyEntry ke;
    ke.is_regex = false;
    ke.text = str ? str : "";
    h->keys.push_back(std::move(ke));
}

IFCAPI_EXPORT void ifcopenshell_selector_keylist_append_regex(
    ifcopenshell_selector_keylist_t* h, const char* pattern)
{
    if (!h) return;
    KeyEntry ke;
    ke.is_regex = true;
    ke.text = pattern ? pattern : "";
    try { ke.pattern = std::regex(ke.text); }
    catch (...) { ke.pattern = std::regex(""); }
    h->keys.push_back(std::move(ke));
}

IFCAPI_EXPORT int ifcopenshell_util_selector_set_element_value(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* element_h,
    const ifcopenshell_selector_keylist_t* keys_h,
    const ifcopenshell_value_t* value,
    const char* concat)
{
    if (!file_h || !file_h->ptr) {
        set_error("ifcopenshell_util_selector_set_element_value: NULL file");
        return -1;
    }
    if (!keys_h) {
        set_error("ifcopenshell_util_selector_set_element_value: NULL keys");
        return -1;
    }
    IfcParse::IfcFile* file = file_h->ptr;
    IfcUtil::IfcBaseClass* element = element_h ? element_h->ptr : nullptr;
    Cursor c = element ? Cursor::instance(element) : Cursor::none();
    std::string concat_s = concat ? concat : ", ";
    try {
        return do_set(file, file_h, std::move(c), keys_h->keys, 0, value, concat_s);
    } catch (const std::exception& ex) {
        set_error(std::string("set_element_value: ") + ex.what());
        return -1;
    } catch (...) {
        set_error("set_element_value: unknown error");
        return -1;
    }
}

IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_none(void)         { return make_none(); }
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_bool(bool b)       { return make_bool(b); }
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_int(int64_t i)     { return make_int(i); }
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_double(double d)   { return make_double(d); }
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_string(const char* s) {
    return make_string(s ? std::string(s) : std::string());
}
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_instance(ifcopenshell_ifc_instance_t* h) {
    return make_instance(h ? h->ptr : nullptr);
}
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_list(void) { return make_list(); }
IFCAPI_EXPORT void ifcopenshell_value_list_append(ifcopenshell_value_t* list, ifcopenshell_value_t* item) {
    if (!list) { delete item; return; }
    if (list->kind != IFCSEL_VALUE_LIST) { delete item; return; }
    list->list_val.push_back(item ? item : make_none());
}

}  // extern "C"
