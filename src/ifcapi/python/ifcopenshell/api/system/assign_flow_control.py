# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid


def assign_flow_control(file, relating_flow_element, related_flow_control):
    if related_flow_control.AssignedToFlowElement:
        assignment = related_flow_control.AssignedToFlowElement[0]
        if assignment.RelatingFlowElement == relating_flow_element:
            return assignment
        return

    if relating_flow_element.HasControlElements:
        assignment = relating_flow_element.HasControlElements[0]
        if related_flow_control in assignment.RelatedControlElements:
            return assignment

        related_flow_controls = set(assignment.RelatedControlElements)
        related_flow_controls.add(related_flow_control)
        assignment.RelatedControlElements = list(related_flow_controls)
        ifcopenshell.api.owner.update_owner_history(file, element=assignment)
        return assignment

    assignment = file.create_entity(
        "IfcRelFlowControlElements",
        **{
            "GlobalId": ifcopenshell.guid.new(),
            "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
            "RelatedControlElements": [related_flow_control],
            "RelatingFlowElement": relating_flow_element,
        },
    )
    return assignment
