/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTEXT_H
#define IFCAPI_BINDINGS_CONTEXT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a geometric representation context or subcontext.
 */
struct ContextAddContextOptions {
    std::string context_type;
    std::string context_identifier;
    std::string target_view;
    std::optional<double> target_scale;
    std::optional<express::Base> parent;
};

IFCAPI_BINDING express::Base context_add_context(
    ifcopenshell::file* file,
    const ContextAddContextOptions& options);
IFCAPI_BINDING void context_remove_context(ifcopenshell::file* file, express::Base* context);
IFCAPI_BINDING void context_edit_context(
    ifcopenshell::file* file,
    express::Base* context,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTEXT_H */
