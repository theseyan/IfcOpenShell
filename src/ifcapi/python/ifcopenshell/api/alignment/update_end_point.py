# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def update_end_point(file, curve):
    _capi.call_status(
        "alignment_update_end_point",
        _capi.file_handle(file),
        _capi.instance_handle(curve),
    )
