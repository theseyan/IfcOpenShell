// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/entity.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"
#include "ifcparse/utils.h"

#include <cctype>
#include <cstdlib>
#include <iterator>
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

static std::string typed_value_to_string(const AttributeValue& inner) {
    if (inner.isNull()) return "";
    switch (inner.type()) {
        case IfcUtil::Argument_STRING:
            return static_cast<std::string>(inner);
        case IfcUtil::Argument_DOUBLE:
            return std::to_string(static_cast<double>(inner));
        case IfcUtil::Argument_INT:
            return std::to_string(static_cast<int>(inner));
        case IfcUtil::Argument_BOOL:
            return static_cast<bool>(inner) ? "true" : "false";
        case IfcUtil::Argument_AGGREGATE_OF_INT: {
            auto vec = static_cast<std::vector<int>>(inner);
            std::string result = "(";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i) result += ",";
                result += std::to_string(vec[i]);
            }
            result += ")";
            return result;
        }
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: {
            auto vec = static_cast<std::vector<double>>(inner);
            std::string result = "(";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i) result += ",";
                result += std::to_string(vec[i]);
            }
            result += ")";
            return result;
        }
        case IfcUtil::Argument_AGGREGATE_OF_STRING: {
            auto vec = static_cast<std::vector<std::string>>(inner);
            std::string result = "(";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i) result += ",";
                result += "'" + vec[i] + "'";
            }
            result += ")";
            return result;
        }
        default:
            return "?";
    }
}

namespace ifcapi {
namespace bindings {

bool entity_set_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr,
    const std::string& type_name,
    const char* str_value) {
    ifcopenshell_clear_error();
    if (!instance || attr.empty() || type_name.empty()) return false;
    try {
        auto* file = instance->file_;
        if (!file) return false;
        auto* type_inst = create_type_value(file, type_name.c_str(), str_value);
        if (!type_inst) {
            set_error("Failed to create type instance");
            return false;
        }
        auto* entity_decl = instance->declaration().as_entity();
        if (!entity_decl) return false;
        instance->set_attribute_value(entity_decl->attribute_index(attr), type_inst);
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

std::vector<std::string> entity_get_typed_value(IfcUtil::IfcBaseClass* instance, const std::string& attr) {
    if (!instance || attr.empty()) return {};
    try {
        auto* entity_decl = instance->declaration().as_entity();
        if (!entity_decl) return {};
        auto val = instance->get_attribute_value(entity_decl->attribute_index(attr));
        if (val.isNull() || val.type() != IfcUtil::Argument_ENTITY_INSTANCE) return {};
        auto* ref = static_cast<IfcUtil::IfcBaseClass*>(val);
        if (!ref) return {};
        auto* type_decl = ref->declaration().as_type_declaration();
        if (!type_decl) return {};
        return {type_decl->name(), typed_value_to_string(ref->get_attribute_value(0))};
    } catch (...) {
        return {};
    }
}

bool entity_set_aggregate_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr,
    const std::vector<std::string>& type_names,
    const std::vector<std::string>& str_values) {
    ifcopenshell_clear_error();
    if (!instance || attr.empty() || type_names.size() != str_values.size()) return false;
    try {
        auto* file = instance->file_;
        if (!file) return false;
        auto aggregate = aggregate_of_instance::ptr(new aggregate_of_instance());
        for (size_t i = 0; i < type_names.size(); ++i) {
            auto* typed_value = create_type_value(file, type_names[i].c_str(), str_values[i].c_str());
            if (!typed_value) {
                set_error("Failed to create typed value in aggregate");
                return false;
            }
            aggregate->push(typed_value);
        }
        auto* entity_decl = instance->declaration().as_entity();
        if (!entity_decl) return false;
        instance->set_attribute_value(entity_decl->attribute_index(attr), aggregate);
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

std::vector<std::string> entity_get_aggregate_typed_value(IfcUtil::IfcBaseClass* instance, const std::string& attr) {
    if (!instance || attr.empty()) return {};
    try {
        auto* entity_decl = instance->declaration().as_entity();
        if (!entity_decl) return {};
        auto val = instance->get_attribute_value(entity_decl->attribute_index(attr));
        if (val.isNull()) return {};
        auto aggregate = static_cast<aggregate_of_instance::ptr>(val);
        if (!aggregate) return {};
        std::vector<std::string> result;
        result.reserve(aggregate->size() * 2);
        for (auto it = aggregate->begin(); it != aggregate->end(); ++it) {
            auto* typed_value = *it;
            auto* type_decl = typed_value ? typed_value->declaration().as_type_declaration() : nullptr;
            if (!type_decl) continue;
            result.push_back(type_decl->name());
            result.push_back(typed_value_to_string(typed_value->get_attribute_value(0)));
        }
        return result;
    } catch (...) {
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi

extern "C" {

/* --- Aggregate memory management --- */

void ifcopenshell_free_string_array(char** arr, uint32_t count) {
    if (!arr) return;
    for (uint32_t i = 0; i < count; ++i) std::free(arr[i]);
    std::free(arr);
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
