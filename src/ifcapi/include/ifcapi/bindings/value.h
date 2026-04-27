/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_VALUE_H
#define IFCAPI_BINDINGS_VALUE_H

#include "ifcapi/value.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace IfcUtil {
class IfcBaseClass;
}

namespace IfcParse {
class IfcFile;
}

namespace ifcapi {
namespace bindings {

void value_free(ifcopenshell_value_t* value);
ifcopenshell_value_t* selector_get_element_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const std::string& query);
ifcopenshell_value_t* compute_derived(IfcUtil::IfcBaseClass* instance, const std::string& attribute_name);
int32_t value_kind(const ifcopenshell_value_t* value);
bool value_as_bool(const ifcopenshell_value_t* value);
int64_t value_as_int64(const ifcopenshell_value_t* value);
double value_as_double(const ifcopenshell_value_t* value);
std::string value_as_string(const ifcopenshell_value_t* value);
IfcUtil::IfcBaseClass* value_as_instance(const ifcopenshell_value_t* value);
size_t value_list_size(const ifcopenshell_value_t* value);
const ifcopenshell_value_t* value_list_at(const ifcopenshell_value_t* value, size_t index);
size_t value_dict_size(const ifcopenshell_value_t* value);
std::string value_dict_key_at(const ifcopenshell_value_t* value, size_t index);
const ifcopenshell_value_t* value_dict_value_at(const ifcopenshell_value_t* value, size_t index);

} // namespace bindings
} // namespace ifcapi

#endif
