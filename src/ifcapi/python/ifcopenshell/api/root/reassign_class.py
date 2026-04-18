# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.api.aggregate
import ifcopenshell.api.geometry
import ifcopenshell.api.pset
import ifcopenshell.api.spatial
import ifcopenshell.api.type
import ifcopenshell.util.element
import ifcopenshell.util.representation
import ifcopenshell.util.schema
import ifcopenshell.util.type


def reassign_class(file, product, ifc_class="IfcBuildingElementProxy", predefined_type=None, occurrence_class=None):
    was_type_product_before = product.is_a("IfcTypeProduct")
    schema = ifcopenshell.schema_by_name(file.schema)
    is_type_product_after = schema.declaration_by_name(ifc_class)._is("IfcTypeProduct")

    if was_type_product_before == is_type_product_after:
        return _simple_reassignment(file, schema, product, ifc_class, predefined_type, occurrence_class)

    switch_type = "occurrence_to_type" if is_type_product_after else "type_to_occurrence"

    return _switch_between_class_types(file, schema, product, switch_type, ifc_class, predefined_type)


def _switch_between_class_types(file, schema, element, switch_type, ifc_class, predefined_type):
    psets_to_reassign = []

    representations = list(ifcopenshell.util.representation.get_representations_iter(element))
    for rep in representations:
        if switch_type == "type_to_occurrence":
            rep = ifcopenshell.util.representation.resolve_representation(rep)
        ifcopenshell.api.geometry.unassign_representation(file, product=element, representation=rep)

    if switch_type == "type_to_occurrence":
        occurrences = ifcopenshell.util.element.get_types(element)
        psets_to_reassign = element.HasPropertySets or []

        element = _reassign_class_element(file, element, ifc_class, predefined_type)
        ifcopenshell.api.type.unassign_type(file, occurrences)

    else:
        element_type = ifcopenshell.util.element.get_type(element)
        if element_type:
            ifcopenshell.api.type.unassign_type(file, [element])

        if ifcopenshell.util.element.get_container(element):
            ifcopenshell.api.spatial.unassign_container(file, [element])
        elif ifcopenshell.util.element.get_aggregate(element):
            ifcopenshell.api.aggregate.unassign_object(file, [element])

        psets = ifcopenshell.util.element.get_psets(element)
        for pset_name in psets:
            pset = file.by_id(psets[pset_name]["id"])
            psets_to_reassign.append(pset)
            ifcopenshell.api.pset.unassign_pset(file, [element], pset)

        element = _reassign_class_element(file, element, ifc_class, predefined_type)

    for pset in psets_to_reassign:
        ifcopenshell.api.pset.assign_pset(file, [element], pset)

    for rep in representations:
        ifcopenshell.api.geometry.assign_representation(file, product=element, representation=rep)

    if switch_type == "type_to_occurrence" and representations:
        ifcopenshell.api.geometry.edit_object_placement(file, product=element)

    return element


def _simple_reassignment(file, schema, element, ifc_class, predefined_type, occurrence_class):
    element = _reassign_class_element(file, element, ifc_class, predefined_type)
    if element.is_a("IfcTypeProduct"):
        if occurrence_class:
            oc = occurrence_class
        else:
            oc = next(
                iter(ifcopenshell.util.type.get_applicable_entities(ifc_class, file.schema))
            )
        assert not schema.declaration_by_name(oc)._is(
            "IfcTypeProduct"
        ), f"Unexpected occurrence_class: '{oc}' / '{occurrence_class}'."

        for occurrence in ifcopenshell.util.element.get_types(element):
            _reassign_class_element(file, occurrence, oc, predefined_type)
    else:
        element_type = ifcopenshell.util.element.get_type(element)
        if element_type:
            ifc_class_ = next(iter(ifcopenshell.util.type.get_applicable_types(ifc_class, file.schema)))
            element_type = _reassign_class_element(file, element_type, ifc_class_, predefined_type)
            ifc_class = element.is_a()
            for occurrence in ifcopenshell.util.element.get_types(element_type):
                if occurrence == element:
                    continue
                _reassign_class_element(file, occurrence, ifc_class, predefined_type)
    return element


def _reassign_class_element(file, element, ifc_class, predefined_type):
    element = ifcopenshell.util.schema.reassign_class(file, element, ifc_class)
    if predefined_type and hasattr(element, "PredefinedType"):
        try:
            element.PredefinedType = predefined_type
        except:
            element.PredefinedType = "USERDEFINED"
            if element.is_a("IfcTypeProduct"):
                element.ElementType = predefined_type
            else:
                element.ObjectType = predefined_type

    return element
