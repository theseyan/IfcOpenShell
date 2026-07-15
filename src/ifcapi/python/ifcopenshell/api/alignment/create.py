# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create(
    file,
    name,
    include_vertical=False,
    include_cant=False,
    include_geometry=True,
    start_station=0.0,
):
    owner = _capi.owner_options(file)
    options = {
        "name": name,
        "include_vertical": include_vertical,
        "include_cant": include_cant,
        "include_geometry": include_geometry,
        "start_station": start_station,
        **owner.values,
    }
    return _capi.call_handle(file, "alignment_create", _capi.file_handle(file), options)
