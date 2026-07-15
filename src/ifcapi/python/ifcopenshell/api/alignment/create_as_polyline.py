# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_as_polyline(file, name, points, start_station=0.0):
    owner = _capi.owner_options(file)
    options = {
        "name": name,
        "points": _capi.instance_list(points),
        "start_station": start_station,
        **owner.values,
    }
    return _capi.call_handle(
        file, "alignment_create_as_polyline", _capi.file_handle(file), options
    )
