// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ALIGNMENT_H
#define IFCAPI_DETAIL_ALIGNMENT_H

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace ifcapi::detail::alignment {

using Matrix = std::array<double, 16>;

void require_ifc4x3(ifcopenshell::file* file);
void require_owned(ifcopenshell::file* file, express::Base entity, const char* name);
void require_type(express::Base entity, const char* type, const char* name);
express::Base create(ifcopenshell::file* file, const char* type);
express::Base point(ifcopenshell::file* file, const std::vector<double>& coordinates);
express::Base direction(ifcopenshell::file* file, const std::vector<double>& ratios);
express::Base axis2d(ifcopenshell::file* file, express::Base location, double angle = 0.0);
express::Base axis3d(
    ifcopenshell::file* file,
    express::Base location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction);
express::Base length_measure(ifcopenshell::file* file, double value);
double wrapped_double(express::Base value);
std::string enum_value(express::Base entity, const char* attr);
std::vector<express::Base> inverses(express::Base entity, const char* attr);
std::vector<express::Base> refs(express::Base entity, const char* attr);
express::Base first_inverse_relating(express::Base entity, const char* inverse, const char* relating_attr);
express::Base find_nest_with_related_type(express::Base parent, const char* type);
std::vector<express::Base> representations(express::Base product);

std::vector<express::Base> map_horizontal(ifcopenshell::file* file, express::Base segment);
std::vector<express::Base> map_vertical(ifcopenshell::file* file, express::Base segment);
std::vector<express::Base> map_cant(ifcopenshell::file* file, express::Base segment, double rail_head_distance);
std::vector<express::Base> map_segment(ifcopenshell::file* file, express::Base layout, express::Base segment);

Matrix evaluate_curve_segment(express::Base segment, bool at_end, bool curvature = false);
Matrix endpoint(ifcopenshell::file* file, express::Base segment);
void update_zero_placement(ifcopenshell::file* file, express::Base zero_segment, const Matrix& placement);
void update_transition(express::Base previous, express::Base next);
express::Base append_segment_to_curve(
    ifcopenshell::file* file,
    express::Base layout_segment,
    express::Base curve,
    Matrix* endpoint_out);
void create_geometric_representation(ifcopenshell::file* file, express::Base alignment);

std::string default_referent_label(express::Base previous_segment, express::Base segment);
std::string station_string(ifcopenshell::file* file, double station);

} // namespace ifcapi::detail::alignment

#endif
