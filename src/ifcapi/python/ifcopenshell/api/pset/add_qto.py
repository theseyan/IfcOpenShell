# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new IfcElementQuantity to a product."""

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell.api.pset._capi import get_lib, raise_last_error


def add_qto(file, product, name):
    """Add a new IfcElementQuantity to *product* and return it."""
    lib = get_lib()

    owner_history = None
    try:
        owner_history = ifcopenshell.api.owner.create_owner_history(file)
    except Exception:
        owner_history = None

    oh_handle = owner_history._handle if owner_history is not None else 0
    handle = lib.ifcopenshell_api_pset_add_qto(
        file._ptr, product._handle, name.encode("utf-8"), oh_handle
    )
    if not handle:
        raise_last_error(f"Failed to add quantity set '{name}'")
    return ifcopenshell.entity_instance(file, handle)
