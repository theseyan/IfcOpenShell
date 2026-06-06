# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new property set to a product."""

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell._capi_utils import owner_context, raise_last_error
from ifcopenshell.api.pset._capi import build_props, free_props


def add_pset(file, product, name, ifc2x3_subclass=None):
    """Add a new property set to *product* and return it."""
    owner_history, user, application = owner_context(file)
    props_handle = build_props({})
    try:
        handle = _capi.pset_add_pset(
            file._handle,
            product._handle,
            name,
            owner_history._handle if owner_history else None,
            user._handle if user else None,
            application._handle if application else None,
            ifc2x3_subclass,
        )
    finally:
        free_props(props_handle)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise_last_error(f"Failed to add property set '{name}'")
