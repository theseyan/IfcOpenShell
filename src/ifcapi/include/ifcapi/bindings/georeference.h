// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_GEOREFERENCE_H
#define IFCAPI_BINDINGS_GEOREFERENCE_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/bindings/types.h"

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
    /// IFC class for the coordinate operation, e.g. "IfcMapConversion", "IfcMapConversionScaled", or "IfcRigidOperation".
    std::string ifc_class;
    /// Name for the IfcProjectedCRS.
    std::string name;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/**
 * Add georeferencing entities to a model.
 *
 * Creates an IfcProjectedCRS and a coordinate operation (IfcMapConversion,
 * IfcMapConversionScaled, or IfcRigidOperation) linking the Model context
 * to the projected CRS. For IFC2X3, creates ePSet_MapConversion and
 * ePSet_ProjectedCRS property sets on the IfcProject instead. Does nothing
 * if georeferencing entities already exist.
 *
 * @param file File that receives the new entities.
 * @param options Coordinate operation class, CRS name, and ownership options.
 */
IFCAPI_BINDING void georeference_add_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceAddGeoreferencingOptions& options);

/**
 * Options for editing the true north direction.
 *
 * When omitted, existing true north is removed from all geometric contexts.
 * When provided, the first two values are used as (X, Y); missing values
 * default to 0.0. The direction is not normalized.
 */
struct GeoreferenceEditTrueNorthOptions {
    /// Direction ratios (X, Y) for true north. When omitted, true north is removed.
    std::optional<std::array<double, 2>> true_north;
};

/**
 * Options for editing map conversion and projected CRS attributes.
 *
 * coordinate_operation and projected_crs are optional property bags. When
 * omitted, the corresponding entity is left unchanged. When provided,
 * the attributes are applied to the existing IfcCoordinateOperation or
 * IfcProjectedCRS (or the IFC2X3 equivalent property sets).
 */
struct GeoreferenceEditGeoreferencingOptions {
    /// Property bag for IfcCoordinateOperation attributes (e.g. Eastings, Northings). Omit to leave it unchanged.
    std::optional<ifcopenshell_pset_props_t*> coordinate_operation;
    /// Property bag for IfcProjectedCRS attributes (e.g. Name, MapZone). Omit to leave it unchanged.
    std::optional<ifcopenshell_pset_props_t*> projected_crs;
};

/**
 * Options for editing the world coordinate system (WCS).
 *
 * All fields are optional. Missing numeric fields default to 0.0.
 * is_si controls whether coordinates are in SI metres (true, default)
 * or already in project units (false).
 */
struct GeoreferenceEditWcsOptions {
    /// X offset in SI metres (or project units when is_si is false). Defaults to 0.0.
    std::optional<double> x;
    /// Y offset in SI metres (or project units when is_si is false). Defaults to 0.0.
    std::optional<double> y;
    /// Z offset in SI metres (or project units when is_si is false). Defaults to 0.0.
    std::optional<double> z;
    /// Rotation of the X axis in degrees. Defaults to 0.0.
    std::optional<double> rotation;
    /// When true (default), x/y/z are in SI metres and are scaled to project units. When false, values are used as-is.
    std::optional<bool> is_si;
};

/**
 * Set or remove the true north direction on all geometric representation contexts.
 *
 * When omitted, any existing TrueNorth reference is removed from every
 * IfcGeometricRepresentationContext. When provided, the first two values are
 * used as (X, Y) direction ratios; missing values default to 0.0. The
 * direction is not normalized.
 *
 * @param file File whose contexts to update.
 * @param options True north direction ratios, or omission to remove true north.
 */
IFCAPI_BINDING void georeference_edit_true_north(
    ifcopenshell::file* file,
    const GeoreferenceEditTrueNorthOptions& options);

/**
 * Edit map conversion and projected CRS attributes.
 *
 * For IFC4+, applies attribute changes from the property bags to the first
 * IfcCoordinateOperation and/or IfcProjectedCRS in the file. For IFC2X3,
 * applies changes to the ePSet_MapConversion and ePSet_ProjectedCRS property
 * sets on the IfcProject. Entities must already exist (call
 * georeference_add_georeferencing first if needed).
 *
 * @param file File containing the georeferencing entities.
 * @param options Property bags for coordinate operation and projected CRS.
 */
IFCAPI_BINDING void georeference_edit_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceEditGeoreferencingOptions& options);

/**
 * Edit the world coordinate system (WCS) on all geometric representation contexts.
 *
 * Replaces the WorldCoordinateSystem placement on every
 * IfcGeometricRepresentationContext. The rotation parameter specifies the
 * X-axis rotation in degrees. When is_si is true (default), coordinates are
 * interpreted as SI metres and scaled to project length units. The old WCS
 * placement is deleted if unreferenced.
 *
 * @param file File whose contexts to update.
 * @param options WCS offset, rotation, and unit-system options.
 */
IFCAPI_BINDING void georeference_edit_wcs(
    ifcopenshell::file* file,
    const GeoreferenceEditWcsOptions& options);

/**
 * Remove all georeferencing entities from a model.
 *
 * For IFC4+, deletes all IfcProjectedCRS and IfcCoordinateOperation entities
 * (including orphaned MapUnit). For IFC2X3, removes the ePSet_MapConversion
 * and ePSet_ProjectedCRS property sets from the IfcProject.
 *
 * @param file File from which to remove georeferencing.
 */
IFCAPI_BINDING void georeference_remove_georeferencing(ifcopenshell::file* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOREFERENCE_H */
