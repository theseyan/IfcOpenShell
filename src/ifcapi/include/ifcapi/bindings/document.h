// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DOCUMENT_H
#define IFCAPI_BINDINGS_DOCUMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base document_add_information(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* parent,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base document_add_reference(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* information);
IFCAPI_BINDING express::Base document_assign_document(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* document,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void document_unassign_document(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* document,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void document_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);
IFCAPI_BINDING void document_remove_information(
    ifcopenshell::file* file,
    express::Base* information);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DOCUMENT_H */
