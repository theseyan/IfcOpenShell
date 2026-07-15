// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

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
    ASSERT(ifcopenshell_parse_instance_list_size(values, &size), "list size succeeds");
    ifcopenshell_parse_instance_list_destroy(values);
    return size;
}

static void test_copy_and_named_constituents(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *material = NULL;
    ifcopenshell_instance_t *copied = NULL;
    ifcopenshell_instance_t *wall = NULL;
    ifcopenshell_instance_t *context = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcMaterial", &material), "create material succeeds");
    ASSERT(ifcopenshell_material_copy_material(file, material, &copied), "copy material succeeds");
    ASSERT(copied != NULL, "copy returns an entity");
    ASSERT(type_count(file, "IfcMaterial") == 2, "copy creates one material");

    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &wall), "create wall succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(
               file, "IfcGeometricRepresentationContext", &context),
           "create context succeeds");
    ifcopenshell_material_constituent_entry_options_t record = {"Part", material};
    ifcopenshell_material_constituent_entry_options_list_t records = {&record, 1};
    ifcopenshell_material_set_shape_aspect_constituents_options_t options = {0};
    ASSERT(ifcopenshell_material_set_shape_aspect_constituents(
               file, wall, context, &records, &options),
           "first named assignment succeeds without representation");
    ASSERT(type_count(file, "IfcMaterialConstituentSet") == 1, "one constituent set exists");
    ASSERT(type_count(file, "IfcMaterialConstituent") == 1, "one constituent exists");
    ASSERT(ifcopenshell_material_set_shape_aspect_constituents(
               file, wall, context, &records, &options),
           "idempotent named assignment succeeds");
    ASSERT(type_count(file, "IfcMaterialConstituentSet") == 1, "matching set is reused");

    size_t before = type_count(file, "IfcWall");
    ifcopenshell_instance_t *invalid_copy = NULL;
    ASSERT(!ifcopenshell_material_copy_material(file, wall, &invalid_copy), "unsupported copy is rejected");
    ASSERT(invalid_copy == NULL, "unsupported copy returns no partial entity");
    ASSERT(type_count(file, "IfcWall") == before, "unsupported copy leaves graph unchanged");

    ifcopenshell_instance_destroy(context);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_instance_destroy(copied);
    ifcopenshell_instance_destroy(material);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_material_smoke_tests(void) {
    printf("=== Material C ABI tests ===\n");
    tests_run = 0;
    tests_failed = 0;
    test_copy_and_named_constituents();
    printf("Material C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
