# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def remove_address(file, address=None):
    for inverse in file.get_inverse(address):
        if inverse.is_a() in ("IfcOrganization", "IfcPerson"):
            if inverse.Addresses == (address,):
                inverse.Addresses = None
    file.remove(address)
