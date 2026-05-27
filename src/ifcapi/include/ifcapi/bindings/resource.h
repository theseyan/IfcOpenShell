// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_RESOURCE_H
#define IFCAPI_BINDINGS_RESOURCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void resource_edit_resource_time(
    ifcopenshell::file* file,
    express::Base* resource_time,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_RESOURCE_H */
