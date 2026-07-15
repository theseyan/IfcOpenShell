// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_ALIGNMENT_H
#define IFCAPI_BINDINGS_ALIGNMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi::bindings {

struct AlignmentCreateOptions {
    std::string name;
    bool include_vertical = false;
    bool include_cant = false;
    bool include_geometry = true;
    double start_station = 0.0;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentCreateByPiMethodOptions {
    std::string name;
    std::vector<std::vector<double>> horizontal_points;
    std::vector<double> radii;
    std::vector<std::vector<double>> vertical_points;
    std::vector<double> vertical_lengths;
    double start_station = 0.0;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentCreateFromCsvTextOptions {
    std::string csv_text;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentCreatePolylineOptions {
    std::string name;
    std::vector<express::Base> points;
    double start_station = 0.0;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentCreateOffsetCurveOptions {
    std::string name;
    std::vector<express::Base> offsets;
    double start_station = 0.0;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentCreateLayoutSegmentResult {
    express::Base segment;
    std::vector<double> endpoint;
    bool has_endpoint = false;
};

struct AlignmentAddStationingReferentOptions {
    express::Base alignment;
    double distance_along = 0.0;
    double station = 0.0;
    std::string name;
    express::Base positioned_product;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct AlignmentMapSegmentOptions {
    express::Base segment;
    std::optional<double> rail_head_distance;
};

/** Create an IFC4X3 alignment, its requested layouts, stationing, zero segments, and optional geometry. */
IFCAPI_BINDING express::Base alignment_create(ifcopenshell::file* file, const AlignmentCreateOptions& options);

/** Create an IFC4X3 horizontal and optional vertical alignment from PI records. */
IFCAPI_BINDING express::Base alignment_create_by_pi_method(
    ifcopenshell::file* file,
    const AlignmentCreateByPiMethodOptions& options);

/** Create one or more alignments from the alignment CSV text contract. */
IFCAPI_BINDING express::Base alignment_create_from_csv_text(
    ifcopenshell::file* file,
    const AlignmentCreateFromCsvTextOptions& options);

/** Create an alignment represented by an IfcPolyline. */
IFCAPI_BINDING express::Base alignment_create_as_polyline(
    ifcopenshell::file* file,
    const AlignmentCreatePolylineOptions& options);

/** Create an alignment represented by an IfcOffsetCurveByDistances. */
IFCAPI_BINDING express::Base alignment_create_as_offset_curve(
    ifcopenshell::file* file,
    const AlignmentCreateOffsetCurveOptions& options);

/** Create and append one semantic layout segment, returning its optional 4x4 endpoint matrix. */
IFCAPI_BINDING AlignmentCreateLayoutSegmentResult alignment_create_layout_segment(
    ifcopenshell::file* file,
    express::Base layout,
    express::Base design_parameters);

/** Append an existing IfcAlignmentSegment to its matching semantic layout. */
IFCAPI_BINDING AlignmentCreateLayoutSegmentResult alignment_add_segment_to_layout(
    ifcopenshell::file* file,
    express::Base layout,
    express::Base segment);

/** Map one semantic segment. A Helmert segment returns both mapped halves. */
IFCAPI_BINDING std::vector<express::Base> alignment_map_segment(
    ifcopenshell::file* file,
    const AlignmentMapSegmentOptions& options);

/** Populate the geometric representation of a semantic alignment when absent. */
IFCAPI_BINDING void alignment_create_representation(ifcopenshell::file* file, express::Base alignment);

/** Create per-segment product representations; Helmert 1:2 mappings are rejected. */
IFCAPI_BINDING void alignment_create_segment_representations(ifcopenshell::file* file, express::Base alignment);

/** Add a further vertical layout, moving existing vertical geometry to child alignments when required. */
IFCAPI_BINDING express::Base alignment_add_vertical_layout(ifcopenshell::file* file, express::Base parent_alignment);

/** Add a station referent and keep the referent nest sorted by station. */
IFCAPI_BINDING express::Base alignment_add_stationing_referent(
    ifcopenshell::file* file,
    const AlignmentAddStationingReferentOptions& options);

/** Append the mandatory zero-length segment; returns false when already present or not applicable. */
IFCAPI_BINDING bool alignment_add_zero_length_segment(ifcopenshell::file* file, express::Base layout);
IFCAPI_BINDING bool alignment_has_zero_length_segment(express::Base layout);

IFCAPI_BINDING void alignment_layout_horizontal_by_pi_method(
    ifcopenshell::file* file,
    express::Base layout,
    const std::vector<std::vector<double>>& points,
    const std::vector<double>& radii);
IFCAPI_BINDING void alignment_layout_vertical_by_pi_method(
    ifcopenshell::file* file,
    express::Base layout,
    const std::vector<std::vector<double>>& points,
    const std::vector<double>& lengths);

IFCAPI_BINDING std::optional<express::Base> alignment_get_alignment(express::Base layout);
IFCAPI_BINDING std::optional<express::Base> alignment_get_alignment_layout_nest(express::Base alignment);
IFCAPI_BINDING std::vector<express::Base> alignment_get_alignment_layouts(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_alignment_segment_nest(express::Base layout);
IFCAPI_BINDING double alignment_get_alignment_start_station(ifcopenshell::file* file, express::Base alignment);
IFCAPI_BINDING express::Base alignment_get_axis_subcontext(ifcopenshell::file* file);
IFCAPI_BINDING std::optional<express::Base> alignment_get_basis_curve(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_cant_layout(express::Base alignment);
IFCAPI_BINDING std::vector<express::Base> alignment_get_child_alignments(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_curve(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_curve_segment(express::Base layout, express::Base segment);
IFCAPI_BINDING std::string alignment_get_curve_segment_transition_code(
    express::Base segment,
    express::Base next_segment,
    double position_tolerance = 0.001);
IFCAPI_BINDING void alignment_update_curve_segment_transition_code(
    express::Base segment,
    express::Base next_segment,
    double position_tolerance = 0.001);
IFCAPI_BINDING std::optional<express::Base> alignment_get_horizontal_layout(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_layout(express::Base segment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_layout_curve(express::Base layout);
IFCAPI_BINDING std::vector<express::Base> alignment_get_layout_segments(express::Base layout);
IFCAPI_BINDING std::vector<express::Base> alignment_get_mapped_segments(express::Base layout_segment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_parent_alignment(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_referent_nest(express::Base alignment);
IFCAPI_BINDING std::optional<express::Base> alignment_get_vertical_layout(express::Base alignment);

IFCAPI_BINDING double alignment_distance_along_from_station(
    ifcopenshell::file* file,
    express::Base alignment,
    double station);
IFCAPI_BINDING void alignment_name_segments(const std::string& prefix, express::Base layout);
IFCAPI_BINDING std::string alignment_default_referent_label(
    std::optional<express::Base> previous_segment,
    std::optional<express::Base> segment);
IFCAPI_BINDING std::string alignment_station_as_string(ifcopenshell::file* file, double station);
IFCAPI_BINDING void alignment_update_end_point(ifcopenshell::file* file, express::Base curve);
IFCAPI_BINDING void alignment_update_fallback_position(ifcopenshell::file* file, express::Base linear_placement);

} // namespace ifcapi::bindings

#endif
