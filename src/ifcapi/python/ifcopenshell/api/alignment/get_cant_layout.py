# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_cant_layout(alignment):
    return _capi.call_handle(
        alignment.file,
        "alignment_get_cant_layout",
        _capi.instance_handle(alignment),
        nullable=True,
    )
