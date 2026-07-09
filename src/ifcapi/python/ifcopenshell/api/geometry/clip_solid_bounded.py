# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

from typing import Optional, Sequence

import ifcopenshell
from . import _capi


def clip_solid_bounded(
    file: ifcopenshell.file,
    item: ifcopenshell.entity_instance,
    location: Sequence[float],
    normal: Sequence[float],
    boundary_points: Sequence[Sequence[float]],
    boundary_position: Sequence[float] = (0.0, 0.0, 0.0),
    element: Optional[ifcopenshell.entity_instance] = None,
) -> ifcopenshell.entity_instance:
    """Clip a solid with a polygonally bounded half-space, returning an IfcBooleanClippingResult."""
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "geometry_clip_solid_bounded",
        _capi.file_handle(file),
        {
            "item": _capi.instance_handle(item),
            "location": location,
            "normal": normal,
            "boundary_points": boundary_points,
            "boundary_position": boundary_position,
            "element": _capi.instance_handle(element),
            "owner_history": _capi.instance_handle(owner_history),
            "user": _capi.instance_handle(user),
            "application": _capi.instance_handle(application),
        },
    )
