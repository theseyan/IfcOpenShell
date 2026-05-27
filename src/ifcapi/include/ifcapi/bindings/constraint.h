// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_CONSTRAINT_H
#define IFCAPI_BINDINGS_CONSTRAINT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base constraint_add_objective(ifcopenshell::file* file);
IFCAPI_BINDING express::Base constraint_add_metric(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* objective);
IFCAPI_BINDING IFCAPI_OWNED std::vector<express::Base> constraint_add_metric_reference(
    ifcopenshell::file* file,
    express::Base* metric,
    const std::string& reference_path);
IFCAPI_BINDING express::Base constraint_assign_constraint(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* constraint,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void constraint_unassign_constraint(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* constraint,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void constraint_remove_constraint(
    ifcopenshell::file* file,
    express::Base* constraint);
IFCAPI_BINDING void constraint_remove_metric(
    ifcopenshell::file* file,
    express::Base* metric);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONSTRAINT_H */
