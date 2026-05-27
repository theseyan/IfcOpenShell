/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SHAPE_BUILDER_H
#define IFCAPI_BINDINGS_SHAPE_BUILDER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <vector>

namespace ifcapi {
namespace bindings {

struct ShapeBuilderMepTransitionShapeResult {
    express::Base representation;
    bool has_result = false;
    double start_length = 0.0;
    double end_length = 0.0;
    double angle = 0.0;
    std::vector<double> profile_offset;
    double transition_length = 0.0;
    double full_transition_length = 0.0;
};

struct ShapeBuilderMepBendShapeResult {
    express::Base representation;
    double start_length = 0.0;
    double end_length = 0.0;
    double radius = 0.0;
    double angle = 0.0;
    int lateral_axis = 0;
    double lateral_sign = 0.0;
    int z_axis_sign = 1;
    double main_profile_dimension = 0.0;
};

IFCAPI_BINDING express::Base shape_builder_mesh(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& faces);
IFCAPI_BINDING express::Base shape_builder_faceted_brep(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& faces);
IFCAPI_BINDING express::Base shape_builder_triangulated_face_set(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& faces);
IFCAPI_BINDING express::Base shape_builder_polygonal_face_set(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<std::vector<int>>>& faces);
IFCAPI_BINDING express::Base shape_builder_vertex(
    ifcopenshell::file* file,
    const std::vector<double>& position);
IFCAPI_BINDING express::Base shape_builder_edge(
    ifcopenshell::file* file,
    const std::vector<double>& start,
    const std::vector<double>& end);
IFCAPI_BINDING express::Base shape_builder_face(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points);
IFCAPI_BINDING express::Base shape_builder_polyline(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    bool closed,
    const std::vector<double>& position_offset,
    bool has_position_offset,
    const std::vector<int>& arc_points);
IFCAPI_BINDING express::Base shape_builder_axis2_placement_3d(
    ifcopenshell::file* file,
    const std::vector<double>& position,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis);
IFCAPI_BINDING express::Base shape_builder_axis2_placement_2d(
    ifcopenshell::file* file,
    const std::vector<double>& position,
    const std::vector<double>& x_direction,
    bool has_x_direction);
IFCAPI_BINDING express::Base shape_builder_circle(
    ifcopenshell::file* file,
    const std::vector<double>& center,
    double radius);
IFCAPI_BINDING express::Base shape_builder_plane(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& normal);
IFCAPI_BINDING express::Base shape_builder_profile(
    ifcopenshell::file* file,
    express::Base* outer_curve,
    const char* name,
    const std::vector<express::Base>& inner_curves,
    const char* profile_type);
IFCAPI_BINDING express::Base shape_builder_sphere(
    ifcopenshell::file* file,
    double radius,
    const std::vector<double>& center);
IFCAPI_BINDING express::Base shape_builder_block(
    ifcopenshell::file* file,
    const std::vector<double>& position,
    double x_length,
    double y_length,
    double z_length);
IFCAPI_BINDING express::Base shape_builder_half_space_solid(
    ifcopenshell::file* file,
    express::Base* plane,
    bool agreement_flag);
IFCAPI_BINDING express::Base shape_builder_extrude(
    ifcopenshell::file* file,
    express::Base* profile_or_curve,
    double magnitude,
    const std::vector<double>& position,
    const std::vector<double>& extrusion_vector,
    const std::vector<double>& position_z_axis,
    const std::vector<double>& position_x_axis,
    const std::vector<double>& position_y_axis,
    bool has_position_y_axis);
IFCAPI_BINDING express::Base shape_builder_swept_disk_solid(
    ifcopenshell::file* file,
    express::Base* path_curve,
    double radius);
IFCAPI_BINDING express::Base shape_builder_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<express::Base>& items,
    const char* representation_type);
IFCAPI_BINDING express::Base shape_builder_deep_copy(
    ifcopenshell::file* file,
    express::Base* element);
IFCAPI_BINDING express::Base shape_builder_curve_between_two_points(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points);
IFCAPI_BINDING express::Base shape_builder_ellipse_curve(
    ifcopenshell::file* file,
    double x_axis_radius,
    double y_axis_radius,
    const std::vector<double>& position,
    const std::vector<std::vector<double>>& trim_points,
    const std::vector<double>& ref_x_direction,
    const std::vector<int>& trim_points_mask);
IFCAPI_BINDING express::Base shape_builder_indexed_polycurve_2d(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& segments);
IFCAPI_BINDING express::Base shape_builder_translate(
    ifcopenshell::file* file,
    express::Base* item,
    const std::vector<double>& translation,
    bool create_copy);
IFCAPI_BINDING express::Base shape_builder_rotate(
    ifcopenshell::file* file,
    express::Base* item,
    double angle,
    const std::vector<double>& pivot_point,
    bool counter_clockwise,
    bool create_copy);
IFCAPI_BINDING express::Base shape_builder_mirror(
    ifcopenshell::file* file,
    express::Base* item,
    const std::vector<double>& mirror_axes,
    const std::vector<double>& mirror_point,
    bool create_copy,
    const std::vector<double>& placement_matrix);
IFCAPI_BINDING std::vector<std::vector<double>> shape_builder_get_polyline_coords(
    express::Base* polyline);
IFCAPI_BINDING express::Base shape_builder_set_polyline_coords(
    ifcopenshell::file* file,
    express::Base* polyline,
    const std::vector<std::vector<double>>& coords);
IFCAPI_BINDING double shape_builder_mep_transition_calculate(
    const std::vector<double>& start_half_dim,
    const std::vector<double>& end_half_dim,
    const std::vector<double>& offset,
    const std::vector<double>& diff,
    bool has_diff,
    bool end_profile,
    double length,
    bool has_length,
    double angle,
    bool has_angle);
IFCAPI_BINDING double shape_builder_mep_transition_length(
    const std::vector<double>& start_half_dim,
    const std::vector<double>& end_half_dim,
    double angle,
    const std::vector<double>& profile_offset);
IFCAPI_BINDING ShapeBuilderMepTransitionShapeResult shape_builder_mep_transition_shape(
    ifcopenshell::file* file,
    express::Base* start_segment,
    express::Base* end_segment,
    double start_length,
    double end_length,
    double angle,
    const std::vector<double>& profile_offset);
IFCAPI_BINDING ShapeBuilderMepBendShapeResult shape_builder_mep_bend_shape(
    ifcopenshell::file* file,
    express::Base* segment,
    double start_length,
    double end_length,
    double angle,
    double radius,
    const std::vector<double>& bend_vector,
    bool flip_z_axis);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SHAPE_BUILDER_H */
