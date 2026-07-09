# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2025 Dion Moult <dion@thinkmoult.com>
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
from ifcopenshell.api.geometry import _capi


def add_shape_aspect(
    file: ifcopenshell.file,
    name: str,
    items: list[ifcopenshell.entity_instance],
    representation: ifcopenshell.entity_instance,
    part_of_product: ifcopenshell.entity_instance,
    description: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    """Adds a shape aspect to items that are part of a representation and product

    Existing shape aspects will be reused where possible. If the items already
    belong to another shape aspect with a different name, this relationship
    will be purged.

    Warning: it is not possible to add a shape aspect to types (i.e.
    IfcRepresentationMap) in IFC2X3.

    :param name: The name of the shape aspect. This is case sensitive.
    :param items: IfcRepresentationItems that will be assigned to this aspect.
    :param representation: The IfcShapeRepresentation that the items are in.
    :param part_of_product: The IfcRepresentationMap or
        IfcProductDefinitionShape that the representation is in.
    :param description: A description to set for the shape aspect. It's usually
        not necessary.
    :return: The IfcShapeAspect
    """
    return _capi.call_handle(
        file,
        "geometry_add_shape_aspect",
        _capi.file_handle(file),
        {
            "name": name,
            "items": _capi.instance_list(items),
            "representation": _capi.instance_handle(representation),
            "part_of_product": _capi.instance_handle(part_of_product),
            "description": description,
        },
    )
