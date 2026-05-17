// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_COPY_H
#define IFCAPI_DETAIL_COPY_H

#include "guid.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/logic/tribool.hpp>

#include <string>
#include <vector>

namespace ifcapi {
namespace detail {

template <typename Argument>
inline void copy_attribute_value(IfcUtil::IfcBaseClass* result, size_t index, const Argument& value) {
    switch (value.type()) {
        case IfcUtil::Argument_INT:
            result->set_attribute_value(index, static_cast<int>(value)); break;
        case IfcUtil::Argument_BOOL:
            result->set_attribute_value(index, static_cast<bool>(value)); break;
        case IfcUtil::Argument_LOGICAL:
            result->set_attribute_value(index, static_cast<boost::logic::tribool>(value)); break;
        case IfcUtil::Argument_DOUBLE:
            result->set_attribute_value(index, static_cast<double>(value)); break;
        case IfcUtil::Argument_STRING:
        case IfcUtil::Argument_ENUMERATION:
            result->set_attribute_value(index, static_cast<std::string>(value)); break;
        case IfcUtil::Argument_BINARY:
            result->set_attribute_value(index, static_cast<boost::dynamic_bitset<>>(value)); break;
        case IfcUtil::Argument_ENTITY_INSTANCE:
            result->set_attribute_value(index, static_cast<IfcUtil::IfcBaseClass*>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_INT:
            result->set_attribute_value(index, static_cast<std::vector<int>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE:
            result->set_attribute_value(index, static_cast<std::vector<double>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_STRING:
            result->set_attribute_value(index, static_cast<std::vector<std::string>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_BINARY:
            result->set_attribute_value(index, static_cast<std::vector<boost::dynamic_bitset<>>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE:
            result->set_attribute_value(index, static_cast<aggregate_of_instance::ptr>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
            result->set_attribute_value(index, static_cast<std::vector<std::vector<int>>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
            result->set_attribute_value(index, static_cast<std::vector<std::vector<double>>>(value)); break;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE:
            result->set_attribute_value(index, static_cast<aggregate_of_aggregate_of_instance::ptr>(value)); break;
        default:
            break;
    }
}

inline IfcUtil::IfcBaseClass* copy_single(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* source) {
    auto* declaration = source ? source->declaration().as_entity() : nullptr;
    if (!file || !source || !declaration) return nullptr;
    auto* result = file->create(declaration);
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        auto value = source->get_attribute_value(i);
        if (value.isNull()) continue;
        if (attrs[i]->name() == "GlobalId") {
            result->set_attribute_value(i, ifcapi::guid_new());
        } else {
            copy_attribute_value(result, i, value);
        }
    }
    return result;
}

inline IfcUtil::IfcBaseClass* shallow_copy(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* source) {
    auto* declaration = source ? source->declaration().as_entity() : nullptr;
    if (!file || !source || !declaration) return nullptr;
    auto* result = file->create(declaration);
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = source->get_attribute_value(i);
            if (value.isNull()) continue;
            if (attrs[i]->name() == "GlobalId") {
                result->set_attribute_value(i, ifcapi::guid_new());
                continue;
            }
            copy_attribute_value(result, i, value);
        } catch (...) {
        }
    }
    return result;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_COPY_H */
