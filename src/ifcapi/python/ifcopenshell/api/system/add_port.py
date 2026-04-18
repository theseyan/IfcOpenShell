# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.root
import ifcopenshell.api.system


def add_port(file, element=None):
    port = ifcopenshell.api.root.create_entity(file, ifc_class="IfcDistributionPort")
    if element:
        ifcopenshell.api.system.assign_port(file, element=element, port=port)
    return port
