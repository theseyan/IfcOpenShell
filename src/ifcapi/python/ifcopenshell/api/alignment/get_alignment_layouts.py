# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_alignment_layouts(alignment):
    return _capi.call_handle_list(
        alignment.file,
        "alignment_get_alignment_layouts",
        _capi.instance_handle(alignment),
    )
