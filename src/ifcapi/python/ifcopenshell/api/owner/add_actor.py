# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.root


def add_actor(file, actor=None, ifc_class="IfcActor"):
    ifc_class = ifc_class or "IfcActor"
    actor_ = ifcopenshell.api.root.create_entity(file, ifc_class=ifc_class)
    actor_.TheActor = actor
    return actor_
