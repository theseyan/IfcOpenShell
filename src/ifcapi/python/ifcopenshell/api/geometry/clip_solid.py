# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

from typing import Optional, Sequence

import ifcopenshell
from . import _capi


def clip_solid(
    file: ifcopenshell.file,
    item: ifcopenshell.entity_instance,
    location: Sequence[float],
    normal: Sequence[float],
    element: Optional[ifcopenshell.entity_instance] = None,
) -> ifcopenshell.entity_instance:
    """Clip a solid with a half-space plane, returning an IfcBooleanClippingResult."""
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "geometry_clip_solid",
        _capi.file_handle(file),
        _capi.instance_handle(item),
        _capi.double_list(location),
        _capi.double_list(normal),
        _capi.instance_handle(element),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
