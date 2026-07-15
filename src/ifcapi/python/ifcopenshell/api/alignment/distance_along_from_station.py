# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def distance_along_from_station(file, alignment, station):
    return _capi.call_value(
        "alignment_distance_along_from_station",
        _capi.file_handle(file),
        _capi.instance_handle(alignment),
        station,
    )
