# SPDX-License-Identifier: LGPL-3.0-or-later
import numpy as np

from ifcopenshell.api.alignment import _capi


def _add_segment_to_layout(file, layout, layout_segment):
    result = _capi.call_value(
        "alignment_add_segment_to_layout",
        _capi.file_handle(file),
        _capi.instance_handle(layout),
        _capi.instance_handle(layout_segment),
    )
    return np.array(result.endpoint).reshape((4, 4)) if result.has_endpoint else None
