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
 * Release a selector value.
 *
 * @param value Selector value to release.
 */
void value_free(ifcopenshell_selector_value_t* value);

/**
 * Create a value representing no value.
 *
 * @return New selector value representing no value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_none();

/**
 * Create a boolean value.
 *
 * @param value The boolean value.
 * @return New boolean selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_bool(bool value);

/**
 * Create a 64-bit integer value.
 *
 * @param value The integer value.
 * @return New integer selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_int(int64_t value);

/**
 * Create a double-precision floating-point value.
 *
 * @param value The double value.
 * @return New double selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_double(double value);

/**
 * Create a string value.
 *
 * @param value The string value.
 * @return New string selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_string(const std::string& value);

/**
 * Create an instance (entity reference) value.
 *
 * When value is omitted, creates a value representing no value.
 *
 * @param value The IFC entity instance.
 * @return New entity-reference selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_instance(std::optional<express::Base> value);

/**
 * Create an empty list value.
 *
 * @return New list selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_list();

/**
 * Create an empty mapping value.
 *
 * @return New mapping selector value. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_dict();

/**
 * Append an item to a list value.
 *
 * @param list The list selector value.
 * @param item The item to append. When omitted, appends a no-value entry.
 * @return True if the item was appended.
 */
IFCAPI_BINDING bool value_list_append(
    ifcopenshell_selector_value_t* list,
    std::optional<const ifcopenshell_selector_value_t*> item);

/**
 * Set a key-value pair in a mapping value.
 *
 * @param dict The mapping value.
 * @param key The string key.
 * @param value The value to associate with the key. When omitted, assigns a no-value entry.
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
 * @return The extracted value, or no result if the query cannot be evaluated. Release it with value_free.
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
 * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
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
 * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
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
 * @param value The value to set. When omitted, the target is unset.
 * @param concat When provided and non-empty, it is prepended to the value.
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
 * @return The computed value, or no result if it cannot be computed. Release it with value_free.
 */
IFCAPI_BINDING IFCAPI_OWNED std::optional<ifcopenshell_selector_value_t*> compute_derived(
    express::Base* instance,
    const std::string& attribute_name);

/**
 * Register an IFC file for schema-aware derived-value evaluation.
 *
 * The registered file is used when evaluating derived attributes for the
 * specified schema.
 *
 * @param schema_name The IFC schema identifier (e.g. "IFC4").
 * @param file The IFC file to register.
 * @return True after the file is registered.
 */
IFCAPI_BINDING bool register_scratch_file(const char* schema_name, ifcopenshell::file* file);

/**
 * Return the kind of a selector value.
 *
 * @param value The selector value.
 * @return The value kind, or the no-value kind when no value is provided.
 */
IFCAPI_BINDING int32_t value_kind(const ifcopenshell_selector_value_t* value);

/**
 * Extract a boolean from a selector value.
 *
 * @param value The selector value.
 * @return The boolean value, or false when no value is provided or the value is not boolean.
 */
IFCAPI_BINDING bool value_as_bool(const ifcopenshell_selector_value_t* value);

/**
 * Extract a 64-bit integer from a selector value.
 *
 * @param value The selector value.
 * @return The integer value, or 0 when no value is provided or the value is not an integer.
 */
IFCAPI_BINDING int64_t value_as_int64(const ifcopenshell_selector_value_t* value);

/**
 * Extract a double from a selector value.
 *
 * @param value The selector value.
 * @return The double value, or 0.0 when no value is provided or the value is not a double.
 */
IFCAPI_BINDING double value_as_double(const ifcopenshell_selector_value_t* value);

/**
 * Extract a string from a selector value.
 *
 * @param value The selector value.
 * @return The string value, or an empty string when no value is provided or the value is not a string.
 */
IFCAPI_BINDING std::string value_as_string(const ifcopenshell_selector_value_t* value);

/**
 * Extract an entity instance from a selector value.
 *
 * @param value The selector value.
 * @return The IFC entity, or no result when no value is provided or the value is not an entity.
 */
IFCAPI_BINDING express::Base value_as_instance(const ifcopenshell_selector_value_t* value);

/**
 * Return the number of items in a list value.
 *
 * @param value The selector value.
 * @return Number of items, or 0 when no value is provided or the value is not a list.
 */
IFCAPI_BINDING size_t value_list_size(const ifcopenshell_selector_value_t* value);

/**
 * Return an item from a list value at the given index.
 *
 * @param value The list value.
 * @param index Zero-based item index.
 * @return The item at the index, or no result if the index is out of range or the value is not a list.
 */
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_list_at(const ifcopenshell_selector_value_t* value, size_t index);

/**
 * Return the number of entries in a mapping value.
 *
 * @param value The selector value.
 * @return Number of entries, or 0 when no value is provided or the value is not a mapping.
 */
IFCAPI_BINDING size_t value_dict_size(const ifcopenshell_selector_value_t* value);

/**
 * Return the key at the given index in a mapping value.
 *
 * @param value The mapping value.
 * @param index Zero-based entry index.
 * @return The key string, or an empty string if the index is out of range or the value is not a mapping.
 */
IFCAPI_BINDING std::string value_dict_key_at(const ifcopenshell_selector_value_t* value, size_t index);

/**
 * Return the value at the given index in a mapping value.
 *
 * @param value The mapping value.
 * @param index Zero-based entry index.
 * @return The value at the index, or no result if the index is out of range or the value is not a mapping.
 */
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_dict_value_at(const ifcopenshell_selector_value_t* value, size_t index);

} // namespace bindings
} // namespace ifcapi

#endif
