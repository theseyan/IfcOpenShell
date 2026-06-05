# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
from ifcopenshell.api.geometry import _capi


def map_representation(
    file: ifcopenshell.file, representation: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    return _capi.call_handle(
        file,
        "geometry_map_representation",
        _capi.file_handle(file),
        _capi.instance_handle(representation),
    )
