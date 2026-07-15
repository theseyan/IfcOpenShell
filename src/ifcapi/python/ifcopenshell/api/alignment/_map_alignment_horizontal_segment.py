# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def _map_alignment_horizontal_segment(file, segment):
    mapped = _capi.call_handle_list(
        file,
        "alignment_map_segment",
        _capi.file_handle(file),
        {"segment": _capi.instance_handle(segment)},
    )
    return (mapped[0], mapped[1] if len(mapped) > 1 else None)
