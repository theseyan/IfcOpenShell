// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_PSET_H
#define IFCAPI_DETAIL_PSET_H

#include "ifcapi/detail/attribute.h"

#include <string>

namespace ifcapi {
namespace detail {

inline IfcUtil::IfcBaseClass* named_property_set(IfcUtil::IfcBaseClass* product, const std::string& name) {
    for (auto* rel : read_inverse_aggregate(product, "IsDefinedBy")) {
        if (!rel || !rel->declaration().is("IfcRelDefinesByProperties")) {
            continue;
        }
        auto* pset = read_ref_attr(rel, "RelatingPropertyDefinition");
        if (pset && pset->declaration().is("IfcPropertySet") && read_string_attr(pset, "Name") == name) {
            return pset;
        }
    }
    return nullptr;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_PSET_H */
