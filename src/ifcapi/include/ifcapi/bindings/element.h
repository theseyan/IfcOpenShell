/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ELEMENT_H
#define IFCAPI_BINDINGS_ELEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/// Options for retrieving the spatial container of an element.
struct ElementGetContainerOptions {
    /// When true, only return a directly containing spatial element. When false (default), walk up the hierarchy.
    std::optional<bool> direct_only;
    /// If set, only return a container that is of this IFC class (e.g. "IfcBuildingStorey").
    std::optional<std::string> ifc_class;
};

/// Options for retrieving the material of an element.
struct ElementGetMaterialOptions {
    /// When true, unwrap IfcMaterialLayerSetUsage and IfcMaterialProfileSetUsage to their underlying set.
    std::optional<bool> should_skip_usage;
    /// When true (default), inherit the material from the element's type if the element has no direct material.
    std::optional<bool> should_inherit;
};

/// Options for retrieving shape aspects of an element.
struct ElementGetShapeAspectsOptions {
    /// When true (default), include shape aspects inherited from the element's type.
    std::optional<bool> should_inherit;
};

/// Options for retrieving the decomposition of an element.
struct ElementGetDecompositionOptions {
    /// When true (default), recursively traverse the spatial decomposition tree. When false, return only direct children.
    std::optional<bool> is_recursive;
};

/// Options for retrieving property set identifiers of an element.
struct ElementGetPsetIdsOptions {
    /// When true, exclude element quantities and return only property sets.
    std::optional<bool> psets_only;
    /// When true, exclude property sets and return only element quantities.
    std::optional<bool> qtos_only;
    /// When true (default), include property sets inherited from the element's type.
    std::optional<bool> should_inherit;
};

/**
 * Return the type element associated with an element occurrence.
 *
 * For an IfcTypeObject, returns the element itself. For an IfcObject,
 * follows IsTypedBy (IFC4+) or IsDefinedBy/IfcRelDefinesByType (IFC2X3).
 *
 * @param instance The element to query.
 * @return The related type element, or empty if none.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_type(express::Base* instance);

/**
 * Return the aggregate parent of an element.
 *
 * Follows the Decomposes inverse to find the RelatingObject via
 * IfcRelAggregates. In IFC2X3, returns empty if the relationship
 * is IfcRelNests rather than IfcRelAggregates.
 *
 * @param instance The element to query.
 * @return The aggregate parent, or empty if not aggregated.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_aggregate(express::Base* instance);

/**
 * Return the nest parent of an element.
 *
 * Follows the Nests inverse (IFC4+) or Decomposes/IfcRelNests (IFC2X3)
 * to find the RelatingObject.
 *
 * @param instance The element to query.
 * @return The nesting parent, or empty if not nested.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_nest(express::Base* instance);

/**
 * Return the spatial container of an element.
 *
 * By default walks up the spatial hierarchy to find an indirect container
 * (e.g. a building storey for an element inside an aggregate). When
 * direct_only is true, only a direct ContainedInStructure relationship
 * is considered.
 *
 * @param instance The element to query.
 * @param options Container lookup options.
 * @return The spatial container, or empty if not contained.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_container(
    express::Base* instance,
    const ElementGetContainerOptions& options);

/**
 * Return the parent of an element in the spatial hierarchy.
 *
 * Checks spatial containment, aggregation, nesting, filling, and voiding
 * relationships in that order, returning the first parent found.
 *
 * @param instance The element to query.
 * @return The parent element, or empty if at the top of the hierarchy.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_parent(express::Base* instance);

/**
 * Return the material associated with an element.
 *
 * Searches HasAssociations for IfcRelAssociatesMaterial. When
 * should_inherit is true (default), falls back to the element's type.
 * When should_skip_usage is true, unwraps material set usages
 * (IfcMaterialLayerSetUsage, IfcMaterialProfileSetUsage) to their
 * underlying set.
 *
 * @param instance The element to query.
 * @param options Material lookup options.
 * @return The material entity, or empty if none is associated.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_material(
    express::Base* instance,
    const ElementGetMaterialOptions& options);

/**
 * Return all occurrences of a type element.
 *
 * Follows Types (IFC4+) or ObjectTypeOf (IFC2X3) to find the
 * RelatedObjects.
 *
 * @param type_element The type element (e.g. IfcWallType).
 * @return List of element occurrences of that type.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_types(express::Base* type_element);

/**
 * Return the shape aspects of an element.
 *
 * For an IfcProduct, reads HasShapeAspects from the Representation.
 * For an IfcTypeProduct, reads from RepresentationMaps. When
 * should_inherit is true (default), also includes shape aspects from
 * the element's type.
 *
 * @param element The IfcProduct or IfcTypeProduct.
 * @param options Shape aspect lookup options.
 * @return List of IfcShapeAspect entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_shape_aspects(
    express::Base* element,
    const ElementGetShapeAspectsOptions& options);

/**
 * Return the groups that an element is assigned to.
 *
 * Follows HasAssignments to find IfcRelAssignsToGroup relationships.
 *
 * @param element The element to query.
 * @return List of IfcGroup entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_groups(express::Base* element);

/**
 * Return the controls assigned to an element.
 *
 * Follows HasAssignments to find IfcRelAssignsToControl relationships.
 *
 * @param element The element to query.
 * @return List of IfcControl entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_controls(express::Base* element);

/**
 * Return the direct aggregation parts of an element.
 *
 * Follows IsDecomposedBy to find RelatedObjects via IfcRelAggregates.
 *
 * @param element The element to query.
 * @return List of aggregated parts.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_parts(express::Base* element);

/**
 * Return elements directly contained in a spatial element.
 *
 * Follows ContainsElements to find RelatedElements.
 *
 * @param element The spatial element (e.g. IfcBuildingStorey).
 * @return List of contained elements.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_contained(express::Base* element);

/**
 * Return spatial elements that reference an element.
 *
 * Follows ReferencedInStructures to find RelatingStructure.
 * Useful for multi-storey elements or elements spanning multiple spaces.
 *
 * @param element The element to query.
 * @return List of referenced IfcSpatialElement entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_referenced_structures(express::Base* element);

/**
 * Return elements referenced by a spatial structure.
 *
 * Follows ReferencesElements to find RelatedElements.
 *
 * @param structure The spatial element (e.g. IfcBuildingStorey).
 * @return List of referenced elements.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_structure_referenced_elements(express::Base* structure);

/**
 * Return opening elements associated with an element.
 *
 * Follows HasOpenings to find RelatedOpeningElement. Also traverses
 * aggregate parents to collect inherited openings.
 *
 * @param element The building element (e.g. IfcWall).
 * @return List of IfcOpeningElement entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_openings(express::Base* element);

/**
 * Return the opening element that an element fills.
 *
 * Follows FillsVoids to find the RelatingOpeningElement.
 * Typically applies to windows and doors.
 *
 * @param element The filling element (e.g. IfcWindow).
 * @return The IfcOpeningElement being filled, or empty if none.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_filled_void(express::Base* element);

/**
 * Return the building element voided by an opening.
 *
 * Follows VoidsElements to find the RelatingBuildingElement.
 *
 * @param element The IfcOpeningElement.
 * @return The building element being voided, or empty if none.
 */
IFCAPI_BINDING std::optional<express::Base> element_get_voided_element(express::Base* element);

/**
 * Check whether an element's predefined type is user-defined.
 *
 * Returns true if the element's PredefinedType is USERDEFINED, or if
 * the type's PredefinedType is USERDEFINED or not set and an
 * ElementType or ObjectType is present.
 *
 * @param element The element to check.
 * @return True if the predefined type is user-defined.
 */
IFCAPI_BINDING bool element_is_userdefined_type(express::Base* element);

/**
 * Return elements that have an external reference assigned.
 *
 * For IfcExternalReference subtypes, follows ExternalReferenceForResources.
 * For classification/document/library references, follows the appropriate
 * inverse attribute.
 *
 * @param reference The IfcExternalReference or IfcExternalInformation entity.
 * @return List of elements using the reference.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_referenced_elements(express::Base* reference);

/**
 * Return elements that use a material, directly or via a material set.
 *
 * Traverses inverse relationships from the material to find all elements
 * associated through IfcRelAssociatesMaterial, as well as elements using
 * the material as part of a layer, profile, constituent, or material list.
 *
 * @param material The IfcMaterial or material set entity.
 * @return List of elements using the material.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_material(express::Base* material);

/**
 * Return elements whose geometric representation uses a style.
 *
 * Traverses from IfcSurfaceStyle through IfcStyledItem and
 * IfcShapeRepresentation to find all elements using the style.
 *
 * @param style The IfcPresentationStyle entity.
 * @return List of elements using the style.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_style(express::Base* style);

/**
 * Return elements that use a geometric representation.
 *
 * Follows OfProductRepresentation and RepresentationMap to find all
 * IfcProduct and IfcTypeProduct entities sharing the representation.
 *
 * @param representation The IfcShapeRepresentation entity.
 * @return List of elements using the representation.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_representation(express::Base* representation);

/**
 * Return elements that use a profile definition in their representation.
 *
 * Traverses from the IfcProfileDef through representation items to find
 * all elements whose geometry references the profile.
 *
 * @param profile The IfcProfileDef entity.
 * @return List of elements using the profile.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_profile(express::Base* profile);

/**
 * Return elements assigned to a presentation layer.
 *
 * Follows AssignedItems on the IfcPresentationLayerAssignment to find
 * all elements whose geometry is on the layer.
 *
 * @param layer The IfcPresentationLayerAssignment entity.
 * @return List of elements on the layer.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_layer(express::Base* layer);

/**
 * Return the presentation layers that an element is part of.
 *
 * Traverses the element's representation to find IfcPresentationLayerAssignment
 * entities.
 *
 * @param element The element to query.
 * @return List of IfcPresentationLayerAssignment entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_layers(express::Base* element);

/**
 * Return the surface styles used in an element's representation.
 *
 * Collects styles from the element's materials and body representation,
 * including styles on mapped items and boolean operands.
 *
 * @param element The element to query.
 * @return List of IfcSurfaceStyle entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_styles(express::Base* element);

/**
 * Replace all references to an element with another element.
 *
 * Traverses all inverse relationships of old_element and substitutes
 * references to old_element with new_element.
 *
 * @param old_element The element to be replaced.
 * @param new_element The replacement element.
 */
IFCAPI_BINDING void element_replace_element(
    express::Base* old_element,
    express::Base* new_element);

/**
 * Recursively remove an element and its owned subgraph.
 *
 * Traverses forward through the element's subgraph. Each subelement is
 * deleted only if it has no inverses outside the subgraph. Protected
 * elements and elements with external references are preserved.
 *
 * @param element The root element to remove.
 */
IFCAPI_BINDING void element_remove_deep(express::Base* element);

/**
 * Return the full spatial decomposition of an element.
 *
 * Collects all subelements by traversing ContainsElements,
 * IsDecomposedBy, HasOpenings, HasFillings, and IsNestedBy
 * relationships. When is_recursive is true (default), the traversal
 * is breadth-first through the entire hierarchy.
 *
 * @param element The root element.
 * @param options Decomposition traversal options.
 * @return Ordered list of decomposed subelements (excluding the root).
 */
IFCAPI_BINDING std::vector<express::Base> element_get_decomposition(
    express::Base* element,
    const ElementGetDecompositionOptions& options);

/**
 * Return property set and quantity identifiers of an element.
 *
 * Collects IfcPropertySet, IfcElementQuantity, and related property
 * definition entities. For IfcTypeObject, reads HasPropertySets.
 * For other objects, reads IsDefinedBy/IfcRelDefinesByProperties.
 * When should_inherit is true (default), also includes property sets
 * from the element's type.
 *
 * @param element The element to query.
 * @param options Property set filtering options.
 * @return List of property definition entities.
 */
IFCAPI_BINDING std::vector<express::Base> element_get_pset_ids(
    express::Base* element,
    const ElementGetPsetIdsOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ELEMENT_H */
