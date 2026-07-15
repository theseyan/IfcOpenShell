# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_mapped_segments(layout_segment):
    return _capi.call_handle_list(
        layout_segment.file,
        "alignment_get_mapped_segments",
        _capi.instance_handle(layout_segment),
    )
