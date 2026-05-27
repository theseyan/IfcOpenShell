// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LIBRARY_H
#define IFCAPI_BINDINGS_LIBRARY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base library_add_library(
    ifcopenshell::file* file,
    const std::string& name);
IFCAPI_BINDING express::Base library_add_reference(
    ifcopenshell::file* file,
    express::Base* library);
IFCAPI_BINDING express::Base library_assign_reference(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* reference,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void library_unassign_reference(
    ifcopenshell::file* file,
    express::Base* reference,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void library_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference);
IFCAPI_BINDING void library_remove_library(
    ifcopenshell::file* file,
    express::Base* library);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LIBRARY_H */
