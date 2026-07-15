# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_child_alignments(alignment):
    return _capi.call_handle_list(
        alignment.file,
        "alignment_get_child_alignments",
        _capi.instance_handle(alignment),
    )
