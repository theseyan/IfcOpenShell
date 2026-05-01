# SPDX-License-Identifier: LGPL-3.0-or-later

"""
Native implementation of ifcopenshell.api.root functions via libIfcApi.
"""

import ifcopenshell
import ifcopenshell.api.owner
import ctypes
from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr

from . import _get_lib, _check_error, _file_pointer


def create_entity(file, ifc_class="IfcBuildingElementProxy", predefined_type=None, name=None):
    """Drop-in replacement for ifcopenshell.api.root.create_entity using native C++ backend."""
    lib = _get_lib()

    # The Python create_owner_history is still needed until the owner module is ported.
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    owner_history_handle = _generated_instance_handle_ptr(owner_history._handle) if owner_history else None
    file_handle = ifcopenshell._ifc_file_handle_ptr(_file_pointer(file))

    ifc_class_b = ifc_class.encode("utf-8") if isinstance(ifc_class, str) else ifc_class
    pt_b = predefined_type.encode("utf-8") if predefined_type else None
    name_b = name.encode("utf-8") if name else None

    handle = _generated_capi.call_handle(
        lib,
        lib.ifcopenshell_ifcapi_root_create_entity,
        file_handle,
        ifc_class_b,
        pt_b,
        name_b,
        owner_history_handle,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
        handle_pointer_type=ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
    )
    if not handle:
        _check_error()
        raise RuntimeError("ifcopenshell_ifcapi_root_create_entity returned NULL with no error")

    return ifcopenshell.entity_instance(file, handle)
