// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

namespace {

inline const IfcParse::schema_definition* as_schema(const void* p) {
    return reinterpret_cast<const IfcParse::schema_definition*>(p);
}
inline const IfcParse::declaration* as_decl(const void* p) {
    return reinterpret_cast<const IfcParse::declaration*>(p);
}
inline const IfcParse::entity* as_entity_(const void* p) {
    return reinterpret_cast<const IfcParse::entity*>(p);
}
inline const IfcParse::type_declaration* as_type_(const void* p) {
    return reinterpret_cast<const IfcParse::type_declaration*>(p);
}
inline const IfcParse::enumeration_type* as_enum_(const void* p) {
    return reinterpret_cast<const IfcParse::enumeration_type*>(p);
}
inline const IfcParse::select_type* as_select_(const void* p) {
    return reinterpret_cast<const IfcParse::select_type*>(p);
}
inline const IfcParse::attribute* as_attr(const void* p) {
    return reinterpret_cast<const IfcParse::attribute*>(p);
}
inline const IfcParse::inverse_attribute* as_iattr(const void* p) {
    return reinterpret_cast<const IfcParse::inverse_attribute*>(p);
}
inline const IfcParse::parameter_type* as_pt(const void* p) {
    return reinterpret_cast<const IfcParse::parameter_type*>(p);
}

template <typename T>
const void** alloc_ptr_array(const std::vector<T>& v, uint32_t* out_count) {
    if (out_count) *out_count = static_cast<uint32_t>(v.size());
    if (v.empty()) return nullptr;
    const void** arr = static_cast<const void**>(std::malloc(sizeof(void*) * v.size()));
    if (!arr) return nullptr;
    for (size_t i = 0; i < v.size(); ++i) {
        arr[i] = reinterpret_cast<const void*>(v[i]);
    }
    return arr;
}

} // namespace

extern "C" {

IFCAPI_EXPORT void ifcopenshell_free_ptr_array(void* arr) {
    std::free(arr);
}

/* --- schema_definition --- */

IFCAPI_EXPORT const void* ifcopenshell_schema_by_name(const char* name) {
    if (!name) return nullptr;
    try {
        return IfcParse::schema_by_name(std::string(name));
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

IFCAPI_EXPORT const void* ifcopenshell_file_schema_def(const ifcopenshell_ifc_file_t* file) {
    if (!file) return nullptr;
    auto* f = file->ptr;
    return f->schema();
}

IFCAPI_EXPORT const char* ifcopenshell_schema_name(const void* schema) {
    auto* s = as_schema(schema);
    if (!s) return nullptr;
    return s->name().c_str();
}

IFCAPI_EXPORT const void* ifcopenshell_schema_declaration_by_name(const void* schema, const char* name) {
    auto* s = as_schema(schema);
    if (!s || !name) return nullptr;
    try {
        return s->declaration_by_name(std::string(name));
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

IFCAPI_EXPORT const void** ifcopenshell_schema_declarations(const void* schema, uint32_t* out_count) {
    auto* s = as_schema(schema);
    if (!s) { if (out_count) *out_count = 0; return nullptr; }
    return alloc_ptr_array(s->declarations(), out_count);
}

/* --- declaration --- */

IFCAPI_EXPORT const char* ifcopenshell_declaration_name(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->name().c_str() : nullptr;
}

IFCAPI_EXPORT bool ifcopenshell_declaration_is(const void* decl, const char* name) {
    auto* d = as_decl(decl);
    if (!d || !name) return false;
    try {
        return d->is(std::string(name));
    } catch (...) { return false; }
}

IFCAPI_EXPORT const void* ifcopenshell_declaration_as_entity(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->as_entity() : nullptr;
}

IFCAPI_EXPORT const void* ifcopenshell_declaration_as_type(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->as_type_declaration() : nullptr;
}

IFCAPI_EXPORT const void* ifcopenshell_declaration_as_select(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->as_select_type() : nullptr;
}

IFCAPI_EXPORT const void* ifcopenshell_declaration_as_enumeration(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->as_enumeration_type() : nullptr;
}

IFCAPI_EXPORT const void* ifcopenshell_declaration_schema(const void* decl) {
    auto* d = as_decl(decl);
    return d ? d->schema() : nullptr;
}

/* --- entity --- */

IFCAPI_EXPORT bool ifcopenshell_entity_is_abstract(const void* entity) {
    auto* e = as_entity_(entity);
    return e ? e->is_abstract() : false;
}

IFCAPI_EXPORT const void* ifcopenshell_entity_supertype(const void* entity) {
    auto* e = as_entity_(entity);
    return e ? e->supertype() : nullptr;
}

IFCAPI_EXPORT uint32_t ifcopenshell_entity_attribute_count(const void* entity) {
    auto* e = as_entity_(entity);
    return e ? static_cast<uint32_t>(e->attribute_count()) : 0;
}

IFCAPI_EXPORT const void* ifcopenshell_entity_attribute_by_index(const void* entity, uint32_t index) {
    auto* e = as_entity_(entity);
    if (!e) return nullptr;
    try {
        return e->attribute_by_index(index);
    } catch (...) { return nullptr; }
}

IFCAPI_EXPORT int32_t ifcopenshell_entity_attribute_index_by_name(const void* entity, const char* name) {
    auto* e = as_entity_(entity);
    if (!e || !name) return -1;
    try {
        return static_cast<int32_t>(e->attribute_index(std::string(name)));
    } catch (...) { return -1; }
}

IFCAPI_EXPORT const void** ifcopenshell_entity_all_attributes(const void* entity, uint32_t* out_count) {
    auto* e = as_entity_(entity);
    if (!e) { if (out_count) *out_count = 0; return nullptr; }
    return alloc_ptr_array(e->all_attributes(), out_count);
}

IFCAPI_EXPORT const void** ifcopenshell_entity_all_inverse_attributes(const void* entity, uint32_t* out_count) {
    auto* e = as_entity_(entity);
    if (!e) { if (out_count) *out_count = 0; return nullptr; }
    return alloc_ptr_array(e->all_inverse_attributes(), out_count);
}

IFCAPI_EXPORT bool ifcopenshell_entity_attribute_derived(const void* entity, uint32_t index) {
    auto* e = as_entity_(entity);
    if (!e) return false;
    // Walk up to find which entity declares attribute at this flattened index.
    // all_attributes() order matches flattened index; derived() is per-entity,
    // so we accumulate: supertype's derived, then own.
    std::vector<bool> flat;
    std::function<void(const IfcParse::entity*)> collect = [&](const IfcParse::entity* ent) {
        if (!ent) return;
        collect(ent->supertype());
        const auto& d = ent->derived();
        flat.insert(flat.end(), d.begin(), d.end());
    };
    collect(e);
    if (index >= flat.size()) return false;
    return flat[index];
}

/* --- type_declaration --- */

IFCAPI_EXPORT const void* ifcopenshell_type_declaration_declared_type(const void* type_decl) {
    auto* t = as_type_(type_decl);
    return t ? t->declared_type() : nullptr;
}

/* --- enumeration_type --- */

IFCAPI_EXPORT const char** ifcopenshell_enumeration_items(const void* enum_decl, uint32_t* out_count) {
    auto* en = as_enum_(enum_decl);
    if (!en) { if (out_count) *out_count = 0; return nullptr; }
    const auto& items = en->enumeration_items();
    if (out_count) *out_count = static_cast<uint32_t>(items.size());
    if (items.empty()) return nullptr;
    const char** arr = static_cast<const char**>(std::malloc(sizeof(char*) * items.size()));
    if (!arr) return nullptr;
    for (size_t i = 0; i < items.size(); ++i) {
        arr[i] = items[i].c_str();
    }
    return arr;
}

/* --- select_type --- */

IFCAPI_EXPORT const void** ifcopenshell_select_list(const void* select_decl, uint32_t* out_count) {
    auto* s = as_select_(select_decl);
    if (!s) { if (out_count) *out_count = 0; return nullptr; }
    return alloc_ptr_array(s->select_list(), out_count);
}

/* --- attribute --- */

IFCAPI_EXPORT const char* ifcopenshell_attribute_name(const void* attr) {
    auto* a = as_attr(attr);
    return a ? a->name().c_str() : nullptr;
}

IFCAPI_EXPORT bool ifcopenshell_attribute_optional(const void* attr) {
    auto* a = as_attr(attr);
    return a ? a->optional() : false;
}

IFCAPI_EXPORT const void* ifcopenshell_attribute_type_of_attribute(const void* attr) {
    auto* a = as_attr(attr);
    return a ? a->type_of_attribute() : nullptr;
}

/* --- inverse_attribute --- */

IFCAPI_EXPORT const char* ifcopenshell_inverse_attribute_name(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? i->name().c_str() : nullptr;
}

IFCAPI_EXPORT int ifcopenshell_inverse_attribute_type_of_aggregation(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? static_cast<int>(i->type_of_aggregation()) : -1;
}

IFCAPI_EXPORT int32_t ifcopenshell_inverse_attribute_bound1(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? i->bound1() : 0;
}

IFCAPI_EXPORT int32_t ifcopenshell_inverse_attribute_bound2(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? i->bound2() : 0;
}

IFCAPI_EXPORT const void* ifcopenshell_inverse_attribute_entity_reference(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? i->entity_reference() : nullptr;
}

IFCAPI_EXPORT const void* ifcopenshell_inverse_attribute_attribute_reference(const void* iattr) {
    auto* i = as_iattr(iattr);
    return i ? i->attribute_reference() : nullptr;
}

/* --- parameter_type (polymorphic) --- */

IFCAPI_EXPORT int ifcopenshell_parameter_type_kind(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return -1;
    if (p->as_named_type())       return IFCAPI_PT_NAMED;
    if (p->as_simple_type())      return IFCAPI_PT_SIMPLE;
    if (p->as_aggregation_type()) return IFCAPI_PT_AGGREGATION;
    return -1;
}

IFCAPI_EXPORT const void* ifcopenshell_named_type_declared_type(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return nullptr;
    auto* n = p->as_named_type();
    return n ? n->declared_type() : nullptr;
}

IFCAPI_EXPORT int ifcopenshell_simple_type_data_type(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return -1;
    auto* s = p->as_simple_type();
    return s ? static_cast<int>(s->declared_type()) : -1;
}

IFCAPI_EXPORT int ifcopenshell_aggregation_type_kind(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return -1;
    auto* a = p->as_aggregation_type();
    return a ? static_cast<int>(a->type_of_aggregation()) : -1;
}

IFCAPI_EXPORT int32_t ifcopenshell_aggregation_type_bound1(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return 0;
    auto* a = p->as_aggregation_type();
    return a ? a->bound1() : 0;
}

IFCAPI_EXPORT int32_t ifcopenshell_aggregation_type_bound2(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return 0;
    auto* a = p->as_aggregation_type();
    return a ? a->bound2() : 0;
}

IFCAPI_EXPORT const void* ifcopenshell_aggregation_type_element(const void* pt) {
    auto* p = as_pt(pt);
    if (!p) return nullptr;
    auto* a = p->as_aggregation_type();
    return a ? a->type_of_element() : nullptr;
}

} // extern "C"
