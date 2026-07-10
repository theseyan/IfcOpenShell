/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ATTRIBUTE_H
#define IFCAPI_BINDINGS_ATTRIBUTE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Return the primitive type name of a schema attribute.
 *
 * Possible values include "string", "float", "integer", "boolean",
 * "enum", "entity", "select", "aggregate", "binary", and "unknown".
 *
 * @param attribute Schema attribute declaration.
 * @return A static string describing the primitive type.
 */
const char* attribute_get_primitive_type(const ifcopenshell::attribute* attribute);

/**
 * Return the enumeration items of a schema attribute.
 *
 * If the attribute is not an enumeration type, returns an empty list.
 *
 * @param attribute Schema attribute declaration.
 * @return Ordered list of enumeration value names.
 */
std::vector<std::string> attribute_get_enum_items(const ifcopenshell::attribute* attribute);

/**
 * Options for editing attributes of a product.
 */
struct AttributeEditAttributesOptions {
    /// Product whose attributes are being edited.
    express::Base product;
    /// Property bag of attribute name/value pairs to apply to the product.
    ifcopenshell_pset_props_t* attributes;
    /// When true, synchronize PredefinedType based on ElementType or ObjectType.
    bool sync_predefined_type;
    /// When true, update the product's OwnerHistory after modifying attributes.
    bool update_owner_history;
    /// IfcPersonAndOrganization used for OwnerHistory updates. Omit to use the file default.
    std::optional<express::Base> user;
    /// IfcApplication used for OwnerHistory updates. Omit to use the file default.
    std::optional<express::Base> application;
};

/**
 * Edit arbitrary attributes on a product.
 *
 * Applies the attribute values from the property bag to the product.
 * Optionally synchronizes PredefinedType when ElementType or ObjectType
 * changes, and optionally updates OwnerHistory.
 *
 * @param file The IFC file containing the product.
 * @param options Attribute editing options.
 */
IFCAPI_BINDING void attribute_edit_attributes(
    ifcopenshell::file* file,
    const AttributeEditAttributesOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ATTRIBUTE_H */
