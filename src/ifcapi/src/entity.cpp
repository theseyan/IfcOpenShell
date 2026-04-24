// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"
#include "ifcparse/utils.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

// Shared error helpers (defined in root.cpp)
#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

static char* alloc_cstr(const std::string& s) {
    char* result = static_cast<char*>(std::malloc(s.size() + 1));
    if (result) std::memcpy(result, s.c_str(), s.size() + 1);
    return result;
}

static IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

static const IfcParse::enumeration_type* resolve_enum_type_for_attr(
    const IfcParse::entity* entity_decl, size_t attr_idx)
{
    auto all_attrs = entity_decl->all_attributes();
    if (attr_idx >= all_attrs.size()) return nullptr;
    const IfcParse::parameter_type* pt = all_attrs[attr_idx]->type_of_attribute();
    while (pt) {
        auto* named = pt->as_named_type();
        if (!named) break;
        auto* decl = named->declared_type();
        if (auto* et = decl->as_enumeration_type()) return et;
        if (auto* td = decl->as_type_declaration()) {
            pt = td->declared_type();
        } else {
            break;
        }
    }
    return nullptr;
}

extern "C" {

const char* ifcopenshell_entity_type(const ifcopenshell_ifc_instance_t* instance) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return "";
    return e->declaration().name().c_str();
}

ifcopenshell_ifc_file_t* ifcopenshell_ifc_instance_file(const ifcopenshell_ifc_instance_t* instance) {
    auto* e = instance ? instance->ptr : nullptr;
    auto* file = e ? e->file_ : nullptr;
    return file ? ifcopenshell::capi::wrap_file(file, false) : nullptr;
}

bool ifcopenshell_entity_is_a(const ifcopenshell_ifc_instance_t* instance, const char* type_name) {
    if (!type_name) return false;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    return e->declaration().is(type_name);
}

bool ifcopenshell_entity_has_attr(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    if (!attr) return false;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    auto* entity_decl = e->declaration().as_entity();
    if (!entity_decl) return false;
    return entity_decl->attribute_index(attr) >= 0;
}

ifcopenshell_attr_type ifcopenshell_entity_attr_type(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    if (!attr) return IFCAPI_ATTR_UNKNOWN;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return IFCAPI_ATTR_UNKNOWN;
    auto* entity_decl = e->declaration().as_entity();
    if (!entity_decl) return IFCAPI_ATTR_UNKNOWN;

    auto idx = entity_decl->attribute_index(attr);
    if (idx < 0) return IFCAPI_ATTR_UNKNOWN;

    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return IFCAPI_ATTR_NULL;
        auto t = val.type();
        switch (t) {
            case IfcUtil::Argument_STRING: return IFCAPI_ATTR_STRING;
            case IfcUtil::Argument_INT: return IFCAPI_ATTR_INT;
            case IfcUtil::Argument_DOUBLE: return IFCAPI_ATTR_DOUBLE;
            case IfcUtil::Argument_BOOL:
            case IfcUtil::Argument_LOGICAL: return IFCAPI_ATTR_BOOL;
            case IfcUtil::Argument_ENUMERATION: return IFCAPI_ATTR_ENUM;
            case IfcUtil::Argument_ENTITY_INSTANCE: return IFCAPI_ATTR_REFERENCE;
            case IfcUtil::Argument_DERIVED: return IFCAPI_ATTR_DERIVED;
            case IfcUtil::Argument_AGGREGATE_OF_INT:
            case IfcUtil::Argument_AGGREGATE_OF_DOUBLE:
            case IfcUtil::Argument_AGGREGATE_OF_STRING:
            case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE:
            case IfcUtil::Argument_AGGREGATE_OF_BINARY:
            case IfcUtil::Argument_AGGREGATE_OF_EMPTY_AGGREGATE:
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE:
                return IFCAPI_ATTR_AGGREGATE;
            default:
                return IFCAPI_ATTR_UNKNOWN;
        }
    } catch (...) {
        return IFCAPI_ATTR_NULL;
    }
}

bool ifcopenshell_entity_attr_is_null(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    return ifcopenshell_entity_attr_type(instance, attr) == IFCAPI_ATTR_NULL;
}

uint32_t ifcopenshell_entity_attr_count(const ifcopenshell_ifc_instance_t* instance) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return 0;
    auto* entity_decl = e->declaration().as_entity();
    if (!entity_decl) return 0;
    return static_cast<uint32_t>(entity_decl->all_attributes().size());
}

char* ifcopenshell_entity_attr_name(const ifcopenshell_ifc_instance_t* instance, uint32_t index) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return nullptr;
    auto* entity_decl = e->declaration().as_entity();
    if (!entity_decl) return nullptr;
    auto attrs = entity_decl->all_attributes();
    if (index >= attrs.size()) return nullptr;
    return alloc_cstr(attrs[index]->name());
}

char* ifcopenshell_entity_to_string(const ifcopenshell_ifc_instance_t* instance) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return nullptr;
    try {
        std::ostringstream oss;
        e->toString(oss);
        return alloc_cstr(oss.str());
    } catch (...) {
        return nullptr;
    }
}

/* --- Getters --- */

char* ifcopenshell_entity_get_string(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return nullptr;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        return alloc_cstr((std::string)val);
    } catch (...) {
        return nullptr;
    }
}

int64_t ifcopenshell_entity_get_int(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return 0;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return 0;
        return static_cast<int64_t>((int)val);
    } catch (...) {
        return 0;
    }
}

double ifcopenshell_entity_get_double(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return 0.0;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return 0.0;
        return (double)val;
    } catch (...) {
        return 0.0;
    }
}

int ifcopenshell_entity_get_bool(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return -1;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return -1;
        return (bool)val ? 1 : 0;
    } catch (...) {
        return -1;
    }
}

char* ifcopenshell_entity_get_enum(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return nullptr;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        auto ref = (EnumerationReference)val;
        return alloc_cstr(ref.value());
    } catch (...) {
        return nullptr;
    }
}

ifcopenshell_ifc_instance_t* ifcopenshell_entity_get_reference(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return 0;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return 0;
        auto* ref = (IfcUtil::IfcBaseClass*)val;
        return ifcopenshell::capi::wrap_instance(ref);
    } catch (...) {
        return 0;
    }
}

/* --- Setters --- */

void ifcopenshell_entity_set_string(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* val) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        e->set_attribute_value(idx, std::string(val ? val : ""));
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

void ifcopenshell_entity_set_int(ifcopenshell_ifc_instance_t* instance, const char* attr, int64_t val) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        e->set_attribute_value(idx, static_cast<int>(val));
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

void ifcopenshell_entity_set_double(ifcopenshell_ifc_instance_t* instance, const char* attr, double val) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        e->set_attribute_value(idx, val);
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

void ifcopenshell_entity_set_bool(ifcopenshell_ifc_instance_t* instance, const char* attr, int val) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        e->set_attribute_value(idx, static_cast<bool>(val));
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

bool ifcopenshell_entity_set_enum(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* val) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr || !val) return false;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        auto* enum_type = resolve_enum_type_for_attr(entity_decl, idx);
        if (!enum_type) return false;

        const auto& items = enum_type->enumeration_items();
        std::string val_str(val);
        auto it = std::find(items.begin(), items.end(), val_str);
        if (it == items.end()) return false;

        size_t enum_idx = static_cast<size_t>(std::distance(items.begin(), it));
        e->set_attribute_value(idx, EnumerationReference(enum_type, enum_idx));
        return true;
    } catch (...) {
        return false;
    }
}

void ifcopenshell_entity_set_reference(ifcopenshell_ifc_instance_t* instance, const char* attr, ifcopenshell_ifc_instance_t* ref) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* ref_e = ref ? ref->ptr : nullptr;
        if (!ref_e) {
            set_error("Referenced entity not found");
            return;
        }
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        e->set_attribute_value(idx, ref_e);
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

void ifcopenshell_entity_set_null(ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return;
    try {
        auto* entity_decl = e->declaration().as_entity();
        size_t idx = entity_decl->attribute_index(attr);
        // Match SWIG's setArgumentAsNull: route through the templated
        // set_attribute_value with Blank{} so the file's inverse index is
        // properly maintained. The low-level unset_attribute_value()
        // bypasses unregister_inverse and leaves stale inverses behind.
        e->set_attribute_value(idx, Blank{});
    } catch (const std::exception& ex) {
        set_error(ex.what());
    }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_create_type_instance(ifcopenshell_ifc_file_t* file_ptr, const char* type_name, const char* str_value) {
    // Simple types (IfcLabel, IfcReal, etc.) are inline values, not file entities.
    // They don't get STEP IDs. Use ifcopenshell_entity_set_typed_value instead.
    set_error("Use ifcopenshell_entity_set_typed_value for simple type values");
    return 0;
}

// Helper: create an IfcBaseClass* wrapper for a simple type value
static IfcUtil::IfcBaseClass* create_type_value(IfcParse::IfcFile* file, const char* type_name, const char* str_value) {
    auto* decl = file->schema()->declaration_by_name(type_name);
    auto* type_decl = decl->as_type_declaration();
    if (!type_decl) return nullptr;

    auto* inst = file->create(type_decl);
    if (!inst) return nullptr;

    auto* param_type = type_decl->declared_type();

    // Aggregation-typed defined types (e.g. IfcLineIndex = LIST OF
    // IfcPositiveInteger). The wrapped attribute is a vector, not a
    // simple scalar; parse the canonical SPF list literal "(a,b,c,...)"
    // emitted by the Python layer and dispatch to the right vector setter.
    auto* aggregation = param_type ? param_type->as_aggregation_type() : nullptr;
    if (aggregation && str_value) {
        std::string raw(str_value);
        // Strip surrounding whitespace and matching parentheses/brackets.
        size_t l = 0, r = raw.size();
        while (l < r && std::isspace(static_cast<unsigned char>(raw[l]))) ++l;
        while (r > l && std::isspace(static_cast<unsigned char>(raw[r - 1]))) --r;
        if (l < r && (raw[l] == '(' || raw[l] == '[') && (raw[r - 1] == ')' || raw[r - 1] == ']')) {
            ++l;
            --r;
        }
        std::vector<std::string> tokens;
        {
            std::string cur;
            for (size_t i = l; i < r; ++i) {
                char c = raw[i];
                if (c == ',') {
                    tokens.push_back(cur);
                    cur.clear();
                } else {
                    cur.push_back(c);
                }
            }
            if (!cur.empty() || !tokens.empty()) tokens.push_back(cur);
        }
        // Trim each token.
        for (auto& t : tokens) {
            size_t a = 0, b = t.size();
            while (a < b && std::isspace(static_cast<unsigned char>(t[a]))) ++a;
            while (b > a && std::isspace(static_cast<unsigned char>(t[b - 1]))) --b;
            t = t.substr(a, b - a);
        }
        // Determine element kind by walking the element parameter_type.
        const IfcParse::parameter_type* elem = aggregation->type_of_element();
        // unwrap named_type chain to a simple_type if possible.
        const IfcParse::simple_type* elem_simple = elem ? elem->as_simple_type() : nullptr;
        if (!elem_simple && elem) {
            const IfcParse::named_type* nt = elem->as_named_type();
            while (nt && !elem_simple) {
                auto* inner_decl = nt->declared_type();  // IfcParse::declaration*
                if (!inner_decl) break;
                auto* td = inner_decl->as_type_declaration();
                if (!td) break;
                auto* dt = td->declared_type();          // IfcParse::parameter_type*
                if (!dt) break;
                elem_simple = dt->as_simple_type();
                if (!elem_simple) nt = dt->as_named_type();
            }
        }
        IfcParse::simple_type::data_type kind = elem_simple
            ? elem_simple->declared_type()
            : IfcParse::simple_type::number_type;
        try {
            switch (kind) {
                case IfcParse::simple_type::integer_type: {
                    std::vector<int> v;
                    v.reserve(tokens.size());
                    for (auto& t : tokens) if (!t.empty()) v.push_back(std::stoi(t));
                    inst->set_attribute_value(0, v);
                    break;
                }
                case IfcParse::simple_type::real_type:
                case IfcParse::simple_type::number_type: {
                    std::vector<double> v;
                    v.reserve(tokens.size());
                    for (auto& t : tokens) if (!t.empty()) v.push_back(std::stod(t));
                    inst->set_attribute_value(0, v);
                    break;
                }
                case IfcParse::simple_type::boolean_type:
                case IfcParse::simple_type::logical_type:
                    // List-of-bool/logical typed defined types are exceedingly
                    // rare in published IFC schemas and the parser exposes
                    // them as ``vector<dynamic_bitset>``, not ``vector<bool>``.
                    // Fall back to string storage if encountered.
                    inst->set_attribute_value(0, std::string(str_value));
                    break;
                default: {
                    std::vector<std::string> v;
                    v.reserve(tokens.size());
                    for (auto& t : tokens) {
                        // Strip surrounding single quotes if present.
                        if (t.size() >= 2 && t.front() == '\'' && t.back() == '\'') {
                            v.push_back(t.substr(1, t.size() - 2));
                        } else {
                            v.push_back(t);
                        }
                    }
                    inst->set_attribute_value(0, v);
                    break;
                }
            }
        } catch (const std::exception& e) {
            set_error(std::string("Failed to parse aggregate literal for ") + type_name + ": " + e.what());
            return nullptr;
        }
        return inst;
    }

    // Walk named_type chain to find the underlying simple_type
    auto* simple = param_type ? param_type->as_simple_type() : nullptr;
    if (!simple) {
        auto* named = param_type ? param_type->as_named_type() : nullptr;
        while (named && !simple) {
            auto* inner = named->declared_type();
            simple = inner ? inner->as_type_declaration() ?
                (inner->as_type_declaration()->declared_type() ? inner->as_type_declaration()->declared_type()->as_simple_type() : nullptr)
                : nullptr : nullptr;
            if (!simple) {
                named = inner && inner->as_type_declaration() && inner->as_type_declaration()->declared_type()
                    ? inner->as_type_declaration()->declared_type()->as_named_type() : nullptr;
            }
        }
    }

    if (simple && str_value) {
        switch (simple->declared_type()) {
            case IfcParse::simple_type::string_type:
                inst->set_attribute_value(0, std::string(str_value));
                break;
            case IfcParse::simple_type::real_type:
            case IfcParse::simple_type::number_type:
                inst->set_attribute_value(0, std::stod(str_value));
                break;
            case IfcParse::simple_type::integer_type:
                inst->set_attribute_value(0, static_cast<int>(std::stoi(str_value)));
                break;
            case IfcParse::simple_type::boolean_type:
            case IfcParse::simple_type::logical_type: {
                std::string s(str_value);
                bool val = (s == "true" || s == "True" || s == "TRUE" || s == "1" || s == ".T.");
                inst->set_attribute_value(0, val);
                break;
            }
            default:
                inst->set_attribute_value(0, std::string(str_value));
                break;
        }
    } else if (str_value) {
        // Fallback: treat as string
        inst->set_attribute_value(0, std::string(str_value));
    }
    return inst;
}

bool ifcopenshell_entity_set_typed_value(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* type_name, const char* str_value)
{
    ifcopenshell_clear_error();
    if (!instance || !attr || !type_name) return false;
    try {
        auto* entity = instance->ptr;
        if (!entity) return false;
        auto* file = entity->file_;
        if (!file) return false;

        auto* type_inst = create_type_value(file, type_name, str_value);
        if (!type_inst) {
            set_error("Failed to create type instance");
            return false;
        }

        auto* entity_decl = entity->declaration().as_entity();
        if (!entity_decl) return false;
        size_t idx = entity_decl->attribute_index(attr);
        entity->set_attribute_value(idx, type_inst);
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

const char* ifcopenshell_entity_get_typed_value(const ifcopenshell_ifc_instance_t* instance, const char* attr, char** value_out)
{
    if (!instance || !attr) return nullptr;
    try {
        auto* entity = instance->ptr;
        if (!entity) return nullptr;

        auto* entity_decl = entity->declaration().as_entity();
        if (!entity_decl) return nullptr;
        size_t idx = entity_decl->attribute_index(attr);
        auto val = entity->get_attribute_value(idx);
        if (val.isNull()) return nullptr;

        // Check if value is a reference to a type instance (id 0 or type declaration)
        if (val.type() != IfcUtil::Argument_ENTITY_INSTANCE) return nullptr;
        auto* ref = (IfcUtil::IfcBaseClass*)val;
        if (!ref) return nullptr;

        auto* type_decl = ref->declaration().as_type_declaration();
        if (!type_decl) return nullptr;

        // Read the wrapped value (attribute 0 of the type instance)
        if (value_out) {
            try {
                auto inner = ref->get_attribute_value(0);
                if (!inner.isNull()) {
                    std::string str_val;
                    switch (inner.type()) {
                        case IfcUtil::Argument_STRING:
                            str_val = (std::string)inner;
                            break;
                        case IfcUtil::Argument_DOUBLE:
                            str_val = std::to_string((double)inner);
                            break;
                        case IfcUtil::Argument_INT:
                            str_val = std::to_string((int)inner);
                            break;
                        case IfcUtil::Argument_BOOL:
                            str_val = (bool)inner ? "true" : "false";
                            break;
                        case IfcUtil::Argument_AGGREGATE_OF_INT: {
                            auto vec = (std::vector<int>)inner;
                            str_val = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) str_val += ",";
                                str_val += std::to_string(vec[k]);
                            }
                            str_val += ")";
                            break;
                        }
                        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: {
                            auto vec = (std::vector<double>)inner;
                            str_val = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) str_val += ",";
                                str_val += std::to_string(vec[k]);
                            }
                            str_val += ")";
                            break;
                        }
                        case IfcUtil::Argument_AGGREGATE_OF_STRING: {
                            auto vec = (std::vector<std::string>)inner;
                            str_val = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) str_val += ",";
                                str_val += "'" + vec[k] + "'";
                            }
                            str_val += ")";
                            break;
                        }
                        default:
                            str_val = "?";
                            break;
                    }
                    *value_out = strdup(str_val.c_str());
                } else {
                    *value_out = nullptr;
                }
            } catch (...) {
                *value_out = nullptr;
            }
        }
        // Return the type name (static pointer from schema)
        return type_decl->name().c_str();
    } catch (...) {
        return nullptr;
    }
}

/* --- Aggregate helpers --- */

static size_t resolve_attr_idx(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* entity_decl = e->declaration().as_entity();
    return entity_decl->attribute_index(attr);
}

int32_t ifcopenshell_entity_get_aggregate_size(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return -1;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return -1;
        auto t = val.type();
        if (t == IfcUtil::Argument_AGGREGATE_OF_INT ||
            t == IfcUtil::Argument_AGGREGATE_OF_DOUBLE ||
            t == IfcUtil::Argument_AGGREGATE_OF_STRING ||
            t == IfcUtil::Argument_AGGREGATE_OF_BINARY ||
            t == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            auto agg = (aggregate_of_instance::ptr)val;
            if (agg) return static_cast<int32_t>(agg->size());
            // For non-entity aggregates, try vector casts
            try { auto v = (std::vector<int>)val; return static_cast<int32_t>(v.size()); } catch (...) {}
            try { auto v = (std::vector<double>)val; return static_cast<int32_t>(v.size()); } catch (...) {}
            try { auto v = (std::vector<std::string>)val; return static_cast<int32_t>(v.size()); } catch (...) {}
        }
        // Try entity aggregate directly
        try {
            auto agg = (aggregate_of_instance::ptr)val;
            if (agg) return static_cast<int32_t>(agg->size());
        } catch (...) {}
        return -1;
    } catch (...) {
        return -1;
    }
}

int64_t* ifcopenshell_entity_get_aggregate_int(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count) {
    if (count) *count = 0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr || !count) return nullptr;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        auto vec = (std::vector<int>)val;
        if (vec.empty()) return nullptr;
        auto* out = static_cast<int64_t*>(std::malloc(vec.size() * sizeof(int64_t)));
        if (!out) return nullptr;
        for (size_t i = 0; i < vec.size(); ++i) out[i] = vec[i];
        *count = static_cast<uint32_t>(vec.size());
        return out;
    } catch (...) {
        return nullptr;
    }
}

double* ifcopenshell_entity_get_aggregate_double(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count) {
    if (count) *count = 0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr || !count) return nullptr;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        auto vec = (std::vector<double>)val;
        if (vec.empty()) return nullptr;
        auto* out = static_cast<double*>(std::malloc(vec.size() * sizeof(double)));
        if (!out) return nullptr;
        std::memcpy(out, vec.data(), vec.size() * sizeof(double));
        *count = static_cast<uint32_t>(vec.size());
        return out;
    } catch (...) {
        return nullptr;
    }
}

char** ifcopenshell_entity_get_aggregate_string(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count) {
    if (count) *count = 0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr || !count) return nullptr;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        auto vec = (std::vector<std::string>)val;
        if (vec.empty()) return nullptr;
        auto** out = static_cast<char**>(std::malloc(vec.size() * sizeof(char*)));
        if (!out) return nullptr;
        for (size_t i = 0; i < vec.size(); ++i) {
            out[i] = alloc_cstr(vec[i]);
        }
        *count = static_cast<uint32_t>(vec.size());
        return out;
    } catch (...) {
        return nullptr;
    }
}

ifcopenshell_ifc_instance_t** ifcopenshell_entity_get_aggregate_ref(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count) {
    if (count) *count = 0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr || !count) return nullptr;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto val = e->get_attribute_value(idx);
        if (val.isNull()) return nullptr;
        auto agg = (aggregate_of_instance::ptr)val;
        if (!agg || agg->size() == 0) return nullptr;
        auto* out = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(agg->size() * sizeof(ifcopenshell_ifc_instance_t*)));
        if (!out) return nullptr;
        uint32_t i = 0;
        for (auto it = agg->begin(); it != agg->end(); ++it) {
            out[i++] = ifcopenshell::capi::wrap_instance(*it);
        }
        *count = i;
        return out;
    } catch (...) {
        return nullptr;
    }
}

int32_t ifcopenshell_entity_get_aggregate_typed_value(const ifcopenshell_ifc_instance_t* instance, const char* attr,
    char*** type_names_out, char*** values_out)
{
    if (!instance || !attr || !type_names_out || !values_out) return -1;
    *type_names_out = nullptr;
    *values_out = nullptr;
    try {
        auto* entity = instance->ptr;
        if (!entity) return -1;

        size_t idx = resolve_attr_idx(entity, attr);
        auto val = entity->get_attribute_value(idx);
        if (val.isNull()) return -1;
        if (val.type() != IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) return -1;

        auto agg = (aggregate_of_instance::ptr)val;
        if (!agg || agg->size() == 0) return 0;

        uint32_t n = static_cast<uint32_t>(agg->size());
        auto* names = static_cast<char**>(std::malloc(n * sizeof(char*)));
        auto* vals = static_cast<char**>(std::malloc(n * sizeof(char*)));
        if (!names || !vals) { std::free(names); std::free(vals); return -1; }

        uint32_t i = 0;
        for (auto it = agg->begin(); it != agg->end(); ++it, ++i) {
            auto* inst = *it;
            auto* type_decl = inst->declaration().as_type_declaration();
            if (!type_decl) {
                // Not a typed value — clean up and fail
                for (uint32_t j = 0; j < i; ++j) { std::free(names[j]); std::free(vals[j]); }
                std::free(names); std::free(vals);
                return -1;
            }
            names[i] = strdup(type_decl->name().c_str());

            // Read inner value (attribute 0)
            try {
                auto inner = inst->get_attribute_value(0);
                std::string str_val;
                if (!inner.isNull()) {
                    switch (inner.type()) {
                        case IfcUtil::Argument_STRING: str_val = (std::string)inner; break;
                        case IfcUtil::Argument_DOUBLE: str_val = std::to_string((double)inner); break;
                        case IfcUtil::Argument_INT: str_val = std::to_string((int)inner); break;
                        case IfcUtil::Argument_BOOL: str_val = (bool)inner ? "true" : "false"; break;
                        case IfcUtil::Argument_AGGREGATE_OF_INT: {
                            auto vec = (std::vector<int>)inner;
                            std::string s = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) s += ",";
                                s += std::to_string(vec[k]);
                            }
                            s += ")";
                            str_val = s;
                            break;
                        }
                        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: {
                            auto vec = (std::vector<double>)inner;
                            std::string s = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) s += ",";
                                s += std::to_string(vec[k]);
                            }
                            s += ")";
                            str_val = s;
                            break;
                        }
                        case IfcUtil::Argument_AGGREGATE_OF_STRING: {
                            auto vec = (std::vector<std::string>)inner;
                            std::string s = "(";
                            for (size_t k = 0; k < vec.size(); ++k) {
                                if (k) s += ",";
                                s += "'" + vec[k] + "'";
                            }
                            s += ")";
                            str_val = s;
                            break;
                        }
                        default: str_val = ""; break;
                    }
                }
                vals[i] = strdup(str_val.c_str());
            } catch (...) {
                vals[i] = strdup("");
            }
        }
        *type_names_out = names;
        *values_out = vals;
        return static_cast<int32_t>(n);
    } catch (...) {
        return -1;
    }
}

/* --- Aggregate setters --- */

bool ifcopenshell_entity_set_aggregate_int(ifcopenshell_ifc_instance_t* instance, const char* attr, const int64_t* vals, uint32_t count) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return false;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        std::vector<int> vec(count);
        for (uint32_t i = 0; i < count; ++i) vec[i] = static_cast<int>(vals[i]);
        e->set_attribute_value(idx, vec);
        return true;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return false;
    }
}

bool ifcopenshell_entity_set_aggregate_double(ifcopenshell_ifc_instance_t* instance, const char* attr, const double* vals, uint32_t count) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return false;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        std::vector<double> vec(vals, vals + count);
        e->set_attribute_value(idx, vec);
        return true;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return false;
    }
}

bool ifcopenshell_entity_set_aggregate_string(ifcopenshell_ifc_instance_t* instance, const char* attr, const char** vals, uint32_t count) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return false;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        std::vector<std::string> vec(count);
        for (uint32_t i = 0; i < count; ++i) vec[i] = vals[i] ? vals[i] : "";
        e->set_attribute_value(idx, vec);
        return true;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return false;
    }
}

bool ifcopenshell_entity_set_aggregate_ref(ifcopenshell_ifc_instance_t* instance, const char* attr, ifcopenshell_ifc_instance_t** refs, uint32_t count) {
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !attr) return false;
    try {
        size_t idx = resolve_attr_idx(e, attr);
        auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
        for (uint32_t i = 0; i < count; ++i) {
            auto* ref = (refs[i] ? refs[i]->ptr : nullptr);
            if (!ref) {
                set_error("Referenced entity not found");
                return false;
            }
            agg->push(ref);
        }
        e->set_attribute_value(idx, agg);
        return true;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return false;
    }
}

bool ifcopenshell_entity_set_aggregate_typed_value(ifcopenshell_ifc_instance_t* instance, const char* attr,
    const char** type_names, const char** str_values, uint32_t count)
{
    ifcopenshell_clear_error();
    if (!instance || !attr) return false;
    try {
        auto* entity = instance->ptr;
        if (!entity) return false;
        auto* file = entity->file_;
        if (!file) return false;

        auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
        for (uint32_t i = 0; i < count; ++i) {
            auto* inst = create_type_value(file, type_names[i], str_values[i]);
            if (!inst) {
                set_error("Failed to create typed value in aggregate");
                return false;
            }
            agg->push(inst);
        }
        size_t idx = resolve_attr_idx(entity, attr);
        entity->set_attribute_value(idx, agg);
        return true;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return false;
    }
}

/* --- Aggregate memory management --- */

void ifcopenshell_free_int_array(int64_t* arr) { std::free(arr); }
void ifcopenshell_free_double_array(double* arr) { std::free(arr); }
void ifcopenshell_free_string_array(char** arr, uint32_t count) {
    if (!arr) return;
    for (uint32_t i = 0; i < count; ++i) std::free(arr[i]);
    std::free(arr);
}

/* --- Inverse attribute access --- */

bool ifcopenshell_entity_is_inverse_attr(const ifcopenshell_ifc_instance_t* instance, const char* attr) {
    auto* entity = instance ? instance->ptr : nullptr;
    if (!entity || !attr) return false;
    try {
        auto* entity_decl = entity->declaration().as_entity();
        if (!entity_decl) return false;
        const std::string name(attr);
        for (auto* inv : entity_decl->all_inverse_attributes()) {
            if (inv->name() == name) return true;
        }
    } catch (...) {}
    return false;
}

ifcopenshell_ifc_instance_t** ifcopenshell_entity_get_inverse_attr(ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count)
{
    if (count) *count = 0;
    if (!instance || !attr || !count) return nullptr;
    try {
        auto* base = instance->ptr;
        if (!base) return nullptr;
        auto* entity = dynamic_cast<IfcUtil::IfcBaseEntity*>(base);
        if (!entity) return nullptr;
        auto result = entity->get_inverse(attr);
        if (!result || result->size() == 0) return nullptr;
        auto n = static_cast<uint32_t>(result->size());
        auto* out = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(n * sizeof(ifcopenshell_ifc_instance_t*)));
        if (!out) return nullptr;
        for (uint32_t i = 0; i < n; ++i) {
            out[i] = ifcopenshell::capi::wrap_instance((*result)[i]);
        }
        *count = n;
        return out;
    } catch (...) {
        return nullptr;
    }
}

/* --- Utility: deep removal --- */

void ifcopenshell_util_remove_deep2(ifcopenshell_ifc_instance_t* instance) {
    if (!instance || !instance->ptr) return;
    auto* entity = instance->ptr;
    auto* file = entity->file_;
    if (!file) return;
    try {
        // Collect all entities referenced by this entity (traverse 1 level).
        std::vector<IfcUtil::IfcBaseClass*> referenced;
        auto traversed = file->traverse(entity, 1);
        if (traversed) {
            for (auto& ref : *traversed) {
                if (ref != entity) {
                    referenced.push_back(ref);
                }
            }
        }

        // Remove the entity itself.
        file->removeEntity(entity);

        // Recursively remove referenced entities that are now orphaned.
        for (auto* ref : referenced) {
            try {
                auto* check = file->instance_by_id(ref->id());
                if (!check) continue;
                auto inverses = file->getInverse(ref->id(), nullptr, -1);
                if (!inverses || inverses->size() == 0) {
                    auto* h = ifcopenshell::capi::wrap_instance(ref);
                    ifcopenshell_util_remove_deep2(h);
                    ifcopenshell_ifc_instance_destroy(h);
                }
            } catch (...) {
                // Entity was already removed or inaccessible — skip.
            }
        }
    } catch (...) {}
}

} // extern "C"
