# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new IfcElementQuantity to a product."""

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell._capi_utils import owner_context, raise_last_error


def add_qto(file, product, name):
    """Add a new IfcElementQuantity to *product* and return it."""
    owner_history, user, application = owner_context(file)
    handle = _capi.pset_add_qto(
        file._handle,
        {
            "product": product._handle,
            "name": name,
            "owner_history": owner_history._handle if owner_history else None,
            "user": user._handle if user else None,
            "application": application._handle if application else None,
        },
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise_last_error(f"Failed to add quantity set '{name}'")
