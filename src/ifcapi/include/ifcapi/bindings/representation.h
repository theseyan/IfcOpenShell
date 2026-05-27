/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_REPRESENTATION_H
#define IFCAPI_BINDINGS_REPRESENTATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base representation_get_context(
    ifcopenshell::file* file,
    const char* context_type,
    const char* subcontext,
    const char* target_view);
IFCAPI_BINDING express::Base representation_resolve(express::Base* representation);
IFCAPI_BINDING express::Base representation_get_product_representation(
    express::Base* element,
    IFCAPI_NULLABLE express::Base* context,
    const char* context_type,
    const char* subcontext,
    const char* target_view);
IFCAPI_BINDING std::vector<express::Base> representation_resolve_base_items(express::Base* representation);
IFCAPI_BINDING std::vector<express::Base> representation_get_prioritised_contexts(ifcopenshell::file* file);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_REPRESENTATION_H */
