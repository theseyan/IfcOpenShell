// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COGO_H
#define IFCAPI_BINDINGS_COGO_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/IfcFile.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* cogo_add_survey_point(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* survey_point,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* site,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void cogo_assign_survey_point(
    IfcUtil::IfcBaseClass* annotation,
    IfcUtil::IfcBaseClass* survey_point);
IFCAPI_BINDING void cogo_edit_survey_point(IfcUtil::IfcBaseClass* annotation, double x, double y, double z);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COGO_H */
