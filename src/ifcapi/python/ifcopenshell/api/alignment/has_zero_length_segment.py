# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def has_zero_length_segment(layout):
    return bool(
        _capi.call_value(
            "alignment_has_zero_length_segment", _capi.instance_handle(layout)
        )
    )
