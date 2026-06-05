# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.
from typing import Any

import ifcopenshell
from ifcopenshell.api.pset import _capi as pset_capi
from ifcopenshell.api.style import _capi


def edit_surface_style(
    file: ifcopenshell.file, style: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcPresentationItem

    For more information about the attributes and data types of an
    IfcPresentationItem, consult the IFC documentation.

    The IfcPresentationItem is expected to be one of IfcSurfaceStyleShading,
    IfcSurfaceStyleRendering, IfcSurfaceStyleWithTextures,
    IfcSurfaceStyleLighting, IfcSurfaceStyleReflectance, or
    IfcExternallyDefinedSurfaceStyle.

    To represent a colour, a nested dictionary should be used. See the
    example below.

    :param style: The IfcPresentationStyle entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        # Create a new surface style
        style = ifcopenshell.api.style.add_style(model)

        # Create a blank rendering style.
        rendering = ifcopenshell.api.style.add_surface_style(model,
            style=style, ifc_class="IfcSurfaceStyleRendering")

        # Edit the attributes of the rendering style.
        ifcopenshell.api.style.edit_surface_style(model,
            style=rendering, attributes={
                # A surface colour and transparency is still supplied for
                # viewport display only. This will supersede the shading
                # presentation item.
                "SurfaceColour": { "Name": None, "Red": 1.0, "Green": 0.8, "Blue": 0.8 },
                "Transparency": 0., # 0 is opaque, 1 is transparent

                # NOTDEFINED is assumed to be a PHYSICAL (PBR) lighting
                # model. In IFC4X3, you may choose PHYSICAL directly.
                "ReflectanceMethod": "NOTDEFINED",

                # For PBR shading, you may specify these parameters:
                "DiffuseColour": { "Name": None, "Red": 0.9, "Green": 0.8, "Blue": 0.8 },
                "SpecularColour": 0.1, # Metallic factor
                "SpecularHighlight": {"SpecularRoughness": 0.5}, # Roughness factor
            })
    """
    props = pset_capi.build_props(attributes)
    try:
        _capi.call_status(
            "style_edit_surface_style",
            "Failed to edit surface style",
            _capi.file_handle(file),
            _capi.instance_handle(style),
            props,
        )
    finally:
        pset_capi.free_props(props)
