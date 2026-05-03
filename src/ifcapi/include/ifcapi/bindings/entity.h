/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void entity_remove_deep2(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING void entity_remove_deep2_ex(
    IfcUtil::IfcBaseClass* instance,
    const std::vector<const IfcUtil::IfcBaseClass*>& also_consider,
    const std::vector<const IfcUtil::IfcBaseClass*>& do_not_delete);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ENTITY_H */
