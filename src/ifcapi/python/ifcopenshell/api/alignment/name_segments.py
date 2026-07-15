# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def name_segments(prefix, layout):
    _capi.call_status("alignment_name_segments", prefix, _capi.instance_handle(layout))
