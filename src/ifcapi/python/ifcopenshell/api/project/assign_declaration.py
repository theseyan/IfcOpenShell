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

from typing import Union

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def assign_declaration(
    file: ifcopenshell.file,
    definitions: list[ifcopenshell.entity_instance],
    relating_context: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Declares the list of elements to the project

    Feature was added in IFC4.

    All data in a model must be directly or indirectly related to the
    project. Most data is indirectly related, existing instead within the
    spatial decomposition tree. Other data, such as types, may be declared
    at the top level.

    Most of the time, the API handles declaration automatically for you.
    There is one scenario where you might want to explicitly declare objects
    to the project, and that's when you want to organise objects into
    project libraries for future use (such as an assets library). Assigning
    a declaration lets you say that an object belongs to a library.

    :param definitions: The list of objects you want to declare. Typically a list of assets.
    :param relating_context: The IfcProject, or more commonly the
        IfcProjectLibrary that you want the object to be part of.
    :return: The new IfcRelDeclares relationship or None if all definitions
        were already declared / do not support declaration.

    Example:

    .. code:: python

        # Programmatically generate a library. You could do this visually too.
        library = ifcopenshell.api.project.create_file()
        root = ifcopenshell.api.root.create_entity(library, ifc_class="IfcProject", name="Demo Library")
        context = ifcopenshell.api.root.create_entity(library,
            ifc_class="IfcProjectLibrary", name="Demo Library")

        # It's necessary to say our library is part of our project.
        ifcopenshell.api.project.assign_declaration(library, definitions=[context], relating_context=root)

        # Assign units for our example library
        unit = ifcopenshell.api.unit.add_si_unit(library,
            unit_type="LENGTHUNIT", prefix="MILLI")
        ifcopenshell.api.unit.assign_unit(library, units=[unit])

        # Let's create a single asset of a 200mm thick concrete wall
        wall_type = ifcopenshell.api.root.create_entity(library, ifc_class="IfcWallType", name="WAL01")
        concrete = ifcopenshell.api.material.add_material(file, name="CON", category="concrete")
        rel = ifcopenshell.api.material.assign_material(library,
            products=[wall_type], type="IfcMaterialLayerSet")
        layer = ifcopenshell.api.material.add_layer(library,
            layer_set=rel.RelatingMaterial, material=concrete)
        layer.Name = "Structure"
        layer.LayerThickness = 200

        # Mark our wall type as a reusable asset in our library.
        ifcopenshell.api.project.assign_declaration(library,
            definitions=[wall_type], relating_context=context)

        # All done, just for fun let's save our asset library to disk for later use.
        library.write("/path/to/my-library.ifc")
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    definition_list = [e._handle for e in definitions]
    handle = _capi.project_assign_declaration(
        file._handle,
        definition_list,
        relating_context._handle,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    return None
