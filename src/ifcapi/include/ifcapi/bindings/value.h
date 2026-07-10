/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_VALUE_H
#define IFCAPI_BINDINGS_VALUE_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/value.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace IfcUtil {
class IfcBaseClass;
}

namespace IfcParse {
class IfcFile;
}

namespace ifcapi {
namespace bindings {

/**
 * Free a selector value handle.
 *
 * @param value Value handle to free.
 */
void value_free(ifcopenshell_selector_value_t* value);

/**
 * Create a None value.
 *
 * @return New None value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_none();

/**
 * Create a boolean value.
 *
 * @param value The boolean value.
 * @return New boolean value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_bool(bool value);

/**
 * Create a 64-bit integer value.
 *
 * @param value The integer value.
 * @return New integer value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_int(int64_t value);

/**
 * Create a double-precision floating-point value.
 *
 * @param value The double value.
 * @return New double value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_double(double value);

/**
 * Create a string value.
 *
 * @param value The string value.
 * @return New string value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_string(const std::string& value);

/**
 * Create an instance (entity reference) value.
 *
 * If value is empty, creates a None value.
 *
 * @param value The IFC entity instance.
 * @return New instance value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_instance(std::optional<express::Base> value);

/**
 * Create an empty list value.
 *
 * @return New list value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_list();

/**
 * Create an empty dictionary value.
 *
 * @return New dict value handle. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_dict();

/**
 * Append an item to a list value.
 *
 * @param list The list value handle.
 * @param item The item to append. If empty, appends a None value.
 * @return True if the item was appended.
 */
IFCAPI_BINDING bool value_list_append(
    ifcopenshell_selector_value_t* list,
    std::optional<const ifcopenshell_selector_value_t*> item);

/**
 * Set a key-value pair in a dictionary value.
 *
 * @param dict The dict value handle.
 * @param key The string key.
 * @param value The value to associate with the key. If empty, sets a None value.
 * @return True if the key-value pair was set.
 */
IFCAPI_BINDING bool value_dict_set(
    ifcopenshell_selector_value_t* dict,
    const std::string& key,
    std::optional<const ifcopenshell_selector_value_t*> value);

/**
 * Get a value from an element using a selector query.
 *
 * Parses the query as a get-element key path and extracts the
 * corresponding value from the element's attributes.
 *
 * @param file Optional IFC file context.
 * @param element The element to query.
 * @param query The selector key path (e.g. "Name", "Pset_WallCommon.FireRating").
 * @return The extracted value, or empty on error. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED std::optional<ifcopenshell_selector_value_t*> selector_get_element_value(
    std::optional<ifcopenshell::file*> file,
    express::Base* element,
    const std::string& query);

/**
 * Filter all elements in a file using a selector query.
 *
 * Parses the query as a filter expression and returns all matching
 * elements as a list value.
 *
 * @param file The IFC file to search.
 * @param query The filter query string.
 * @return List value of matching elements, or empty on error. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED std::optional<ifcopenshell_selector_value_t*> selector_filter_all(
    ifcopenshell::file* file,
    const std::string& query);

/**
 * Filter a list of elements using a selector query.
 *
 * Parses the query as a filter expression and returns the subset
 * of elements that match.
 *
 * @param file The IFC file context.
 * @param query The filter query string.
 * @param elements The elements to filter.
 * @return List value of matching elements, or empty on error. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED std::optional<ifcopenshell_selector_value_t*> selector_filter_elements(
    ifcopenshell::file* file,
    const std::string& query,
    const std::vector<express::Base>& elements);

/**
 * Set a value on an element using a selector query.
 *
 * Parses the query to identify the target attribute (e.g. a property
 * set property or direct attribute) and sets it to the provided value.
 *
 * @param file The IFC file context.
 * @param element The element to modify.
 * @param query The selector key path identifying the target.
 * @param value The value to set. If empty, unsets the target.
 * @param concat If non-null and non-empty, concatenated with the value as a prefix.
 */
IFCAPI_BINDING void selector_set_element_value(
    ifcopenshell::file* file,
    std::optional<express::Base> element,
    const std::string& query,
    std::optional<const ifcopenshell_selector_value_t*> value,
    const char* concat);

/**
 * Compute a derived attribute value for an instance.
 *
 * Evaluates derived attributes defined in the IFC schema (e.g.
 * IfcDirection.DirectionRatios) for the given instance and attribute.
 *
 * @param instance The entity instance.
 * @param attribute_name The name of the derived attribute.
 * @return The computed value, or empty on error. Free with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED std::optional<ifcopenshell_selector_value_t*> compute_derived(
    express::Base* instance,
    const std::string& attribute_name);

/**
 * Register a scratch file for a given schema.
 *
 * Registers a temporary IFC file for the specified schema name,
 * used internally for schema-aware operations.
 *
 * @param schema_name The IFC schema identifier (e.g. "IFC4").
 * @param file The IFC file to register.
 * @return True if registration succeeded.
 */
IFCAPI_BINDING bool register_scratch_file(const char* schema_name, ifcopenshell::file* file);

/**
 * Return the kind of a selector value.
 *
 * @param value The value handle.
 * @return One of the ifcopenshell_selector_value_kind_t values, or IFCSEL_VALUE_NONE if null.
 */
IFCAPI_BINDING int32_t value_kind(const ifcopenshell_selector_value_t* value);

/**
 * Extract a boolean from a selector value.
 *
 * @param value The value handle.
 * @return The boolean value, or false if null or not a boolean.
 */
IFCAPI_BINDING bool value_as_bool(const ifcopenshell_selector_value_t* value);

/**
 * Extract a 64-bit integer from a selector value.
 *
 * @param value The value handle.
 * @return The integer value, or 0 if null or not an integer.
 */
IFCAPI_BINDING int64_t value_as_int64(const ifcopenshell_selector_value_t* value);

/**
 * Extract a double from a selector value.
 *
 * @param value The value handle.
 * @return The double value, or 0.0 if null or not a double.
 */
IFCAPI_BINDING double value_as_double(const ifcopenshell_selector_value_t* value);

/**
 * Extract a string from a selector value.
 *
 * @param value The value handle.
 * @return The string value, or empty string if null or not a string.
 */
IFCAPI_BINDING std::string value_as_string(const ifcopenshell_selector_value_t* value);

/**
 * Extract an entity instance from a selector value.
 *
 * @param value The value handle.
 * @return The entity instance, or empty if null or not an instance.
 */
IFCAPI_BINDING express::Base value_as_instance(const ifcopenshell_selector_value_t* value);

/**
 * Return the number of items in a list value.
 *
 * @param value The value handle.
 * @return Number of items, or 0 if null or not a list.
 */
IFCAPI_BINDING size_t value_list_size(const ifcopenshell_selector_value_t* value);

/**
 * Return an item from a list value at the given index.
 *
 * @param value The list value handle.
 * @param index Zero-based item index.
 * @return The item at the index, or null if out of range or not a list.
 */
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_list_at(const ifcopenshell_selector_value_t* value, size_t index);

/**
 * Return the number of entries in a dictionary value.
 *
 * @param value The value handle.
 * @return Number of entries, or 0 if null or not a dict.
 */
IFCAPI_BINDING size_t value_dict_size(const ifcopenshell_selector_value_t* value);

/**
 * Return the key at the given index in a dictionary value.
 *
 * @param value The dict value handle.
 * @param index Zero-based entry index.
 * @return The key string, or empty if out of range or not a dict.
 */
IFCAPI_BINDING std::string value_dict_key_at(const ifcopenshell_selector_value_t* value, size_t index);

/**
 * Return the value at the given index in a dictionary value.
 *
 * @param value The dict value handle.
 * @param index Zero-based entry index.
 * @return The value at the index, or null if out of range or not a dict.
 */
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_dict_value_at(const ifcopenshell_selector_value_t* value, size_t index);

} // namespace bindings
} // namespace ifcapi

#endif
