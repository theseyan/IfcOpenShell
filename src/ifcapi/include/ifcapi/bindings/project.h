// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_PROJECT_H
#define IFCAPI_BINDINGS_PROJECT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

struct ProjectAppendAssetCache;

/// A deterministic source-identity to target-entity entry returned by an
/// append-asset cache.
struct ProjectAppendAssetCacheEntry {
    std::int64_t source_identity;
    express::Base target;
};

/// Deterministic parallel representation of all valid cache entries.
struct ProjectAppendAssetCacheEntries {
    std::vector<std::int64_t> source_identities;
    std::vector<std::int64_t> source_ids;
    std::vector<std::string> source_types;
    std::vector<express::Base> targets;
};

/**
 * Options for appending a reusable asset from a library file.
 *
 * `library` is the source IFC file and `file` is the target IFC file passed to
 * project_append_asset. The optional owned cache carries semantic source
 * identity mappings across calls; it must be disposed before the target file.
 * The target file must outlive the cache. Native code validates and prunes the
 * cache after removals, and a cache may not be shared between target files
 * while it contains mappings.
 *
 * The operation copies only the selected asset's forward graph and the
 * supported inverse relationships. Existing roots are reused by GlobalId,
 * and the uniqueness flag enables the upstream name/equivalence reuse rules.
 * Length measures are converted using the source and target project units;
 * complete mixed-unit model conversion is not generally promised.
 */
struct ProjectAppendAssetOptions {
    /// Source/library file containing the asset.
    ifcopenshell::file* library;
    /// Asset in the source/library file to append.
    express::Base element;
    /// Optional reusable native identity cache.
    std::optional<ProjectAppendAssetCache*> cache = std::nullopt;
    /// Reuse supported named/equivalent assets when true.
    std::optional<bool> assume_asset_uniqueness_by_name = true;
};

/// Allocate an empty reusable append-asset cache.
IFCAPI_BINDING IFCAPI_OWNED ProjectAppendAssetCache* project_append_asset_cache_new();

/// Dispose an append-asset cache. The target file must outlive the cache.
IFCAPI_BINDING void project_append_asset_cache_free(ProjectAppendAssetCache* cache);

/// Seed/update one semantic source-entity mapping in an append-asset cache.
IFCAPI_BINDING void project_append_asset_cache_set(
    ProjectAppendAssetCache* cache,
    express::Base source,
    express::Base target);

/// Return valid cache mappings in ascending source-identity order.
IFCAPI_BINDING ProjectAppendAssetCacheEntries project_append_asset_cache_entries(
    ProjectAppendAssetCache* cache);

/**
 * Append one supported asset from a source/library file into the target file.
 *
 * Returns the existing or newly copied target asset, or an empty value for an
 * unsupported entity class. Native code owns graph traversal, inverse
 * filtering, deduplication, context replacement, placement correction, type
 * assignment, unit conversion, and reusable-cache cleanup.
 */
IFCAPI_BINDING std::optional<express::Base> project_append_asset(
    ifcopenshell::file* file,
    const ProjectAppendAssetOptions& options);

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
