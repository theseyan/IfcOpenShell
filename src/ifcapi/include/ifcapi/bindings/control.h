/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTROL_H
#define IFCAPI_BINDINGS_CONTROL_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base control_assign_control(
    ifcopenshell::file* file,
    express::Base* relating_control,
    const std::vector<express::Base>& related_objects,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void control_unassign_control(
    ifcopenshell::file* file,
    express::Base* relating_control,
    const std::vector<express::Base>& related_objects,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTROL_H */
