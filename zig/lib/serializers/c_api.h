#ifndef IFCOPENSHELL_ZIG_SERIALIZERS_C_API_H
#define IFCOPENSHELL_ZIG_SERIALIZERS_C_API_H

#include <stddef.h>

#include "ifcgeom/c_api.h"
#include "ifcparse/c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_ifcserializers_settings ifcopenshell_ifcserializers_settings_t;
typedef struct ifcopenshell_ifcserializers_string_list ifcopenshell_ifcserializers_string_list_t;

ifcopenshell_ifcserializers_settings_t* ifcopenshell_ifcserializers_settings_create(void);

void ifcopenshell_ifcserializers_settings_destroy(ifcopenshell_ifcserializers_settings_t* settings);

int ifcopenshell_ifcserializers_settings_set_bool(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int value
);

int ifcopenshell_ifcserializers_settings_set_int(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int value
);

int ifcopenshell_ifcserializers_settings_set_double(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    double value
);

int ifcopenshell_ifcserializers_settings_set_string(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    const char* value
);

int ifcopenshell_ifcserializers_settings_get_bool(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int* out_value
);

int ifcopenshell_ifcserializers_settings_get_int(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int* out_value
);

int ifcopenshell_ifcserializers_settings_get_double(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    double* out_value
);

const char* ifcopenshell_ifcserializers_settings_get_string(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name
);

const char* ifcopenshell_ifcserializers_settings_get_type(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name
);

ifcopenshell_ifcserializers_string_list_t* ifcopenshell_ifcserializers_settings_setting_names(
    const ifcopenshell_ifcserializers_settings_t* settings
);

const char* ifcopenshell_ifcserializers_settings_last_error(
    const ifcopenshell_ifcserializers_settings_t* settings
);

void ifcopenshell_ifcserializers_string_list_destroy(ifcopenshell_ifcserializers_string_list_t* list);

size_t ifcopenshell_ifcserializers_string_list_count(const ifcopenshell_ifcserializers_string_list_t* list);

void ifcopenshell_ifcserializers_string_list_reset(ifcopenshell_ifcserializers_string_list_t* list);

const char* ifcopenshell_ifcserializers_string_list_get(
    const ifcopenshell_ifcserializers_string_list_t* list,
    size_t index
);

const char* ifcopenshell_ifcserializers_string_list_next(ifcopenshell_ifcserializers_string_list_t* list);

int ifcopenshell_ifcserializers_export_obj(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_svg(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_step(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* step_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_iges(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* iges_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_ttl(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_gltf(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* gltf_filename,
    const char* geometry_library,
    int num_threads
);

int ifcopenshell_ifcserializers_export_xml(
    const ifcopenshell_ifcparse_file_t* file,
    const char* xml_filename
);

int ifcopenshell_ifcserializers_export_json(
    const ifcopenshell_ifcparse_file_t* file,
    const char* json_filename
);

int ifcopenshell_ifcserializers_has_gltf(void);

int ifcopenshell_ifcserializers_has_json(void);

const char* ifcopenshell_ifcserializers_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
