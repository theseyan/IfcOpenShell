# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def add_zero_length_segment(file, layout):
    return bool(
        _capi.call_value(
            "alignment_add_zero_length_segment",
            _capi.file_handle(file),
            _capi.instance_handle(layout),
        )
    )
