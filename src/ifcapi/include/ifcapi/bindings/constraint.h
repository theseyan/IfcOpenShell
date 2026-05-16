// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_CONSTRAINT_H
#define IFCAPI_BINDINGS_CONSTRAINT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* constraint_add_objective(IfcParse::IfcFile* file);
IFCAPI_BINDING IfcUtil::IfcBaseClass* constraint_add_metric(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* objective);
IFCAPI_BINDING std::vector<IfcUtil::IfcBaseClass*> constraint_add_metric_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* metric,
    const std::string& reference_path);
IFCAPI_BINDING IfcUtil::IfcBaseClass* constraint_assign_constraint(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* constraint,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void constraint_unassign_constraint(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* constraint,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void constraint_remove_constraint(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constraint);
IFCAPI_BINDING void constraint_remove_metric(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* metric);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONSTRAINT_H */
