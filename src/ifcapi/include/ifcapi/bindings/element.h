/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ELEMENT_H
#define IFCAPI_BINDINGS_ELEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_type(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_aggregate(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_nest(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_container(
    IfcUtil::IfcBaseClass* instance,
    bool direct_only,
    const char* ifc_class);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_parent(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_material(
    IfcUtil::IfcBaseClass* instance,
    bool should_skip_usage,
    bool should_inherit);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ELEMENT_H */
