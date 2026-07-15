// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_COPY_H
#define IFCAPI_DETAIL_COPY_H

#include "guid.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/logic/tribool.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace ifcapi {
namespace detail {

template <typename Argument>
inline void copy_attribute_value(express::Base result, size_t index, const Argument& value) {
    switch (value.type()) {
        case ifcopenshell::Argument_INT:
            result.set_attribute_value(index, static_cast<int>(value)); break;
        case ifcopenshell::Argument_BOOL:
            result.set_attribute_value(index, static_cast<bool>(value)); break;
        case ifcopenshell::Argument_LOGICAL:
            result.set_attribute_value(index, static_cast<boost::logic::tribool>(value)); break;
        case ifcopenshell::Argument_DOUBLE:
            result.set_attribute_value(index, static_cast<double>(value)); break;
        case ifcopenshell::Argument_STRING:
        case ifcopenshell::Argument_ENUMERATION:
            result.set_attribute_value(index, static_cast<std::string>(value)); break;
        case ifcopenshell::Argument_BINARY:
            result.set_attribute_value(index, static_cast<boost::dynamic_bitset<>>(value)); break;
        case ifcopenshell::Argument_ENTITY_INSTANCE:
            result.set_attribute_value(index, static_cast<express::Base>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_INT:
            result.set_attribute_value(index, static_cast<std::vector<int>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE:
            result.set_attribute_value(index, static_cast<std::vector<double>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_STRING:
            result.set_attribute_value(index, static_cast<std::vector<std::string>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_BINARY:
            result.set_attribute_value(index, static_cast<std::vector<boost::dynamic_bitset<>>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE:
            result.set_attribute_value(index, static_cast<std::vector<express::Base>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
            result.set_attribute_value(index, static_cast<std::vector<std::vector<int>>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
            result.set_attribute_value(index, static_cast<std::vector<std::vector<double>>>(value)); break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE:
            result.set_attribute_value(index, static_cast<std::vector<std::vector<express::Base>>>(value)); break;
        default:
            break;
    }
}

inline express::Base copy_single(ifcopenshell::file* file, express::Base source) {
    auto* declaration = source ? source.declaration().as_entity() : nullptr;
    if (!file || !source || !declaration) return {};
    auto result = file->create(declaration);
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        auto value = source.get_attribute_value(i);
        if (value.isNull()) continue;
        if (attrs[i]->name() == "GlobalId") {
            result.set_attribute_value(i, ifcapi::guid_new());
        } else {
            copy_attribute_value(result, i, value);
        }
    }
    return result;
}

inline express::Base shallow_copy(ifcopenshell::file* file, express::Base source) {
    auto* declaration = source ? source.declaration().as_entity() : nullptr;
    if (!file || !source || !declaration) return {};
    auto result = file->create(declaration);
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = source.get_attribute_value(i);
            if (value.isNull()) continue;
            if (attrs[i]->name() == "GlobalId") {
                result.set_attribute_value(i, ifcapi::guid_new());
                continue;
            }
            copy_attribute_value(result, i, value);
        } catch (...) {
        }
    }
    return result;
}

inline bool copy_excluded(express::Base entity, const std::vector<std::string>& exclude) {
    if (!entity) return false;
    for (const auto& ifc_class : exclude) {
        if (entity.declaration().is(ifc_class)) return true;
    }
    return false;
}

inline express::Base deep_copy_impl(
    ifcopenshell::file* file,
    express::Base source,
    const std::vector<std::string>& exclude,
    std::unordered_map<unsigned, express::Base>& memo)
{
    if (!file || !source) return {};
    auto* declaration = source.declaration().as_entity();
    if (!declaration) return source;

    const auto id = source.id();
    if (id) {
        auto found = memo.find(id);
        if (found != memo.end()) return found->second;
    }

    auto result = file->create(declaration);
    if (id) memo.emplace(id, result);
    const auto attributes = declaration->all_attributes();
    for (size_t i = 0; i < attributes.size(); ++i) {
        try {
            auto value = source.get_attribute_value(i);
            if (value.isNull()) continue;
            if (attributes[i]->name() == "GlobalId") {
                result.set_attribute_value(i, ifcapi::guid_new());
                continue;
            }
            if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                auto reference = static_cast<express::Base>(value);
                result.set_attribute_value(
                    i,
                    copy_excluded(reference, exclude)
                        ? reference
                        : deep_copy_impl(file, reference, exclude, memo));
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                std::vector<express::Base> copied;
                for (auto reference : static_cast<std::vector<express::Base>>(value)) {
                    copied.push_back(
                        copy_excluded(reference, exclude)
                            ? reference
                            : deep_copy_impl(file, reference, exclude, memo));
                }
                result.set_attribute_value(i, copied);
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
                std::vector<std::vector<express::Base>> copied;
                for (const auto& row : static_cast<std::vector<std::vector<express::Base>>>(value)) {
                    std::vector<express::Base> copied_row;
                    for (auto reference : row) {
                        copied_row.push_back(
                            copy_excluded(reference, exclude)
                                ? reference
                                : deep_copy_impl(file, reference, exclude, memo));
                    }
                    copied.push_back(std::move(copied_row));
                }
                result.set_attribute_value(i, copied);
            } else {
                copy_attribute_value(result, i, value);
            }
        } catch (...) {
        }
    }
    return result;
}

inline express::Base deep_copy(
    ifcopenshell::file* file,
    express::Base source,
    const std::vector<std::string>& exclude = {})
{
    std::unordered_map<unsigned, express::Base> memo;
    return deep_copy_impl(file, source, exclude, memo);
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_COPY_H */
