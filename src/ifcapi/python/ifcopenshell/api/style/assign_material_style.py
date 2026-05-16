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

import ifcopenshell
from ifcopenshell.api.style import _capi


def assign_material_style(
    file: ifcopenshell.file,
    material: ifcopenshell.entity_instance,
    style: ifcopenshell.entity_instance,
    context: ifcopenshell.entity_instance,
    should_use_presentation_style_assignment: bool = False,
) -> None:
    """Assigns a style to a material

    A style may either be assigned directly to an object's representation,
    or to a material which is then associated with the object. If both
    exist, then the style assigned directly to the object's representation
    takes precedence. It is recommended to use materials and assign styles
    to materials. This API function provides that capability.

    :param material: The IfcMaterial which you want to assign the style to.
    :param style: The IfcPresentationStyle (typically IfcSurfaceStyle) that
        you want to assign to the material. This will then be applied to all
        objects that have that material.
    :param context: The IfcGeometricRepresentationSubContext at which this
        style should be used. Typically this is the Model BODY context.
    :param should_use_presentation_style_assignment: This is a technical
        detail to accomodate a bug in Revit. This should always be left as
        the default of False, unless you are finding that colours aren't
        showing up in Revit. In that case, set it to True, but keep in mind
        that this is no longer a valid IFC. Blame Autodesk.
    :return: None

    Example:

    .. code:: python

        # A model context is needed to store 3D geometry
        model3d = ifcopenshell.api.context.add_context(model, context_type="Model")

        # Specifically, we want to store body geometry
        body = ifcopenshell.api.context.add_context(model,
            context_type="Model", context_identifier="Body", target_view="MODEL_VIEW", parent=model3d)

        # Let's create a new wall. The wall does not have any geometry yet.
        wall = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # Let's use the "3D Body" representation we created earlier to add a
        # new wall-like body geometry, 5 meters long, 3 meters high, and
        # 200mm thick
        representation = ifcopenshell.api.geometry.add_wall_representation(model,
            context=body, length=5, height=3, thickness=0.2)

        # Assign our new body geometry back to our wall
        ifcopenshell.api.geometry.assign_representation(model,
            product=wall, representation=representation)

        # Place our wall at the origin
        ifcopenshell.api.geometry.edit_object_placement(model, product=wall)

        # Let's prepare a concrete material. Note that our concrete material
        # does not have any colours (styles) at this point.
        concrete = ifcopenshell.api.material.add_material(model, name="CON01", category="concrete")

        # Assign our concrete material to our wall
        ifcopenshell.api.material.assign_material(model,
            products=[wall], type="IfcMaterial", material=concrete)

        # Create a new surface style
        style = ifcopenshell.api.style.add_style(model)

        # Create a simple grey shading colour and transparency.
        ifcopenshell.api.style.add_surface_style(model,
            style=style, ifc_class="IfcSurfaceStyleShading", attributes={
                "SurfaceColour": { "Name": None, "Red": 0.5, "Green": 0.5, "Blue": 0.5 },
                "Transparency": 0., # 0 is opaque, 1 is transparent
            })

        # Now any element (like our wall) with a concrete material will have
        # a grey colour applied.
        ifcopenshell.api.style.assign_material_style(model, material=concrete, style=style, context=body)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_style_assign_material_style,
        "Failed to assign material style",
        _capi.file_handle(file),
        _capi.instance_handle(material),
        _capi.instance_handle(style),
        _capi.instance_handle(context),
        should_use_presentation_style_assignment,
    )
