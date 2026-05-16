/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CLASSIFICATION_H
#define IFCAPI_BINDINGS_CLASSIFICATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/aggregate_of_instance.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING aggregate_of_instance::ptr classification_get_references(
    IfcUtil::IfcBaseClass* element,
    bool should_inherit);
IFCAPI_BINDING IfcUtil::IfcBaseClass* classification_add_classification(
    IfcParse::IfcFile* file,
    const std::string& name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* classification_add_reference(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference,
    const std::string& identification,
    bool has_identification,
    const std::string& name,
    bool has_name,
    IfcUtil::IfcBaseClass* classification,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void classification_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void classification_remove_classification(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* classification);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CLASSIFICATION_H */
