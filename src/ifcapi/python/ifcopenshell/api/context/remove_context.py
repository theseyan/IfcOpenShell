# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""context.remove_context — remove an IfcGeometricRepresentationContext."""

import ifcopenshell
import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.util.element


def remove_context(file, context):
    for subcontext in context.HasSubContexts:
        ifcopenshell.api.context.remove_context(file, context=subcontext)

    if getattr(context, "ParentContext", None):
        new = context.ParentContext
        for inverse in file.get_inverse(context):
            if inverse.is_a("IfcCoordinateOperation"):
                inverse.SourceCRS = inverse.TargetCRS
                ifcopenshell.util.element.remove_deep2(file, inverse)
            else:
                ifcopenshell.util.element.replace_attribute(inverse, context, new)
        file.remove(context)
    else:
        representations_in_context = context.RepresentationsInContext
        file.remove(context)
        for rep in representations_in_context:
            for element in ifcopenshell.util.element.get_elements_by_representation(file, rep):
                ifcopenshell.api.geometry.unassign_representation(file, product=element, representation=rep)
            ifcopenshell.api.geometry.remove_representation(file, representation=rep)
