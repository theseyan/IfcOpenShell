# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def layout_horizontal_alignment_by_pi_method(file, layout, hpoints, radii):
    _capi.call_status(
        "alignment_layout_horizontal_by_pi_method",
        _capi.file_handle(file),
        _capi.instance_handle(layout),
        _capi.point_list(hpoints),
        list(radii),
    )
