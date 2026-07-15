# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def get_basis_curve(alignment):
    return _capi.call_handle(
        alignment.file,
        "alignment_get_basis_curve",
        _capi.instance_handle(alignment),
        nullable=True,
    )
