// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selector/internal.h"
#include "ifcapi/express_rules.h"

namespace ifcapi {
namespace bindings {

void value_free(ifcopenshell_value_t* value) {
    delete value;
}

ifcopenshell_value_t* selector_get_element_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const std::string& query)
{
    if (!element) {
        ifcopenshell::capi::set_last_error("get_element_value: null argument");
        return nullptr;
    }

    ifcopenshell_selector_node_t* ast = selector_parse_get_element(query);
    if (!ast) return nullptr;

    std::vector<KeyEntry> keys = extract_keys(ast);
    selector_node_free(ast);

    try {
        return get_element_value_impl(file, element, keys);
    } catch (const std::exception& ex) {
        ifcopenshell::capi::set_last_error(ex.what());
        return nullptr;
    } catch (...) {
        ifcopenshell::capi::set_last_error("get_element_value: unknown exception");
        return nullptr;
    }
}

int32_t value_kind(const ifcopenshell_value_t* value) {
    return value ? static_cast<int32_t>(value->kind) : IFCSEL_VALUE_NONE;
}

bool value_as_bool(const ifcopenshell_value_t* value) {
    return value ? value->b_val : false;
}

int64_t value_as_int64(const ifcopenshell_value_t* value) {
    return value ? value->i_val : 0;
}

double value_as_double(const ifcopenshell_value_t* value) {
    return value ? value->d_val : 0.0;
}

std::string value_as_string(const ifcopenshell_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_STRING) ? value->s_val : std::string();
}

IfcUtil::IfcBaseClass* value_as_instance(const ifcopenshell_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_INSTANCE) ? value->inst_val : nullptr;
}

size_t value_list_size(const ifcopenshell_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_LIST) ? value->list_val.size() : 0;
}

const ifcopenshell_value_t* value_list_at(const ifcopenshell_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_LIST || index >= value->list_val.size()) return nullptr;
    return value->list_val[index];
}

size_t value_dict_size(const ifcopenshell_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_DICT) ? value->dict_val.size() : 0;
}

std::string value_dict_key_at(const ifcopenshell_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_DICT || index >= value->dict_val.size()) return std::string();
    return value->dict_val[index].first;
}

const ifcopenshell_value_t* value_dict_value_at(const ifcopenshell_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_DICT || index >= value->dict_val.size()) return nullptr;
    return value->dict_val[index].second;
}

} // namespace bindings
} // namespace ifcapi
