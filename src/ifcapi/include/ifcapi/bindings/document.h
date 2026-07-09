// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DOCUMENT_H
#define IFCAPI_BINDINGS_DOCUMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/// Options for adding a new document information entity.
struct DocumentAddInformationOptions {
    std::optional<express::Base> parent;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/// Options for assigning a document to products.
struct DocumentAssignDocumentOptions {
    std::vector<express::Base> products;
    express::Base document;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/// Options for unassigning a document from products.
struct DocumentUnassignDocumentOptions {
    std::vector<express::Base> products;
    express::Base document;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/// Add a new document information to the project.
IFCAPI_BINDING express::Base document_add_information(
    ifcopenshell::file* file,
    const DocumentAddInformationOptions& options);

/// Create a new reference to a document.
IFCAPI_BINDING express::Base document_add_reference(
    ifcopenshell::file* file,
    std::optional<express::Base> information);

/// Assign a document to a list of products.
IFCAPI_BINDING express::Base document_assign_document(
    ifcopenshell::file* file,
    const DocumentAssignDocumentOptions& options);

/// Unassign a document from a list of products.
IFCAPI_BINDING void document_unassign_document(
    ifcopenshell::file* file,
    const DocumentUnassignDocumentOptions& options);

IFCAPI_BINDING void document_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);
IFCAPI_BINDING void document_remove_information(
    ifcopenshell::file* file,
    express::Base* information);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DOCUMENT_H */
