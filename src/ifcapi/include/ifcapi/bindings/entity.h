/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <vector>

namespace ifcapi {
namespace bindings {

/// Options for removing an entity and its owned subgraph.
struct EntityRemoveDeepOptions {
    /// Additional entities to include when determining whether references are external to the subgraph.
    std::vector<express::Base> also_consider;
    /// Entities to preserve even when they belong to the subgraph.
    std::vector<express::Base> do_not_delete;
};

/**
 * Remove an entity and the unshared entities it owns, recursively.
 *
 * Equivalent to entity_remove_deep_with_options with the default options.
 * The start element must have no inverses outside the subgraph.
 *
 * @param instance The root entity to remove.
 */
IFCAPI_BINDING void entity_remove_deep(express::Base* instance);

/**
 * Remove an entity and its owned subgraph with fine-grained control.
 *
 * An owned entity is removed only when it has no references from outside the
 * removal set. The also_consider list extends that set for this decision, and
 * the do_not_delete list protects specific entities from deletion.
 *
 * @param instance The root entity to remove.
 * @param options Additional control over the removal process.
 */
IFCAPI_BINDING void entity_remove_deep_with_options(
    express::Base* instance,
    const EntityRemoveDeepOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ENTITY_H */
