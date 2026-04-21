// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// EXPRESS DERIVE-rule runtime helpers.
//
// These declarations are the entire surface that generated rule code
// targets. The C++ emitter (src/ifcapi/codegen/express/) is responsible
// for translating each Python helper used by the upstream
// `rule_compiler.py` to a call into this namespace, so the set of names
// here mirrors the runtime helpers at the top of the auto-generated
// `ifcopenshell/express/rules/<SCHEMA>.py`.
//
// Implementations live in src/ifcapi/src/express/runtime.cpp.

#ifndef IFCAPI_EXPRESS_RUNTIME_H
#define IFCAPI_EXPRESS_RUNTIME_H

#include "ifcapi/express/value.h"

#include <string>
#include <string_view>

namespace ifcapi {
namespace express {

/* --- INDETERMINATE predicates / fall-throughs --------------------- */

inline bool is_indeterminate(const Value& v) noexcept {
    return v.is_indeterminate();
}

/// EXPRESS NVL(a, b) — return `a` unless it is INDETERMINATE, in which
/// case return `b`.
inline Value nvl(const Value& a, const Value& b) {
    return a.is_indeterminate() ? b : a;
}

/// EXPRESS EXISTS(v) — true iff `v` is not INDETERMINATE.
inline Value exists(const Value& v) noexcept {
    return !v.is_indeterminate();
}

/* --- Aggregate inspection ----------------------------------------- */

/// EXPRESS SIZEOF / HIINDEX / BLENGTH on a list/set/string. Returns
/// INDETERMINATE for INDETERMINATE input or unsupported types. HIINDEX
/// returns the same value (1-based length) and LOINDEX returns 1 for a
/// non-empty aggregate, INDETERMINATE otherwise.
Value sizeof_(const Value& v);
Value hiindex(const Value& v);
Value loindex(const Value& v);
Value blength(const Value& v);

/* --- Set operations ------------------------------------------------ */

/// EXPRESS set union / intersection. Operands are first coerced to
/// SetData (deduplicated insertion-order). INDETERMINATE absorbs.
Value set_union(const Value& a, const Value& b);
Value set_intersect(const Value& a, const Value& b);

/* --- Indexing & attribute access ----------------------------------- */

/// EXPRESS 1-based indexing into a list/set. Out-of-range yields
/// INDETERMINATE.
Value express_getitem(const Value& container, const Value& idx);

/// Attribute access on an entity (`self.Foo`). Translates to a call into
/// the existing C ABI's attribute-by-name reader. Unknown attributes,
/// nullptr entities, or INDETERMINATE input yield INDETERMINATE.
Value express_getattr(const Value& v, std::string_view attr_name);

/* --- Type queries -------------------------------------------------- */

/// EXPRESS TYPEOF — returns a SetData of lowercase
/// `schema.entity_name` strings comprising the supertype chain of `v`.
/// INDETERMINATE for non-entity / INDETERMINATE input.
Value typeof_(const Value& v);

/// EXPRESS USEDIN(inst, "schema.type.attr"). Returns a SetData of all
/// instances that reference `inst` via the named inverse attribute.
Value usedin(const Value& v, std::string_view qualified_attr);

/* --- Range / iteration helpers ------------------------------------- */

/// EXPRESS-style integer range [lo, hi] inclusive, returned as a list.
/// Yields an empty list if any operand is INDETERMINATE.
Value express_range(const Value& lo, const Value& hi);

/* --- Entity construction (used inside rule code) ------------------ */

/// Construct a fresh entity of `type_name` in a per-thread scratch file
/// for `schema_name`, then assign each (name, value) pair as an
/// attribute. INDETERMINATE values are skipped (left unset). The
/// returned EntityRef remains valid for the lifetime of the calling
/// thread; rule invocations are short-lived so this is acceptable.
///
/// Returns INDETERMINATE on failure (unknown schema/type, type-system
/// mismatch, etc.).
Value make_entity(std::string_view schema_name,
                  std::string_view type_name,
                  std::initializer_list<std::pair<std::string_view, Value>> attrs = {});

/* --- Math passthroughs --------------------------------------------- */

Value math_sqrt(const Value& v);
Value math_sin(const Value& v);
Value math_cos(const Value& v);
Value math_tan(const Value& v);
Value math_asin(const Value& v);
Value math_acos(const Value& v);
Value math_atan(const Value& v);
Value math_atan2(const Value& y, const Value& x);
Value math_log(const Value& v);
Value math_exp(const Value& v);
Value math_pow(const Value& base, const Value& exp);
Value math_abs(const Value& v);

}  // namespace express
}  // namespace ifcapi

#endif  // IFCAPI_EXPRESS_RUNTIME_H
