# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_alignment_layout_nest(alignment):
    return _capi.call_handle(
        alignment.file,
        "alignment_get_alignment_layout_nest",
        _capi.instance_handle(alignment),
        nullable=True,
    )
