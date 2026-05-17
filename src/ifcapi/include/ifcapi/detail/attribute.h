// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ATTRIBUTE_H
#define IFCAPI_DETAIL_ATTRIBUTE_H

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace ifcapi {
namespace detail {

struct OptionalString {
    bool has_value = false;
    std::string value;
};

inline int find_attr_index(const IfcParse::entity* decl, const char* name) {
    if (!decl) {
        return -1;
    }
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline int attr_index_of(IfcUtil::IfcBaseClass* entity, const char* attr) {
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) {
        return -1;
    }
    return find_attr_index(base->declaration().as_entity(), attr);
}

inline bool entity_has_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    return attr_index_of(entity, attr) >= 0;
}

inline bool is_attr_null(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return true;
    }
    try {
        return entity->get_attribute_value(static_cast<size_t>(idx)).isNull();
    } catch (...) {
        return true;
    }
}

inline std::string read_string_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return std::string();
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return std::string();
        }
        return static_cast<std::string>(val);
    } catch (...) {
        return std::string();
    }
}

inline OptionalString read_optional_string_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    OptionalString result;
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return result;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (!val.isNull()) {
            result.has_value = true;
            result.value = static_cast<std::string>(val);
        }
    } catch (...) {
    }
    return result;
}

inline bool optional_string_attr_equals(
    IfcUtil::IfcBaseClass* entity,
    const char* attr,
    const OptionalString& expected)
{
    auto actual = read_optional_string_attr(entity, attr);
    if (actual.has_value != expected.has_value) {
        return false;
    }
    return !expected.has_value || actual.value == expected.value;
}

inline IfcUtil::IfcBaseClass* read_ref_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return nullptr;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return nullptr;
        }
        return static_cast<IfcUtil::IfcBaseClass*>(val);
    } catch (...) {
        return nullptr;
    }
}

inline std::vector<IfcUtil::IfcBaseClass*> get_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!entity || attr_idx < 0) {
        return result;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(attr_idx));
        if (val.isNull()) {
            return result;
        }
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& item : *agg) {
                result.push_back(item);
            }
        }
    } catch (...) {
    }
    return result;
}

inline std::vector<IfcUtil::IfcBaseClass*> read_ref_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr) {
    return get_ref_aggregate(entity, attr_index_of(entity, attr));
}

inline std::vector<IfcUtil::IfcBaseClass*> read_inverse_aggregate(
    IfcUtil::IfcBaseClass* entity,
    const char* attr)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) {
        return result;
    }
    try {
        auto values = base->get_inverse(attr);
        if (!values) {
            return result;
        }
        for (size_t i = 0; i < values->size(); ++i) {
            if ((*values)[i]) {
                result.push_back((*values)[i]);
            }
        }
    } catch (...) {
    }
    return result;
}

inline void set_ref_aggregate(
    IfcUtil::IfcBaseClass* entity,
    int attr_idx,
    const std::vector<IfcUtil::IfcBaseClass*>& refs)
{
    if (!entity || attr_idx < 0) {
        return;
    }
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* ref : refs) {
        agg->push(ref);
    }
    entity->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

inline bool write_ref_aggregate(
    IfcUtil::IfcBaseClass* entity,
    const char* attr,
    const std::vector<IfcUtil::IfcBaseClass*>& refs)
{
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    set_ref_aggregate(entity, idx, refs);
    return true;
}

inline void write_string_attr(IfcUtil::IfcBaseClass* entity, const char* attr, const std::string& value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

inline void copy_string_attr_preserving_null(
    IfcUtil::IfcBaseClass* target,
    const char* target_attr,
    IfcUtil::IfcBaseClass* source,
    const char* source_attr)
{
    int idx = attr_index_of(target, target_attr);
    if (idx < 0) {
        return;
    }
    if (is_attr_null(source, source_attr)) {
        target->set_attribute_value(static_cast<size_t>(idx), Blank{});
        return;
    }
    target->set_attribute_value(static_cast<size_t>(idx), read_string_attr(source, source_attr));
}

inline void write_optional_string_attr(
    IfcUtil::IfcBaseClass* entity,
    const char* attr,
    const OptionalString& value)
{
    if (value.has_value) {
        write_string_attr(entity, attr, value.value);
    }
}

inline void write_int_attr(IfcUtil::IfcBaseClass* entity, const char* attr, int value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

inline int read_int_attr(IfcUtil::IfcBaseClass* entity, const char* attr, int fallback = 0) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return fallback;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return fallback;
        }
        return static_cast<int>(val);
    } catch (...) {
        return fallback;
    }
}

inline void write_double_attr(IfcUtil::IfcBaseClass* entity, const char* attr, double value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

inline std::vector<double> read_double_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr) {
    std::vector<double> result;
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return result;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return result;
        }
        return static_cast<std::vector<double>>(val);
    } catch (...) {
    }
    return result;
}

inline void write_double_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr, const std::vector<double>& values) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), values);
    }
}

inline void write_double_aggregate_aggregate(
    IfcUtil::IfcBaseClass* entity,
    const char* attr,
    const std::vector<std::vector<double>>& values)
{
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), values);
    }
}

inline const IfcParse::enumeration_type* resolve_enum_type(IfcUtil::IfcBaseClass* entity, int attr_idx) {
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base || attr_idx < 0) {
        return nullptr;
    }
    auto* entity_decl = base->declaration().as_entity();
    if (!entity_decl) {
        return nullptr;
    }
    auto attrs = entity_decl->all_attributes();
    if (static_cast<size_t>(attr_idx) >= attrs.size()) {
        return nullptr;
    }
    const IfcParse::parameter_type* pt = attrs[static_cast<size_t>(attr_idx)]->type_of_attribute();
    while (pt) {
        auto* named = pt->as_named_type();
        if (!named) {
            break;
        }
        auto* decl = named->declared_type();
        if (auto* enum_type = decl->as_enumeration_type()) {
            return enum_type;
        }
        if (auto* type_decl = decl->as_type_declaration()) {
            pt = type_decl->declared_type();
        } else {
            break;
        }
    }
    return nullptr;
}

inline bool write_enum_attr(IfcUtil::IfcBaseClass* entity, const char* attr, const std::string& value) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    auto* enum_type = resolve_enum_type(entity, idx);
    if (!enum_type) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
        return true;
    }
    const auto& items = enum_type->enumeration_items();
    auto it = std::find(items.begin(), items.end(), value);
    if (it == items.end()) {
        return false;
    }
    entity->set_attribute_value(
        static_cast<size_t>(idx),
        EnumerationReference(enum_type, static_cast<size_t>(std::distance(items.begin(), it))));
    return true;
}

inline void write_blank_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

inline void set_ref(IfcUtil::IfcBaseClass* entity, int attr_idx, IfcUtil::IfcBaseClass* ref) {
    if (entity && attr_idx >= 0 && ref) {
        entity->set_attribute_value(static_cast<size_t>(attr_idx), ref);
    }
}

inline void write_ref_attr(IfcUtil::IfcBaseClass* entity, const char* attr, IfcUtil::IfcBaseClass* ref) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return;
    }
    if (ref) {
        entity->set_attribute_value(static_cast<size_t>(idx), ref);
    } else {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

inline bool exists_in_file(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || !entity->id()) return false;
    try {
        return file->instance_by_id(entity->id()) == entity;
    } catch (const IfcParse::IfcException&) {
        return false;
    }
}

inline bool contains_ref(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

inline void append_unique(std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    if (value && !contains_ref(values, value)) {
        values.push_back(value);
    }
}

inline void append_unique(
    std::vector<IfcUtil::IfcBaseClass*>& values,
    const std::vector<IfcUtil::IfcBaseClass*>& additional)
{
    for (auto* value : additional) {
        append_unique(values, value);
    }
}

inline std::vector<const IfcUtil::IfcBaseClass*> to_const_refs(
    const std::vector<IfcUtil::IfcBaseClass*>& values)
{
    std::vector<const IfcUtil::IfcBaseClass*> result;
    result.reserve(values.size());
    for (auto* value : values) {
        if (value) {
            result.push_back(value);
        }
    }
    return result;
}

inline std::vector<IfcUtil::IfcBaseClass*> to_mutable_refs(
    const std::vector<const IfcUtil::IfcBaseClass*>& values)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(values.size());
    for (auto* value : values) {
        if (value) {
            result.push_back(const_cast<IfcUtil::IfcBaseClass*>(value));
        }
    }
    return result;
}

inline std::set<IfcUtil::IfcBaseClass*> to_ref_set(const std::vector<IfcUtil::IfcBaseClass*>& values) {
    return std::set<IfcUtil::IfcBaseClass*>(values.begin(), values.end());
}

inline std::vector<IfcUtil::IfcBaseClass*> to_ref_vector(const std::set<IfcUtil::IfcBaseClass*>& values) {
    return std::vector<IfcUtil::IfcBaseClass*>(values.begin(), values.end());
}

inline std::vector<IfcUtil::IfcBaseClass*> instances_by_type(IfcParse::IfcFile* file, const char* ifc_class) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !file->schema()) {
        return result;
    }
    const auto* declaration = file->schema()->declaration_by_name(ifc_class);
    if (!declaration) {
        return result;
    }
    auto instances = file->instances_by_type(declaration);
    if (!instances) {
        return result;
    }
    for (auto* instance : *instances) {
        if (instance) {
            result.push_back(instance);
        }
    }
    return result;
}

inline IfcUtil::IfcBaseClass* first_instance_by_type(IfcParse::IfcFile* file, const char* ifc_class) {
    auto instances = instances_by_type(file, ifc_class);
    return instances.empty() ? nullptr : instances.front();
}

inline IfcUtil::IfcBaseClass* create_typed_double(
    IfcParse::IfcFile* file,
    const char* ifc_type,
    double value)
{
    if (!file || !file->schema()) {
        return nullptr;
    }
    auto* declaration = file->schema()->declaration_by_name(ifc_type);
    auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    if (!type_declaration) {
        return nullptr;
    }
    auto* instance = file->create(type_declaration);
    if (instance) {
        instance->set_attribute_value(0, value);
    }
    return instance;
}

inline size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || entity->id() <= 0) {
        return 0;
    }
    try {
        auto inverses = file->getInverse(entity->id(), nullptr, -1);
        return inverses ? inverses->size() : 0;
    } catch (...) {
        return 0;
    }
}

inline void replace_attribute_reference(
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* old_reference,
    IfcUtil::IfcBaseClass* new_reference)
{
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(element);
    auto* declaration = base ? base->declaration().as_entity() : nullptr;
    if (!element || !old_reference || !new_reference || !declaration) {
        return;
    }
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = element->get_attribute_value(i);
            if (value.isNull()) {
                continue;
            }
            if (value.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
                if (static_cast<IfcUtil::IfcBaseClass*>(value) == old_reference) {
                    element->set_attribute_value(i, new_reference);
                }
            } else if (value.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto aggregate = (aggregate_of_instance::ptr)value;
                if (!aggregate) {
                    continue;
                }
                bool changed = false;
                auto replacement = aggregate_of_instance::ptr(new aggregate_of_instance());
                for (auto& item : *aggregate) {
                    if (item == old_reference) {
                        replacement->push(new_reference);
                        changed = true;
                    } else {
                        replacement->push(item);
                    }
                }
                if (changed) {
                    element->set_attribute_value(i, replacement);
                }
            }
        } catch (...) {
        }
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_ATTRIBUTE_H */
