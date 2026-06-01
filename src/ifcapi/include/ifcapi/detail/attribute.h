// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ATTRIBUTE_H
#define IFCAPI_DETAIL_ATTRIBUTE_H

#include "ifcparse/file.h"
#include "ifcparse/instance_data.h"

#include <algorithm>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace ifcapi {
namespace detail {

inline express::Base deref_or_empty(express::Base* value) {
    return value ? *value : express::Base();
}

struct OptionalString {
    bool has_value = false;
    std::string value;
};

class entity_view {
    mutable express::Entity entity_;

  public:
    entity_view() = default;
    entity_view(const express::Base& entity) : entity_(entity.as<express::Entity>()) {}
    entity_view(const express::Entity& entity) : entity_(entity) {}

    explicit operator bool() const { return static_cast<bool>(entity_); }
    const express::Entity& get() const { return entity_; }
    express::Entity& get() { return entity_; }

    ifcopenshell::file* file() const { return entity_ ? entity_.file() : nullptr; }
    uint32_t id() const { return entity_ ? entity_.id() : 0; }

    const ifcopenshell::entity* declaration() const {
        return entity_ ? entity_.declaration().as_entity() : nullptr;
    }

    bool is(const std::string& ifc_class) const {
        return entity_ && entity_.declaration().is(ifc_class);
    }

    int attr_index(const char* attr) const {
        auto* decl = declaration();
        if (!decl) {
            return -1;
        }
        ptrdiff_t idx = decl->attribute_index(attr);
        return idx < 0 ? -1 : static_cast<int>(idx);
    }

    bool has_attr(const char* attr) const { return attr_index(attr) >= 0; }

    attribute_value attr(size_t idx) const { return entity_.get_attribute_value(idx); }

    attribute_value attr(const char* name) const {
        int idx = attr_index(name);
        if (idx < 0) {
            return attribute_value();
        }
        return attr(static_cast<size_t>(idx));
    }

    bool is_null(const char* name) const {
        int idx = attr_index(name);
        return idx < 0 || attr(static_cast<size_t>(idx)).isNull();
    }

    template <typename T>
    std::optional<T> get_optional(const char* name) const {
        int idx = attr_index(name);
        if (idx < 0) {
            return std::nullopt;
        }
        auto value = attr(static_cast<size_t>(idx));
        if (value.isNull()) {
            return std::nullopt;
        }
        return static_cast<T>(value);
    }

    template <typename T>
    T get_or(const char* name, const T& fallback) const {
        auto value = get_optional<T>(name);
        return value ? *value : fallback;
    }

    template <typename T>
    void set(size_t idx, const T& value) const {
        entity_.set_attribute_value(idx, value);
    }

    template <typename T>
    bool set(const char* name, const T& value) const {
        int idx = attr_index(name);
        if (idx < 0) {
            return false;
        }
        set(static_cast<size_t>(idx), value);
        return true;
    }

    bool unset(const char* name) const {
        int idx = attr_index(name);
        if (idx < 0) {
            return false;
        }
        entity_.set_attribute_value(static_cast<size_t>(idx), blank{});
        return true;
    }

    std::vector<express::Entity> inverse(const char* name) const {
        if (!entity_) {
            return {};
        }
        const auto* decl = declaration();
        if (!decl) {
            return {};
        }
        for (const auto* attr : decl->all_inverse_attributes()) {
            if (attr->name() == name) {
                return entity_.get_inverse(name);
            }
        }
        return {};
    }
};

inline const ifcopenshell::enumeration_type* resolve_enum_type(entity_view entity, int attr_idx);

inline int find_attr_index(const ifcopenshell::entity* decl, const char* name) {
    if (!decl) {
        return -1;
    }
    ptrdiff_t idx = decl->attribute_index(name);
    return idx < 0 ? -1 : static_cast<int>(idx);
}

inline int attr_index_of(entity_view entity, const char* attr) {
    return entity.attr_index(attr);
}

inline bool entity_has_attr(entity_view entity, const char* attr) {
    return entity.has_attr(attr);
}

inline bool is_attr_null(entity_view entity, const char* attr) {
    return entity.is_null(attr);
}

inline std::string read_string_attr(entity_view entity, const char* attr) {
    return entity.get_or<std::string>(attr, std::string());
}

inline OptionalString read_optional_string_attr(entity_view entity, const char* attr) {
    OptionalString result;
    auto value = entity.get_optional<std::string>(attr);
    if (value) {
        result.has_value = true;
        result.value = *value;
    }
    return result;
}

inline bool optional_string_attr_equals(
    entity_view entity,
    const char* attr,
    const OptionalString& expected)
{
    auto actual = read_optional_string_attr(entity, attr);
    if (actual.has_value != expected.has_value) {
        return false;
    }
    return !expected.has_value || actual.value == expected.value;
}

inline express::Base read_ref_attr(entity_view entity, const char* attr) {
    return entity.get_or<express::Base>(attr, express::Base());
}

inline std::vector<express::Base> get_ref_aggregate(entity_view entity, int attr_idx) {
    if (!entity || attr_idx < 0) {
        return {};
    }
    auto value = entity.attr(static_cast<size_t>(attr_idx));
    if (value.isNull()) {
        return {};
    }
    return static_cast<std::vector<express::Base>>(value);
}

inline std::vector<express::Base> read_ref_aggregate(entity_view entity, const char* attr) {
    return get_ref_aggregate(entity, attr_index_of(entity, attr));
}

inline std::vector<express::Base> read_inverse_aggregate(entity_view entity, const char* attr) {
    std::vector<express::Base> result;
    for (const auto& inverse : entity.inverse(attr)) {
        result.push_back(inverse);
    }
    return result;
}

inline void set_ref_aggregate(
    entity_view entity,
    int attr_idx,
    const std::vector<express::Base>& refs)
{
    if (entity && attr_idx >= 0) {
        entity.set(static_cast<size_t>(attr_idx), refs);
    }
}

inline bool write_ref_aggregate(
    entity_view entity,
    const char* attr,
    const std::vector<express::Base>& refs)
{
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    set_ref_aggregate(entity, idx, refs);
    return true;
}

inline void write_string_attr(entity_view entity, const char* attr, const std::string& value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        if (auto* enum_type = resolve_enum_type(entity, idx)) {
            const auto& items = enum_type->enumeration_items();
            auto it = std::find(items.begin(), items.end(), value);
            if (it != items.end()) {
                entity.set(
                    static_cast<size_t>(idx),
                    enumeration_reference(enum_type, static_cast<size_t>(std::distance(items.begin(), it))));
                return;
            }
        }
        entity.set(static_cast<size_t>(idx), value);
    }
}

inline void clear_attr(entity_view entity, const char* attr) {
    entity.unset(attr);
}

inline void copy_string_attr_preserving_null(
    entity_view target,
    const char* target_attr,
    entity_view source,
    const char* source_attr)
{
    int idx = attr_index_of(target, target_attr);
    if (idx < 0) {
        return;
    }
    if (is_attr_null(source, source_attr)) {
        target.unset(target_attr);
        return;
    }
    target.set(static_cast<size_t>(idx), read_string_attr(source, source_attr));
}

inline void write_optional_string_attr(
    entity_view entity,
    const char* attr,
    const OptionalString& value)
{
    if (value.has_value) {
        write_string_attr(entity, attr, value.value);
    }
}

inline void write_int_attr(entity_view entity, const char* attr, int value) {
    entity.set(attr, value);
}

inline int read_int_attr(entity_view entity, const char* attr, int fallback = 0) {
    return entity.get_or<int>(attr, fallback);
}

inline void write_double_attr(entity_view entity, const char* attr, double value) {
    entity.set(attr, value);
}

inline std::vector<double> read_double_aggregate(entity_view entity, const char* attr) {
    return entity.get_or<std::vector<double>>(attr, std::vector<double>());
}

inline void write_double_aggregate(entity_view entity, const char* attr, const std::vector<double>& values) {
    entity.set(attr, values);
}

inline void write_double_aggregate_aggregate(
    entity_view entity,
    const char* attr,
    const std::vector<std::vector<double>>& values)
{
    entity.set(attr, values);
}

inline const ifcopenshell::enumeration_type* resolve_enum_type(entity_view entity, int attr_idx) {
    auto* entity_decl = entity.declaration();
    if (!entity_decl || attr_idx < 0) {
        return nullptr;
    }
    auto attrs = entity_decl->all_attributes();
    if (static_cast<size_t>(attr_idx) >= attrs.size()) {
        return nullptr;
    }
    const ifcopenshell::parameter_type* pt = attrs[static_cast<size_t>(attr_idx)]->type_of_attribute();
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

inline bool write_enum_attr(entity_view entity, const char* attr, const std::string& value) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    auto* enum_type = resolve_enum_type(entity, idx);
    if (!enum_type) {
        entity.set(static_cast<size_t>(idx), value);
        return true;
    }
    const auto& items = enum_type->enumeration_items();
    auto it = std::find(items.begin(), items.end(), value);
    if (it == items.end()) {
        return false;
    }
    entity.set(
        static_cast<size_t>(idx),
        enumeration_reference(enum_type, static_cast<size_t>(std::distance(items.begin(), it))));
    return true;
}

inline void write_blank_attr(entity_view entity, const char* attr) {
    entity.unset(attr);
}

inline void set_ref(entity_view entity, int attr_idx, const express::Base& ref) {
    if (entity && attr_idx >= 0 && ref) {
        entity.set(static_cast<size_t>(attr_idx), ref);
    }
}

inline void write_ref_attr(entity_view entity, const char* attr, const express::Base& ref) {
    if (ref) {
        entity.set(attr, ref);
    } else {
        entity.unset(attr);
    }
}

inline bool exists_in_file(ifcopenshell::file* file, const express::Base& entity) {
    if (!file || !entity || entity.id() == 0) {
        return false;
    }
    return file->instance_by_id(static_cast<int>(entity.id())) == entity;
}

inline bool contains_ref(const std::vector<express::Base>& values, const express::Base& value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

inline void append_unique(std::vector<express::Base>& values, const express::Base& value) {
    if (value && !contains_ref(values, value)) {
        values.push_back(value);
    }
}

inline void append_unique(
    std::vector<express::Base>& values,
    const std::vector<express::Base>& additional)
{
    for (const auto& value : additional) {
        append_unique(values, value);
    }
}

inline std::vector<express::Base> to_const_refs(const std::vector<express::Base>& values) {
    return values;
}

inline std::vector<express::Base> to_mutable_refs(const std::vector<express::Base>& values) {
    return values;
}

inline std::set<express::Base> to_ref_set(const std::vector<express::Base>& values) {
    return std::set<express::Base>(values.begin(), values.end());
}

inline std::vector<express::Base> to_ref_vector(const std::set<express::Base>& values) {
    return std::vector<express::Base>(values.begin(), values.end());
}

inline const ifcopenshell::declaration* declaration_by_name(ifcopenshell::file* file, const char* ifc_class) {
    if (!file || !file->schema()) {
        return nullptr;
    }
    try {
        return file->schema()->declaration_by_name(ifc_class);
    } catch (const ifcopenshell::exception&) {
        return nullptr;
    }
}

inline std::vector<express::Base> instances_by_type(ifcopenshell::file* file, const char* ifc_class) {
    auto* declaration = declaration_by_name(file, ifc_class);
    return declaration ? file->instances_by_type(declaration) : std::vector<express::Base>();
}

inline express::Base first_instance_by_type(ifcopenshell::file* file, const char* ifc_class) {
    auto instances = instances_by_type(file, ifc_class);
    return instances.empty() ? express::Base() : instances.front();
}

inline express::Base create_typed_double(
    ifcopenshell::file* file,
    const char* ifc_type,
    double value)
{
    auto* declaration = declaration_by_name(file, ifc_type);
    auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    if (!type_declaration) {
        return express::Base();
    }
    auto instance = file->create(type_declaration);
    if (instance) {
        instance.set_attribute_value(0, value);
    }
    return instance;
}

inline express::Base create_typed_string(
    ifcopenshell::file* file,
    const char* ifc_type,
    const std::string& value)
{
    auto* declaration = declaration_by_name(file, ifc_type);
    auto* type_declaration = declaration ? declaration->as_type_declaration() : nullptr;
    if (!type_declaration) {
        return express::Base();
    }
    auto instance = file->create(type_declaration);
    if (instance) {
        instance.set_attribute_value(0, value);
    }
    return instance;
}

inline size_t total_inverses(ifcopenshell::file* file, const express::Base& entity) {
    return file && entity && entity.id() > 0 ? file->get_total_inverses(static_cast<int>(entity.id())) : 0;
}

inline void replace_attribute_reference(
    entity_view element,
    const express::Base& old_reference,
    const express::Base& new_reference)
{
    auto* declaration = element.declaration();
    if (!element || !old_reference || !new_reference || !declaration) {
        return;
    }
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        auto value = element.attr(i);
        if (value.isNull()) {
            continue;
        }
        if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
            if (static_cast<express::Base>(value) == old_reference) {
                element.set(i, new_reference);
            }
        } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            auto aggregate = static_cast<std::vector<express::Base>>(value);
            bool changed = false;
            for (auto& item : aggregate) {
                if (item == old_reference) {
                    item = new_reference;
                    changed = true;
                }
            }
            if (changed) {
                element.set(i, aggregate);
            }
        }
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_ATTRIBUTE_H */
