# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_representation(file, alignment):
    _capi.call_status(
        "alignment_create_representation",
        _capi.file_handle(file),
        _capi.instance_handle(alignment),
    )
