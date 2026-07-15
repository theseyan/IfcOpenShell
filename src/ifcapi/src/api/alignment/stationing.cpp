// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/alignment.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#if IFCOPENSHELL_WITH_IFCGEOM
#include "ifcgeom/ConversionSettings.h"
#include "ifcgeom/abstract_mapping.h"
#endif

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <memory>
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

std::optional<double> station_value(express::Base referent) {
    for (auto rel : inverses(referent, "IsDefinedBy")) {
        auto pset = read_ref_attr(rel, "RelatingPropertyDefinition");
        if (!pset || read_string_attr(pset, "Name") != "Pset_Stationing") continue;
        for (auto property : refs(pset, "HasProperties")) {
            if (read_string_attr(property, "Name") == "Station") {
                auto value = read_ref_attr(property, "NominalValue");
                if (value) return wrapped_double(value);
            }
        }
    }
    return std::nullopt;
}

void add_station_property(
    ifcopenshell::file* file,
    express::Base referent,
    double station,
    express::Base owner_history)
{
    auto property = create(file, "IfcPropertySingleValue");
    write_string_attr(property, "Name", "Station");
    write_ref_attr(property, "NominalValue", length_measure(file, station));
    auto pset = rooted(file, "IfcPropertySet");
    write_string_attr(pset, "Name", "Pset_Stationing");
    write_ref_aggregate(pset, "HasProperties", {property});
    if (owner_history) write_ref_attr(pset, "OwnerHistory", owner_history);
    auto rel = rooted(file, "IfcRelDefinesByProperties");
    write_ref_aggregate(rel, "RelatedObjects", {referent});
    write_ref_attr(rel, "RelatingPropertyDefinition", pset);
    if (owner_history) write_ref_attr(rel, "OwnerHistory", owner_history);
}

std::array<double, 16> evaluate_linear_placement(express::Base relative) {
#if IFCOPENSHELL_WITH_IFCGEOM
    ifcopenshell::geometry::Settings settings;
    std::unique_ptr<ifcopenshell::geometry::abstract_mapping> mapping(
        ifcopenshell::geometry::impl::mapping_implementations().construct(relative.file(), settings));
    auto matrix = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(relative));
    if (!matrix) throw std::runtime_error("Unable to evaluate IfcAxis2PlacementLinear");
    std::array<double, 16> result{};
    const auto& components = matrix->ccomponents();
    for (int row = 0; row < 4; ++row) for (int col = 0; col < 4; ++col) result[row * 4 + col] = components(row, col);
    return result;
#else
    (void)relative;
    throw std::runtime_error("Linear placement evaluation requires IfcGeom");
#endif
}

std::string type(express::Base segment) {
    auto parameters = read_ref_attr(segment, "DesignParameters");
    return enum_value(parameters, "PredefinedType");
}

} // namespace

namespace ifcapi::detail::alignment {

std::string default_referent_label(express::Base previous, express::Base current) {
    auto sample = current ? current : previous;
    if (!sample || !sample.declaration().is("IfcAlignmentSegment")) throw std::runtime_error("A segment is required");
    auto parameters = ifcapi::detail::read_ref_attr(sample, "DesignParameters");
    if (!previous) {
        if (parameters.declaration().is("IfcAlignmentHorizontalSegment")) return "P.O.B.";
        if (parameters.declaration().is("IfcAlignmentVerticalSegment")) return "V.P.O.B.";
        return "C.P.O.B.";
    }
    if (!current) {
        if (parameters.declaration().is("IfcAlignmentHorizontalSegment")) return "P.O.E.";
        if (parameters.declaration().is("IfcAlignmentVerticalSegment")) return "V.P.O.E.";
        return "C.P.O.E.";
    }
    auto previous_parameters = ifcapi::detail::read_ref_attr(previous, "DesignParameters");
    auto current_parameters = ifcapi::detail::read_ref_attr(current, "DesignParameters");
    if (previous_parameters.declaration().name() != current_parameters.declaration().name()) {
        throw std::runtime_error("previous_segment and segment must have matching design parameter types");
    }
    const auto previous_type = type(previous), current_type = type(current);
    if (parameters.declaration().is("IfcAlignmentHorizontalSegment")) {
        const bool previous_line = previous_type == "LINE";
        const bool previous_circle = previous_type == "CIRCULARARC";
        const bool current_line = current_type == "LINE";
        const bool current_circle = current_type == "CIRCULARARC";
        if (previous_line && current_line) return "P.I.";
        if (previous_line && current_circle) return "P.C.";
        if (previous_line) return "T.S.";
        if (previous_circle && current_line) return "P.T.";
        if (previous_circle && current_circle) return "P.C.C.";
        if (previous_circle) return "C.S.";
        if (current_circle) return "S.C.";
        if (current_line) return "S.T.";
        return "xx";
    }
    if (parameters.declaration().is("IfcAlignmentVerticalSegment")) {
        if (previous_type == "CONSTANTGRADIENT" && current_type == "CONSTANTGRADIENT") return "P.V.I";
        if (previous_type == "CONSTANTGRADIENT" && current_type == "PARABOLICARC") return "P.V.C.";
        if (previous_type == "PARABOLICARC" && current_type == "CONSTANTGRADIENT") return "P.V.T.";
        if (previous_type == "PARABOLICARC" && current_type == "PARABOLICARC") return "V.C.C.";
    }
    return "xx";
}

std::string station_string(ifcopenshell::file* file, double input) {
    auto unit = ifcapi::bindings::unit_get_project_unit(file, "LENGTHUNIT");
    if (!unit) throw std::runtime_error("Project length unit is required for station formatting");
    const bool conversion = unit.declaration().is("IfcConversionBasedUnit");
    double station = input;
    int separator = conversion ? 2 : 3;
    int precision = conversion ? 2 : 3;
    if (conversion) {
        station = ifcapi::bindings::unit_convert(
            input, "", ifcapi::detail::read_string_attr(unit, "Name"), "", "foot");
    } else {
        const auto prefix = ifcapi::detail::read_optional_string_attr(unit, "Prefix");
        station = ifcapi::bindings::unit_convert(
            input, prefix.has_value ? prefix.value : "", ifcapi::detail::read_string_attr(unit, "Name"), "", "meter");
    }
    const double absolute = std::abs(station);
    const double shifter = std::pow(10.0, separator);
    long long leading = static_cast<long long>(std::floor(absolute / shifter));
    double trailing = absolute - static_cast<double>(leading) * shifter;
    if (std::abs(trailing - shifter) <= 5.0 * std::pow(10.0, -(precision + 1))) { trailing = 0.0; ++leading; }
    if (station < 0.0) leading = -leading;
    std::ostringstream result;
    if (station < 0.0 && leading == 0) result << '-';
    result << leading << '+' << std::fixed << std::setprecision(precision)
           << std::setw(separator + precision + 1) << std::setfill('0') << trailing;
    return result.str();
}

} // namespace ifcapi::detail::alignment

namespace ifcapi::bindings {

std::string alignment_default_referent_label(
    std::optional<express::Base> previous_segment,
    std::optional<express::Base> segment)
{
    return ifcapi::detail::alignment::default_referent_label(
        previous_segment.value_or(express::Base()), segment.value_or(express::Base()));
}

std::string alignment_station_as_string(ifcopenshell::file* file, double station) {
    return ifcapi::detail::alignment::station_string(file, station);
}

void alignment_update_fallback_position(ifcopenshell::file* file, express::Base placement) {
    require_ifc4x3(file);
    require_owned(file, placement, "linear_placement");
    require_type(placement, "IfcLinearPlacement", "linear_placement");
    auto relative = read_ref_attr(placement, "RelativePlacement");
    if (!relative) throw std::runtime_error("IfcLinearPlacement.RelativePlacement is required");
    const auto matrix = evaluate_linear_placement(relative);
    const double length_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    auto cartesian = read_ref_attr(placement, "CartesianPosition");
    if (!cartesian) {
        cartesian = axis3d(file, point(file, {0.0, 0.0, 0.0}), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
        write_ref_attr(placement, "CartesianPosition", cartesian);
    }
    write_double_aggregate(
        read_ref_attr(cartesian, "Location"), "Coordinates",
        {matrix[3] / length_scale, matrix[7] / length_scale, matrix[11] / length_scale});
    auto ref = read_ref_attr(cartesian, "RefDirection");
    if (!ref) { ref = direction(file, {1.0, 0.0, 0.0}); write_ref_attr(cartesian, "RefDirection", ref); }
    write_double_aggregate(ref, "DirectionRatios", {matrix[0], matrix[4], matrix[8]});
    auto axis = read_ref_attr(cartesian, "Axis");
    if (!axis) { axis = direction(file, {0.0, 0.0, 1.0}); write_ref_attr(cartesian, "Axis", axis); }
    write_double_aggregate(axis, "DirectionRatios", {matrix[2], matrix[6], matrix[10]});
}

express::Base alignment_add_stationing_referent(
    ifcopenshell::file* file,
    const AlignmentAddStationingReferentOptions& options)
{
    require_ifc4x3(file);
    require_owned(file, options.alignment, "alignment");
    require_owned(file, options.positioned_product, "positioned_product");
    require_type(options.alignment, "IfcAlignment", "alignment");
    express::Base owner_history = options.owner_history.value_or(express::Base());
    if (!owner_history && options.user && options.application) {
        owner_history = ifcapi::detail::ensure_owner_history(file, {}, *options.user, *options.application);
    }
    auto basis = alignment_get_basis_curve(options.alignment);
    express::Base object_placement;
    if (basis && (*basis).declaration().is("IfcCompositeCurve") && !refs(*basis, "Segments").empty()) {
        object_placement = create(file, "IfcLinearPlacement");
        auto relative = create(file, "IfcAxis2PlacementLinear");
        auto location = create(file, "IfcPointByDistanceExpression");
        write_ref_attr(location, "DistanceAlong", length_measure(file, options.distance_along));
        write_ref_attr(location, "BasisCurve", *basis);
        write_ref_attr(relative, "Location", location);
        write_ref_attr(object_placement, "RelativePlacement", relative);
        alignment_update_fallback_position(file, object_placement);
    } else {
        object_placement = create(file, "IfcLocalPlacement");
        auto alignment_placement = read_ref_attr(options.alignment, "ObjectPlacement");
        auto relative = read_ref_attr(alignment_placement, "RelativePlacement");
        auto location = read_ref_attr(relative, "Location");
        auto coordinates = location ? read_double_aggregate(location, "Coordinates") : std::vector<double>{0.0, 0.0};
        write_ref_attr(object_placement, "RelativePlacement", axis2d(file, point(file, coordinates)));
    }
    auto referent = rooted(file, "IfcReferent");
    if (owner_history) write_ref_attr(referent, "OwnerHistory", owner_history);
    write_string_attr(referent, "Name", options.name);
    write_ref_attr(referent, "ObjectPlacement", object_placement);
    write_enum_attr(referent, "PredefinedType", "STATION");
    add_station_property(file, referent, options.station, owner_history);
    auto nest = alignment_get_referent_nest(options.alignment);
    express::Base relation;
    if (!nest) {
        relation = rooted(file, "IfcRelNests");
        if (owner_history) write_ref_attr(relation, "OwnerHistory", owner_history);
        write_ref_attr(relation, "RelatingObject", options.alignment);
        write_ref_aggregate(relation, "RelatedObjects", {referent});
    } else {
        relation = *nest;
        auto related = refs(relation, "RelatedObjects");
        related.push_back(referent);
        std::stable_sort(related.begin(), related.end(), [](express::Base a, express::Base b) {
            return station_value(a).value_or(0.0) < station_value(b).value_or(0.0);
        });
        write_ref_aggregate(relation, "RelatedObjects", related);
        if (options.user && options.application) ifcapi::detail::update_owner_history(file, relation, *options.user, *options.application);
    }
    auto positions = inverses(referent, "Positions");
    if (positions.empty()) {
        auto rel = rooted(file, "IfcRelPositions");
        if (owner_history) write_ref_attr(rel, "OwnerHistory", owner_history);
        write_ref_attr(rel, "RelatingPositioningElement", referent);
        write_ref_aggregate(rel, "RelatedProducts", {options.positioned_product});
    } else {
        auto products = refs(positions.front(), "RelatedProducts");
        if (std::find(products.begin(), products.end(), options.positioned_product) == products.end()) products.push_back(options.positioned_product);
        write_ref_aggregate(positions.front(), "RelatedProducts", products);
    }
    return referent;
}

} // namespace ifcapi::bindings
