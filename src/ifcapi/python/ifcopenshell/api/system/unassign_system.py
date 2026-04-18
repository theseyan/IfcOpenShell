# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""system.unassign_system — unassign elements from a system."""

import ifcopenshell
import ifcopenshell.api.group


def unassign_system(file, products, system):
    ifcopenshell.api.group.unassign_group(file, products=products, group=system)
