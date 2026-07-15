// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"

#if IFCOPENSHELL_WITH_IFCGEOM
#include "ifcgeom/function_item_evaluator.h"
#endif

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {

constexpr double pi = 3.14159265358979323846;

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

double real(express::Base entity, const char* attr) { return entity_view(entity).get_or<double>(attr, 0.0); }

double signed_power_term(double length, double coefficient, double degree) {
    if (coefficient == 0.0) return 0.0;
    return length * std::pow(std::abs(coefficient), -1.0 / degree) * (coefficient / std::abs(coefficient));
}

double curve_factor(express::Base parameters) {
    const double start_radius = real(parameters, "StartRadiusOfCurvature");
    const double end_radius = real(parameters, "EndRadiusOfCurvature");
    const double length = real(parameters, "SegmentLength");
    return (end_radius == 0.0 ? 0.0 : length / end_radius) -
           (start_radius == 0.0 ? 0.0 : length / start_radius);
}

express::Base origin_placement(ifcopenshell::file* file) { return axis2d(file, point(file, {0.0, 0.0})); }

express::Base line(ifcopenshell::file* file) {
    auto result = create(file, "IfcLine");
    write_ref_attr(result, "Pnt", point(file, {0.0, 0.0}));
    auto vector = create(file, "IfcVector");
    write_ref_attr(vector, "Orientation", direction(file, {1.0, 0.0}));
    write_double_attr(vector, "Magnitude", 1.0);
    write_ref_attr(result, "Dir", vector);
    return result;
}

void optional_term(express::Base curve, const char* attr, double value) {
    if (value == 0.0) clear_attr(curve, attr);
    else write_double_attr(curve, attr, value);
}

express::Base curve_segment(
    ifcopenshell::file* file,
    express::Base start_point,
    double start_direction,
    double segment_start,
    double segment_length,
    express::Base parent)
{
    auto result = create(file, "IfcCurveSegment");
    write_enum_attr(result, "Transition", "DISCONTINUOUS");
    write_ref_attr(result, "Placement", axis2d(file, start_point, start_direction));
    write_ref_attr(result, "SegmentStart", length_measure(file, segment_start));
    write_ref_attr(result, "SegmentLength", length_measure(file, segment_length));
    write_ref_attr(result, "ParentCurve", parent);
    return result;
}

std::vector<double> helmert_point(double a0, double a1, double a2, double length) {
#if IFCOPENSHELL_WITH_IFCGEOM
    return ifcopenshell::geometry::helmert_curve_point(a0, a1, a2, length);
#else
    throw std::runtime_error("Helmert mapping requires IfcGeom");
#endif
}

express::Base map_line(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double angle = real(p, "StartDirection") * angle_scale;
    return curve_segment(file, read_ref_attr(p, "StartPoint"), angle, 0.0, real(p, "SegmentLength"), line(file));
}

express::Base map_circle(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double angle = real(p, "StartDirection") * angle_scale;
    const double radius = real(p, "StartRadiusOfCurvature");
    auto circle = create(file, "IfcCircle");
    write_ref_attr(circle, "Position", origin_placement(file));
    write_double_attr(circle, "Radius", std::abs(radius));
    return curve_segment(
        file, read_ref_attr(p, "StartPoint"), angle, 0.0,
        real(p, "SegmentLength") * radius / std::abs(radius), circle);
}

express::Base map_clothoid(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double start_radius = real(p, "StartRadiusOfCurvature");
    const double end_radius = real(p, "EndRadiusOfCurvature");
    const double length = real(p, "SegmentLength");
    const double factor = curve_factor(p);
    const double constant = length / std::sqrt(std::abs(factor)) * factor / std::abs(factor);
    auto clothoid = create(file, "IfcClothoid");
    write_ref_attr(clothoid, "Position", origin_placement(file));
    write_double_attr(clothoid, "ClothoidConstant", constant);
    double offset = 0.0;
    if ((std::abs(start_radius) < std::abs(end_radius) && start_radius != 0.0) || end_radius == 0.0) {
        offset = -length - (end_radius != 0.0 ? length * start_radius / (end_radius - start_radius) : 0.0);
    } else if (start_radius != 0.0) {
        offset = length * end_radius / (start_radius - end_radius);
    }
    return curve_segment(
        file, read_ref_attr(p, "StartPoint"), real(p, "StartDirection") * angle_scale,
        offset, length, clothoid);
}

express::Base map_cubic(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double start_radius = real(p, "StartRadiusOfCurvature");
    const double end_radius = real(p, "EndRadiusOfCurvature");
    const double length = real(p, "SegmentLength");
    double coefficient = 0.0;
    double offset = 0.0;
    if (end_radius != 0.0 && start_radius != 0.0 && end_radius != start_radius) {
        const double factor = (start_radius - end_radius) / end_radius;
        coefficient = factor / (6.0 * start_radius * length);
        offset = length / factor;
    } else if (end_radius != 0.0) {
        coefficient = 1.0 / (6.0 * end_radius * length);
    } else if (start_radius != 0.0) {
        coefficient = -1.0 / (6.0 * start_radius * length);
        offset = -length;
    }
    auto polynomial = create(file, "IfcPolynomialCurve");
    write_ref_attr(polynomial, "Position", origin_placement(file));
    write_double_aggregate(polynomial, "CoefficientsX", {0.0, 1.0});
    write_double_aggregate(polynomial, "CoefficientsY", {0.0, 0.0, 0.0, coefficient});
    return curve_segment(
        file, read_ref_attr(p, "StartPoint"), real(p, "StartDirection") * angle_scale,
        offset, length, polynomial);
}

std::vector<express::Base> map_helmert(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double start_radius = real(p, "StartRadiusOfCurvature");
    const double length = real(p, "SegmentLength");
    const double factor = curve_factor(p);
    const double a0_1 = start_radius == 0.0 ? 0.0 : length / start_radius;
    const double a1_1 = 0.0;
    const double a2_1 = 2.0 * factor;
    const double A0_1 = signed_power_term(length, a0_1, 1.0);
    const double A1_1 = signed_power_term(length, a1_1, 2.0);
    const double A2_1 = signed_power_term(length, a2_1, 3.0);
    const auto first_point = helmert_point(A0_1, A1_1, A2_1, length / 2.0);

    auto parent1 = create(file, "IfcSecondOrderPolynomialSpiral");
    write_ref_attr(parent1, "Position", origin_placement(file));
    write_double_attr(parent1, "QuadraticTerm", A2_1);
    optional_term(parent1, "LinearTerm", A1_1);
    optional_term(parent1, "ConstantTerm", A0_1);

    const double start_angle = real(p, "StartDirection") * angle_scale;
    auto first = curve_segment(file, read_ref_attr(p, "StartPoint"), start_angle, 0.0, length / 2.0, parent1);

    const double a0_2 = -factor + (start_radius == 0.0 ? 0.0 : length / start_radius);
    const double a1_2 = 4.0 * factor;
    const double a2_2 = -2.0 * factor;
    const double A0_2 = signed_power_term(length, a0_2, 1.0);
    const double A1_2 = signed_power_term(length, a1_2, 2.0);
    const double A2_2 = signed_power_term(length, a2_2, 3.0);
    const auto second_point = helmert_point(A0_2, A1_2, A2_2, length / 2.0);
    const double parent_angle = first_point[2] - second_point[2];
    const double parent_x = first_point[0] - second_point[0] * std::cos(parent_angle) + second_point[1] * std::sin(parent_angle);
    const double parent_y = first_point[1] - second_point[0] * std::sin(parent_angle) - second_point[1] * std::cos(parent_angle);

    auto parent2 = create(file, "IfcSecondOrderPolynomialSpiral");
    write_ref_attr(parent2, "Position", axis2d(file, point(file, {parent_x, parent_y}), parent_angle));
    write_double_attr(parent2, "QuadraticTerm", A2_2);
    optional_term(parent2, "LinearTerm", A1_2);
    optional_term(parent2, "ConstantTerm", A0_2);

    auto coordinates = read_double_aggregate(read_ref_attr(p, "StartPoint"), "Coordinates");
    const double x = coordinates[0] + first_point[0] * std::cos(start_angle) - first_point[1] * std::sin(start_angle);
    const double y = coordinates[1] + first_point[0] * std::sin(start_angle) + first_point[1] * std::cos(start_angle);
    auto second = curve_segment(file, point(file, {x, y}), start_angle + first_point[2], length / 2.0, length / 2.0, parent2);
    return {first, second};
}

express::Base map_polynomial_spiral(
    ifcopenshell::file* file,
    express::Base p,
    double angle_scale,
    const char* type,
    const std::vector<double>& coefficients)
{
    const double length = real(p, "SegmentLength");
    auto curve = create(file, type);
    write_ref_attr(curve, "Position", origin_placement(file));
    static const char* attrs[] = {
        "ConstantTerm", "LinearTerm", "QuadraticTerm", "CubicTerm",
        "QuarticTerm", "QuinticTerm", "SexticTerm", "SepticTerm"};
    for (size_t i = 0; i < coefficients.size(); ++i) {
        const double term = signed_power_term(length, coefficients[i], static_cast<double>(i + 1));
        if (i == coefficients.size() - 1) write_double_attr(curve, attrs[i], term);
        else optional_term(curve, attrs[i], term);
    }
    return curve_segment(
        file, read_ref_attr(p, "StartPoint"), real(p, "StartDirection") * angle_scale,
        0.0, length, curve);
}

express::Base map_cosine(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double length = real(p, "SegmentLength");
    const double factor = curve_factor(p);
    const double a0 = 0.5 * factor + (real(p, "StartRadiusOfCurvature") == 0.0 ? 0.0 : length / real(p, "StartRadiusOfCurvature"));
    const double a1 = -0.5 * factor;
    auto curve = create(file, "IfcCosineSpiral");
    write_ref_attr(curve, "Position", origin_placement(file));
    write_double_attr(curve, "CosineTerm", signed_power_term(length, a1, 1.0));
    optional_term(curve, "ConstantTerm", signed_power_term(length, a0, 1.0));
    return curve_segment(file, read_ref_attr(p, "StartPoint"), real(p, "StartDirection") * angle_scale, 0.0, length, curve);
}

express::Base map_sine(ifcopenshell::file* file, express::Base p, double angle_scale) {
    const double length = real(p, "SegmentLength");
    const double factor = curve_factor(p);
    const double a0 = real(p, "StartRadiusOfCurvature") == 0.0 ? 0.0 : length / real(p, "StartRadiusOfCurvature");
    auto curve = create(file, "IfcSineSpiral");
    write_ref_attr(curve, "Position", origin_placement(file));
    write_double_attr(curve, "SineTerm", signed_power_term(length, -factor / (2.0 * pi), 1.0));
    optional_term(curve, "LinearTerm", signed_power_term(length, factor, 2.0));
    optional_term(curve, "ConstantTerm", signed_power_term(length, a0, 1.0));
    return curve_segment(file, read_ref_attr(p, "StartPoint"), real(p, "StartDirection") * angle_scale, 0.0, length, curve);
}

express::Base find_cant_segment(express::Base horizontal_segment) {
    auto nests = inverses(horizontal_segment, "Nests");
    if (nests.empty()) return {};
    auto horizontal_layout = read_ref_attr(nests.front(), "RelatingObject");
    auto horizontal_segments = ifcapi::detail::alignment::find_nest_with_related_type(horizontal_layout, "IfcAlignmentSegment");
    auto ordered = refs(horizontal_segments, "RelatedObjects");
    auto it = std::find(ordered.begin(), ordered.end(), horizontal_segment);
    if (it == ordered.end()) return {};
    const size_t index = static_cast<size_t>(std::distance(ordered.begin(), it));
    auto alignment = ifcapi::detail::alignment::first_inverse_relating(horizontal_layout, "Nests", "RelatingObject");
    std::vector<express::Base> candidates;
    for (auto rel : inverses(alignment, "IsNestedBy")) {
        for (auto layout : refs(rel, "RelatedObjects")) if (layout.declaration().is("IfcAlignmentCant")) candidates.push_back(layout);
    }
    for (auto aggregate : inverses(alignment, "IsDecomposedBy")) {
        for (auto child : refs(aggregate, "RelatedObjects")) {
            for (auto rel : inverses(child, "IsNestedBy")) {
                for (auto layout : refs(rel, "RelatedObjects")) if (layout.declaration().is("IfcAlignmentCant")) candidates.push_back(layout);
            }
        }
    }
    for (auto layout : candidates) {
        auto nest = find_nest_with_related_type(layout, "IfcAlignmentSegment");
        auto segments = refs(nest, "RelatedObjects");
        if (index < segments.size()) return segments[index];
    }
    return {};
}

express::Base map_viennese(ifcopenshell::file* file, express::Base segment, double angle_scale) {
    auto p = read_ref_attr(segment, "DesignParameters");
    const double length = real(p, "SegmentLength");
    const double start_radius = real(p, "StartRadiusOfCurvature");
    const double gravity_height = real(p, "GravityCenterLineHeight");
    double start_left = 0.0, end_left = 0.0, start_right = 0.0, end_right = 0.0, rail = 1.0;
    auto cant_segment = find_cant_segment(segment);
    if (cant_segment) {
        auto cp = read_ref_attr(cant_segment, "DesignParameters");
        start_left = real(cp, "StartCantLeft");
        start_right = real(cp, "StartCantRight");
        end_left = entity_view(cp).get_optional<double>("EndCantLeft").value_or(0.0);
        end_right = entity_view(cp).get_optional<double>("EndCantRight").value_or(0.0);
        auto cant_layout = first_inverse_relating(cant_segment, "Nests", "RelatingObject");
        rail = real(cant_layout, "RailHeadDistance");
    }
    const double start_angle = rail == 0.0 ? 0.0 : (start_right - start_left) / rail;
    const double end_angle = rail == 0.0 ? 0.0 : (end_right - end_left) / rail;
    const double cant_factor = -420.0 * gravity_height / length * (end_angle - start_angle);
    const double factor = curve_factor(p);
    const double a0 = start_radius == 0.0 ? 0.0 : length / start_radius;
    return map_polynomial_spiral(
        file, p, angle_scale, "IfcSeventhOrderPolynomialSpiral",
        {a0, 0.0, cant_factor, -4.0 * cant_factor,
         5.0 * cant_factor + 35.0 * factor,
         -2.0 * cant_factor - 84.0 * factor, 70.0 * factor, -20.0 * factor});
}

} // namespace

namespace ifcapi::detail::alignment {

std::vector<express::Base> map_horizontal(ifcopenshell::file* file, express::Base segment) {
    require_ifc4x3(file);
    require_owned(file, segment, "segment");
    require_type(segment, "IfcAlignmentSegment", "segment");
    auto parameters = ifcapi::detail::read_ref_attr(segment, "DesignParameters");
    require_type(parameters, "IfcAlignmentHorizontalSegment", "DesignParameters");
    const auto type = enum_value(parameters, "PredefinedType");
    const double angle_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "PLANEANGLEUNIT");
    if (type == "LINE") return {map_line(file, parameters, angle_scale)};
    if (type == "CIRCULARARC") return {map_circle(file, parameters, angle_scale)};
    if (type == "CLOTHOID") return {map_clothoid(file, parameters, angle_scale)};
    if (type == "CUBIC") return {map_cubic(file, parameters, angle_scale)};
    if (type == "HELMERTCURVE") return map_helmert(file, parameters, angle_scale);
    if (type == "BLOSSCURVE") {
        const double factor = curve_factor(parameters);
        const double length = real(parameters, "SegmentLength");
        const double a0 = real(parameters, "StartRadiusOfCurvature") == 0.0 ? 0.0 : length / real(parameters, "StartRadiusOfCurvature");
        return {map_polynomial_spiral(file, parameters, angle_scale, "IfcThirdOrderPolynomialSpiral", {a0, 0.0, 3.0 * factor, -2.0 * factor})};
    }
    if (type == "COSINECURVE") return {map_cosine(file, parameters, angle_scale)};
    if (type == "SINECURVE") return {map_sine(file, parameters, angle_scale)};
    if (type == "VIENNESEBEND") return {map_viennese(file, segment, angle_scale)};
    throw std::runtime_error("Unsupported horizontal alignment segment type: " + type);
}

} // namespace ifcapi::detail::alignment
