# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_by_pi_method(
    file, name, hpoints, radii, vpoints=None, lengths=None, start_station=0.0
):
    owner = _capi.owner_options(file)
    options = {
        "name": name,
        "horizontal_points": _capi.point_list(hpoints),
        "radii": list(radii),
        "vertical_points": _capi.point_list(vpoints or ()),
        "vertical_lengths": list(lengths or ()),
        "start_station": start_station,
        **owner.values,
    }
    return _capi.call_handle(
        file, "alignment_create_by_pi_method", _capi.file_handle(file), options
    )
