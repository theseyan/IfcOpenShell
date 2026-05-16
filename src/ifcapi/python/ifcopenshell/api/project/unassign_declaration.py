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
from ifcopenshell import _generated_capi
from ifcopenshell.api.project import _capi


def unassign_declaration(
    file: ifcopenshell.file,
    definitions: list[ifcopenshell.entity_instance],
    relating_context: ifcopenshell.entity_instance,
) -> None:
    """Unassigns a list of objects from a project or project library

    Typically used to remove an asset from a project library.

    :param definitions: The list of objects you want to undeclare.
        Typically a list of assets.
    :param relating_context: The IfcProject, or more commonly the
        IfcProjectLibrary that you want the object to no longer be part of.
    :return: None

    Example:

    .. code:: python

        # Programmatically generate a library. You could do this visually too.
        library = ifcopenshell.api.project.create_file()
        root = ifcopenshell.api.root.create_entity(library, ifc_class="IfcProject", name="Demo Library")
        context = ifcopenshell.api.root.create_entity(library,
            ifc_class="IfcProjectLibrary", name="Demo Library")

        # It's necessary to say our library is part of our project.
        ifcopenshell.api.project.assign_declaration(library, definitions=[context], relating_context=root)

        # Remove the library from our project
        ifcopenshell.api.project.unassign_declaration(library, definitions=[context], relating_context=root)
    """
    lib = _capi.get_lib()
    _, user, application = _capi.owner_context(file)
    definition_list = _capi.instance_list(definitions)
    _generated_capi.status_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_project_unassign_declaration(
            _capi.file_handle(file),
            definition_list,
            _capi.instance_handle(relating_context),
            _capi.instance_handle(user),
            _capi.instance_handle(application),
        ),
        "Failed to unassign declaration",
    )
