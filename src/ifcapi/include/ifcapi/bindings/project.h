// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROJECT_H
#define IFCAPI_BINDINGS_PROJECT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning declarations to a project context.
 */
struct ProjectAssignDeclarationOptions {
    /// Objects to declare to the project or project library.
    std::vector<express::Base> definitions;
    /// The IfcProject or IfcProjectLibrary context.
    express::Base relating_context;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning declarations from a project context.
 */
struct ProjectUnassignDeclarationOptions {
    /// Objects to remove from the declaration.
    std::vector<express::Base> definitions;
    /// The IfcProject or IfcProjectLibrary context.
    express::Base relating_context;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Declare objects to a project or project library context.
 *
 * Creates or updates an IfcRelDeclares relationship linking the given
 * definitions to the relating context. If the definitions are already
 * declared to a different context, they are moved.
 */
IFCAPI_BINDING express::Base project_assign_declaration(
    ifcopenshell::file* file,
    const ProjectAssignDeclarationOptions& options);

/**
 * Remove declarations of objects from a project or project library context.
 *
 * Removes the definitions from any existing IfcRelDeclares relationship.
 * If no definitions remain, the relationship itself is removed.
 */
IFCAPI_BINDING void project_unassign_declaration(
    ifcopenshell::file* file,
    const ProjectUnassignDeclarationOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PROJECT_H */
