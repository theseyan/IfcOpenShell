# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Add empty georeferencing entities to a model."""

import ifcopenshell
import ifcopenshell.api.georeference
import ifcopenshell.api.pset
import ifcopenshell.util.element


def add_georeferencing(file, ifc_class="IfcMapConversion", name="EPSG:3857"):
    """Create IfcProjectedCRS and IfcCoordinateOperation with blank parameters."""
    if file.schema == "IFC2X3":
        if not (project := file.by_type("IfcProject")):
            return
        project = project[0]
        if ifcopenshell.util.element.get_pset(project, "ePSet_ProjectedCRS"):
            return
        conversion = ifcopenshell.api.pset.add_pset(file, project, "ePSet_MapConversion")
        crs = ifcopenshell.api.pset.add_pset(file, project, "ePSet_ProjectedCRS")
        ifcopenshell.api.pset.edit_pset(file, crs, properties={"Name": name})
        ifcopenshell.api.pset.edit_pset(
            file,
            conversion,
            properties={
                "Eastings": file.createIfcLengthMeasure(0),
                "Northings": file.createIfcLengthMeasure(0),
                "OrthogonalHeight": file.createIfcLengthMeasure(0),
            },
        )
        return

    has_crs = bool(file.by_type("IfcProjectedCRS"))
    has_conversion = bool(file.by_type("IfcCoordinateOperation"))
    if has_crs and has_conversion:
        return
    if has_crs or has_conversion:
        ifcopenshell.api.georeference.remove_georeferencing(file)

    source_crs = None
    for context in file.by_type("IfcGeometricRepresentationContext", include_subtypes=False):
        if context.ContextType == "Model":
            source_crs = context
            break
    if not source_crs:
        return

    projected_crs = file.create_entity("IfcProjectedCRS", Name=name)
    if ifc_class == "IfcMapConversion":
        file.create_entity(
            ifc_class, SourceCRS=source_crs, TargetCRS=projected_crs,
            Eastings=0, Northings=0, OrthogonalHeight=0,
        )
    elif ifc_class == "IfcMapConversionScaled":
        file.create_entity(
            ifc_class, SourceCRS=source_crs, TargetCRS=projected_crs,
            Eastings=0, Northings=0, OrthogonalHeight=0,
            FactorX=1, FactorY=1, FactorZ=1,
        )
    elif ifc_class == "IfcRigidOperation":
        file.create_entity(
            ifc_class, SourceCRS=source_crs, TargetCRS=projected_crs,
            FirstCoordinate=file.createIfcLengthMeasure(0),
            SecondCoordinate=file.createIfcLengthMeasure(0),
        )
