// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/alignment.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"
#include "guid.h"

#if IFCOPENSHELL_WITH_IFCGEOM
#include "ifcgeom/ConversionSettings.h"
#include "ifcgeom/abstract_mapping.h"
#include "ifcgeom/function_item_evaluator.h"
#endif

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace {

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

double real(express::Base entity, const char* attr) { return entity_view(entity).get_or<double>(attr, 0.0); }

Matrix vector_matrix(const std::vector<double>& values) {
    Matrix result{};
    if (values.size() != 16) throw std::runtime_error("Expected a 4x4 placement matrix");
    std::copy(values.begin(), values.end(), result.begin());
    return result;
}

express::Base create_zero_parent_line(ifcopenshell::file* file) {
    auto parent = create(file, "IfcLine");
    write_ref_attr(parent, "Pnt", point(file, {0.0, 0.0}));
    auto vector = create(file, "IfcVector");
    write_ref_attr(vector, "Orientation", direction(file, {1.0, 0.0}));
    write_double_attr(vector, "Magnitude", 1.0);
    write_ref_attr(parent, "Dir", vector);
    return parent;
}

express::Base create_zero_curve_segment(ifcopenshell::file* file, bool is_3d) {
    auto result = create(file, "IfcCurveSegment");
    write_enum_attr(result, "Transition", "DISCONTINUOUS");
    write_ref_attr(
        result, "Placement",
        is_3d ? axis3d(file, point(file, {0.0, 0.0, 0.0}), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0})
              : axis2d(file, point(file, {0.0, 0.0})));
    write_ref_attr(result, "SegmentStart", length_measure(file, 0.0));
    write_ref_attr(result, "SegmentLength", length_measure(file, 0.0));
    write_ref_attr(result, "ParentCurve", create_zero_parent_line(file));
    return result;
}

express::Base guid_root(ifcopenshell::file* file, const char* type) {
    auto result = create(file, type);
    ifcapi::detail::write_string_attr(result, "GlobalId", ifcapi::guid_new());
    return result;
}

} // namespace

namespace ifcapi::detail::alignment {

Matrix evaluate_curve_segment(express::Base segment, bool at_end, bool curvature) {
#if IFCOPENSHELL_WITH_IFCGEOM
    if (!segment || !segment.declaration().is("IfcCurveSegment")) {
        throw std::runtime_error("segment must be IfcCurveSegment");
    }
    ifcopenshell::geometry::Settings settings;
    settings.set("compute-curvature", curvature);
    std::unique_ptr<ifcopenshell::geometry::abstract_mapping> mapping(
        ifcopenshell::geometry::impl::mapping_implementations().construct(segment.file(), settings));
    auto item = mapping->map(segment);
    auto function = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::function_item>(item);
    if (!function) throw std::runtime_error("Unable to map IfcCurveSegment for evaluation");
    ifcopenshell::geometry::function_item_evaluator evaluator(settings, function);
    const double parameter = at_end ? evaluator.evaluation_points().back() : evaluator.evaluation_points().front();
    const auto matrix = evaluator.evaluate(parameter);
    Matrix result{};
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) result[row * 4 + col] = matrix(row, col);
    return result;
#else
    (void)segment;
    (void)at_end;
    (void)curvature;
    throw std::runtime_error("Alignment curve evaluation requires IfcGeom");
#endif
}

Matrix endpoint(ifcopenshell::file* file, express::Base segment) {
    require_owned(file, segment, "segment");
    if (segment.declaration().is("IfcCurveSegment")) return evaluate_curve_segment(segment, true);
    require_type(segment, "IfcAlignmentSegment", "segment");
    auto layout_opt = ifcapi::bindings::alignment_get_layout(segment);
    if (!layout_opt) throw std::runtime_error("Alignment segment is not nested in a layout");
    auto layout = *layout_opt;
    auto mapped = map_segment(file, layout, segment);
    if (mapped.empty()) throw std::runtime_error("Alignment segment did not map to geometry");
    express::Base temporary;
    if (layout.declaration().is("IfcAlignmentVertical")) {
        temporary = create(file, "IfcGradientCurve");
        write_ref_aggregate(temporary, "Segments", mapped);
    } else if (layout.declaration().is("IfcAlignmentCant")) {
        temporary = create(file, "IfcSegmentedReferenceCurve");
        write_ref_aggregate(temporary, "Segments", mapped);
        auto p = read_ref_attr(segment, "DesignParameters");
        const double left = entity_view(p).get_optional<double>("EndCantLeft").value_or(real(p, "StartCantLeft"));
        const double right = entity_view(p).get_optional<double>("EndCantRight").value_or(real(p, "StartCantRight"));
        const double cant = right - left;
        const double rail = real(layout, "RailHeadDistance");
        const double ay = rail == 0.0 ? 0.0 : cant / rail;
        const double az = rail == 0.0 ? 1.0 : std::sqrt(rail * rail - cant * cant) / rail;
        auto end = axis3d(
            file, point(file, {real(p, "StartDistAlong"), 0.5 * cant, 0.0}),
            {0.0, ay, az}, {1.0, 0.0, 0.0});
        write_ref_attr(temporary, "EndPoint", end);
    }
    Matrix result = evaluate_curve_segment(mapped.back(), true);
    if (temporary) file->remove_entity(temporary);
    for (auto mapped_segment : mapped) {
        auto value = mapped_segment;
        ifcapi::bindings::entity_remove_deep(&value);
    }
    return result;
}

void update_zero_placement(ifcopenshell::file* file, express::Base zero, const Matrix& m) {
    const double scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    const double x = m[3] / scale, y = m[7] / scale, z = m[11] / scale;
    const double rdx = m[0], rdy = m[4], rdz = m[8];
    const double adx = m[2], ady = m[6], adz = m[10];
    if (zero.declaration().is("IfcCurveSegment")) {
        auto placement = read_ref_attr(zero, "Placement");
        auto location = read_ref_attr(placement, "Location");
        write_double_aggregate(location, "Coordinates", placement.declaration().is("IfcAxis2Placement2D")
            ? std::vector<double>{x, y} : std::vector<double>{x, y, z});
        write_double_aggregate(read_ref_attr(placement, "RefDirection"), "DirectionRatios",
            placement.declaration().is("IfcAxis2Placement2D") ? std::vector<double>{rdx, rdy} : std::vector<double>{rdx, rdy, rdz});
        if (placement.declaration().is("IfcAxis2Placement3D")) {
            write_double_aggregate(read_ref_attr(placement, "Axis"), "DirectionRatios", {adx, ady, adz});
        }
        return;
    }
    auto p = read_ref_attr(zero, "DesignParameters");
    if (p.declaration().is("IfcAlignmentHorizontalSegment")) {
        write_double_aggregate(read_ref_attr(p, "StartPoint"), "Coordinates", {x, y});
        write_double_attr(p, "StartDirection", std::atan2(rdy, rdx) /
            ifcapi::bindings::unit_calculate_unit_scale(file, "PLANEANGLEUNIT"));
    } else if (p.declaration().is("IfcAlignmentVerticalSegment")) {
        write_double_attr(p, "StartDistAlong", x);
        write_double_attr(p, "StartHeight", y);
        write_double_attr(p, "StartGradient", rdy / rdx);
        write_double_attr(p, "EndGradient", rdy / rdx);
    } else if (p.declaration().is("IfcAlignmentCantSegment")) {
        const double slope = ady / std::sqrt(ady * ady + adz * adz);
        auto layout = ifcapi::bindings::alignment_get_layout(zero);
        const double rail = layout ? real(*layout, "RailHeadDistance") : 0.0;
        write_double_attr(p, "StartDistAlong", x);
        write_double_attr(p, "StartCantLeft", y - slope * rail / 2.0);
        write_double_attr(p, "StartCantRight", y + slope * rail / 2.0);
        write_double_attr(p, "EndCantLeft", y - slope * rail / 2.0);
        write_double_attr(p, "EndCantRight", y + slope * rail / 2.0);
    }
}

void update_transition(express::Base previous, express::Base next) {
    write_enum_attr(previous, "Transition", ifcapi::bindings::alignment_get_curve_segment_transition_code(previous, next, 0.001));
}

express::Base append_segment_to_curve(
    ifcopenshell::file* file,
    express::Base layout_segment,
    express::Base curve,
    Matrix* endpoint_out)
{
    auto layout = ifcapi::bindings::alignment_get_layout(layout_segment);
    if (!layout) throw std::runtime_error("layout segment is not nested");
    auto mapped = map_segment(file, *layout, layout_segment);
    auto segments = refs(curve, "Segments");
    express::Base zero;
    if (ifcapi::bindings::alignment_has_zero_length_segment(curve) && !segments.empty()) {
        zero = segments.back();
        segments.pop_back();
    }
    express::Base previous = segments.empty() ? express::Base() : segments.back();
    for (auto value : mapped) {
        if (!inverses(value, "UsingCurves").empty()) throw std::runtime_error("IfcCurveSegment already belongs to a curve");
        segments.push_back(value);
    }
    if (zero) segments.push_back(zero);
    write_ref_aggregate(curve, "Segments", segments);
    if (previous) update_transition(previous, mapped.front());
    for (size_t i = 1; i < mapped.size(); ++i) update_transition(mapped[i - 1], mapped[i]);
    Matrix end = evaluate_curve_segment(mapped.back(), true);
    if (endpoint_out) *endpoint_out = end;
    if (zero) {
        update_zero_placement(file, zero, end);
        update_transition(mapped.back(), zero);
    } else {
        write_enum_attr(mapped.back(), "Transition", "DISCONTINUOUS");
    }
    return mapped.front();
}

} // namespace ifcapi::detail::alignment

namespace ifcapi::bindings {

std::string alignment_get_curve_segment_transition_code(
    express::Base segment,
    express::Base next_segment,
    double position_tolerance)
{
    ifcapi::detail::alignment::require_type(segment, "IfcCurveSegment", "segment");
    ifcapi::detail::alignment::require_type(next_segment, "IfcCurveSegment", "next_segment");
    auto first_curves = ifcapi::detail::alignment::inverses(segment, "UsingCurves");
    auto next_curves = ifcapi::detail::alignment::inverses(next_segment, "UsingCurves");
    if (first_curves.size() != 1 || next_curves.size() != 1 || first_curves.front() != next_curves.front()) {
        throw std::runtime_error("Both segments must belong to exactly the same one curve");
    }
    const auto end = ifcapi::detail::alignment::evaluate_curve_segment(segment, true, true);
    const auto start = ifcapi::detail::alignment::evaluate_curve_segment(next_segment, false, true);
    auto close = [](double a, double b, double tolerance) {
        return std::abs(a - b) <= tolerance + 1e-5 * std::abs(b);
    };
    const bool same_position = close(end[3], start[3], position_tolerance) &&
                               close(end[7], start[7], position_tolerance) &&
                               close(end[11], start[11], position_tolerance);
    if (!same_position) return "DISCONTINUOUS";
    const bool same_gradient = close(end[0], start[0], 1e-8) && close(end[4], start[4], 1e-8) && close(end[8], start[8], 1e-8);
    if (!same_gradient) return "CONTINUOUS";
    const bool same_curvature = close(end[12], start[12], 1e-8) && close(end[13], start[13], 1e-8) && close(end[14], start[14], 1e-8);
    return same_curvature ? "CONTSAMEGRADIENTSAMECURVATURE" : "CONTSAMEGRADIENT";
}

void alignment_update_curve_segment_transition_code(
    express::Base segment,
    express::Base next_segment,
    double position_tolerance)
{
    ifcapi::detail::write_enum_attr(
        segment, "Transition",
        alignment_get_curve_segment_transition_code(segment, next_segment, position_tolerance));
}

bool alignment_add_zero_length_segment(ifcopenshell::file* file, express::Base layout) {
    ifcapi::detail::alignment::require_ifc4x3(file);
    ifcapi::detail::alignment::require_owned(file, layout, "layout");
    if (layout.declaration().is("IfcOffsetCurveByDistances") || layout.declaration().is("IfcPolyline") ||
        layout.declaration().is("IfcIndexedPolyCurve")) return false;
    if (alignment_has_zero_length_segment(layout)) return false;
    if (layout.declaration().is("IfcCompositeCurve")) {
        auto zero = create_zero_curve_segment(file, layout.declaration().is("IfcSegmentedReferenceCurve"));
        auto segments = ifcapi::detail::alignment::refs(layout, "Segments");
        if (!segments.empty()) {
            auto end = ifcapi::detail::alignment::evaluate_curve_segment(segments.back(), true);
            ifcapi::detail::alignment::update_zero_placement(file, zero, end);
            segments.push_back(zero);
            ifcapi::detail::write_ref_aggregate(layout, "Segments", segments);
            ifcapi::detail::alignment::update_transition(segments[segments.size() - 2], zero);
        } else {
            ifcapi::detail::write_ref_aggregate(layout, "Segments", {zero});
        }
        if (layout.declaration().is("IfcSegmentedReferenceCurve") || layout.declaration().is("IfcGradientCurve")) {
            auto base = ifcapi::detail::read_ref_attr(layout, "BaseCurve");
            if (base) alignment_add_zero_length_segment(file, base);
        }
        return true;
    }
    const auto segments = alignment_get_layout_segments(layout);
    express::Base zero;
    if (layout.declaration().is("IfcAlignmentHorizontal")) {
        double x = 0.0, y = 0.0, angle = 0.0;
        if (!segments.empty()) {
            auto end = ifcapi::detail::alignment::endpoint(file, segments.back());
            const double scale = unit_calculate_unit_scale(file, "LENGTHUNIT");
            x = end[3] / scale; y = end[7] / scale;
            angle = std::atan2(end[4], end[0]) / unit_calculate_unit_scale(file, "PLANEANGLEUNIT");
        }
        auto p = ifcapi::detail::alignment::create(file, "IfcAlignmentHorizontalSegment");
        ifcapi::detail::write_ref_attr(p, "StartPoint", ifcapi::detail::alignment::point(file, {x, y}));
        ifcapi::detail::write_double_attr(p, "StartDirection", angle);
        ifcapi::detail::write_double_attr(p, "StartRadiusOfCurvature", 0.0);
        ifcapi::detail::write_double_attr(p, "EndRadiusOfCurvature", 0.0);
        ifcapi::detail::write_double_attr(p, "SegmentLength", 0.0);
        ifcapi::detail::write_enum_attr(p, "PredefinedType", "LINE");
        zero = guid_root(file, "IfcAlignmentSegment");
        ifcapi::detail::write_ref_attr(zero, "DesignParameters", p);
    } else if (layout.declaration().is("IfcAlignmentVertical")) {
        double distance = 0.0, height = 0.0, gradient = 0.0;
        if (!segments.empty()) {
            auto p0 = ifcapi::detail::read_ref_attr(segments.back(), "DesignParameters");
            distance = real(p0, "StartDistAlong") + real(p0, "HorizontalLength");
            gradient = real(p0, "EndGradient");
            auto end = ifcapi::detail::alignment::endpoint(file, segments.back());
            height = end[7] / unit_calculate_unit_scale(file, "LENGTHUNIT");
        }
        auto p = ifcapi::detail::alignment::create(file, "IfcAlignmentVerticalSegment");
        ifcapi::detail::write_double_attr(p, "StartDistAlong", distance);
        ifcapi::detail::write_double_attr(p, "HorizontalLength", 0.0);
        ifcapi::detail::write_double_attr(p, "StartHeight", height);
        ifcapi::detail::write_double_attr(p, "StartGradient", gradient);
        ifcapi::detail::write_double_attr(p, "EndGradient", gradient);
        ifcapi::detail::write_enum_attr(p, "PredefinedType", "CONSTANTGRADIENT");
        zero = guid_root(file, "IfcAlignmentSegment");
        ifcapi::detail::write_ref_attr(zero, "DesignParameters", p);
    } else if (layout.declaration().is("IfcAlignmentCant")) {
        double distance = 0.0, left = 0.0, right = 0.0;
        if (!segments.empty()) {
            auto p0 = ifcapi::detail::read_ref_attr(segments.back(), "DesignParameters");
            distance = real(p0, "StartDistAlong") + real(p0, "HorizontalLength");
            left = ifcapi::detail::entity_view(p0).get_optional<double>("EndCantLeft").value_or(real(p0, "StartCantLeft"));
            right = ifcapi::detail::entity_view(p0).get_optional<double>("EndCantRight").value_or(real(p0, "StartCantRight"));
        }
        auto p = ifcapi::detail::alignment::create(file, "IfcAlignmentCantSegment");
        ifcapi::detail::write_double_attr(p, "StartDistAlong", distance);
        ifcapi::detail::write_double_attr(p, "HorizontalLength", 0.0);
        ifcapi::detail::write_double_attr(p, "StartCantLeft", left);
        ifcapi::detail::write_double_attr(p, "StartCantRight", right);
        ifcapi::detail::write_enum_attr(p, "PredefinedType", "CONSTANTCANT");
        zero = guid_root(file, "IfcAlignmentSegment");
        ifcapi::detail::write_ref_attr(zero, "DesignParameters", p);
    } else {
        throw std::runtime_error("Unsupported alignment zero-length target");
    }
    NestAssignObjectOptions options;
    options.products = {zero};
    options.relating_object = layout;
    nest_assign_object(file, options);
    return true;
}

void alignment_update_end_point(ifcopenshell::file* file, express::Base curve) {
    ifcapi::detail::alignment::require_owned(file, curve, "curve");
    if (!curve.declaration().is("IfcGradientCurve") && !curve.declaration().is("IfcSegmentedReferenceCurve")) {
        throw std::runtime_error("curve must be IfcGradientCurve or IfcSegmentedReferenceCurve");
    }
    if (!alignment_has_zero_length_segment(curve)) alignment_add_zero_length_segment(file, curve);
    auto segments = ifcapi::detail::alignment::refs(curve, "Segments");
    auto placement = ifcapi::detail::read_ref_attr(segments.back(), "Placement");
    auto endpoint = ifcapi::detail::read_ref_attr(curve, "EndPoint");
    if (!endpoint) {
        endpoint = curve.declaration().is("IfcGradientCurve")
            ? ifcapi::detail::alignment::axis2d(file, ifcapi::detail::alignment::point(file, {0.0, 0.0}))
            : ifcapi::detail::alignment::axis3d(file, ifcapi::detail::alignment::point(file, {0.0, 0.0, 0.0}), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
        ifcapi::detail::write_ref_attr(curve, "EndPoint", endpoint);
    }
    const auto matrix = vector_matrix(placement_get_axis2_placement(&placement));
    auto location = ifcapi::detail::read_ref_attr(endpoint, "Location");
    ifcapi::detail::write_double_aggregate(location, "Coordinates", curve.declaration().is("IfcGradientCurve")
        ? std::vector<double>{matrix[3], matrix[7]} : std::vector<double>{matrix[3], matrix[7], matrix[11]});
    auto ref = ifcapi::detail::read_ref_attr(endpoint, "RefDirection");
    if (!ref) {
        ref = ifcapi::detail::alignment::direction(file, curve.declaration().is("IfcGradientCurve")
            ? std::vector<double>{1.0, 0.0} : std::vector<double>{1.0, 0.0, 0.0});
        ifcapi::detail::write_ref_attr(endpoint, "RefDirection", ref);
    }
    ifcapi::detail::write_double_aggregate(ref, "DirectionRatios", curve.declaration().is("IfcGradientCurve")
        ? std::vector<double>{matrix[0], matrix[4]} : std::vector<double>{matrix[0], matrix[4], matrix[8]});
    if (curve.declaration().is("IfcSegmentedReferenceCurve")) {
        auto axis = ifcapi::detail::read_ref_attr(endpoint, "Axis");
        if (!axis) { axis = ifcapi::detail::alignment::direction(file, {0.0, 0.0, 1.0}); ifcapi::detail::write_ref_attr(endpoint, "Axis", axis); }
        ifcapi::detail::write_double_aggregate(axis, "DirectionRatios", {matrix[2], matrix[6], matrix[10]});
    }
}

} // namespace ifcapi::bindings
