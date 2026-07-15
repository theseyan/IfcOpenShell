// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/alignment.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"

#include <stdexcept>

namespace {

using ifcapi::detail::alignment::inverses;
using ifcapi::detail::alignment::refs;

std::optional<express::Base> optional(express::Base value) {
    return value ? std::optional<express::Base>(value) : std::nullopt;
}

bool is_layout(express::Base value) {
    return value && (value.declaration().is("IfcAlignmentHorizontal") ||
                     value.declaration().is("IfcAlignmentVertical") ||
                     value.declaration().is("IfcAlignmentCant"));
}

size_t mapped_count(express::Base segment) {
    auto parameters = ifcapi::detail::read_ref_attr(segment, "DesignParameters");
    if (!parameters) return 0;
    return ifcapi::detail::alignment::enum_value(parameters, "PredefinedType") == "HELMERTCURVE" ? 2 : 1;
}

std::optional<double> station_property(express::Base referent) {
    for (auto rel : inverses(referent, "IsDefinedBy")) {
        if (!rel.declaration().is("IfcRelDefinesByProperties")) continue;
        auto pset = ifcapi::detail::read_ref_attr(rel, "RelatingPropertyDefinition");
        if (!pset || ifcapi::detail::read_string_attr(pset, "Name") != "Pset_Stationing") continue;
        for (auto property : refs(pset, "HasProperties")) {
            if (ifcapi::detail::read_string_attr(property, "Name") != "Station") continue;
            auto value = ifcapi::detail::read_ref_attr(property, "NominalValue");
            if (value) return ifcapi::detail::alignment::wrapped_double(value);
        }
    }
    return std::nullopt;
}

} // namespace

namespace ifcapi::bindings {

std::optional<express::Base> alignment_get_alignment(express::Base layout) {
    for (auto rel : inverses(layout, "Nests")) {
        auto value = ifcapi::detail::read_ref_attr(rel, "RelatingObject");
        if (value && value.declaration().is("IfcAlignment")) return value;
    }
    return std::nullopt;
}

std::optional<express::Base> alignment_get_alignment_layout_nest(express::Base alignment) {
    for (auto relation : inverses(alignment, "IsNestedBy")) {
        for (auto related : refs(relation, "RelatedObjects")) {
            if (is_layout(related)) return relation;
        }
    }
    return std::nullopt;
}

std::vector<express::Base> alignment_get_alignment_layouts(express::Base alignment) {
    std::vector<express::Base> result;
    for (auto rel : inverses(alignment, "IsNestedBy")) {
        for (auto value : refs(rel, "RelatedObjects")) if (is_layout(value)) result.push_back(value);
    }
    return result;
}

std::optional<express::Base> alignment_get_alignment_segment_nest(express::Base layout) {
    return optional(ifcapi::detail::alignment::find_nest_with_related_type(layout, "IfcAlignmentSegment"));
}

std::optional<express::Base> alignment_get_parent_alignment(express::Base alignment) {
    for (auto rel : inverses(alignment, "Decomposes")) {
        auto parent = ifcapi::detail::read_ref_attr(rel, "RelatingObject");
        if (parent && parent.declaration().is("IfcAlignment")) return parent;
    }
    return std::nullopt;
}

std::vector<express::Base> alignment_get_child_alignments(express::Base alignment) {
    std::vector<express::Base> result;
    for (auto rel : inverses(alignment, "IsDecomposedBy")) {
        for (auto value : refs(rel, "RelatedObjects")) {
            if (value.declaration().is("IfcAlignment")) result.push_back(value);
        }
    }
    return result;
}

std::optional<express::Base> alignment_get_referent_nest(express::Base alignment) {
    ifcapi::detail::alignment::require_type(alignment, "IfcAlignment", "alignment");
    return optional(ifcapi::detail::alignment::find_nest_with_related_type(alignment, "IfcReferent"));
}

std::optional<express::Base> alignment_get_horizontal_layout(express::Base alignment) {
    for (auto value : alignment_get_alignment_layouts(alignment)) {
        if (value.declaration().is("IfcAlignmentHorizontal")) return value;
    }
    return std::nullopt;
}

std::optional<express::Base> alignment_get_vertical_layout(express::Base alignment) {
    for (auto value : alignment_get_alignment_layouts(alignment)) {
        if (value.declaration().is("IfcAlignmentVertical")) return value;
    }
    return std::nullopt;
}

std::optional<express::Base> alignment_get_cant_layout(express::Base alignment) {
    for (auto value : alignment_get_alignment_layouts(alignment)) {
        if (value.declaration().is("IfcAlignmentCant")) return value;
    }
    return std::nullopt;
}

std::optional<express::Base> alignment_get_layout(express::Base segment) {
    ifcapi::detail::alignment::require_type(segment, "IfcAlignmentSegment", "segment");
    for (auto rel : inverses(segment, "Nests")) {
        auto layout = ifcapi::detail::read_ref_attr(rel, "RelatingObject");
        if (is_layout(layout)) return layout;
    }
    return std::nullopt;
}

std::vector<express::Base> alignment_get_layout_segments(express::Base layout) {
    std::vector<express::Base> result;
    for (auto rel : inverses(layout, "IsNestedBy")) {
        for (auto value : refs(rel, "RelatedObjects")) {
            if (value.declaration().is("IfcAlignmentSegment")) result.push_back(value);
        }
    }
    return result;
}

std::optional<express::Base> alignment_get_curve(express::Base alignment) {
    for (auto representation : ifcapi::detail::alignment::representations(alignment)) {
        const auto identifier = ifcapi::detail::read_string_attr(representation, "RepresentationIdentifier");
        const auto type = ifcapi::detail::read_string_attr(representation, "RepresentationType");
        if (identifier == "Axis" && (type == "Curve2D" || type == "Curve3D")) {
            auto items = refs(representation, "Items");
            return items.empty() ? std::nullopt : optional(items.front());
        }
    }
    return std::nullopt;
}

std::optional<express::Base> alignment_get_basis_curve(express::Base alignment) {
    for (auto representation : ifcapi::detail::alignment::representations(alignment)) {
        const auto identifier = ifcapi::detail::read_string_attr(representation, "RepresentationIdentifier");
        const auto type = ifcapi::detail::read_string_attr(representation, "RepresentationType");
        if ((identifier == "Axis" && (type == "Curve2D" || type == "Curve3D")) ||
            (identifier == "FootPrint" && type == "Curve2D")) {
            auto items = refs(representation, "Items");
            return items.empty() ? std::nullopt : optional(items.front());
        }
    }
    auto parent = alignment_get_parent_alignment(alignment);
    return parent ? alignment_get_basis_curve(*parent) : std::nullopt;
}

std::optional<express::Base> alignment_get_layout_curve(express::Base layout) {
    auto alignment = alignment_get_alignment(layout);
    if (!alignment) return std::nullopt;
    auto curve = alignment_get_curve(*alignment);
    if (!curve) return std::nullopt;
    auto value = *curve;
    if (layout.declaration().is("IfcAlignmentHorizontal")) {
        if (value.declaration().is("IfcGradientCurve")) value = ifcapi::detail::read_ref_attr(value, "BaseCurve");
        else if (value.declaration().is("IfcSegmentedReferenceCurve")) {
            value = ifcapi::detail::read_ref_attr(ifcapi::detail::read_ref_attr(value, "BaseCurve"), "BaseCurve");
        }
    } else if (layout.declaration().is("IfcAlignmentVertical") &&
               value.declaration().is("IfcSegmentedReferenceCurve")) {
        value = ifcapi::detail::read_ref_attr(value, "BaseCurve");
    }
    return optional(value);
}

std::optional<express::Base> alignment_get_curve_segment(express::Base layout, express::Base segment) {
    size_t index = 0;
    for (auto value : alignment_get_layout_segments(layout)) {
        if (value == segment) break;
        index += mapped_count(value);
    }
    auto curve = alignment_get_layout_curve(layout);
    if (!curve) return std::nullopt;
    auto segments = refs(*curve, "Segments");
    return index < segments.size() ? optional(segments[index]) : std::nullopt;
}

std::vector<express::Base> alignment_get_mapped_segments(express::Base layout_segment) {
    ifcapi::detail::alignment::require_type(layout_segment, "IfcAlignmentSegment", "layout_segment");
    auto layout = alignment_get_layout(layout_segment);
    if (!layout) return {};
    size_t index = 0;
    for (auto value : alignment_get_layout_segments(*layout)) {
        if (value == layout_segment) break;
        index += mapped_count(value);
    }
    auto curve = alignment_get_layout_curve(*layout);
    if (!curve) return {};
    auto curve_segments = refs(*curve, "Segments");
    std::vector<express::Base> result;
    const auto count = mapped_count(layout_segment);
    for (size_t i = 0; i < count && index + i < curve_segments.size(); ++i) result.push_back(curve_segments[index + i]);
    return result;
}

double alignment_get_alignment_start_station(ifcopenshell::file* file, express::Base alignment) {
    ifcapi::detail::alignment::require_owned(file, alignment, "alignment");
    ifcapi::detail::alignment::require_type(alignment, "IfcAlignment", "alignment");
    auto parent = alignment_get_parent_alignment(alignment);
    if (parent) return alignment_get_alignment_start_station(file, *parent);
    for (auto rel : inverses(alignment, "IsNestedBy")) {
        for (auto value : refs(rel, "RelatedObjects")) {
            if (value.declaration().is("IfcReferent")) {
                auto station = station_property(value);
                if (station) return *station;
            }
        }
    }
    return 0.0;
}

double alignment_distance_along_from_station(ifcopenshell::file* file, express::Base alignment, double station) {
    return station - alignment_get_alignment_start_station(file, alignment);
}

void alignment_name_segments(const std::string& prefix, express::Base layout) {
    if (!is_layout(layout)) throw std::runtime_error("layout must be an alignment layout");
    size_t index = 1;
    for (auto segment : alignment_get_layout_segments(layout)) {
        ifcapi::detail::write_string_attr(segment, "Name", prefix + std::to_string(index++));
    }
}

bool alignment_has_zero_length_segment(express::Base layout) {
    if (!layout) throw std::runtime_error("layout is required");
    if (layout.declaration().is("IfcCompositeCurve")) {
        auto segments = refs(layout, "Segments");
        if (segments.empty()) return false;
        return ifcapi::detail::alignment::wrapped_double(
                   ifcapi::detail::read_ref_attr(segments.back(), "SegmentLength")) == 0.0;
    }
    if (!is_layout(layout)) throw std::runtime_error("unsupported alignment layout type");
    auto segments = alignment_get_layout_segments(layout);
    if (segments.empty()) return false;
    auto parameters = ifcapi::detail::read_ref_attr(segments.back(), "DesignParameters");
    const char* attr = parameters.declaration().is("IfcAlignmentHorizontalSegment") ? "SegmentLength" : "HorizontalLength";
    return ifcapi::detail::entity_view(parameters).get_or<double>(attr, 0.0) == 0.0;
}

} // namespace ifcapi::bindings
