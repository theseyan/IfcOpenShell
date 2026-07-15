# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_segment_representations(file, alignment):
    _capi.call_status(
        "alignment_create_segment_representations",
        _capi.file_handle(file),
        _capi.instance_handle(alignment),
    )
