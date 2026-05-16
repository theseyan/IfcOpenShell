// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROFILE_H
#define IFCAPI_BINDINGS_PROFILE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_add_parameterized_profile(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const std::string& profile_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_copy_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING void profile_remove_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROFILE_H */
