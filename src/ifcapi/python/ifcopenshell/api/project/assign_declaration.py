# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.owner
import ifcopenshell.guid
import ifcopenshell.util.element


def assign_declaration(file, definitions, relating_context):
    all_declares = relating_context.Declares
    definitions_set = set(definitions)

    previous_declares_rels = set()
    objects_without_contexts = []
    objects_with_contexts = []

    for definition in definitions_set:
        has_context = getattr(definition, "HasContext", None)
        if has_context is None:
            continue

        object_rel = next(iter(has_context), None)
        if object_rel is None:
            objects_without_contexts.append(definition)
            continue

        if object_rel not in all_declares:
            previous_declares_rels.add(object_rel)
            objects_with_contexts.append(definition)

    objects_to_change = objects_without_contexts + objects_with_contexts
    if not objects_to_change:
        return None

    for has_context in previous_declares_rels:
        related_definitions = set(has_context.RelatedDefinitions) - set(objects_with_contexts)
        if related_definitions:
            has_context.RelatedDefinitions = list(related_definitions)
            ifcopenshell.api.owner.update_owner_history(file, element=has_context)
        else:
            history = has_context.OwnerHistory
            file.remove(has_context)
            if history:
                ifcopenshell.util.element.remove_deep2(file, history)

    declares = next(iter(all_declares), None)
    if declares:
        declares.RelatedDefinitions = list(set(declares.RelatedDefinitions) | set(objects_to_change))
        ifcopenshell.api.owner.update_owner_history(file, element=declares)
    else:
        declares = file.create_entity(
            "IfcRelDeclares",
            **{
                "GlobalId": ifcopenshell.guid.new(),
                "OwnerHistory": ifcopenshell.api.owner.create_owner_history(file),
                "RelatedDefinitions": list(objects_to_change),
                "RelatingContext": relating_context,
            },
        )
    return declares
