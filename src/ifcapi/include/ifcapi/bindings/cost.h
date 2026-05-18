// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COST_H
#define IFCAPI_BINDINGS_COST_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void cost_edit_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_value,
    ifcopenshell_pset_props_t* attributes,
    bool has_unit_basis,
    bool unit_basis_is_null,
    double value_component,
    IfcUtil::IfcBaseClass* unit_component);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COST_H */
