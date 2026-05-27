// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COGO_H
#define IFCAPI_BINDINGS_COGO_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/file.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base cogo_add_survey_point(
    ifcopenshell::file* file,
    express::Base* survey_point,
    IFCAPI_NULLABLE express::Base* site,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void cogo_assign_survey_point(
    express::Base* annotation,
    express::Base* survey_point);
IFCAPI_BINDING void cogo_edit_survey_point(express::Base* annotation, double x, double y, double z);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COGO_H */
