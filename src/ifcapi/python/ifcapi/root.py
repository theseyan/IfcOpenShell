# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""
Native implementation of ifcopenshell.api.root functions via libIfcApi.
"""

import ifcopenshell
import ifcopenshell.api.owner

from . import _get_lib, _check_error, _file_pointer


def create_entity(file, ifc_class="IfcBuildingElementProxy", predefined_type=None, name=None):
    """Drop-in replacement for ifcopenshell.api.root.create_entity using native C++ backend."""
    lib = _get_lib()

    # The Python create_owner_history is still needed until the owner module is ported.
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    oh_id = owner_history.id() if owner_history else 0

    file_ptr = _file_pointer(file)

    ifc_class_b = ifc_class.encode("utf-8") if isinstance(ifc_class, str) else ifc_class
    pt_b = predefined_type.encode("utf-8") if predefined_type else None
    name_b = name.encode("utf-8") if name else None

    step_id = lib.ifcopenshell_root_create_entity(file_ptr, ifc_class_b, pt_b, name_b, oh_id)
    if step_id == 0:
        _check_error()
        raise RuntimeError("ifcopenshell_root_create_entity returned 0 with no error")

    return file.by_id(step_id)
