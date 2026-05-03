# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

from typing import Optional

import ifcopenshell
from ifcopenshell import _generated_capi
from ifcopenshell.api.geometry import _capi


def copy_representation(
    file: ifcopenshell.file,
    source: ifcopenshell.entity_instance,
    target: ifcopenshell.entity_instance,
    context_identifier: str = "Body",
) -> Optional[ifcopenshell.entity_instance]:
    """Copy a geometric representation from one element to another."""
    lib = _capi.get_lib()
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_geometry_copy_representation,
        _capi.file_handle(file),
        _capi.instance_handle(source),
        _capi.instance_handle(target),
        _generated_capi.encode_string(context_identifier),
        nullable=True,
    )
