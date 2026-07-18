// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s (%s)\n", __FILE__, __LINE__, m, ifcopenshell_last_error_message()); tests_failed++; } } while (0)

static size_t count_type(ifcopenshell_file_t *file, const char *type) {
    ifcopenshell_parse_instance_list_t *items = NULL;
    size_t size = 0;
    ASSERT(ifcopenshell_file_by_type(file, type, &items), "type query succeeds");
    ASSERT(ifcopenshell_parse_instance_list_size(items, &size), "type count succeeds");
    ifcopenshell_parse_instance_list_destroy(items);
    return size;
}

static ifcopenshell_double_list_list_t point_rows(double (*points)[2], size_t size, ifcopenshell_double_list_t *rows) {
    for (size_t i = 0; i < size; ++i) rows[i] = (ifcopenshell_double_list_t){points[i], 2, NULL};
    return (ifcopenshell_double_list_list_t){rows, size, NULL};
}

static void test_semantic_segments_defaults_and_atomicity(void) {
    ifcopenshell_file_t *file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");

    double points[4][2] = {{0, 0}, {2, 0}, {2, 1}, {0, 1}};
    ifcopenshell_double_list_t rows[4];
    ifcopenshell_double_list_list_t point_list = point_rows(points, 4, rows);
    ifcopenshell_double_list_list_any_2_double_list_list_any_3_variant_t point_variant = {0};
    point_variant.kind = 0;
    point_variant.value_0 = point_list;
    uint32_t line_values[] = {0, 1, 2, 3, 0};
    ifcopenshell_uint32_list_t line_indices = {line_values, 5, NULL};
    ifcopenshell_shape_builder_line_segment_t line = {&line_indices};
    ifcopenshell_ShapeBuilderLineSegment_ShapeBuilderArcSegment_variant_t segment = {0};
    segment.kind = 0;
    segment.value_0 = &line;
    ifcopenshell_ShapeBuilderLineSegment_ShapeBuilderArcSegment_variant_list_t segments = {&segment, 1};
    ifcopenshell_shape_builder_polyline_options_t polyline_options = {0};
    polyline_options.points = &point_variant;
    polyline_options.segments = &segments;
    polyline_options.has_segments = true;
    ifcopenshell_instance_t *polyline = NULL;
    ASSERT(ifcopenshell_shape_builder_polyline(file, &polyline_options, &polyline), "semantic line segment creates a polyline");

    ifcopenshell_shape_builder_extrude_options_t extrude_options = {0};
    extrude_options.profile_or_curve = polyline;
    ifcopenshell_instance_t *extrusion = NULL;
    ASSERT(ifcopenshell_shape_builder_extrude(file, &extrude_options, &extrusion), "extrude resolves omitted placement, direction, and magnitude defaults");
    ifcopenshell_parse_attribute_value_t *depth_value = NULL;
    double depth = 0.0;
    ASSERT(ifcopenshell_instance_get_argument_by_name(extrusion, "Depth", &depth_value), "extrusion depth read succeeds");
    ASSERT(ifcopenshell_parse_attribute_value_as_double(depth_value, &depth) && depth == 1.0, "omitted extrusion magnitude defaults to one");

    size_t before = count_type(file, "IfcIndexedPolyCurve");
    uint32_t invalid_values[] = {0};
    ifcopenshell_uint32_list_t invalid_indices = {invalid_values, 1, NULL};
    line.line_indices = &invalid_indices;
    ifcopenshell_instance_t *invalid = NULL;
    ASSERT(!ifcopenshell_shape_builder_polyline(file, &polyline_options, &invalid), "a line segment with fewer than two indices is rejected");
    ASSERT(strstr(ifcopenshell_last_error_message(), "line segment") != NULL, "invalid line cardinality reports its semantic cause");
    ASSERT(count_type(file, "IfcIndexedPolyCurve") == before, "invalid segments are rejected before IFC mutation");

    ifcopenshell_parse_attribute_value_destroy(depth_value);
    ifcopenshell_instance_destroy(extrusion);
    ifcopenshell_instance_destroy(polyline);
    ifcopenshell_file_destroy(file);
}

static void test_fixed_vectors_and_mixed_dimensions(void) {
    double origin_values[] = {0, 0, 0};
    double z_values[] = {0, 0, 1};
    double x_values[] = {1, 0, 0};
    ifcopenshell_double_list_t origin = {origin_values, 3, NULL};
    ifcopenshell_double_list_t z_axis = {z_values, 3, NULL};
    ifcopenshell_double_list_t x_axis = {x_values, 3, NULL};
    ifcopenshell_double_list_t matrix = {0};
    ASSERT(ifcopenshell_placement_matrix_from_axes(&origin, &z_axis, &x_axis, &matrix), "fixed XYZ axes create a placement matrix");
    ASSERT(matrix.size == 16, "placement matrix has a fixed sixteen-value result");
    ifcopenshell_double_list_destroy(&matrix);

    origin.size = 2;
    ASSERT(!ifcopenshell_placement_matrix_from_axes(&origin, &z_axis, &x_axis, &matrix), "short XYZ input is rejected by the native ABI");
    ASSERT(strstr(ifcopenshell_last_error_message(), "cardinality") != NULL, "fixed XYZ rejection reports its cardinality cause");

    ifcopenshell_file_t *file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    double points[2][3] = {{0, 0, 0}, {1, 0, 0}};
    ifcopenshell_double_list_t rows[2] = {
        {points[0], 2, NULL},
        {points[1], 3, NULL},
    };
    ifcopenshell_double_list_list_t mixed_points = {rows, 2, NULL};
    ifcopenshell_double_list_list_any_2_double_list_list_any_3_variant_t mixed_variant = {0};
    mixed_variant.kind = 0;
    mixed_variant.value_0 = mixed_points;
    ifcopenshell_shape_builder_polyline_options_t options = {0};
    options.points = &mixed_variant;
    ifcopenshell_instance_t *result = NULL;
    const size_t before = count_type(file, "IfcIndexedPolyCurve");
    ASSERT(!ifcopenshell_shape_builder_polyline(file, &options, &result), "mixed XY and XYZ points are rejected by the native ABI");
    ASSERT(count_type(file, "IfcIndexedPolyCurve") == before, "mixed-dimensional points fail before IFC mutation");
    ifcopenshell_file_destroy(file);
}

static void test_trim_alternatives_and_mep_xor(void) {
    ifcopenshell_file_t *file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    double trim_points[2][2] = {{1, 0}, {0, 1}};
    ifcopenshell_double_list_t trim_rows[2];
    ifcopenshell_double_list_list_t trim_list = point_rows(trim_points, 2, trim_rows);
    ifcopenshell_shape_builder_ellipse_point_trim_t point_trim = {&trim_list};
    uint32_t cardinal_values[] = {0, 1};
    ifcopenshell_uint32_list_t cardinal_points = {cardinal_values, 2, NULL};
    ifcopenshell_shape_builder_ellipse_cardinal_trim_t cardinal_trim = {&cardinal_points};
    ifcopenshell_ShapeBuilderEllipsePointTrim_ShapeBuilderEllipseCardinalTrim_variant_t trim_value = {0};
    trim_value.kind = 0;
    trim_value.value_0 = &point_trim;
    ifcopenshell_shape_builder_ellipse_trim_t trim = {&trim_value};
    ifcopenshell_shape_builder_ellipse_curve_options_t ellipse = {0};
    ellipse.x_axis_radius = 2.0;
    ellipse.y_axis_radius = 1.0;
    ellipse.trim = &trim;
    ellipse.has_trim = true;
    size_t before = count_type(file, "IfcEllipse");
    ifcopenshell_instance_t *result = NULL;
    ASSERT(ifcopenshell_shape_builder_ellipse_curve(file, &ellipse, &result), "explicit ellipse point trim creates a trimmed curve");
    ifcopenshell_instance_destroy(result);
    trim_value.kind = 1;
    trim_value.value_1 = &cardinal_trim;
    ASSERT(ifcopenshell_shape_builder_ellipse_curve(file, &ellipse, &result), "explicit ellipse cardinal trim creates a trimmed curve");
    ifcopenshell_instance_destroy(result);
    cardinal_values[1] = 4;
    before = count_type(file, "IfcEllipse");
    ASSERT(!ifcopenshell_shape_builder_ellipse_curve(file, &ellipse, &result), "out-of-range cardinal trim is rejected");
    ASSERT(count_type(file, "IfcEllipse") == before, "invalid ellipse trim is rejected before mutation");

    ifcopenshell_shape_builder_mep_profile_half_dimensions_t start = {1.0, 0.5, 0.0};
    ifcopenshell_shape_builder_mep_profile_half_dimensions_t end = {0.5, 0.25, 0.0};
    ifcopenshell_shape_builder_mep_offset_t offset = {0.0, 0.0};
    ifcopenshell_shape_builder_mep_transition_from_angle_t from_angle = {30.0};
    ifcopenshell_shape_builder_mep_transition_from_length_t from_length = {2.0};
    ifcopenshell_shape_builder_mep_transition_calculate_options_t calculate = {0};
    calculate.start_half_dim = &start;
    calculate.end_half_dim = &end;
    calculate.offset = &offset;
    ifcopenshell_ShapeBuilderMepTransitionFromLength_ShapeBuilderMepTransitionFromAngle_variant_t calculation = {0};
    calculation.kind = 1;
    calculation.value_1 = &from_angle;
    calculate.calculation = &calculation;
    double calculated = NAN;
    ASSERT(ifcopenshell_shape_builder_mep_transition_calculate(&calculate, &calculated) && isfinite(calculated), "named MEP dimensions and angle calculate a finite transition length");
    calculation.kind = 0;
    calculation.value_0 = &from_length;
    ASSERT(ifcopenshell_shape_builder_mep_transition_calculate(&calculate, &calculated) && isfinite(calculated), "named MEP dimensions and length calculate a finite transition angle");

    ifcopenshell_file_destroy(file);
}

int ifcapi_run_shape_builder_smoke_tests(void) {
    printf("=== ShapeBuilder C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_semantic_segments_defaults_and_atomicity();
    test_fixed_vectors_and_mixed_dimensions();
    test_trim_alternatives_and_mep_xor();
    printf("ShapeBuilder C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
