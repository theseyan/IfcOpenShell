# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def update_fallback_position(file, lp):
    _capi.call_status(
        "alignment_update_fallback_position",
        _capi.file_handle(file),
        _capi.instance_handle(lp),
    )
