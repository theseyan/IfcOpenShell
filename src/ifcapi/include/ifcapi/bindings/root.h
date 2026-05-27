/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ROOT_H
#define IFCAPI_BINDINGS_ROOT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base root_create_entity(
    ifcopenshell::file* file,
    const std::string& ifc_class,
    const char* predefined_type,
    const char* name,
    IFCAPI_NULLABLE express::Base* owner_history);
IFCAPI_BINDING void root_remove_product(
    ifcopenshell::file* file,
    express::Base* product,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ROOT_H */
