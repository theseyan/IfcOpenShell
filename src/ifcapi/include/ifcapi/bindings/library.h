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
    std::vector<express::Base> products;
    express::Base reference;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/// Options for unassigning a library reference from products.
struct LibraryUnassignReferenceOptions {
    express::Base reference;
    std::vector<express::Base> products;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING express::Base library_add_library(
    ifcopenshell::file* file,
    const std::string& name);
IFCAPI_BINDING express::Base library_add_reference(
    ifcopenshell::file* file,
    express::Base* library);

/// Assign a library reference to a list of products.
IFCAPI_BINDING express::Base library_assign_reference(
    ifcopenshell::file* file,
    const LibraryAssignReferenceOptions& options);

/// Unassign a library reference from a list of products.
IFCAPI_BINDING void library_unassign_reference(
    ifcopenshell::file* file,
    const LibraryUnassignReferenceOptions& options);

IFCAPI_BINDING void library_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);
IFCAPI_BINDING void library_remove_library(
    ifcopenshell::file* file,
    express::Base* library);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LIBRARY_H */
