/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CONTEXT_H
#define IFCAPI_BINDINGS_CONTEXT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* context_add_context(
    IfcParse::IfcFile* file,
    const char* context_type,
    const char* context_identifier,
    const char* target_view,
    bool has_target_scale,
    double target_scale,
    IfcUtil::IfcBaseClass* parent);
IFCAPI_BINDING void context_remove_context(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* context);
IFCAPI_BINDING void context_edit_context(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONTEXT_H */
