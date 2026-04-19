// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_VALUE_H
#define IFCAPI_VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef IFCAPI_EXPORT
#  ifdef _WIN32
#    ifdef IFCAPI_BUILD
#      define IFCAPI_EXPORT __declspec(dllexport)
#    else
#      define IFCAPI_EXPORT __declspec(dllimport)
#    endif
#  else
#    define IFCAPI_EXPORT __attribute__((visibility("default")))
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations — full definitions are in ifcopenshell_api_internal.hpp. */
struct ifcopenshell_ifc_file_t;
struct ifcopenshell_ifc_instance_t;

/* Opaque value handle. Defined in selector_get.cpp. */
typedef struct ifcopenshell_value_t ifcopenshell_value_t;

typedef enum {
    IFCSEL_VALUE_NONE     = 0,
    IFCSEL_VALUE_BOOL     = 1,
    IFCSEL_VALUE_INT      = 2,
    IFCSEL_VALUE_DOUBLE   = 3,
    IFCSEL_VALUE_STRING   = 4,
    IFCSEL_VALUE_INSTANCE = 5,
    IFCSEL_VALUE_LIST     = 6,
    IFCSEL_VALUE_DICT     = 7,
} ifcopenshell_value_kind_t;

/* Free a root value.  Children are owned by their parent and freed
   recursively; do NOT call ifcopenshell_value_free on child pointers
   returned by value_list_at / value_dict_value_at. */
IFCAPI_EXPORT void ifcopenshell_value_free(ifcopenshell_value_t* v);

/* Kind discriminator. */
IFCAPI_EXPORT ifcopenshell_value_kind_t
ifcopenshell_value_kind(const ifcopenshell_value_t* v);

/* Scalar accessors — call only when kind matches. */
IFCAPI_EXPORT bool        ifcopenshell_value_as_bool(const ifcopenshell_value_t* v);
IFCAPI_EXPORT int64_t     ifcopenshell_value_as_int64(const ifcopenshell_value_t* v);
IFCAPI_EXPORT double      ifcopenshell_value_as_double(const ifcopenshell_value_t* v);
IFCAPI_EXPORT const char* ifcopenshell_value_as_string(const ifcopenshell_value_t* v);

/* Instance accessor.  Returns a freshly allocated, caller-owned handle
   wrapping the same underlying entity (owned=false).  The entity itself
   remains owned by the IfcFile. */
IFCAPI_EXPORT struct ifcopenshell_ifc_instance_t*
ifcopenshell_value_as_instance(const ifcopenshell_value_t* v);

/* List accessors.  Returned pointers are owned by the parent value. */
IFCAPI_EXPORT size_t
ifcopenshell_value_list_size(const ifcopenshell_value_t* v);

IFCAPI_EXPORT const ifcopenshell_value_t*
ifcopenshell_value_list_at(const ifcopenshell_value_t* v, size_t idx);

/* Dict accessors.  Returned pointers are owned by the parent value. */
IFCAPI_EXPORT size_t
ifcopenshell_value_dict_size(const ifcopenshell_value_t* v);

IFCAPI_EXPORT const char*
ifcopenshell_value_dict_key_at(const ifcopenshell_value_t* v, size_t idx);

IFCAPI_EXPORT const ifcopenshell_value_t*
ifcopenshell_value_dict_value_at(const ifcopenshell_value_t* v, size_t idx);

/* ------------------------------------------------------------------ */
/* Value builders — used by handwritten C++ callers (e.g. set_element_value)
   to construct values from Python. The returned value is owned by the
   caller and must be released with ifcopenshell_value_free unless it has
   been transferred (e.g. via ifcopenshell_value_list_append, which takes
   ownership of `item`). */
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_none(void);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_bool(bool b);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_int(int64_t i);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_double(double d);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_string(const char* s);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_instance(struct ifcopenshell_ifc_instance_t* h);
IFCAPI_EXPORT ifcopenshell_value_t* ifcopenshell_value_new_list(void);
IFCAPI_EXPORT void ifcopenshell_value_list_append(ifcopenshell_value_t* list, ifcopenshell_value_t* item);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IFCAPI_VALUE_H */
