/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_TYPE_H
#define IFCAPI_BINDINGS_TYPE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base type_assign_type(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    express::Base* relating_type,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base type_assign_type_ex(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    express::Base* relating_type,
    bool should_map_representations,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING bool type_map_type_representations(
    ifcopenshell::file* file,
    express::Base* related_object,
    express::Base* relating_type);
IFCAPI_BINDING void type_unassign_type(
    ifcopenshell::file* file,
    const std::vector<express::Base>& objects,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_TYPE_H */
