# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_curve_segment(layout, segment):
    return _capi.call_handle(
        layout.file,
        "alignment_get_curve_segment",
        _capi.instance_handle(layout),
        _capi.instance_handle(segment),
        nullable=True,
    )
