# SPDX-License-Identifier: LGPL-3.0-or-later
import numpy as np

from ifcopenshell.api.alignment import _capi


def create_layout_segment(file, layout, design_parameters):
    result = _capi.call_value(
        "alignment_create_layout_segment",
        _capi.file_handle(file),
        _capi.instance_handle(layout),
        _capi.instance_handle(design_parameters),
    )
    return np.array(result.endpoint).reshape((4, 4)) if result.has_endpoint else None
