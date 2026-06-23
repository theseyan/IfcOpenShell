/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_VALUE_H
#define IFCAPI_BINDINGS_VALUE_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/value.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace IfcUtil {
class IfcBaseClass;
}

namespace IfcParse {
class IfcFile;
}

namespace ifcapi {
namespace bindings {

void value_free(ifcopenshell_selector_value_t* value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_none();
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_bool(bool value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_int(int64_t value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_double(double value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_string(const std::string& value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_instance(IFCAPI_NULLABLE express::Base* value);
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_selector_value_t* value_new_list();
IFCAPI_BINDING bool value_list_append(ifcopenshell_selector_value_t* list, IFCAPI_NULLABLE const ifcopenshell_selector_value_t* item);
IFCAPI_BINDING IFCAPI_OWNED IFCAPI_NULLABLE ifcopenshell_selector_value_t* selector_get_element_value(
    IFCAPI_NULLABLE ifcopenshell::file* file,
    express::Base* element,
    const std::string& query);
IFCAPI_BINDING IFCAPI_OWNED IFCAPI_NULLABLE ifcopenshell_selector_value_t* selector_filter_all(
    ifcopenshell::file* file,
    const std::string& query);
IFCAPI_BINDING IFCAPI_OWNED IFCAPI_NULLABLE ifcopenshell_selector_value_t* selector_filter_elements(
    ifcopenshell::file* file,
    const std::string& query,
    const std::vector<express::Base>& elements);
IFCAPI_BINDING bool selector_set_element_value(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* element,
    const std::vector<std::string>& keys,
    const std::vector<bool>& regex_flags,
    IFCAPI_NULLABLE const ifcopenshell_selector_value_t* value,
    const char* concat);
IFCAPI_BINDING IFCAPI_OWNED IFCAPI_NULLABLE ifcopenshell_selector_value_t* compute_derived(
    express::Base* instance,
    const std::string& attribute_name);
IFCAPI_BINDING bool register_scratch_file(const char* schema_name, size_t file_ptr);
IFCAPI_BINDING int32_t value_kind(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING bool value_as_bool(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING int64_t value_as_int64(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING double value_as_double(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING std::string value_as_string(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING express::Base value_as_instance(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING size_t value_list_size(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_list_at(const ifcopenshell_selector_value_t* value, size_t index);
IFCAPI_BINDING size_t value_dict_size(const ifcopenshell_selector_value_t* value);
IFCAPI_BINDING std::string value_dict_key_at(const ifcopenshell_selector_value_t* value, size_t index);
IFCAPI_BINDING const ifcopenshell_selector_value_t* value_dict_value_at(const ifcopenshell_selector_value_t* value, size_t index);

} // namespace bindings
} // namespace ifcapi

#endif
