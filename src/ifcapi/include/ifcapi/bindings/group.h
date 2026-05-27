/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GROUP_H
#define IFCAPI_BINDINGS_GROUP_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base group_add_group(
    ifcopenshell::file* file,
    const std::string& name,
    const char* description,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base group_update_group_products(
    ifcopenshell::file* file,
    express::Base* group,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base group_assign_group(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* group,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void group_unassign_group(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* group,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void group_remove_group(
    ifcopenshell::file* file,
    express::Base* group);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GROUP_H */
