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
struct ifcopenshell_file_t;
struct ifcopenshell_instance_t;

/* Opaque value handle. Defined in selector_get.cpp. */
typedef struct ifcopenshell_selector_value_t ifcopenshell_selector_value_t;

typedef enum {
    IFCSEL_VALUE_NONE     = 0,
    IFCSEL_VALUE_BOOL     = 1,
    IFCSEL_VALUE_INT      = 2,
    IFCSEL_VALUE_DOUBLE   = 3,
    IFCSEL_VALUE_STRING   = 4,
    IFCSEL_VALUE_INSTANCE = 5,
    IFCSEL_VALUE_LIST     = 6,
    IFCSEL_VALUE_DICT     = 7,
} ifcopenshell_selector_value_kind_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IFCAPI_VALUE_H */
