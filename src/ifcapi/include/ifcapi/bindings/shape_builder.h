/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SHAPE_BUILDER_H
#define IFCAPI_BINDINGS_SHAPE_BUILDER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

struct ShapeBuilderMepTransitionShapeResult {
    express::Base representation;
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

/**
 * Options for creating a 2D/3D polyline or indexed polycurve.
 */
struct ShapeBuilderPolylineOptions {
    std::vector<std::vector<double>> points;
    std::optional<bool> closed;
    std::optional<std::vector<double>> position_offset;
    std::vector<int> arc_points;
};

/**
 * Options for extruding a profile or curve into a swept solid.
 */
struct ShapeBuilderExtrudeOptions {
    express::Base profile_or_curve;
    double magnitude = 0.0;
    std::vector<double> position;
    std::vector<double> extrusion_vector;
    std::vector<double> position_z_axis;
    std::vector<double> position_x_axis;
    std::optional<std::vector<double>> position_y_axis;
};

struct ShapeBuilderAxis2Placement2dOptions {
    std::vector<double> position;
    std::optional<std::vector<double>> x_direction;
};

struct ShapeBuilderAxis2Placement3dOptions {
    std::vector<double> position;
    std::vector<double> z_axis;
    std::vector<double> x_axis;
};

struct ShapeBuilderProfileOptions {
    express::Base outer_curve;
    std::optional<std::string> name;
    std::vector<express::Base> inner_curves;
    std::optional<std::string> profile_type;
};

struct ShapeBuilderRepresentationOptions {
    express::Base context;
    std::vector<express::Base> items;
    std::optional<std::string> representation_type;
};

struct ShapeBuilderEllipseCurveOptions {
    double x_axis_radius = 0.0;
    double y_axis_radius = 0.0;
    std::vector<double> position;
    std::vector<std::vector<double>> trim_points;
    std::optional<std::vector<double>> ref_x_direction;
    std::vector<int> trim_points_mask;
};

struct ShapeBuilderTranslateOptions {
    express::Base item;
    std::vector<double> translation;
    bool create_copy = false;
};

struct ShapeBuilderRotateOptions {
    express::Base item;
    double angle = 0.0;
    std::vector<double> pivot_point;
    bool counter_clockwise = false;
    bool create_copy = false;
};

struct ShapeBuilderMirrorOptions {
    express::Base item;
    std::vector<double> mirror_axes;
    std::vector<double> mirror_point;
    bool create_copy = false;
    std::vector<double> placement_matrix;
};

struct ShapeBuilderMepTransitionCalculateOptions {
    std::vector<double> start_half_dim;
    std::vector<double> end_half_dim;
    std::vector<double> offset;
    std::optional<std::vector<double>> diff;
    bool end_profile = false;
    std::optional<double> length;
    std::optional<double> angle;
};

struct ShapeBuilderMepTransitionShapeOptions {
    express::Base start_segment;
    express::Base end_segment;
    double start_length = 0.0;
    double end_length = 0.0;
    double angle = 0.0;
    std::vector<double> profile_offset;
};

struct ShapeBuilderSphereOptions {
    double radius = 1.0;
    std::vector<double> center;
};

struct ShapeBuilderBlockOptions {
    std::vector<double> position;
    double x_length = 1.0;
    double y_length = 1.0;
    double z_length = 1.0;
};

struct ShapeBuilderHalfSpaceSolidOptions {
    express::Base plane;
    bool agreement_flag = false;
};

struct ShapeBuilderMepTransitionLengthOptions {
    std::vector<double> start_half_dim;
    std::vector<double> end_half_dim;
    double angle = 0.0;
    std::vector<double> profile_offset;
};

struct ShapeBuilderMepBendShapeOptions {
    express::Base segment;
    double start_length = 0.0;
    double end_length = 0.0;
    double angle = 0.0;
    double radius = 0.0;
    std::vector<double> bend_vector;
    bool flip_z_axis = false;
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
    const ShapeBuilderPolylineOptions& options);
IFCAPI_BINDING express::Base shape_builder_axis2_placement_3d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement3dOptions& options);
IFCAPI_BINDING express::Base shape_builder_axis2_placement_2d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement2dOptions& options);
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
    const ShapeBuilderProfileOptions& options);
IFCAPI_BINDING express::Base shape_builder_sphere(
    ifcopenshell::file* file,
    const ShapeBuilderSphereOptions& options);
IFCAPI_BINDING express::Base shape_builder_block(
    ifcopenshell::file* file,
    const ShapeBuilderBlockOptions& options);
IFCAPI_BINDING express::Base shape_builder_half_space_solid(
    ifcopenshell::file* file,
    const ShapeBuilderHalfSpaceSolidOptions& options);
IFCAPI_BINDING express::Base shape_builder_extrude(
    ifcopenshell::file* file,
    const ShapeBuilderExtrudeOptions& options);
IFCAPI_BINDING express::Base shape_builder_swept_disk_solid(
    ifcopenshell::file* file,
    express::Base* path_curve,
    double radius);
IFCAPI_BINDING express::Base shape_builder_representation(
    ifcopenshell::file* file,
    const ShapeBuilderRepresentationOptions& options);
IFCAPI_BINDING express::Base shape_builder_deep_copy(
    ifcopenshell::file* file,
    express::Base* element);
IFCAPI_BINDING express::Base shape_builder_curve_between_two_points(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points);
IFCAPI_BINDING express::Base shape_builder_ellipse_curve(
    ifcopenshell::file* file,
    const ShapeBuilderEllipseCurveOptions& options);
IFCAPI_BINDING express::Base shape_builder_indexed_polycurve_2d(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& segments);
IFCAPI_BINDING express::Base shape_builder_translate(
    ifcopenshell::file* file,
    const ShapeBuilderTranslateOptions& options);
IFCAPI_BINDING express::Base shape_builder_rotate(
    ifcopenshell::file* file,
    const ShapeBuilderRotateOptions& options);
IFCAPI_BINDING express::Base shape_builder_mirror(
    ifcopenshell::file* file,
    const ShapeBuilderMirrorOptions& options);
IFCAPI_BINDING std::vector<std::vector<double>> shape_builder_get_polyline_coords(
    express::Base* polyline);
IFCAPI_BINDING express::Base shape_builder_set_polyline_coords(
    ifcopenshell::file* file,
    express::Base* polyline,
    const std::vector<std::vector<double>>& coords);
IFCAPI_BINDING double shape_builder_mep_transition_calculate(
    const ShapeBuilderMepTransitionCalculateOptions& options);
IFCAPI_BINDING double shape_builder_mep_transition_length(
    const ShapeBuilderMepTransitionLengthOptions& options);
IFCAPI_BINDING std::optional<ShapeBuilderMepTransitionShapeResult> shape_builder_mep_transition_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepTransitionShapeOptions& options);
IFCAPI_BINDING ShapeBuilderMepBendShapeResult shape_builder_mep_bend_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepBendShapeOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SHAPE_BUILDER_H */
