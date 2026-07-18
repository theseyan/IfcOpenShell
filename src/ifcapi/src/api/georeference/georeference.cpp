// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/georeference.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/pset.h"
#include "ifcopenshell_api_internal.hpp"
#include "../pset/attribute_props.hpp"

#include "ifcparse/schema.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr double PI = 3.141592653589793238462643383279502884;

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

express::Base create_cartesian_point(ifcopenshell::file* file, const std::vector<double>& coordinates) {
    auto point = create_entity(file, "IfcCartesianPoint");
    ifcapi::detail::write_double_aggregate(point, "Coordinates", coordinates);
    return point;
}

express::Base create_direction(ifcopenshell::file* file, const std::vector<double>& ratios) {
    auto direction = create_entity(file, "IfcDirection");
    ifcapi::detail::write_double_aggregate(direction, "DirectionRatios", ratios);
    return direction;
}

express::Base create_axis2_placement_3d(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto placement = create_entity(file, "IfcAxis2Placement3D");
    ifcapi::detail::write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    ifcapi::detail::write_ref_attr(placement, "Axis", create_direction(file, axis));
    ifcapi::detail::write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

express::Base create_axis2_placement_2d(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& ref_direction)
{
    auto placement = create_entity(file, "IfcAxis2Placement2D");
    ifcapi::detail::write_ref_attr(placement, "Location", create_cartesian_point(file, location));
    ifcapi::detail::write_ref_attr(placement, "RefDirection", create_direction(file, ref_direction));
    return placement;
}

bool is_close(double a, double b) {
    return std::abs(a - b) <= 1e-08 + 1e-05 * std::abs(b);
}

bool all_close(double a, double b, double c, double x, double y, double z) {
    return is_close(a, x) && is_close(b, y) && is_close(c, z);
}

std::vector<express::Base> geometric_contexts(ifcopenshell::file* file) {
    std::vector<express::Base> result;
    if (!file || !file->schema()) {
        return result;
    }
    auto* declaration = file->schema()->declaration_by_name("IfcGeometricRepresentationContext");
    auto instances = file->instances_by_type(declaration);
    for (auto instance : instances) {
        if (instance && instance.declaration().name() == "IfcGeometricRepresentationContext") {
            result.push_back(instance);
        }
    }
    return result;
}

void add_ifc2x3_georeferencing(
    ifcopenshell::file* file,
    const std::string& name,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto project = ifcapi::detail::first_instance_by_type(file, "IfcProject");
    if (!project || ifcapi::detail::named_property_set(project, "ePSet_ProjectedCRS")) {
        return;
    }

    auto conversion = ifcapi::bindings::pset_add_pset(
        file,
        ifcapi::bindings::PsetAddPsetOptions{
            project, "ePSet_MapConversion", owner_history, user, application, {}});
    auto crs = ifcapi::bindings::pset_add_pset(
        file,
        ifcapi::bindings::PsetAddPsetOptions{
            project, "ePSet_ProjectedCRS", owner_history, user, application, {}});

    std::unique_ptr<ifcopenshell_pset_props_t, void (*)(ifcopenshell_pset_props_t*)> crs_props(
        ifcapi::bindings::pset_props_new(), ifcapi::bindings::pset_props_free);
    ifcapi::bindings::pset_props_set_typed_string(crs_props.get(), "Name", name, "IfcLabel");
    if (!ifcapi::bindings::pset_edit_pset(file, ifcapi::bindings::PsetEditPsetOptions{crs, {}, crs_props.get(), {}, true})) {
        return;
    }

    std::unique_ptr<ifcopenshell_pset_props_t, void (*)(ifcopenshell_pset_props_t*)> conversion_props(
        ifcapi::bindings::pset_props_new(), ifcapi::bindings::pset_props_free);
    ifcapi::bindings::pset_props_set_typed_double(conversion_props.get(), "Eastings", 0.0, "IfcLengthMeasure");
    ifcapi::bindings::pset_props_set_typed_double(conversion_props.get(), "Northings", 0.0, "IfcLengthMeasure");
    ifcapi::bindings::pset_props_set_typed_double(conversion_props.get(), "OrthogonalHeight", 0.0, "IfcLengthMeasure");
    ifcapi::bindings::pset_edit_pset(
        file, ifcapi::bindings::PsetEditPsetOptions{conversion, {}, conversion_props.get(), {}, true});
}

void remove_ifc2x3_georeferencing(ifcopenshell::file* file) {
    auto project = ifcapi::detail::first_instance_by_type(file, "IfcProject");
    if (!project) {
        ifcapi::detail::set_error(ifcapi::detail::ERROR_RUNTIME, "No IfcProject found");
        return;
    }
    if (auto pset = ifcapi::detail::named_property_set(project, "ePSet_ProjectedCRS")) {
        ifcapi::bindings::pset_remove_pset(file, &project, &pset);
    }
    if (auto pset = ifcapi::detail::named_property_set(project, "ePSet_MapConversion")) {
        ifcapi::bindings::pset_remove_pset(file, &project, &pset);
    }
}

void edit_ifc2x3_georeferencing(
    ifcopenshell::file* file,
    const ifcapi::bindings::GeoreferenceEditGeoreferencingOptions& options)
{
    auto project = ifcapi::detail::first_instance_by_type(file, "IfcProject");
    if (!project) {
        return;
    }
    if (options.projected_crs) {
        if (auto crs = ifcapi::detail::named_property_set(project, "ePSet_ProjectedCRS")) {
            ifcapi::bindings::pset_edit_pset(file, ifcapi::bindings::PsetEditPsetOptions{crs, {}, *options.projected_crs, {}, true});
        }
    }
    if (options.coordinate_operation) {
        if (auto conversion = ifcapi::detail::named_property_set(project, "ePSet_MapConversion")) {
            ifcapi::bindings::pset_edit_pset(
                file, ifcapi::bindings::PsetEditPsetOptions{conversion, {}, *options.coordinate_operation, {}, true});
        }
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

void georeference_add_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceAddGeoreferencingOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto owner_history_value = options.owner_history.value_or(express::Base());
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        if (is_ifc2x3(file)) {
            add_ifc2x3_georeferencing(file, options.name, owner_history_value, user_value, application_value);
            return;
        }

        const bool has_crs = !ifcapi::detail::instances_by_type(file, "IfcProjectedCRS").empty();
        const bool has_conversion = !ifcapi::detail::instances_by_type(file, "IfcCoordinateOperation").empty();
        if (has_crs && has_conversion) {
            return;
        }
        if (has_crs || has_conversion) {
            georeference_remove_georeferencing(file);
        }

        express::Base source_crs = {};
        for (auto context : geometric_contexts(file)) {
            if (ifcapi::detail::read_string_attr(context, "ContextType") == "Model") {
                source_crs = context;
                break;
            }
        }
        if (!source_crs) {
            return;
        }

        auto projected_crs = create_entity(file, "IfcProjectedCRS");
        ifcapi::detail::write_string_attr(projected_crs, "Name", options.name);

        if (options.ifc_class == "IfcMapConversion" || options.ifc_class == "IfcMapConversionScaled") {
            auto conversion = create_entity(file, options.ifc_class.c_str());
            ifcapi::detail::write_ref_attr(conversion, "SourceCRS", source_crs);
            ifcapi::detail::write_ref_attr(conversion, "TargetCRS", projected_crs);
            ifcapi::detail::write_double_attr(conversion, "Eastings", 0.0);
            ifcapi::detail::write_double_attr(conversion, "Northings", 0.0);
            ifcapi::detail::write_double_attr(conversion, "OrthogonalHeight", 0.0);
            if (options.ifc_class == "IfcMapConversionScaled") {
                ifcapi::detail::write_double_attr(conversion, "FactorX", 1.0);
                ifcapi::detail::write_double_attr(conversion, "FactorY", 1.0);
                ifcapi::detail::write_double_attr(conversion, "FactorZ", 1.0);
            }
        } else if (options.ifc_class == "IfcRigidOperation") {
            auto conversion = create_entity(file, options.ifc_class.c_str());
            ifcapi::detail::write_ref_attr(conversion, "SourceCRS", source_crs);
            ifcapi::detail::write_ref_attr(conversion, "TargetCRS", projected_crs);
            int first_idx = ifcapi::detail::attr_index_of(conversion, "FirstCoordinate");
            int second_idx = ifcapi::detail::attr_index_of(conversion, "SecondCoordinate");
            auto first_coordinate = ifcapi::detail::create_typed_double(file, "IfcLengthMeasure", 0.0);
            auto second_coordinate = ifcapi::detail::create_typed_double(file, "IfcLengthMeasure", 0.0);
            if (!first_coordinate || !second_coordinate) {
                throw std::runtime_error("Failed to create IfcLengthMeasure for IfcRigidOperation coordinates");
            }
            if (first_idx >= 0) {
                conversion.set_attribute_value(static_cast<size_t>(first_idx), first_coordinate);
            }
            if (second_idx >= 0) {
                conversion.set_attribute_value(static_cast<size_t>(second_idx), second_coordinate);
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void georeference_edit_true_north(
    ifcopenshell::file* file,
    const GeoreferenceEditTrueNorthOptions& options) {
    ifcopenshell_clear_error();
    try {
        for (auto context : geometric_contexts(file)) {
            auto true_north = ifcapi::detail::read_ref_attr(context, "TrueNorth");
            if (!options.true_north) {
                if (true_north) {
                    ifcapi::detail::write_ref_attr(context, "TrueNorth", {});
                    if (ifcapi::detail::total_inverses(file, true_north) == 0) {
                        ifcapi::bindings::entity_remove_deep(&true_north);
                    }
                }
                continue;
            }

            const auto& vec = *options.true_north;
            double x = vec[0];
            double y = vec[1];

            if (true_north) {
                if (ifcapi::detail::total_inverses(file, true_north) != 1) {
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
        ifcapi::detail::set_error(e);
    }
}

void georeference_edit_georeferencing(
    ifcopenshell::file* file,
    const GeoreferenceEditGeoreferencingOptions& options)
{
    ifcopenshell_clear_error();
    try {
        if (is_ifc2x3(file)) {
            edit_ifc2x3_georeferencing(file, options);
            return;
        }
        if (options.projected_crs) {
            auto crs_items = ifcapi::detail::instances_by_type(file, "IfcProjectedCRS");
            if (crs_items.empty()) {
                throw std::runtime_error("IfcProjectedCRS not found");
            }
            ifcapi::detail::apply_attribute_props(crs_items.front(), *options.projected_crs);
        }
        if (options.coordinate_operation) {
            auto conversion_items = ifcapi::detail::instances_by_type(file, "IfcCoordinateOperation");
            if (conversion_items.empty()) {
                throw std::runtime_error("IfcCoordinateOperation not found");
            }
            ifcapi::detail::apply_attribute_props(conversion_items.front(), *options.coordinate_operation);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void georeference_edit_wcs(ifcopenshell::file* file, const GeoreferenceEditWcsOptions& options) {
    ifcopenshell_clear_error();
    try {
        double x = options.x.value_or(0.0);
        double y = options.y.value_or(0.0);
        double z = options.z.value_or(0.0);
        double rotation = options.rotation.value_or(0.0);
        bool is_si = options.is_si.value_or(true);

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

        for (auto context : geometric_contexts(file)) {
            auto old_wcs = ifcapi::detail::read_ref_attr(context, "WorldCoordinateSystem");
            auto dimension = static_cast<int>(context.get_attribute_value(
                static_cast<size_t>(ifcapi::detail::attr_index_of(context, "CoordinateSpaceDimension"))));
            express::Base placement = {};
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
                if (ifcapi::detail::total_inverses(file, old_wcs) == 0) {
                    ifcapi::bindings::entity_remove_deep(&old_wcs);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

void georeference_remove_georeferencing(ifcopenshell::file* file) {
    ifcopenshell_clear_error();
    try {
        if (is_ifc2x3(file)) {
            remove_ifc2x3_georeferencing(file);
            return;
        }

        auto projected_crs_items = ifcapi::detail::instances_by_type(file, "IfcProjectedCRS");
        auto coordinate_operations = ifcapi::detail::instances_by_type(file, "IfcCoordinateOperation");
        for (auto projected_crs : projected_crs_items) {
            auto unit = ifcapi::detail::read_ref_attr(projected_crs, "MapUnit");
            if (unit && ifcapi::detail::total_inverses(file, unit) == 1) {
                ifcapi::detail::write_ref_attr(projected_crs, "MapUnit", {});
                ifcapi::bindings::entity_remove_deep(&unit);
            }
            file->remove_entity(projected_crs);
        }
        for (auto coordinate_operation : coordinate_operations) {
            file->remove_entity(coordinate_operation);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

} // namespace bindings
} // namespace ifcapi
