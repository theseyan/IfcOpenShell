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

const char* attribute_get_primitive_type(const ifcopenshell::attribute* attribute);
std::vector<std::string> attribute_get_enum_items(const ifcopenshell::attribute* attribute);

/**
 * Options for editing attributes of a product.
 */
struct AttributeEditAttributesOptions {
    /// The product whose attributes are being edited.
    express::Base product;
    /// Attribute property bag to apply.
    ifcopenshell_pset_props_t* attributes;
    /// Whether to synchronize PredefinedType based on ElementType/ObjectType.
    bool sync_predefined_type;
    /// Whether to update OwnerHistory.
    bool update_owner_history;
    /// Optional user for owner history updates.
    std::optional<express::Base> user;
    /// Optional application for owner history updates.
    std::optional<express::Base> application;
};

IFCAPI_BINDING void attribute_edit_attributes(
    ifcopenshell::file* file,
    const AttributeEditAttributesOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ATTRIBUTE_H */
