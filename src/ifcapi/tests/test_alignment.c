// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"
#include <stdio.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s (%s)\n", __FILE__, __LINE__, m, ifcopenshell_last_error_message()); tests_failed++; } } while (0)

static size_t list_size(ifcopenshell_parse_instance_list_t *list) {
    size_t result = 0;
    ASSERT(ifcopenshell_parse_instance_list_size(list, &result), "instance list size succeeds");
    return result;
}

static ifcopenshell_file_t *new_alignment_file(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *project = NULL, *model = NULL, *axis = NULL, *units = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4X3_ADD2", 0, "", &file), "new IFC4X3 file succeeds");
    ifcopenshell_root_create_entity_options_t project_options = {0};
    project_options.ifc_class = "IfcProject";
    project_options.name = "Project";
    ASSERT(ifcopenshell_root_create_entity(file, &project_options, &project), "project creation succeeds");
    ifcopenshell_unit_assign_unit_options_t unit_options = {0};
    ASSERT(ifcopenshell_unit_assign_unit(file, &unit_options, &units), "default project units creation succeeds");
    ifcopenshell_context_add_context_options_t model_options = {0};
    model_options.context_type = "Model";
    model_options.context_identifier = "";
    model_options.target_view = "";
    ASSERT(ifcopenshell_context_add_context(file, &model_options, &model), "model context creation succeeds");
    ifcopenshell_context_add_context_options_t axis_options = {0};
    axis_options.context_type = "Model";
    axis_options.context_identifier = "Axis";
    axis_options.target_view = "MODEL_VIEW";
    axis_options.parent = model;
    axis_options.has_parent = true;
    ASSERT(ifcopenshell_context_add_context(file, &axis_options, &axis), "axis context creation succeeds");
    ifcopenshell_instance_destroy(axis);
    ifcopenshell_instance_destroy(model);
    ifcopenshell_instance_destroy(units);
    ifcopenshell_instance_destroy(project);
    return file;
}

static void test_creation_queries_pi_and_csv(void) {
    ifcopenshell_file_t *file = new_alignment_file();
    ifcopenshell_instance_t *alignment = NULL, *layout = NULL, *curve = NULL;
    ifcopenshell_alignment_create_options_t create = {0};
    create.name = "C alignment";
    create.include_geometry = false;
    ASSERT(ifcopenshell_alignment_create(file, &create, &alignment), "semantic alignment creation succeeds");
    ASSERT(ifcopenshell_alignment_get_horizontal_layout(alignment, &layout) && layout != NULL, "horizontal layout query succeeds");
    bool has_zero = false, added = true;
    ASSERT(ifcopenshell_alignment_has_zero_length_segment(layout, &has_zero) && has_zero, "semantic zero segment exists");
    ASSERT(ifcopenshell_alignment_add_zero_length_segment(file, layout, &added) && !added, "zero segment addition is idempotent");
    ASSERT(ifcopenshell_alignment_get_curve(alignment, &curve) && curve == NULL, "nullable curve query returns null without geometry");

    double p0[] = {0.0, 0.0}, p1[] = {100.0, 0.0}, p2[] = {200.0, 100.0};
    ifcopenshell_double_list_t point_rows[] = {{p0, 2, NULL}, {p1, 2, NULL}, {p2, 2, NULL}};
    ifcopenshell_double_list_list_t points = {point_rows, 3, NULL};
    double radius_values[] = {25.0};
    ifcopenshell_double_list_t radii = {radius_values, 1, NULL};
    ifcopenshell_double_list_list_t empty_points = {0};
    ifcopenshell_double_list_t empty_values = {0};
    ifcopenshell_alignment_create_by_pi_method_options_t pi = {0};
    pi.name = "PI alignment";
    pi.horizontal_points = &points;
    pi.radii = &radii;
    pi.vertical_points = &empty_points;
    pi.vertical_lengths = &empty_values;
    ifcopenshell_instance_t *pi_alignment = NULL, *pi_layout = NULL, *first = NULL;
    ifcopenshell_parse_instance_list_t *segments = NULL, *mapped = NULL;
    ASSERT(ifcopenshell_alignment_create_by_pi_method(file, &pi, &pi_alignment), "PI authoring succeeds");
    ASSERT(ifcopenshell_alignment_get_horizontal_layout(pi_alignment, &pi_layout), "PI layout query succeeds");
    ASSERT(ifcopenshell_alignment_get_layout_segments(pi_layout, &segments) && list_size(segments) >= 4, "PI layout creates ordered segments");
    ASSERT(ifcopenshell_parse_instance_list_get(segments, 0, &first), "first PI segment lookup succeeds");
    ifcopenshell_alignment_map_segment_options_t map = {0};
    map.segment = first;
    ASSERT(ifcopenshell_alignment_map_segment(file, &map, &mapped) && list_size(mapped) == 1, "representative horizontal segment maps through C ABI");

    ifcopenshell_alignment_create_from_csv_text_options_t csv = {0};
    csv.csv_text = "0,0,0,100,0,25,200,100,0\n0,10,0,100,12,30,200,10,0\n";
    ifcopenshell_instance_t *csv_alignment = NULL, *vertical = NULL;
    ASSERT(ifcopenshell_alignment_create_from_csv_text(file, &csv, &csv_alignment), "semantic CSV text authoring succeeds");
    ASSERT(ifcopenshell_alignment_get_vertical_layout(csv_alignment, &vertical) && vertical != NULL, "CSV creates its vertical layout");

    ifcopenshell_instance_destroy(vertical);
    ifcopenshell_instance_destroy(csv_alignment);
    ifcopenshell_parse_instance_list_destroy(mapped);
    ifcopenshell_instance_destroy(first);
    ifcopenshell_parse_instance_list_destroy(segments);
    ifcopenshell_instance_destroy(pi_layout);
    ifcopenshell_instance_destroy(pi_alignment);
    ifcopenshell_instance_destroy(layout);
    ifcopenshell_instance_destroy(alignment);
    ifcopenshell_file_destroy(file);
}

static void test_helmert_maps_two_segments(void) {
    ifcopenshell_file_t *file = new_alignment_file();
    ifcopenshell_instance_t *point = NULL, *parameters = NULL, *segment = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcCartesianPoint", &point), "point creation succeeds");
    double coordinates_values[] = {0.0, 0.0};
    ifcopenshell_double_list_t coordinates = {coordinates_values, 2, NULL};
    ASSERT(ifcopenshell_instance_set_argument_double_list(point, 0, &coordinates), "point coordinates assignment succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcAlignmentHorizontalSegment", &parameters), "Helmert parameters creation succeeds");
    ASSERT(ifcopenshell_instance_set_argument_instance(parameters, 2, point), "Helmert start point assignment succeeds");
    ASSERT(ifcopenshell_instance_set_argument_double(parameters, 3, 0.0), "Helmert direction assignment succeeds");
    ASSERT(ifcopenshell_instance_set_argument_double(parameters, 4, 0.0), "Helmert start radius assignment succeeds");
    ASSERT(ifcopenshell_instance_set_argument_double(parameters, 5, 300.0), "Helmert end radius assignment succeeds");
    ASSERT(ifcopenshell_instance_set_argument_double(parameters, 6, 100.0), "Helmert length assignment succeeds");
    bool enum_set = false;
    ASSERT(ifcopenshell_instance_set_argument_enumeration_by_name(parameters, 8, "HELMERTCURVE", &enum_set) && enum_set, "Helmert type assignment succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcAlignmentSegment", &segment), "alignment segment creation succeeds");
    ASSERT(ifcopenshell_instance_set_argument_instance(segment, 7, parameters), "design parameters assignment succeeds");
    ifcopenshell_alignment_map_segment_options_t options = {0};
    options.segment = segment;
    ifcopenshell_parse_instance_list_t *mapped = NULL;
    ASSERT(ifcopenshell_alignment_map_segment(file, &options, &mapped) && list_size(mapped) == 2, "Helmert maps to two curve segments");
    ifcopenshell_parse_instance_list_destroy(mapped);
    ifcopenshell_instance_destroy(segment);
    ifcopenshell_instance_destroy(parameters);
    ifcopenshell_instance_destroy(point);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_alignment_smoke_tests(void) {
    printf("=== Alignment C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_creation_queries_pi_and_csv();
    test_helmert_maps_two_segments();
    printf("Alignment C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
