/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ATTRIBUTE_H
#define IFCAPI_BINDINGS_ATTRIBUTE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

const char* attribute_get_primitive_type(const ifcopenshell::attribute* attribute);
std::vector<std::string> attribute_get_enum_items(const ifcopenshell::attribute* attribute);
IFCAPI_BINDING void attribute_edit_attributes(
    ifcopenshell::file* file,
    express::Base* product,
    ifcopenshell_pset_props_t* attributes,
    bool sync_predefined_type,
    bool update_owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ATTRIBUTE_H */
