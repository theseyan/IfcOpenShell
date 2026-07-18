# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def layout_vertical_alignment_by_pi_method(file, layout, vpoints, lengths):
    _capi.call_status(
        "alignment_layout_vertical_by_pi_method",
        _capi.file_handle(file),
        _capi.instance_handle(layout),
        {"pis": _capi.vertical_pi_layout(vpoints, lengths)},
    )
