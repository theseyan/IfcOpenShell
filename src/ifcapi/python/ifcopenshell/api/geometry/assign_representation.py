# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
from ifcopenshell.api.geometry import _capi


def assign_representation(
    file: ifcopenshell.file, product: ifcopenshell.entity_instance, representation: ifcopenshell.entity_instance
) -> None:
    lib = _capi.get_lib()
    changed_product = _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_geometry_assign_representation,
        _capi.file_handle(file),
        _capi.instance_handle(product),
        _capi.instance_handle(representation),
    )
    if changed_product:
        ifcopenshell.api.owner.update_owner_history(file, element=changed_product)
