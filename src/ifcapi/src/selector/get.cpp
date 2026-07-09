// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selector/internal.h"
#include "ifcapi/express_rules.h"

namespace ifcapi {
namespace bindings {

void value_free(ifcopenshell_selector_value_t* value) {
    delete value;
}

std::optional<ifcopenshell_selector_value_t*> selector_get_element_value(
    std::optional<ifcopenshell::file*> file,
    express::Base* element_ptr,
    const std::string& query)
{
    auto element = ifcapi::detail::deref_or_empty(element_ptr);
    if (!element) {
        ifcopenshell::capi::set_last_error("get_element_value: null argument");
        return {};
    }

    ifcopenshell_selector_node_t* ast = selector_parse_get_element(query);
    if (!ast) return {};

    std::vector<KeyEntry> keys = extract_keys(ast);
    selector_node_free(ast);

    try {
        return get_element_value_impl(file.value_or(nullptr), element, keys);
    } catch (const std::exception& ex) {
        ifcopenshell::capi::set_last_error(ex.what());
        return {};
    } catch (...) {
        ifcopenshell::capi::set_last_error("get_element_value: unknown exception");
        return {};
    }
}

int32_t value_kind(const ifcopenshell_selector_value_t* value) {
    return value ? static_cast<int32_t>(value->kind) : IFCSEL_VALUE_NONE;
}

bool value_as_bool(const ifcopenshell_selector_value_t* value) {
    return value ? value->b_val : false;
}

int64_t value_as_int64(const ifcopenshell_selector_value_t* value) {
    return value ? value->i_val : 0;
}

double value_as_double(const ifcopenshell_selector_value_t* value) {
    return value ? value->d_val : 0.0;
}

std::string value_as_string(const ifcopenshell_selector_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_STRING) ? value->s_val : std::string();
}

express::Base value_as_instance(const ifcopenshell_selector_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_INSTANCE) ? value->inst_val : express::Base();
}

size_t value_list_size(const ifcopenshell_selector_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_LIST) ? value->list_val.size() : 0;
}

const ifcopenshell_selector_value_t* value_list_at(const ifcopenshell_selector_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_LIST || index >= value->list_val.size()) return {};
    return value->list_val[index];
}

size_t value_dict_size(const ifcopenshell_selector_value_t* value) {
    return (value && value->kind == IFCSEL_VALUE_DICT) ? value->dict_val.size() : 0;
}

std::string value_dict_key_at(const ifcopenshell_selector_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_DICT || index >= value->dict_val.size()) return std::string();
    return value->dict_val[index].first;
}

const ifcopenshell_selector_value_t* value_dict_value_at(const ifcopenshell_selector_value_t* value, size_t index) {
    if (!value || value->kind != IFCSEL_VALUE_DICT || index >= value->dict_val.size()) return {};
    return value->dict_val[index].second;
}

} // namespace bindings
} // namespace ifcapi
