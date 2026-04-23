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

/// Opaque forward declaration of the file handle used by global rules.
/// Generated rule signatures are `bool ruleX(IfcFile* file)`. Resolved
/// at link time to `IfcParse::IfcFile` via the dispatch layer.
struct IfcFile;

/// Lookup all instances of `type_name` (and subtypes) in `file`,
/// returning a Value of tag::Set populated with EntityRef-wrapped Values.
/// Used by global rules' implicit `IfcType` collection variables.
Value file_by_type(IfcFile* file, const char* type_name);

/* --- DERIVED-attribute dispatch registry -------------------------- */

/// Pointer to a generated `calc_<Entity>_<Attr>(EntityRef)` function.
using DeriveFn = Value (*)(EntityRef);

/// Per-schema registration entry point. Generated `<SCHEMA>.cpp` files
/// invoke this at static-init time to register one calc per derived
/// attribute keyed on (schema_name, declaring_entity_name, attr_name).
/// Lower-cases storage; lookup is also case-insensitive.
void register_derived(std::string_view schema_name,
                      std::string_view entity_name,
                      std::string_view attr_name,
                      DeriveFn fn);

/// Resolve a derived calc for `attr_name` on the entity declaration
/// chain rooted at `decl`. Returns nullptr if `attr_name` is not a
/// derived attribute on `decl` or any of its supertypes.
DeriveFn lookup_derived(const void* decl, std::string_view attr_name);


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

/// EXPRESS IN — set membership. Returns INDETERMINATE if `container` is
/// not iterable or is INDETERMINATE; otherwise true iff any element of
/// `container` compares equal to `needle`.
Value express_in(const Value& needle, const Value& container);

/// Coerce a value to a numeric Value (EXPRESS VALUE() built-in).
Value express_value(const Value& v);

/// Iteration helper — exposes the elements of a list/set as a contiguous
/// view of `Value` for range-based-for. INDETERMINATE / scalar input
/// yields an empty view.
const std::vector<Value>& iter(const Value& v);

/// EXPRESS aggregate repetition: `[expr] * n` semantic. Returns a fresh
/// list of `n` copies of `expr`.
Value repeat(const Value& v, const Value& count);

/// Materialise a mutable copy of a list/set as `std::vector<Value>` so
/// generated code can perform indexed assignment without aliasing the
/// shared backing storage.
std::vector<Value> to_list(const Value& v);

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

/// Functional 1-based list element setter — returns a new Value containing
/// a copy of the input list with element at `idx` replaced. INDETERMINATE
/// for non-list / out-of-range input.
Value set_index(const Value& container, const Value& idx, const Value& v);

/// Functional entity attribute setter — returns a new Value (a freshly
/// constructed entity) with the named attribute replaced. INDETERMINATE
/// for non-entity input.
Value set_attr(const Value& v, std::string_view attr_name, const Value& value);

/* --- Type queries -------------------------------------------------- */

/// EXPRESS TYPEOF — returns a SetData of lowercase
/// `schema.entity_name` strings comprising the supertype chain of `v`.
/// INDETERMINATE for non-entity / INDETERMINATE input.
Value typeof_(const Value& v);

/// EXPRESS USEDIN(inst, "schema.type.attr"). Returns a SetData of all
/// instances that reference `inst` via the named inverse attribute.
Value usedin(const Value& v, std::string_view qualified_attr);

/// Overload for the codegen path where `qualified_attr` is itself a
/// Value-wrapped string literal (the more common shape after our string
/// literal rewrite).
inline Value usedin(const Value& v, const Value& qualified_attr) {
    if (qualified_attr.is_indeterminate() || !qualified_attr.is_string())
        return Indeterminate{};
    return usedin(v, std::string_view(qualified_attr.as_string()));
}

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
