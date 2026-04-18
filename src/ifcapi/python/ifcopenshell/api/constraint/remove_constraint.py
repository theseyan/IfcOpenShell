# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.util.element


def remove_constraint(file, constraint):
    file.remove(constraint)
    for rel in file.by_type("IfcRelAssociatesConstraint"):
        if not rel.RelatingConstraint:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
