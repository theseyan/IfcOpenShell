# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def _update_curve_segment_transition_code(prev_segment, segment):
    _capi.call_status(
        "alignment_update_curve_segment_transition_code",
        _capi.instance_handle(prev_segment),
        _capi.instance_handle(segment),
        0.001,
    )
