// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROFILE_H
#define IFCAPI_BINDINGS_PROFILE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base profile_add_parameterized_profile(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const std::string& profile_type);
IFCAPI_BINDING express::Base profile_add_arbitrary_profile(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& profile,
    const char* name,
    bool has_name);
IFCAPI_BINDING express::Base profile_add_arbitrary_profile_with_voids(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& outer_profile,
    const std::vector<std::vector<std::vector<double>>>& inner_profiles,
    const char* name,
    bool has_name);
IFCAPI_BINDING express::Base profile_copy_profile(
    ifcopenshell::file* file,
    express::Base* profile);
IFCAPI_BINDING void profile_edit_profile(
    express::Base* profile,
    ifcopenshell_pset_props_t* attributes);
IFCAPI_BINDING void profile_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROFILE_H */
