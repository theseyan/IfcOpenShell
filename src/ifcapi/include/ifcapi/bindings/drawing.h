// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DRAWING_H
#define IFCAPI_BINDINGS_DRAWING_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base drawing_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void drawing_unassign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DRAWING_H */
