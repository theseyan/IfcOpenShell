# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def add_vertical_layout(file, parent_alignment):
    return _capi.call_handle(
        file,
        "alignment_add_vertical_layout",
        _capi.file_handle(file),
        _capi.instance_handle(parent_alignment),
    )
