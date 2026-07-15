# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_curve_segment_transition_code(segment, next_segment, position_tolerance=0.001):
    return _capi.call_value(
        "alignment_get_curve_segment_transition_code",
        _capi.instance_handle(segment),
        _capi.instance_handle(next_segment),
        position_tolerance,
    )
