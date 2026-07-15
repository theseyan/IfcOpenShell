// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <stdbool.h>
#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg)                                                   \
    do {                                                                    \
        tests_run++;                                                        \
        if (!(cond)) {                                                      \
            fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); \
            tests_failed++;                                                 \
        }                                                                   \
    } while (0)

static size_t type_count(ifcopenshell_file_t *file, const char *type) {
    ifcopenshell_parse_instance_list_t *values = NULL;
    size_t size = 0;
    ASSERT(ifcopenshell_file_by_type(file, type, &values), "by_type succeeds");
    ASSERT(values != NULL, "by_type returns a list");
    if (values) {
        ASSERT(ifcopenshell_parse_instance_list_size(values, &size), "list size succeeds");
        ifcopenshell_parse_instance_list_destroy(values);
    }
    return size;
}

static void test_surface_components_and_texture_records(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *parent = NULL;
    ifcopenshell_instance_t *rendering = NULL;
    ifcopenshell_instance_t *shading = NULL;
    void *attributes = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcSurfaceStyle", &parent), "create surface style succeeds");
    ASSERT(ifcopenshell_pset_props_new(&attributes), "create empty attribute bag succeeds");
    ASSERT(ifcopenshell_style_add_surface_style(
               file, parent, "IfcSurfaceStyleRendering", attributes, &rendering),
           "add rendering succeeds");
    ASSERT(ifcopenshell_style_add_surface_style(
               file, parent, "IfcSurfaceStyleShading", attributes, &shading),
           "replace with shading succeeds");
    ASSERT(type_count(file, "IfcSurfaceStyleRendering") == 0, "rendering was removed");
    ASSERT(type_count(file, "IfcSurfaceStyleShading") == 1, "one shading remains");

    ifcopenshell_style_surface_texture_options_t records[2] = {0};
    records[0].repeat_s = true;
    records[0].repeat_t = false;
    records[0].url_reference = "generated.png";
    records[0].uv_mode = "Generated";
    records[0].has_uv_mode = true;
    records[1].repeat_s = false;
    records[1].repeat_t = true;
    records[1].url_reference = "camera.png";
    records[1].uv_mode = "Camera";
    records[1].has_uv_mode = true;
    ifcopenshell_style_surface_texture_options_list_t descriptors = {records, 2};
    ifcopenshell_instance_list_t uv_maps = {NULL, 0};
    ifcopenshell_parse_instance_list_t *textures = NULL;
    size_t texture_count = 0;
    ASSERT(ifcopenshell_style_add_surface_textures(file, &descriptors, &uv_maps, &textures), "add textures succeeds");
    ASSERT(textures != NULL, "returned texture list is owned");
    ASSERT(ifcopenshell_parse_instance_list_size(textures, &texture_count), "texture result size succeeds");
    ASSERT(texture_count == 2, "two texture records return two entities");
    ASSERT(type_count(file, "IfcTextureCoordinateGenerator") == 2, "generated and camera mappings exist");
    ifcopenshell_parse_instance_list_destroy(textures);
    ifcopenshell_pset_props_free(attributes);

    ifcopenshell_instance_destroy(shading);
    ifcopenshell_instance_destroy(rendering);
    ifcopenshell_instance_destroy(parent);
    ifcopenshell_file_destroy(file);
}

static void test_ifc2x3_texture_noop(void) {
    ifcopenshell_file_t *file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC2X3", 0, "", &file), "new IFC2X3 file succeeds");
    ifcopenshell_style_surface_texture_options_t record = {0};
    record.repeat_s = true;
    record.repeat_t = true;
    record.url_reference = "ignored.png";
    ifcopenshell_style_surface_texture_options_list_t descriptors = {&record, 1};
    ifcopenshell_instance_list_t uv_maps = {NULL, 0};
    ifcopenshell_parse_instance_list_t *textures = NULL;
    size_t size = 1;
    ASSERT(ifcopenshell_style_add_surface_textures(file, &descriptors, &uv_maps, &textures), "IFC2X3 no-op succeeds");
    ASSERT(ifcopenshell_parse_instance_list_size(textures, &size), "IFC2X3 result size succeeds");
    ASSERT(size == 0, "IFC2X3 result is empty");
    ASSERT(type_count(file, "IfcImageTexture") == 0, "IFC2X3 creates no texture");
    ifcopenshell_parse_instance_list_destroy(textures);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_style_smoke_tests(void) {
    printf("=== Style C ABI tests ===\n");
    tests_run = 0;
    tests_failed = 0;
    test_surface_components_and_texture_records();
    test_ifc2x3_texture_noop();
    printf("Style C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
