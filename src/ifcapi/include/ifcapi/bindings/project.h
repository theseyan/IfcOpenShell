// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROJECT_H
#define IFCAPI_BINDINGS_PROJECT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* project_assign_declaration(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& definitions,
    IfcUtil::IfcBaseClass* relating_context,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void project_unassign_declaration(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& definitions,
    IfcUtil::IfcBaseClass* relating_context,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROJECT_H */
