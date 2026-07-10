/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_BOUNDARY_H
#define IFCAPI_BINDINGS_BOUNDARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Shallow-copy a space boundary relationship, deep-copying its connection geometry.
 *
 * @param file File that receives the copied entities.
 * @param boundary IfcRelSpaceBoundary entity to copy.
 * @return Newly created copy, or a null handle on failure.
 */
IFCAPI_BINDING express::Base boundary_copy_boundary(
    ifcopenshell::file* file,
    express::Base* boundary);

/**
 * Options for assigning connection geometry to a space boundary.
 */
struct BoundaryAssignConnectionGeometryOptions {
    /// Closed outer polyline of the connection plane.
    std::vector<std::vector<double>> outer_boundary;
    /// Origin of the connection plane relative to the bounded space.
    std::vector<double> location;
    /// Local axis direction of the connection plane.
    std::vector<double> axis;
    /// Local reference direction of the connection plane.
    std::vector<double> ref_direction;
    /// Closed inner polylines representing openings in the connection plane.
    std::vector<std::vector<std::vector<double>>> inner_boundaries;
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
 * Removes the ConnectionGeometry attribute first (deep-removing its entities),
 * then removes the boundary entity itself with history cleanup.
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
 * the schema supports them (IFC4+); nullopt clears those attributes.
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
