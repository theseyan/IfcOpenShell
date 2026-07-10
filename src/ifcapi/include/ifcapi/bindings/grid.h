/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GRID_H
#define IFCAPI_BINDINGS_GRID_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Create an IfcGridAxis and append it to the specified grid axis aggregate.
 *
 * @param file IFC file that receives the new axis.
 * @param grid IfcGrid entity owning the axis.
 * @param axis_tag Label for the axis (e.g. "A", "1").
 * @param same_sense True if the axis direction agrees with the curve direction.
 * @param uvw_axes Name of the grid aggregate to append to: "UAxes", "VAxes", or "WAxes".
 * @return Newly created IfcGridAxis, or a null handle on failure.
 */
IFCAPI_BINDING express::Base grid_create_grid_axis(
    ifcopenshell::file* file,
    express::Base* grid,
    const std::string& axis_tag,
    bool same_sense,
    const std::string& uvw_axes);

/**
 * Remove an IfcGridAxis and its associated AxisCurve.
 *
 * The axis entity is removed from the file and its AxisCurve (if any) is
 * deep-removed.
 *
 * @param file IFC file to modify.
 * @param axis IfcGridAxis entity to remove.
 */
IFCAPI_BINDING void grid_remove_grid_axis(
    ifcopenshell::file* file,
    express::Base* axis);

/**
 * Create or replace the AxisCurve of a grid axis as a 2D IfcPolyline.
 *
 * Points are given in world coordinates; when is_si is true they are divided
 * by the file's LENGTHUNIT scale. The points are transformed into the grid's
 * local coordinate system using the grid's ObjectPlacement. If the axis
 * already has an AxisCurve, it is deep-removed after replacement.
 *
 * @param file IFC file that receives the polyline.
 * @param p1 First endpoint (at least three coordinates; X and Y are used).
 * @param p2 Second endpoint (at least three coordinates; X and Y are used).
 * @param grid_axis IfcGridAxis whose AxisCurve to set.
 * @param is_si True if p1/p2 are in SI metres; false if already in model units.
 */
IFCAPI_BINDING void grid_create_axis_curve(
    ifcopenshell::file* file,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    express::Base* grid_axis,
    bool is_si);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GRID_H */
