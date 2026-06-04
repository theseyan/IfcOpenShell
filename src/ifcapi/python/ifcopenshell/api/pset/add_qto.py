# SPDX-License-Identifier: LGPL-3.0-or-later

"""Create and assign a new IfcElementQuantity to a product."""

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def add_qto(file, product, name):
    """Add a new IfcElementQuantity to *product* and return it."""
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    handle = _capi.pset_add_qto(
        file._handle,
        product._handle,
        name,
        owner_history._handle if owner_history is not None else None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    if _capi.last_error_kind() == _capi.IFCOPENSHELL_ERROR_NONE:
        return None
    raise RuntimeError(_capi.last_error_message() or f"Failed to add quantity set '{name}'")
