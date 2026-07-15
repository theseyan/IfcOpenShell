# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_axis_subcontext(file):
    return _capi.call_handle(
        file, "alignment_get_axis_subcontext", _capi.file_handle(file)
    )
