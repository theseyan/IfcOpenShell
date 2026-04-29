// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_H
#define IFCAPI_H

/// @file ifcapi.h
/// @brief High-level IFC authoring C API.
///
/// This library reimplements the business logic from ifcopenshell.api (Python)
/// in C++, exposed as a plain C API. Functions accept raw IfcParse::IfcFile*
/// pointers (as void*) and STEP entity IDs, making them callable from any
/// language with C FFI support.
///
/// Error handling follows the thread-local pattern: on failure a function
/// returns a sentinel value and the error message is retrievable via
/// ifcopenshell_last_error_message(). Call ifcopenshell_clear_error() before a sequence of calls
/// if you need precise attribution.

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
#  ifdef IFCAPI_BUILD
#    define IFCAPI_EXPORT __declspec(dllexport)
#  else
#    define IFCAPI_EXPORT __declspec(dllimport)
#  endif
#else
#  define IFCAPI_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Opaque handle types (defined by the autogen layer in              */
/*  ifcopenshell_api.h). Forward-declare them here so this header     */
/*  stays standalone — both layers share the same struct definitions  */
/*  and consumers can mix calls freely.                               */
/* ------------------------------------------------------------------ */

typedef struct ifcopenshell_ifc_file_t ifcopenshell_ifc_file_t;
typedef struct ifcopenshell_ifc_instance_t ifcopenshell_ifc_instance_t;
typedef struct ifcopenshell_ifc_instance_streamer_t ifcopenshell_ifc_instance_streamer_t;
typedef struct ifcopenshell_ifcgeom_taxonomy_item_t ifcopenshell_ifcgeom_taxonomy_item_t;

/* ------------------------------------------------------------------ */
/*  Error handling                                                     */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* The high-level layer reports errors through the same thread-local  */
/* string used by the autogen low-level layer. Use the autogen        */
/* entry points ifcopenshell_last_error_message() and                 */
/* ifcopenshell_clear_error() (declared in ifcopenshell_api.h) to     */
/* read and reset it.                                                 */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT void ifcopenshell_free_string(char* str);

/* ------------------------------------------------------------------ */
/*  File operations                                                    */
/* ------------------------------------------------------------------ */

/// Frees an array of instance handles returned by ifcapi functions.
/// Each handle wrapper is destroyed (the underlying entities remain owned by the file).
IFCAPI_EXPORT void ifcopenshell_free_instance_array(ifcopenshell_ifc_instance_t** arr, uint32_t count);

/// Frees an int32_t array returned by the API.
IFCAPI_EXPORT void ifcopenshell_free_int32_array(int32_t* arr);

/* ------------------------------------------------------------------ */
/*  Entity attribute type tags                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    IFCAPI_ATTR_NULL       = 0,
    IFCAPI_ATTR_STRING     = 1,
    IFCAPI_ATTR_INT        = 2,
    IFCAPI_ATTR_DOUBLE     = 3,
    IFCAPI_ATTR_BOOL       = 4,
    IFCAPI_ATTR_ENUM       = 5,
    IFCAPI_ATTR_REFERENCE  = 6,
    IFCAPI_ATTR_AGGREGATE  = 7,
    IFCAPI_ATTR_DERIVED    = 8,
    IFCAPI_ATTR_UNKNOWN    = 99
} ifcopenshell_attr_type;

/* ------------------------------------------------------------------ */
/*  Entity operations                                                  */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/*  Aggregate memory management                                        */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT void ifcopenshell_free_string_array(char** arr, uint32_t count);

/* ------------------------------------------------------------------ */
/*  Inverse attribute access                                           */
/* ------------------------------------------------------------------ */

/// Returns true if the given name is an inverse attribute on the entity.
/* ------------------------------------------------------------------ */
/*  Utility: deep removal                                              */
/* ------------------------------------------------------------------ */

/// Recursively remove an entity and any entities it references that would
/// become orphaned (total_inverses == 0 after removal).
IFCAPI_EXPORT void ifcopenshell_util_remove_deep2(ifcopenshell_ifc_instance_t* instance);

/* ------------------------------------------------------------------ */
/*  High-level: root module                                            */
/* ------------------------------------------------------------------ */

/// Creates a new rooted IFC entity.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_root_create_entity(
    ifcopenshell_ifc_file_t* file,
    const char* ifc_class,
    const char* predefined_type,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history);

/* ------------------------------------------------------------------ */
/*  High-level: aggregate module                                       */
/* ------------------------------------------------------------------ */

/// Assign products as parts of a whole (IfcRelAggregates).
/// Returns the STEP ID of the IfcRelAggregates entity, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_aggregate_assign_object(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* relating_object);

/// Unassign products from their aggregate.
IFCAPI_EXPORT void ifcopenshell_aggregate_unassign_object(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count);

/* ------------------------------------------------------------------ */
/*  High-level: spatial module                                         */
/* ------------------------------------------------------------------ */

/// Assign products to a spatial container (IfcRelContainedInSpatialStructure).
/// Returns the STEP ID of the relationship entity, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_spatial_assign_container(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* relating_structure);

/// Unassign products from their spatial container.
IFCAPI_EXPORT void ifcopenshell_spatial_unassign_container(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count);

/* ------------------------------------------------------------------ */
/*  High-level: type module                                            */
/* ------------------------------------------------------------------ */

/// Assign objects to a type (IfcRelDefinesByType).
/// Returns the STEP ID of the IfcRelDefinesByType entity, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_type_assign_type(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type);

/// Variant of ifcopenshell_type_assign_type that takes a `should_map_representations`
/// flag. When true (the default), this also
/// propagates the relating type's IfcRepresentationMaps onto each related
/// object as IfcMappedItem-based IfcShapeRepresentations and clears any
/// PredefinedType/ObjectType to avoid double-typing.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_type_assign_type_ex(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_type,
    bool should_map_representations);

/// Replace `related_object`'s representations with mapped versions of every
/// IfcRepresentationMap attached to `relating_type`. Existing representations
/// on `related_object` are unassigned and removed first.
IFCAPI_EXPORT bool ifcopenshell_type_map_type_representations(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* related_object,
    ifcopenshell_ifc_instance_t* relating_type);

/// Unassign objects from their type.
IFCAPI_EXPORT void ifcopenshell_type_unassign_type(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count);

/* ------------------------------------------------------------------ */
/*  High-level: nest module                                            */
/* ------------------------------------------------------------------ */

/// Assign objects as nested children of a host (IfcRelNests).
/// Returns the STEP ID of the IfcRelNests entity, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_nest_assign_object(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count,
    ifcopenshell_ifc_instance_t* relating_object);

/// Unassign objects from their nest.
IFCAPI_EXPORT void ifcopenshell_nest_unassign_object(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** objects,
    uint32_t object_count);

/* ------------------------------------------------------------------ */
/*  High-level: group module                                           */
/* ------------------------------------------------------------------ */

/// Assign products to a group (IfcRelAssignsToGroup).
/// Returns the STEP ID of the IfcRelAssignsToGroup entity, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_group_assign_group(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* group);

/// Unassign products from a group.
IFCAPI_EXPORT void ifcopenshell_group_unassign_group(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t** products,
    uint32_t product_count,
    ifcopenshell_ifc_instance_t* group);

/* ------------------------------------------------------------------ */
/*  High-level: placement utilities                                    */
/* ------------------------------------------------------------------ */
/*                                                                     */
/*  All matrices are 4x4, row-major, passed as double[16]. Row-major   */
/*  layout means m[i][j] == out[i*4 + j]. The translation is in the    */
/*  last column (out[3], out[7], out[11]).                             */

/// Build a 4x4 placement matrix from origin O, Z axis, and X axis vectors.
IFCAPI_EXPORT void ifcopenshell_placement_a2p(
    const double o[3], const double z[3], const double x[3], double out[16]);

/// Parse an IfcAxis2Placement (3D, 2D, 1D) into a 4x4 matrix.
/// Returns false if the entity is not an axis placement or uses an
/// unsupported form (e.g. IfcAxis2PlacementLinear without coordinates).
IFCAPI_EXPORT bool ifcopenshell_placement_get_axis2placement(const ifcopenshell_ifc_instance_t* instance, double out[16]);

/// Parse an IfcLocalPlacement into a 4x4 matrix, recursing through
/// PlacementRelTo. Passing id=0 returns identity with true.
IFCAPI_EXPORT bool ifcopenshell_placement_get_local_placement(const ifcopenshell_ifc_instance_t* instance, double out[16]);

/// Parse an IfcCartesianTransformationOperator3D(nonUniform) into a 4x4 matrix.
IFCAPI_EXPORT bool ifcopenshell_placement_get_cartesian_xform_3d(const ifcopenshell_ifc_instance_t* instance, double out[16]);

/// Parse an IfcMappedItem's placement (source mapping origin composed with target op).
IFCAPI_EXPORT bool ifcopenshell_placement_get_mappeditem_xform(const ifcopenshell_ifc_instance_t* instance, double out[16]);

/// Get the Z elevation of an IfcBuildingStorey (from placement, or Elevation attr).
IFCAPI_EXPORT bool ifcopenshell_placement_get_storey_elevation(const ifcopenshell_ifc_instance_t* instance, double* out);

/// Build a 4x4 rotation matrix around one principal axis ('X', 'Y', or 'Z').
IFCAPI_EXPORT void ifcopenshell_placement_rotation(
    double angle_rad, char axis, double out[16]);

/* ------------------------------------------------------------------ */
/*  High-level: element graph utilities                                */
/* ------------------------------------------------------------------ */

/// Get the full decomposition (recursive if is_recursive is true) of an element.
/// Traverses ContainsElements, IsDecomposedBy, HasOpenings, HasFillings, IsNestedBy.
/// Caller must free the returned array with ifcopenshell_free_instance_array(). Returns NULL when empty.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_element_get_decomposition(const ifcopenshell_ifc_instance_t* instance, bool is_recursive, uint32_t* out_count);

/// Returns STEP IDs of property-set / element-quantity definitions applicable
/// to the given element, including inherited-from-type when should_inherit is
/// true. Returns NULL when empty. Caller frees with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_element_get_pset_ids(const ifcopenshell_ifc_instance_t* instance,
    bool psets_only, bool qtos_only, bool should_inherit,
    uint32_t* out_count);

/* ------------------------------------------------------------------ */
/*  High-level: representation utilities                               */
/* ------------------------------------------------------------------ */

/// Returns the first matching IfcGeometricRepresentation[Sub]Context, or 0.
/// NULL/empty strings mean "any". If subcontext or target_view are non-empty,
/// the search is constrained to subcontexts.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_representation_get_context(
    const ifcopenshell_ifc_file_t* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view);

/// Unwraps a representation whose single item is an IfcMappedItem pointing at
/// another representation. Returns the resolved representation's STEP ID, or 0.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_representation_resolve(const ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* rep);

/// Gets the first IfcShapeRepresentation on *element_id* that matches the
/// given context. Pass context_id != 0 to match a specific context entity;
/// otherwise context_type / subcontext / target_view strings are used
/// (upstream `is_representation_of_context` semantics). Returns 0 if none.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_representation_get_product_representation(
    const ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    ifcopenshell_ifc_instance_t* context,
    const char* context_type,
    const char* subcontext,
    const char* target_view);

/// Resolve a representation to its leaf items, recursively expanding IfcMappedItem
/// (via MappingSource.MappedRepresentation) and IfcBooleanResult (both operands).
/// Returns a heap-allocated array of STEP IDs. Caller must free via ifcopenshell_free_instance_array.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_representation_resolve_base_items(
    const ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* rep, uint32_t* out_count);

/// Returns all IfcGeometricRepresentationContext (and SubContext) entities sorted
/// from high priority to low priority (see util/representation.py:get_prioritised_contexts).
/// Caller must free via ifcopenshell_free_instance_array.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_representation_get_prioritised_contexts(
    const ifcopenshell_ifc_file_t* file, uint32_t* out_count);

/* ------------------------------------------------------------------ */
/*  High-level: geometry module (representation assignment)            */
/* ------------------------------------------------------------------ */

/// Assign a representation to a product.
///
/// If `product` is an IfcProduct that is typed by an IfcTypeProduct that
/// already has IfcRepresentationMaps (and the new representation is not
/// itself a MappedRepresentation), the assignment is silently re-routed to
/// the type.
///
/// If `product` is an IfcTypeProduct, a new IfcRepresentationMap is created
/// for it and a MappedRepresentation is added to every related occurrence.
///
/// Always updates the OwnerHistory of the (possibly re-routed) product.
/// Returns the (possibly re-routed) product on success, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_geometry_assign_representation(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* product,
    ifcopenshell_ifc_instance_t* representation);

/// Build a new IfcShapeRepresentation of type "MappedRepresentation" that
/// references `representation` via an IfcMappedItem. If `representation`
/// already has an IfcRepresentationMap among its inverses, that map is
/// reused; otherwise a new IfcRepresentationMap is created.
/// Returns the new IfcShapeRepresentation, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_geometry_map_representation(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* representation);

/// Remove `representation` from `product`'s IfcProductDefinitionShape
/// (or IfcMaterialDefinitionRepresentation). The representation entity
/// itself is not deleted; use ifcopenshell_geometry_remove_representation
/// for that.
IFCAPI_EXPORT void ifcopenshell_geometry_unassign_representation(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* product,
    ifcopenshell_ifc_instance_t* representation);

/// Copy a named representation (by `context_identifier`, e.g. "Body") from
/// `source` to `target`, deep-copying the entity graph and replacing any
/// existing matching representation on `target`. Returns the new
/// representation, or 0 if `source` has no matching representation.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_geometry_copy_representation(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* source,
    ifcopenshell_ifc_instance_t* target,
    const char* context_identifier);

/// Compute the axis-aligned 2D bounding-box extents of an IfcProfileDef.
/// Handles parametric profiles (Rectangle, Circle, I/L/T/U/Z/C/Ellipse/
/// AsymmetricI shape) directly from their attributes, and computes the
/// extents of IfcArbitraryClosedProfileDef from its OuterCurve points
/// (IfcIndexedPolyCurve / IfcPolyline). Writes width to `*x_out` and
/// height to `*y_out`. Returns true on success.
IFCAPI_EXPORT bool ifcopenshell_geometry_profile_extents(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* profile,
    double* x_out,
    double* y_out);

/* parameter_type kind tag */
#define IFCAPI_PT_NAMED       0
#define IFCAPI_PT_SIMPLE      1
#define IFCAPI_PT_AGGREGATION 2

/* simple_type::data_type values (match IfcSchema.h enum) */
#define IFCAPI_SIMPLE_BINARY   0
#define IFCAPI_SIMPLE_BOOLEAN  1
#define IFCAPI_SIMPLE_INTEGER  2
#define IFCAPI_SIMPLE_LOGICAL  3
#define IFCAPI_SIMPLE_NUMBER   4
#define IFCAPI_SIMPLE_REAL     5
#define IFCAPI_SIMPLE_STRING   6

/* aggregation_type::aggregate_type values */
#define IFCAPI_AGG_ARRAY 0
#define IFCAPI_AGG_BAG   1
#define IFCAPI_AGG_LIST  2
#define IFCAPI_AGG_SET   3

/* inverse_attribute::aggregate_type values */
#define IFCAPI_INV_AGG_BAG         0
#define IFCAPI_INV_AGG_SET         1
#define IFCAPI_INV_AGG_UNSPECIFIED 2

/* ------------------------------------------------------------------ */
/*  util.attribute                                                     */
/* ------------------------------------------------------------------ */
/* Returns the leaf-level primitive type string for an attribute. One of:
 *   "string", "float", "integer", "boolean", "entity", "enum", "select",
 *   "binary", "logical", "aggregate", "derived", "unknown".
 * For aggregate / select attributes the caller is expected to walk the
 * parameter_type tree separately when nested results are required.
 * Returned pointer is a static string literal — caller must NOT free. */
IFCAPI_EXPORT const char* ifcopenshell_util_attribute_get_primitive_type(const void* attribute);

/* Returns a heap-allocated NULL-terminated array of enum item C strings
 * (length written to *out_count). Caller must free with
 * ifcopenshell_free_string_array(arr, *out_count). Returns NULL with
 * *out_count == 0 if the attribute is not an enumeration. */
IFCAPI_EXPORT char** ifcopenshell_util_attribute_get_enum_items(
    const void* attribute, uint32_t* out_count);

/* ------------------------------------------------------------------ */
/*  util.classification                                                */
/* ------------------------------------------------------------------ */
/* Returns the IfcClassificationReference instances associated with
 * `element` (de-duplicated, with type-inheritance rules matching
 * ifcopenshell.util.classification.get_references). Caller must free with
 * ifcopenshell_free_instance_array. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_classification_get_references(
    ifcopenshell_ifc_instance_t* element,
    bool should_inherit,
    uint32_t* out_count);

/* ------------------------------------------------------------------ */
/*  util.element.get_styles                                            */
/* ------------------------------------------------------------------ */
/* Returns the IfcSurfaceStyle entities associated with `element`
 * (drawn from material representations and from the element's body
 * representation). Caller must free with ifcopenshell_free_instance_array. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_styles(
    ifcopenshell_ifc_instance_t* element,
    uint32_t* out_count);

/* ------------------------------------------------------------------ */
/*  util.element traversal helpers                                     */
/* ------------------------------------------------------------------ */

/* Get all occurrences of a type element. Caller frees with
 * ifcopenshell_free_instance_array. Returns NULL when empty. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_types(
    ifcopenshell_ifc_instance_t* type_element, uint32_t* out_count);

/* Get IfcShapeAspect entities associated with a product or product type. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_shape_aspects(
    ifcopenshell_ifc_instance_t* element, bool should_inherit, uint32_t* out_count);

/* Get IfcGroup entities `element` is assigned to via IfcRelAssignsToGroup. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_groups(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get IfcControl entities assigned to `element` via IfcRelAssignsToControl. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_controls(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get the parts of `element` that have an aggregation relationship. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_parts(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get the contained elements of a spatial element. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_contained(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get IfcSpatialElement entities referenced by `element`'s ReferencedInStructures. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_referenced_structures(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get the elements referenced by `structure` via IfcRelReferencedInSpatialStructure. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_structure_referenced_elements(
    ifcopenshell_ifc_instance_t* structure, uint32_t* out_count);

/* Get IfcRelVoidsElements relationships for `element`, recursing through aggregates. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_openings(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Get the IfcOpeningElement filled by `element`, or NULL. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_element_get_filled_void(
    ifcopenshell_ifc_instance_t* element);

/* Get the building element voided by an opening, or NULL. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_element_get_voided_element(
    ifcopenshell_ifc_instance_t* element);

/* True if the predefined type of `element` is userdefined. */
IFCAPI_EXPORT bool ifcopenshell_util_element_is_userdefined_type(
    ifcopenshell_ifc_instance_t* element);

/* Returns elements with `reference` (IfcExternalReference subtype) assigned. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_referenced_elements(
    ifcopenshell_ifc_instance_t* reference, uint32_t* out_count);

/* Returns all elements (including via material sets/usages/lists) that use `material`. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_elements_by_material(
    ifcopenshell_ifc_instance_t* material, uint32_t* out_count);

/* Returns all elements whose representation uses `style`. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_elements_by_style(
    ifcopenshell_ifc_instance_t* style, uint32_t* out_count);

/* Returns all elements using a geometric IfcShapeRepresentation. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_elements_by_representation(
    ifcopenshell_ifc_instance_t* representation, uint32_t* out_count);

/* Returns elements whose representation uses an IfcProfileDef. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_elements_by_profile(
    ifcopenshell_ifc_instance_t* profile, uint32_t* out_count);

/* Returns elements whose geometry is assigned to a presentation layer. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_elements_by_layer(
    ifcopenshell_ifc_instance_t* layer, uint32_t* out_count);

/* Returns the presentation layers that any portion of `element`'s geometry is on. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_layers(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count);

/* Replace every occurrence of `old_element` with `new_element` in the file's graph. */
IFCAPI_EXPORT void ifcopenshell_util_element_replace_element(
    ifcopenshell_ifc_instance_t* old_element,
    ifcopenshell_ifc_instance_t* new_element);

/* Recursively purges a subgraph rooted at `element` (only entities whose inverses are
 * fully contained in the subgraph). See ifcopenshell.util.element.remove_deep. */
IFCAPI_EXPORT void ifcopenshell_util_element_remove_deep(
    ifcopenshell_ifc_instance_t* element);


#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ */
/*  Selector parser AST                                                */
/* ------------------------------------------------------------------ */
#include "ifcapi/selector/ast.h"

/* ------------------------------------------------------------------ */
/*  Selector value type + get_element_value                           */
/* ------------------------------------------------------------------ */
#include "ifcapi/value.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Selector format(): evaluates a format query string                 */
/* ------------------------------------------------------------------ */

/* Returns a freshly malloc'd UTF-8 string with the formatted result.
   Returns NULL if the query evaluated to None (e.g. {{undefined}} with no
   element) or on a parse / evaluation error. Use
   ifcopenshell_last_error_message() to disambiguate. The caller must free
   the returned buffer with ifcopenshell_free_string(). Both file and
   instance may be NULL, in which case variable substitutions evaluate to
   None. */
IFCAPI_EXPORT char* ifcopenshell_selector_format(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* instance,
    const char* query);

/* ------------------------------------------------------------------ */
/*  Selector key extraction (used by set_element_value)                */
/* ------------------------------------------------------------------ */

typedef struct ifcopenshell_selector_keys_t ifcopenshell_selector_keys_t;

IFCAPI_EXPORT ifcopenshell_selector_keys_t*
ifcopenshell_selector_parse_keys(const char* query);

IFCAPI_EXPORT uint32_t
ifcopenshell_selector_keys_count(ifcopenshell_selector_keys_t* h);

/* Returns the raw text of the i-th key. For regex keys the text is the
   pattern source (no surrounding slashes). The pointer is valid until
   ifcopenshell_selector_keys_free(). */
IFCAPI_EXPORT const char*
ifcopenshell_selector_keys_get(ifcopenshell_selector_keys_t* h, uint32_t i);

IFCAPI_EXPORT bool
ifcopenshell_selector_keys_is_regex(ifcopenshell_selector_keys_t* h, uint32_t i);

IFCAPI_EXPORT void
ifcopenshell_selector_keys_free(ifcopenshell_selector_keys_t* h);

/* ------------------------------------------------------------------ */
/*  util.pset: PSD template lookup                                     */
/* ------------------------------------------------------------------ */

typedef struct ifcopenshell_pset_template_t ifcopenshell_pset_template_t;

/* Set the directory containing Pset_IFC2X3.ifc, Pset_IFC4_ADD2.ifc and
   Pset_IFC4X3.ifc. Must be called once per process before
   ifcopenshell_util_pset_get_template(). The directory string is copied. */
IFCAPI_EXPORT void ifcopenshell_util_pset_set_template_dir(const char* dir);

/* Returns a process-cached, non-owned handle for the templates of the
   given schema_identifier (as in file.schema_identifier; e.g. "IFC2X3",
   "IFC4_ADD2", "IFC4X3_ADD2"). Returns NULL on failure. */
IFCAPI_EXPORT ifcopenshell_pset_template_t* ifcopenshell_util_pset_get_template(
    const char* schema_identifier);

/* Returns the IfcPropertySetTemplate instance with matching Name, or NULL.
   The instance lives in the cached templates file; the caller owns the
   returned handle and must free it with ifcopenshell_ifc_instance_destroy. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_pset_template_get_by_name(
    ifcopenshell_pset_template_t* pqt, const char* name);

/* Returns true if a template with that name exists in the cached file. */
IFCAPI_EXPORT bool ifcopenshell_util_pset_template_is_templated(
    ifcopenshell_pset_template_t* pqt, const char* name);

/* Get applicable property set templates. Returns a malloc'd array of
   instance handles; caller frees with ifcopenshell_free_instance_array.
   pset_only/qto_only mirror Python; ifc_class="" means any class. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_pset_template_get_applicable(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema,
    uint32_t* out_count);

/* Names variant — returns NULL-terminated array of strings; caller frees
   each entry plus the outer array with ifcopenshell_free_string_array. */
IFCAPI_EXPORT char** ifcopenshell_util_pset_template_get_applicable_names(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema,
    uint32_t* out_count);

/* Returns "PSET", "QTO", or NULL (mixed/undetermined). The returned C
   string is statically allocated and must NOT be freed. */
IFCAPI_EXPORT const char* ifcopenshell_util_pset_template_pset_type(
    ifcopenshell_ifc_instance_t* pset_template);

/* ------------------------------------------------------------------ */
/*  api.pset: add_pset / add_qto / edit_pset / edit_qto                */
/* ------------------------------------------------------------------ */

/* Builder for the properties dict consumed by edit_pset / edit_qto.
   The caller constructs the builder, populates entries via the per-entry
   setters below, and frees it with ifcopenshell_pset_props_free.
   All strings and arrays are copied internally — callers retain ownership. */
typedef struct ifcopenshell_pset_props_t ifcopenshell_pset_props_t;

IFCAPI_EXPORT ifcopenshell_pset_props_t* ifcopenshell_pset_props_new(void);
IFCAPI_EXPORT void ifcopenshell_pset_props_free(ifcopenshell_pset_props_t* props);

IFCAPI_EXPORT void ifcopenshell_pset_props_set_null(ifcopenshell_pset_props_t* props, const char* key);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_bool(ifcopenshell_pset_props_t* props, const char* key, bool v);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_int(ifcopenshell_pset_props_t* props, const char* key, int64_t v);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_double(ifcopenshell_pset_props_t* props, const char* key, double v);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_string(ifcopenshell_pset_props_t* props, const char* key, const char* v);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_instance(
    ifcopenshell_pset_props_t* props, const char* key, ifcopenshell_ifc_instance_t* v);

/* The (value, ifc_type) tuple form — value already wrapped to a specific IFC simple type. */
IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_string(
    ifcopenshell_pset_props_t* props, const char* key, const char* v, const char* ifc_type);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_double(
    ifcopenshell_pset_props_t* props, const char* key, double v, const char* ifc_type);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_int(
    ifcopenshell_pset_props_t* props, const char* key, int64_t v, const char* ifc_type);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_typed_bool(
    ifcopenshell_pset_props_t* props, const char* key, bool v, const char* ifc_type);

/* Aggregate values (enum / list properties). */
IFCAPI_EXPORT void ifcopenshell_pset_props_set_string_list(
    ifcopenshell_pset_props_t* props, const char* key, const char** vals, uint32_t count);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_double_list(
    ifcopenshell_pset_props_t* props, const char* key, const double* vals, uint32_t count);
IFCAPI_EXPORT void ifcopenshell_pset_props_set_int_list(
    ifcopenshell_pset_props_t* props, const char* key, const int64_t* vals, uint32_t count);

/* Nested dict — value is itself a properties builder. The outer builder takes
   ownership of the inner; do NOT free the inner separately. */
IFCAPI_EXPORT void ifcopenshell_pset_props_set_dict(
    ifcopenshell_pset_props_t* outer, const char* key, ifcopenshell_pset_props_t* inner);

/* Attach an IfcUnit instance to the most recently appended entry. Mirrors
   upstream ``unpack_unit_value`` for ``{NominalValue, Unit}`` shaped values:
   the unit becomes the resulting IfcPropertySingleValue's ``Unit`` attribute. */
IFCAPI_EXPORT void ifcopenshell_pset_props_set_unit_for_last(
    ifcopenshell_pset_props_t* props, ifcopenshell_ifc_instance_t* unit);

/* Create an empty IfcPropertySet on `product`. owner_history may be NULL.
   `ifc2x3_subclass` may be NULL; when non-NULL it overrides the default
   IfcMaterialProperties / IfcProfileProperties subclass selection in
   IFC2X3 (no effect for >=IFC4). Returns a caller-owned handle (free with
   ifcopenshell_ifc_instance_destroy) that wraps the resulting (or
   pre-existing) IfcPropertySet, or NULL on error. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_pset_add_pset(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* product,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history,
    const char* ifc2x3_subclass);

/* Create an empty IfcElementQuantity on `product`. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_pset_add_qto(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* product,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history);

/* Edit a property set (name + properties). Both `name` and `properties` may
   be NULL. `pset_template` overrides the auto-loaded buildingSMART template
   when non-NULL. Returns true on success. */
IFCAPI_EXPORT bool ifcopenshell_api_pset_edit_pset(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* pset,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    ifcopenshell_ifc_instance_t* pset_template,
    bool should_purge);

IFCAPI_EXPORT bool ifcopenshell_api_pset_edit_qto(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* qto,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    ifcopenshell_ifc_instance_t* qto_template);

/* ------------------------------------------------------------------ */
/*  util.unit                                                          */
/* ------------------------------------------------------------------ */

/* Returns the project unit-scale factor (multiplier from project units to
   SI) for the given unit_type ("LENGTHUNIT", "AREAUNIT", ...). Returns 1.0
   when the project has no UnitsInContext, when the requested unit is not
   present, or on any error. The returned value follows the upstream
   ifcopenshell.util.unit.calculate_unit_scale semantics:

       si_value = project_value * unit_scale
       project_value = si_value / unit_scale

   Pass NULL or empty string for unit_type to default to "LENGTHUNIT". */
IFCAPI_EXPORT double ifcopenshell_util_unit_calculate_unit_scale(
    ifcopenshell_ifc_file_t* file,
    const char* unit_type);

/* Pure-string helpers. The const char* returns reference static storage and
   must NOT be freed by the caller. char* returns are heap-allocated and must
   be released with ifcopenshell_free_string. */
/* Entity-based helpers. Returned ifc_instance handles are caller-owned and
   must be released with ifcopenshell_ifc_instance_destroy. char* returns
   must be released with ifcopenshell_free_string. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_unit_get_unit_assignment(
    ifcopenshell_ifc_file_t* file);
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_unit_get_project_unit(
    ifcopenshell_ifc_file_t* file, const char* unit_type);
IFCAPI_EXPORT char* ifcopenshell_util_unit_get_full_unit_name(
    ifcopenshell_ifc_instance_t* unit);
IFCAPI_EXPORT char* ifcopenshell_util_unit_get_unit_symbol(
    ifcopenshell_ifc_instance_t* unit);
IFCAPI_EXPORT double ifcopenshell_util_unit_convert_unit(
    double value,
    ifcopenshell_ifc_instance_t* from_unit,
    ifcopenshell_ifc_instance_t* to_unit);
/* Inspect a property/quantity. Either *out_unit is populated (a directly
   attached Unit), or *out_measure_class is populated with the IFC measure class
   string (e.g. "IfcLengthMeasure") and the caller resolves it via
   get_project_unit. Both out arguments may be NULL on input if not needed. The
   returned char* must be released with ifcopenshell_free_string. */
IFCAPI_EXPORT void ifcopenshell_util_unit_resolve_property(
    ifcopenshell_ifc_instance_t* prop,
    ifcopenshell_ifc_instance_t** out_unit,
    char** out_measure_class);
IFCAPI_EXPORT void ifcopenshell_util_unit_resolve_property_table(
    ifcopenshell_ifc_instance_t* prop,
    ifcopenshell_ifc_instance_t** out_defining_unit,
    char** out_defining_measure_class,
    ifcopenshell_ifc_instance_t** out_defined_unit,
    char** out_defined_measure_class);

/* ------------------------------------------------------------------ */
/*  api.geometry.edit_object_placement                                 */
/* ------------------------------------------------------------------ */

/* Set the IfcLocalPlacement on `product` from a 4x4 row-major matrix
   (16 doubles). Pass NULL for `matrix` to use the identity. When `is_si`
   is true, the matrix translation column (matrix[3], matrix[7], matrix[11])
   is divided by the project's length-unit scale before being recorded.
   When `should_transform_children` is false, descendants whose placements
   referenced the previous placement have their relative placements rewritten
   so their world transforms are preserved.

   Returns a freshly wrapped handle for the new IfcLocalPlacement (caller
   frees with ifcopenshell_ifc_instance_destroy), or NULL on error / when
   `product` does not carry an ObjectPlacement attribute. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_geometry_edit_object_placement(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* product,
    const double* matrix,
    bool is_si,
    bool should_transform_children);

/* ------------------------------------------------------------------ */
/*  util.schema.reassign_class                                         */
/* ------------------------------------------------------------------ */

/* Mirrors ifcopenshell.util.schema.reassign_class(file, element, new_class).
   Returns the new (reassigned) instance handle (caller-owned, free with
   ifcopenshell_ifc_instance_destroy), or NULL on failure. */
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_schema_reassign_class(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    const char* new_class);

#ifdef __cplusplus
}
#endif

#endif /* IFCAPI_H */
