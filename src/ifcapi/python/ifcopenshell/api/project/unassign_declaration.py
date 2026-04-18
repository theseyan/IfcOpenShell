# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.util.element


def unassign_declaration(file, definitions, relating_context):
    definitions = set(definitions)
    rels = {rel for obj in definitions if (rel := next(iter(obj.HasContext), None))}

    for rel in rels:
        related_definitions = set(rel.RelatedDefinitions) - definitions
        if related_definitions:
            rel.RelatedDefinitions = list(related_definitions)
            ifcopenshell.api.owner.update_owner_history(file, element=rel)
        else:
            history = rel.OwnerHistory
            file.remove(rel)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)
