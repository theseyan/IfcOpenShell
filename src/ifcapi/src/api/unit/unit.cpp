// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/unit.h"
#include "../pset/attribute_props.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

express::Base create_dimensional_exponents(
    ifcopenshell::file* file,
    const std::vector<int64_t>& dimensions)
{
    std::array<int, 7> values{{0, 0, 0, 0, 0, 0, 0}};
    for (size_t i = 0; i < std::min(values.size(), dimensions.size()); ++i) {
        values[i] = static_cast<int>(dimensions[i]);
    }
    const auto* decl = file->schema()->declaration_by_name("IfcDimensionalExponents");
    auto result = file->create(decl);
    static const char* attrs[] = {
        "LengthExponent",
        "MassExponent",
        "TimeExponent",
        "ElectricCurrentExponent",
        "ThermodynamicTemperatureExponent",
        "AmountOfSubstanceExponent",
        "LuminousIntensityExponent",
    };
    for (size_t i = 0; i < values.size(); ++i) {
        int idx = ifcapi::detail::attr_index_of(result, attrs[i]);
        if (idx >= 0) result.set_attribute_value(static_cast<size_t>(idx), values[i]);
    }
    return result;
}

express::Base create_dimensional_exponents(
    ifcopenshell::file* file,
    const std::array<int, 7>& dimensions)
{
    return create_dimensional_exponents(
        file, std::vector<int64_t>(dimensions.begin(), dimensions.end()));
}

express::Base create_supplied_dimensional_exponents(
    ifcopenshell::file* file,
    const std::vector<int64_t>& dimensions)
{
    if (dimensions.size() > 7) {
        throw std::runtime_error("Dimensions accepts at most seven integer exponents");
    }
    auto result = file->create(file->schema()->declaration_by_name("IfcDimensionalExponents"));
    for (size_t index = 0; index < dimensions.size(); ++index) {
        result.set_attribute_value(index, static_cast<int>(dimensions[index]));
    }
    return result;
}

express::Base create_si_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const std::string& name,
    const std::string& prefix = {})
{
    auto result = file->create(file->schema()->declaration_by_name("IfcSIUnit"));
    ifcapi::detail::write_string_attr(result, "UnitType", unit_type);
    ifcapi::detail::write_string_attr(result, "Name", name);
    if (prefix.empty()) {
        ifcapi::detail::write_blank_attr(result, "Prefix");
    } else {
        ifcapi::detail::write_string_attr(result, "Prefix", prefix);
    }
    return result;
}

express::Base create_measure_with_unit(
    ifcopenshell::file* file,
    double value,
    express::Base unit)
{
    auto real = file->create(file->schema()->declaration_by_name("IfcReal"));
    real.set_attribute_value(0, value);
    auto result = file->create(file->schema()->declaration_by_name("IfcMeasureWithUnit"));
    ifcapi::detail::write_ref_attr(result, "ValueComponent", real);
    ifcapi::detail::write_ref_attr(result, "UnitComponent", unit);
    return result;
}

express::Base create_conversion_based_unit(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& unit_type,
    const std::array<int, 7>& dimensions,
    const std::string& si_name,
    double conversion,
    double offset = 0.0)
{
    auto exponents = create_dimensional_exponents(file, dimensions);
    auto si_unit = create_si_unit(file, unit_type, si_name, unit_type == "MASSUNIT" ? "KILO" : "");
    auto conversion_factor = create_measure_with_unit(file, conversion, si_unit);
    const char* ifc_class = offset == 0.0
        ? "IfcConversionBasedUnit"
        : "IfcConversionBasedUnitWithOffset";
    auto result = file->create(file->schema()->declaration_by_name(ifc_class));
    ifcapi::detail::write_ref_attr(result, "Dimensions", exponents);
    ifcapi::detail::write_string_attr(result, "UnitType", unit_type);
    ifcapi::detail::write_string_attr(result, "Name", name);
    ifcapi::detail::write_ref_attr(result, "ConversionFactor", conversion_factor);
    if (offset != 0.0) {
        int index = ifcapi::detail::attr_index_of(result, "ConversionOffset");
        if (index >= 0) result.set_attribute_value(static_cast<size_t>(index), offset);
    }
    return result;
}

express::Base create_metric_convenience_unit(
    ifcopenshell::file* file,
    const std::string& kind,
    const std::string& raw)
{
    std::string name = ifcapi::bindings::unit_get_unit_name(raw);
    if (name.empty()) throw std::runtime_error("Unrecognised metric unit text: " + raw);
    if (kind == "area") name = "SQUARE_" + name;
    if (kind == "volume") name = "CUBIC_" + name;
    std::string unit_type = kind;
    std::transform(unit_type.begin(), unit_type.end(), unit_type.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return create_si_unit(file, unit_type + "UNIT", name, ifcapi::bindings::unit_get_prefix(raw));
}

express::Base create_imperial_convenience_unit(
    ifcopenshell::file* file,
    const std::string& kind,
    const std::string& raw)
{
    int exponent = kind == "length" ? 1 : kind == "area" ? 2 : 3;
    std::string prefix = kind == "length" ? "" : kind == "area" ? "square " : "cubic ";
    std::string base_name;
    if (raw == "INCHES") base_name = "inch";
    else if (raw == "FEET") base_name = "foot";
    else if (raw == "MILES") base_name = "mile";
    else if (raw == "THOU") base_name = "thou";
    else throw std::runtime_error("Unrecognised imperial unit text: " + raw);
    const std::string name = prefix + base_name;
    const std::string unit_type = kind == "length" ? "LENGTHUNIT" : kind == "area" ? "AREAUNIT" : "VOLUMEUNIT";
    auto conversion = ifcapi::detail::unit::si_conversions().find(name);
    if (conversion == ifcapi::detail::unit::si_conversions().end()) {
        throw std::runtime_error("Missing conversion factor for " + name);
    }
    std::array<int, 7> dimensions{{exponent, 0, 0, 0, 0, 0, 0}};
    const std::string si_name = exponent == 1 ? "METRE" : exponent == 2 ? "SQUARE_METRE" : "CUBIC_METRE";
    return create_conversion_based_unit(file, name, unit_type, dimensions, si_name, conversion->second);
}

std::string assignment_key(express::Base unit) {
    if (unit.declaration().is("IfcMonetaryUnit")) return unit.declaration().name();
    return ifcapi::detail::read_string_attr(unit, "UnitType");
}

void remove_from_assignment(ifcopenshell::file* file, express::Base unit) {
    auto unit_assignment = ifcapi::bindings::unit_get_unit_assignment(file);
    if (!unit_assignment || !unit) return;
    auto units = ifcapi::detail::read_ref_aggregate(unit_assignment, "Units");
    auto it = std::find(units.begin(), units.end(), unit);
    if (it == units.end()) return;
    units.erase(it);
    if (units.empty()) {
        file->remove_entity(unit_assignment);
    } else {
        ifcapi::detail::write_ref_aggregate(unit_assignment, "Units", units);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base unit_assign_unit(
    ifcopenshell::file* file,
    const UnitAssignUnitOptions& options)
{
    if (!file) throw std::runtime_error("unit_assign_unit requires a file");

    std::vector<express::Base> new_units;
    if (options.units && !options.units->empty()) {
        new_units = *options.units;
    } else {
        struct ConvenienceInput {
            const char* kind;
            bool is_metric;
            std::string raw;
        };
        const ConvenienceInput inputs[] = {
            {"length", options.length_is_metric.value_or(true), options.length_raw.value_or("MILLIMETERS")},
            {"area", options.area_is_metric.value_or(true), options.area_raw.value_or("METERS")},
            {"volume", options.volume_is_metric.value_or(true), options.volume_raw.value_or("METERS")},
        };
        new_units.reserve(3);
        for (const auto& input : inputs) {
            new_units.push_back(input.is_metric
                ? create_metric_convenience_unit(file, input.kind, input.raw)
                : create_imperial_convenience_unit(file, input.kind, input.raw));
        }
    }

    auto assignment = unit_get_unit_assignment(file);
    if (!assignment) {
        auto projects = file->instances_by_type(file->schema()->declaration_by_name("IfcProject"));
        auto project = projects.at(0);
        assignment = file->create(file->schema()->declaration_by_name("IfcUnitAssignment"));
        detail::write_ref_attr(project, "UnitsInContext", assignment);
    }

    std::vector<std::string> new_keys;
    new_keys.reserve(new_units.size());
    for (auto unit : new_units) {
        if (!unit) throw std::runtime_error("unit_assign_unit received an empty unit");
        new_keys.push_back(assignment_key(unit));
    }

    std::vector<express::Base> assigned;
    for (auto unit : detail::read_ref_aggregate(assignment, "Units")) {
        const auto key = assignment_key(unit);
        if (std::find(new_keys.begin(), new_keys.end(), unit.declaration().name()) == new_keys.end() &&
            std::find(new_keys.begin(), new_keys.end(), key) == new_keys.end() &&
            std::find(assigned.begin(), assigned.end(), unit) == assigned.end()) {
            assigned.push_back(unit);
        }
    }
    for (auto unit : new_units) {
        if (std::find(assigned.begin(), assigned.end(), unit) == assigned.end()) assigned.push_back(unit);
    }
    detail::write_ref_aggregate(assignment, "Units", assigned);
    return assignment;
}

express::Base unit_add_conversion_based_unit(
    ifcopenshell::file* file,
    const UnitAddConversionBasedUnitOptions& options)
{
    if (!file) throw std::runtime_error("unit_add_conversion_based_unit requires a file");
    const std::string name = options.name.value_or("foot");
    std::string unit_type = "USERDEFINED";
    for (const auto& item : detail::unit::imperial_types()) {
        if (item.first == name) {
            unit_type = item.second;
            break;
        }
    }
    const auto dimensions = detail::unit::named_dimensions().at(unit_type);
    const std::string si_name = detail::unit::si_type_names().at(unit_type);
    const auto conversion_it = detail::unit::si_conversions().find(name);
    const double conversion = conversion_it == detail::unit::si_conversions().end() ? 1.0 : conversion_it->second;

    double offset = options.conversion_offset.value_or(0.0);
    if (offset == 0.0) {
        const auto offset_it = detail::unit::si_offsets().find(name);
        if (offset_it != detail::unit::si_offsets().end()) offset = offset_it->second;
    }
    return create_conversion_based_unit(file, name, unit_type, dimensions, si_name, conversion, offset);
}

void unit_edit_named_unit(
    ifcopenshell::file* file,
    const UnitEditNamedUnitOptions& options)
{
    if (!file) throw std::runtime_error("unit_edit_named_unit requires a file");
    if (!options.unit) throw std::runtime_error("unit_edit_named_unit requires a unit");

    const ifcapi_pset::Entry* dimensions_entry = nullptr;
    if (options.attributes) {
        for (const auto& entry : options.attributes->entries) {
            if (entry.key == "Dimensions") {
                dimensions_entry = &entry;
                break;
            }
        }
    }
    if (dimensions_entry) {
        if (dimensions_entry->kind != ifcapi_pset::Kind::INT_LIST) {
            throw std::runtime_error("Dimensions requires integer exponents");
        }
        auto dimensions = detail::read_ref_attr(options.unit, "Dimensions");
        if (!dimensions) throw std::runtime_error("Named unit has no Dimensions entity");
        if (file->instances_by_reference(static_cast<int>(dimensions.id())).size() > 1) {
            detail::write_ref_attr(
                options.unit,
                "Dimensions",
                create_supplied_dimensional_exponents(file, dimensions_entry->int_list));
        } else {
            for (size_t index = 0; index < dimensions_entry->int_list.size(); ++index) {
                dimensions.set_attribute_value(index, static_cast<int>(dimensions_entry->int_list[index]));
            }
        }
    }
    if (options.attributes) {
        for (const auto& entry : options.attributes->entries) {
            if (entry.key != "Dimensions") detail::apply_attribute_prop(options.unit, entry);
        }
    }
}

express::Base unit_add_si_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const char* prefix)
{
    const auto* decl = file->schema()->declaration_by_name("IfcSIUnit");
    auto result = file->create(decl);
    detail::write_string_attr(result, "UnitType", unit_type);
    const auto& names = detail::unit::si_type_names();
    auto it = names.find(unit_type);
    if (it != names.end()) detail::write_string_attr(result, "Name", it->second);
    if (prefix) {
        detail::write_string_attr(result, "Prefix", prefix);
    } else {
        detail::write_blank_attr(result, "Prefix");
    }
    return result;
}

express::Base unit_add_monetary_unit(
    ifcopenshell::file* file,
    const std::string& currency)
{
    const auto* decl = file->schema()->declaration_by_name("IfcMonetaryUnit");
    auto result = file->create(decl);
    detail::write_string_attr(result, "Currency", currency);
    return result;
}

express::Base unit_add_context_dependent_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const std::string& name,
    const std::vector<int64_t>& dimensions)
{
    const auto* decl = file->schema()->declaration_by_name("IfcContextDependentUnit");
    auto result = file->create(decl);
    detail::write_ref_attr(result, "Dimensions", create_dimensional_exponents(file, dimensions));
    detail::write_string_attr(result, "UnitType", unit_type);
    detail::write_string_attr(result, "Name", name);
    return result;
}

express::Base unit_add_derived_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const char* userdefinedtype,
    const std::vector<express::Base>& units,
    const std::vector<int64_t>& exponents)
{
    if (units.size() != exponents.size()) {
        throw std::runtime_error("Derived unit units and exponents must have the same length");
    }
    std::vector<express::Base> elements;
    elements.reserve(units.size());
    const auto* element_decl = file->schema()->declaration_by_name("IfcDerivedUnitElement");
    for (size_t i = 0; i < units.size(); ++i) {
        auto element = file->create(element_decl);
        detail::write_ref_attr(element, "Unit", units[i]);
        int idx = detail::attr_index_of(element, "Exponent");
        if (idx >= 0) element.set_attribute_value(static_cast<size_t>(idx), static_cast<int>(exponents[i]));
        elements.push_back(element);
    }

    const auto* decl = file->schema()->declaration_by_name("IfcDerivedUnit");
    auto result = file->create(decl);
    detail::write_ref_aggregate(result, "Elements", elements);
    detail::write_string_attr(result, "UnitType", unit_type);
    if (userdefinedtype) {
        detail::write_string_attr(result, "UserDefinedType", userdefinedtype);
    } else {
        detail::write_blank_attr(result, "UserDefinedType");
    }
    return result;
}

void unit_unassign_unit(
    ifcopenshell::file* file,
    const std::vector<express::Base>& units)
{
    auto unit_assignment = unit_get_unit_assignment(file);
    if (!unit_assignment) return;
    auto assigned_units = detail::read_ref_aggregate(unit_assignment, "Units");
    std::vector<express::Base> kept;
    for (auto unit : assigned_units) {
        if (std::find(units.begin(), units.end(), unit) == units.end()) {
            kept.push_back(unit);
        }
    }
    if (kept.empty()) {
        file->remove_entity(unit_assignment);
    } else {
        detail::write_ref_aggregate(unit_assignment, "Units", kept);
    }
}

void unit_remove_unit(
    ifcopenshell::file* file,
    express::Base* unit_ptr)
{
    auto unit = ifcapi::detail::deref_or_empty(unit_ptr);
    remove_from_assignment(file, unit);
    entity_remove_deep(&unit);
}

} // namespace bindings
} // namespace ifcapi
