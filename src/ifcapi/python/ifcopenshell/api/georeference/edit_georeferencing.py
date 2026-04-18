# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit georeferencing attributes (CRS and map conversion)."""

import ifcopenshell.api.pset
import ifcopenshell.util.element


def edit_georeferencing(file, coordinate_operation=None, projected_crs=None):
    """Edit the attributes of a map conversion and/or projected CRS."""
    if file.schema == "IFC2X3":
        if not (project := file.by_type("IfcProject")):
            return
        project = project[0]
        if projected_crs:
            if crs := ifcopenshell.util.element.get_pset(project, "ePSet_ProjectedCRS"):
                crs = file.by_id(crs["id"])
                for k, v in projected_crs.items():
                    if k == "Description":
                        v = file.createIfcText(v)
                    elif k == "Name":
                        v = file.createIfcLabel(v)
                    elif v is not None:
                        v = file.createIfcIdentifier(v)
                ifcopenshell.api.pset.edit_pset(file, crs, properties=projected_crs)
        if coordinate_operation:
            if conversion := ifcopenshell.util.element.get_pset(project, "ePSet_MapConversion"):
                conversion = file.by_id(conversion["id"])
                for k, v in coordinate_operation.items():
                    if k in ("XAxisAbscissa", "XAxisOrdinate", "Scale"):
                        v = file.createIfcReal(v)
                    else:
                        v = file.createIfcLengthMeasure(v)
                ifcopenshell.api.pset.edit_pset(file, conversion, properties=coordinate_operation)
        return

    if projected_crs:
        crs = file.by_type("IfcProjectedCRS")[0]
        for name, value in projected_crs.items():
            setattr(crs, name, value)
    if coordinate_operation:
        conversion = file.by_type("IfcCoordinateOperation")[0]
        for name, value in coordinate_operation.items():
            setattr(conversion, name, value)
