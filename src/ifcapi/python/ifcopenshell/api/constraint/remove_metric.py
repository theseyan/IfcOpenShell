# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def _delete_reference(file, reference):
    if reference.InnerReference:
        _delete_reference(file, reference.InnerReference)
    file.remove(reference)


def remove_metric(file, metric):
    if hasattr(metric, "ReferencePath") and metric.ReferencePath:
        _delete_reference(file, metric.ReferencePath)

    file.remove(metric)
    for rel in file.by_type("IfcRelAssociatesConstraint"):
        if not rel.RelatingConstraint:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
    for resource_rel in file.by_type("IfcResourceConstraintRelationship"):
        if not resource_rel.RelatingConstraint:
            file.remove(resource_rel)
