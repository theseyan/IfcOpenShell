// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/alignment.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace {

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

express::Base rooted(ifcopenshell::file* file, const char* type) {
    auto result = create(file, type);
    write_string_attr(result, "GlobalId", ifcapi::guid_new());
    return result;
}

express::Base project(ifcopenshell::file* file) {
    auto result = ifcapi::detail::first_instance_by_type(file, "IfcProject");
    if (!result) throw std::runtime_error("Alignment authoring requires an IfcProject");
    return result;
}

void aggregate_to_project(ifcopenshell::file* file, express::Base product) {
    ifcapi::bindings::AggregateAssignObjectOptions options;
    options.products = {product};
    options.relating_object = project(file);
    ifcapi::bindings::aggregate_assign_object(file, options);
}

void nest(ifcopenshell::file* file, express::Base parent, const std::vector<express::Base>& children) {
    ifcapi::bindings::NestAssignObjectOptions options;
    options.products = children;
    options.relating_object = parent;
    ifcapi::bindings::nest_assign_object(file, options);
}

express::Base local_placement(ifcopenshell::file* file, bool is_3d = false) {
    auto result = create(file, "IfcLocalPlacement");
    write_ref_attr(result, "RelativePlacement", is_3d
        ? axis3d(file, point(file, {0.0, 0.0, 0.0}), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0})
        : axis2d(file, point(file, {0.0, 0.0})));
    return result;
}

void validate_point(const std::array<double, 2>& point, const char* name) {
    if (!std::isfinite(point[0]) || !std::isfinite(point[1])) {
        throw std::runtime_error(std::string(name) + " must contain finite coordinates");
    }
}

void validate_horizontal_pis(const ifcapi::bindings::AlignmentHorizontalPiLayout& pis) {
    validate_point(pis.start_point, "horizontal start_point");
    validate_point(pis.end_point, "horizontal end_point");
    for (const auto& intersection : pis.intersections) {
        validate_point(intersection.point, "horizontal PI point");
        if (!std::isfinite(intersection.radius)) throw std::runtime_error("horizontal PI radius must be finite");
    }
}

void validate_vertical_pis(const ifcapi::bindings::AlignmentVerticalPiLayout& pis) {
    validate_point(pis.start_point, "vertical start_point");
    validate_point(pis.end_point, "vertical end_point");
    for (const auto& intersection : pis.intersections) {
        validate_point(intersection.point, "vertical PI point");
        if (!std::isfinite(intersection.curve_length)) {
            throw std::runtime_error("vertical PI curve_length must be finite");
        }
    }
}

void assign_root_history(
    ifcopenshell::file* file,
    express::Base root,
    const std::optional<express::Base>& history,
    const std::optional<express::Base>& user,
    const std::optional<express::Base>& application)
{
    auto value = history.value_or(express::Base());
    if (!value && user && application) value = ifcapi::detail::ensure_owner_history(file, {}, *user, *application);
    if (value) write_ref_attr(root, "OwnerHistory", value);
}

std::vector<double> parse_csv_row(const std::string& row) {
    std::vector<double> values;
    std::stringstream stream(row);
    std::string value;
    while (std::getline(stream, value, ',')) {
        if (!value.empty() && value.back() == '\r') value.pop_back();
        if (!value.empty()) values.push_back(std::stod(value));
    }
    return values;
}

void decode_pi_row(
    const std::vector<double>& values,
    std::vector<std::vector<double>>& points,
    std::vector<double>& transitions)
{
    size_t index = 0;
    while (index < values.size()) {
        if (index + 1 >= values.size()) throw std::runtime_error("CSV alignment row has an incomplete coordinate pair");
        points.push_back({values[index], values[index + 1]});
        index += 2;
        if (index < values.size() && (index + 1) % 3 == 0) transitions.push_back(values[index++]);
    }
    if (transitions.size() >= 2) transitions = {transitions.begin() + 1, transitions.end() - 1};
    else transitions.clear();
}

ifcapi::bindings::AlignmentHorizontalPiLayout horizontal_pis(
    const std::vector<std::vector<double>>& points,
    const std::vector<double>& radii)
{
    if (points.size() < 2) throw std::runtime_error("horizontal_points must contain at least two points");
    for (const auto& point : points) {
        if (point.size() != 2) throw std::runtime_error("horizontal_points points must contain two values");
    }
    if (points.size() - 2 != radii.size()) {
        throw std::runtime_error("radii should have two fewer elements than points");
    }
    ifcapi::bindings::AlignmentHorizontalPiLayout result;
    result.start_point = {points.front()[0], points.front()[1]};
    result.end_point = {points.back()[0], points.back()[1]};
    for (size_t i = 0; i < radii.size(); ++i) {
        result.intersections.push_back({{points[i + 1][0], points[i + 1][1]}, radii[i]});
    }
    validate_horizontal_pis(result);
    return result;
}

ifcapi::bindings::AlignmentVerticalPiLayout vertical_pis(
    const std::vector<std::vector<double>>& points,
    const std::vector<double>& lengths)
{
    if (points.size() < 2) throw std::runtime_error("vertical_points must contain at least two points");
    for (const auto& point : points) {
        if (point.size() != 2) throw std::runtime_error("vertical_points points must contain two values");
    }
    if (points.size() - 2 != lengths.size()) {
        throw std::runtime_error("lengths should have two fewer elements than points");
    }
    ifcapi::bindings::AlignmentVerticalPiLayout result;
    result.start_point = {points.front()[0], points.front()[1]};
    result.end_point = {points.back()[0], points.back()[1]};
    for (size_t i = 0; i < lengths.size(); ++i) {
        result.intersections.push_back({{points[i + 1][0], points[i + 1][1]}, lengths[i]});
    }
    validate_vertical_pis(result);
    return result;
}

} // namespace

namespace ifcapi::bindings {

AlignmentCreateLayoutSegmentResult alignment_create_layout_segment(
    ifcopenshell::file* file,
    express::Base layout,
    express::Base design_parameters)
{
    require_ifc4x3(file);
    require_owned(file, layout, "layout");
    require_owned(file, design_parameters, "design_parameters");
    const bool horizontal = layout.declaration().is("IfcAlignmentHorizontal");
    const bool vertical = layout.declaration().is("IfcAlignmentVertical");
    const bool cant = layout.declaration().is("IfcAlignmentCant");
    if (!horizontal && !vertical && !cant) throw std::runtime_error("layout must be an alignment layout");
    if ((horizontal && !design_parameters.declaration().is("IfcAlignmentHorizontalSegment")) ||
        (vertical && !design_parameters.declaration().is("IfcAlignmentVerticalSegment")) ||
        (cant && !design_parameters.declaration().is("IfcAlignmentCantSegment"))) {
        throw std::runtime_error("design_parameters subtype does not match layout");
    }
    auto segment = rooted(file, "IfcAlignmentSegment");
    write_ref_attr(segment, "DesignParameters", design_parameters);
    return alignment_add_segment_to_layout(file, layout, segment);
}

AlignmentCreateLayoutSegmentResult alignment_add_segment_to_layout(
    ifcopenshell::file* file,
    express::Base layout,
    express::Base segment)
{
    require_ifc4x3(file);
    require_owned(file, layout, "layout");
    require_owned(file, segment, "segment");
    require_type(segment, "IfcAlignmentSegment", "segment");
    const bool horizontal = layout.declaration().is("IfcAlignmentHorizontal");
    const bool vertical = layout.declaration().is("IfcAlignmentVertical");
    const bool cant = layout.declaration().is("IfcAlignmentCant");
    if (!horizontal && !vertical && !cant) throw std::runtime_error("layout must be an alignment layout");
    auto design_parameters = read_ref_attr(segment, "DesignParameters");
    if ((horizontal && !design_parameters.declaration().is("IfcAlignmentHorizontalSegment")) ||
        (vertical && !design_parameters.declaration().is("IfcAlignmentVerticalSegment")) ||
        (cant && !design_parameters.declaration().is("IfcAlignmentCantSegment"))) {
        throw std::runtime_error("segment DesignParameters subtype does not match layout");
    }
    auto before = alignment_get_layout_segments(layout);
    const bool had_zero = !before.empty() && alignment_has_zero_length_segment(layout);
    express::Base prior_zero = had_zero ? before.back() : express::Base();
    nest(file, layout, {segment});
    auto ordered = alignment_get_layout_segments(layout);
    if (had_zero && ordered.size() >= 2) {
        std::swap(ordered[ordered.size() - 1], ordered[ordered.size() - 2]);
        auto relation = alignment_get_alignment_segment_nest(layout);
        write_ref_aggregate(*relation, "RelatedObjects", ordered);
    }
    Matrix endpoint_matrix = endpoint(file, segment);
    if (prior_zero) update_zero_placement(file, prior_zero, endpoint_matrix);
    auto curve = alignment_get_layout_curve(layout);
    if (curve) append_segment_to_curve(file, segment, *curve, &endpoint_matrix);
    AlignmentCreateLayoutSegmentResult result;
    result.segment = segment;
    result.endpoint.assign(endpoint_matrix.begin(), endpoint_matrix.end());
    result.has_endpoint = true;
    return result;
}

void alignment_layout_horizontal_by_pi_method(
    ifcopenshell::file* file,
    express::Base layout,
    const AlignmentLayoutHorizontalByPiMethodOptions& options)
{
    const auto& pis = options.pis;
    require_ifc4x3(file);
    require_owned(file, layout, "layout");
    require_type(layout, "IfcAlignmentHorizontal", "layout");
    validate_horizontal_pis(pis);
    const double angle_scale = unit_calculate_unit_scale(file, "PLANEANGLEUNIT");
    double x_back = pis.start_point[0], y_back = pis.start_point[1];
    const auto& first_pi = pis.intersections.empty() ? pis.end_point : pis.intersections.front().point;
    double x_pi = first_pi[0], y_pi = first_pi[1];
    for (size_t index = 0; index < pis.intersections.size(); ++index) {
        const double input_radius = pis.intersections[index].radius;
        const double dx_back = x_pi - x_back, dy_back = y_pi - y_back;
        const double angle_back = std::atan2(dy_back, dx_back);
        const double back_length = std::hypot(dx_back, dy_back);
        const auto& forward = index + 1 < pis.intersections.size()
            ? pis.intersections[index + 1].point
            : pis.end_point;
        const double angle_forward = std::atan2(forward[1] - y_pi, forward[0] - x_pi);
        const double delta = angle_forward - angle_back;
        const double tangent = std::abs(input_radius * std::tan(delta / 2.0));
        const double arc_length = std::abs(input_radius * delta);
        const double radius = delta == 0.0 ? input_radius : input_radius * delta / std::abs(delta);
        const double x_pc = x_pi - tangent * std::cos(angle_back);
        const double y_pc = y_pi - tangent * std::sin(angle_back);
        const double x_pt = x_pi + tangent * std::cos(angle_forward);
        const double y_pt = y_pi + tangent * std::sin(angle_forward);
        if (back_length - tangent > 1e-3) {
            auto p = create(file, "IfcAlignmentHorizontalSegment");
            write_ref_attr(p, "StartPoint", point(file, {x_back, y_back}));
            write_double_attr(p, "StartDirection", angle_back / angle_scale);
            write_double_attr(p, "StartRadiusOfCurvature", 0.0);
            write_double_attr(p, "EndRadiusOfCurvature", 0.0);
            write_double_attr(p, "SegmentLength", back_length - tangent);
            write_enum_attr(p, "PredefinedType", "LINE");
            alignment_create_layout_segment(file, layout, p);
        }
        if (radius != 0.0) {
            auto p = create(file, "IfcAlignmentHorizontalSegment");
            write_ref_attr(p, "StartPoint", point(file, {x_pc, y_pc}));
            write_double_attr(p, "StartDirection", angle_back / angle_scale);
            write_double_attr(p, "StartRadiusOfCurvature", radius);
            write_double_attr(p, "EndRadiusOfCurvature", radius);
            write_double_attr(p, "SegmentLength", arc_length);
            write_enum_attr(p, "PredefinedType", "CIRCULARARC");
            alignment_create_layout_segment(file, layout, p);
        }
        x_back = x_pt; y_back = y_pt; x_pi = forward[0]; y_pi = forward[1];
    }
    const double dx = x_pi - x_back, dy = y_pi - y_back;
    const double length = std::hypot(dx, dy);
    if (length > 1e-3) {
        auto p = create(file, "IfcAlignmentHorizontalSegment");
        write_ref_attr(p, "StartPoint", point(file, {x_back, y_back}));
        write_double_attr(p, "StartDirection", std::atan2(dy, dx) / angle_scale);
        write_double_attr(p, "StartRadiusOfCurvature", 0.0);
        write_double_attr(p, "EndRadiusOfCurvature", 0.0);
        write_double_attr(p, "SegmentLength", length);
        write_enum_attr(p, "PredefinedType", "LINE");
        alignment_create_layout_segment(file, layout, p);
    }
}

void alignment_layout_vertical_by_pi_method(
    ifcopenshell::file* file,
    express::Base layout,
    const AlignmentLayoutVerticalByPiMethodOptions& options)
{
    const auto& pis = options.pis;
    require_ifc4x3(file);
    require_owned(file, layout, "layout");
    require_type(layout, "IfcAlignmentVertical", "layout");
    validate_vertical_pis(pis);
    double x_begin = pis.start_point[0], y_begin = pis.start_point[1];
    const auto& first_pi = pis.intersections.empty() ? pis.end_point : pis.intersections.front().point;
    double x_pi = first_pi[0], y_pi = first_pi[1];
    for (size_t index = 0; index < pis.intersections.size(); ++index) {
        const double length = pis.intersections[index].curve_length;
        const double dx_back = x_pi - x_begin, dy_back = y_pi - y_begin;
        const double start_slope = dy_back / dx_back;
        const auto& forward = index + 1 < pis.intersections.size()
            ? pis.intersections[index + 1].point
            : pis.end_point;
        const double end_slope = (forward[1] - y_pi) / (forward[0] - x_pi);
        const double x_end = x_pi + length / 2.0;
        const double y_end = y_pi + end_slope * length / 2.0;
        const double gradient_length = dx_back - length / 2.0;
        if (gradient_length > 1e-3) {
            auto p = create(file, "IfcAlignmentVerticalSegment");
            write_double_attr(p, "StartDistAlong", x_begin);
            write_double_attr(p, "HorizontalLength", gradient_length);
            write_double_attr(p, "StartHeight", y_begin);
            write_double_attr(p, "StartGradient", start_slope);
            write_double_attr(p, "EndGradient", start_slope);
            write_enum_attr(p, "PredefinedType", "CONSTANTGRADIENT");
            alignment_create_layout_segment(file, layout, p);
        }
        if (length > 0.0) {
            const double k = (end_slope - start_slope) / length;
            auto p = create(file, "IfcAlignmentVerticalSegment");
            write_double_attr(p, "StartDistAlong", x_pi - length / 2.0);
            write_double_attr(p, "HorizontalLength", length);
            write_double_attr(p, "StartHeight", y_pi - start_slope * length / 2.0);
            write_double_attr(p, "StartGradient", start_slope);
            write_double_attr(p, "EndGradient", end_slope);
            write_double_attr(p, "RadiusOfCurvature", 1.0 / k);
            write_enum_attr(p, "PredefinedType", "PARABOLICARC");
            alignment_create_layout_segment(file, layout, p);
        }
        x_begin = x_end; y_begin = y_end; x_pi = forward[0]; y_pi = forward[1];
    }
    const double dx = x_pi - x_begin, dy = y_pi - y_begin;
    if (dx > 1e-3) {
        const double slope = dy / dx;
        auto p = create(file, "IfcAlignmentVerticalSegment");
        write_double_attr(p, "StartDistAlong", x_begin);
        write_double_attr(p, "HorizontalLength", dx);
        write_double_attr(p, "StartHeight", y_begin);
        write_double_attr(p, "StartGradient", slope);
        write_double_attr(p, "EndGradient", slope);
        write_enum_attr(p, "PredefinedType", "CONSTANTGRADIENT");
        alignment_create_layout_segment(file, layout, p);
    }
}

express::Base alignment_create(ifcopenshell::file* file, const AlignmentCreateOptions& options) {
    require_ifc4x3(file);
    project(file);
    if (options.include_cant && !options.include_vertical) throw std::runtime_error("Cant layout requires a vertical layout");
    auto alignment = rooted(file, "IfcAlignment");
    write_string_attr(alignment, "Name", options.name);
    write_ref_attr(alignment, "ObjectPlacement", local_placement(file));
    assign_root_history(file, alignment, options.owner_history, options.user, options.application);
    std::vector<express::Base> layouts;
    layouts.push_back(rooted(file, "IfcAlignmentHorizontal"));
    if (options.include_vertical) layouts.push_back(rooted(file, "IfcAlignmentVertical"));
    if (options.include_cant) {
        auto cant = rooted(file, "IfcAlignmentCant");
        write_double_attr(cant, "RailHeadDistance", 1.0);
        layouts.push_back(cant);
    }
    nest(file, alignment, layouts);
    if (options.include_geometry) create_geometric_representation(file, alignment);
    AlignmentAddStationingReferentOptions station;
    station.alignment = alignment;
    station.distance_along = 0.0;
    station.station = options.start_station;
    station.name = station_string(file, options.start_station);
    station.positioned_product = alignment;
    station.owner_history = options.owner_history;
    station.user = options.user;
    station.application = options.application;
    alignment_add_stationing_referent(file, station);
    for (auto layout : layouts) {
        alignment_add_zero_length_segment(file, layout);
        auto curve = alignment_get_layout_curve(layout);
        if (curve) alignment_add_zero_length_segment(file, *curve);
    }
    aggregate_to_project(file, alignment);
    return alignment;
}

express::Base alignment_create_by_pi_method(
    ifcopenshell::file* file,
    const AlignmentCreateByPiMethodOptions& options)
{
    validate_horizontal_pis(options.horizontal);
    if (options.vertical) validate_vertical_pis(*options.vertical);
    const double start_station = options.start_station.value_or(0.0);
    if (!std::isfinite(start_station)) throw std::runtime_error("start_station must be finite");
    AlignmentCreateOptions create_options;
    create_options.name = options.name;
    create_options.include_vertical = options.vertical.has_value();
    create_options.start_station = start_station;
    create_options.owner_history = options.owner_history;
    create_options.user = options.user;
    create_options.application = options.application;
    auto alignment = alignment_create(file, create_options);
    AlignmentLayoutHorizontalByPiMethodOptions horizontal_options;
    horizontal_options.pis = options.horizontal;
    alignment_layout_horizontal_by_pi_method(file, *alignment_get_horizontal_layout(alignment), horizontal_options);
    if (options.vertical) {
        AlignmentLayoutVerticalByPiMethodOptions vertical_options;
        vertical_options.pis = *options.vertical;
        alignment_layout_vertical_by_pi_method(file, *alignment_get_vertical_layout(alignment), vertical_options);
    }
    return alignment;
}

express::Base alignment_create_from_csv_text(
    ifcopenshell::file* file,
    const AlignmentCreateFromCsvTextOptions& csv_options)
{
    require_ifc4x3(file);
    std::stringstream stream(csv_options.csv_text);
    std::string row;
    std::vector<std::vector<double>> rows;
    while (std::getline(stream, row)) if (!row.empty()) rows.push_back(parse_csv_row(row));
    if (rows.empty()) throw std::runtime_error("CSV alignment input is empty");
    std::vector<std::vector<double>> horizontal_points;
    std::vector<double> radii;
    decode_pi_row(rows.front(), horizontal_points, radii);
    const auto horizontal = horizontal_pis(horizontal_points, radii);
    std::vector<AlignmentVerticalPiLayout> vertical_layouts;
    for (size_t i = 1; i < rows.size(); ++i) {
        std::vector<std::vector<double>> vertical_points;
        std::vector<double> lengths;
        decode_pi_row(rows[i], vertical_points, lengths);
        vertical_layouts.push_back(vertical_pis(vertical_points, lengths));
    }
    AlignmentCreateOptions options;
    options.name = "Alignment_from_CSV";
    options.owner_history = csv_options.owner_history;
    options.user = csv_options.user;
    options.application = csv_options.application;
    auto alignment = alignment_create(file, options);
    AlignmentLayoutHorizontalByPiMethodOptions horizontal_options;
    horizontal_options.pis = horizontal;
    alignment_layout_horizontal_by_pi_method(file, *alignment_get_horizontal_layout(alignment), horizontal_options);
    for (const auto& vertical_pis : vertical_layouts) {
        auto vertical = alignment_add_vertical_layout(file, alignment);
        AlignmentLayoutVerticalByPiMethodOptions vertical_options;
        vertical_options.pis = vertical_pis;
        alignment_layout_vertical_by_pi_method(file, vertical, vertical_options);
    }
    return alignment;
}

express::Base alignment_create_as_polyline(
    ifcopenshell::file* file,
    const AlignmentCreatePolylineOptions& options)
{
    require_ifc4x3(file);
    project(file);
    if (options.points.size() < 2) throw std::runtime_error("points must contain at least two IfcCartesianPoint entities");
    size_t dimension = 0;
    for (auto value : options.points) {
        require_owned(file, value, "point"); require_type(value, "IfcCartesianPoint", "point");
        const auto coordinates = read_double_aggregate(value, "Coordinates");
        if (dimension == 0) dimension = coordinates.size();
        if ((dimension != 2 && dimension != 3) || coordinates.size() != dimension) throw std::runtime_error("polyline points must have consistent 2D or 3D coordinates");
    }
    auto alignment = rooted(file, "IfcAlignment");
    write_string_attr(alignment, "Name", options.name);
    write_ref_attr(alignment, "ObjectPlacement", local_placement(file, dimension == 3));
    assign_root_history(file, alignment, options.owner_history, options.user, options.application);
    auto curve = create(file, "IfcPolyline");
    write_ref_aggregate(curve, "Points", options.points);
    auto representation = create(file, "IfcShapeRepresentation");
    write_ref_attr(representation, "ContextOfItems", alignment_get_axis_subcontext(file));
    write_string_attr(representation, "RepresentationIdentifier", "Axis");
    write_string_attr(representation, "RepresentationType", dimension == 3 ? "Curve3D" : "Curve2D");
    write_ref_aggregate(representation, "Items", {curve});
    geometry_assign_representation(file, &alignment, &representation);
    AlignmentAddStationingReferentOptions station;
    station.alignment = alignment;
    station.station = options.start_station;
    station.name = station_string(file, options.start_station);
    station.positioned_product = alignment;
    station.owner_history = options.owner_history;
    station.user = options.user;
    station.application = options.application;
    alignment_add_stationing_referent(file, station);
    aggregate_to_project(file, alignment);
    return alignment;
}

express::Base alignment_create_as_offset_curve(
    ifcopenshell::file* file,
    const AlignmentCreateOffsetCurveOptions& options)
{
    require_ifc4x3(file);
    project(file);
    if (options.offsets.empty()) throw std::runtime_error("offsets must not be empty");
    express::Base basis;
    for (auto offset : options.offsets) {
        require_owned(file, offset, "offset"); require_type(offset, "IfcPointByDistanceExpression", "offset");
        auto value = read_ref_attr(offset, "BasisCurve");
        if (!basis) basis = value;
        else if (basis != value) throw std::runtime_error("all offsets must share one basis curve");
    }
    auto alignment = rooted(file, "IfcAlignment");
    write_string_attr(alignment, "Name", options.name);
    assign_root_history(file, alignment, options.owner_history, options.user, options.application);
    const bool is_3d = entity_view(basis).get_or<int>("Dim", 2) == 3;
    write_ref_attr(alignment, "ObjectPlacement", local_placement(file, is_3d));
    auto curve = create(file, "IfcOffsetCurveByDistances");
    write_ref_attr(curve, "BasisCurve", basis);
    write_ref_aggregate(curve, "OffsetValues", options.offsets);
    auto representation = create(file, "IfcShapeRepresentation");
    write_ref_attr(representation, "ContextOfItems", alignment_get_axis_subcontext(file));
    write_string_attr(representation, "RepresentationIdentifier", "Axis");
    write_string_attr(representation, "RepresentationType", is_3d ? "Curve3D" : "Curve2D");
    write_ref_aggregate(representation, "Items", {curve});
    geometry_assign_representation(file, &alignment, &representation);
    aggregate_to_project(file, alignment);
    return alignment;
}

} // namespace ifcapi::bindings
