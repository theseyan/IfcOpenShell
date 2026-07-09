// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COGO_H
#define IFCAPI_BINDINGS_COGO_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/file.h"

#include <optional>

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a survey point to the model.
 */
struct CogoAddSurveyPointOptions {
    express::Base survey_point;
    std::optional<express::Base> site;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Add a single survey point as an IfcAnnotation.
 */
IFCAPI_BINDING express::Base cogo_add_survey_point(
    ifcopenshell::file* file,
    const CogoAddSurveyPointOptions& options);

IFCAPI_BINDING void cogo_assign_survey_point(
    express::Base* annotation,
    express::Base* survey_point);

IFCAPI_BINDING void cogo_edit_survey_point(express::Base* annotation, double x, double y, double z);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COGO_H */
