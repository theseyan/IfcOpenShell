#ifndef IFCOPENSHELL_ZIG_IFCGEOM_C_API_H
#define IFCOPENSHELL_ZIG_IFCGEOM_C_API_H

#include <stddef.h>

#include "ifcparse/c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_ifcgeom_settings ifcopenshell_ifcgeom_settings_t;
typedef struct ifcopenshell_ifcgeom_iterator ifcopenshell_ifcgeom_iterator_t;
typedef struct ifcopenshell_ifcgeom_string_list ifcopenshell_ifcgeom_string_list_t;
typedef struct ifcopenshell_ifcgeom_mesh ifcopenshell_ifcgeom_mesh_t;

typedef enum ifcopenshell_ifcgeom_element_kind {
    IFCOPENSHELL_IFCGEOM_ELEMENT_NONE = 0,
    IFCOPENSHELL_IFCGEOM_ELEMENT_TRIANGULATION = 1,
    IFCOPENSHELL_IFCGEOM_ELEMENT_BREP = 2,
    IFCOPENSHELL_IFCGEOM_ELEMENT_SERIALIZED = 3,
} ifcopenshell_ifcgeom_element_kind_t;

ifcopenshell_ifcgeom_settings_t* ifcopenshell_ifcgeom_settings_create(void);

void ifcopenshell_ifcgeom_settings_destroy(ifcopenshell_ifcgeom_settings_t* settings);

int ifcopenshell_ifcgeom_settings_set_bool(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
);

int ifcopenshell_ifcgeom_settings_set_int(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
);

int ifcopenshell_ifcgeom_settings_set_double(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double value
);

int ifcopenshell_ifcgeom_settings_set_string(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* value
);

int ifcopenshell_ifcgeom_settings_set_int_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const int* values,
    size_t value_count
);

int ifcopenshell_ifcgeom_settings_set_string_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* const* values,
    size_t value_count
);

int ifcopenshell_ifcgeom_settings_set_double_vector(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const double* values,
    size_t value_count
);

int ifcopenshell_ifcgeom_settings_get_bool(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
);

int ifcopenshell_ifcgeom_settings_get_int(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
);

int ifcopenshell_ifcgeom_settings_get_double(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double* out_value
);

const char* ifcopenshell_ifcgeom_settings_get_string(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
);

const char* ifcopenshell_ifcgeom_settings_get_type(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
);

ifcopenshell_ifcgeom_string_list_t* ifcopenshell_ifcgeom_settings_setting_names(
    const ifcopenshell_ifcgeom_settings_t* settings
);

const char* ifcopenshell_ifcgeom_settings_last_error(
    const ifcopenshell_ifcgeom_settings_t* settings
);

void ifcopenshell_ifcgeom_string_list_destroy(ifcopenshell_ifcgeom_string_list_t* list);

size_t ifcopenshell_ifcgeom_string_list_count(const ifcopenshell_ifcgeom_string_list_t* list);

void ifcopenshell_ifcgeom_string_list_reset(ifcopenshell_ifcgeom_string_list_t* list);

const char* ifcopenshell_ifcgeom_string_list_get(
    const ifcopenshell_ifcgeom_string_list_t* list,
    size_t index
);

const char* ifcopenshell_ifcgeom_string_list_next(ifcopenshell_ifcgeom_string_list_t* list);

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads
);

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_type_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
);

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_globalid_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
);

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_id_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const int* values,
    size_t value_count,
    int include,
    int num_threads
);

void ifcopenshell_ifcgeom_iterator_destroy(ifcopenshell_ifcgeom_iterator_t* iterator);

int ifcopenshell_ifcgeom_iterator_initialize(ifcopenshell_ifcgeom_iterator_t* iterator);

int ifcopenshell_ifcgeom_iterator_next(ifcopenshell_ifcgeom_iterator_t* iterator);

int ifcopenshell_ifcgeom_iterator_progress(const ifcopenshell_ifcgeom_iterator_t* iterator);

int ifcopenshell_ifcgeom_iterator_had_error(const ifcopenshell_ifcgeom_iterator_t* iterator);

const char* ifcopenshell_ifcgeom_iterator_unit_name(ifcopenshell_ifcgeom_iterator_t* iterator);

double ifcopenshell_ifcgeom_iterator_unit_magnitude(ifcopenshell_ifcgeom_iterator_t* iterator);

void ifcopenshell_ifcgeom_iterator_compute_bounds(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int with_geometry
);

int ifcopenshell_ifcgeom_iterator_bounds_min(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
);

int ifcopenshell_ifcgeom_iterator_bounds_max(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
);

int ifcopenshell_ifcgeom_iterator_current_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
);

int ifcopenshell_ifcgeom_iterator_current_parent_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
);

const char* ifcopenshell_ifcgeom_iterator_current_type(ifcopenshell_ifcgeom_iterator_t* iterator);

const char* ifcopenshell_ifcgeom_iterator_current_guid(ifcopenshell_ifcgeom_iterator_t* iterator);

int ifcopenshell_ifcgeom_iterator_current_transform(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_matrix_16[16]
);

ifcopenshell_ifcgeom_element_kind_t ifcopenshell_ifcgeom_iterator_current_kind(
    ifcopenshell_ifcgeom_iterator_t* iterator
);

ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_iterator_get_mesh(
    ifcopenshell_ifcgeom_iterator_t* iterator
);

const char* ifcopenshell_ifcgeom_iterator_log(ifcopenshell_ifcgeom_iterator_t* iterator);

const char* ifcopenshell_ifcgeom_iterator_last_error(
    const ifcopenshell_ifcgeom_iterator_t* iterator
);

void ifcopenshell_ifcgeom_mesh_destroy(ifcopenshell_ifcgeom_mesh_t* mesh);

int ifcopenshell_ifcgeom_mesh_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

int ifcopenshell_ifcgeom_mesh_parent_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_mesh_name(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_mesh_type(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_mesh_guid(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_mesh_context(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_mesh_unique_id(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_verts_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const double* ifcopenshell_ifcgeom_mesh_verts_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_faces_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const int* ifcopenshell_ifcgeom_mesh_faces_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_edges_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const int* ifcopenshell_ifcgeom_mesh_edges_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_normals_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const double* ifcopenshell_ifcgeom_mesh_normals_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_uvs_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const double* ifcopenshell_ifcgeom_mesh_uvs_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_material_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const int* ifcopenshell_ifcgeom_mesh_material_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const int* ifcopenshell_ifcgeom_mesh_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_edges_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const int* ifcopenshell_ifcgeom_mesh_edges_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_colors_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const double* ifcopenshell_ifcgeom_mesh_colors_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

size_t ifcopenshell_ifcgeom_mesh_transform_count(const ifcopenshell_ifcgeom_mesh_t* mesh);

const double* ifcopenshell_ifcgeom_mesh_transform_data(const ifcopenshell_ifcgeom_mesh_t* mesh);

const char* ifcopenshell_ifcgeom_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
