/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ATTRIBUTE_H
#define IFCAPI_BINDINGS_ATTRIBUTE_H

#include "ifcapi/bindings/contract.h"

#include <string>
#include <vector>

namespace IfcParse {
class attribute;
}

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING const char* attribute_get_primitive_type(const IfcParse::attribute* attribute);
IFCAPI_BINDING std::vector<std::string> attribute_get_enum_items(const IfcParse::attribute* attribute);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ATTRIBUTE_H */

