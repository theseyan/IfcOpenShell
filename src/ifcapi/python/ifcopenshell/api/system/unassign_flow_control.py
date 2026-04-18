# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_flow_control(file, relating_flow_element, related_flow_control):
    if not related_flow_control.AssignedToFlowElement:
        return
    assignment = related_flow_control.AssignedToFlowElement[0]
    if assignment.RelatingFlowElement != relating_flow_element:
        return
    if len(assignment.RelatedControlElements) == 1:
        history = assignment.OwnerHistory
        file.remove(assignment)
        if history:
            ifcopenshell.util.element.remove_deep2(file, history)
        return
    related_flow_controls = list(assignment.RelatedControlElements)
    related_flow_controls.remove(related_flow_control)
    assignment.RelatedControlElements = related_flow_controls
    ifcopenshell.api.owner.update_owner_history(file, element=assignment)
