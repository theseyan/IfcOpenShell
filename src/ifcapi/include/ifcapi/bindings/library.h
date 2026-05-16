// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LIBRARY_H
#define IFCAPI_BINDINGS_LIBRARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* library_add_library(
    IfcParse::IfcFile* file,
    const std::string& name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* library_add_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* library);
IFCAPI_BINDING IfcUtil::IfcBaseClass* library_assign_reference(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void library_unassign_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void library_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference);
IFCAPI_BINDING void library_remove_library(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* library);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LIBRARY_H */
