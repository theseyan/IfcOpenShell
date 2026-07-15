// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, m); tests_failed++; } } while (0)

static ifcopenshell_double_list_list_t path_view(double (*points)[3], size_t size, ifcopenshell_double_list_t *rows) {
    for (size_t i = 0; i < size; ++i) rows[i] = (ifcopenshell_double_list_t){points[i], 3, NULL};
    return (ifcopenshell_double_list_list_t){rows, size, NULL};
}

static ifcopenshell_geometry_compute_wall_mounted_handrail_options_t options_for(
    const ifcopenshell_double_list_list_t *path, const char *terminal) {
    ifcopenshell_geometry_compute_wall_mounted_handrail_options_t options = {0};
    options.railing_path = path;
    options.support_spacing = 1.0;
    options.railing_diameter = 0.05;
    options.clear_width = 0.04;
    options.height = 1.0;
    options.terminal_type = terminal;
    options.has_terminal_type = true;
    options.unit_scale = 1.0;
    options.has_unit_scale = true;
    return options;
}

static void test_pure_compute_and_owned_nested_results(void) {
    double points[2][3] = {{0, 0, 1}, {2, 0, 1}};
    ifcopenshell_double_list_t rows[2];
    ifcopenshell_double_list_list_t path = path_view(points, 2, rows);
    const char *terminals[] = {"180", "TO_END_POST", "TO_WALL", "TO_FLOOR", "TO_END_POST_AND_FLOOR", "NONE"};
    for (size_t t = 0; t < sizeof(terminals) / sizeof(terminals[0]); ++t) {
        ifcopenshell_geometry_compute_wall_mounted_handrail_options_t options = options_for(&path, terminals[t]);
        ifcopenshell_geometry_wall_mounted_handrail_result_t result = {0};
        ASSERT(ifcopenshell_geometry_compute_wall_mounted_handrail_geometry(&options, &result), "pure compute succeeds without a file");
        ASSERT(result.handrail_radius == 0.025, "handrail radius is half the diameter");
        ASSERT(result.supports.size == 3, "automatic compute returns three support records");
        ASSERT(result.supports.items != NULL, "multiple nested result records are owned");
        ASSERT(result.supports.items[0].arc_polyline.size == 3, "support contains a three-point nested numeric array");
        ASSERT(result.supports.items[0].disk_position.size == 3, "support disk position is a numeric array");
        ASSERT(isfinite(result.supports.items[0].disk_z_rotation), "support rotation is finite");
        if (strcmp(terminals[t], "NONE") == 0) {
            ASSERT(result.handrail_polyline.size == 2, "NONE leaves the path open");
            ASSERT(result.handrail_arc_point_indices.size == 0, "NONE has no cap arc indices");
        }
        ifcopenshell_geometry_wall_mounted_handrail_result_destroy(&result);
        ASSERT(result.supports.items == NULL && result.supports.size == 0, "result destruction clears nested record storage");
    }
}

static void test_empty_record_list_and_repeated_destruction(void) {
    double points[2][3] = {{0, 0, 1}, {2, 0, 1}};
    ifcopenshell_double_list_t rows[2];
    ifcopenshell_double_list_list_t path = path_view(points, 2, rows);
    ifcopenshell_geometry_compute_wall_mounted_handrail_options_t options = options_for(&path, "NONE");
    options.use_manual_supports = true;
    options.has_use_manual_supports = true;
    options.support_spacing = 0.0;
    for (int i = 0; i < 50; ++i) {
        ifcopenshell_geometry_wall_mounted_handrail_result_t result = {0};
        ASSERT(ifcopenshell_geometry_compute_wall_mounted_handrail_geometry(&options, &result), "manual compute repeatedly succeeds");
        ASSERT(result.supports.size == 0 && result.supports.items == NULL, "manual two-point path returns an empty record list");
        ifcopenshell_geometry_wall_mounted_handrail_result_destroy(&result);
        ifcopenshell_geometry_wall_mounted_handrail_result_destroy(&result);
    }
}

static void test_collinear_degenerate_and_invalid_inputs(void) {
    double points[4][3] = {{0, 0, 1}, {0, 0, 1}, {0.6, 0.8, 1}, {1.2, 1.6, 1}};
    ifcopenshell_double_list_t rows[4];
    ifcopenshell_double_list_list_t path = path_view(points, 4, rows);
    ifcopenshell_geometry_compute_wall_mounted_handrail_options_t options = options_for(&path, "NONE");
    ifcopenshell_geometry_wall_mounted_handrail_result_t result = {0};
    ASSERT(ifcopenshell_geometry_compute_wall_mounted_handrail_geometry(&options, &result), "coincident and collinear path computes");
    for (size_t i = 0; i < result.handrail_polyline.size; ++i)
        for (size_t j = 0; j < result.handrail_polyline.items[i].size; ++j)
            ASSERT(isfinite(result.handrail_polyline.items[i].items[j]), "degenerate result coordinate is finite");
    ifcopenshell_geometry_wall_mounted_handrail_result_destroy(&result);

    options.terminal_type = "INVALID";
    ASSERT(!ifcopenshell_geometry_compute_wall_mounted_handrail_geometry(&options, &result), "invalid terminal fails");
    ASSERT(strstr(ifcopenshell_last_error_message(), "terminal") != NULL, "invalid terminal sets a clear last error");
    options.terminal_type = "NONE";
    options.railing_diameter = 0.0;
    ASSERT(!ifcopenshell_geometry_compute_wall_mounted_handrail_geometry(&options, &result), "invalid diameter fails");
    ASSERT(ifcopenshell_last_error_message()[0] != '\0', "invalid dimension preserves last error behavior");
}

static void test_ifc_wrapper_defaults_and_atomicity(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *project = NULL, *parent = NULL, *context = NULL, *representation = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ifcopenshell_root_create_entity_options_t project_options = {0};
    project_options.ifc_class = "IfcProject";
    ASSERT(ifcopenshell_root_create_entity(file, &project_options, &project), "project creation succeeds");
    ifcopenshell_context_add_context_options_t parent_options = {0};
    parent_options.context_type = "Model";
    parent_options.context_identifier = "";
    parent_options.target_view = "";
    ASSERT(ifcopenshell_context_add_context(file, &parent_options, &parent), "parent context creation succeeds");
    ifcopenshell_context_add_context_options_t context_options = {0};
    context_options.context_type = "Model";
    context_options.context_identifier = "Body";
    context_options.target_view = "MODEL_VIEW";
    context_options.parent = parent;
    context_options.has_parent = true;
    ASSERT(ifcopenshell_context_add_context(file, &context_options, &context), "body context creation succeeds");

    double points[2][3] = {{0, 0, 1}, {2, 0, 1}};
    ifcopenshell_double_list_t rows[2];
    ifcopenshell_double_list_list_t path = path_view(points, 2, rows);
    ifcopenshell_geometry_add_railing_representation_options_t options = {0};
    options.context = context;
    options.railing_path = &path;
    options.has_railing_path = true;
    options.terminal_type = "NONE";
    options.has_terminal_type = true;
    ASSERT(ifcopenshell_geometry_add_railing_representation(file, &options, &representation), "IFC wrapper resolves omitted dimension and flag defaults");
    bool is_representation = false;
    ASSERT(ifcopenshell_instance_is_a(representation, "IfcShapeRepresentation", &is_representation) && is_representation, "wrapper returns an IfcShapeRepresentation");

    ifcopenshell_parse_attribute_value_t *items_value = NULL;
    ifcopenshell_parse_instance_list_t *items = NULL;
    size_t item_count = 0;
    ASSERT(ifcopenshell_instance_get_argument_by_name(representation, "Items", &items_value), "representation items read succeeds");
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(items_value, &items), "representation items are an instance list");
    ASSERT(ifcopenshell_parse_instance_list_size(items, &item_count) && item_count == 7, "default two-metre railing has seven items");

    ifcopenshell_parse_instance_list_t *before_list = NULL, *after_list = NULL;
    size_t before = 0, after = 0;
    ASSERT(ifcopenshell_file_by_type(file, "IfcSweptDiskSolid", &before_list), "query before invalid wrapper succeeds");
    ASSERT(ifcopenshell_parse_instance_list_size(before_list, &before), "count before invalid wrapper succeeds");
    double invalid_point[1][3] = {{0, 0, 1}};
    ifcopenshell_double_list_t invalid_row[1];
    ifcopenshell_double_list_list_t invalid_path = path_view(invalid_point, 1, invalid_row);
    options.railing_path = &invalid_path;
    ifcopenshell_instance_t *invalid_result = NULL;
    ASSERT(!ifcopenshell_geometry_add_railing_representation(file, &options, &invalid_result), "invalid wrapper input fails");
    ASSERT(ifcopenshell_file_by_type(file, "IfcSweptDiskSolid", &after_list), "query after invalid wrapper succeeds");
    ASSERT(ifcopenshell_parse_instance_list_size(after_list, &after) && after == before, "failed compute leaves no partial IFC solids");

    ifcopenshell_parse_instance_list_destroy(after_list);
    ifcopenshell_parse_instance_list_destroy(before_list);
    ifcopenshell_parse_instance_list_destroy(items);
    ifcopenshell_parse_attribute_value_destroy(items_value);
    ifcopenshell_instance_destroy(representation);
    ifcopenshell_instance_destroy(context);
    ifcopenshell_instance_destroy(parent);
    ifcopenshell_instance_destroy(project);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_geometry_smoke_tests(void) {
    printf("=== Geometry C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_pure_compute_and_owned_nested_results();
    test_empty_record_list_and_repeated_destruction();
    test_collinear_degenerate_and_invalid_inputs();
    test_ifc_wrapper_defaults_and_atomicity();
    printf("Geometry C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
