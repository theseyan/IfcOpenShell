# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_alignment_start_station(file, alignment):
    return _capi.call_value(
        "alignment_get_alignment_start_station",
        _capi.file_handle(file),
        _capi.instance_handle(alignment),
    )
