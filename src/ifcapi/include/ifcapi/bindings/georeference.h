// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_GEOREFERENCE_H
#define IFCAPI_BINDINGS_GEOREFERENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Options for adding georeferencing entities to a model.
 */
struct GeoreferenceAddGeoreferencingOptions {
    std::string ifc_class;
    std::string name;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING void georeference_add_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceAddGeoreferencingOptions& options);

/**
 * Options for editing the true north direction.
 *
 * When true_north is std::nullopt, existing true north is removed from all
 * geometric contexts. When present, it must contain exactly two elements
 * representing a unitised 2D direction vector.
 */
struct GeoreferenceEditTrueNorthOptions {
    std::optional<std::vector<double>> true_north;
};

/**
 * Options for editing map conversion and projected CRS attributes.
 *
 * coordinate_operation and projected_crs are optional property bags.
 * When nullptr, the corresponding entity is left unchanged. When present,
 * the attributes are applied to the existing IfcCoordinateOperation or
 * IfcProjectedCRS (or the IFC2X3 equivalent property sets).
 */
struct GeoreferenceEditGeoreferencingOptions {
    std::optional<ifcopenshell_pset_props_t*> coordinate_operation;
    std::optional<ifcopenshell_pset_props_t*> projected_crs;
};

/**
 * Options for editing the world coordinate system (WCS).
 *
 * All fields are optional. Missing numeric fields default to 0.0.
 * is_si controls whether coordinates are in project units (true, default)
 * or already scaled to SI (false).
 */
struct GeoreferenceEditWcsOptions {
    std::optional<double> x;
    std::optional<double> y;
    std::optional<double> z;
    std::optional<double> rotation;
    std::optional<bool> is_si;
};

IFCAPI_BINDING void georeference_edit_true_north(
    ifcopenshell::file* file,
    const GeoreferenceEditTrueNorthOptions& options);

IFCAPI_BINDING void georeference_edit_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceEditGeoreferencingOptions& options);

IFCAPI_BINDING void georeference_edit_wcs(
    ifcopenshell::file* file,
    const GeoreferenceEditWcsOptions& options);

IFCAPI_BINDING void georeference_remove_georeferencing(ifcopenshell::file* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOREFERENCE_H */
