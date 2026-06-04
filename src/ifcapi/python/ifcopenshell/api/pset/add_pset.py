# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new property set to a product."""

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell.api.pset._capi import build_props, free_props


def add_pset(file, product, name, ifc2x3_subclass=None):
    """Add a new property set to *product* and return it."""
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    props_handle = build_props({})
    try:
        handle = _capi.pset_add_pset(
            file._handle,
            product._handle,
            name,
            owner_history._handle if owner_history is not None else None,
            user._handle if user is not None else None,
            application._handle if application is not None else None,
            ifc2x3_subclass,
        )
    finally:
        free_props(props_handle)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or f"Failed to add property set '{name}'")
