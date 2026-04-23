// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// Public C ABI for invoking compiled EXPRESS DERIVE rules.
//
// The Python wrapper (entity_instance._get_derived) calls
// `ifcopenshell_compute_derived` to obtain the value of a DERIVE attribute
// using the native dispatch table populated by the generated
// src/ifcapi/src/express/generated/<SCHEMA>.cpp translation units. The
// returned ifcopenshell_value_t* is caller-owned (free with
// ifcopenshell_value_free) and NULL when no rule is registered for the
// (entity, attribute) pair, when the input is invalid, or when the rule
// evaluates to EXPRESS INDETERMINATE.

#ifndef IFCAPI_EXPRESS_RULES_H
#define IFCAPI_EXPRESS_RULES_H

#include "ifcapi/value.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ifcopenshell_ifc_instance_t;

/// Evaluate the DERIVE attribute `attr_name` on `instance` using the
/// registered native rule for the instance's schema. Walks the supertype
/// chain and invokes the first matching rule.
///
/// Returns NULL when:
///   * `instance` or `attr_name` is NULL.
///   * No DERIVE rule is registered for (entity, attr_name).
///   * The rule evaluates to INDETERMINATE (EXPRESS "?").
///
/// Otherwise returns a freshly allocated ifcopenshell_value_t* that the
/// caller must release with ifcopenshell_value_free.
IFCAPI_EXPORT ifcopenshell_value_t*
ifcopenshell_compute_derived(struct ifcopenshell_ifc_instance_t* instance,
                             const char* attr_name);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // IFCAPI_EXPRESS_RULES_H
