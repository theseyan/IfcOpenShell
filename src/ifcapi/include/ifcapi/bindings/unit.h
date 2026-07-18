/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_UNIT_H
#define IFCAPI_BINDINGS_UNIT_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/bindings/types.h"
#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Extract the SI prefix from a combined unit text string.
 *
 * For example, "KILOMETRE" returns "KILO", "MILLIGRAM" returns "MILLI".
 * Returns an empty string if no prefix is found.
 *
 * @param text Combined prefix and unit name string.
 * @return The prefix portion, or empty string if none.
 */
IFCAPI_BINDING std::string unit_get_prefix(const std::string& text);
/**
 * Return the numeric multiplier for an SI prefix.
 *
 * For example, "KILO" returns 1000.0, "MILLI" returns 0.001.
 * Returns 1.0 if no prefix is found.
 *
 * @param text SI prefix string (e.g. "KILO", "MILLI").
 * @return The multiplier value.
 */
IFCAPI_BINDING double unit_get_prefix_multiplier(const std::string& text);
/**
 * Extract the base unit name from a combined unit text string.
 *
 * Normalises "METER" to "METRE" before matching. For example,
 * "KILOMETRE" returns "METRE", "SQUARE_METRE" returns "SQUARE_METRE".
 * Returns an empty string if no unit name is found.
 *
 * @param text Combined prefix and unit name string.
 * @return The base unit name, or empty string if not recognised.
 */
IFCAPI_BINDING std::string unit_get_unit_name(const std::string& text);
/**
 * Extract the base unit name, accepting both metric and imperial names.
 *
 * Like unit_get_unit_name but also recognises imperial unit names
 * (e.g. "FOOT", "INCH"). Returns an empty string if not recognised.
 *
 * @param text Unit name string.
 * @return The base unit name, or empty string if not recognised.
 */
IFCAPI_BINDING std::string unit_get_unit_name_universal(const std::string& text);
/**
 * Derive the IFC measure class name from a unit type string.
 *
 * For example, "LENGTHUNIT" returns "IfcLengthMeasure",
 * "USERDEFINED" returns "IfcNumericMeasure". Returns an empty string
 * for unrecognised input.
 *
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @return The corresponding IFC measure class name.
 */
IFCAPI_BINDING std::string unit_get_measure_class(const std::string& unit_type);
/**
 * Derive the IFC unit type string from a measure class name.
 *
 * The inverse of unit_get_measure_class. For example,
 * "IfcLengthMeasure" returns "LENGTHUNIT".
 *
 * @param measure_class IFC measure class name.
 * @return The corresponding IFC unit type, or empty string if unrecognised.
 */
IFCAPI_BINDING std::string unit_get_measure_unit_type(const std::string& measure_class);
/**
 * Infer the IFC measure class from a unit symbol string.
 *
 * For example, "m" returns "IfcLengthMeasure", "m2" returns "IfcAreaMeasure",
 * "kg" returns "IfcMassMeasure". Returns "IfcNumericMeasure" for unknown symbols.
 *
 * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
 * @return The corresponding IFC measure class name.
 */
IFCAPI_BINDING std::string unit_get_symbol_measure_class(const std::string& symbol);
/**
 * Infer the IFC quantity class from a unit symbol string.
 *
 * For example, "m" returns "IfcQuantityLength", "m2" returns "IfcQuantityArea",
 * "kg" returns "IfcQuantityWeight". Returns "IfcQuantityCount" for unknown symbols.
 *
 * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
 * @return The corresponding IFC quantity class name.
 */
IFCAPI_BINDING std::string unit_get_symbol_quantity_class(const std::string& symbol);
/**
 * Return the dimensional exponents for a given SI unit name.
 *
 * Returns a 7-element sequence of integers corresponding to the
 * IfcDimensionalExponents attributes: Length, Mass, Time,
 * ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance,
 * LuminousIntensity. Falls back to the "OTHERWISE" entry for
 * unknown types.
 *
 * @param name SI unit name (e.g. "METRE", "GRAM").
 * @return 7-element sequence of dimensional exponents.
 */
IFCAPI_BINDING Dimensions7 unit_get_si_dimensions(const std::string& name);
/**
 * Return the named dimensional exponents for a given unit type name.
 *
 * Like unit_get_si_dimensions but returns all zeros for unknown types
 * instead of falling back.
 *
 * @param name Unit type name.
 * @return 7-element sequence of dimensional exponents.
 */
IFCAPI_BINDING Dimensions7 unit_get_named_dimensions(const std::string& name);
/**
 * Convert a numeric value between units specified by prefix and name strings.
 *
 * Supports SI conversions and prefix-aware scaling. For example,
 * converting from ("KILO", "METRE") to ("", "METRE") multiplies by 1000.
 *
 * @param value Numeric value to convert.
 * @param from_prefix Source SI prefix (e.g. "KILO"). Empty for base unit.
 * @param from_unit Source unit name (e.g. "METRE", "GRAM").
 * @param to_prefix Target SI prefix.
 * @param to_unit Target unit name.
 * @return The converted value.
 */
IFCAPI_BINDING double unit_convert(
    double value,
    const std::string& from_prefix,
    const std::string& from_unit,
    const std::string& to_prefix,
    const std::string& to_unit);
/**
 * Format a length value as an imperial or metric string.
 *
 * For imperial output: returns feet-inches notation with fractional
 * precision (e.g. "5' - 3 1/4\""). For metric: returns a rounded
 * decimal value.
 *
 * @param value Length value to format.
 * @param precision Fractional precision denominator for imperial (e.g. 16 for 1/16").
 * @param decimal_places Number of decimal places for metric output.
 * @param suppress_zero_inches When true, omits zero inches in imperial output.
 * @param unit_system "imperial" or "metric".
 * @param input_unit Source unit: "foot" or "inch". Defaults to "foot" when empty.
 * @param output_unit Target unit: "foot" or "inch". Defaults to "foot" when empty.
 * @return Formatted length string.
 */
IFCAPI_BINDING std::string unit_format_length(
    double value,
    double precision,
    int decimal_places,
    bool suppress_zero_inches,
    const std::string& unit_system,
    const std::string& input_unit,
    const std::string& output_unit);
/**
 * Return the IfcUnitAssignment entity for the project.
 *
 * @param file File to query.
 * @return The IfcUnitAssignment entity, or no result if it is not found.
 */
IFCAPI_BINDING express::Base unit_get_unit_assignment(ifcopenshell::file* file);
/**
 * Return the project unit for a given unit type.
 *
 * Searches the project's IfcUnitAssignment for a unit whose UnitType
 * matches the given string.
 *
 * @param file File to query.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @return The matching unit entity, or no result if it is not found.
 */
IFCAPI_BINDING express::Base unit_get_project_unit(ifcopenshell::file* file, const std::string& unit_type);
/**
 * Return the full name of an IfcSIUnit (prefix + name).
 *
 * For example, a unit with Prefix "KILO" and Name "METRE" returns
 * "KILOMETRE".
 *
 * @param unit IfcSIUnit entity.
 * @return Concatenated prefix and name string.
 */
IFCAPI_BINDING std::string unit_get_full_unit_name(express::Base* unit);
/**
 * Return the display symbol for a unit entity.
 *
 * For IfcSIUnit: prefix symbol + unit symbol (e.g. "km").
 * For IfcContextDependentUnit with USERDEFINED UnitType: the Name attribute.
 * Returns "?" for unrecognised units.
 *
 * @param unit IfcUnit entity (IfcSIUnit, IfcContextDependentUnit, etc.).
 * @return Symbol string.
 */
IFCAPI_BINDING std::string unit_get_unit_symbol(express::Base* unit);
/**
 * Convert a numeric value between two IFC unit entities.
 *
 * Reads the Prefix and Name attributes from each unit to perform the
 * conversion.
 *
 * @param value Numeric value to convert.
 * @param from_unit Source IfcUnit entity.
 * @param to_unit Target IfcUnit entity.
 * @return The converted value.
 */
IFCAPI_BINDING double unit_convert_unit(
    double value,
    express::Base* from_unit,
    express::Base* to_unit);
/**
 * Resolve the unit entity attached to a property or quantity.
 *
 * Returns the Unit attribute directly attached to the property (for
 * IfcPropertySingleValue, IfcPhysicalSimpleQuantity, etc.). Returns no result
 * if the unit must be inferred from the measure class.
 *
 * @param prop IfcProperty or IfcPhysicalQuantity entity.
 * @return The attached IfcUnit, or no result if none is attached.
 */
IFCAPI_BINDING express::Base unit_resolve_property_unit(express::Base* prop);
/**
 * Resolve the IFC measure class for a property or quantity.
 *
 * Returns the measure class name (e.g. "IfcLengthMeasure") derived from
 * the property's NominalValue or quantity type. Returns an empty string
 * if a unit entity is directly attached (use unit_resolve_property_unit
 * instead).
 *
 * @param prop IfcProperty or IfcPhysicalQuantity entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
IFCAPI_BINDING std::string unit_resolve_property_measure_class(express::Base* prop);
/**
 * Resolve the defining unit of an IfcPropertyTableValue.
 *
 * Returns the DefiningUnit attribute, or no result if the unit must be
 * inferred from the DefiningValues measure class.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return The DefiningUnit entity, or no result.
 */
IFCAPI_BINDING express::Base unit_resolve_property_table_defining_unit(express::Base* prop);
/**
 * Resolve the defining measure class of an IfcPropertyTableValue.
 *
 * Returns the measure class name derived from the DefiningValues.
 * Returns an empty string if a DefiningUnit entity is directly attached.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
IFCAPI_BINDING std::string unit_resolve_property_table_defining_measure_class(express::Base* prop);
/**
 * Resolve the defined unit of an IfcPropertyTableValue.
 *
 * Returns the DefinedUnit attribute, or no result if the unit must be
 * inferred from the DefinedValues measure class.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return The DefinedUnit entity, or no result.
 */
IFCAPI_BINDING express::Base unit_resolve_property_table_defined_unit(express::Base* prop);
/**
 * Resolve the defined measure class of an IfcPropertyTableValue.
 *
 * Returns the measure class name derived from the DefinedValues.
 * Returns an empty string if a DefinedUnit entity is directly attached.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
IFCAPI_BINDING std::string unit_resolve_property_table_defined_measure_class(express::Base* prop);
/**
 * Calculate the scale factor from SI for a project unit type.
 *
 * Returns the multiplier needed to convert from the project's unit to SI
 * base units. For example, if the project length unit is millimetres,
 * returns 0.001. Defaults to 1.0 when no unit is found.
 *
 * @param file File to query for the project unit.
 * @param unit_type IFC unit type (e.g. "LENGTHUNIT"). Defaults to "LENGTHUNIT" when empty.
 * @return Scale factor to SI.
 */
IFCAPI_BINDING double unit_calculate_unit_scale(ifcopenshell::file* file, const std::string& unit_type);

/**
 * Options for assigning project units.
 *
 * A non-empty explicit units sequence bypasses convenience-unit creation.
 * When units is omitted or empty, length, area, and volume convenience units
 * are created from the corresponding metric flag and raw unit text. Omitted
 * convenience fields default to millimetres, square metres, and cubic metres.
 */
struct UnitAssignUnitOptions {
    /// Explicit units to assign. Omission and an empty sequence both select convenience-unit creation.
    std::optional<std::vector<express::Base>> units = std::nullopt;
    /// Whether the convenience length unit is metric. Omission defaults to true.
    std::optional<bool> length_is_metric = true;
    /// Raw convenience length text. Omission defaults to MILLIMETERS.
    std::optional<std::string> length_raw = "MILLIMETERS";
    /// Whether the convenience area unit is metric. Omission defaults to true.
    std::optional<bool> area_is_metric = true;
    /// Raw convenience area text. Omission defaults to METERS.
    std::optional<std::string> area_raw = "METERS";
    /// Whether the convenience volume unit is metric. Omission defaults to true.
    std::optional<bool> volume_is_metric = true;
    /// Raw convenience volume text. Omission defaults to METERS.
    std::optional<std::string> volume_raw = "METERS";
};

/**
 * Assign explicit or convenience units to the first IfcProject.
 *
 * Reuses an existing IfcUnitAssignment, replaces assigned units with matching
 * UnitType (or the existing monetary unit), preserves unrelated units, and
 * returns the effective assignment. Replaced unit entities remain in the file.
 */
IFCAPI_BINDING express::Base unit_assign_unit(
    ifcopenshell::file* file,
    const UnitAssignUnitOptions& options);

/** Options for creating an IfcConversionBasedUnit. */
struct UnitAddConversionBasedUnitOptions {
    /// Conversion name. Omission defaults to foot; unknown names use USERDEFINED fallback semantics.
    std::optional<std::string> name = "foot";
    /// Explicit nonzero offset. Zero and omission use the built-in offset for the selected name.
    std::optional<double> conversion_offset = std::nullopt;
};

/**
 * Create a conversion-based named unit from the native unit table.
 *
 * The operation creates dimensional exponents, the SI conversion target, an
 * IfcReal conversion value, and an IfcMeasureWithUnit. A nonzero effective
 * offset selects IfcConversionBasedUnitWithOffset when the schema provides it.
 */
IFCAPI_BINDING express::Base unit_add_conversion_based_unit(
    ifcopenshell::file* file,
    const UnitAddConversionBasedUnitOptions& options);

/** Options for editing an IfcNamedUnit. */
struct UnitEditNamedUnitOptions {
    /// IfcSIUnit, IfcConversionBasedUnit, IfcConversionBasedUnitWithOffset, or IfcContextDependentUnit to edit.
    express::Base unit;
    /// Plain attribute property bag. Dimensions accepts up to seven integer exponents; omitted trailing values stay unset.
    ifcopenshell_pset_props_t* attributes;
};

/** One component of an IfcDerivedUnit definition. */
struct UnitDerivedUnitElement {
    /** Named unit used by the derived unit. */
    express::Base unit;
    /** Power to which the named unit is raised. */
    std::int64_t exponent;
};

/** Inputs for creating an IfcDerivedUnit. */
struct UnitAddDerivedUnitOptions {
    /** IFC derived-unit enum value. */
    std::string unit_type;
    /** Name used when unit_type is USERDEFINED. */
    std::optional<std::string> userdefinedtype;
    /** Component units paired with their exponents. */
    std::vector<UnitDerivedUnitElement> elements;
};

/**
 * Edit a named unit without owner-history or predefined-type synchronization.
 *
 * Shared dimensional exponents are copied before editing; uniquely owned
 * dimensions are mutated in place. Remaining attributes are applied in input
 * order after Dimensions has been handled.
 */
IFCAPI_BINDING void unit_edit_named_unit(
    ifcopenshell::file* file,
    const UnitEditNamedUnitOptions& options);
/**
 * Create an IfcSIUnit entity.
 *
 * Sets the UnitType, Name (derived from the unit type), and optional
 * Prefix. When prefix is omitted, the Prefix attribute is left blank.
 *
 * @param file File that receives the new entity.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @param prefix SI prefix (e.g. "KILO", "MILLI"). When omitted, the base unit is used.
 * @return Newly created IfcSIUnit.
 */
IFCAPI_BINDING express::Base unit_add_si_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const char* prefix);
/**
 * Create an IfcMonetaryUnit entity.
 *
 * @param file File that receives the new entity.
 * @param currency ISO 4217 currency code (e.g. "USD", "EUR").
 * @return Newly created IfcMonetaryUnit.
 */
IFCAPI_BINDING express::Base unit_add_monetary_unit(
    ifcopenshell::file* file,
    const std::string& currency);
/**
 * Create an IfcContextDependentUnit entity.
 *
 * Creates a unit with custom dimensional exponents, unit type, and name.
 *
 * @param file File that receives the new entity.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @param name Display name for the unit (e.g. "bag", "each").
 * @param dimensions Dimensional exponents ordered as length, mass, time,
 * electric current, thermodynamic temperature, amount of substance, and
 * luminous intensity.
 * @return Newly created IfcContextDependentUnit.
 */
IFCAPI_BINDING express::Base unit_add_context_dependent_unit(
    ifcopenshell::file* file,
    const std::string& unit_type,
    const std::string& name,
    const Dimensions7& dimensions);
/**
 * Create an IfcDerivedUnit entity.
 *
 * Constructs a derived unit from semantic unit/exponent components (e.g.
 * m/s from [{metre, 1}, {second, -1}]).
 *
 * @param file File that receives the new entity.
 * @param options Unit type, optional user-defined type, and semantic components.
 * @return Newly created IfcDerivedUnit.
 */
IFCAPI_BINDING express::Base unit_add_derived_unit(
    ifcopenshell::file* file,
    const UnitAddDerivedUnitOptions& options);
/**
 * Remove units from the project's IfcUnitAssignment.
 *
 * Removes the given units from the IfcUnitAssignment's Units aggregate.
 * If the aggregate becomes empty, the IfcUnitAssignment entity itself is
 * deleted. The unit entities are not deleted.
 *
 * @param file File containing the unit assignment.
 * @param units Units to unassign.
 */
IFCAPI_BINDING void unit_unassign_unit(
    ifcopenshell::file* file,
    const std::vector<express::Base>& units);
/**
 * Remove a unit entity from the file.
 *
 * Removes the unit from the project's IfcUnitAssignment (if present) and
 * then deletes the unit entity (and its dimensional exponents, if owned).
 *
 * @param file File containing the unit.
 * @param unit IfcUnit entity to remove.
 */
IFCAPI_BINDING void unit_remove_unit(
    ifcopenshell::file* file,
    express::Base* unit);

/** Edit an IfcDerivedUnit using the shared attribute property writer. */
IFCAPI_BINDING void unit_edit_derived_unit(
    ifcopenshell::file* file,
    express::Base unit,
    ifcopenshell_pset_props_t* attributes);

/** Edit an IfcMonetaryUnit using the shared attribute property writer. */
IFCAPI_BINDING void unit_edit_monetary_unit(
    ifcopenshell::file* file,
    express::Base unit,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_UNIT_H */
