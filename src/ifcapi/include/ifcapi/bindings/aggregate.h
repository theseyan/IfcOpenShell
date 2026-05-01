/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_AGGREGATE_H
#define IFCAPI_BINDINGS_AGGREGATE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* aggregate_assign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* relating_object);
IFCAPI_BINDING void aggregate_unassign_object(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_AGGREGATE_H */
