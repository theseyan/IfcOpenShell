// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROFILE_H
#define IFCAPI_BINDINGS_PROFILE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_add_parameterized_profile(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const std::string& profile_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_add_arbitrary_profile(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& profile,
    const char* name,
    bool has_name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_add_arbitrary_profile_with_voids(
    IfcParse::IfcFile* file,
    const std::vector<std::vector<double>>& outer_profile,
    const std::vector<std::vector<std::vector<double>>>& inner_profiles,
    const char* name,
    bool has_name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* profile_copy_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING void profile_edit_profile(
    IfcUtil::IfcBaseClass* profile,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING void profile_remove_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROFILE_H */
