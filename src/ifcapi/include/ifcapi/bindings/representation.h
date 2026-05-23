/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_REPRESENTATION_H
#define IFCAPI_BINDINGS_REPRESENTATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/aggregate_of_instance.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* representation_get_context(
    IfcParse::IfcFile* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view);
IFCAPI_BINDING IfcUtil::IfcBaseClass* representation_resolve(IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING IfcUtil::IfcBaseClass* representation_get_product_representation(
    IfcUtil::IfcBaseClass* element,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* context,
    const char* context_type,
    const char* subcontext,
    const char* target_view);
IFCAPI_BINDING aggregate_of_instance::ptr representation_resolve_base_items(IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING aggregate_of_instance::ptr representation_get_prioritised_contexts(IfcParse::IfcFile* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_REPRESENTATION_H */
