/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ROOT_H
#define IFCAPI_BINDINGS_ROOT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* root_create_entity(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const char* predefined_type,
    const char* name,
    IfcUtil::IfcBaseClass* owner_history);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ROOT_H */
