# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_layout_segments(layout):
    return _capi.call_handle_list(
        layout.file, "alignment_get_layout_segments", _capi.instance_handle(layout)
    )
