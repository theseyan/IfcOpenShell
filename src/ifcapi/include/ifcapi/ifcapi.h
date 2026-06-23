// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_H
#define IFCAPI_H

/// @file ifcapi.h
/// @brief High-level IFC authoring C API.
///
/// This library reimplements the business logic from ifcopenshell.api (Python)
/// in C++, exposed as a plain C API. Functions accept raw IfcParse::IfcFile*
/// pointers (as void*) and STEP entity IDs, making them callable from any
/// language with C FFI support.
///
/// Error handling follows the thread-local pattern: on failure a function
/// returns a sentinel value and the error message is retrievable via
/// ifcopenshell_last_error_message(). Call ifcopenshell_clear_error() before a sequence of calls
/// if you need precise attribution.

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
#  ifdef IFCAPI_BUILD
#    define IFCAPI_EXPORT __declspec(dllexport)
#  else
#    define IFCAPI_EXPORT __declspec(dllimport)
#  endif
#else
#  define IFCAPI_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Opaque handle types (defined by the autogen layer in              */
/*  ifcopenshell_api.h). Forward-declare them here so this header     */
/*  stays standalone — both layers share the same struct definitions  */
/*  and consumers can mix calls freely.                               */
/* ------------------------------------------------------------------ */

typedef struct ifcopenshell_file_t ifcopenshell_file_t;
typedef struct ifcopenshell_instance_t ifcopenshell_instance_t;
typedef struct ifcopenshell_instance_streamer_t ifcopenshell_instance_streamer_t;
typedef struct ifcopenshell_geom_taxonomy_item_t ifcopenshell_geom_taxonomy_item_t;

/* ------------------------------------------------------------------ */
/*  Error handling                                                     */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* The high-level layer reports errors through the same thread-local  */
/* string used by the autogen low-level layer. Use the autogen        */
/* entry points ifcopenshell_last_error_message() and                 */
/* ifcopenshell_clear_error() (declared in ifcopenshell_api.h) to     */
/* read and reset it.                                                 */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/*  Entity attribute type tags                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    IFCAPI_ATTR_NULL       = 0,
    IFCAPI_ATTR_STRING     = 1,
    IFCAPI_ATTR_INT        = 2,
    IFCAPI_ATTR_DOUBLE     = 3,
    IFCAPI_ATTR_BOOL       = 4,
    IFCAPI_ATTR_ENUM       = 5,
    IFCAPI_ATTR_REFERENCE  = 6,
    IFCAPI_ATTR_AGGREGATE  = 7,
    IFCAPI_ATTR_DERIVED    = 8,
    IFCAPI_ATTR_UNKNOWN    = 99
} ifcopenshell_attr_type;

/* ------------------------------------------------------------------ */
/*  Entity operations                                                  */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/*  Inverse attribute access                                           */
/* ------------------------------------------------------------------ */

/// Returns true if the given name is an inverse attribute on the entity.
/* ------------------------------------------------------------------ */
/*  High-level: element graph utilities                                */
/* ------------------------------------------------------------------ */

/* parameter_type kind tag */
#define IFCAPI_PT_NAMED       0
#define IFCAPI_PT_SIMPLE      1
#define IFCAPI_PT_AGGREGATION 2

/* simple_type::data_type values (match IfcSchema.h enum) */
#define IFCAPI_SIMPLE_BINARY   0
#define IFCAPI_SIMPLE_BOOLEAN  1
#define IFCAPI_SIMPLE_INTEGER  2
#define IFCAPI_SIMPLE_LOGICAL  3
#define IFCAPI_SIMPLE_NUMBER   4
#define IFCAPI_SIMPLE_REAL     5
#define IFCAPI_SIMPLE_STRING   6

/* aggregation_type::aggregate_type values */
#define IFCAPI_AGG_ARRAY 0
#define IFCAPI_AGG_BAG   1
#define IFCAPI_AGG_LIST  2
#define IFCAPI_AGG_SET   3

/* inverse_attribute::aggregate_type values */
#define IFCAPI_INV_AGG_BAG         0
#define IFCAPI_INV_AGG_SET         1
#define IFCAPI_INV_AGG_UNSPECIFIED 2

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ */
/*  Selector value type + get_element_value                           */
/* ------------------------------------------------------------------ */
#include "ifcapi/value.h"

#endif /* IFCAPI_H */
