// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_API_PSET_ATTRIBUTE_PROPS_HPP
#define IFCAPI_API_PSET_ATTRIBUTE_PROPS_HPP

#include "ifcapi/detail/attribute.h"
#include "props.hpp"

#include <stdexcept>
#include <string>

namespace ifcapi {
namespace detail {

inline void apply_attribute_prop(IfcUtil::IfcBaseClass* entity, const ifcapi_pset::Entry& entry) {
    if (!entity) return;
    int idx = attr_index_of(entity, entry.key.c_str());
    if (idx < 0) {
        throw std::runtime_error("Attribute " + entry.key + " not found on " + entity->declaration().name());
    }
    const auto attr_idx = static_cast<size_t>(idx);
    switch (entry.kind) {
        case ifcapi_pset::Kind::NONE:
            entity->set_attribute_value(attr_idx, Blank{});
            break;
        case ifcapi_pset::Kind::BOOL:
        case ifcapi_pset::Kind::TYPED_BOOL:
            entity->set_attribute_value(attr_idx, entry.b_val);
            break;
        case ifcapi_pset::Kind::INT:
        case ifcapi_pset::Kind::TYPED_INT:
            entity->set_attribute_value(attr_idx, static_cast<int>(entry.i_val));
            break;
        case ifcapi_pset::Kind::DOUBLE:
        case ifcapi_pset::Kind::TYPED_DOUBLE:
            entity->set_attribute_value(attr_idx, entry.d_val);
            break;
        case ifcapi_pset::Kind::STRING:
        case ifcapi_pset::Kind::TYPED_STRING:
            entity->set_attribute_value(attr_idx, entry.s_val);
            break;
        case ifcapi_pset::Kind::STRING_LIST:
            entity->set_attribute_value(attr_idx, entry.str_list);
            break;
        case ifcapi_pset::Kind::INSTANCE:
            entity->set_attribute_value(attr_idx, entry.inst);
            break;
        case ifcapi_pset::Kind::DOUBLE_LIST:
            entity->set_attribute_value(attr_idx, entry.dbl_list);
            break;
        case ifcapi_pset::Kind::INT_LIST: {
            std::vector<int> values;
            values.reserve(entry.int_list.size());
            for (auto value : entry.int_list) values.push_back(static_cast<int>(value));
            entity->set_attribute_value(attr_idx, values);
            break;
        }
        default:
            throw std::runtime_error("Unsupported attribute value for " + entry.key);
    }
}

inline void apply_attribute_props(IfcUtil::IfcBaseClass* entity, ifcopenshell_pset_props_t* props) {
    if (!props) return;
    for (const auto& entry : props->entries) {
        apply_attribute_prop(entity, entry);
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_API_PSET_ATTRIBUTE_PROPS_HPP */
