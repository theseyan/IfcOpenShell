# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.nest
import ifcopenshell.api.root
import ifcopenshell.guid


def add_resource(
    file,
    parent_resource=None,
    ifc_class="IfcCrewResource",
    name=None,
    predefined_type="NOTDEFINED",
):
    resource = ifcopenshell.api.root.create_entity(
        file,
        ifc_class=ifc_class,
        predefined_type=predefined_type,
        name=name or "Unnamed",
    )
    if parent_resource:
        ifcopenshell.api.nest.assign_object(file, related_objects=[resource], relating_object=parent_resource)
    elif file.schema != "IFC2X3":
        contexts = file.by_type("IfcContext")
        if contexts:
            # Declare the resource under the project context
            rels = contexts[0].Declares or []
            for rel in rels:
                if rel.is_a("IfcRelDeclares"):
                    definitions = list(rel.RelatedDefinitions)
                    definitions.append(resource)
                    rel.RelatedDefinitions = definitions
                    return resource
            file.create_entity(
                "IfcRelDeclares",
                GlobalId=ifcopenshell.guid.new(),
                RelatingContext=contexts[0],
                RelatedDefinitions=[resource],
            )
    return resource
