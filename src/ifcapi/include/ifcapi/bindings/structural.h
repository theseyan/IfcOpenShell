// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_STRUCTURAL_H
#define IFCAPI_BINDINGS_STRUCTURAL_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_analysis_model(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_assign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void structural_unassign_structural_analysis_model(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* structural_analysis_model,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history);
IFCAPI_BINDING IfcUtil::IfcBaseClass* structural_add_structural_activity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* applied_load,
    IfcUtil::IfcBaseClass* structural_member,
    const std::string& ifc_class,
    const std::string& predefined_type,
    const std::string& global_or_local,
    IfcUtil::IfcBaseClass* activity_owner_history,
    IfcUtil::IfcBaseClass* relationship_owner_history);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_STRUCTURAL_H */
