/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ENTITY_H
#define IFCAPI_BINDINGS_ENTITY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING bool entity_set_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr,
    const std::string& type_name,
    const char* str_value);
IFCAPI_BINDING std::vector<std::string> entity_get_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr);
IFCAPI_BINDING bool entity_set_aggregate_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr,
    const std::vector<std::string>& type_names,
    const std::vector<std::string>& str_values);
IFCAPI_BINDING std::vector<std::string> entity_get_aggregate_typed_value(
    IfcUtil::IfcBaseClass* instance,
    const std::string& attr);
IFCAPI_BINDING void entity_remove_deep2(IfcUtil::IfcBaseClass* instance);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ENTITY_H */
