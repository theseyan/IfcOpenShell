# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi

_horizontal_callback = None
_vertical_callback = None
_cant_callback = None


def register_referent_name_callback(horizontal=None, vertical=None, cant=None):
    global _horizontal_callback, _vertical_callback, _cant_callback
    _horizontal_callback = horizontal
    _vertical_callback = vertical
    _cant_callback = cant


def _get_segment_start_point_label(prev_segment, segment):
    sample = segment if segment is not None else prev_segment
    parameters = sample.DesignParameters
    if parameters.is_a("IfcAlignmentHorizontalSegment"):
        callback = _horizontal_callback
    elif parameters.is_a("IfcAlignmentVerticalSegment"):
        callback = _vertical_callback
    elif parameters.is_a("IfcAlignmentCantSegment"):
        callback = _cant_callback
    else:
        raise TypeError(f"Unexpected design parameter type: {parameters.is_a()}")
    if callback is not None:
        return callback(prev_segment, segment)
    return _capi.call_value(
        "alignment_default_referent_label",
        _capi.instance_handle(prev_segment),
        _capi.instance_handle(segment),
    )
