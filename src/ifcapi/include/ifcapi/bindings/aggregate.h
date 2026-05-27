/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_AGGREGATE_H
#define IFCAPI_BINDINGS_AGGREGATE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base aggregate_assign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* relating_object,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void aggregate_unassign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_AGGREGATE_H */
