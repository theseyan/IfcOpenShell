/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <vector>

namespace ifcapi {
namespace bindings {

struct EntityRemoveDeepOptions {
    std::vector<express::Base> also_consider;
    std::vector<express::Base> do_not_delete;
};

IFCAPI_BINDING void entity_remove_deep(express::Base* instance);
IFCAPI_BINDING void entity_remove_deep_with_options(
    express::Base* instance,
    const EntityRemoveDeepOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ENTITY_H */
