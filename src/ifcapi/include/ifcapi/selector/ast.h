// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Public C ABI for the native selector parser AST.

#ifndef IFCAPI_SELECTOR_AST_H
#define IFCAPI_SELECTOR_AST_H

/* ------------------------------------------------------------------ */
/*  Opaque node handle                                                 */
/* ------------------------------------------------------------------ */

typedef struct ifcopenshell_selector_node_t ifcopenshell_selector_node_t;

/* ------------------------------------------------------------------ */
/*  Node-kind enumeration                                              */
/*                                                                     */
/*  Values < IFCSEL_TOKEN_FIRST are rule nodes.                        */
/*  Values >= IFCSEL_TOKEN_FIRST are token nodes.                      */
/* ------------------------------------------------------------------ */

typedef enum {
    /* ---- Filter grammar rule nodes ---- */
    IFCSEL_NODE_START               = 0,
    IFCSEL_NODE_FILTER_GROUP        = 1,
    IFCSEL_NODE_FACET_LIST          = 2,
    IFCSEL_NODE_FACET               = 3,
    IFCSEL_NODE_INSTANCE            = 4,
    IFCSEL_NODE_ENTITY              = 5,
    IFCSEL_NODE_ATTRIBUTE           = 6,
    IFCSEL_NODE_TYPE_FACET          = 7,
    IFCSEL_NODE_MATERIAL_FACET      = 8,
    IFCSEL_NODE_QUERY_FACET         = 9,
    IFCSEL_NODE_CLASSIFICATION      = 10,
    IFCSEL_NODE_LOCATION            = 11,
    IFCSEL_NODE_GROUP               = 12,
    IFCSEL_NODE_PARENT              = 13,
    IFCSEL_NODE_PROPERTY            = 14,
    IFCSEL_NODE_PSET                = 15,
    IFCSEL_NODE_PROP                = 16,
    IFCSEL_NODE_KEYS                = 17,
    IFCSEL_NODE_ATTRIBUTE_NAME      = 18,
    IFCSEL_NODE_IFC_CLASS           = 19,
    IFCSEL_NODE_GLOBALID            = 20,
    IFCSEL_NODE_VALUE               = 21,
    IFCSEL_NODE_UNQUOTED_STRING     = 22,
    IFCSEL_NODE_REGEX_STRING        = 23,
    IFCSEL_NODE_QUOTED_STRING       = 24,
    IFCSEL_NODE_SPECIAL             = 25,
    IFCSEL_NODE_NULL                = 26,
    IFCSEL_NODE_TRUE                = 27,
    IFCSEL_NODE_FALSE               = 28,
    IFCSEL_NODE_COMPARISON          = 29,
    IFCSEL_NODE_NOT                 = 30,
    IFCSEL_NODE_EQUALS              = 31,
    IFCSEL_NODE_MORETHANEQUALTO     = 32,
    IFCSEL_NODE_LESSTHANEQUALTO     = 33,
    IFCSEL_NODE_MORETHAN            = 34,
    IFCSEL_NODE_LESSTHAN            = 35,
    IFCSEL_NODE_CONTAINS            = 36,
    /* ---- Get-element grammar rule nodes ---- */
    IFCSEL_NODE_KEYS_LIST           = 37,
    IFCSEL_NODE_KEY                 = 38,
    /* ---- Format grammar rule nodes ---- */
    IFCSEL_NODE_ADD                 = 39,
    IFCSEL_NODE_SUBTRACT            = 40,
    IFCSEL_NODE_MULTIPLY            = 41,
    IFCSEL_NODE_DIVIDE              = 42,
    IFCSEL_NODE_FUNCTION            = 43,
    IFCSEL_NODE_VARIABLE            = 44,
    IFCSEL_NODE_QUERY_PATH          = 45,
    IFCSEL_NODE_ROUND               = 46,
    IFCSEL_NODE_NUMBER_FN           = 47,
    IFCSEL_NODE_INT_FN              = 48,
    IFCSEL_NODE_FORMAT_LENGTH       = 49,
    IFCSEL_NODE_METRIC_LENGTH       = 50,
    IFCSEL_NODE_IMPERIAL_LENGTH     = 51,
    IFCSEL_NODE_LOWER               = 52,
    IFCSEL_NODE_UPPER               = 53,
    IFCSEL_NODE_TITLE               = 54,
    IFCSEL_NODE_CONCAT              = 55,
    IFCSEL_NODE_SUBSTR              = 56,
    IFCSEL_NODE_SORT                = 57,
    IFCSEL_NODE_REVERSE             = 58,
    IFCSEL_NODE_JOIN                = 59,
    IFCSEL_NODE_BOOLEAN             = 60,

    /* ---- Boundary between rule and token kinds ---- */
    IFCSEL_TOKEN_FIRST              = 100,

    /* ---- Token kinds ---- */
    IFCSEL_TOKEN_ESCAPED_STRING     = 100,
    IFCSEL_TOKEN_SIGNED_NUMBER      = 101,
    IFCSEL_TOKEN_NUMBER             = 102,
    IFCSEL_TOKEN_SIGNED_INT         = 103,
    IFCSEL_TOKEN_TRUE               = 104,
    IFCSEL_TOKEN_FALSE              = 105,
    IFCSEL_TOKEN_ANON               = 106   /* anonymous terminal (inline regex) */
} ifcsel_node_kind;

#endif /* IFCAPI_SELECTOR_AST_H */
