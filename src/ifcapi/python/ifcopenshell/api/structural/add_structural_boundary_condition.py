# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def add_structural_boundary_condition(
    file,
    name=None,
    connection=None,
    ifc_class="IfcBoundaryNodeCondition",
):
    if connection:
        if connection.is_a("IfcRelConnectsStructuralMember"):
            related_connection = connection.RelatedStructuralConnection
        else:
            related_connection = connection

        if related_connection.is_a("IfcStructuralPointConnection"):
            boundary_class = "IfcBoundaryNodeCondition"
        elif related_connection.is_a("IfcStructuralCurveConnection"):
            boundary_class = "IfcBoundaryEdgeCondition"
        elif related_connection.is_a("IfcStructuralSurfaceConnection"):
            boundary_class = "IfcBoundaryFaceCondition"
        else:
            boundary_class = ifc_class

        condition = file.create_entity(boundary_class, Name=name)
        connection.AppliedCondition = condition
        return condition
    else:
        return file.create_entity(ifc_class, Name=name)
