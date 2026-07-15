# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_layout(segment):
    return _capi.call_handle(
        segment.file,
        "alignment_get_layout",
        _capi.instance_handle(segment),
        nullable=True,
    )
