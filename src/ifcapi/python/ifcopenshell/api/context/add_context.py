# SPDX-License-Identifier: LGPL-3.0-or-later

"""context.add_context — create geometric representation contexts."""

import ifcopenshell


def add_context(file, context_type=None, context_identifier=None,
                target_view=None, target_scale=None, parent=None):
    if not parent:
        if context_type == "Plan":
            origin = file.createIfcAxis2Placement2D(
                file.createIfcCartesianPoint((0.0, 0.0)),
                file.createIfcDirection((1.0, 0.0)),
            )
            context = file.createIfcGeometricRepresentationContext(
                None, "Plan", 2, 1.0e-05, origin)
        else:
            origin = file.createIfcAxis2Placement3D(
                file.createIfcCartesianPoint((0.0, 0.0, 0.0)),
                file.createIfcDirection((0.0, 0.0, 1.0)),
                file.createIfcDirection((1.0, 0.0, 0.0)),
            )
            context = file.createIfcGeometricRepresentationContext(
                None, context_type, 3, 1.0e-05, origin)

        project = file.by_type("IfcProject")[0]
        existing = project.RepresentationContexts
        contexts = list(existing) if existing else []
        contexts.append(context)
        project.RepresentationContexts = contexts
        return context

    return file.create_entity("IfcGeometricRepresentationSubContext", **{
        "ContextIdentifier": context_identifier,
        "ContextType": context_type,
        "ParentContext": parent,
        "TargetView": target_view,
        "TargetScale": target_scale,
    })
