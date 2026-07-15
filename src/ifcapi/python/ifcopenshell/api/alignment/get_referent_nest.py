# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_referent_nest(file, alignment):
    return _capi.call_handle(
        file,
        "alignment_get_referent_nest",
        _capi.instance_handle(alignment),
        nullable=True,
    )
