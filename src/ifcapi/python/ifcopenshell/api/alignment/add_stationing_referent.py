# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def add_stationing_referent(
    file, alignment, distance_along, station, name, positioned_product
):
    owner = _capi.owner_options(file)
    options = {
        "alignment": _capi.instance_handle(alignment),
        "distance_along": distance_along,
        "station": station,
        "name": name,
        "positioned_product": _capi.instance_handle(positioned_product),
        **owner.values,
    }
    return _capi.call_handle(
        file, "alignment_add_stationing_referent", _capi.file_handle(file), options
    )
