// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_CLASSIFICATION_H
#define IFCAPI_BINDINGS_CLASSIFICATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<express::Base> classification_get_references(
    express::Base* element,
    bool should_inherit);

IFCAPI_BINDING express::Base classification_add_classification(
    ifcopenshell::file* file,
    const std::string& name);

/**
 * Options for adding a classification reference to products.
 */
struct ClassificationAddReferenceOptions {
    /// Products to associate with the classification reference.
    std::vector<express::Base> products;
    /// An existing classification reference from a library. If provided, identification/name are ignored.
    std::optional<express::Base> reference;
    /// Optional identification code for a new reference (e.g. "Pr_12_23_34").
    std::optional<std::string> identification;
    /// Optional human-readable name for a new reference.
    std::optional<std::string> name;
    /// Optional IfcClassification that the reference belongs to.
    std::optional<express::Base> classification;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing a classification reference from products.
 */
struct ClassificationRemoveReferenceOptions {
    /// The classification reference to disassociate.
    express::Base reference;
    /// Products to disassociate from the reference.
    std::vector<express::Base> products;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Add a classification reference and associate it with products.
 *
 * If an existing reference handle is provided, it is used directly.
 * Otherwise, a new IfcClassificationReference is created using the
 * optional identification, name, and classification fields.
 */
IFCAPI_BINDING express::Base classification_add_reference(
    ifcopenshell::file* file,
    const ClassificationAddReferenceOptions& options);

/**
 * Remove a classification reference association from products.
 *
 * If the reference is no longer associated with any products after
 * removal, the reference entity itself is also deleted.
 */
IFCAPI_BINDING void classification_remove_reference(
    ifcopenshell::file* file,
    const ClassificationRemoveReferenceOptions& options);

IFCAPI_BINDING void classification_remove_classification(
    ifcopenshell::file* file,
    express::Base* classification);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CLASSIFICATION_H */
