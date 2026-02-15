#ifndef IFCOPENSHELL_ZIG_IFCGEOM_C_API_H
#define IFCOPENSHELL_ZIG_IFCGEOM_C_API_H

#include <stddef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#include "ifcparse/c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_ifcgeom_settings ifcopenshell_ifcgeom_settings_t;
typedef struct ifcopenshell_ifcgeom_iterator ifcopenshell_ifcgeom_iterator_t;
typedef struct ifcopenshell_ifcgeom_string_list ifcopenshell_ifcgeom_string_list_t;
typedef struct ifcopenshell_ifcgeom_mesh ifcopenshell_ifcgeom_mesh_t;
typedef struct ifcopenshell_ifcgeom_serialized ifcopenshell_ifcgeom_serialized_t;
typedef struct ifcopenshell_ifcgeom_created_shape ifcopenshell_ifcgeom_created_shape_t;
typedef struct ifcopenshell_ifcgeom_tree ifcopenshell_ifcgeom_tree_t;
typedef struct ifcopenshell_ifcgeom_id_list ifcopenshell_ifcgeom_id_list_t;
typedef struct ifcopenshell_ifcgeom_clash_list ifcopenshell_ifcgeom_clash_list_t;

typedef struct ifcopenshell_ifcgeom_clash {
    int clash_type;
    int a_id;
    int b_id;
    double distance;
    double p1[3];
    double p2[3];
} ifcopenshell_ifcgeom_clash_t;

typedef enum ifcopenshell_ifcgeom_element_kind {
    IFCOPENSHELL_IFCGEOM_ELEMENT_NONE = 0,
    IFCOPENSHELL_IFCGEOM_ELEMENT_TRIANGULATION = 1,
    IFCOPENSHELL_IFCGEOM_ELEMENT_BREP = 2,
    IFCOPENSHELL_IFCGEOM_ELEMENT_SERIALIZED = 3,
} ifcopenshell_ifcgeom_element_kind_t;

typedef enum ifcopenshell_ifcgeom_created_shape_kind {
    IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_NONE = 0,
    IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRIANGULATION = 1,
    IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_BREP = 2,
    IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_SERIALIZED = 3,
    IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRANSFORM = 4,
} ifcopenshell_ifcgeom_created_shape_kind_t;

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_settings_t* ifcopenshell_ifcgeom_settings_create(void);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_settings_destroy(ifcopenshell_ifcgeom_settings_t* settings);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_bool(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_int(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_double(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_string(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_int_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const int* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_string_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* const* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_set_double_vector(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const double* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_get_bool(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_get_int(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_settings_get_double(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double* out_value
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_settings_get_string(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_settings_get_type(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_string_list_t* ifcopenshell_ifcgeom_settings_setting_names(
    const ifcopenshell_ifcgeom_settings_t* settings
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_settings_last_error(
    const ifcopenshell_ifcgeom_settings_t* settings
);

EMSCRIPTEN_KEEPALIVE void* ifcopenshell_ifcgeom_settings_native(ifcopenshell_ifcgeom_settings_t* settings);

EMSCRIPTEN_KEEPALIVE const void* ifcopenshell_ifcgeom_settings_native_const(const ifcopenshell_ifcgeom_settings_t* settings);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_string_list_destroy(ifcopenshell_ifcgeom_string_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_string_list_count(const ifcopenshell_ifcgeom_string_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_string_list_reset(ifcopenshell_ifcgeom_string_list_t* list);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_string_list_get(
    const ifcopenshell_ifcgeom_string_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_string_list_next(ifcopenshell_ifcgeom_string_list_t* list);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_tree_t* ifcopenshell_ifcgeom_tree_create(void);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_tree_destroy(ifcopenshell_ifcgeom_tree_t* tree);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_tree_add_file(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_by_id(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    int id,
    int completely_within,
    double extend
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_box(
    ifcopenshell_ifcgeom_tree_t* tree,
    const double min_xyz[3],
    const double max_xyz[3],
    int completely_within
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_point(
    ifcopenshell_ifcgeom_tree_t* tree,
    const double xyz[3],
    double extend
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_intersection_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    double tolerance,
    int check_all
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_collision_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    int allow_touching
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_clearance_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    double clearance,
    int check_all
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_tree_last_error(const ifcopenshell_ifcgeom_tree_t* tree);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_id_list_destroy(ifcopenshell_ifcgeom_id_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_id_list_count(const ifcopenshell_ifcgeom_id_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_id_list_reset(ifcopenshell_ifcgeom_id_list_t* list);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_id_list_get(
    const ifcopenshell_ifcgeom_id_list_t* list,
    size_t index,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_id_list_next(
    ifcopenshell_ifcgeom_id_list_t* list,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_clash_list_destroy(ifcopenshell_ifcgeom_clash_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_clash_list_count(const ifcopenshell_ifcgeom_clash_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_clash_list_reset(ifcopenshell_ifcgeom_clash_list_t* list);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcgeom_clash_t* ifcopenshell_ifcgeom_clash_list_get(
    const ifcopenshell_ifcgeom_clash_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcgeom_clash_t* ifcopenshell_ifcgeom_clash_list_next(
    ifcopenshell_ifcgeom_clash_list_t* list
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_type_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_globalid_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_id_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const int* values,
    size_t value_count,
    int include,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_iterator_destroy(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_initialize(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_next(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_progress(const ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_had_error(const ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_iterator_unit_name(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE double ifcopenshell_ifcgeom_iterator_unit_magnitude(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_iterator_compute_bounds(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int with_geometry
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_bounds_min(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_bounds_max(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_current_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_current_parent_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_iterator_current_type(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_iterator_current_guid(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_iterator_current_transform(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_matrix_16[16]
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_element_kind_t ifcopenshell_ifcgeom_iterator_current_kind(
    ifcopenshell_ifcgeom_iterator_t* iterator
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_iterator_get_mesh(
    ifcopenshell_ifcgeom_iterator_t* iterator
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_serialized_t* ifcopenshell_ifcgeom_iterator_get_serialized(
    ifcopenshell_ifcgeom_iterator_t* iterator
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_create_mesh_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int id
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_created_shape_t* ifcopenshell_ifcgeom_create_shape_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int id
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_iterator_log(ifcopenshell_ifcgeom_iterator_t* iterator);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_iterator_last_error(
    const ifcopenshell_ifcgeom_iterator_t* iterator
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_mesh_destroy(ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_mesh_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_mesh_parent_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_mesh_name(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_mesh_type(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_mesh_guid(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_mesh_context(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_mesh_unique_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_verts_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_mesh_verts_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_faces_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_mesh_faces_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_edges_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_mesh_edges_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_normals_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_mesh_normals_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_uvs_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_mesh_uvs_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_material_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_mesh_material_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_mesh_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_edges_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_mesh_edges_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_colors_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_mesh_colors_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_mesh_transform_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_mesh_transform_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_serialized_destroy(ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_serialized_id(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_serialized_parent_id(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_name(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_type(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_guid(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_context(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_unique_id(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_serialized_brep_data(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_serialized_surface_styles_count(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_serialized_surface_styles_data(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_serialized_surface_style_ids_count(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const int* ifcopenshell_ifcgeom_serialized_surface_style_ids_data(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcgeom_serialized_transform_count(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE const double* ifcopenshell_ifcgeom_serialized_transform_data(const ifcopenshell_ifcgeom_serialized_t* serialized);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcgeom_created_shape_destroy(ifcopenshell_ifcgeom_created_shape_t* shape);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcgeom_created_shape_kind_t ifcopenshell_ifcgeom_created_shape_kind(
    const ifcopenshell_ifcgeom_created_shape_t* shape
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_created_shape_mesh(
    const ifcopenshell_ifcgeom_created_shape_t* shape
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcgeom_serialized_t* ifcopenshell_ifcgeom_created_shape_serialized(
    const ifcopenshell_ifcgeom_created_shape_t* shape
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcgeom_created_shape_transform(
    const ifcopenshell_ifcgeom_created_shape_t* shape,
    double out_matrix_16[16]
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_map_shape_repr_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    int id
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcgeom_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
