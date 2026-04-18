# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def add_structural_activity(
    file,
    applied_load=None,
    structural_member=None,
    ifc_class="IfcStructuralPlanarAction",
    predefined_type="CONST",
    global_or_local="GLOBAL_COORDS",
):
    activity = ifcopenshell.api.root.create_entity(
        file,
        ifc_class=ifc_class,
        predefined_type=predefined_type,
    )
    activity.AppliedLoad = applied_load
    activity.GlobalOrLocal = global_or_local

    rel = ifcopenshell.api.root.create_entity(file, ifc_class="IfcRelConnectsStructuralActivity")
    rel.RelatingElement = structural_member
    rel.RelatedStructuralActivity = activity
    return activity
