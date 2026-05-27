/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTEXT_H
#define IFCAPI_BINDINGS_CONTEXT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base context_add_context(
    ifcopenshell::file* file,
    const char* context_type,
    const char* context_identifier,
    const char* target_view,
    bool has_target_scale,
    double target_scale,
    IFCAPI_NULLABLE express::Base* parent);
IFCAPI_BINDING void context_remove_context(ifcopenshell::file* file, express::Base* context);
IFCAPI_BINDING void context_edit_context(
    ifcopenshell::file* file,
    express::Base* context,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTEXT_H */
