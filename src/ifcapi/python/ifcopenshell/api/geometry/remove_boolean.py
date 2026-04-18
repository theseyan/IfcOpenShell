# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_boolean(file: ifcopenshell.file, item: ifcopenshell.entity_instance) -> None:
    """Removes a boolean operation without deleting the operands

    The first operand will replace the boolean result itself, and the second
    operand will be reset as a top level representation item.

    This may affect the Items of IfcShapeRepresentation, so it is recommended
    to run :func:`ifcopenshell.api.geometry.validate_type` after all boolean
    modifications are complete.

    :param item: This may either be an IfcBooleanResult or an
        IfcRepresentationItem that is participating in one or more boolean
        results (in which case all are removed).
    """
    if not item.is_a("IfcBooleanResult"):
        for inverse in file.get_inverse(item):
            if inverse.is_a("IfcBooleanResult"):
                remove_boolean(file, inverse)
        return

    representations = []
    queue = list(file.get_inverse(item))
    while queue:
        inverse = queue.pop()
        if inverse.is_a("IfcShapeRepresentation"):
            representations.append(inverse)
        elif inverse.is_a("IfcBooleanResult"):
            queue.extend(file.get_inverse(inverse))
        elif inverse.is_a("IfcCsgSolid"):
            queue.extend(file.get_inverse(inverse))

    first = item.FirstOperand
    second = item.SecondOperand
    for inverse in file.get_inverse(item):
        ifcopenshell.util.element.replace_attribute(inverse, item, first)

    for representation in set(representations):
        representation.Items = list(representation.Items) + [second]

    file.remove(item)
