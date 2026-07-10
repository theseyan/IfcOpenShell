// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LIBRARY_H
#define IFCAPI_BINDINGS_LIBRARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/// Options for assigning a library reference to products.
struct LibraryAssignReferenceOptions {
    /// Products to associate with the library reference.
    std::vector<express::Base> products;
    /// The IfcLibraryReference to assign.
    express::Base reference;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/// Options for unassigning a library reference from products.
struct LibraryUnassignReferenceOptions {
    /// The IfcLibraryReference to unassign.
    express::Base reference;
    /// Products to disassociate from the reference.
    std::vector<express::Base> products;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Create a new IfcLibraryInformation with the given name.
 */
IFCAPI_BINDING express::Base library_add_library(
    ifcopenshell::file* file,
    const std::string& name);

/**
 * Create a new IfcLibraryReference linked to a library.
 *
 * In IFC4+, the reference's ReferencedLibrary attribute is set. In IFC2X3,
 * the reference is appended to the library's LibraryReference aggregate.
 */
IFCAPI_BINDING express::Base library_add_reference(
    ifcopenshell::file* file,
    express::Base* library);

/**
 * Assign a library reference to products via IfcRelAssociatesLibrary.
 *
 * Products already associated with the reference are skipped. If an
 * existing relationship is present, new products are merged into it.
 */
IFCAPI_BINDING express::Base library_assign_reference(
    ifcopenshell::file* file,
    const LibraryAssignReferenceOptions& options);

/**
 * Remove products from an IfcRelAssociatesLibrary relationship.
 *
 * If no products remain after removal, the relationship is deleted.
 */
IFCAPI_BINDING void library_unassign_reference(
    ifcopenshell::file* file,
    const LibraryUnassignReferenceOptions& options);

/**
 * Remove an IfcLibraryReference and its association relationships.
 *
 * Deletes the reference entity and all IfcRelAssociatesLibrary relationships
 * that reference it.
 */
IFCAPI_BINDING void library_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);

/**
 * Remove an IfcLibraryInformation and all its references.
 *
 * Deletes all child IfcLibraryReference entities, the library entity itself,
 * and all associated IfcRelAssociatesLibrary relationships.
 */
IFCAPI_BINDING void library_remove_library(
    ifcopenshell::file* file,
    express::Base* library);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LIBRARY_H */
