// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

const std::unordered_map<std::string, std::string>& si_type_names() {
    static const std::unordered_map<std::string, std::string> names = {
        {"ABSORBEDDOSEUNIT", "GRAY"},
        {"AMOUNTOFSUBSTANCEUNIT", "MOLE"},
        {"AREAUNIT", "SQUARE_METRE"},
        {"DOSEEQUIVALENTUNIT", "SIEVERT"},
        {"ELECTRICCAPACITANCEUNIT", "FARAD"},
        {"ELECTRICCHARGEUNIT", "COULOMB"},
        {"ELECTRICCONDUCTANCEUNIT", "SIEMENS"},
        {"ELECTRICCURRENTUNIT", "AMPERE"},
        {"ELECTRICRESISTANCEUNIT", "OHM"},
        {"ELECTRICVOLTAGEUNIT", "VOLT"},
        {"ENERGYUNIT", "JOULE"},
        {"FORCEUNIT", "NEWTON"},
        {"FREQUENCYUNIT", "HERTZ"},
        {"ILLUMINANCEUNIT", "LUX"},
        {"INDUCTANCEUNIT", "HENRY"},
        {"LENGTHUNIT", "METRE"},
        {"LUMINOUSFLUXUNIT", "LUMEN"},
        {"LUMINOUSINTENSITYUNIT", "CANDELA"},
        {"MAGNETICFLUXDENSITYUNIT", "TESLA"},
        {"MAGNETICFLUXUNIT", "WEBER"},
        {"MASSUNIT", "GRAM"},
        {"PLANEANGLEUNIT", "RADIAN"},
        {"POWERUNIT", "WATT"},
        {"PRESSUREUNIT", "PASCAL"},
        {"RADIOACTIVITYUNIT", "BECQUEREL"},
        {"SOLIDANGLEUNIT", "STERADIAN"},
        {"THERMODYNAMICTEMPERATUREUNIT", "KELVIN"},
        {"TIMEUNIT", "SECOND"},
        {"VOLUMEUNIT", "CUBIC_METRE"},
        {"USERDEFINED", "METRE"},
    };
    return names;
}

IfcUtil::IfcBaseClass* create_dimensional_exponents(
    IfcParse::IfcFile* file,
    const std::vector<int64_t>& dimensions)
{
    std::array<int, 7> values{{0, 0, 0, 0, 0, 0, 0}};
    for (size_t i = 0; i < std::min(values.size(), dimensions.size()); ++i) {
        values[i] = static_cast<int>(dimensions[i]);
    }
    const auto* decl = file->schema()->declaration_by_name("IfcDimensionalExponents");
    auto* result = file->create(decl);
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
        if (idx >= 0) result->set_attribute_value(static_cast<size_t>(idx), values[i]);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> mutable_units(const std::vector<const IfcUtil::IfcBaseClass*>& units) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(units.size());
    for (auto* unit : units) {
        if (unit) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(unit));
    }
    return result;
}

void remove_from_assignment(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* unit) {
    auto* unit_assignment = ifcapi::bindings::unit_get_unit_assignment(file);
    if (!unit_assignment || !unit) return;
    auto units = ifcapi::detail::read_ref_aggregate(unit_assignment, "Units");
    auto it = std::find(units.begin(), units.end(), unit);
    if (it == units.end()) return;
    units.erase(it);
    if (units.empty()) {
        file->removeEntity(unit_assignment);
    } else {
        ifcapi::detail::write_ref_aggregate(unit_assignment, "Units", units);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* unit_add_si_unit(
    IfcParse::IfcFile* file,
    const std::string& unit_type,
    const char* prefix)
{
    const auto* decl = file->schema()->declaration_by_name("IfcSIUnit");
    auto* result = file->create(decl);
    detail::write_string_attr(result, "UnitType", unit_type);
    auto it = si_type_names().find(unit_type);
    if (it != si_type_names().end()) detail::write_string_attr(result, "Name", it->second);
    if (prefix) {
        detail::write_string_attr(result, "Prefix", prefix);
    } else {
        detail::write_blank_attr(result, "Prefix");
    }
    return result;
}

IfcUtil::IfcBaseClass* unit_add_monetary_unit(
    IfcParse::IfcFile* file,
    const std::string& currency)
{
    const auto* decl = file->schema()->declaration_by_name("IfcMonetaryUnit");
    auto* result = file->create(decl);
    detail::write_string_attr(result, "Currency", currency);
    return result;
}

IfcUtil::IfcBaseClass* unit_add_context_dependent_unit(
    IfcParse::IfcFile* file,
    const std::string& unit_type,
    const std::string& name,
    const std::vector<int64_t>& dimensions)
{
    const auto* decl = file->schema()->declaration_by_name("IfcContextDependentUnit");
    auto* result = file->create(decl);
    detail::write_ref_attr(result, "Dimensions", create_dimensional_exponents(file, dimensions));
    detail::write_string_attr(result, "UnitType", unit_type);
    detail::write_string_attr(result, "Name", name);
    return result;
}

IfcUtil::IfcBaseClass* unit_add_derived_unit(
    IfcParse::IfcFile* file,
    const std::string& unit_type,
    const char* userdefinedtype,
    const std::vector<const IfcUtil::IfcBaseClass*>& units,
    const std::vector<int64_t>& exponents)
{
    if (units.size() != exponents.size()) {
        throw std::runtime_error("Derived unit units and exponents must have the same length");
    }
    std::vector<IfcUtil::IfcBaseClass*> elements;
    elements.reserve(units.size());
    const auto* element_decl = file->schema()->declaration_by_name("IfcDerivedUnitElement");
    for (size_t i = 0; i < units.size(); ++i) {
        auto* element = file->create(element_decl);
        detail::write_ref_attr(element, "Unit", const_cast<IfcUtil::IfcBaseClass*>(units[i]));
        int idx = detail::attr_index_of(element, "Exponent");
        if (idx >= 0) element->set_attribute_value(static_cast<size_t>(idx), static_cast<int>(exponents[i]));
        elements.push_back(element);
    }

    const auto* decl = file->schema()->declaration_by_name("IfcDerivedUnit");
    auto* result = file->create(decl);
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
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& units)
{
    auto* unit_assignment = unit_get_unit_assignment(file);
    if (!unit_assignment) return;
    auto assigned_units = detail::read_ref_aggregate(unit_assignment, "Units");
    auto units_to_remove = mutable_units(units);
    std::vector<IfcUtil::IfcBaseClass*> kept;
    for (auto* unit : assigned_units) {
        if (std::find(units_to_remove.begin(), units_to_remove.end(), unit) == units_to_remove.end()) {
            kept.push_back(unit);
        }
    }
    if (kept.empty()) {
        file->removeEntity(unit_assignment);
    } else {
        detail::write_ref_aggregate(unit_assignment, "Units", kept);
    }
}

void unit_remove_unit(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* unit)
{
    remove_from_assignment(file, unit);
    entity_remove_deep2(unit);
}

} // namespace bindings
} // namespace ifcapi
