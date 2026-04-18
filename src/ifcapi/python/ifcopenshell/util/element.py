# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Element utilities backed by the native C API.

Hot-path graph traversal (``get_type``, ``get_aggregate``, ``get_nest``,
``get_container``, ``get_decomposition``) is implemented in C++ and called
here via ctypes. Value-marshalling helpers (``get_pset``, ``get_psets``, etc.)
remain in Python on top of the low-level C API.
"""

import ctypes

import ifcopenshell
from ifcopenshell.entity_instance import entity_instance
import ifcopenshell.guid


_bound = False


def _bind():
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib
    lib.ifcopenshell_element_get_type.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_type.argtypes = [ctypes.c_void_p]

    lib.ifcopenshell_element_get_aggregate.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_aggregate.argtypes = [ctypes.c_void_p]

    lib.ifcopenshell_element_get_nest.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_nest.argtypes = [ctypes.c_void_p]

    lib.ifcopenshell_element_get_container.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_container.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_char_p]

    lib.ifcopenshell_element_get_parent.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_parent.argtypes = [ctypes.c_void_p]

    lib.ifcopenshell_element_get_decomposition.restype = ctypes.POINTER(ctypes.c_void_p)
    lib.ifcopenshell_element_get_decomposition.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.POINTER(ctypes.c_uint32)]

    lib.ifcopenshell_element_get_pset_ids.restype = ctypes.POINTER(ctypes.c_void_p)
    lib.ifcopenshell_element_get_pset_ids.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.POINTER(ctypes.c_uint32)]

    lib.ifcopenshell_element_get_material.restype = ctypes.c_void_p
    lib.ifcopenshell_element_get_material.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_bool]
    lib.ifcopenshell_free_instance_array_only.restype = None
    lib.ifcopenshell_free_instance_array_only.argtypes = [ctypes.POINTER(ctypes.c_void_p)]

    _bound = True
    return lib


def _resolve(element, h):
    """Wrap a returned C handle into an entity_instance, or return None."""
    if not h:
        return None
    return entity_instance(element.file, h)


def remove_deep2(ifc_file, element, also_consider=None, do_not_delete=None):
    """Recursively purge a subgraph safely, starting at an element.

    Only removes elements whose inverses are fully contained in the subgraph.
    """
    if also_consider is None:
        also_consider = []
    if do_not_delete is None:
        do_not_delete = set()

    total_inverses = ifc_file.get_total_inverses(element)
    if total_inverses > 0:
        also_considered_inverses = 0
        for considered_element in also_consider:
            traverse = ifc_file.traverse(considered_element, max_levels=1)
            if element in traverse:
                also_considered_inverses += 1
                if total_inverses == also_considered_inverses:
                    break
        else:
            if also_considered_inverses < total_inverses:
                return

    to_delete = set()
    subgraph = list(ifc_file.traverse(element, breadth_first=True))
    subgraph.extend(also_consider)
    subgraph_set = set(subgraph)
    subelement_queue = [element]
    processed_ids = set()

    while subelement_queue:
        subelement = subelement_queue.pop(0)
        subelement_id = subelement.id()
        if (
            subelement_id
            and subelement_id not in processed_ids
            and subelement not in do_not_delete
            and (
                ifc_file.get_total_inverses(subelement) < 2
                or len(set(ifc_file.get_inverse(subelement)) - subgraph_set) == 0
            )
        ):
            to_delete.add(subelement)
            subelement_queue.extend(ifc_file.traverse(subelement, max_levels=1)[1:])
        processed_ids.add(subelement_id)

    for subelement in filter(lambda e: e in to_delete, subgraph[::-1]):
        ifc_file.remove(subelement)


def get_aggregate(element):
    """Return the aggregate parent of *element*, or None."""
    lib = _bind()
    rid = lib.ifcopenshell_element_get_aggregate(element._handle)
    return _resolve(element, rid)


def get_nest(element):
    """Return the nesting host of *element*, or None."""
    lib = _bind()
    rid = lib.ifcopenshell_element_get_nest(element._handle)
    return _resolve(element, rid)


def get_container(element, should_get_direct=False, ifc_class=None):
    """Return the spatial container of *element*, or None.

    If ``should_get_direct`` is True, only returns a direct container. Otherwise
    walks the aggregation chain. An optional ``ifc_class`` filters by type.
    """
    lib = _bind()
    class_arg = ifc_class.encode("utf-8") if ifc_class else None
    rid = lib.ifcopenshell_element_get_container(element._handle, bool(should_get_direct), class_arg
    )
    return _resolve(element, rid)


def get_type(element):
    """Return the type element assigned to *element*, or None."""
    lib = _bind()
    rid = lib.ifcopenshell_element_get_type(element._handle)
    return _resolve(element, rid)


def get_predefined_type(element):
    """Return the predefined type string of *element*, or None."""
    if (element_type := get_type(element)):
        predefined_type = getattr(element_type, "PredefinedType", None)
        if predefined_type == "USERDEFINED" or not predefined_type:
            predefined_type = getattr(element_type, "ElementType", ...)
            if predefined_type is ...:
                predefined_type = getattr(element_type, "ProcessType", None)
        if predefined_type and predefined_type != "NOTDEFINED":
            return predefined_type

    predefined_type = getattr(element, "PredefinedType", None)
    if predefined_type == "USERDEFINED" or not predefined_type:
        predefined_type = getattr(element, "ObjectType", None)
    return predefined_type


def get_grouped_by(group):
    """Return the list of products assigned to *group*."""
    is_grouped_by = getattr(group, "IsGroupedBy", ())
    if not is_grouped_by:
        return []
    result = []
    for rel in is_grouped_by:
        result.extend(rel.RelatedObjects)
    return result


# ---------------------------------------------------------------------------
# Property set utilities
# ---------------------------------------------------------------------------

def get_psets(element, psets_only=False, qtos_only=False, should_inherit=True, verbose=False):
    """Retrieve property sets as a dict of {pset_name: {prop_name: value, ...}}.

    Graph traversal (walking ``IsDefinedBy``, ``IsTypedBy``, and the
    material/profile/type special-cases) is handled by the native C API.
    Per-property value extraction remains in Python.
    """
    lib = _bind()
    ifc_file = element.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_element_get_pset_ids(element._handle,
        bool(psets_only), bool(qtos_only), bool(should_inherit),
        ctypes.byref(count),
    )
    try:
        if not ptr or count.value == 0:
            return {}
        defns = [entity_instance(ifc_file, ptr[i]) for i in range(count.value)]
    finally:
        if ptr:
            lib.ifcopenshell_free_instance_array_only(ptr)

    psets = {}
    for defn in defns:
        if defn is None:
            continue
        psets.setdefault(defn.Name, {}).update(get_property_definition(defn, verbose=verbose))
    return psets


def get_property_definition(definition, verbose=False):
    """Return a dict of property names to values from a property definition."""
    if not definition:
        return {}

    ifc_class = definition.is_a()
    props = {}

    if ifc_class == "IfcPropertySet":
        props.update(_get_properties(getattr(definition, "HasProperties", ()) or (), verbose=verbose))
    elif ifc_class in ("IfcMaterialProperties", "IfcProfileProperties"):
        props.update(_get_properties(getattr(definition, "Properties", ()) or (), verbose=verbose))
    elif ifc_class == "IfcExtendedMaterialProperties":
        props.update(_get_properties(getattr(definition, "ExtendedProperties", ()) or (), verbose=verbose))
    elif ifc_class == "IfcElementQuantity":
        props.update(_get_quantities(getattr(definition, "Quantities", ()) or (), verbose=verbose))

    props["id"] = definition.id()
    return props


def _get_properties(properties, verbose=False):
    """Extract property name→value pairs from a list of IfcProperty entities."""
    results = {}
    for prop in properties or ():
        prop_name = prop.Name
        ifc_class = prop.is_a()
        if ifc_class == "IfcPropertySingleValue":
            nv = getattr(prop, "NominalValue", None)
            results[prop_name] = nv.wrappedValue if nv else None
            if verbose:
                results[prop_name] = {
                    "id": prop.id(), "class": ifc_class,
                    "value": results[prop_name],
                    "value_type": nv.is_a() if nv else None,
                }
        elif ifc_class == "IfcPropertyEnumeratedValue":
            ev = getattr(prop, "EnumerationValues", None)
            results[prop_name] = [v.wrappedValue for v in ev] if ev else None
            if verbose:
                results[prop_name] = {"id": prop.id(), "class": ifc_class, "value": results[prop_name]}
        elif ifc_class == "IfcPropertyListValue":
            lv = getattr(prop, "ListValues", None)
            results[prop_name] = [v.wrappedValue for v in lv] if lv else None
            if verbose:
                results[prop_name] = {"id": prop.id(), "class": ifc_class, "value": results[prop_name]}
    return results


def _get_quantities(quantities, verbose=False):
    """Extract quantity name→value pairs from a list of IfcQuantity entities."""
    results = {}
    for q in quantities or ():
        q_name = q.Name
        if q.is_a("IfcPhysicalSimpleQuantity"):
            results[q_name] = q[3]
            if verbose:
                results[q_name] = {"id": q.id(), "class": q.is_a(), "value": results[q_name]}
    return results


def get_material(element, should_skip_usage=False, should_inherit=True):
    """Return the material or material set assigned to *element*, or None."""
    lib = _bind()
    rid = lib.ifcopenshell_element_get_material(element._handle, bool(should_skip_usage), bool(should_inherit)
    )
    return _resolve(element, rid)


def get_materials(element, should_inherit=True):
    """Return a flat list of IfcMaterial entities assigned to *element*."""
    material = get_material(element, should_skip_usage=True, should_inherit=should_inherit)
    if not material:
        return []
    if material.is_a("IfcMaterial"):
        return [material]
    if material.is_a("IfcMaterialLayerSet"):
        return [l.Material for l in material.MaterialLayers]
    if material.is_a("IfcMaterialProfileSet"):
        return [p.Material for p in material.MaterialProfiles]
    if material.is_a("IfcMaterialConstituentSet"):
        return [c.Material for c in material.MaterialConstituents]
    if material.is_a("IfcMaterialList"):
        return list(material.Materials)
    return []


def get_pset(element, name, prop=None, psets_only=False, qtos_only=False, should_inherit=True, verbose=False):
    """Retrieve a single property set or single property by name."""
    psets = get_psets(element, psets_only=psets_only, qtos_only=qtos_only,
                      should_inherit=should_inherit, verbose=verbose)
    result = psets.get(name)
    if result is None:
        return None
    if prop:
        return result.get(prop)
    return result


def get_elements_by_pset(pset):
    """Return a set of all elements associated with *pset*."""
    ifc_file = pset.file
    is_ifc2x3 = ifc_file.schema == "IFC2X3"
    elements = set()

    # Type objects
    if hasattr(pset, "DefinesType"):
        for t in getattr(pset, "DefinesType", ()) or ():
            elements.add(t)

    # Occurrences via IfcRelDefinesByProperties
    rels = getattr(pset, "PropertyDefinitionOf", ()) if is_ifc2x3 else getattr(pset, "DefinesOccurrence", ())
    for rel in rels or ():
        for obj in rel.RelatedObjects or ():
            elements.add(obj)

    return elements


def copy(ifc_file, element):
    """Copy a single element. Any referenced elements are not copied. GlobalIds are regenerated."""
    if not ifc_file:
        ifc_file = element.file
    new = ifc_file.create_entity(element.is_a())
    for i, attribute in enumerate(element):
        if attribute is None:
            continue
        if new.attribute_name(i) == "GlobalId":
            new[i] = ifcopenshell.guid.new()
        else:
            new[i] = attribute
    return new


def copy_deep(ifc_file, element, exclude=None, exclude_callback=None, copied_entities=None):
    """Recursively copy an element and all of its directly related subelements."""
    if not ifc_file:
        ifc_file = element.file
    if copied_entities is None:
        copied_entities = {}
    else:
        copied_entity = copied_entities.get(element.id(), None)
        if copied_entity:
            return copied_entity
    new = ifc_file.create_entity(element.is_a())
    if element.id():
        copied_entities[element.id()] = new
    for i, attribute in enumerate(element):
        if attribute is None:
            continue
        if isinstance(attribute, ifcopenshell.entity_instance):
            if exclude and any(attribute.is_a(e) for e in exclude):
                pass
            elif exclude_callback and exclude_callback(attribute):
                pass
            else:
                attribute = copy_deep(ifc_file, attribute, exclude=exclude,
                                      copied_entities=copied_entities,
                                      exclude_callback=exclude_callback)
        elif isinstance(attribute, tuple) and attribute and isinstance(attribute[0], ifcopenshell.entity_instance):
            if exclude and any(attribute[0].is_a(e) for e in exclude):
                pass
            elif exclude_callback and exclude_callback(attribute[0]):
                pass
            else:
                attribute = [copy_deep(ifc_file, item, exclude=exclude,
                                       exclude_callback=exclude_callback,
                                       copied_entities=copied_entities) for item in attribute]
        if new.attribute_name(i) == "GlobalId":
            new[i] = ifcopenshell.guid.new()
        else:
            new[i] = attribute
    return new


def get_referenced_elements(reference):
    """Get all elements that a reference (doc/classification/library) is associated with."""
    results = set()
    ifc_file = reference.file
    if ifc_file.schema == "IFC2X3":
        rel_type = None
        if reference.is_a("IfcDocumentReference") or reference.is_a("IfcDocumentInformation"):
            rel_type = "IfcRelAssociatesDocument"
        elif reference.is_a("IfcClassificationReference") or reference.is_a("IfcClassification"):
            rel_type = "IfcRelAssociatesClassification"
        elif reference.is_a("IfcLibraryReference") or reference.is_a("IfcLibraryInformation"):
            rel_type = "IfcRelAssociatesLibrary"
        if rel_type:
            attr = {"IfcRelAssociatesDocument": "RelatingDocument",
                    "IfcRelAssociatesClassification": "RelatingClassification",
                    "IfcRelAssociatesLibrary": "RelatingLibrary"}.get(rel_type)
            for rel in ifc_file.by_type(rel_type):
                if getattr(rel, attr) == reference:
                    for obj in rel.RelatedObjects:
                        results.add(obj)
    else:
        for inv_attr in ("DocumentRefForObjects", "DocumentInfoForObjects",
                         "ClassificationRefForObjects", "LibraryRefForObjects",
                         "ExternalReferenceForResources"):
            for rel in getattr(reference, inv_attr, ()) or ():
                for obj in getattr(rel, "RelatedObjects", ()) or ():
                    results.add(obj)
                for obj in getattr(rel, "RelatedResourceObjects", ()) or ():
                    results.add(obj)
    return results


def get_components(element, include_ports=False):
    """Retrieves the components of an element that have a nest relationship."""
    objects = []
    is_ifc2x3 = element.file.schema == "IFC2X3"
    if is_ifc2x3:
        if is_decomposed_by := getattr(element, "IsDecomposedBy", ()):
            for rel in is_decomposed_by:
                if rel.is_a("IfcRelNests"):
                    objects.extend(rel.RelatedObjects)
    else:
        if is_nested_by := getattr(element, "IsNestedBy", None):
            for rel in is_nested_by:
                objects.extend(rel.RelatedObjects)
    if include_ports:
        return objects
    return [e for e in objects if not e.is_a("IfcPort")]


def get_decomposition(element, is_recursive=True):
    """Retrieves all subelements of an element based on spatial decomposition hierarchy."""
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_element_get_decomposition(element._handle, bool(is_recursive), ctypes.byref(count)
    )
    try:
        if not ptr or count.value == 0:
            return set()
        result = {entity_instance(element.file, ptr[i]) for i in range(count.value)}
    finally:
        if ptr:
            lib.ifcopenshell_free_instance_array_only(ptr)
    return result


def replace_attribute(element, old, new):
    """Replace an attribute value on an entity, searching through all attributes."""
    for i, attr_value in enumerate(element):
        if attr_value == old:
            element[i] = new
        elif isinstance(attr_value, (tuple, list)):
            new_list = list(attr_value)
            changed = False
            for j, item in enumerate(new_list):
                if item == old:
                    new_list[j] = new
                    changed = True
            if changed:
                element[i] = new_list
