// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DOCUMENT_H
#define IFCAPI_BINDINGS_DOCUMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* document_add_information(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* document_add_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* information);
IFCAPI_BINDING IfcUtil::IfcBaseClass* document_assign_document(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* document,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void document_unassign_document(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* document,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void document_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference);
IFCAPI_BINDING void document_remove_information(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* information);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DOCUMENT_H */
