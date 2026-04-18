# SPDX-License-Identifier: LGPL-3.0-or-later

"""Minimal constraint utilities."""


def get_constrained_elements(constraint):
    """Get elements associated with a constraint via IfcRelAssociatesConstraint."""
    elements = set()
    for rel in constraint.file.get_inverse(constraint):
        if rel.is_a("IfcRelAssociatesConstraint"):
            elements.update(rel.RelatedObjects)
    return elements
