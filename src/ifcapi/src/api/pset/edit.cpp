// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcopenshell_api_internal.hpp"
#include "api/pset/props.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

using ifcapi_pset::Entry;
using ifcapi_pset::Kind;

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

int find_attr_index(const IfcParse::entity* decl, const char* name) {
    if (!decl) return -1;
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) return static_cast<int>(i);
    }
    return -1;
}

bool entity_is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    if (!e) return false;
    return e->declaration().is(name);
}

bool entity_has_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    if (!e) return false;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return false;
    return find_attr_index(be->declaration().as_entity(), attr) >= 0;
}

int attr_index_of(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return -1;
    return find_attr_index(be->declaration().as_entity(), attr);
}

std::string read_string_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return std::string();
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return std::string();
        return (std::string)val;
    } catch (...) {
        return std::string();
    }
}

IfcUtil::IfcBaseClass* read_ref_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return nullptr;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)val;
    } catch (...) {
        return nullptr;
    }
}

std::vector<IfcUtil::IfcBaseClass*> read_ref_aggregate(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    int idx = attr_index_of(e, attr);
    if (idx < 0) return result;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return result;
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& it : *agg) result.push_back(it);
        }
    } catch (...) {}
    return result;
}

bool write_ref_aggregate(IfcUtil::IfcBaseClass* e, const char* attr,
                         const std::vector<IfcUtil::IfcBaseClass*>& items) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return false;
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* p : items) agg->push(p);
    e->set_attribute_value(static_cast<size_t>(idx), agg);
    return true;
}

void write_string_attr(IfcUtil::IfcBaseClass* e, const char* attr, const std::string& v) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    e->set_attribute_value(static_cast<size_t>(idx), v);
}

// Walk a parameter_type chain to its underlying simple_type.
const IfcParse::simple_type* underlying_simple_type(const IfcParse::type_declaration* td) {
    auto* pt = td ? td->declared_type() : nullptr;
    while (pt) {
        if (auto* s = pt->as_simple_type()) return s;
        if (auto* n = pt->as_named_type()) {
            auto* d = n->declared_type();
            if (auto* td2 = d ? d->as_type_declaration() : nullptr) {
                pt = td2->declared_type();
                continue;
            }
        }
        return nullptr;
    }
    return nullptr;
}

std::string lowercase(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// Create an inline IFC simple-type wrapper instance (e.g. IfcLabel("hello")) from a primitive.
IfcUtil::IfcBaseClass* create_typed_value(IfcParse::IfcFile* file, const std::string& type_name,
                                          const Entry& src, bool from_list = false,
                                          size_t list_idx = 0) {
    const IfcParse::declaration* decl = nullptr;
    try {
        decl = file->schema()->declaration_by_name(type_name);
    } catch (...) {
        return nullptr;
    }
    auto* type_decl = decl ? decl->as_type_declaration() : nullptr;
    if (!type_decl) return nullptr;

    auto* inst = file->create(type_decl);
    if (!inst) return nullptr;

    auto* simple = underlying_simple_type(type_decl);
    auto kind = simple ? simple->declared_type() : IfcParse::simple_type::string_type;

    auto write_string = [&](const std::string& s) {
        inst->set_attribute_value(0, s);
    };
    auto write_double = [&](double d) {
        inst->set_attribute_value(0, d);
    };
    auto write_int = [&](int i) {
        inst->set_attribute_value(0, i);
    };
    auto write_bool = [&](bool b) {
        inst->set_attribute_value(0, b);
    };

    if (from_list) {
        switch (src.kind) {
            case Kind::STRING_LIST:
                if (list_idx < src.str_list.size()) write_string(src.str_list[list_idx]);
                break;
            case Kind::DOUBLE_LIST:
                if (list_idx < src.dbl_list.size()) {
                    if (kind == IfcParse::simple_type::integer_type) {
                        write_int(static_cast<int>(src.dbl_list[list_idx]));
                    } else if (kind == IfcParse::simple_type::string_type) {
                        write_string(std::to_string(src.dbl_list[list_idx]));
                    } else {
                        write_double(src.dbl_list[list_idx]);
                    }
                }
                break;
            case Kind::INT_LIST:
                if (list_idx < src.int_list.size()) {
                    if (kind == IfcParse::simple_type::real_type ||
                        kind == IfcParse::simple_type::number_type) {
                        write_double(static_cast<double>(src.int_list[list_idx]));
                    } else if (kind == IfcParse::simple_type::string_type) {
                        write_string(std::to_string(src.int_list[list_idx]));
                    } else {
                        write_int(static_cast<int>(src.int_list[list_idx]));
                    }
                }
                break;
            default:
                break;
        }
        return inst;
    }

    switch (kind) {
        case IfcParse::simple_type::string_type:
            switch (src.kind) {
                case Kind::STRING:        write_string(src.s_val); break;
                case Kind::TYPED_STRING:  write_string(src.s_val); break;
                case Kind::INT:           write_string(std::to_string(src.i_val)); break;
                case Kind::TYPED_INT:     write_string(std::to_string(src.i_val)); break;
                case Kind::DOUBLE:        write_string(std::to_string(src.d_val)); break;
                case Kind::TYPED_DOUBLE:  write_string(std::to_string(src.d_val)); break;
                case Kind::BOOL:          write_string(src.b_val ? "true" : "false"); break;
                case Kind::TYPED_BOOL:    write_string(src.b_val ? "true" : "false"); break;
                default: write_string(src.s_val); break;
            }
            break;
        case IfcParse::simple_type::real_type:
        case IfcParse::simple_type::number_type:
            switch (src.kind) {
                case Kind::DOUBLE:        write_double(src.d_val); break;
                case Kind::TYPED_DOUBLE:  write_double(src.d_val); break;
                case Kind::INT:           write_double(static_cast<double>(src.i_val)); break;
                case Kind::TYPED_INT:     write_double(static_cast<double>(src.i_val)); break;
                case Kind::STRING:
                case Kind::TYPED_STRING:
                    try { write_double(std::stod(src.s_val)); }
                    catch (...) { write_double(0.0); }
                    break;
                case Kind::BOOL:          write_double(src.b_val ? 1.0 : 0.0); break;
                default: write_double(0.0); break;
            }
            break;
        case IfcParse::simple_type::integer_type:
            switch (src.kind) {
                case Kind::INT:           write_int(static_cast<int>(src.i_val)); break;
                case Kind::TYPED_INT:     write_int(static_cast<int>(src.i_val)); break;
                case Kind::DOUBLE:        write_int(static_cast<int>(src.d_val)); break;
                case Kind::TYPED_DOUBLE:  write_int(static_cast<int>(src.d_val)); break;
                case Kind::STRING:
                case Kind::TYPED_STRING:
                    try { write_int(std::stoi(src.s_val)); } catch (...) { write_int(0); }
                    break;
                case Kind::BOOL:          write_int(src.b_val ? 1 : 0); break;
                default: write_int(0); break;
            }
            break;
        case IfcParse::simple_type::boolean_type:
        case IfcParse::simple_type::logical_type:
            switch (src.kind) {
                case Kind::BOOL:          write_bool(src.b_val); break;
                case Kind::TYPED_BOOL:    write_bool(src.b_val); break;
                case Kind::INT:           write_bool(src.i_val != 0); break;
                case Kind::DOUBLE:        write_bool(src.d_val != 0.0); break;
                case Kind::STRING:
                case Kind::TYPED_STRING: {
                    std::string s = src.s_val;
                    write_bool(s == "true" || s == "True" || s == "TRUE" || s == "1" || s == ".T.");
                    break;
                }
                default: write_bool(false); break;
            }
            break;
        default:
            // Fallback as string.
            write_string(src.s_val);
            break;
    }

    return inst;
}

// Determine the IFC primary measure type for a single-value property.
std::string primary_measure_type(IfcUtil::IfcBaseClass* pset_template, const std::string& prop_name,
                                 IfcUtil::IfcBaseClass* old_value, const Entry& new_value) {
    // An explicit type from the new value (TYPED_* kinds or an INSTANCE
    // wrapping a typed value) overrides any old/template-derived type;
    // mirrors upstream where ``prop.NominalValue = file.createIfcBoolean(...)``
    // swaps the underlying value type.
    if (new_value.kind == Kind::TYPED_STRING || new_value.kind == Kind::TYPED_DOUBLE
        || new_value.kind == Kind::TYPED_INT || new_value.kind == Kind::TYPED_BOOL) {
        return new_value.ifc_type;
    }
    if (new_value.kind == Kind::INSTANCE && new_value.inst) {
        return new_value.inst->declaration().name();
    }
    if (old_value) {
        return old_value->declaration().name();
    }
    if (pset_template) {
        for (auto* pt : read_ref_aggregate(pset_template, "HasPropertyTemplates")) {
            if (read_string_attr(pt, "Name") == prop_name) {
                std::string pmt = read_string_attr(pt, "PrimaryMeasureType");
                return pmt.empty() ? std::string("IfcLabel") : pmt;
            }
        }
    }
    switch (new_value.kind) {
        case Kind::STRING: return "IfcLabel";
        case Kind::BOOL:   return "IfcBoolean";
        case Kind::INT:    return "IfcInteger";
        case Kind::DOUBLE: return "IfcReal";
        default: return std::string();
    }
}

// Get aggregate of properties on a pset, handling HasProperties/Properties/ExtendedProperties.
const char* properties_attr_name(IfcUtil::IfcBaseClass* pset) {
    if (entity_has_attr(pset, "HasProperties")) return "HasProperties";
    if (entity_has_attr(pset, "Properties"))    return "Properties";
    if (entity_is_a(pset, "IfcMaterialProperties") && entity_has_attr(pset, "ExtendedProperties"))
        return "ExtendedProperties";
    return nullptr;
}

uint32_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e || !file) return 0;
    try {
        auto inv = file->getInverse(e->id(), nullptr, -1);
        return inv ? static_cast<uint32_t>(inv->size()) : 0u;
    } catch (...) {
        return 0;
    }
}

bool process_existing_single_value(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* prop,
                                   IfcUtil::IfcBaseClass* pset_template,
                                   const Entry& entry, bool should_purge,
                                   bool& removed) {
    removed = false;
    auto kind = entry.kind;
    if (kind == Kind::NONE) {
        if (should_purge) {
            file->removeEntity(prop);
            removed = true;
            return true;
        }
        // Set NominalValue = NULL.
        int idx = attr_index_of(prop, "NominalValue");
        if (idx >= 0) prop->unset_attribute_value(static_cast<size_t>(idx));
        return true;
    }

    if (kind == Kind::INSTANCE) {
        // Caller passed an entity_instance already wrapping a typed value (or a raw IfcValue/IfcSimpleProperty).
        if (!entry.inst) return false;
        if (entity_is_a(entry.inst, "IfcValue") || entry.inst->declaration().as_type_declaration()) {
            int idx = attr_index_of(prop, "NominalValue");
            if (idx >= 0) prop->set_attribute_value(static_cast<size_t>(idx), entry.inst);
        }
        if (entry.unit) {
            int u_idx = attr_index_of(prop, "Unit");
            if (u_idx >= 0) prop->set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
        }
        return true;
    }

    auto* old_value = read_ref_attr(prop, "NominalValue");
    std::string pmt = primary_measure_type(pset_template, read_string_attr(prop, "Name"), old_value, entry);
    if (pmt.empty()) pmt = "IfcLabel";
    auto* typed = create_typed_value(file, pmt, entry);
    if (!typed) return false;
    int idx = attr_index_of(prop, "NominalValue");
    if (idx >= 0) prop->set_attribute_value(static_cast<size_t>(idx), typed);
    if (entry.unit) {
        int u_idx = attr_index_of(prop, "Unit");
        if (u_idx >= 0) prop->set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
    }
    return true;
}

bool process_existing_enumerated(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* prop,
                                 const Entry& entry, bool should_purge, bool& removed) {
    removed = false;
    if (entry.kind != Kind::STRING_LIST && entry.kind != Kind::DOUBLE_LIST && entry.kind != Kind::INT_LIST) {
        return true;
    }
    bool empty_list = (entry.kind == Kind::STRING_LIST && entry.str_list.empty())
        || (entry.kind == Kind::DOUBLE_LIST && entry.dbl_list.empty())
        || (entry.kind == Kind::INT_LIST && entry.int_list.empty());
    if (empty_list && should_purge) {
        file->removeEntity(prop);
        removed = true;
        return true;
    }

    std::string pmt;
    auto* ref = read_ref_attr(prop, "EnumerationReference");
    if (ref) {
        auto vals = read_ref_aggregate(ref, "EnumerationValues");
        if (!vals.empty()) pmt = vals.front()->declaration().name();
    }
    if (pmt.empty()) {
        auto vals = read_ref_aggregate(prop, "EnumerationValues");
        if (!vals.empty()) pmt = vals.front()->declaration().name();
    }
    if (pmt.empty()) pmt = "IfcLabel";

    std::vector<IfcUtil::IfcBaseClass*> sel_vals;
    size_t n = (entry.kind == Kind::STRING_LIST) ? entry.str_list.size()
        : (entry.kind == Kind::DOUBLE_LIST) ? entry.dbl_list.size() : entry.int_list.size();
    for (size_t i = 0; i < n; ++i) {
        auto* tv = create_typed_value(file, pmt, entry, true, i);
        if (tv) sel_vals.push_back(tv);
    }
    if (sel_vals.empty()) {
        int idx = attr_index_of(prop, "EnumerationValues");
        if (idx >= 0) prop->unset_attribute_value(static_cast<size_t>(idx));
    } else {
        write_ref_aggregate(prop, "EnumerationValues", sel_vals);
    }
    return true;
}

IfcUtil::IfcBaseClass* build_new_property(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset_template,
                                          const std::string& name, const Entry& entry) {
    if (entry.kind == Kind::INSTANCE) {
        if (!entry.inst) return nullptr;
        if (entity_is_a(entry.inst, "IfcProperty")) {
            return entry.inst;
        }
        // Treat as an inline typed value to wrap in IfcPropertySingleValue.
        const auto* sv_decl = file->schema()->declaration_by_name("IfcPropertySingleValue");
        auto* sv = file->create(sv_decl);
        write_string_attr(sv, "Name", name);
        int nv_idx = attr_index_of(sv, "NominalValue");
        if (nv_idx >= 0) sv->set_attribute_value(static_cast<size_t>(nv_idx), entry.inst);
        if (entry.unit) {
            int u_idx = attr_index_of(sv, "Unit");
            if (u_idx >= 0) sv->set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
        }
        return sv;
    }

    if (entry.kind == Kind::STRING_LIST || entry.kind == Kind::DOUBLE_LIST || entry.kind == Kind::INT_LIST) {
        size_t n = (entry.kind == Kind::STRING_LIST) ? entry.str_list.size()
            : (entry.kind == Kind::DOUBLE_LIST) ? entry.dbl_list.size() : entry.int_list.size();
        if (n == 0) return nullptr;
        if (!pset_template) return nullptr;
        for (auto* pt : read_ref_aggregate(pset_template, "HasPropertyTemplates")) {
            if (read_string_attr(pt, "Name") != name) continue;
            std::string tt = read_string_attr(pt, "TemplateType");
            std::string pmt = read_string_attr(pt, "PrimaryMeasureType");
            if (tt == "P_LISTVALUE") {
                if (pmt.empty()) return nullptr;
                std::vector<IfcUtil::IfcBaseClass*> items;
                for (size_t i = 0; i < n; ++i) {
                    if (auto* tv = create_typed_value(file, pmt, entry, true, i)) items.push_back(tv);
                }
                const auto* lv_decl = file->schema()->declaration_by_name("IfcPropertyListValue");
                auto* lv = file->create(lv_decl);
                write_string_attr(lv, "Name", name);
                write_ref_aggregate(lv, "ListValues", items);
                return lv;
            }
            if (tt == "P_ENUMERATEDVALUE") {
                if (pmt.empty()) pmt = "IfcLabel";
                auto* enumerators = read_ref_attr(pt, "Enumerators");
                std::vector<IfcUtil::IfcBaseClass*> enum_value_items;
                if (enumerators) enum_value_items = read_ref_aggregate(enumerators, "EnumerationValues");

                const auto* pe_decl = file->schema()->declaration_by_name("IfcPropertyEnumeration");
                auto* pe = file->create(pe_decl);
                write_string_attr(pe, "Name", name);
                write_ref_aggregate(pe, "EnumerationValues", enum_value_items);

                std::vector<IfcUtil::IfcBaseClass*> sel;
                for (size_t i = 0; i < n; ++i) {
                    if (auto* tv = create_typed_value(file, pmt, entry, true, i)) sel.push_back(tv);
                }
                const auto* pev_decl = file->schema()->declaration_by_name("IfcPropertyEnumeratedValue");
                auto* pev = file->create(pev_decl);
                write_string_attr(pev, "Name", name);
                write_ref_aggregate(pev, "EnumerationValues", sel);
                int er_idx = attr_index_of(pev, "EnumerationReference");
                if (er_idx >= 0) pev->set_attribute_value(static_cast<size_t>(er_idx), pe);
                return pev;
            }
            break;
        }
        return nullptr;
    }

    // Scalar single value path.
    Entry tmp = entry;
    std::string pmt = primary_measure_type(pset_template, name, nullptr, tmp);
    if (pmt.empty()) pmt = "IfcLabel";
    auto* tv = create_typed_value(file, pmt, entry);
    if (!tv) return nullptr;
    const auto* sv_decl = file->schema()->declaration_by_name("IfcPropertySingleValue");
    auto* sv = file->create(sv_decl);
    write_string_attr(sv, "Name", name);
    int nv_idx = attr_index_of(sv, "NominalValue");
    if (nv_idx >= 0) sv->set_attribute_value(static_cast<size_t>(nv_idx), tv);
    if (entry.unit) {
        int u_idx = attr_index_of(sv, "Unit");
        if (u_idx >= 0) sv->set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
    }
    return sv;
}

IfcUtil::IfcBaseClass* fetch_pset_template(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset,
                                           IfcUtil::IfcBaseClass* explicit_template) {
    if (explicit_template) return explicit_template;
    std::string name = read_string_attr(pset, "Name");
    if (name.empty()) return nullptr;
    auto* cache = ifcopenshell_util_pset_get_template(file->schema()->name().c_str());
    if (!cache) return nullptr;
    auto* h = ifcopenshell_util_pset_template_get_by_name(cache, name.c_str());
    if (!h) return nullptr;
    auto* base = h->ptr;
    delete h;
    return base;
}

}  // namespace

extern "C" {

/* ---- Properties builder ---- */

IFCAPI_EXPORT ifcopenshell_pset_props_t* ifcopenshell_pset_props_new(void) {
    return new ifcopenshell_pset_props_t();
}

IFCAPI_EXPORT void ifcopenshell_pset_props_free(ifcopenshell_pset_props_t* p) {
    delete p;
}

static Entry& append_entry(ifcopenshell_pset_props_t* p, const char* key) {
    p->entries.emplace_back();
    p->entries.back().key = key ? key : "";
    return p->entries.back();
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_null(ifcopenshell_pset_props_t* p, const char* key) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::NONE;
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_bool(ifcopenshell_pset_props_t* p, const char* key, bool v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::BOOL;
    e.b_val = v;
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_int(ifcopenshell_pset_props_t* p, const char* key, int64_t v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INT;
    e.i_val = v;
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_double(ifcopenshell_pset_props_t* p, const char* key, double v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DOUBLE;
    e.d_val = v;
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_string(ifcopenshell_pset_props_t* p, const char* key, const char* v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::STRING;
    e.s_val = v ? v : "";
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_instance(
    ifcopenshell_pset_props_t* p, const char* key, ifcopenshell_ifc_instance_t* v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INSTANCE;
    e.inst = v ? v->ptr : nullptr;
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_string(
    ifcopenshell_pset_props_t* p, const char* key, const char* v, const char* ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_STRING;
    e.s_val = v ? v : "";
    e.ifc_type = ifc_type ? ifc_type : "";
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_double(
    ifcopenshell_pset_props_t* p, const char* key, double v, const char* ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_DOUBLE;
    e.d_val = v;
    e.ifc_type = ifc_type ? ifc_type : "";
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_int(
    ifcopenshell_pset_props_t* p, const char* key, int64_t v, const char* ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_INT;
    e.i_val = v;
    e.ifc_type = ifc_type ? ifc_type : "";
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_bool(
    ifcopenshell_pset_props_t* p, const char* key, bool v, const char* ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_BOOL;
    e.b_val = v;
    e.ifc_type = ifc_type ? ifc_type : "";
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_string_list(
    ifcopenshell_pset_props_t* p, const char* key, const char** vals, uint32_t count) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::STRING_LIST;
    e.str_list.reserve(count);
    for (uint32_t i = 0; i < count; ++i) e.str_list.emplace_back(vals && vals[i] ? vals[i] : "");
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_double_list(
    ifcopenshell_pset_props_t* p, const char* key, const double* vals, uint32_t count) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DOUBLE_LIST;
    e.dbl_list.assign(vals, vals + count);
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_int_list(
    ifcopenshell_pset_props_t* p, const char* key, const int64_t* vals, uint32_t count) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INT_LIST;
    e.int_list.assign(vals, vals + count);
}

IFCAPI_EXPORT void ifcopenshell_pset_props_set_dict(
    ifcopenshell_pset_props_t* outer, const char* key, ifcopenshell_pset_props_t* inner) {
    if (!outer) {
        delete inner;
        return;
    }
    auto& e = append_entry(outer, key);
    e.kind = Kind::DICT;
    e.nested.reset(inner);
}

// Attach an IfcUnit to the most recently added entry (mirrors upstream's
// ``unpack_unit_value`` shape ``{NominalValue, Unit}``). The unit becomes the
// ``Unit`` attribute on the resulting IfcPropertySingleValue.
IFCAPI_EXPORT void ifcopenshell_pset_props_set_unit_for_last(
    ifcopenshell_pset_props_t* p, ifcopenshell_ifc_instance_t* unit) {
    if (!p || p->entries.empty()) return;
    p->entries.back().unit = unit ? unit->ptr : nullptr;
}

/* ---- edit_pset ---- */

IFCAPI_EXPORT bool ifcopenshell_api_pset_edit_pset(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* pset_h,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    ifcopenshell_ifc_instance_t* pset_template_h,
    bool should_purge)
{
    if (!file_h || !pset_h) {
        set_error("ifcopenshell_api_pset_edit_pset: missing required argument");
        return false;
    }
    auto* file = file_h->ptr;
    auto* pset = pset_h->ptr;
    if (!file || !pset) {
        set_error("ifcopenshell_api_pset_edit_pset: NULL underlying pointer");
        return false;
    }
    try {
        if (name) write_string_attr(pset, "Name", std::string(name));

        // Map of remaining property entries (key → entry pointer for direct lookup
        // / removal). We track removal from the dict similarly to the Python loop.
        std::unordered_map<std::string, const Entry*> remaining;
        std::vector<std::string> ordered_keys;
        if (properties) {
            for (const auto& e : properties->entries) {
                if (remaining.find(e.key) == remaining.end()) ordered_keys.push_back(e.key);
                remaining[e.key] = &e;
            }
        }

        IfcUtil::IfcBaseClass* pset_template = pset_template_h ? pset_template_h->ptr : nullptr;
        pset_template = fetch_pset_template(file, pset, pset_template);

        const char* attr_name = properties_attr_name(pset);
        if (!attr_name) {
            set_error("Pset has no HasProperties/Properties/ExtendedProperties attribute");
            return false;
        }

        std::vector<IfcUtil::IfcBaseClass*> existing = read_ref_aggregate(pset, attr_name);
        std::vector<IfcUtil::IfcBaseClass*> kept;

        for (auto* prop : existing) {
            std::string pname = read_string_attr(prop, "Name");
            auto it = remaining.find(pname);
            if (it == remaining.end()) {
                kept.push_back(prop);
                continue;
            }
            // If shared with another pset, treat as new (don't mutate in place).
            if (total_inverses(file, prop) > 1) continue;

            const Entry* e = it->second;
            bool removed = false;
            if (entity_is_a(prop, "IfcPropertySingleValue")) {
                if (!process_existing_single_value(file, prop, pset_template, *e, should_purge, removed)) {
                    kept.push_back(prop);
                    continue;
                }
            } else if (entity_is_a(prop, "IfcPropertyEnumeratedValue")) {
                if (!process_existing_enumerated(file, prop, *e, should_purge, removed)) {
                    kept.push_back(prop);
                    continue;
                }
            } else {
                kept.push_back(prop);
                remaining.erase(it);
                continue;
            }
            if (!removed) kept.push_back(prop);
            remaining.erase(it);
        }

        // Add new properties for the keys still in `remaining`, preserving original order.
        for (const auto& key : ordered_keys) {
            auto it = remaining.find(key);
            if (it == remaining.end()) continue;
            const Entry* e = it->second;
            if (e->kind == Kind::NONE && should_purge) continue;
            auto* np = build_new_property(file, pset_template, key, *e);
            if (np) kept.push_back(np);
        }

        write_ref_aggregate(pset, attr_name, kept);
        return true;
    } catch (const std::exception& ex) {
        set_error(std::string("ifcopenshell_api_pset_edit_pset: ") + ex.what());
        return false;
    }
}

/* ---- edit_qto ---- */

namespace {

const char* qto_value_attr_for_class(const std::string& cls) {
    if (cls == "IfcQuantityLength") return "LengthValue";
    if (cls == "IfcQuantityArea")   return "AreaValue";
    if (cls == "IfcQuantityVolume") return "VolumeValue";
    if (cls == "IfcQuantityCount")  return "CountValue";
    if (cls == "IfcQuantityTime")   return "TimeValue";
    if (cls == "IfcQuantityWeight") return "WeightValue";
    if (cls == "IfcQuantityNumber") return "NumberValue";
    return nullptr;
}

bool name_keyword_match(const std::string& name_lower, const char* const* kws) {
    for (const char* const* p = kws; *p; ++p) {
        if (name_lower.find(*p) != std::string::npos) return true;
    }
    return false;
}

std::string infer_qto_class(const std::string& name, const Entry& e) {
    std::string n = lowercase(name);
    bool is_int_like = (e.kind == Kind::INT || e.kind == Kind::TYPED_INT);
    bool is_float_like = (e.kind == Kind::DOUBLE || e.kind == Kind::TYPED_DOUBLE);
    if (is_float_like) {
        static const char* area_kws[]   = {"area", nullptr};
        static const char* volume_kws[] = {"volume", nullptr};
        static const char* weight_kws[] = {"weight", "mass", nullptr};
        static const char* length_kws[] = {"length", "width", "height", "depth", "distance", nullptr};
        static const char* time_kws[]   = {"time", "duration", nullptr};
        if (name_keyword_match(n, area_kws))   return "Area";
        if (name_keyword_match(n, volume_kws)) return "Volume";
        if (name_keyword_match(n, weight_kws)) return "Weight";
        if (name_keyword_match(n, length_kws)) return "Length";
        if (name_keyword_match(n, time_kws))   return "Time";
        return "Length";
    }
    if (is_int_like) return "Count";
    return "Length";
}

std::string qto_canonical_type(const std::string& name, const Entry& e, IfcUtil::IfcBaseClass* qto_template) {
    std::string explicit_type;
    if (e.kind == Kind::INSTANCE && e.inst) {
        explicit_type = e.inst->declaration().name();
    } else if ((e.kind == Kind::TYPED_DOUBLE || e.kind == Kind::TYPED_INT
                || e.kind == Kind::TYPED_BOOL || e.kind == Kind::TYPED_STRING)
               && !e.ifc_type.empty()) {
        explicit_type = e.ifc_type;
    }
    if (!explicit_type.empty()) {
        std::string n = explicit_type;
        // Strip "Ifc" prefix and "Measure" suffix.
        if (n.rfind("Ifc", 0) == 0) n = n.substr(3);
        const std::string suffix = "Measure";
        if (n.size() >= suffix.size() && n.compare(n.size()-suffix.size(), suffix.size(), suffix) == 0) {
            n = n.substr(0, n.size()-suffix.size());
        }
        if (n == "Numeric") n = "Number";
        if (n == "Mass")    n = "Weight";
        return n;
    }
    if (qto_template) {
        for (auto* pt : read_ref_aggregate(qto_template, "HasPropertyTemplates")) {
            if (read_string_attr(pt, "Name") != name) continue;
            std::string tt = read_string_attr(pt, "TemplateType");
            if (tt.size() > 2) {
                std::string suf = tt.substr(2);
                std::string out;
                if (!suf.empty()) {
                    out.reserve(suf.size());
                    for (auto& c : suf) out.push_back((char)std::tolower((unsigned char)c));
                    out[0] = (char)std::toupper((unsigned char)out[0]);
                }
                return out;
            }
        }
    }
    return infer_qto_class(name, e);
}

double entry_to_double(const Entry& e) {
    switch (e.kind) {
        case Kind::DOUBLE:        return e.d_val;
        case Kind::TYPED_DOUBLE:  return e.d_val;
        case Kind::INT:           return static_cast<double>(e.i_val);
        case Kind::TYPED_INT:     return static_cast<double>(e.i_val);
        case Kind::BOOL:          return e.b_val ? 1.0 : 0.0;
        case Kind::INSTANCE:
            if (e.inst) {
                try {
                    auto v = e.inst->get_attribute_value(0);
                    if (!v.isNull()) {
                        switch (v.type()) {
                            case IfcUtil::Argument_DOUBLE: return (double)v;
                            case IfcUtil::Argument_INT: return (double)((int)v);
                            default: break;
                        }
                    }
                } catch (...) {}
            }
            return 0.0;
        case Kind::STRING:
        case Kind::TYPED_STRING:
            try { return std::stod(e.s_val); } catch (...) { return 0.0; }
        default:
            return 0.0;
    }
}

int64_t entry_to_int(const Entry& e) {
    switch (e.kind) {
        case Kind::INT:        return e.i_val;
        case Kind::TYPED_INT:  return e.i_val;
        case Kind::DOUBLE:     return static_cast<int64_t>(e.d_val);
        case Kind::TYPED_DOUBLE: return static_cast<int64_t>(e.d_val);
        case Kind::BOOL:       return e.b_val ? 1 : 0;
        case Kind::INSTANCE:
            if (e.inst) {
                try {
                    auto v = e.inst->get_attribute_value(0);
                    if (!v.isNull()) {
                        switch (v.type()) {
                            case IfcUtil::Argument_INT: return (int)v;
                            case IfcUtil::Argument_DOUBLE: return (int64_t)((double)v);
                            default: break;
                        }
                    }
                } catch (...) {}
            }
            return 0;
        case Kind::STRING:
        case Kind::TYPED_STRING:
            try { return std::stoll(e.s_val); } catch (...) { return 0; }
        default:
            return 0;
    }
}

}  // namespace

IFCAPI_EXPORT bool ifcopenshell_api_pset_edit_qto(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* qto_h,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    ifcopenshell_ifc_instance_t* qto_template_h)
{
    if (!file_h || !qto_h) {
        set_error("ifcopenshell_api_pset_edit_qto: missing required argument");
        return false;
    }
    auto* file = file_h->ptr;
    auto* qto = qto_h->ptr;
    if (!file || !qto) {
        set_error("ifcopenshell_api_pset_edit_qto: NULL underlying pointer");
        return false;
    }
    try {
        if (name) write_string_attr(qto, "Name", std::string(name));

        const char* attr_name = entity_is_a(qto, "IfcPhysicalComplexQuantity") ? "HasQuantities" : "Quantities";

        std::unordered_map<std::string, const Entry*> remaining;
        std::vector<std::string> ordered_keys;
        if (properties) {
            for (const auto& e : properties->entries) {
                if (remaining.find(e.key) == remaining.end()) ordered_keys.push_back(e.key);
                remaining[e.key] = &e;
            }
        }

        IfcUtil::IfcBaseClass* qto_template = qto_template_h ? qto_template_h->ptr : nullptr;
        if (!qto_template) {
            std::string qname = read_string_attr(qto, "Name");
            if (!qname.empty()) {
                auto* cache = ifcopenshell_util_pset_get_template(file->schema()->name().c_str());
                if (cache) {
                    auto* h = ifcopenshell_util_pset_template_get_by_name(cache, qname.c_str());
                    if (h) { qto_template = h->ptr; delete h; }
                }
            }
        }

        std::vector<IfcUtil::IfcBaseClass*> existing = read_ref_aggregate(qto, attr_name);
        std::vector<IfcUtil::IfcBaseClass*> kept;

        for (auto* prop : existing) {
            std::string pname = read_string_attr(prop, "Name");
            auto it = remaining.find(pname);
            if (it == remaining.end()) {
                kept.push_back(prop);
                continue;
            }
            const Entry* e = it->second;
            if (e->kind == Kind::NONE) {
                file->removeEntity(prop);
                remaining.erase(it);
                continue;
            }
            if (e->kind == Kind::DICT && e->nested) {
                if (entity_is_a(prop, "IfcPhysicalComplexQuantity")) {
                    ifcopenshell_pset_props_t sub;
                    std::string discrim;
                    bool has_discrim = false;
                    for (const auto& se : e->nested->entries) {
                        if (se.key == "Discrimination" && se.kind == Kind::STRING) {
                            discrim = se.s_val;
                            has_discrim = true;
                        } else {
                            sub.entries.push_back(se);
                        }
                    }
                    if (has_discrim) write_string_attr(prop, "Discrimination", discrim);
                    ifcopenshell_ifc_file_t fh{file, false};
                    ifcopenshell_ifc_instance_t ih{prop, false};
                    ifcopenshell_api_pset_edit_qto(&fh, &ih, nullptr, &sub, nullptr);
                }
                kept.push_back(prop);
                remaining.erase(it);
                continue;
            }
            if (entity_is_a(prop, "IfcPhysicalSimpleQuantity")) {
                bool is_count = entity_is_a(prop, "IfcQuantityCount");
                bool is_ifc4x3 = (std::string(file->schema()->name()) == "IFC4X3");
                if (is_count && is_ifc4x3) {
                    int64_t v = entry_to_int(*e);
                    prop->set_attribute_value(3, static_cast<int>(v));
                } else {
                    prop->set_attribute_value(3, entry_to_double(*e));
                }
                kept.push_back(prop);
                remaining.erase(it);
                continue;
            }
            // Bounded/Reference/Table value subtypes are kept as-is; only IfcPropertySingleValue is rewritten.
            kept.push_back(prop);
            remaining.erase(it);
        }

        for (const auto& key : ordered_keys) {
            auto it = remaining.find(key);
            if (it == remaining.end()) continue;
            const Entry* e = it->second;
            if (e->kind == Kind::NONE) continue;

            if (e->kind == Kind::DICT && e->nested) {
                ifcopenshell_pset_props_t sub;
                std::string discrim;
                for (const auto& se : e->nested->entries) {
                    if (se.key == "Discrimination" && se.kind == Kind::STRING) {
                        discrim = se.s_val;
                    } else {
                        sub.entries.push_back(se);
                    }
                }
                const IfcParse::declaration* cq_decl = nullptr;
                try { cq_decl = file->schema()->declaration_by_name("IfcPhysicalComplexQuantity"); }
                catch (...) { cq_decl = nullptr; }
                if (!cq_decl) continue;
                auto* cq = file->create(cq_decl);
                write_string_attr(cq, "Name", key);
                write_string_attr(cq, "Discrimination", discrim);
                ifcopenshell_ifc_file_t fh{file, false};
                ifcopenshell_ifc_instance_t ih{cq, false};
                ifcopenshell_api_pset_edit_qto(&fh, &ih, nullptr, &sub, nullptr);
                kept.push_back(cq);
                continue;
            }

            std::string canonical = qto_canonical_type(key, *e, qto_template);
            std::string ifc_class = "IfcQuantity" + canonical;
            const char* value_attr = qto_value_attr_for_class(ifc_class);
            if (!value_attr) continue;
            const IfcParse::declaration* decl = nullptr;
            try { decl = file->schema()->declaration_by_name(ifc_class); } catch (...) { decl = nullptr; }
            if (!decl) continue;
            auto* nq = file->create(decl);
            write_string_attr(nq, "Name", key);
            int va = attr_index_of(nq, value_attr);
            if (va >= 0) {
                bool is_count = (ifc_class == "IfcQuantityCount");
                bool is_ifc4x3 = (std::string(file->schema()->name()) == "IFC4X3");
                if (is_count && is_ifc4x3) {
                    nq->set_attribute_value(static_cast<size_t>(va), static_cast<int>(entry_to_int(*e)));
                } else {
                    nq->set_attribute_value(static_cast<size_t>(va), entry_to_double(*e));
                }
            }
            kept.push_back(nq);
        }

        write_ref_aggregate(qto, attr_name, kept);
        return true;
    } catch (const std::exception& ex) {
        set_error(std::string("ifcopenshell_api_pset_edit_qto: ") + ex.what());
        return false;
    }
}

}  // extern "C"
