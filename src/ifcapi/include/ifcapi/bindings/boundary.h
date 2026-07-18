/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_BOUNDARY_H
#define IFCAPI_BINDINGS_BOUNDARY_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/bindings/types.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Create a copy of a space boundary relationship and its connection geometry.
 *
 * @param file File that receives the copied entities.
 * @param boundary IfcRelSpaceBoundary entity to copy.
 * @return Newly created boundary relationship, or no result if the copy cannot be created.
 */
IFCAPI_BINDING express::Base boundary_copy_boundary(
    ifcopenshell::file* file,
    express::Base* boundary);

/**
 * Options for assigning connection geometry to a space boundary.
 */
struct BoundaryAssignConnectionGeometryOptions {
    /// Outer boundary of the connection plane, in SI metres and converted to project units using unit_scale.
    std::vector<std::array<double, 2>> outer_boundary;
    /// Origin of the connection plane relative to the bounded space, in SI metres and converted to project units using unit_scale.
    std::array<double, 3> location;
    /// Local axis direction of the connection plane.
    std::array<double, 3> axis;
    /// Local reference direction of the connection plane.
    std::array<double, 3> ref_direction;
    /// Inner boundaries representing openings in the connection plane, in SI metres and converted to project units using unit_scale.
    std::vector<std::vector<std::array<double, 2>>> inner_boundaries;
    /// Scale that converts model units to SI units.
    double unit_scale = 1.0;
};

/**
 * Assign a planar connection geometry to a space boundary relationship.
 */
IFCAPI_BINDING void boundary_assign_connection_geometry(
    ifcopenshell::file* file,
    express::Base* rel_space_boundary,
    const BoundaryAssignConnectionGeometryOptions& options);
/**
 * Remove a space boundary relationship and its connection geometry.
 *
 * Removes the connection geometry and then removes the boundary relationship.
 * Unreferenced entities belonging to the connection geometry are removed.
 *
 * @param file IFC file containing the boundary.
 * @param boundary IfcRelSpaceBoundary entity to remove.
 */
IFCAPI_BINDING void boundary_remove_boundary(
    ifcopenshell::file* file,
    express::Base* boundary);

/**
 * Options for editing attributes of a space boundary relationship.
 */
struct BoundaryEditAttributesOptions {
    /// The space that the boundary relates to.
    express::Base relating_space;
    /// The building element that defines the boundary.
    express::Base related_building_element;
    /// Optional parent boundary for inner boundaries.
    std::optional<express::Base> parent_boundary;
    /// Optional corresponding boundary on the other side of the element.
    std::optional<express::Base> corresponding_boundary;
    /// Physical or virtual enum value.
    std::string physical_or_virtual;
    /// Internal or external enum value.
    std::string internal_or_external;
};

/**
 * Edit attributes of a space boundary relationship.
 *
 * Updates the relating space, related building element, and boundary
 * classification. ParentBoundary and CorrespondingBoundary are set only when
 * the schema supports them (IFC4+). When omitted, those attributes are cleared.
 *
 * @param entity IfcRelSpaceBoundary entity to modify.
 * @param options Attribute values to set.
 */
IFCAPI_BINDING void boundary_edit_attributes(
    express::Base* entity,
    const BoundaryEditAttributesOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_BOUNDARY_H */
