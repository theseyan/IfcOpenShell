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
from ifcopenshell.api import _relationship_capi


def unassign_object(file: ifcopenshell.file, related_objects: list[ifcopenshell.entity_instance]) -> None:
    """Unassigns related_objects from their nests.

    An object (the whole within a decomposition) is Nested by zero or one more smaller objects.
    This function will remove this nesting relationship.

    If the object is not part of a nesting relationship, nothing will happen.

    :param related_objects: The list of children of the nesting relationship,
        typically IfcElements.
    :return: None

    Example:

    .. code:: python

        task = ifcopenshell.api.root.create_entity(model, ifc_class="IfcTasks")
        subtask1 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcTask")
        subtask2 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcTask")
        ifcopenshell.api.nest.assign_object(model, related_objects=[subtask1], relating_object=task)
        ifcopenshell.api.nest.assign_object(model, related_objects=[subtask2], relating_object=task)
        # nothing is returned
        rel = ifcopenshell.api.nest.unassign_object(model, related_objects=[subtask1])
        # nothing is returned, relationship is removed
        ifcopenshell.api.nest.unassign_object(model, related_objects=[subtask2])
    """

    object_list = _relationship_capi.instance_list(related_objects)
    user, application = _relationship_capi.owner_user_application(file)
    _relationship_capi.call_status(
        "nest_unassign_object",
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_list_ptr(object_list),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
