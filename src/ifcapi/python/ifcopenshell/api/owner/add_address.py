# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_address(file, assigned_object=None, ifc_class="IfcPostalAddress"):
    address = file.create_entity(ifc_class, "OFFICE")
    addresses = list(assigned_object.Addresses) if assigned_object.Addresses else []
    addresses.append(address)
    assigned_object.Addresses = addresses
    return address
