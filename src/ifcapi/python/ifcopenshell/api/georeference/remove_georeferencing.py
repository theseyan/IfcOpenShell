# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Remove georeferencing data from a model."""

import ifcopenshell.api.pset
import ifcopenshell.util.element


def remove_georeferencing(file):
    """Remove all georeferencing entities (CRS, map conversion, or psets in IFC2X3)."""
    if file.schema == "IFC2X3":
        project = file.by_type("IfcProject")[0]
        if pset := ifcopenshell.util.element.get_pset(project, "ePSet_ProjectedCRS"):
            ifcopenshell.api.pset.remove_pset(file, project, file.by_id(pset["id"]))
        if pset := ifcopenshell.util.element.get_pset(project, "ePSet_MapConversion"):
            ifcopenshell.api.pset.remove_pset(file, project, file.by_id(pset["id"]))
        return

    for projected_crs in file.by_type("IfcProjectedCRS"):
        if (unit := projected_crs.MapUnit) and file.get_total_inverses(unit) == 1:
            projected_crs.MapUnit = None
            ifcopenshell.util.element.remove_deep2(file, unit)
        file.remove(projected_crs)
    for coordinate_operation in file.by_type("IfcCoordinateOperation"):
        file.remove(coordinate_operation)
