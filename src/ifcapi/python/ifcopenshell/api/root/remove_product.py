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
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def remove_product(file: ifcopenshell.file, product: ifcopenshell.entity_instance) -> None:
    """Removes a product

    This is effectively a smart delete function that not only removes a
    product, but also all of its relationships. It is always recommended to
    use this function to prevent orphaned data in your IFC model.

    This is intended to be used for removing:

    - IfcAnnotation
    - IfcElement
    - IfcElementType
    - IfcSpatialElement
    - IfcSpatialElementType

    For example, geometric representations are removed. Placement
    coordinates are also removed. Properties are removed. Material, type,
    containment, aggregation, and nesting relationships are removed (but
    naturally, the materials, types, containers, etc themselves remain).

    :param product: The element to remove.
    :return: None

    Example:

    .. code:: python

        # We have a wall.
        wall = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # No we don't.
        ifcopenshell.api.root.remove_product(model, product=wall)
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    _capi.root_remove_product(
        file._handle,
        product._handle,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
