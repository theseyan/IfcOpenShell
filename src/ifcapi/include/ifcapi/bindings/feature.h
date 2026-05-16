/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_FEATURE_H
#define IFCAPI_BINDINGS_FEATURE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* feature_add_feature(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* feature,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* feature_add_filling(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* opening,
    IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING void feature_remove_feature(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* feature,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void feature_remove_filling(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_FEATURE_H */
