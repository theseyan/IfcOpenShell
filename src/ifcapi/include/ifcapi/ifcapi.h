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

/* ------------------------------------------------------------------ */
/*  GUID utilities                                                     */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT char* ifcopenshell_guid_new(void);
IFCAPI_EXPORT char* ifcopenshell_guid_compress(const char* uuid_hex);
IFCAPI_EXPORT char* ifcopenshell_guid_expand(const char* guid);
IFCAPI_EXPORT void ifcopenshell_free_string(char* str);

/* ------------------------------------------------------------------ */
/*  File operations                                                    */
/* ------------------------------------------------------------------ */

/// Creates a new empty IFC file for the given schema (e.g. "IFC4", "IFC2X3").
/// Opens an IFC file from disk.  Returns an opaque IfcFile pointer (free with
/// ifcopenshell_file_free), or NULL on failure (use ifcopenshell_last_error_message() for details).
IFCAPI_EXPORT ifcopenshell_ifc_file_t* ifcopenshell_file_open(const char* path);

/// Returns an opaque IfcFile pointer. Free with ifcopenshell_file_free().
IFCAPI_EXPORT ifcopenshell_ifc_file_t* ifcopenshell_file_create(const char* schema_name);

/// Frees a file created by ifcopenshell_file_create().
IFCAPI_EXPORT void ifcopenshell_file_free(ifcopenshell_ifc_file_t* file);

/// Returns the schema identifier string (e.g. "IFC4"). Do NOT free.
IFCAPI_EXPORT const char* ifcopenshell_file_schema(const ifcopenshell_ifc_file_t* file);

/// Creates a blank entity of the given type. Returns its STEP ID, or 0 on error.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_file_create_entity(ifcopenshell_ifc_file_t* file, const char* type_name);

/// Returns the number of entities of the given type.
IFCAPI_EXPORT int32_t ifcopenshell_file_by_type_count(const ifcopenshell_ifc_file_t* file, const char* type_name);

/// Fills ids_out with STEP IDs of entities of the given type.
/// ids_out must have space for at least ifcopenshell_file_by_type_count() elements.
/// Returns the number of IDs written.
IFCAPI_EXPORT int32_t ifcopenshell_file_by_type(const ifcopenshell_ifc_file_t* file, const char* type_name, ifcopenshell_ifc_instance_t** ids_out);

/// Frees an array of instance handles returned by ifcapi functions.
/// Each handle wrapper is destroyed (the underlying entities remain owned by the file).
IFCAPI_EXPORT void ifcopenshell_free_instance_array(ifcopenshell_ifc_instance_t** arr, uint32_t count);

/// Frees only the array buffer returned by APIs returning ifcopenshell_ifc_instance_t**,
/// without destroying the contained handles. Use this when ownership of the
/// individual handles is transferred to caller-side wrappers.
IFCAPI_EXPORT void ifcopenshell_free_instance_array_only(ifcopenshell_ifc_instance_t** arr);

/// Removes an entity from the file and nullifies all references to it.
IFCAPI_EXPORT void ifcopenshell_file_remove(ifcopenshell_ifc_instance_t* instance);

/// Returns the STEP ID of the entity with the given GlobalId, or 0 if not found.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_file_by_guid(const ifcopenshell_ifc_file_t* file, const char* guid);

/// Returns the total number of entities in the file.
IFCAPI_EXPORT uint32_t ifcopenshell_file_entity_count(const ifcopenshell_ifc_file_t* file);

/// Returns all STEP IDs in the file. Caller must free with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_file_entity_ids(const ifcopenshell_ifc_file_t* file, uint32_t* count);

/// Returns STEP IDs of entities that reference the given entity.
/// Caller must free the returned array with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_file_get_inverse(ifcopenshell_ifc_instance_t* instance, uint32_t* count);

/// Recursively traverses all entities referenced by the given entity.
/// max_levels=0 means unlimited. Caller must free with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_file_traverse(ifcopenshell_ifc_instance_t* instance, int max_levels, uint32_t* count);

/// Writes the file to disk in IFC-SPF format.
IFCAPI_EXPORT bool ifcopenshell_file_write(ifcopenshell_ifc_file_t* file, const char* path);

/// Serializes the file to an IFC-SPF string. Caller must free with ifcopenshell_free_string().
IFCAPI_EXPORT char* ifcopenshell_file_to_string(ifcopenshell_ifc_file_t* file);

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

/// Returns the IFC type name of the entity (e.g. "IfcWall"). Do NOT free.
IFCAPI_EXPORT const char* ifcopenshell_entity_type(const ifcopenshell_ifc_instance_t* instance);

/// Returns true if the entity is-a (subtype of) the given type name.
/// If type_name is NULL, returns false.
IFCAPI_EXPORT bool ifcopenshell_entity_is_a(const ifcopenshell_ifc_instance_t* instance, const char* type_name);

/// Returns true if the entity has an attribute with the given name.
IFCAPI_EXPORT bool ifcopenshell_entity_has_attr(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Returns the type tag of the attribute's current value.
IFCAPI_EXPORT ifcopenshell_attr_type ifcopenshell_entity_attr_type(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Returns true if the attribute is null/unset.
IFCAPI_EXPORT bool ifcopenshell_entity_attr_is_null(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Returns the number of attributes on the entity.
IFCAPI_EXPORT uint32_t ifcopenshell_entity_attr_count(const ifcopenshell_ifc_instance_t* instance);

/// Returns the name of the attribute at the given index. Caller must free with ifcopenshell_free_string().
IFCAPI_EXPORT char* ifcopenshell_entity_attr_name(const ifcopenshell_ifc_instance_t* instance, uint32_t index);

/// Returns the IFC-SPF string representation of the entity. Caller must free with ifcopenshell_free_string().
IFCAPI_EXPORT char* ifcopenshell_entity_to_string(const ifcopenshell_ifc_instance_t* instance);

/* ------------------------------------------------------------------ */
/*  Entity attribute getters (caller frees char* with free_string)     */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT char* ifcopenshell_entity_get_string(const ifcopenshell_ifc_instance_t* instance, const char* attr);
IFCAPI_EXPORT int64_t ifcopenshell_entity_get_int(const ifcopenshell_ifc_instance_t* instance, const char* attr);
IFCAPI_EXPORT double ifcopenshell_entity_get_double(const ifcopenshell_ifc_instance_t* instance, const char* attr);
IFCAPI_EXPORT int ifcopenshell_entity_get_bool(const ifcopenshell_ifc_instance_t* instance, const char* attr);
IFCAPI_EXPORT char* ifcopenshell_entity_get_enum(const ifcopenshell_ifc_instance_t* instance, const char* attr);
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_entity_get_reference(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/* ------------------------------------------------------------------ */
/*  Entity attribute setters                                           */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT void ifcopenshell_entity_set_string(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* val);
IFCAPI_EXPORT void ifcopenshell_entity_set_int(ifcopenshell_ifc_instance_t* instance, const char* attr, int64_t val);
IFCAPI_EXPORT void ifcopenshell_entity_set_double(ifcopenshell_ifc_instance_t* instance, const char* attr, double val);
IFCAPI_EXPORT void ifcopenshell_entity_set_bool(ifcopenshell_ifc_instance_t* instance, const char* attr, int val);
/// Returns true if the enum value was valid and set, false otherwise.
IFCAPI_EXPORT bool ifcopenshell_entity_set_enum(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* val);
IFCAPI_EXPORT void ifcopenshell_entity_set_reference(ifcopenshell_ifc_instance_t* instance, const char* attr, ifcopenshell_ifc_instance_t* ref);
IFCAPI_EXPORT void ifcopenshell_entity_set_null(ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Create a simple type instance (e.g. IfcLabel, IfcReal) and return its STEP ID.
/// The value is parsed from the string representation based on the type.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_file_create_type_instance(ifcopenshell_ifc_file_t* file, const char* type_name, const char* str_value);

/// Set an attribute to reference a simple type instance (like NominalValue = IfcLabel("test")).
/// type_name: the IFC type name (e.g. "IfcLabel"). str_value: string representation of the value.
IFCAPI_EXPORT bool ifcopenshell_entity_set_typed_value(ifcopenshell_ifc_instance_t* instance, const char* attr, const char* type_name, const char* str_value);

/// Read a typed value from an attribute. Returns the IFC type name (e.g. "IfcLabel").
/// If value_out is non-NULL, writes the string representation of the wrapped value (caller frees).
/// Returns NULL if the attribute is not a typed value.
IFCAPI_EXPORT const char* ifcopenshell_entity_get_typed_value(const ifcopenshell_ifc_instance_t* instance, const char* attr, char** value_out);

/* ------------------------------------------------------------------ */
/*  Entity aggregate (list) attribute getters                          */
/* ------------------------------------------------------------------ */

/// Returns the number of elements in an aggregate attribute, or -1 on error.
IFCAPI_EXPORT int32_t ifcopenshell_entity_get_aggregate_size(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Returns an array of int values from an aggregate attribute. Caller frees with ifcopenshell_free_int_array().
IFCAPI_EXPORT int64_t* ifcopenshell_entity_get_aggregate_int(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count);

/// Returns an array of double values. Caller frees with ifcopenshell_free_double_array().
IFCAPI_EXPORT double* ifcopenshell_entity_get_aggregate_double(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count);

/// Returns an array of string values. Caller frees each string with ifcopenshell_free_string(), then the array with ifcopenshell_free_string_array().
IFCAPI_EXPORT char** ifcopenshell_entity_get_aggregate_string(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count);

/// Returns an array of STEP IDs (entity references). Caller frees with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_entity_get_aggregate_ref(const ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count);

/// Reads an aggregate of inline typed values (e.g. SET OF IfcValue).
/// Fills type_names_out and values_out arrays (caller frees each string, then arrays).
/// Returns number of elements, or -1 on error.
IFCAPI_EXPORT int32_t ifcopenshell_entity_get_aggregate_typed_value(const ifcopenshell_ifc_instance_t* instance, const char* attr,
    char*** type_names_out, char*** values_out);

/* ------------------------------------------------------------------ */
/*  Entity aggregate (list) attribute setters                          */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT bool ifcopenshell_entity_set_aggregate_int(ifcopenshell_ifc_instance_t* instance, const char* attr, const int64_t* vals, uint32_t count);
IFCAPI_EXPORT bool ifcopenshell_entity_set_aggregate_double(ifcopenshell_ifc_instance_t* instance, const char* attr, const double* vals, uint32_t count);
IFCAPI_EXPORT bool ifcopenshell_entity_set_aggregate_string(ifcopenshell_ifc_instance_t* instance, const char* attr, const char** vals, uint32_t count);
IFCAPI_EXPORT bool ifcopenshell_entity_set_aggregate_ref(ifcopenshell_ifc_instance_t* instance, const char* attr, ifcopenshell_ifc_instance_t** refs, uint32_t count);

/// Sets an aggregate attribute to a list of typed values.
/// type_names and str_values must have `count` elements each.
/// Each pair (type_names[i], str_values[i]) creates an inline typed value like IfcLabel('foo').
IFCAPI_EXPORT bool ifcopenshell_entity_set_aggregate_typed_value(ifcopenshell_ifc_instance_t* instance, const char* attr,
    const char** type_names, const char** str_values, uint32_t count);

/* ------------------------------------------------------------------ */
/*  Aggregate memory management                                        */
/* ------------------------------------------------------------------ */

IFCAPI_EXPORT void ifcopenshell_free_int_array(int64_t* arr);
IFCAPI_EXPORT void ifcopenshell_free_double_array(double* arr);
IFCAPI_EXPORT void ifcopenshell_free_string_array(char** arr, uint32_t count);

/* ------------------------------------------------------------------ */
/*  Inverse attribute access                                           */
/* ------------------------------------------------------------------ */

/// Returns true if the given name is an inverse attribute on the entity.
IFCAPI_EXPORT bool ifcopenshell_entity_is_inverse_attr(const ifcopenshell_ifc_instance_t* instance, const char* attr);

/// Returns entity IDs for a named inverse attribute (e.g. "IsDecomposedBy").
/// Caller must free with ifcopenshell_free_instance_array().
IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_entity_get_inverse_attr(ifcopenshell_ifc_instance_t* instance, const char* attr, uint32_t* count);

/* ------------------------------------------------------------------ */
/*  Utility: deep removal                                              */
/* ------------------------------------------------------------------ */

/// Recursively remove an entity and any entities it references that would
/// become orphaned (total_inverses == 0 after removal). Equivalent to
/// ifcopenshell.util.element.remove_deep2().
IFCAPI_EXPORT void ifcopenshell_util_remove_deep2(ifcopenshell_ifc_instance_t* instance);

/* ------------------------------------------------------------------ */
/*  High-level: root module                                            */
/* ------------------------------------------------------------------ */

/// Creates a new rooted IFC entity (equivalent to ifcopenshell.api.root.create_entity).
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
/// flag matching the upstream Python API. When true (the default), this also
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

/// Get the construction type of an element (resolves IsTypedBy / IfcRelDefinesByType).
/// Returns the type STEP ID, or 0 if none.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_type(const ifcopenshell_ifc_instance_t* instance);

/// Get the aggregate parent via IfcRelAggregates. Returns 0 if none.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_aggregate(const ifcopenshell_ifc_instance_t* instance);

/// Get the nest parent via IfcRelNests (IFC4+) or Decomposes (IFC2X3). Returns 0 if none.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_nest(const ifcopenshell_ifc_instance_t* instance);

/// Get the spatial container of an element.
/// If direct_only is true, only returns the direct IfcRelContainedInSpatialStructure link.
/// Otherwise walks aggregation chain. Optional ifc_class filter (NULL or empty = any).
/// Returns 0 if no container matches.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_container(const ifcopenshell_ifc_instance_t* instance, bool direct_only, const char* ifc_class);

/// Get the spatial parent of an element. Tries direct container, then aggregate,
/// then nest, then filled void, then voided element. Returns 0 if none.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_parent(const ifcopenshell_ifc_instance_t* instance);

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

/// Returns the STEP ID of the material associated with *element*, or 0.
/// If should_skip_usage is true, layer/profile set usages are unwrapped to
/// their underlying set. If should_inherit is true, materials inherited from
/// the element's type are considered.
IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_element_get_material(const ifcopenshell_ifc_instance_t* instance, bool should_skip_usage, bool should_inherit);

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

/* ------------------------------------------------------------------ */
/*  Schema introspection                                               */
/* ------------------------------------------------------------------ */
/*
 * These functions expose IfcParse's static schema metadata (declarations,
 * attributes, types, enumerations, selects, inverse attributes). All
 * returned handles are non-owning pointers into program-lifetime objects
 * (schema singletons) and must NOT be freed. Returned C strings likewise
 * point into schema storage and remain valid for the program's lifetime.
 *
 * Array-returning functions (enumeration_items, select_list,
 * all_attributes, all_inverse_attributes) allocate a heap array of
 * pointers; caller must free the outer array with ifcopenshell_free_ptr_array.
 * The pointed-to strings/handles stay owned by the schema.
 */

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

/* Free a heap-allocated array of pointers returned by this API. The
   pointees themselves are NOT freed (they are owned by the schema). */
IFCAPI_EXPORT void ifcopenshell_free_ptr_array(void* arr);

/* --- schema_definition --- */
IFCAPI_EXPORT const void* ifcopenshell_schema_by_name(const char* name);
IFCAPI_EXPORT const void* ifcopenshell_file_schema_def(const ifcopenshell_ifc_file_t* file);
IFCAPI_EXPORT const char* ifcopenshell_schema_name(const void* schema);
IFCAPI_EXPORT const void* ifcopenshell_schema_declaration_by_name(const void* schema, const char* name);
IFCAPI_EXPORT const void** ifcopenshell_schema_declarations(const void* schema, uint32_t* out_count);

/* --- declaration (polymorphic) --- */
IFCAPI_EXPORT const char* ifcopenshell_declaration_name(const void* decl);
IFCAPI_EXPORT bool        ifcopenshell_declaration_is(const void* decl, const char* name);
IFCAPI_EXPORT const void* ifcopenshell_declaration_as_entity(const void* decl);
IFCAPI_EXPORT const void* ifcopenshell_declaration_as_type(const void* decl);
IFCAPI_EXPORT const void* ifcopenshell_declaration_as_select(const void* decl);
IFCAPI_EXPORT const void* ifcopenshell_declaration_as_enumeration(const void* decl);
IFCAPI_EXPORT const void* ifcopenshell_declaration_schema(const void* decl);

/* --- entity --- */
IFCAPI_EXPORT bool        ifcopenshell_entity_is_abstract(const void* entity);
IFCAPI_EXPORT const void* ifcopenshell_entity_supertype(const void* entity);
IFCAPI_EXPORT uint32_t    ifcopenshell_entity_attribute_count(const void* entity);
IFCAPI_EXPORT const void* ifcopenshell_entity_attribute_by_index(const void* entity, uint32_t index);
IFCAPI_EXPORT int32_t     ifcopenshell_entity_attribute_index_by_name(const void* entity, const char* name);
IFCAPI_EXPORT const void** ifcopenshell_entity_all_attributes(const void* entity, uint32_t* out_count);
IFCAPI_EXPORT const void** ifcopenshell_entity_all_inverse_attributes(const void* entity, uint32_t* out_count);
IFCAPI_EXPORT bool        ifcopenshell_entity_attribute_derived(const void* entity, uint32_t index);

/* --- type_declaration --- */
IFCAPI_EXPORT const void* ifcopenshell_type_declaration_declared_type(const void* type_decl);

/* --- enumeration_type --- */
IFCAPI_EXPORT const char** ifcopenshell_enumeration_items(const void* enum_decl, uint32_t* out_count);

/* --- select_type --- */
IFCAPI_EXPORT const void** ifcopenshell_select_list(const void* select_decl, uint32_t* out_count);

/* --- attribute --- */
IFCAPI_EXPORT const char* ifcopenshell_attribute_name(const void* attr);
IFCAPI_EXPORT bool        ifcopenshell_attribute_optional(const void* attr);
IFCAPI_EXPORT const void* ifcopenshell_attribute_type_of_attribute(const void* attr);

/* --- inverse_attribute --- */
IFCAPI_EXPORT const char* ifcopenshell_inverse_attribute_name(const void* iattr);
IFCAPI_EXPORT int         ifcopenshell_inverse_attribute_type_of_aggregation(const void* iattr);
IFCAPI_EXPORT int32_t     ifcopenshell_inverse_attribute_bound1(const void* iattr);
IFCAPI_EXPORT int32_t     ifcopenshell_inverse_attribute_bound2(const void* iattr);
IFCAPI_EXPORT const void* ifcopenshell_inverse_attribute_entity_reference(const void* iattr);
IFCAPI_EXPORT const void* ifcopenshell_inverse_attribute_attribute_reference(const void* iattr);

/* --- parameter_type (polymorphic) --- */
IFCAPI_EXPORT int         ifcopenshell_parameter_type_kind(const void* pt);
IFCAPI_EXPORT const void* ifcopenshell_named_type_declared_type(const void* pt);
IFCAPI_EXPORT int         ifcopenshell_simple_type_data_type(const void* pt);
IFCAPI_EXPORT int         ifcopenshell_aggregation_type_kind(const void* pt);
IFCAPI_EXPORT int32_t     ifcopenshell_aggregation_type_bound1(const void* pt);
IFCAPI_EXPORT int32_t     ifcopenshell_aggregation_type_bound2(const void* pt);
IFCAPI_EXPORT const void* ifcopenshell_aggregation_type_element(const void* pt);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ */
/*  Selector parser AST                                                */
/* ------------------------------------------------------------------ */
#include "ifcapi/selector_ast.h"

/* ------------------------------------------------------------------ */
/*  Selector value type + get_element_value                           */
/* ------------------------------------------------------------------ */
#include "ifcapi/value.h"

#ifdef __cplusplus
extern "C" {
#endif

IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_selector_get_element_value(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    const char* query);

#ifdef __cplusplus
}
#endif

#endif /* IFCAPI_H */
