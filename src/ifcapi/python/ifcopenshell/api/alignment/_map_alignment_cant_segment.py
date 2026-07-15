# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def _map_alignment_cant_segment(file, segment, rail_head_distance):
    mapped = _capi.call_handle_list(
        file,
        "alignment_map_segment",
        _capi.file_handle(file),
        {
            "segment": _capi.instance_handle(segment),
            "rail_head_distance": rail_head_distance,
        },
    )
    return (mapped[0], mapped[1] if len(mapped) > 1 else None)
