# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_by_pi_method(
    file, name, hpoints, radii, vpoints=None, lengths=None, start_station=0.0
):
    owner = _capi.owner_options(file)
    options = {
        "name": name,
        "horizontal": _capi.horizontal_pi_layout(hpoints, radii),
        "start_station": start_station,
        **owner.values,
    }
    if vpoints and lengths:
        options["vertical"] = _capi.vertical_pi_layout(vpoints, lengths)
    return _capi.call_handle(
        file, "alignment_create_by_pi_method", _capi.file_handle(file), options
    )
