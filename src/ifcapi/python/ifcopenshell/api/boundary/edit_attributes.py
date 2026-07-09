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
from typing import Optional

import ifcopenshell
from ifcopenshell.api.boundary import _capi


def edit_attributes(
    file: ifcopenshell.file,
    entity: ifcopenshell.entity_instance,
    relating_space: ifcopenshell.entity_instance,
    related_building_element: ifcopenshell.entity_instance,
    parent_boundary: Optional[ifcopenshell.entity_instance] = None,
    corresponding_boundary: Optional[ifcopenshell.entity_instance] = None,
    physical_or_virtual: str = "NOTDEFINED",
    internal_or_external: str = "NOTDEFINED",
) -> None:
    """Modify the relationships of a space boundary relationship

    :param entity: The IfcRelSpaceBoundary to modify
    :param relating_space: The IfcSpace or IfcExternalSpatialElement that
        the space boundary is related to.
    :param related_building_element: The IfcElement that defines the
        boundary, typically an IfcWall.
    :param parent_boundary: A parent IfcRelSpaceBoundary, only provided if
        this is an inner boundary. This can apply to 1st and 2nd level
        boundaries.
    :param corresponding_boundary: The other IfcRelSpaceBoundary on the
        other side of the related element. The pair together represents a
        thermal boundary. This only applies to 2nd level boundaries.
    :param physical_or_virtual: IfcPhysicalOrVirtualEnum value: "PHYSICAL",
        "VIRTUAL", or "NOTDEFINED".
    :param internal_or_external: IfcInternalOrExternalEnum value:
        "INTERNAL", "EXTERNAL", "EXTERNAL_EARTH", "EXTERNAL_WATER",
        "EXTERNAL_FIRE", or "NOTDEFINED".
    :return: None
    """
    _capi.call_status(
        "boundary_edit_attributes",
        "Failed to edit boundary attributes",
        _capi.instance_handle(entity),
        {
            "relating_space": _capi.instance_handle(relating_space),
            "related_building_element": _capi.instance_handle(related_building_element),
            "parent_boundary": _capi.instance_handle(parent_boundary),
            "corresponding_boundary": _capi.instance_handle(corresponding_boundary),
            "physical_or_virtual": physical_or_virtual,
            "internal_or_external": internal_or_external,
        },
    )
