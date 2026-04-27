// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selector/internal.h"
#include "ifcapi/express_rules.h"

extern "C" ifcopenshell_value_t* ifcopenshell_selector_get_element_value(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    const char* query);

namespace ifcapi {
namespace bindings {

void value_free(ifcopenshell_value_t* value) {
    ifcopenshell_value_free(value);
}

ifcopenshell_value_t* selector_get_element_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const std::string& query)
{
    ifcopenshell_ifc_file_t file_handle{file, false};
    ifcopenshell_ifc_instance_t element_handle{element, false};
    return ifcopenshell_selector_get_element_value(file ? &file_handle : nullptr, &element_handle, query.c_str());
}

ifcopenshell_value_t* compute_derived(IfcUtil::IfcBaseClass* instance, const std::string& attribute_name) {
    if (instance == nullptr) {
        return nullptr;
    }
    ifcopenshell_ifc_instance_t* handle = ifcopenshell::capi::wrap_instance(instance, false);
    ifcopenshell_value_t* result = ifcopenshell_compute_derived(handle, attribute_name.c_str());
    delete handle;
    return result;
}

int32_t value_kind(const ifcopenshell_value_t* value) {
    return static_cast<int32_t>(ifcopenshell_value_kind(value));
}

bool value_as_bool(const ifcopenshell_value_t* value) {
    return ifcopenshell_value_as_bool(value);
}

int64_t value_as_int64(const ifcopenshell_value_t* value) {
    return ifcopenshell_value_as_int64(value);
}

double value_as_double(const ifcopenshell_value_t* value) {
    return ifcopenshell_value_as_double(value);
}

std::string value_as_string(const ifcopenshell_value_t* value) {
    const char* result = ifcopenshell_value_as_string(value);
    return result ? std::string(result) : std::string();
}

IfcUtil::IfcBaseClass* value_as_instance(const ifcopenshell_value_t* value) {
    ifcopenshell_ifc_instance_t* handle = ifcopenshell_value_as_instance(value);
    if (handle == nullptr) {
        return nullptr;
    }
    IfcUtil::IfcBaseClass* instance = handle->ptr;
    delete handle;
    return instance;
}

size_t value_list_size(const ifcopenshell_value_t* value) {
    return ifcopenshell_value_list_size(value);
}

const ifcopenshell_value_t* value_list_at(const ifcopenshell_value_t* value, size_t index) {
    return ifcopenshell_value_list_at(value, index);
}

size_t value_dict_size(const ifcopenshell_value_t* value) {
    return ifcopenshell_value_dict_size(value);
}

std::string value_dict_key_at(const ifcopenshell_value_t* value, size_t index) {
    const char* result = ifcopenshell_value_dict_key_at(value, index);
    return result ? std::string(result) : std::string();
}

const ifcopenshell_value_t* value_dict_value_at(const ifcopenshell_value_t* value, size_t index) {
    return ifcopenshell_value_dict_value_at(value, index);
}

} // namespace bindings
} // namespace ifcapi

/* ====================================================================
 *  C ABI — value accessors
 * ==================================================================== */

extern "C" {

void ifcopenshell_value_free(ifcopenshell_value_t* v) {
    delete v;
}

ifcopenshell_value_kind_t ifcopenshell_value_kind(const ifcopenshell_value_t* v) {
    return v ? v->kind : IFCSEL_VALUE_NONE;
}

bool ifcopenshell_value_as_bool(const ifcopenshell_value_t* v) {
    return v ? v->b_val : false;
}
int64_t ifcopenshell_value_as_int64(const ifcopenshell_value_t* v) {
    return v ? v->i_val : 0;
}
double ifcopenshell_value_as_double(const ifcopenshell_value_t* v) {
    return v ? v->d_val : 0.0;
}
const char* ifcopenshell_value_as_string(const ifcopenshell_value_t* v) {
    return (v && v->kind == IFCSEL_VALUE_STRING) ? v->s_val.c_str() : nullptr;
}

ifcopenshell_ifc_instance_t* ifcopenshell_value_as_instance(const ifcopenshell_value_t* v) {
    if (!v || v->kind != IFCSEL_VALUE_INSTANCE || !v->inst_val) return nullptr;
    return ifcopenshell::capi::wrap_instance(v->inst_val, false);
}

size_t ifcopenshell_value_list_size(const ifcopenshell_value_t* v) {
    return (v && v->kind == IFCSEL_VALUE_LIST) ? v->list_val.size() : 0;
}
const ifcopenshell_value_t* ifcopenshell_value_list_at(const ifcopenshell_value_t* v, size_t idx) {
    if (!v || v->kind != IFCSEL_VALUE_LIST || idx >= v->list_val.size()) return nullptr;
    return v->list_val[idx];
}

size_t ifcopenshell_value_dict_size(const ifcopenshell_value_t* v) {
    return (v && v->kind == IFCSEL_VALUE_DICT) ? v->dict_val.size() : 0;
}
const char* ifcopenshell_value_dict_key_at(const ifcopenshell_value_t* v, size_t idx) {
    if (!v || v->kind != IFCSEL_VALUE_DICT || idx >= v->dict_val.size()) return nullptr;
    return v->dict_val[idx].first.c_str();
}
const ifcopenshell_value_t* ifcopenshell_value_dict_value_at(const ifcopenshell_value_t* v, size_t idx) {
    if (!v || v->kind != IFCSEL_VALUE_DICT || idx >= v->dict_val.size()) return nullptr;
    return v->dict_val[idx].second;
}

/* ---- Main entry point ---- */

ifcopenshell_value_t* ifcopenshell_selector_get_element_value(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* element,
    const char* query)
{
    if (!element || !element->ptr || !query) {
        ifcopenshell::capi::set_last_error("get_element_value: null argument");
        return nullptr;
    }
    IfcParse::IfcFile* ifc_file = file ? file->ptr : nullptr;

    ifcopenshell_selector_node_t* ast = ifcopenshell_selector_parse_get_element(query);
    if (!ast) return nullptr;

    std::vector<KeyEntry> keys = extract_keys(ast);
    ifcopenshell_selector_node_free(ast);

    try {
        return get_element_value_impl(ifc_file, element->ptr, keys);
    } catch (const std::exception& ex) {
        ifcopenshell::capi::set_last_error(ex.what());
        return nullptr;
    } catch (...) {
        ifcopenshell::capi::set_last_error("get_element_value: unknown exception");
        return nullptr;
    }
}

} /* extern "C" */
