/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PSET_H
#define IFCAPI_BINDINGS_PSET_H

#include "ifcapi/bindings/contract.h"

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
 * Options for adding a property set to an object.
 */
struct PsetAddPsetOptions {
    /// The object to attach the property set to (IfcObject, IfcContext,
    /// IfcTypeObject, IfcMaterial, IfcMaterialDefinition, or IfcProfileDef).
    express::Base product;
    /// Name of the property set (e.g. "Pset_WallCommon").
    std::string name;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
    /// Optional IFC2X3 subclass for material/profile property sets (e.g.
    /// "IfcExtendedMaterialProperties"). Defaults to "IfcExtendedMaterialProperties"
    /// for materials and "IfcGeneralProfileProperties" for profiles.
    std::optional<std::string> ifc2x3_subclass;
};

/**
 * Options for adding a quantity set to an object.
 */
struct PsetAddQtoOptions {
    /// The object to attach the quantity set to (IfcObject, IfcContext, or IfcTypeObject).
    express::Base product;
    /// Name of the quantity set (e.g. "BaseQuantities").
    std::string name;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for editing a property set.
 */
struct PsetEditPsetOptions {
    /// The IfcPropertySet (or IfcMaterialProperties, IfcProfileProperties, etc.) to edit.
    express::Base pset;
    /// Optional new name for the property set.
    std::optional<std::string> name;
    /// Property name-to-value entries to set. Properties not mentioned remain
    /// unchanged. Use pset_props_set_null to assign a blank property value.
    ifcopenshell_pset_props_t* properties = nullptr;
    /// Optional IfcPropertySetTemplate to use for type inference. If omitted,
    /// the template is looked up by the property set's Name attribute.
    std::optional<express::Base> pset_template;
    /// If true (default), blank-valued properties are removed from the set.
    /// If false, blank-valued properties retain a blank NominalValue.
    std::optional<bool> should_purge = true;
};

/**
 * Options for editing a quantity set.
 */
struct PsetEditQtoOptions {
    /// The IfcElementQuantity (or IfcPhysicalComplexQuantity) to edit.
    express::Base qto;
    /// Optional new name for the quantity set.
    std::optional<std::string> name;
    /// Quantity key-value pairs to set. Keys are quantity names. Scalar values
    /// are set as IfcPhysicalSimpleQuantity subtypes (inferred from name/value).
    /// Mapping values with a "Discrimination" key create IfcPhysicalComplexQuantity.
    ifcopenshell_pset_props_t* properties = nullptr;
    /// Optional IfcPropertySetTemplate to use for quantity type inference.
    /// If omitted, the template is looked up by the quantity set's Name attribute.
    std::optional<express::Base> qto_template;
};

/**
 * Options for assigning a property set to products.
 */
struct PsetAssignPsetOptions {
    /// Products to associate with the property set. IfcTypeProduct instances
    /// are added to HasPropertySets; others are linked via IfcRelDefinesByProperties.
    std::vector<express::Base> products;
    /// The property set to assign.
    express::Base pset;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unsharing a property set across products.
 */
struct PsetUnsharePsetOptions {
    /// Products that should receive their own copy of the property set.
    /// Must be a subset of the products currently sharing the pset.
    std::vector<express::Base> products;
    /// The shared property set to unshare.
    express::Base pset;
    /// Optional owner history for the new IfcRelDefinesByProperties relationships.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Allocate a new property builder. Release it with pset_props_free when done.
 */
IFCAPI_BINDING ifcopenshell_pset_props_t* pset_props_new();

/**
 * Release a property builder allocated by pset_props_new.
 */
IFCAPI_BINDING void pset_props_free(ifcopenshell_pset_props_t* props);

/**
 * Set a property to a blank value. When editing, the property is removed if
 * should_purge is true; otherwise its NominalValue is set to blank.
 */
IFCAPI_BINDING void pset_props_set_null(ifcopenshell_pset_props_t* props, const std::string& key);

/**
 * Set a boolean property value.
 */
IFCAPI_BINDING void pset_props_set_bool(ifcopenshell_pset_props_t* props, const std::string& key, bool value);

/**
 * Set a list-of-booleans property value.
 */
IFCAPI_BINDING void pset_props_set_bool_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<bool>& values);

/**
 * Set an integer property value.
 */
IFCAPI_BINDING void pset_props_set_int(ifcopenshell_pset_props_t* props, const std::string& key, int64_t value);

/**
 * Set a double property value.
 */
IFCAPI_BINDING void pset_props_set_double(ifcopenshell_pset_props_t* props, const std::string& key, double value);

/**
 * Set a string property value. The IFC type is inferred as IfcLabel.
 */
IFCAPI_BINDING void pset_props_set_string(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::string& value);

/**
 * Set a property to an existing IFC entity (for example, a typed value or
 * an IfcProperty). When omitted, the property is cleared.
 */
IFCAPI_BINDING void pset_props_set_instance(
    ifcopenshell_pset_props_t* props, const std::string& key, std::optional<express::Base> value);

/**
 * Set a string property with an explicit IFC type (e.g. "IfcURIReference").
 */
IFCAPI_BINDING void pset_props_set_typed_string(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::string& value, const std::string& ifc_type);

/**
 * Set a double property with an explicit IFC type (e.g. "IfcLengthMeasure").
 */
IFCAPI_BINDING void pset_props_set_typed_double(
    ifcopenshell_pset_props_t* props, const std::string& key, double value, const std::string& ifc_type);

/**
 * Set an integer property with an explicit IFC type (e.g. "IfcInteger").
 */
IFCAPI_BINDING void pset_props_set_typed_int(
    ifcopenshell_pset_props_t* props, const std::string& key, int64_t value, const std::string& ifc_type);

/**
 * Set a boolean property with an explicit IFC type (e.g. "IfcBoolean").
 */
IFCAPI_BINDING void pset_props_set_typed_bool(
    ifcopenshell_pset_props_t* props, const std::string& key, bool value, const std::string& ifc_type);

/**
 * Set a list-of-strings property value (creates IfcPropertyListValue).
 */
IFCAPI_BINDING void pset_props_set_string_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<std::string>& values);

/**
 * Set a list-of-doubles property value (creates IfcPropertyListValue).
 */
IFCAPI_BINDING void pset_props_set_double_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<double>& values);

/**
 * Set a list-of-integers property value (creates IfcPropertyListValue).
 */
IFCAPI_BINDING void pset_props_set_int_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<int64_t>& values);

/**
 * Set a list-of-instances property value (creates IfcPropertyListValue).
 */
IFCAPI_BINDING void pset_props_set_instance_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<express::Base>& values);

/**
 * Set a date property value (IfcCalendarDate / IfcDate).
 */
IFCAPI_BINDING void pset_props_set_date(
    ifcopenshell_pset_props_t* props, const std::string& key, int year, int month, int day);

/**
 * Set a date-time property value (IfcLocalTime / IfcDateTime).
 */
IFCAPI_BINDING void pset_props_set_datetime(
    ifcopenshell_pset_props_t* props, const std::string& key, int year, int month, int day, int hour, int minute, int second,
    int microsecond, bool has_timezone, int timezone_offset_minutes);

/**
 * Set a duration property value (IfcDuration).
 */
IFCAPI_BINDING void pset_props_set_duration(
    ifcopenshell_pset_props_t* props, const std::string& key, bool negative, int years, int months, int days, int hours,
    int minutes, int seconds, int microseconds);

/**
 * Set a nested mapping property. Ownership of the nested property data is
 * transferred to the outer data; do not release it separately. Used for
 * IfcPhysicalComplexQuantity in quantity sets.
 */
IFCAPI_BINDING void pset_props_set_dict(
    ifcopenshell_pset_props_t* outer, const std::string& key, ifcopenshell_pset_props_t* inner);

/**
 * Attach a unit to the most recently added property entry.
 *
 * Sets the Unit attribute on the resulting IfcPropertySingleValue. When
 * omitted, the unit is cleared.
 */
IFCAPI_BINDING void pset_props_set_unit_for_last(
    ifcopenshell_pset_props_t* props, std::optional<express::Base> unit);

/**
 * Add or return an existing property set attached to an object, context, type,
 * material, or profile definition.
 *
 * For IfcObject/IfcContext, creates IfcRelDefinesByProperties. For
 * IfcTypeObject, appends to HasPropertySets. For IfcMaterial, creates
 * IfcMaterialProperties (or IfcExtendedMaterialProperties in IFC2X3).
 * For IfcProfileDef, creates IfcProfileProperties (or
 * IfcGeneralProfileProperties in IFC2X3). If a property set with the
 * same name already exists on the product, it is returned without
 * creating a new one.
 */
IFCAPI_BINDING express::Base pset_add_pset(
    ifcopenshell::file* file,
    const PsetAddPsetOptions& options);

/**
 * Add or return an existing quantity set (IfcElementQuantity) on an object.
 *
 * If the name ends with "BaseQuantities", MethodOfMeasurement is set to
 * "BaseQuantities". If a quantity set with the same name already exists,
 * it is returned without creating a new one.
 */
IFCAPI_BINDING express::Base pset_add_qto(
    ifcopenshell::file* file,
    const PsetAddQtoOptions& options);

/**
 * Edit properties on an existing property set.
 *
 * Updates existing properties in-place (when not shared with other psets),
 * adds new properties for keys not yet present, and optionally removes
 * blank-valued properties. Uses the pset template for type inference when
 * available. Returns true on success, false on error.
 */
IFCAPI_BINDING bool pset_edit_pset(
    ifcopenshell::file* file,
    const PsetEditPsetOptions& options);

/**
 * Edit quantities on an existing quantity set.
 *
 * Updates existing quantities in-place, adds new quantities for keys not
 * yet present. Quantity types (Length, Area, Volume, Count, Weight, Time)
 * are inferred from the value type and name. Returns true on success,
 * false on error.
 */
IFCAPI_BINDING bool pset_edit_qto(
    ifcopenshell::file* file,
    const PsetEditQtoOptions& options);

/**
 * Assign a property set to products.
 *
 * For IfcTypeProduct instances, the pset is appended to HasPropertySets.
 * For other products, creates or updates an IfcRelDefinesByProperties
 * relationship.
 */
IFCAPI_BINDING express::Base pset_assign_pset(
    ifcopenshell::file* file,
    const PsetAssignPsetOptions& options);

/**
 * Remove products from a property set's associations.
 *
 * For IfcTypeProduct instances, removes the pset from HasPropertySets.
 * For other products, removes them from IfcRelDefinesByProperties
 * relationships. Orphaned relationships are deleted. The pset itself is
 * not deleted.
 */
IFCAPI_BINDING void pset_unassign_pset(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* pset);

/**
 * Remove a property set from a specific product.
 *
 * Removes the IfcRelDefinesByProperties relationship between the product
 * and pset. If the pset has no remaining associations, the pset and its
 * properties are deleted.
 */
IFCAPI_BINDING void pset_remove_pset(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* pset);

/**
 * Unshare a property set by creating independent copies for specified products.
 *
 * When all assigned products are selected, one product retains the original
 * and the other products receive copies. When only some products are
 * selected, each selected product receives a copy and the original remains
 * assigned to the unselected products. Returns the newly created copies.
 */
IFCAPI_BINDING std::vector<express::Base> pset_unshare_pset(
    ifcopenshell::file* file,
    const PsetUnsharePsetOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PSET_H */
