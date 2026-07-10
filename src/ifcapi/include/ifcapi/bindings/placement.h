/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PLACEMENT_H
#define IFCAPI_BINDINGS_PLACEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Compute a 4x4 row-major transformation matrix from origin and axes.
 *
 * @param origin XYZ origin of the placement.
 * @param z_axis Direction ratios for the Z axis.
 * @param x_axis Direction ratios for the X axis (Y is derived).
 * @return 16-element row-major 4x4 matrix.
 */
IFCAPI_BINDING std::vector<double> placement_matrix_from_axes(
    const std::vector<double>& origin,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis);

/**
 * Extract a 4x4 row-major matrix from an IfcAxis2Placement entity.
 *
 * Supports IfcAxis2Placement2D, IfcAxis2Placement3D, and IfcAxis1Placement.
 * Returns an identity matrix if the instance is null or unsupported.
 *
 * @param instance IfcAxis2Placement entity.
 * @return 16-element row-major 4x4 matrix.
 */
IFCAPI_BINDING std::vector<double> placement_get_axis2_placement(express::Base* instance);

/**
 * Compute the cumulative 4x4 row-major world matrix of an IfcLocalPlacement.
 *
 * Walks the PlacementRelTo chain to compute the full transformation.
 * Returns an identity matrix if the instance is nullopt.
 *
 * @param instance IfcLocalPlacement entity, or nullopt for identity.
 * @return 16-element row-major 4x4 matrix.
 */
IFCAPI_BINDING std::vector<double> placement_get_local_placement(
    std::optional<express::Base> instance);

/**
 * Extract a 4x4 row-major matrix from an IfcCartesianTransformationOperator3D.
 *
 * Handles uniform and non-uniform scaling. Returns an identity matrix if the
 * instance is null or not a cartesian transformation operator.
 *
 * @param instance IfcCartesianTransformationOperator3D entity.
 * @return 16-element row-major 4x4 matrix.
 */
IFCAPI_BINDING std::vector<double> placement_get_cartesian_xform_3d(express::Base* instance);

/**
 * Compute the combined 4x4 row-major matrix for an IfcMappedItem.
 *
 * Multiplies the MappingTarget transformation by the MappingOrigin placement.
 * Returns an identity matrix if the instance is null or not an IfcMappedItem.
 *
 * @param instance IfcMappedItem entity.
 * @return 16-element row-major 4x4 matrix.
 */
IFCAPI_BINDING std::vector<double> placement_get_mappeditem_xform(express::Base* instance);

/**
 * Return the elevation of a building storey in model units.
 *
 * Uses the Z-translation of the storey's ObjectPlacement when available,
 * falling back to the Elevation attribute. Returns 0.0 if the instance is
 * null or has no placement.
 *
 * @param instance IfcBuildingStorey entity.
 * @return Elevation in model units.
 */
IFCAPI_BINDING double placement_get_storey_elevation(express::Base* instance);

/**
 * Build a 4x4 row-major rotation matrix about a principal axis.
 *
 * @param angle_rad Rotation angle in radians.
 * @param axis Rotation axis: "X", "Y", or "Z" (case-insensitive).
 * @return 16-element row-major 4x4 rotation matrix.
 */
IFCAPI_BINDING std::vector<double> placement_rotation(double angle_rad, const std::string& axis);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PLACEMENT_H */
