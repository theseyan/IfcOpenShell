// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/pset_template.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcopenshell_api_internal.hpp"
#include "api/pset/props.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/exception.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

using ifcapi_pset::Entry;
using ifcapi_pset::Kind;
using ifcapi::detail::attr_index_of;
using ifcapi::detail::entity_has_attr;
using ifcapi::detail::find_attr_index;
using ifcapi::detail::is_attr_null;
using ifcapi::detail::read_ref_aggregate;
using ifcapi::detail::read_ref_attr;
using ifcapi::detail::read_string_attr;
using ifcapi::detail::write_blank_attr;
using ifcapi::detail::write_ref_aggregate;
using ifcapi::detail::write_ref_attr;
using ifcapi::detail::write_string_attr;

using ifcapi::detail::set_error;

ifcapi::detail::Error value_error(const std::string& message) {
    return {ifcapi::detail::ERROR_VALUE, ifcapi::detail::ERROR_CODE_UNSPECIFIED, message};
}

ifcapi::detail::Error not_implemented_error(const std::string& message) {
    return {ifcapi::detail::ERROR_NOT_IMPLEMENTED, ifcapi::detail::ERROR_CODE_UNSPECIFIED, message};
}

ifcapi::detail::Error key_error(const std::string& key) {
    return {ifcapi::detail::ERROR_KEY, ifcapi::detail::ERROR_CODE_UNSPECIFIED, key};
}

bool entity_is_a(express::Base e, const char* name) {
    if (!e) return false;
    return e.declaration().is(name);
}

// Walk a parameter_type chain to its underlying simple_type.
const ifcopenshell::simple_type* underlying_simple_type(const ifcopenshell::type_declaration* td) {
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
express::Base create_typed_value(ifcopenshell::file* file, const std::string& type_name,
                                          const Entry& src, bool from_list = false,
                                          size_t list_idx = 0) {
    const ifcopenshell::declaration* decl = nullptr;
    try {
        decl = file->schema()->declaration_by_name(type_name);
    } catch (...) {
        return {};
    }
    auto* type_decl = decl ? decl->as_type_declaration() : nullptr;
    if (!type_decl) return {};

    auto inst = file->create(type_decl);
    if (!inst) return {};

    auto* simple = underlying_simple_type(type_decl);
    auto kind = simple ? simple->declared_type() : ifcopenshell::simple_type::string_type;

    auto write_string = [&](const std::string& s) {
        inst.set_attribute_value(0, s);
    };
    auto write_double = [&](double d) {
        inst.set_attribute_value(0, d);
    };
    auto write_int = [&](int i) {
        inst.set_attribute_value(0, i);
    };
    auto write_bool = [&](bool b) {
        inst.set_attribute_value(0, b);
    };

    if (from_list) {
        switch (src.kind) {
            case Kind::BOOL_LIST:
                if (list_idx < src.bool_list.size()) write_bool(static_cast<bool>(src.bool_list[list_idx]));
                break;
            case Kind::STRING_LIST:
                if (list_idx < src.str_list.size()) write_string(src.str_list[list_idx]);
                break;
            case Kind::DOUBLE_LIST:
                if (list_idx < src.dbl_list.size()) {
                    if (kind == ifcopenshell::simple_type::integer_type) {
                        write_int(static_cast<int>(src.dbl_list[list_idx]));
                    } else if (kind == ifcopenshell::simple_type::string_type) {
                        write_string(std::to_string(src.dbl_list[list_idx]));
                    } else {
                        write_double(src.dbl_list[list_idx]);
                    }
                }
                break;
            case Kind::INT_LIST:
                if (list_idx < src.int_list.size()) {
                    if (kind == ifcopenshell::simple_type::real_type ||
                        kind == ifcopenshell::simple_type::number_type) {
                        write_double(static_cast<double>(src.int_list[list_idx]));
                    } else if (kind == ifcopenshell::simple_type::string_type) {
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
        case ifcopenshell::simple_type::string_type:
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
        case ifcopenshell::simple_type::real_type:
        case ifcopenshell::simple_type::number_type:
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
        case ifcopenshell::simple_type::integer_type:
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
        case ifcopenshell::simple_type::boolean_type:
        case ifcopenshell::simple_type::logical_type:
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
std::string primary_measure_type(express::Base pset_template, const std::string& prop_name,
                                 express::Base old_value, const Entry& new_value) {
    // An explicit type from the new value (TYPED_* kinds or an INSTANCE
    // wrapping a typed value) overrides any old/template-derived type;
    // mirrors upstream where ``prop.NominalValue = file.createIfcBoolean(...)``
    // swaps the underlying value type.
    if (new_value.kind == Kind::TYPED_STRING || new_value.kind == Kind::TYPED_DOUBLE
        || new_value.kind == Kind::TYPED_INT || new_value.kind == Kind::TYPED_BOOL) {
        return new_value.ifc_type;
    }
    if (new_value.kind == Kind::INSTANCE && new_value.inst) {
        return new_value.inst.declaration().name();
    }
    if (old_value) {
        return old_value.declaration().name();
    }
    if (pset_template) {
        for (auto pt : read_ref_aggregate(pset_template, "HasPropertyTemplates")) {
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
const char* properties_attr_name(express::Base pset) {
    if (entity_has_attr(pset, "HasProperties")) return "HasProperties";
    if (entity_has_attr(pset, "Properties"))    return "Properties";
    if (entity_is_a(pset, "IfcMaterialProperties") && entity_has_attr(pset, "ExtendedProperties"))
        return "ExtendedProperties";
    return {};
}

uint32_t total_inverses(ifcopenshell::file* file, express::Base e) {
    if (!e || !file) return 0;
    try {
        return static_cast<uint32_t>(file->instances_by_reference(static_cast<int>(e.id())).size());
    } catch (...) {
        return 0;
    }
}

express::Base copy_property_enumeration(ifcopenshell::file* file, express::Base source) {
    if (!file || !source) return {};
    auto result = file->create(&source.declaration());
    if (!result) return {};

    write_string_attr(result, "Name", read_string_attr(source, "Name"));
    if (is_attr_null(source, "EnumerationValues")) {
        write_blank_attr(result, "EnumerationValues");
    } else {
        write_ref_aggregate(result, "EnumerationValues", read_ref_aggregate(source, "EnumerationValues"));
    }
    write_ref_attr(result, "Unit", read_ref_attr(source, "Unit"));
    return result;
}

bool process_existing_single_value(ifcopenshell::file* file, express::Base prop,
                                   express::Base pset_template,
                                   const Entry& entry, bool should_purge,
                                   bool& removed) {
    removed = false;
    auto kind = entry.kind;
    if (kind == Kind::NONE) {
        if (should_purge) {
            file->remove_entity(prop);
            removed = true;
            return true;
        }
        // Set NominalValue = NULL.
        int idx = attr_index_of(prop, "NominalValue");
        if (idx >= 0) prop.set_attribute_value(static_cast<size_t>(idx), blank{});
        return true;
    }

    if (kind == Kind::INSTANCE) {
        // Caller passed an entity_instance already wrapping a typed value (or a raw IfcValue/IfcSimpleProperty).
        if (!entry.inst) return false;
        if (entity_is_a(entry.inst, "IfcValue") || entry.inst.declaration().as_type_declaration()) {
            int idx = attr_index_of(prop, "NominalValue");
            if (idx >= 0) prop.set_attribute_value(static_cast<size_t>(idx), entry.inst);
        } else {
            throw value_error(
                entry.inst.declaration().name() + " cannot be assigned to the property set '"
                + read_string_attr(prop, "Name") + "'");
        }
        if (entry.unit) {
            int u_idx = attr_index_of(prop, "Unit");
            if (u_idx >= 0) prop.set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
        }
        return true;
    }

    auto old_value = read_ref_attr(prop, "NominalValue");
    std::string pmt = primary_measure_type(pset_template, read_string_attr(prop, "Name"), old_value, entry);
    if (pmt.empty()) pmt = "IfcLabel";
    auto typed = create_typed_value(file, pmt, entry);
    if (!typed) return false;
    int idx = attr_index_of(prop, "NominalValue");
    if (idx >= 0) prop.set_attribute_value(static_cast<size_t>(idx), typed);
    if (entry.unit) {
        int u_idx = attr_index_of(prop, "Unit");
        if (u_idx >= 0) prop.set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
    }
    return true;
}

bool process_existing_enumerated(ifcopenshell::file* file, express::Base prop,
                                 const Entry& entry, bool should_purge, bool& removed) {
    removed = false;
    if (entry.kind == Kind::INSTANCE && entry.inst && entity_is_a(entry.inst, "IfcPropertyEnumeratedValue")) {
        if (is_attr_null(entry.inst, "EnumerationValues")) {
            if (should_purge) {
                file->remove_entity(prop);
                removed = true;
                return true;
            }
            write_blank_attr(prop, "EnumerationValues");
        } else {
            write_ref_aggregate(prop, "EnumerationValues", read_ref_aggregate(entry.inst, "EnumerationValues"));
        }

        auto value_reference = read_ref_attr(entry.inst, "EnumerationReference");
        auto prop_reference = read_ref_attr(prop, "EnumerationReference");
        if (!value_reference) {
            if (prop_reference) ifcapi::bindings::entity_remove_deep(&prop_reference);
            write_blank_attr(prop, "EnumerationReference");
        } else if (!prop_reference) {
            write_ref_attr(prop, "EnumerationReference", copy_property_enumeration(file, value_reference));
        } else {
            write_string_attr(prop_reference, "Name", read_string_attr(value_reference, "Name"));
            if (is_attr_null(value_reference, "EnumerationValues")) {
                write_blank_attr(prop_reference, "EnumerationValues");
            } else {
                write_ref_aggregate(
                    prop_reference, "EnumerationValues", read_ref_aggregate(value_reference, "EnumerationValues"));
            }
            write_ref_attr(prop_reference, "Unit", read_ref_attr(value_reference, "Unit"));
        }
        if (entry.unit) write_ref_attr(prop, "Unit", entry.unit);
        return true;
    }

    if (entry.kind != Kind::BOOL_LIST && entry.kind != Kind::STRING_LIST && entry.kind != Kind::DOUBLE_LIST
        && entry.kind != Kind::INT_LIST) {
        throw value_error(
            "Value is not a valid value for enum property " + read_string_attr(prop, "Name"));
    }
    bool empty_list = (entry.kind == Kind::BOOL_LIST && entry.bool_list.empty())
        || (entry.kind == Kind::STRING_LIST && entry.str_list.empty())
        || (entry.kind == Kind::DOUBLE_LIST && entry.dbl_list.empty())
        || (entry.kind == Kind::INT_LIST && entry.int_list.empty());
    if (empty_list && should_purge) {
        file->remove_entity(prop);
        removed = true;
        return true;
    }

    std::string pmt;
    auto ref = read_ref_attr(prop, "EnumerationReference");
    if (ref) {
        auto vals = read_ref_aggregate(ref, "EnumerationValues");
        if (!vals.empty()) pmt = vals.front().declaration().name();
    }
    if (pmt.empty()) {
        auto vals = read_ref_aggregate(prop, "EnumerationValues");
        if (!vals.empty()) pmt = vals.front().declaration().name();
    }
    if (pmt.empty()) pmt = "IfcLabel";

    std::vector<express::Base> sel_vals;
    size_t n = (entry.kind == Kind::BOOL_LIST) ? entry.bool_list.size()
        : (entry.kind == Kind::STRING_LIST) ? entry.str_list.size()
        : (entry.kind == Kind::DOUBLE_LIST) ? entry.dbl_list.size() : entry.int_list.size();
    for (size_t i = 0; i < n; ++i) {
        auto tv = create_typed_value(file, pmt, entry, true, i);
        if (tv) sel_vals.push_back(tv);
    }
    if (sel_vals.empty()) {
        int idx = attr_index_of(prop, "EnumerationValues");
        if (idx >= 0) prop.set_attribute_value(static_cast<size_t>(idx), blank{});
    } else {
        write_ref_aggregate(prop, "EnumerationValues", sel_vals);
    }
    return true;
}

express::Base build_new_property(ifcopenshell::file* file, express::Base pset_template,
                                          const std::string& name, const Entry& entry) {
    if (entry.kind == Kind::INSTANCE) {
        if (!entry.inst) return {};
        if (entity_is_a(entry.inst, "IfcProperty")) {
            return entry.inst;
        }
        if (!entry.inst.declaration().as_type_declaration() && !entity_is_a(entry.inst, "IfcValue")) {
            throw value_error(
                entry.inst.declaration().name() + " cannot be assigned to the property set '" + name + "'");
        }
        const auto* sv_decl = file->schema()->declaration_by_name("IfcPropertySingleValue");
        auto sv = file->create(sv_decl);
        write_string_attr(sv, "Name", name);
        int nv_idx = attr_index_of(sv, "NominalValue");
        if (nv_idx >= 0) sv.set_attribute_value(static_cast<size_t>(nv_idx), entry.inst);
        if (entry.unit) {
            int u_idx = attr_index_of(sv, "Unit");
            if (u_idx >= 0) sv.set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
        }
        return sv;
    }

    if (entry.kind == Kind::BOOL_LIST || entry.kind == Kind::STRING_LIST || entry.kind == Kind::DOUBLE_LIST
        || entry.kind == Kind::INT_LIST) {
        size_t n = (entry.kind == Kind::BOOL_LIST) ? entry.bool_list.size()
            : (entry.kind == Kind::STRING_LIST) ? entry.str_list.size()
            : (entry.kind == Kind::DOUBLE_LIST) ? entry.dbl_list.size() : entry.int_list.size();
        if (n == 0) return {};
        if (!pset_template) {
            throw not_implemented_error("No template found for property '" + name + "'");
        }
        for (auto pt : read_ref_aggregate(pset_template, "HasPropertyTemplates")) {
            if (read_string_attr(pt, "Name") != name) continue;
            std::string tt = read_string_attr(pt, "TemplateType");
            std::string pmt = read_string_attr(pt, "PrimaryMeasureType");
            if (tt == "P_LISTVALUE") {
                if (pmt.empty()) {
                    throw value_error("pset template '" + name + "' is missing PrimaryMeasureType");
                }
                std::vector<express::Base> items;
                for (size_t i = 0; i < n; ++i) {
                    if (auto tv = create_typed_value(file, pmt, entry, true, i)) items.push_back(tv);
                }
                const auto* lv_decl = file->schema()->declaration_by_name("IfcPropertyListValue");
                auto lv = file->create(lv_decl);
                write_string_attr(lv, "Name", name);
                write_ref_aggregate(lv, "ListValues", items);
                return lv;
            }
            if (tt == "P_ENUMERATEDVALUE") {
                if (pmt.empty()) pmt = "IfcLabel";
                auto enumerators = read_ref_attr(pt, "Enumerators");
                std::vector<express::Base> enum_value_items;
                if (enumerators) enum_value_items = read_ref_aggregate(enumerators, "EnumerationValues");

                const auto* pe_decl = file->schema()->declaration_by_name("IfcPropertyEnumeration");
                auto pe = file->create(pe_decl);
                write_string_attr(pe, "Name", name);
                write_ref_aggregate(pe, "EnumerationValues", enum_value_items);

                std::vector<express::Base> sel;
                for (size_t i = 0; i < n; ++i) {
                    if (auto tv = create_typed_value(file, pmt, entry, true, i)) sel.push_back(tv);
                }
                const auto* pev_decl = file->schema()->declaration_by_name("IfcPropertyEnumeratedValue");
                auto pev = file->create(pev_decl);
                write_string_attr(pev, "Name", name);
                write_ref_aggregate(pev, "EnumerationValues", sel);
                int er_idx = attr_index_of(pev, "EnumerationReference");
                if (er_idx >= 0) pev.set_attribute_value(static_cast<size_t>(er_idx), pe);
                return pev;
            }
            throw not_implemented_error("Template type '" + tt + "' is not supported yet");
        }
        throw not_implemented_error("No template found for property '" + name + "'");
    }

    // Scalar single value path.
    Entry tmp = entry;
    std::string pmt = primary_measure_type(pset_template, name, {}, tmp);
    if (pmt.empty()) pmt = "IfcLabel";
    auto tv = create_typed_value(file, pmt, entry);
    if (!tv) return {};
    const auto* sv_decl = file->schema()->declaration_by_name("IfcPropertySingleValue");
    auto sv = file->create(sv_decl);
    write_string_attr(sv, "Name", name);
    int nv_idx = attr_index_of(sv, "NominalValue");
    if (nv_idx >= 0) sv.set_attribute_value(static_cast<size_t>(nv_idx), tv);
    if (entry.unit) {
        int u_idx = attr_index_of(sv, "Unit");
        if (u_idx >= 0) sv.set_attribute_value(static_cast<size_t>(u_idx), entry.unit);
    }
    return sv;
}

express::Base fetch_pset_template(ifcopenshell::file* file, express::Base pset,
                                           express::Base explicit_template) {
    if (explicit_template) return explicit_template;
    std::string name = read_string_attr(pset, "Name");
    if (name.empty()) return {};
    std::vector<std::string> schemas = {file->schema()->name()};
    if (schemas.front().rfind("IFC2X3", 0) == 0) {
        schemas.push_back("IFC4");
        schemas.push_back("IFC4X3");
    }
    for (const auto& schema : schemas) {
        auto* cache = ifcapi::bindings::pset_template_get_template(schema);
        if (!cache) continue;
        if (auto tmpl = ifcapi::bindings::pset_template_get_by_name(cache, name)) return tmpl;
    }
    return {};
}

}  // namespace

namespace ifcapi {
namespace bindings {

/* ---- Properties builder ---- */

ifcopenshell_pset_props_t* pset_props_new() {
    return new ifcopenshell_pset_props_t();
}

void pset_props_free(ifcopenshell_pset_props_t* p) {
    delete p;
}

static Entry& append_entry(ifcopenshell_pset_props_t* p, const std::string& key) {
    p->entries.emplace_back();
    p->entries.back().key = key;
    return p->entries.back();
}

void pset_props_set_null(ifcopenshell_pset_props_t* p, const std::string& key) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::NONE;
}

void pset_props_set_bool(ifcopenshell_pset_props_t* p, const std::string& key, bool v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::BOOL;
    e.b_val = v;
}

void pset_props_set_bool_list(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::vector<bool>& vals) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::BOOL_LIST;
    e.bool_list = vals;
}

void pset_props_set_int(ifcopenshell_pset_props_t* p, const std::string& key, int64_t v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INT;
    e.i_val = v;
}

void pset_props_set_double(ifcopenshell_pset_props_t* p, const std::string& key, double v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DOUBLE;
    e.d_val = v;
}

void pset_props_set_string(ifcopenshell_pset_props_t* p, const std::string& key, const std::string& v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::STRING;
    e.s_val = v;
}

void pset_props_set_instance(ifcopenshell_pset_props_t* p, const std::string& key, std::optional<express::Base> v) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INSTANCE;
    e.inst = v.value_or(express::Base{});
}

void pset_props_set_typed_string(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::string& v, const std::string& ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_STRING;
    e.s_val = v;
    e.ifc_type = ifc_type;
}

void pset_props_set_typed_double(
    ifcopenshell_pset_props_t* p, const std::string& key, double v, const std::string& ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_DOUBLE;
    e.d_val = v;
    e.ifc_type = ifc_type;
}

void pset_props_set_typed_int(
    ifcopenshell_pset_props_t* p, const std::string& key, int64_t v, const std::string& ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_INT;
    e.i_val = v;
    e.ifc_type = ifc_type;
}

void pset_props_set_typed_bool(
    ifcopenshell_pset_props_t* p, const std::string& key, bool v, const std::string& ifc_type) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::TYPED_BOOL;
    e.b_val = v;
    e.ifc_type = ifc_type;
}

void pset_props_set_string_list(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::vector<std::string>& vals) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::STRING_LIST;
    e.str_list = vals;
}

void pset_props_set_double_list(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::vector<double>& vals) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DOUBLE_LIST;
    e.dbl_list = vals;
}

void pset_props_set_int_list(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::vector<int64_t>& vals) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INT_LIST;
    e.int_list = vals;
}

void pset_props_set_instance_list(
    ifcopenshell_pset_props_t* p, const std::string& key, const std::vector<express::Base>& vals) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::INSTANCE_LIST;
    e.inst_list = vals;
}

void pset_props_set_date(ifcopenshell_pset_props_t* p, const std::string& key, int year, int month, int day) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DATE;
    e.year = year;
    e.month = month;
    e.day = day;
}

void pset_props_set_datetime(
    ifcopenshell_pset_props_t* p, const std::string& key, int year, int month, int day, int hour, int minute, int second,
    int microsecond, bool has_timezone, int timezone_offset_minutes) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DATETIME;
    e.year = year;
    e.month = month;
    e.day = day;
    e.hour = hour;
    e.minute = minute;
    e.second = second;
    e.microsecond = microsecond;
    e.has_timezone = has_timezone;
    e.timezone_offset_minutes = timezone_offset_minutes;
}

void pset_props_set_duration(
    ifcopenshell_pset_props_t* p, const std::string& key, bool negative, int years, int months, int days, int hours,
    int minutes, int seconds, int microseconds) {
    if (!p) return;
    auto& e = append_entry(p, key);
    e.kind = Kind::DURATION;
    e.duration_negative = negative;
    e.duration_years = years;
    e.duration_months = months;
    e.duration_days = days;
    e.duration_hours = hours;
    e.duration_minutes = minutes;
    e.duration_seconds = seconds;
    e.duration_microseconds = microseconds;
}

void pset_props_set_dict(ifcopenshell_pset_props_t* outer, const std::string& key, ifcopenshell_pset_props_t* inner) {
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
void pset_props_set_unit_for_last(ifcopenshell_pset_props_t* p, std::optional<express::Base> unit) {
    if (!p || p->entries.empty()) return;
    p->entries.back().unit = unit.value_or(express::Base{});
}

/* ---- edit_pset ---- */

bool pset_edit_pset(
    ifcopenshell::file* file,
    const PsetEditPsetOptions& options)
{
    auto pset = options.pset;
    auto pset_template = options.pset_template.value_or(express::Base());
    if (!file || !pset) {
        set_error("pset_edit_pset: missing required argument");
        return false;
    }
    try {
        if (options.name) write_string_attr(pset, "Name", *options.name);

        // Map of remaining property entries (key → entry pointer for direct lookup
        // / removal). We track removal from the dict similarly to the Python loop.
        std::unordered_map<std::string, const Entry*> remaining;
        std::vector<std::string> ordered_keys;
        if (options.properties) {
            for (const auto& e : options.properties->entries) {
                if (remaining.find(e.key) == remaining.end()) ordered_keys.push_back(e.key);
                remaining[e.key] = &e;
            }
        }

        pset_template = fetch_pset_template(file, pset, pset_template);

        const char* attr_name = properties_attr_name(pset);
        if (!attr_name) {
            set_error("Pset has no HasProperties/Properties/ExtendedProperties attribute");
            return false;
        }

        std::vector<express::Base> existing = read_ref_aggregate(pset, attr_name);
        std::vector<express::Base> kept;

        for (auto prop : existing) {
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
                if (!process_existing_single_value(file, prop, pset_template, *e, options.should_purge, removed)) {
                    kept.push_back(prop);
                    continue;
                }
            } else if (entity_is_a(prop, "IfcPropertyEnumeratedValue")) {
                if (!process_existing_enumerated(file, prop, *e, options.should_purge, removed)) {
                    kept.push_back(prop);
                    continue;
                }
            } else {
                throw not_implemented_error(
                    "Updating '" + prop.declaration().name() + "' properties is not supported yet");
            }
            if (!removed) kept.push_back(prop);
            remaining.erase(it);
        }

        // Add new properties for the keys still in `remaining`, preserving original order.
        for (const auto& key : ordered_keys) {
            auto it = remaining.find(key);
            if (it == remaining.end()) continue;
            const Entry* e = it->second;
            if (e->kind == Kind::NONE && options.should_purge) continue;
            auto np = build_new_property(file, pset_template, key, *e);
            if (np) kept.push_back(np);
        }

        write_ref_aggregate(pset, attr_name, kept);
        return true;
    } catch (const std::exception& ex) {
        ifcapi::detail::set_error(ex);
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

std::string qto_canonical_type(const std::string& name, const Entry& e, express::Base qto_template) {
    std::string explicit_type;
    if (e.kind == Kind::INSTANCE && e.inst) {
        explicit_type = e.inst.declaration().name();
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
        for (auto pt : read_ref_aggregate(qto_template, "HasPropertyTemplates")) {
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
                    auto v = e.inst.get_attribute_value(0);
                    if (!v.isNull()) {
                        switch (v.type()) {
                            case ifcopenshell::Argument_DOUBLE: return (double)v;
                            case ifcopenshell::Argument_INT: return (double)((int)v);
                            default: break;
                        }
                    }
                } catch (...) {}
            }
            throw std::runtime_error(e.inst ? "Unsupported quantity value type: " + e.inst.declaration().name()
                                            : "Unsupported quantity value type");
        case Kind::STRING:
        case Kind::TYPED_STRING:
            try {
                size_t parsed = 0;
                double value = std::stod(e.s_val, &parsed);
                if (parsed != e.s_val.size()) throw std::invalid_argument("trailing characters");
                return value;
            } catch (...) {
                throw value_error("Invalid numeric quantity value: " + e.s_val);
            }
        default:
            throw std::runtime_error("Unsupported quantity value type");
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
                    auto v = e.inst.get_attribute_value(0);
                    if (!v.isNull()) {
                        switch (v.type()) {
                            case ifcopenshell::Argument_INT: return (int)v;
                            case ifcopenshell::Argument_DOUBLE: return (int64_t)((double)v);
                            default: break;
                        }
                    }
                } catch (...) {}
            }
            throw std::runtime_error(e.inst ? "Unsupported quantity value type: " + e.inst.declaration().name()
                                            : "Unsupported quantity value type");
        case Kind::STRING:
        case Kind::TYPED_STRING:
            try {
                size_t parsed = 0;
                int64_t value = std::stoll(e.s_val, &parsed);
                if (parsed != e.s_val.size()) throw std::invalid_argument("trailing characters");
                return value;
            } catch (...) {
                throw value_error("Invalid integer quantity value: " + e.s_val);
            }
        default:
            throw std::runtime_error("Unsupported quantity value type");
    }
}

}  // namespace

bool pset_edit_qto(
    ifcopenshell::file* file,
    const PsetEditQtoOptions& options)
{
    auto qto = options.qto;
    auto qto_template = options.qto_template.value_or(express::Base());
    if (!file || !qto) {
        set_error("pset_edit_qto: missing required argument");
        return false;
    }
    try {
        if (options.name) write_string_attr(qto, "Name", *options.name);

        const char* attr_name = entity_is_a(qto, "IfcPhysicalComplexQuantity") ? "HasQuantities" : "Quantities";

        std::unordered_map<std::string, const Entry*> remaining;
        std::vector<std::string> ordered_keys;
        if (options.properties) {
            for (const auto& e : options.properties->entries) {
                if (remaining.find(e.key) == remaining.end()) ordered_keys.push_back(e.key);
                remaining[e.key] = &e;
            }
        }

        if (!qto_template) {
            std::string qname = read_string_attr(qto, "Name");
            if (!qname.empty()) {
                auto* cache = ifcapi::bindings::pset_template_get_template(file->schema()->name());
                if (cache) {
                    qto_template = ifcapi::bindings::pset_template_get_by_name(cache, qname);
                }
            }
        }

        std::vector<express::Base> existing = read_ref_aggregate(qto, attr_name);
        std::vector<express::Base> kept;

        for (auto prop : existing) {
            std::string pname = read_string_attr(prop, "Name");
            auto it = remaining.find(pname);
            if (it == remaining.end()) {
                kept.push_back(prop);
                continue;
            }
            const Entry* e = it->second;
            if (e->kind == Kind::NONE) {
                file->remove_entity(prop);
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
                    pset_edit_qto(file, PsetEditQtoOptions{prop, {}, &sub, {}});
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
                    prop.set_attribute_value(3, static_cast<int>(v));
                } else {
                    prop.set_attribute_value(3, entry_to_double(*e));
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
                bool has_discrim = false;
                bool has_quantities = false;
                for (const auto& se : e->nested->entries) {
                    if (se.key == "Discrimination" && se.kind == Kind::STRING) {
                        discrim = se.s_val;
                        has_discrim = true;
                    } else {
                        sub.entries.push_back(se);
                        has_quantities = true;
                    }
                }
                if (!has_discrim) throw key_error("Discrimination");
                if (!has_quantities) throw key_error("HasQuantities");
                const ifcopenshell::declaration* cq_decl = nullptr;
                try { cq_decl = file->schema()->declaration_by_name("IfcPhysicalComplexQuantity"); }
                catch (...) { cq_decl = nullptr; }
                if (!cq_decl) continue;
                auto cq = file->create(cq_decl);
                write_string_attr(cq, "Name", key);
                write_string_attr(cq, "Discrimination", discrim);
                pset_edit_qto(file, PsetEditQtoOptions{cq, {}, &sub, {}});
                kept.push_back(cq);
                continue;
            }

            std::string canonical = qto_canonical_type(key, *e, qto_template);
            std::string ifc_class = "IfcQuantity" + canonical;
            const char* value_attr = qto_value_attr_for_class(ifc_class);
            if (!value_attr) throw std::runtime_error("Unsupported quantity type: " + ifc_class);
            const ifcopenshell::declaration* decl = nullptr;
            try { decl = file->schema()->declaration_by_name(ifc_class); } catch (...) { decl = nullptr; }
            if (!decl) throw std::runtime_error("Unsupported quantity type: " + ifc_class);
            auto nq = file->create(decl);
            write_string_attr(nq, "Name", key);
            int va = attr_index_of(nq, value_attr);
            if (va >= 0) {
                bool is_count = (ifc_class == "IfcQuantityCount");
                bool is_ifc4x3 = (std::string(file->schema()->name()) == "IFC4X3");
                if (is_count && is_ifc4x3) {
                    nq.set_attribute_value(static_cast<size_t>(va), static_cast<int>(entry_to_int(*e)));
                } else {
                    nq.set_attribute_value(static_cast<size_t>(va), entry_to_double(*e));
                }
            }
            kept.push_back(nq);
        }

        write_ref_aggregate(qto, attr_name, kept);
        return true;
    } catch (const std::exception& ex) {
        ifcapi::detail::set_error(ex);
        return false;
    }
}

} // namespace bindings
} // namespace ifcapi
