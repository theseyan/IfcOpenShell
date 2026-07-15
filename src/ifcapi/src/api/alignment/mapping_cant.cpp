// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/alignment.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"

#include <cmath>
#include <stdexcept>

namespace {

constexpr double pi = 3.14159265358979323846;

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

double real(express::Base entity, const char* attr) { return entity_view(entity).get_or<double>(attr, 0.0); }

double signed_term(double length, double coefficient, double degree) {
    if (coefficient == 0.0) return 0.0;
    return std::pow(length, (degree + 1.0) / degree) *
           std::pow(std::abs(coefficient), -1.0 / degree) * coefficient / std::abs(coefficient);
}

express::Base cant_axis(ifcopenshell::file* file, double ds, double rail) {
    const double dy = 2.0 * ds;
    const double dz = std::sqrt(rail * rail - dy * dy);
    return direction(file, {0.0, dy / rail, dz / rail});
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

express::Base segment(
    ifcopenshell::file* file,
    double distance,
    double elevation,
    double axis_cant,
    double rail,
    double start_direction,
    double segment_start,
    double length,
    express::Base parent)
{
    auto result = create(file, "IfcCurveSegment");
    write_enum_attr(result, "Transition", "DISCONTINUOUS");
    auto placement = create(file, "IfcAxis2Placement3D");
    write_ref_attr(placement, "Location", point(file, {distance, elevation, 0.0}));
    write_ref_attr(placement, "Axis", cant_axis(file, axis_cant, rail));
    write_ref_attr(placement, "RefDirection", direction(file, {std::cos(start_direction), std::sin(start_direction), 0.0}));
    write_ref_attr(result, "Placement", placement);
    write_ref_attr(result, "SegmentStart", length_measure(file, segment_start));
    write_ref_attr(result, "SegmentLength", length_measure(file, length));
    write_ref_attr(result, "ParentCurve", parent);
    return result;
}

express::Base polynomial_spiral(
    ifcopenshell::file* file,
    const char* type,
    const std::vector<double>& terms,
    double length)
{
    auto result = create(file, type);
    write_ref_attr(result, "Position", origin_placement(file));
    static const char* attrs[] = {
        "ConstantTerm", "LinearTerm", "QuadraticTerm", "CubicTerm",
        "QuarticTerm", "QuinticTerm", "SexticTerm", "SepticTerm"};
    for (size_t i = 0; i < terms.size(); ++i) {
        const double value = signed_term(length, terms[i], static_cast<double>(i + 1));
        if (i == terms.size() - 1) write_double_attr(result, attrs[i], value);
        else optional_term(result, attrs[i], value);
    }
    return result;
}

std::vector<express::Base> map_constant(ifcopenshell::file* file, express::Base p, double rail) {
    const double left = real(p, "StartCantLeft");
    const double right = real(p, "StartCantRight");
    const double mean = 0.5 * (left + right);
    return {segment(file, real(p, "StartDistAlong"), mean, 0.5 * (right - left), rail, 0.0, 0.0,
                    real(p, "HorizontalLength"), line(file))};
}

std::vector<express::Base> map_linear(ifcopenshell::file* file, express::Base p, double rail) {
    const double length = real(p, "HorizontalLength");
    const double start = 0.5 * (real(p, "StartCantLeft") + real(p, "StartCantRight"));
    const double end = 0.5 * (real(p, "EndCantLeft") + real(p, "EndCantRight"));
    const double delta = end - start;
    const double A0 = signed_term(length, start, 1.0);
    const double A1 = signed_term(length, delta, 2.0);
    auto parent = create(file, "IfcClothoid");
    write_ref_attr(parent, "Position", origin_placement(file));
    write_double_attr(parent, "ClothoidConstant", A1);
    const double y = A0 == 0.0 ? 0.0 : length * length / A0;
    const double direction_angle = std::atan(A1 * length * length / std::abs(A1 * A1 * A1));
    return {segment(file, real(p, "StartDistAlong"), y, start, rail, direction_angle, 0.0, length, parent)};
}

std::vector<express::Base> map_helmert(ifcopenshell::file* file, express::Base p, double rail) {
    const double distance = real(p, "StartDistAlong");
    const double length = real(p, "HorizontalLength");
    const double start = real(p, "StartCantLeft") + real(p, "StartCantRight");
    const double end = real(p, "EndCantLeft") + real(p, "EndCantRight");
    const double delta = end - start;
    auto parent1 = polynomial_spiral(file, "IfcSecondOrderPolynomialSpiral", {2.0 * start, 0.0, 4.0 * delta}, length);
    auto first = segment(file, distance, start / 2.0, start / 2.0, rail, 0.0, 0.0, length / 2.0, parent1);

    const double a0 = -2.0 * delta + 2.0 * start;
    const double a1 = 8.0 * delta;
    const double a2 = -4.0 * delta;
    auto parent2 = polynomial_spiral(file, "IfcSecondOrderPolynomialSpiral", {a0, a1, a2}, length);
    const double A2_1 = signed_term(length, 4.0 * delta, 3.0);
    const double half = length / 2.0;
    const double slope = half * half * (2.0 * half / std::pow(A2_1, 3.0));
    auto second = segment(
        file, distance + half, start / 2.0 + delta / 4.0, (start + end) / 4.0,
        rail, std::atan(slope), half, half, parent2);
    return {first, second};
}

std::vector<express::Base> map_bloss(ifcopenshell::file* file, express::Base p, double rail) {
    const double length = real(p, "HorizontalLength");
    const double start = 0.5 * (real(p, "StartCantLeft") + real(p, "StartCantRight"));
    const double end = 0.5 * (real(p, "EndCantLeft") + real(p, "EndCantRight"));
    auto parent = polynomial_spiral(file, "IfcThirdOrderPolynomialSpiral", {start, 0.0, 3.0 * (end - start), -2.0 * (end - start)}, length);
    return {segment(file, real(p, "StartDistAlong"), start, start, rail, 0.0, 0.0, length, parent)};
}

std::vector<express::Base> map_cosine(ifcopenshell::file* file, express::Base p, double rail) {
    const double length = real(p, "HorizontalLength");
    const double start = 0.5 * (real(p, "StartCantLeft") + real(p, "StartCantRight"));
    const double end = 0.5 * (real(p, "EndCantLeft") + real(p, "EndCantRight"));
    const double delta = end - start;
    auto parent = create(file, "IfcCosineSpiral");
    write_ref_attr(parent, "Position", origin_placement(file));
    write_double_attr(parent, "CosineTerm", signed_term(length, -0.5 * delta, 1.0));
    optional_term(parent, "ConstantTerm", signed_term(length, start + 0.5 * delta, 1.0));
    return {segment(file, real(p, "StartDistAlong"), start, start, rail, 0.0, 0.0, length, parent)};
}

std::vector<express::Base> map_sine(ifcopenshell::file* file, express::Base p, double rail) {
    const double length = real(p, "HorizontalLength");
    const double start = 0.5 * (real(p, "StartCantLeft") + real(p, "StartCantRight"));
    const double end = 0.5 * (real(p, "EndCantLeft") + real(p, "EndCantRight"));
    const double delta = end - start;
    auto parent = create(file, "IfcSineSpiral");
    write_ref_attr(parent, "Position", origin_placement(file));
    write_double_attr(parent, "SineTerm", signed_term(length, -delta / (2.0 * pi), 1.0));
    optional_term(parent, "LinearTerm", signed_term(length, delta, 2.0));
    optional_term(parent, "ConstantTerm", signed_term(length, start, 1.0));
    return {segment(file, real(p, "StartDistAlong"), start, start, rail, 0.0, 0.0, length, parent)};
}

std::vector<express::Base> map_viennese(ifcopenshell::file* file, express::Base p, double rail) {
    const double length = real(p, "HorizontalLength");
    const double start = 0.5 * (real(p, "StartCantLeft") + real(p, "StartCantRight"));
    const double end = 0.5 * (real(p, "EndCantLeft") + real(p, "EndCantRight"));
    const double delta = end - start;
    auto parent = polynomial_spiral(
        file, "IfcSeventhOrderPolynomialSpiral",
        {start, 0.0, 0.0, 0.0, 35.0 * delta, -84.0 * delta, 70.0 * delta, -20.0 * delta}, length);
    return {segment(file, real(p, "StartDistAlong"), start, start, rail, 0.0, 0.0, length, parent)};
}

} // namespace

namespace ifcapi::detail::alignment {

std::vector<express::Base> map_cant(ifcopenshell::file* file, express::Base alignment_segment, double rail) {
    require_ifc4x3(file);
    require_owned(file, alignment_segment, "segment");
    require_type(alignment_segment, "IfcAlignmentSegment", "segment");
    auto parameters = ifcapi::detail::read_ref_attr(alignment_segment, "DesignParameters");
    require_type(parameters, "IfcAlignmentCantSegment", "DesignParameters");
    const auto type = enum_value(parameters, "PredefinedType");
    if (type == "CONSTANTCANT") return map_constant(file, parameters, rail);
    if (type == "LINEARTRANSITION") return map_linear(file, parameters, rail);
    if (type == "HELMERTCURVE") return map_helmert(file, parameters, rail);
    if (type == "BLOSSCURVE") return map_bloss(file, parameters, rail);
    if (type == "COSINECURVE") return map_cosine(file, parameters, rail);
    if (type == "SINECURVE") return map_sine(file, parameters, rail);
    if (type == "VIENNESEBEND") return map_viennese(file, parameters, rail);
    throw std::runtime_error("Unsupported cant alignment segment type: " + type);
}

std::vector<express::Base> map_segment(ifcopenshell::file* file, express::Base layout, express::Base segment) {
    if (layout.declaration().is("IfcAlignmentHorizontal")) return map_horizontal(file, segment);
    if (layout.declaration().is("IfcAlignmentVertical")) return map_vertical(file, segment);
    if (layout.declaration().is("IfcAlignmentCant")) {
        return map_cant(file, segment, ifcapi::detail::entity_view(layout).get_or<double>("RailHeadDistance", 0.0));
    }
    throw std::runtime_error("Unsupported alignment layout type");
}

} // namespace ifcapi::detail::alignment

namespace ifcapi::bindings {

std::vector<express::Base> alignment_map_segment(
    ifcopenshell::file* file,
    const AlignmentMapSegmentOptions& options)
{
    const auto segment = options.segment;
    const auto rail_head_distance = options.rail_head_distance;
    ifcapi::detail::alignment::require_ifc4x3(file);
    ifcapi::detail::alignment::require_owned(file, segment, "segment");
    ifcapi::detail::alignment::require_type(segment, "IfcAlignmentSegment", "segment");
    auto parameters = ifcapi::detail::read_ref_attr(segment, "DesignParameters");
    if (parameters.declaration().is("IfcAlignmentHorizontalSegment")) {
        return ifcapi::detail::alignment::map_horizontal(file, segment);
    }
    if (parameters.declaration().is("IfcAlignmentVerticalSegment")) {
        return ifcapi::detail::alignment::map_vertical(file, segment);
    }
    if (parameters.declaration().is("IfcAlignmentCantSegment")) {
        if (!rail_head_distance) throw std::runtime_error("rail_head_distance is required for cant mapping");
        return ifcapi::detail::alignment::map_cant(file, segment, *rail_head_distance);
    }
    throw std::runtime_error("Unsupported alignment DesignParameters type");
}

} // namespace ifcapi::bindings
