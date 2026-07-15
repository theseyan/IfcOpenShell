// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DOCUMENT_H
#define IFCAPI_BINDINGS_DOCUMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/// Options for adding a new document information entity.
struct DocumentAddInformationOptions {
    /// Optional parent entity. Can be an IfcProject, IfcContext, or IfcDocumentInformation.
    /// If omitted, defaults to the first IfcProject.
    std::optional<express::Base> parent;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/// Options for assigning a document to products.
struct DocumentAssignDocumentOptions {
    /// Products to associate with the document.
    std::vector<express::Base> products;
    /// The IfcDocumentInformation or IfcDocumentReference to assign.
    express::Base document;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/// Options for unassigning a document from products.
struct DocumentUnassignDocumentOptions {
    /// Products to disassociate from the document.
    std::vector<express::Base> products;
    /// The IfcDocumentInformation or IfcDocumentReference to unassign.
    express::Base document;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Create a new IfcDocumentInformation and link it to a parent.
 *
 * If the parent is an IfcProject or IfcContext, the document is linked via
 * IfcRelAssociatesDocument. If the parent is another IfcDocumentInformation,
 * the new document is linked as a child via IfcDocumentInformationRelationship.
 * If parent is omitted, the first IfcProject is used.
 */
IFCAPI_BINDING express::Base document_add_information(
    ifcopenshell::file* file,
    const DocumentAddInformationOptions& options);

/**
 * Create a new IfcDocumentReference.
 *
 * If an IfcDocumentInformation is provided, the reference is linked to it
 * (via ReferencedDocument in IFC4+ or DocumentReferences in IFC2X3).
 */
IFCAPI_BINDING express::Base document_add_reference(
    ifcopenshell::file* file,
    std::optional<express::Base> information);

/**
 * Assign a document (IfcDocumentInformation or IfcDocumentReference) to products.
 *
 * Creates or updates an IfcRelAssociatesDocument relationship. Products
 * already associated with the document are skipped.
 */
IFCAPI_BINDING express::Base document_assign_document(
    ifcopenshell::file* file,
    const DocumentAssignDocumentOptions& options);

/**
 * Remove products from a document association.
 *
 * Products are removed from IfcRelAssociatesDocument relationships. If no
 * products remain in a relationship, the relationship is deleted.
 */
IFCAPI_BINDING void document_unassign_document(
    ifcopenshell::file* file,
    const DocumentUnassignDocumentOptions& options);

/**
 * Remove an IfcDocumentReference and its association relationships.
 *
 * Deletes the reference entity and all IfcRelAssociatesDocument relationships
 * that reference it.
 */
IFCAPI_BINDING void document_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);

/**
 * Remove an IfcDocumentInformation and its entire document tree.
 *
 * Recursively deletes all child IfcDocumentReference entities, child
 * IfcDocumentInformation entities (via IsPointer relationships), and all
 * associated IfcRelAssociatesDocument relationships.
 */
IFCAPI_BINDING void document_remove_information(
    ifcopenshell::file* file,
    express::Base* information);

/** Edit an IfcDocumentInformation using the shared attribute property writer. */
IFCAPI_BINDING void document_edit_information(
    ifcopenshell::file* file,
    express::Base information,
    ifcopenshell_pset_props_t* attributes);

/** Edit an IfcDocumentReference using the shared attribute property writer. */
IFCAPI_BINDING void document_edit_reference(
    ifcopenshell::file* file,
    express::Base reference,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DOCUMENT_H */
