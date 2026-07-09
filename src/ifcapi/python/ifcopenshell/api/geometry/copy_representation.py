# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

from typing import Optional

import ifcopenshell
from ifcopenshell.api.geometry import _capi


def copy_representation(
    file: ifcopenshell.file,
    source: ifcopenshell.entity_instance,
    target: ifcopenshell.entity_instance,
    context_identifier: str = "Body",
) -> Optional[ifcopenshell.entity_instance]:
    """Copy a geometric representation from one element to another."""
    return _capi.call_handle(
        file,
        "geometry_copy_representation",
        _capi.file_handle(file),
        {
            "source": _capi.instance_handle(source),
            "target": _capi.instance_handle(target),
            "context_identifier": context_identifier,
        },
        nullable=True,
    )
