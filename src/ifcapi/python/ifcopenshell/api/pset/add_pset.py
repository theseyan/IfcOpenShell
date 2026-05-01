# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new property set to a product."""

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell import _generated_capi
from ifcopenshell.api.pset._capi import get_lib, raise_last_error
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


def add_pset(file, product, name, ifc2x3_subclass=None):
    """Add a new property set to *product* and return it."""
    lib = get_lib()
    _generated_capi.bind(
        lib,
        names=("ifcopenshell_ifcapi_pset_add_pset", "ifcopenshell_ifc_instance_destroy"),
    )

    owner_history = None
    try:
        owner_history = ifcopenshell.api.owner.create_owner_history(file)
    except Exception:
        owner_history = None

    file_handle = _generated_instance_handle_ptr(file._ptr)
    subclass_arg = _generated_capi.encode_string(ifc2x3_subclass) if ifc2x3_subclass else None
    handle = _generated_capi.call_handle(
        lib,
        lib.ifcopenshell_ifcapi_pset_add_pset,
        file_handle,
        _generated_instance_handle_ptr(product._handle),
        _generated_capi.encode_string(name),
        _generated_instance_handle_ptr(owner_history._handle) if owner_history is not None else None,
        subclass_arg,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    if not handle:
        raise_last_error(f"Failed to add property set '{name}'")
    return ifcopenshell.entity_instance(file, handle)
