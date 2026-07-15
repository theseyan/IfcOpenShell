# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_alignment(layout):
    return _capi.call_handle(
        layout.file,
        "alignment_get_alignment",
        _capi.instance_handle(layout),
        nullable=True,
    )
