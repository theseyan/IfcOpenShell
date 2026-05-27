// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_GEOREFERENCE_H
#define IFCAPI_BINDINGS_GEOREFERENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void georeference_add_georeferencing(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const std::string& name,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void georeference_edit_true_north(ifcopenshell::file* file, bool has_true_north, double x, double y);

IFCAPI_BINDING void georeference_edit_georeferencing(
    ifcopenshell::file* file,
    bool has_coordinate_operation,
    ifcopenshell_pset_props_t* coordinate_operation,
    bool has_projected_crs,
    ifcopenshell_pset_props_t* projected_crs);

IFCAPI_BINDING void georeference_edit_wcs(
    ifcopenshell::file* file,
    double x,
    double y,
    double z,
    double rotation,
    bool is_si);

IFCAPI_BINDING void georeference_remove_georeferencing(ifcopenshell::file* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOREFERENCE_H */
