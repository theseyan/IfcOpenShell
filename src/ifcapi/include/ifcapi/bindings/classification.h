/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CLASSIFICATION_H
#define IFCAPI_BINDINGS_CLASSIFICATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/aggregate_of_instance.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING aggregate_of_instance::ptr classification_get_references(
    IfcUtil::IfcBaseClass* element,
    bool should_inherit);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CLASSIFICATION_H */

