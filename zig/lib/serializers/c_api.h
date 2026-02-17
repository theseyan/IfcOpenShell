#ifndef IFCOPENSHELL_ZIG_SERIALIZERS_C_API_H
#define IFCOPENSHELL_ZIG_SERIALIZERS_C_API_H

#include <stddef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#include "ifcgeom/c_api.h"
#include "ifcparse/c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * String ownership contract across IfcOpenShell C APIs:
 * - `const char*` returns are borrowed.
 * - functions returning owned `char*` use `_copy` naming.
 * - free owned strings with `ifcopenshell_string_free` from `ifcparse/c_api.h`.
 */

typedef struct ifcopenshell_ifcserializers_settings ifcopenshell_ifcserializers_settings_t;
typedef struct ifcopenshell_ifcserializers_string_list ifcopenshell_ifcserializers_string_list_t;
typedef struct ifcopenshell_ifcserializers_serializer ifcopenshell_ifcserializers_serializer_t;

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_settings_t* ifcopenshell_ifcserializers_settings_create(void);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcserializers_settings_destroy(ifcopenshell_ifcserializers_settings_t* settings);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_set_bool(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_set_int(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_set_double(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    double value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_set_string(
    ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    const char* value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_get_bool(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_get_int(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_settings_get_double(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name,
    double* out_value
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_settings_get_string(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_settings_get_type(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_string_list_t* ifcopenshell_ifcserializers_settings_setting_names(
    const ifcopenshell_ifcserializers_settings_t* settings
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_settings_last_error(
    const ifcopenshell_ifcserializers_settings_t* settings
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcserializers_string_list_destroy(ifcopenshell_ifcserializers_string_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcserializers_string_list_count(const ifcopenshell_ifcserializers_string_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcserializers_string_list_reset(ifcopenshell_ifcserializers_string_list_t* list);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_string_list_get(
    const ifcopenshell_ifcserializers_string_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_string_list_next(ifcopenshell_ifcserializers_string_list_t* list);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_obj_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_obj_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_svg_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_svg_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_ttl_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_ttl_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcserializers_serializer_destroy(ifcopenshell_ifcserializers_serializer_t* serializer);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_serializer_ready(const ifcopenshell_ifcserializers_serializer_t* serializer);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_serializer_run(
    ifcopenshell_ifcserializers_serializer_t* serializer,
    const ifcopenshell_ifcparse_file_t* file,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_serializer_buffer_primary(
    ifcopenshell_ifcserializers_serializer_t* serializer
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_serializer_buffer_secondary(
    ifcopenshell_ifcserializers_serializer_t* serializer
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_serializer_last_error(
    const ifcopenshell_ifcserializers_serializer_t* serializer
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_obj(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_svg(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_step(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* step_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_iges(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* iges_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_ttl(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_gltf(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* gltf_filename,
    const char* geometry_library,
    int num_threads
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_xml(
    const ifcopenshell_ifcparse_file_t* file,
    const char* xml_filename
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_export_json(
    const ifcopenshell_ifcparse_file_t* file,
    const char* json_filename
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_gltf(void);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_json(void);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_svg(void);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_ttl(void);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_step(void);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcserializers_has_iges(void);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcserializers_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
