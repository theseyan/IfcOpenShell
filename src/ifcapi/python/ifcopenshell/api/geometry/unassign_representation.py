# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
from ifcopenshell.api.geometry import _capi


def unassign_representation(
    file: ifcopenshell.file, product: ifcopenshell.entity_instance, representation: ifcopenshell.entity_instance
) -> None:
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_geometry_unassign_representation,
        _capi.file_handle(file),
        _capi.instance_handle(product),
        _capi.instance_handle(representation),
    )
