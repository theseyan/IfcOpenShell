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
import ctypes

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


_BOUND = False


def _bind() -> ctypes.CDLL:
    global _BOUND
    lib = ifcopenshell._get_lib()
    if not _BOUND:
        _generated_capi.bind(
            lib,
            names=(
                "ifcopenshell_ifcapi_root_create_entity",
                "ifcopenshell_ifc_instance_destroy",
                "ifcopenshell_last_error_kind",
                "ifcopenshell_last_error_message",
            ),
        )
        _BOUND = True
    return lib


def _encode_optional(value: str | None):
    return _generated_capi.encode_string(value) if value else None


def create_entity(
    file: ifcopenshell.file,
    ifc_class: str = "IfcBuildingElementProxy",
    predefined_type: Optional[str] = None,
    name: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    """Create a new rooted product

    This is a critical function used to create almost any rooted product or
    product type. If you want to create walls, spaces, buildings, wall
    types, and so on, use this function.

    Just specify the class you want to create, as well as the predefined
    type and name. It will handle the storage of the predefined type and
    check whether the predefined type is built-in or custom. It will also
    generate a valid GlobalId and store ownership history. It will also
    handle some edge cases for default validity where users might forget to
    populate some mandatory attributes. For example, doors must define an
    operation type but many people forget.

    :param ifc_class: Any rooted IFC class.
    :param predefined_type: Any built-in or user-defined predefined type that
        is applicable to that IFC class. For user-defined predefined types
        just enter in any value and the API will handle it automatically.
    :param name: The name of the new element.
    :return: The newly created element based on the specified IFC class.

    Example:

    .. code:: python

        # We have a project.
        ifcopenshell.api.root.create_entity(model, ifc_class="IfcProject")

        # We have a building.
        ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuilding")

        # We have a wall.
        ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # We have a wall type.
        ifcopenshell.api.root.create_entity(model, ifc_class="IfcWallType")
    """
    lib = _bind()
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    handle = _generated_capi.call_handle_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_root_create_entity,
        ifcopenshell.get_log() or "ifcopenshell_ifcapi_root_create_entity",
        ifcopenshell._ifc_file_handle_ptr(file._ptr),
        _generated_capi.encode_string(ifc_class),
        _encode_optional(predefined_type),
        _encode_optional(name),
        _generated_instance_handle_ptr(owner_history._handle) if owner_history is not None else None,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
        handle_pointer_type=ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    _generated_capi.raise_last_error(lib, "Failed to create entity")
