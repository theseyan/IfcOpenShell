/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <vector>

namespace ifcapi {
namespace bindings {

/// Options for deep removal of an entity.
struct EntityRemoveDeepOptions {
    /// Additional entities to consider as part of the subgraph when checking inverse references.
    std::vector<express::Base> also_consider;
    /// Entities that must not be deleted even if they are part of the subgraph.
    std::vector<express::Base> do_not_delete;
};

/**
 * Recursively remove an entity and its owned subgraph.
 *
 * Equivalent to entity_remove_deep_with_options with empty options.
 * The start element must have no inverses outside the subgraph.
 *
 * @param instance The root entity to remove.
 */
IFCAPI_BINDING void entity_remove_deep(express::Base* instance);

/**
 * Recursively remove an entity and its owned subgraph with fine-grained control.
 *
 * Traverses forward through the entity's subgraph. Each subelement is
 * deleted only if it has fewer than two inverse references, or all of
 * its inverses are within the subgraph. The also_consider list extends
 * the subgraph for inverse checking. The do_not_delete list protects
 * specific entities from deletion.
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
