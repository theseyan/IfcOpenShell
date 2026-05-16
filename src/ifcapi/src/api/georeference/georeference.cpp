// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/georeference.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcSchema.h"

#include <cmath>
#include <string>
#include <vector>

namespace {

constexpr double PI = 3.141592653589793238462643383279502884;

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

IfcUtil::IfcBaseClass* create_cartesian_point(IfcParse::IfcFile* file, const std::vector<double>& coordinates) {
    auto* point = create_entity(file, "IfcCartesianPoint");
    ifcapi::detail::write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

IfcUtil::IfcBaseClass* create_direction(IfcParse::IfcFile* file, const std::vector<double>& ratios) {
    auto* direction = create_entity(file, "IfcDirection");
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

IfcUtil::IfcBaseClass* create_axis2_placement_3d(
    IfcParse::IfcFile* file,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto* placement = create_entity(file, "IfcAxis2Placement3D");
    ifcapi::detail::write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    ifcapi::detail::write_ref_attr(placement, "Axis", create_direction(file, axis));
    ifcapi::detail::write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

IfcUtil::IfcBaseClass* create_axis2_placement_2d(
    IfcParse::IfcFile* file,
    const std::vector<double>& location,
    const std::vector<double>& ref_direction)
{
    auto* placement = create_entity(file, "IfcAxis2Placement2D");
    ifcapi::detail::write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    ifcapi::detail::write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || entity->id() <= 0) {
        return 0;
    }
    try {
        auto inverses = file->getInverse(entity->id(), nullptr, -1);
        return inverses ? inverses->size() : 0;
    } catch (...) {
        return 0;
    }
}

bool is_close(double a, double b) {
    return std::abs(a - b) <= 1e-08 + 1e-05 * std::abs(b);
}

bool all_close(double a, double b, double c, double x, double y, double z) {
    return is_close(a, x) && is_close(b, y) && is_close(c, z);
}

std::vector<IfcUtil::IfcBaseClass*> geometric_contexts(IfcParse::IfcFile* file) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !file->schema()) {
        return result;
    }
    auto* declaration = file->schema()->declaration_by_name("IfcGeometricRepresentationContext");
    auto instances = file->instances_by_type(declaration);
    if (!instances) {
        return result;
    }
    for (auto* instance : *instances) {
        if (instance && instance->declaration().name() == "IfcGeometricRepresentationContext") {
            result.push_back(instance);
        }
    }
    return result;
}

} // namespace

namespace ifcapi {
namespace bindings {

void georeference_edit_true_north(IfcParse::IfcFile* file, bool has_true_north, double x, double y) {
    ifcopenshell_clear_error();
    try {
        for (auto* context : geometric_contexts(file)) {
            auto* true_north = ifcapi::detail::read_ref_attr(context, "TrueNorth");
            if (true_north && !has_true_north) {
                ifcapi::detail::write_ref_attr(context, "TrueNorth", nullptr);
                if (total_inverses(file, true_north) == 0) {
                    ifcapi::bindings::entity_remove_deep2(true_north);
                }
                continue;
            }

            if (true_north) {
                if (total_inverses(file, true_north) != 1) {
                    true_north = create_direction(file, {});
                    ifcapi::detail::write_ref_attr(context, "TrueNorth", true_north);
                }
            } else {
                true_north = create_direction(file, {});
                ifcapi::detail::write_ref_attr(context, "TrueNorth", true_north);
            }
            ifcapi::detail::write_double_aggregate(true_north, "DirectionRatios", {x, y});
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void georeference_edit_wcs(IfcParse::IfcFile* file, double x, double y, double z, double rotation, bool is_si) {
    ifcopenshell_clear_error();
    try {
        double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        if (unit_scale == 0.0) {
            unit_scale = 1.0;
        }
        double xaxis_x = 1.0;
        double xaxis_y = 0.0;
        if (!is_close(rotation, 0.0)) {
            xaxis_x = std::cos(rotation * PI / 180.0);
            xaxis_y = std::sin(rotation * PI / 180.0);
        }
        if (all_close(x, y, z, 0.0, 0.0, 0.0)) {
            x = y = z = 0.0;
        }

        for (auto* context : geometric_contexts(file)) {
            auto* old_wcs = ifcapi::detail::read_ref_attr(context, "WorldCoordinateSystem");
            auto dimension = static_cast<int>(context->get_attribute_value(
                static_cast<size_t>(ifcapi::detail::attr_index_of(context, "CoordinateSpaceDimension"))));
            IfcUtil::IfcBaseClass* placement = nullptr;
            if (dimension == 3) {
                const std::vector<double> xyz = is_si ? std::vector<double>{x / unit_scale, y / unit_scale, z / unit_scale}
                                                      : std::vector<double>{x, y, z};
                placement = create_axis2_placement_3d(file, xyz, {0.0, 0.0, 1.0}, {xaxis_x, xaxis_y, 0.0});
            } else if (dimension == 2) {
                const std::vector<double> xy =
                    is_si ? std::vector<double>{x / unit_scale, y / unit_scale} : std::vector<double>{x, y};
                placement = create_axis2_placement_2d(file, xy, {xaxis_x, xaxis_y});
            }
            if (placement) {
                ifcapi::detail::write_ref_attr(context, "WorldCoordinateSystem", placement);
                if (total_inverses(file, old_wcs) == 0) {
                    ifcapi::bindings::entity_remove_deep2(old_wcs);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
