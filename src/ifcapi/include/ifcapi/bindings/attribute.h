/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ATTRIBUTE_H
#define IFCAPI_BINDINGS_ATTRIBUTE_H

#include "ifcapi/bindings/contract.h"

#include <string>
#include <vector>

namespace IfcParse {
class attribute;
class IfcFile;
}

namespace IfcUtil {
class IfcBaseClass;
}

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

const char* attribute_get_primitive_type(const IfcParse::attribute* attribute);
std::vector<std::string> attribute_get_enum_items(const IfcParse::attribute* attribute);
IFCAPI_BINDING void attribute_edit_attributes(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    ifcopenshell_pset_props_t* attributes,
    bool sync_predefined_type,
    bool update_owner_history,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* user,
    IFCAPI_NULLABLE IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ATTRIBUTE_H */
