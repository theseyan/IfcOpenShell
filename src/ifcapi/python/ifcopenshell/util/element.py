# SPDX-License-Identifier: LGPL-3.0-or-later

import ctypes
from collections import namedtuple
from collections.abc import Generator
from typing import Any, Optional, Union

import ifcopenshell
from ifcopenshell.entity_instance import entity_instance
import ifcopenshell.guid
import ifcopenshell.util.representation


PrioritisedLayer = namedtuple("PrioritisedLayer", "priority material thickness")
PrioritisedProfile = namedtuple("PrioritisedProfile", "priority material profile")


_bound = False


def _bind():
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib

    handle_p = ctypes.c_void_p
    handle_pp = ctypes.POINTER(ctypes.c_void_p)
    u32p = ctypes.POINTER(ctypes.c_uint32)

    def _decl(name, restype, argtypes):
        fn = getattr(lib, name)
        fn.restype = restype
        fn.argtypes = argtypes

    _decl("ifcopenshell_element_get_type", handle_p, [handle_p])
    _decl("ifcopenshell_element_get_aggregate", handle_p, [handle_p])
    _decl("ifcopenshell_element_get_nest", handle_p, [handle_p])
    _decl("ifcopenshell_element_get_container", handle_p, [handle_p, ctypes.c_bool, ctypes.c_char_p])
    _decl("ifcopenshell_element_get_parent", handle_p, [handle_p])
    _decl("ifcopenshell_element_get_decomposition", handle_pp, [handle_p, ctypes.c_bool, u32p])
    _decl("ifcopenshell_element_get_pset_ids", handle_pp,
          [handle_p, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, u32p])
    _decl("ifcopenshell_element_get_material", handle_p, [handle_p, ctypes.c_bool, ctypes.c_bool])
    _decl("ifcopenshell_util_element_get_styles", handle_pp, [handle_p, u32p])

    _decl("ifcopenshell_util_element_get_types", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_shape_aspects", handle_pp, [handle_p, ctypes.c_bool, u32p])
    _decl("ifcopenshell_util_element_get_groups", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_controls", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_parts", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_contained", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_referenced_structures", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_structure_referenced_elements", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_openings", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_filled_void", handle_p, [handle_p])
    _decl("ifcopenshell_util_element_get_voided_element", handle_p, [handle_p])
    _decl("ifcopenshell_util_element_is_userdefined_type", ctypes.c_bool, [handle_p])
    _decl("ifcopenshell_util_element_get_referenced_elements", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_elements_by_material", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_elements_by_style", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_elements_by_representation", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_elements_by_profile", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_elements_by_layer", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_get_layers", handle_pp, [handle_p, u32p])
    _decl("ifcopenshell_util_element_replace_element", None, [handle_p, handle_p])
    _decl("ifcopenshell_util_element_remove_deep", None, [handle_p])
    _decl("ifcopenshell_free_instance_array_only", None, [handle_pp])

    _bound = True
    return lib


def _resolve(file_, h):
    if not h:
        return None
    return entity_instance(file_, h)


def _collect(file_, ptr, count):
    if not ptr or count.value == 0:
        if ptr:
            _bind().ifcopenshell_free_instance_array_only(ptr)
        return []
    try:
        result = [entity_instance(file_, ptr[i]) for i in range(count.value)]
    finally:
        _bind().ifcopenshell_free_instance_array_only(ptr)
    return result


# ---------------------------------------------------------------------------
# Type / occurrences
# ---------------------------------------------------------------------------

def get_type(element):
    lib = _bind()
    return _resolve(element.file, lib.ifcopenshell_element_get_type(element._handle))


def get_types(type):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_types(type._handle, ctypes.byref(count))
    return tuple(_collect(type.file, ptr, count))


def get_predefined_type(element):
    if element_type := get_type(element):
        predefined_type = getattr(element_type, "PredefinedType", None)
        if predefined_type == "USERDEFINED" or not predefined_type:
            predefined_type = getattr(element_type, "ElementType", ...)
            if predefined_type == ...:
                predefined_type = getattr(element_type, "ProcessType", None)
        if predefined_type and predefined_type != "NOTDEFINED":
            return predefined_type

    predefined_type = getattr(element, "PredefinedType", None)
    if predefined_type == "USERDEFINED" or not predefined_type:
        predefined_type = getattr(element, "ObjectType", None)
    return predefined_type


def is_userdefined_type(element):
    return _bind().ifcopenshell_util_element_is_userdefined_type(element._handle)


def get_shape_aspects(element, should_inherit=True):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_shape_aspects(
        element._handle, bool(should_inherit), ctypes.byref(count))
    return _collect(element.file, ptr, count)


# ---------------------------------------------------------------------------
# Property sets / properties / quantities (value marshalling stays in Python)
# ---------------------------------------------------------------------------

def _native_pset_ids(element, psets_only, qtos_only, should_inherit):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_element_get_pset_ids(
        element._handle, bool(psets_only), bool(qtos_only), bool(should_inherit),
        ctypes.byref(count))
    return _collect(element.file, ptr, count)


def get_pset(element, name, prop=None, psets_only=False, qtos_only=False, should_inherit=True, verbose=False):
    pset = None
    type_pset = None
    ifc_file = element.file
    is_ifc2x3 = ifc_file.schema == "IFC2X3"

    if element.is_a("IfcTypeObject"):
        for definition in element.HasPropertySets or []:
            if definition.Name == name:
                pset = definition
                break
    elif (
        (is_ifc2x3_material := (is_ifc2x3 and element.is_a("IfcMaterial")))
        or element.is_a("IfcMaterialDefinition")
        or element.is_a("IfcProfileDef")
    ):
        if is_ifc2x3_material:
            for definition in ifc_file.by_type("IfcExtendedMaterialProperties"):
                if definition.Material == element and definition.Name == name:
                    pset = definition
                    break
        elif not is_ifc2x3:
            for definition in element.HasProperties or []:
                if definition.Name == name:
                    pset = definition
                    break
    elif (is_defined_by := getattr(element, "IsDefinedBy", None)) is not None:
        if should_inherit:
            element_type = get_type(element)
            if element_type:
                type_pset = get_pset(element_type, name, prop, should_inherit=False, verbose=verbose)
        for relationship in is_defined_by:
            if relationship.is_a("IfcRelDefinesByProperties"):
                definition = relationship.RelatingPropertyDefinition
                if definition.Name == name:
                    pset = definition
                    break

    if pset:
        if (
            psets_only
            and not pset.is_a("IfcPropertySet")
            and not pset.is_a("IfcPreDefinedPropertySet")
            and not (is_ifc2x3 and pset.is_a("IfcExtendedMaterialProperties"))
        ):
            pset = None
        elif qtos_only and not pset.is_a("IfcElementQuantity"):
            pset = None

    if type_pset is not None and not prop:
        if psets_only or qtos_only:
            type_pset_element = element.file.by_id(type_pset["id"])
            if (
                psets_only
                and not type_pset_element.is_a("IfcPropertySet")
                and not type_pset_element.is_a("IfcPreDefinedPropertySet")
            ):
                type_pset = None
            elif qtos_only and not type_pset_element.is_a("IfcElementQuantity"):
                type_pset = None

    if pset is None and type_pset is None:
        return

    if not prop:
        if type_pset:
            occurrence_pset = get_property_definition(pset, verbose=verbose)
            if occurrence_pset:
                type_pset.update(occurrence_pset)
            return type_pset
        return get_property_definition(pset, verbose=verbose)

    value = get_property_definition(pset, prop=prop, verbose=verbose)
    if value is None and type_pset is not None:
        return type_pset
    return value


def get_psets(element, psets_only=False, qtos_only=False, should_inherit=True, verbose=False):
    psets = {}
    if should_inherit and not element.is_a("IfcTypeObject"):
        element_type = get_type(element)
        if element_type:
            psets = get_psets(element_type, psets_only=psets_only, qtos_only=qtos_only,
                              should_inherit=False, verbose=verbose)
    for definition in _native_pset_ids(element, psets_only, qtos_only, should_inherit=False):
        psets.setdefault(definition.Name, {}).update(get_property_definition(definition, verbose=verbose))
    return psets


def get_property_definition(definition, prop=None, verbose=False):
    if not definition:
        return

    ifc_class = definition.is_a()

    if prop:
        if ifc_class == "IfcElementQuantity":
            return get_quantity(definition.Quantities, prop, verbose=verbose)
        elif ifc_class == "IfcPropertySet":
            return get_property(definition.HasProperties, prop, verbose=verbose)
        elif ifc_class in ("IfcMaterialProperties", "IfcProfileProperties"):
            return get_property(definition.Properties, prop, verbose=verbose)
        elif ifc_class == "IfcExtendedMaterialProperties":
            return get_property(definition.ExtendedProperties, prop, verbose=verbose)
        else:
            for i in range(4, len(definition)):
                if definition.attribute_name(i) == prop:
                    if (v := definition[i]) is not None:
                        return v
        return

    props = {}
    if ifc_class == "IfcElementQuantity":
        props.update(get_quantities(definition[5], verbose=verbose))
    elif ifc_class == "IfcPropertySet":
        props.update(get_properties(definition[4], verbose=verbose))
    elif ifc_class in ("IfcMaterialProperties", "IfcProfileProperties"):
        props.update(get_properties(definition[2], verbose=verbose))
    elif ifc_class == "IfcExtendedMaterialProperties":
        props.update(get_properties(definition[1], verbose=verbose))
    else:
        for prop_i in range(4, len(definition)):
            if (v := definition[prop_i]) is not None:
                props[definition.attribute_name(prop_i)] = v
    props["id"] = definition.id()
    return props


def get_quantity(quantities, name, verbose=False):
    for quantity in quantities or []:
        if quantity[0] != name:
            continue
        if quantity.is_a("IfcPhysicalSimpleQuantity"):
            result = quantity[3]
        elif quantity.is_a("IfcPhysicalComplexQuantity"):
            data = {k: v for k, v in quantity.get_info().items() if v is not None and k != "Name"}
            data["properties"] = get_quantities(quantity.HasQuantities, verbose=verbose)
            del data["HasQuantities"]
            result = data
        if verbose:
            result = {"id": quantity.id(), "class": quantity.is_a(), "value": result}
        return result


def get_quantities(quantities, verbose=False):
    results = {}
    for quantity in quantities or []:
        quantity_name = quantity[0]
        if quantity.is_a("IfcPhysicalSimpleQuantity"):
            results[quantity_name] = quantity[3]
            if verbose:
                results[quantity_name] = {
                    "id": quantity.id(), "class": quantity.is_a(),
                    "value": results[quantity_name],
                }
        elif quantity.is_a("IfcPhysicalComplexQuantity"):
            data = {k: v for k, v in quantity.get_info().items() if v is not None and k != "Name"}
            data["properties"] = get_quantities(quantity.HasQuantities, verbose=verbose)
            del data["HasQuantities"]
            results[quantity_name] = data
            if verbose:
                results[quantity_name] = {
                    "id": data["id"], "class": data["class"],
                    "value": results[quantity_name],
                }
    return results


def get_property(properties, name, verbose=False):
    for prop in properties or []:
        if prop.Name != name:
            continue
        is_single_value = False
        result_type = None
        if prop.is_a("IfcPropertySingleValue"):
            v = prop[2]
            result = v.wrappedValue if v else None
            result_type = v.is_a() if v else None
            is_single_value = True
        elif prop.is_a("IfcPropertyEnumeratedValue"):
            values = prop[2]
            result = [v.wrappedValue for v in values] if values else None
        elif prop.is_a("IfcPropertyListValue"):
            values = prop[2]
            result = [v.wrappedValue for v in values] if values else None
        elif prop.is_a("IfcPropertyBoundedValue"):
            data = prop.get_info()
            del data["Unit"]
            result = data
        elif prop.is_a("IfcPropertyTableValue"):
            result = prop.get_info()
        elif prop.is_a("IfcComplexProperty"):
            data = {k: v for k, v in prop.get_info().items() if v is not None and k != "Name"}
            data["properties"] = get_properties(prop.HasProperties, verbose=verbose)
            del data["HasProperties"]
            result = data
        if verbose:
            result = {"id": prop.id(), "class": prop.is_a(), "value": result}
            if is_single_value:
                result["value_type"] = result_type
        return result


def get_properties(properties, verbose=False):
    results = {}
    for prop in properties or []:
        ifc_class = prop.is_a()
        prop_name = prop[0]
        if ifc_class == "IfcPropertySingleValue":
            v = prop[2]
            results[prop_name] = v.wrappedValue if v else None
            if verbose:
                results[prop_name] = {
                    "id": prop.id(), "class": prop.is_a(),
                    "value": results[prop_name],
                    "value_type": v.is_a() if v else None,
                }
        elif ifc_class == "IfcPropertyEnumeratedValue":
            values = prop[2]
            results[prop_name] = [v.wrappedValue for v in values] if values else None
            if verbose:
                results[prop_name] = {
                    "id": prop.id(), "class": prop.is_a(),
                    "value": results[prop_name],
                }
        elif ifc_class == "IfcPropertyListValue":
            values = prop[2]
            results[prop_name] = [v.wrappedValue for v in values] if values else None
            if verbose:
                results[prop_name] = {
                    "id": prop.id(), "class": prop.is_a(),
                    "value": results[prop_name],
                }
        elif ifc_class == "IfcPropertyBoundedValue":
            data = prop.get_info()
            del data["Unit"]
            results[prop_name] = data
            if verbose:
                results[prop_name] = {
                    "id": data["id"], "class": data["type"],
                    "value": results[prop_name],
                }
        elif ifc_class == "IfcPropertyTableValue":
            data = prop.get_info()
            results[prop_name] = data
            if verbose:
                results[prop_name] = {
                    "id": data["id"], "class": data["type"],
                    "value": results[prop_name],
                }
        elif ifc_class == "IfcComplexProperty":
            data = {k: v for k, v in prop.get_info().items() if v is not None and k != "Name"}
            data["properties"] = get_properties(prop.HasProperties, verbose=verbose)
            del data["HasProperties"]
            results[prop_name] = data
            if verbose:
                results[prop_name] = {"id": data["id"], "class": data["class"], "value": results[prop_name]}
    return results


def get_elements_by_pset(pset):
    is_ifc2x3 = pset.file.schema == "IFC2X3"
    elements = set()
    if pset.is_a("IfcPropertySet") or pset.is_a("IfcPreDefinedPropertySet") or pset.is_a("IfcElementQuantity"):
        rels = pset.PropertyDefinitionOf if is_ifc2x3 else pset.DefinesOccurrence
        for rel in rels:
            elements.update(rel.RelatedObjects)
        for element_type in pset.DefinesType:
            elements.add(element_type)
    elif pset.is_a("IfcProfileProperties"):
        elements.add(pset.ProfileDefinition)
    elif pset.is_a("IfcMaterialProperties"):
        elements.add(pset.Material)
    else:
        raise Exception(f"Unexpected pset type: '{pset.is_a()}' ({pset}).")
    return elements


def has_property(product, property_name):
    if not property_name:
        return True
    qtos = get_psets(product, qtos_only=True)
    return any(property_name in quantities.keys() for quantities in qtos.values())


def get_element_mass_density(element):
    material = get_material(element)
    if material is None:
        return

    if (
        material.is_a("IfcMaterialLayerSet")
        or material.is_a("IfcMaterialProfileSet")
        or material.is_a("IfcMaterialConstituentSet")
    ):
        return

    if material.is_a("IfcMaterial"):
        return get_pset(material, "Pset_MaterialCommon", "MassDensity")

    if material.is_a("IfcMaterialLayerSetUsage"):
        material_layers = material.ForLayerSet.MaterialLayers
        densities = []
        thicknesses = []
        obj_mass_density = 0
        for material_layer in material_layers:
            material_mass_density = get_pset(material_layer.Material, "Pset_MaterialCommon", "MassDensity")
            if material_mass_density is None:
                return
            densities.append(material_mass_density)
            thickness = material_layer.LayerThickness
            thicknesses.append(thickness)
            obj_mass_density = obj_mass_density + (material_mass_density * thickness)
        total_thickness = sum(thicknesses)
        return obj_mass_density / total_thickness

    if material.is_a("IfcMaterialProfileSetUsage"):
        material_profiles = material.ForProfileSet.MaterialProfiles
        if len(material_profiles) == 1:
            return get_pset(material_profiles[0].Material, "Pset_MaterialCommon", "MassDensity")
        return


# ---------------------------------------------------------------------------
# Materials
# ---------------------------------------------------------------------------

def get_material(element, should_skip_usage=False, should_inherit=True):
    lib = _bind()
    return _resolve(
        element.file,
        lib.ifcopenshell_element_get_material(element._handle, bool(should_skip_usage), bool(should_inherit)),
    )


def get_materials(element, should_inherit=True):
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


def get_material_layers(element):
    material = get_material(element, should_skip_usage=True)
    if not material or not material.is_a("IfcMaterialLayerSet"):
        return []
    return [
        PrioritisedLayer(getattr(layer, "Priority", 0) or 0, layer.Material, layer.LayerThickness)
        for layer in material.MaterialLayers
    ]


def get_material_profiles(element):
    material = get_material(element, should_skip_usage=True)
    if not material or not material.is_a("IfcMaterialProfileSet"):
        return []
    return [
        PrioritisedProfile(
            getattr(material_profile, "Priority", 0) or 0,
            material_profile.Material, material_profile.Profile,
        )
        for material_profile in material.MaterialProfiles
    ]


def get_styles(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_styles(element._handle, ctypes.byref(count))
    return _collect(element.file, ptr, count)


# ---------------------------------------------------------------------------
# Spatial / aggregation / nesting / containment
# ---------------------------------------------------------------------------

def get_aggregate(element):
    lib = _bind()
    return _resolve(element.file, lib.ifcopenshell_element_get_aggregate(element._handle))


def get_nest(element):
    lib = _bind()
    return _resolve(element.file, lib.ifcopenshell_element_get_nest(element._handle))


def get_container(element, should_get_direct=False, ifc_class=None):
    lib = _bind()
    arg = ifc_class.encode("utf-8") if ifc_class else None
    return _resolve(
        element.file,
        lib.ifcopenshell_element_get_container(element._handle, bool(should_get_direct), arg),
    )


def get_parent(element, ifc_class=None):
    lib = _bind()
    parent = _resolve(element.file, lib.ifcopenshell_element_get_parent(element._handle))
    if not ifc_class:
        return parent
    while parent:
        if parent.is_a(ifc_class):
            return parent
        parent = get_parent(parent)
    return None


def get_decomposition(element, is_recursive=True):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_element_get_decomposition(
        element._handle, bool(is_recursive), ctypes.byref(count))
    return set(_collect(element.file, ptr, count))


def get_grouped_by(element, is_recursive=True):
    queue = [element]
    results = []
    while queue:
        element = queue.pop()
        for rel in getattr(element, "IsGroupedBy", []):
            related_objects = rel.RelatedObjects
            queue.extend(related_objects)
            results.extend(related_objects)
        if not is_recursive:
            break
    return results


def get_groups(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_groups(element._handle, ctypes.byref(count))
    return _collect(element.file, ptr, count)


def get_controls(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_controls(element._handle, ctypes.byref(count))
    for c in _collect(element.file, ptr, count):
        yield c


def get_parts(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_parts(element._handle, ctypes.byref(count))
    return _collect(element.file, ptr, count)


def get_contained(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_contained(element._handle, ctypes.byref(count))
    return _collect(element.file, ptr, count)


def get_components(element, include_ports=False):
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


def get_filled_void(element):
    lib = _bind()
    return _resolve(element.file, lib.ifcopenshell_util_element_get_filled_void(element._handle))


def get_voided_element(element):
    lib = _bind()
    return _resolve(element.file, lib.ifcopenshell_util_element_get_voided_element(element._handle))


def get_referenced_structures(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_referenced_structures(element._handle, ctypes.byref(count))
    return _collect(element.file, ptr, count)


def get_structure_referenced_elements(structure):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_structure_referenced_elements(structure._handle, ctypes.byref(count))
    return set(_collect(structure.file, ptr, count))


def get_openings(element):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_openings(element._handle, ctypes.byref(count))
    for r in _collect(element.file, ptr, count):
        yield r


def has_openings(element):
    return bool(next(get_openings(element), False))


# ---------------------------------------------------------------------------
# References (classification / document / library / external)
# ---------------------------------------------------------------------------

def get_referenced_elements(reference):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_referenced_elements(reference._handle, ctypes.byref(count))
    return set(_collect(reference.file, ptr, count))


# ---------------------------------------------------------------------------
# Element search by associated geometry
# ---------------------------------------------------------------------------

def get_elements_by_material(ifc_file, material):
    lib = _bind()
    if not ifc_file:
        ifc_file = material.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_elements_by_material(material._handle, ctypes.byref(count))
    return set(_collect(ifc_file, ptr, count))


def get_elements_by_style(ifc_file, style):
    lib = _bind()
    if not ifc_file:
        ifc_file = style.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_elements_by_style(style._handle, ctypes.byref(count))
    return set(_collect(ifc_file, ptr, count))


def get_elements_by_representation(ifc_file, representation):
    lib = _bind()
    if not ifc_file:
        ifc_file = representation.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_elements_by_representation(representation._handle, ctypes.byref(count))
    return set(_collect(ifc_file, ptr, count))


def get_elements_by_profile(profile):
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_elements_by_profile(profile._handle, ctypes.byref(count))
    return set(_collect(profile.file, ptr, count))


def get_elements_by_layer(ifc_file, layer):
    lib = _bind()
    if not ifc_file:
        ifc_file = layer.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_elements_by_layer(layer._handle, ctypes.byref(count))
    return set(_collect(ifc_file, ptr, count))


def get_layers(ifc_file, element):
    lib = _bind()
    if not ifc_file:
        ifc_file = element.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_util_element_get_layers(element._handle, ctypes.byref(count))
    return _collect(ifc_file, ptr, count)


# ---------------------------------------------------------------------------
# Graph editing
# ---------------------------------------------------------------------------

def replace_element(element, replacement):
    _bind().ifcopenshell_util_element_replace_element(element._handle, replacement._handle)


def replace_attribute(element, old, new):
    for i, attribute_value in enumerate(element):
        if has_element_reference(attribute_value, old):
            element[i] = element.walk(lambda v: v == old, lambda v: new, attribute_value)


def has_element_reference(value, element):
    if isinstance(value, (tuple, list)):
        for v in value:
            if has_element_reference(v, element):
                return True
        return False
    return value == element


def remove_deep(ifc_file, element):
    if not ifc_file:
        ifc_file = element.file
    _bind().ifcopenshell_util_element_remove_deep(element._handle)


def batch_remove_deep2(ifc_file):
    ifc_file.to_delete = set()


def unbatch_remove_deep2(ifc_file):
    assert ifc_file.to_delete is not None
    ifc_string = ifc_file.to_string()
    lines = iter(ifc_string.split("\n"))
    ids_to_delete = iter(sorted([e.id() for e in ifc_file.to_delete]))
    id_to_delete = next(ids_to_delete, None)
    result = []

    for line in lines:
        if id_to_delete is None:
            result.append(line)
            continue
        if line.startswith(f"#{id_to_delete}="):
            id_to_delete = next(ids_to_delete, None)
        else:
            result.append(line)

    ifc_file.to_delete = None
    return ifcopenshell.file.from_string("\n".join(result))


def remove_deep2(ifc_file, element, also_consider=None, do_not_delete=None):
    if also_consider is None:
        also_consider = []
    if do_not_delete is None:
        do_not_delete = set()
    if not ifc_file:
        ifc_file = element.file
    total_inverses = ifc_file.get_total_inverses(element)
    if total_inverses > 0:
        also_considered_inverses = 0
        contained = False
        for considered_element in also_consider:
            traverse = ifc_file.traverse(considered_element, max_levels=1)
            if element in traverse:
                also_considered_inverses += 1
                if total_inverses == also_considered_inverses:
                    contained = True
                    break
        if not contained:
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
            for i, attribute in enumerate(subelement):
                if isinstance(attribute, tuple) and len(attribute) > 10:
                    subelement[i] = []
        processed_ids.add(subelement_id)

    if ifc_file.to_delete is not None:
        ifc_file.to_delete.update(to_delete)
        return

    for subelement in filter(lambda e: e in to_delete, subgraph[::-1]):
        ifc_file.remove(subelement)


def copy(ifc_file, element):
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
            if exclude and any([attribute.is_a(e) for e in exclude]):
                pass
            elif exclude_callback and exclude_callback(attribute):
                pass
            else:
                attribute = copy_deep(
                    ifc_file, attribute, exclude=exclude,
                    copied_entities=copied_entities, exclude_callback=exclude_callback,
                )
        elif isinstance(attribute, tuple) and attribute and isinstance(attribute[0], ifcopenshell.entity_instance):
            if exclude and any([attribute[0].is_a(e) for e in exclude]):
                pass
            elif exclude_callback and exclude_callback(attribute[0]):
                pass
            else:
                attribute = list(attribute)
                for j, item in enumerate(attribute):
                    attribute[j] = copy_deep(
                        ifc_file, item, exclude=exclude,
                        exclude_callback=exclude_callback, copied_entities=copied_entities,
                    )
        if new.attribute_name(i) == "GlobalId":
            new[i] = ifcopenshell.guid.new()
        else:
            new[i] = attribute
    return new
