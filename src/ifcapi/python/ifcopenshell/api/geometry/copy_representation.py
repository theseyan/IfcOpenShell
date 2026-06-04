# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

from typing import Optional

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi


def copy_representation(
    file: ifcopenshell.file,
    source: ifcopenshell.entity_instance,
    target: ifcopenshell.entity_instance,
    context_identifier: str = "Body",
) -> Optional[ifcopenshell.entity_instance]:
    """Copy a geometric representation from one element to another."""
    handle = _capi.ifcopenshell_ifcapi_geometry_copy_representation(
        file._handle,
        source._handle,
        target._handle,
        context_identifier,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    return None
