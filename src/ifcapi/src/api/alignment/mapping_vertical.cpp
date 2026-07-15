// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"

#include <cmath>
#include <stdexcept>

namespace {

constexpr double pi = 3.14159265358979323846;

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

double real(express::Base entity, const char* attr) { return entity_view(entity).get_or<double>(attr, 0.0); }

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

express::Base curve_segment(
    ifcopenshell::file* file,
    express::Base parameters,
    double segment_start,
    double segment_length,
    express::Base parent)
{
    const double gradient = real(parameters, "StartGradient");
    const double angle = std::atan(gradient);
    auto result = create(file, "IfcCurveSegment");
    write_enum_attr(result, "Transition", "DISCONTINUOUS");
    write_ref_attr(
        result, "Placement",
        axis2d(file, point(file, {real(parameters, "StartDistAlong"), real(parameters, "StartHeight")}), angle));
    write_ref_attr(result, "SegmentStart", length_measure(file, segment_start));
    write_ref_attr(result, "SegmentLength", length_measure(file, segment_length));
    write_ref_attr(result, "ParentCurve", parent);
    return result;
}

double polynomial_length(double B, double C, double length) {
    const double a = 4.0 * C * C;
    const double b = 4.0 * B * C;
    const double c = B * B + 1.0;
    auto primitive = [=](double x) {
        const double root = std::sqrt(a * x * x + b * x + c);
        return (b + 2.0 * a * x) / (4.0 * a) * root +
               (4.0 * a * c - b * b) / (8.0 * std::pow(a, 1.5)) *
                   std::log(std::abs(2.0 * a * x + b + 2.0 * std::sqrt(a * (a * x * x + b * x + c))));
    };
    return primitive(length) - primitive(0.0);
}

express::Base map_constant(ifcopenshell::file* file, express::Base p) {
    const double gradient = real(p, "StartGradient");
    const double length = real(p, "HorizontalLength") / std::cos(std::atan(gradient));
    return curve_segment(file, p, 0.0, length, line(file));
}

express::Base map_parabola(ifcopenshell::file* file, express::Base p) {
    const double length = real(p, "HorizontalLength");
    const double start_height = real(p, "StartHeight");
    const double start_gradient = real(p, "StartGradient");
    const double C = (real(p, "EndGradient") - start_gradient) / (2.0 * length);
    auto parent = create(file, "IfcPolynomialCurve");
    write_ref_attr(parent, "Position", origin_placement(file));
    write_double_aggregate(parent, "CoefficientsX", {0.0, 1.0});
    write_double_aggregate(parent, "CoefficientsY", {start_height, start_gradient, C});
    return curve_segment(file, p, 0.0, polynomial_length(start_gradient, C, length), parent);
}

express::Base map_circle(ifcopenshell::file* file, express::Base p) {
    const double horizontal_length = real(p, "HorizontalLength");
    double start_angle = std::atan(real(p, "StartGradient"));
    double end_angle = std::atan(real(p, "EndGradient"));
    double radius, x, y;
    if (start_angle < end_angle) {
        radius = horizontal_length / (std::sin(end_angle) - std::sin(start_angle));
        x = -radius * std::sin(start_angle);
        y = radius * std::cos(start_angle);
        start_angle += 1.5 * pi;
        end_angle += 1.5 * pi;
    } else {
        radius = horizontal_length / (std::sin(start_angle) - std::sin(end_angle));
        x = radius * std::sin(start_angle);
        y = -radius * std::cos(start_angle);
        start_angle += 0.5 * pi;
        end_angle += 0.5 * pi;
    }
    auto parent = create(file, "IfcCircle");
    write_ref_attr(parent, "Position", axis2d(file, point(file, {x, y})));
    write_double_attr(parent, "Radius", radius);
    return curve_segment(file, p, radius * start_angle, radius * (end_angle - start_angle), parent);
}

} // namespace

namespace ifcapi::detail::alignment {

std::vector<express::Base> map_vertical(ifcopenshell::file* file, express::Base segment) {
    require_ifc4x3(file);
    require_owned(file, segment, "segment");
    require_type(segment, "IfcAlignmentSegment", "segment");
    auto parameters = ifcapi::detail::read_ref_attr(segment, "DesignParameters");
    require_type(parameters, "IfcAlignmentVerticalSegment", "DesignParameters");
    const auto type = enum_value(parameters, "PredefinedType");
    if (type == "CONSTANTGRADIENT") return {map_constant(file, parameters)};
    if (type == "PARABOLICARC") return {map_parabola(file, parameters)};
    if (type == "CIRCULARARC") return {map_circle(file, parameters)};
    if (type == "CLOTHOID") throw std::runtime_error("mapping for IfcAlignmentVerticalSegment.CLOTHOID not implemented");
    throw std::runtime_error("Unsupported vertical alignment segment type: " + type);
}

} // namespace ifcapi::detail::alignment
