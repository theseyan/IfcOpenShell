# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def new():
    """Generate a new random IFC GlobalId (22 characters)."""
    lib = ifcopenshell._get_lib()
    result = lib.ifcopenshell_guid_new()
    return result.decode("utf-8") if result else ""
