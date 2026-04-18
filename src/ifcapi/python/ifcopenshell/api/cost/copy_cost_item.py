# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.nest
import ifcopenshell.util.element


def copy_cost_item(file, cost_item=None):
    new_cost_items = []
    _duplicate_cost_item(file, cost_item, new_cost_items)
    return new_cost_items[0] if len(new_cost_items) == 1 else new_cost_items


def _duplicate_cost_item(file, cost_item, new_cost_items):
    new_cost_item = ifcopenshell.util.element.copy_deep(file, cost_item)
    new_cost_items.append(new_cost_item)
    _copy_indirect_attributes(file, cost_item, new_cost_item, new_cost_items)
    return new_cost_item


def _copy_indirect_attributes(file, from_element, to_element, new_cost_items):
    for inverse in file.get_inverse(from_element):
        if inverse.is_a("IfcRelDefinesByProperties"):
            inverse = ifcopenshell.util.element.copy(file, inverse)
            inverse.RelatedObjects = [to_element]
            pset = ifcopenshell.util.element.copy_deep(file, inverse.RelatingPropertyDefinition)
            inverse.RelatingPropertyDefinition = pset
        elif inverse.is_a("IfcRelNests") and inverse.RelatingObject == from_element:
            nested_cost_items = list(inverse.RelatedObjects)
            if nested_cost_items:
                new_nested = []
                for ci in nested_cost_items:
                    new_ci = _duplicate_cost_item(file, ci, new_cost_items)
                    new_nested.append(new_ci)
                inverse = ifcopenshell.util.element.copy(file, inverse)
                inverse.RelatingObject = to_element
                inverse.RelatedObjects = new_nested
                ifcopenshell.api.nest.unassign_object(file, related_objects=new_nested)
                ifcopenshell.api.nest.assign_object(
                    file, related_objects=new_nested, relating_object=to_element
                )
        else:
            for i, value in enumerate(inverse):
                if value == from_element:
                    new_inverse = ifcopenshell.util.element.copy(file, inverse)
                    new_inverse[i] = to_element
                elif isinstance(value, (tuple, list)) and from_element in value:
                    new_value = list(value)
                    new_value.append(to_element)
                    inverse[i] = new_value
