# SPDX-License-Identifier: LGPL-3.0-or-later

from collections.abc import Callable
from functools import partial
from typing import get_args

import ifcopenshell
import ifcopenshell.api.context
import ifcopenshell.api.geometry
import ifcopenshell.api.owner.settings
import ifcopenshell.api.project
import ifcopenshell.api.type
import ifcopenshell.ifcopenshell_wrapper as W
import ifcopenshell.util.element
import ifcopenshell.util.geolocation
import ifcopenshell.util.placement
import ifcopenshell.util.unit

MATERIAL_SETS = ("IfcMaterialLayerSet", "IfcMaterialConstituentSet", "IfcMaterialProfileSet")


class SafeRemovalContext:
    def __init__(self, ifc_file, reuse_identities, assume_asset_uniqueness_by_name):
        self.file = ifc_file
        self.reuse_identities = reuse_identities
        self.assume_asset_uniqueness_by_name = assume_asset_uniqueness_by_name

    def __enter__(self):
        if not self.assume_asset_uniqueness_by_name:
            return
        ifcopenshell.util.element.batch_remove_deep2(self.file)

    def __exit__(self, *args):
        if not self.assume_asset_uniqueness_by_name:
            return
        removed_identities = {}
        assert self.file.to_delete is not None
        removed_elements = self.file.to_delete
        for identity, element in self.reuse_identities.items():
            if element in removed_elements:
                removed_identities[element] = identity
        assert len(removed_identities) == len(removed_elements)
        for element in self.file.to_delete:
            if element in self.file.to_delete:
                self.file.remove(element)
        self.file.to_delete = None
        for identity in removed_identities.values():
            del self.reuse_identities[identity]


def append_asset(file, library, element, reuse_identities=None, assume_asset_uniqueness_by_name=True):
    reuse_identities = {} if reuse_identities is None else reuse_identities
    added_elements = {}
    whitelisted_inverse_attributes = {}
    base_material_class = "IfcMaterial" if file.schema == "IFC2X3" else "IfcMaterialDefinition"

    ctx = _Context(file, library, element, reuse_identities, added_elements,
                   whitelisted_inverse_attributes, base_material_class,
                   assume_asset_uniqueness_by_name, None)

    if element.is_a("IfcTypeProduct"):
        ctx.target_class = "IfcTypeProduct"
        return _append_type_product(ctx)
    elif element.is_a("IfcProduct"):
        ctx.target_class = "IfcProduct"
        return _append_product(ctx)
    elif element.is_a("IfcMaterial"):
        ctx.target_class = "IfcMaterial"
        return _append_material(ctx)
    elif element.is_a("IfcCostSchedule"):
        ctx.target_class = "IfcCostSchedule"
        return _append_cost_schedule(ctx)
    elif element.is_a("IfcProfileDef"):
        ctx.target_class = "IfcProfileDef"
        return _append_profile_def(ctx)
    elif element.is_a("IfcPresentationStyle"):
        ctx.target_class = "IfcPresentationStyle"
        return _append_presentation_style(ctx)


class _Context:
    def __init__(self, file, library, element, reuse_identities, added_elements,
                 whitelisted_inverse_attributes, base_material_class,
                 assume_asset_uniqueness_by_name, target_class):
        self.file = file
        self.library = library
        self.element = element
        self.reuse_identities = reuse_identities
        self.added_elements = added_elements
        self.whitelisted_inverse_attributes = whitelisted_inverse_attributes
        self.base_material_class = base_material_class
        self.assume_asset_uniqueness_by_name = assume_asset_uniqueness_by_name
        self.target_class = target_class
        self.existing_contexts = None


def _by_guid(file, guid):
    try:
        return file.by_guid(guid)
    except RuntimeError:
        return None


def _material_sets_are_equal(set1, set2):
    if set1.is_a() != set2.is_a():
        return False

    ifc_class = set1.is_a()

    if ifc_class == "IfcMaterialLayerSet":
        layers1 = set1.MaterialLayers or []
        layers2 = set2.MaterialLayers or []
        if len(layers1) != len(layers2):
            return False
        for l1, l2 in zip(layers1, layers2):
            if (l1.Material is None) != (l2.Material is None):
                return False
            if l1.Material and l1.Material.Name != l2.Material.Name:
                return False
            if l1.LayerThickness != l2.LayerThickness:
                return False

    elif ifc_class == "IfcMaterialConstituentSet":
        constituents1 = set1.MaterialConstituents or []
        constituents2 = set2.MaterialConstituents or []
        if len(constituents1) != len(constituents2):
            return False
        for c1, c2 in zip(constituents1, constituents2):
            if (c1.Material is None) != (c2.Material is None):
                return False
            if c1.Material and c1.Material.Name != c2.Material.Name:
                return False
            if c1.Name != c2.Name:
                return False

    elif ifc_class == "IfcMaterialProfileSet":
        profiles1 = set1.MaterialProfiles or []
        profiles2 = set2.MaterialProfiles or []
        if len(profiles1) != len(profiles2):
            return False
        for p1, p2 in zip(profiles1, profiles2):
            if (p1.Material is None) != (p2.Material is None):
                return False
            if p1.Material and p1.Material.Name != p2.Material.Name:
                return False
            if (p1.Profile is None) != (p2.Profile is None):
                return False
            if p1.Profile:
                profile_name1 = getattr(p1.Profile, "ProfileName", None)
                profile_name2 = getattr(p2.Profile, "ProfileName", None)
                if profile_name1 != profile_name2:
                    return False

    return True


def _get_existing_element(ctx, element):
    if element.id() in ctx.added_elements:
        return ctx.added_elements[element.id()]
    if element.is_a("IfcRoot"):
        return _by_guid(ctx.file, element.GlobalId)
    elif not ctx.assume_asset_uniqueness_by_name:
        return None
    elif element.is_a("IfcMaterial"):
        name = element.Name
        return next((e for e in ctx.file.by_type("IfcMaterial") if e.Name == name), None)

    elif element.is_a() in MATERIAL_SETS:
        ifc_class = element.is_a()
        name_attr = "LayerSetName" if ifc_class == "IfcMaterialLayerSet" else "Name"
        material_set_name = getattr(element, name_attr)
        if material_set_name is None:
            return
        for candidate in ctx.file.by_type(ifc_class):
            if getattr(candidate, name_attr) == material_set_name:
                if _material_sets_are_equal(element, candidate):
                    return candidate
        return None

    elif element.is_a("IfcProfileDef"):
        profile_name = element.ProfileName
        if profile_name is None:
            return None
        return next((e for e in ctx.file.by_type("IfcProfileDef") if e.ProfileName == profile_name), None)
    elif element.is_a("IfcPresentationStyle"):
        name = element.Name
        if name is None:
            return None
        return next((e for e in ctx.file.by_type(element.is_a()) if e.Name == name), None)

    elif (ifc_class := element.is_a()) == "IfcOrganization":
        attr_name = "Id" if ctx.file.schema == "IFC2X3" else "Identification"
        org_id = getattr(element, attr_name)
        if org_id is not None:
            return next((e for e in ctx.file.by_type("IfcOrganization") if getattr(e, attr_name) == org_id), None)
    elif ifc_class == "IfcPerson":
        attr_name = "Id" if ctx.file.schema == "IFC2X3" else "Identification"
        person_id = getattr(element, attr_name)
        if person_id is not None:
            return next((e for e in ctx.file.by_type("IfcPerson") if getattr(e, attr_name) == person_id), None)

    else:
        return None


def _append_material(ctx):
    ctx.whitelisted_inverse_attributes = {
        "IfcMaterial": ["HasExternalReferences", "HasProperties", "HasRepresentation"]
    }
    ctx.existing_contexts = ctx.file.by_type("IfcGeometricRepresentationContext")
    element = _add_element(ctx, ctx.element)
    if element.HasRepresentation:
        _reuse_existing_contexts(ctx)
    return element


def _append_cost_schedule(ctx):
    ctx.whitelisted_inverse_attributes = {"IfcCostSchedule": ["Controls"], "IfcCostItem": ["IsNestedBy"]}
    return _add_element(ctx, ctx.element)


def _append_profile_def(ctx):
    ctx.whitelisted_inverse_attributes = {"IfcProfileDef": ["HasProperties"]}
    return _add_element(ctx, ctx.element)


def _append_presentation_style(ctx):
    ctx.whitelisted_inverse_attributes = {}
    return _add_element(ctx, ctx.element)


def _append_type_product(ctx):
    ctx.whitelisted_inverse_attributes = {
        "IfcObjectDefinition": ["HasAssociations"],
        "IfcDistributionElementType": ["IsNestedBy"],
        ctx.base_material_class: ["HasExternalReferences", "HasProperties", "HasRepresentation"],
        "IfcRepresentationItem": ["StyledByItem", "LayerAssignment"],
        "IfcRepresentation": ["LayerAssignments"],
        "IfcProductDefinitionShape": ["HasShapeAspects"],
        "IfcRepresentationMap": ["HasShapeAspects"],
    }
    ctx.existing_contexts = ctx.file.by_type("IfcGeometricRepresentationContext")
    element = _add_element(ctx, ctx.element)
    _reuse_existing_contexts(ctx)
    return element


def _append_product(ctx):
    ctx.whitelisted_inverse_attributes = {
        "IfcObjectDefinition": ["HasAssociations"],
        "IfcObject": ["IsDefinedBy.IfcRelDefinesByProperties"],
        "IfcElement": ["HasOpenings"],
        "IfcDistributionElement": ["IsNestedBy"],
        ctx.base_material_class: ["HasExternalReferences", "HasProperties", "HasRepresentation"],
        "IfcRepresentationItem": [
            "StyledByItem",
            "LayerAssignments" if ctx.file.schema == "IFC2X3" else "LayerAssignment",
        ],
        "IfcRepresentation": ["LayerAssignments"],
        "IfcProductDefinitionShape": ["HasShapeAspects"],
        "IfcRepresentationMap": ["HasShapeAspects"],
    }
    ctx.existing_contexts = ctx.file.by_type("IfcGeometricRepresentationContext")
    element = _add_element(ctx, ctx.element)
    _reuse_existing_contexts(ctx)

    placement = element.ObjectPlacement
    if placement is not None:
        matrix = ifcopenshell.util.placement.get_local_placement(placement)
        matrix = ifcopenshell.util.geolocation.auto_local2global(ctx.library, matrix)
        matrix = ifcopenshell.util.geolocation.auto_global2local(ctx.file, matrix)
        with SafeRemovalContext(ctx.file, ctx.reuse_identities, ctx.assume_asset_uniqueness_by_name):
            ifcopenshell.api.geometry.edit_object_placement(ctx.file, element, matrix, is_si=False)

    element_type = ifcopenshell.util.element.get_type(ctx.element)
    if element_type:
        ifcopenshell.api.owner.settings.factory_reset()
        new_type = ifcopenshell.api.project.append_asset(
            ctx.file,
            library=ctx.library,
            element=element_type,
            reuse_identities=ctx.reuse_identities,
        )
        ifcopenshell.api.type.assign_type(
            ctx.file,
            should_run_listeners=False,
            related_objects=[element],
            relating_type=new_type,
            should_map_representations=False,
        )
        ifcopenshell.api.owner.settings.restore()

    return element


def _add_element(ctx, element):
    if element.id() == 0:
        return
    existing_element = _get_existing_element(ctx, element)
    if existing_element:
        return existing_element
    new = _file_add(ctx, element)
    ctx.added_elements[element.id()] = new
    _check_inverses(ctx, element)
    subelement_queue = ctx.library.traverse(element, max_levels=1)[1:]
    while subelement_queue:
        subelement = subelement_queue.pop(0)
        existing_element = _get_existing_element(ctx, subelement)
        if existing_element:
            ctx.added_elements[subelement.id()] = existing_element
            if not _has_whitelisted_inverses(ctx, existing_element):
                _check_inverses(ctx, subelement)
        else:
            ctx.added_elements[subelement.id()] = _file_add(ctx, subelement)
            _check_inverses(ctx, subelement)
            subelement_queue.extend(ctx.library.traverse(subelement, max_levels=1)[1:])
    return new


def _has_whitelisted_inverses(ctx, element):
    for source_class, attributes in ctx.whitelisted_inverse_attributes.items():
        if not element.is_a(source_class):
            continue
        for attribute in attributes:
            attribute_class = None
            if "." in attribute:
                attribute, attribute_class = attribute.split(".")
            value = getattr(element, attribute, [])
            if attribute_class:
                for subvalue in value:
                    if subvalue.is_a(attribute_class):
                        return True
            elif value:
                return True
    return False


def _check_inverses(ctx, element):
    for source_class, attributes in ctx.whitelisted_inverse_attributes.items():
        if not element.is_a(source_class):
            continue
        for attribute in attributes:
            attribute_class = None
            if "." in attribute:
                attribute, attribute_class = attribute.split(".")
            for inverse in getattr(element, attribute, []):
                if attribute_class and inverse.is_a(attribute_class):
                    _add_inverse_element(ctx, inverse)
                elif not attribute_class:
                    _add_inverse_element(ctx, inverse)


def _add_inverse_element(ctx, element):
    skip_not_reused_entities_attr_i = None
    if element.is_a("IfcPresentationLayerAssignment"):
        skip_not_reused_entities_attr_i = 2

    element_identity = element.wrapped_data.identity()

    existing_rel = None
    if (new := ctx.reuse_identities.get(element_identity)) is not None:
        if not new.is_a("IfcRelationship"):
            return
    elif element.is_a("IfcRelationship") and (existing_rel := _by_guid(ctx.file, element.GlobalId)):
        new = existing_rel
    else:
        new = ctx.file.create_entity(element.is_a())
        ctx.reuse_identities[element_identity] = new

    for i, attribute in enumerate(element):
        new_attribute = None
        if isinstance(attribute, ifcopenshell.entity_instance):
            if element.is_a() in ("IfcRelVoidsElement", "IfcRelProjectsElement") or not _is_another_asset(
                ctx, attribute
            ):
                new_attribute = _add_element(ctx, attribute)
        elif isinstance(attribute, tuple) and attribute and isinstance(attribute[0], ifcopenshell.entity_instance):
            new_attribute = []
            for item in attribute:
                if _is_another_asset(ctx, item):
                    continue
                if skip_not_reused_entities_attr_i is not None and i == skip_not_reused_entities_attr_i:
                    identity = item.wrapped_data.identity()
                    if (item := ctx.reuse_identities.get(identity)) is None:
                        continue
                else:
                    item = _add_element(ctx, item)
                new_attribute.append(item)
            if existing_rel:
                new_attribute.extend(existing_rel[i])
                new_attribute = list(set(new_attribute))
        else:
            new_attribute = attribute
        if new_attribute is not None:
            new[i] = new_attribute


def _is_another_asset(ctx, element):
    if element == ctx.element:
        return False
    elif element.is_a("IfcRoot") and _by_guid(ctx.file, element.GlobalId) is not None:
        return False
    elif element.is_a("IfcDistributionPort"):
        return False
    elif element.is_a(ctx.target_class):
        return True
    elif ctx.target_class == "IfcProduct" and element.is_a("IfcTypeProduct"):
        return True
    elif ctx.target_class == "IfcTypeProduct" and element.is_a("IfcProduct"):
        return True
    return False


def _reuse_existing_contexts(ctx):
    added_contexts = set([e for e in ctx.added_elements.values() if e.is_a("IfcGeometricRepresentationContext")])
    added_contexts -= set(ctx.existing_contexts)
    sorted_added_contexts = [c for c in added_contexts if c.is_a() == "IfcGeometricRepresentationContext"]
    sorted_added_contexts.extend([c for c in added_contexts if c.is_a() == "IfcGeometricRepresentationSubContext"])
    for added_context in sorted_added_contexts:
        equivalent_existing_context = _get_equivalent_existing_context(ctx, added_context)
        if not equivalent_existing_context:
            equivalent_existing_context = _create_equivalent_context(ctx, added_context)
        for inverse in ctx.file.get_inverse(added_context):
            ifcopenshell.util.element.replace_attribute(inverse, added_context, equivalent_existing_context)

    with SafeRemovalContext(ctx.file, ctx.reuse_identities, ctx.assume_asset_uniqueness_by_name):
        for added_context in added_contexts:
            ifcopenshell.util.element.remove_deep2(ctx.file, added_context)


def _get_equivalent_existing_context(ctx, added_context):
    for context in ctx.existing_contexts:
        if context.is_a() != added_context.is_a():
            continue
        if context.is_a("IfcGeometricRepresentationSubContext"):
            if (
                context.ContextType == added_context.ContextType
                and context.ContextIdentifier == added_context.ContextIdentifier
                and context.TargetView == added_context.TargetView
            ):
                return context
        elif (
            context.ContextType == added_context.ContextType
            and context.ContextIdentifier == added_context.ContextIdentifier
        ):
            return context


def _create_equivalent_context(ctx, added_context):
    if added_context.is_a("IfcGeometricRepresentationSubContext"):
        parent = _get_equivalent_existing_context(ctx, added_context.ParentContext)
        if not parent:
            parent = _create_equivalent_context(ctx, added_context.ParentContext)
            ctx.existing_contexts.append(parent)
        context = ifcopenshell.api.context.add_context(
            ctx.file,
            parent=parent,
            context_type=added_context.ContextType,
            context_identifier=added_context.ContextIdentifier,
            target_view=added_context.TargetView,
        )
    else:
        context = ifcopenshell.api.context.add_context(
            ctx.file,
            context_type=added_context.ContextType,
            context_identifier=added_context.ContextIdentifier,
        )
    ctx.existing_contexts.append(context)
    return context


def _file_add(ctx, element, conversion_factor=None):
    def get_conversion_factor():
        nonlocal conversion_factor
        if conversion_factor is not None:
            return conversion_factor
        library_scale = ifcopenshell.util.unit.calculate_unit_scale(ctx.library)
        current_scale = ifcopenshell.util.unit.calculate_unit_scale(ifc_file)
        conversion_factor = library_scale / current_scale
        return conversion_factor

    ifc_file = ctx.file
    if not ctx.assume_asset_uniqueness_by_name or element.id() == 0:
        if element.is_a("IfcLengthMeasure"):
            return ifc_file.create_entity(element.is_a(), element.wrappedValue * get_conversion_factor())
        return ifc_file.add(element)

    reuse_identities = ctx.reuse_identities
    element_identity = element.wrapped_data.identity()
    if added_element := reuse_identities.get(element_identity):
        return added_element

    ifc_class = element.is_a()
    attributes_ = None

    def get_attributes():
        nonlocal attributes_
        if attributes_ is not None:
            return attributes_
        attributes_ = element.wrapped_data.declaration().as_entity().all_attributes()
        return attributes_

    def get_existing_element_(subelement):
        element_identity = subelement.wrapped_data.identity()
        if subelement_ := reuse_identities.get(element_identity):
            return subelement_

        sub_ifc_class = subelement.is_a()
        assert sub_ifc_class in ("IfcOrganization", "IfcPerson")
        attr_name = "Id" if ifc_file.schema == "IFC2X3" else "Identification"
        subelement_id = getattr(subelement, attr_name)

        if subelement_id is not None:
            existing_org = next(
                (e for e in ifc_file.by_type(sub_ifc_class) if getattr(e, attr_name) == subelement_id), None
            )
            if existing_org is not None:
                reuse_identities[element_identity] = existing_org
                return existing_org

    if element.is_a("IfcProfileDef"):
        profile_name = element.ProfileName
        if profile_name is not None:
            existing_profile = next(
                (e for e in ifc_file.by_type("IfcProfileDef") if e.ProfileName == profile_name), None
            )
            if existing_profile is not None:
                reuse_identities[element_identity] = existing_profile
                return existing_profile

    elif element.is_a("IfcMaterial"):
        material_name = element.Name
        existing_material = next((e for e in ifc_file.by_type("IfcMaterial") if e.Name == material_name), None)
        if existing_material is not None:
            reuse_identities[element_identity] = existing_material
            return existing_material

    elif ifc_class in MATERIAL_SETS:
        name_attr = "LayerSetName" if ifc_class == "IfcMaterialLayerSet" else "Name"
        material_set_name = getattr(element, name_attr)
        if material_set_name is not None:
            for candidate in ifc_file.by_type(ifc_class):
                if getattr(candidate, name_attr) == material_set_name:
                    if _material_sets_are_equal(element, candidate):
                        reuse_identities[element_identity] = candidate
                        return candidate

    elif element.is_a("IfcPresentationStyle"):
        style_name = element.Name
        if style_name is not None:
            existing_style = next((e for e in ifc_file.by_type(ifc_class) if e.Name == style_name), None)
            if existing_style is not None:
                reuse_identities[element_identity] = existing_style
                return existing_style

    elif ifc_class == "IfcApplication":
        app_id = element.ApplicationIdentifier
        if app_id is not None:
            existing_app = next(
                (e for e in ifc_file.by_type("IfcApplication") if e.ApplicationIdentifier == app_id), None
            )
            if existing_app is not None:
                reuse_identities[element_identity] = existing_app
                return existing_app

    elif ifc_class == "IfcOrganization":
        existing_org = get_existing_element_(element)
        if existing_org is not None:
            reuse_identities[element_identity] = existing_org
            return existing_org

    elif ifc_class == "IfcPerson":
        existing_person = get_existing_element_(element)
        if existing_person is not None:
            reuse_identities[element_identity] = existing_person
            return existing_person

    elif ifc_class == "IfcPersonAndOrganization":
        if (person := get_existing_element_(element.ThePerson)) and (
            org := get_existing_element_(element.TheOrganization)
        ):
            for pao in ifc_file.by_type("IfcPersonAndOrganization"):
                if pao.ThePerson == person and pao.TheOrganization == org:
                    reuse_identities[element_identity] = pao
                    return pao

    attrs = {}

    def get_tuple_type(tuple_):
        while isinstance(tuple_, tuple):
            tuple_ = tuple_[0]
        return type(tuple_)

    def is_length_measure(attribute):
        return "<type IfcLengthMeasure: <real>>" in str(attribute.type_of_attribute())

    def apply_to_array(arr, func):
        if isinstance(arr, tuple):
            return tuple(apply_to_array(sub, func) for sub in arr)
        return func(arr)

    file_add_ = partial(_file_add, ctx, conversion_factor=conversion_factor)
    apply_conversion = lambda x: x * conversion_factor

    for attr_index, attr_value in enumerate(element):
        if attr_value is None:
            continue

        elif isinstance(attr_value, ifcopenshell.entity_instance):
            attr_value = file_add_(attr_value)

        elif isinstance(attr_value, tuple):
            tuple_type = get_tuple_type(attr_value)
            if tuple_type == ifcopenshell.entity_instance:
                attr_value = apply_to_array(attr_value, file_add_)
            elif tuple_type == float:
                attributes = get_attributes()
                if is_length_measure(attributes[attr_index]):
                    get_conversion_factor()
                    attr_value = apply_to_array(attr_value, apply_conversion)

        elif isinstance(attr_value, float):
            attributes = get_attributes()
            if is_length_measure(attributes[attr_index]):
                attr_value *= get_conversion_factor()

        attrs[attr_index] = attr_value

    new = ifc_file.create_entity(ifc_class)
    reuse_identities[element_identity] = new
    for attr_index, attr_value in attrs.items():
        new[attr_index] = attr_value

    return new
