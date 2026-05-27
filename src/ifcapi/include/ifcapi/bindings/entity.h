/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void entity_remove_deep2(express::Base* instance);
IFCAPI_BINDING void entity_remove_deep2_ex(
    express::Base* instance,
    const std::vector<express::Base>& also_consider,
    const std::vector<express::Base>& do_not_delete);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ENTITY_H */
