# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new IfcElementQuantity to a product."""

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.api.pset._capi import get_lib, instance_handle, last_error_kind, owner_context, raise_last_error
from ifcopenshell.entity_instance import _generated_instance_handle_ptr


def add_qto(file, product, name):
    """Add a new IfcElementQuantity to *product* and return it."""
    lib = get_lib()
    _generated_capi.bind(
        lib,
        names=("ifcopenshell_ifcapi_pset_add_qto", "ifcopenshell_ifc_instance_destroy"),
    )

    owner_history, user, application = owner_context(file)
    handle = _generated_capi.call_handle(
        lib,
        lib.ifcopenshell_ifcapi_pset_add_qto,
        _generated_instance_handle_ptr(file._ptr),
        _generated_instance_handle_ptr(product._handle),
        _generated_capi.encode_string(name),
        instance_handle(owner_history),
        instance_handle(user),
        instance_handle(application),
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    if not handle:
        if last_error_kind() == _generated_capi.IFCOPENSHELL_ERROR_NONE:
            return None
        raise_last_error(f"Failed to add quantity set '{name}'")
    return ifcopenshell.entity_instance(file, handle)
