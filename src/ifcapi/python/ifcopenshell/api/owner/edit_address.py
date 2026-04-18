# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_address(file, address=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(address, name, value)
