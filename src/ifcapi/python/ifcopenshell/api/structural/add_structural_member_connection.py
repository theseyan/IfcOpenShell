# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def add_structural_member_connection(
    file,
    relating_structural_member=None,
    related_structural_connection=None,
):
    for connection in related_structural_connection.ConnectsStructuralMembers or []:
        if connection.RelatingStructuralMember == relating_structural_member:
            return connection
    rel = ifcopenshell.api.root.create_entity(file, ifc_class="IfcRelConnectsStructuralMember")
    rel.RelatingStructuralMember = relating_structural_member
    rel.RelatedStructuralConnection = related_structural_connection
    return rel
