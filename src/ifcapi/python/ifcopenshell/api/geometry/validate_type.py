# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Union

import ifcopenshell
from . import _capi


def validate_type(
    file: ifcopenshell.file,
    representation: ifcopenshell.entity_instance,
    preferred_item: Union[ifcopenshell.entity_instance, None] = None,
) -> bool:
    """Validates the RepresentationType of an IfcShapeRepresentation."""
    return _capi.bool_result(
        "geometry_validate_type",
        _capi.file_handle(file),
        _capi.instance_handle(representation),
        {"preferred_item": _capi.instance_handle(preferred_item)},
    )
