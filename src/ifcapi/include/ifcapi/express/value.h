// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// Native value model for the EXPRESS DERIVE-rule runtime.
//
// EXPRESS rules are total functions over a dynamic value space whose
// semantics closely match Python (the upstream `rule_compiler.py` emits
// Python that is then interpreted at runtime). When porting that emit
// stage to C++ we need a value type that exhibits the same dynamic
// behaviour:
//
//   * It carries one of a small fixed set of variant alternatives
//     (entity reference, integer, real, string, boolean, list, set or
//     INDETERMINATE — the EXPRESS analogue of Python's "?" / "missing").
//   * Every operator is total: applying an operator to an INDETERMINATE
//     yields INDETERMINATE rather than raising. INDETERMINATE is the
//     absorbing element of the algebra.
//   * Lists and sets are reference-counted to mirror Python list/set
//     semantics (cheap copies, mutation reflected in shared aliases).
//
// The implementation uses `std::variant` for compile-time exhaustive
// dispatch, with no third-party dependencies. The header is intentionally
// the entire surface — generated rule code includes it directly.

#ifndef IFCAPI_EXPRESS_VALUE_H
#define IFCAPI_EXPRESS_VALUE_H

#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace ifcapi {
namespace express {

class Value;

/// Reference to an entity instance. The pointee is borrowed from an
/// `IfcParse::IfcFile` and remains valid for the duration of a rule
/// invocation.
struct EntityRef {
    void* ptr = nullptr; ///< IfcUtil::IfcBaseClass* — opaque to keep this
                         ///< header free of ifcparse includes.

    constexpr bool operator==(const EntityRef& other) const noexcept {
        return ptr == other.ptr;
    }
    constexpr bool operator!=(const EntityRef& other) const noexcept {
        return ptr != other.ptr;
    }
};

/// Sentinel for EXPRESS INDETERMINATE ("?"). Any operator with an
/// INDETERMINATE operand returns INDETERMINATE.
struct Indeterminate {
    constexpr bool operator==(const Indeterminate&) const noexcept { return true; }
    constexpr bool operator!=(const Indeterminate&) const noexcept { return false; }
};

using ListData = std::vector<Value>;
struct ListPtr { std::shared_ptr<ListData> p; };
using SetData  = std::vector<Value>; ///< Order-preserving set; uniqueness
                                     ///< enforced by insert/union/intersect.
struct SetPtr  { std::shared_ptr<SetData> p; };

inline ListPtr make_list_ptr() { return {std::make_shared<ListData>()}; }
inline SetPtr  make_set_ptr()  { return {std::make_shared<SetData>()}; }

/// Tagged union of all EXPRESS dynamic value alternatives. Use the free
/// helpers below (`is_indeterminate`, `nvl`, ...) rather than poking at
/// the variant directly when implementing rule helpers.
class Value {
   public:
    using Variant = std::variant<
        Indeterminate,
        bool,
        std::int64_t,
        double,
        std::string,
        EntityRef,
        ListPtr,
        SetPtr>;

    enum class Tag : std::uint8_t {
        Indeterminate = 0,
        Bool          = 1,
        Int           = 2,
        Real          = 3,
        Str           = 4,
        Entity        = 5,
        List          = 6,
        Set           = 7,
    };

    /// Constructors — implicit so generated code can write
    /// `return 1.0;` and have it auto-wrap.
    Value() : data_(Indeterminate{}) {}
    Value(Indeterminate)               : data_(Indeterminate{}) {}
    Value(bool b)                      : data_(b) {}
    Value(int v)                       : data_(static_cast<std::int64_t>(v)) {}
    Value(std::int64_t v)              : data_(v) {}
    Value(double v)                    : data_(v) {}
    Value(const char* s)               : data_(std::string(s)) {}
    Value(std::string s)               : data_(std::move(s)) {}
    Value(EntityRef e)                 : data_(e) {}
    Value(ListPtr p)                   : data_(std::move(p)) {}
    Value(SetPtr p)                    : data_(std::move(p)) {}

    /// Convenience: build a fresh empty list/set.
    static Value make_list() { return Value(make_list_ptr()); }
    static Value make_set()  { return Value(make_set_ptr()); }
    static Value make_list(std::initializer_list<Value> items) {
        auto p = make_list_ptr();
        p.p->reserve(items.size());
        for (const auto& v : items) p.p->push_back(v);
        return Value(std::move(p));
    }

    Tag tag() const noexcept { return static_cast<Tag>(data_.index()); }
    const Variant& variant() const noexcept { return data_; }
    Variant& variant() noexcept { return data_; }

    bool is_indeterminate() const noexcept { return tag() == Tag::Indeterminate; }
    bool is_bool()          const noexcept { return tag() == Tag::Bool; }
    bool is_int()           const noexcept { return tag() == Tag::Int; }
    bool is_real()          const noexcept { return tag() == Tag::Real; }
    bool is_number()        const noexcept { return is_int() || is_real(); }
    bool is_string()        const noexcept { return tag() == Tag::Str; }
    bool is_entity()        const noexcept { return tag() == Tag::Entity; }
    bool is_list()          const noexcept { return tag() == Tag::List; }
    bool is_set()           const noexcept { return tag() == Tag::Set; }
    bool is_collection()    const noexcept { return is_list() || is_set(); }

    bool        as_bool()    const { return std::get<bool>(data_); }
    std::int64_t as_int()    const { return std::get<std::int64_t>(data_); }
    double      as_double()  const {
        if (is_int()) return static_cast<double>(as_int());
        return std::get<double>(data_);
    }
    const std::string& as_string() const { return std::get<std::string>(data_); }
    EntityRef          as_entity() const { return std::get<EntityRef>(data_); }
    const ListData&    as_list()   const { return *std::get<ListPtr>(data_).p; }
    ListData&          as_list_mut()     { return *std::get<ListPtr>(data_).p; }
    const SetData&     as_set()    const { return *std::get<SetPtr>(data_).p; }
    SetData&           as_set_mut()      { return *std::get<SetPtr>(data_).p; }

    /// Truthiness — used by generated `if`/`and`/`or` code. Mirrors
    /// Python: 0/0.0/""/empty-collection/false are falsy; INDETERMINATE
    /// is itself indeterminate, callers must short-circuit explicitly.
    /// To keep semantics regular we treat INDETERMINATE as falsy here so
    /// that pure-control-flow lookups work; arithmetic operators handle
    /// it as an absorbing element separately.
    bool truthy() const noexcept {
        switch (tag()) {
            case Tag::Indeterminate: return false;
            case Tag::Bool:          return as_bool();
            case Tag::Int:           return as_int() != 0;
            case Tag::Real:          return as_double() != 0.0;
            case Tag::Str:           return !as_string().empty();
            case Tag::Entity:        return as_entity().ptr != nullptr;
            case Tag::List:          return !as_list().empty();
            case Tag::Set:           return !as_set().empty();
        }
        return false;
    }

   private:
    Variant data_;
};

// ---------------------------------------------------------------------
//  Comparison operators — INDETERMINATE absorbing semantics.
//  Any comparison involving INDETERMINATE yields INDETERMINATE (returned
//  as Value, not bool, since the caller's chained logic must propagate).
// ---------------------------------------------------------------------

namespace detail {

/// Coerce two numeric values (int, real) for arithmetic/comparison.
/// Returns std::pair<double, double> when either is real; otherwise
/// std::pair<int64_t, int64_t>. Non-numeric or INDETERMINATE input is
/// signalled by a nullopt-equivalent (caller checks via tags first).
inline bool both_numeric(const Value& a, const Value& b) noexcept {
    return a.is_number() && b.is_number();
}

inline double to_d(const Value& v) noexcept {
    return v.is_int() ? static_cast<double>(v.as_int()) : v.as_double();
}

}  // namespace detail

inline Value operator==(const Value& a, const Value& b) {
    if (a.is_indeterminate() || b.is_indeterminate()) return Indeterminate{};
    if (detail::both_numeric(a, b)) return detail::to_d(a) == detail::to_d(b);
    if (a.tag() != b.tag()) return false;
    switch (a.tag()) {
        case Value::Tag::Bool:   return a.as_bool() == b.as_bool();
        case Value::Tag::Str:    return a.as_string() == b.as_string();
        case Value::Tag::Entity: return a.as_entity() == b.as_entity();
        // Lists/sets compared by identity for now — EXPRESS rule code does
        // not rely on deep equality of aggregates in any DERIVE we
        // observed; if needed this can be deepened later.
        case Value::Tag::List:
            return std::get<ListPtr>(a.variant()).p.get() ==
                   std::get<ListPtr>(b.variant()).p.get();
        case Value::Tag::Set:
            return std::get<SetPtr>(a.variant()).p.get() ==
                   std::get<SetPtr>(b.variant()).p.get();
        default: return false;
    }
}

inline Value operator!=(const Value& a, const Value& b) {
    Value eq = (a == b);
    if (eq.is_indeterminate()) return eq;
    return !eq.as_bool();
}

#define IFCAPI_DEFINE_NUMERIC_CMP(OP)                                    \
    inline Value operator OP(const Value& a, const Value& b) {           \
        if (a.is_indeterminate() || b.is_indeterminate())                \
            return Indeterminate{};                                      \
        if (!detail::both_numeric(a, b)) return Indeterminate{};         \
        return detail::to_d(a) OP detail::to_d(b);                       \
    }
IFCAPI_DEFINE_NUMERIC_CMP(<)
IFCAPI_DEFINE_NUMERIC_CMP(<=)
IFCAPI_DEFINE_NUMERIC_CMP(>)
IFCAPI_DEFINE_NUMERIC_CMP(>=)
#undef IFCAPI_DEFINE_NUMERIC_CMP

// ---------------------------------------------------------------------
//  Arithmetic — INDETERMINATE absorbing.
// ---------------------------------------------------------------------

#define IFCAPI_DEFINE_NUMERIC_BINOP(OP)                                  \
    inline Value operator OP(const Value& a, const Value& b) {           \
        if (a.is_indeterminate() || b.is_indeterminate())                \
            return Indeterminate{};                                      \
        if (!detail::both_numeric(a, b)) return Indeterminate{};         \
        if (a.is_int() && b.is_int())                                    \
            return static_cast<std::int64_t>(a.as_int() OP b.as_int());  \
        return detail::to_d(a) OP detail::to_d(b);                       \
    }
IFCAPI_DEFINE_NUMERIC_BINOP(+)
IFCAPI_DEFINE_NUMERIC_BINOP(-)
IFCAPI_DEFINE_NUMERIC_BINOP(*)
#undef IFCAPI_DEFINE_NUMERIC_BINOP

inline Value operator/(const Value& a, const Value& b) {
    if (a.is_indeterminate() || b.is_indeterminate()) return Indeterminate{};
    if (!detail::both_numeric(a, b)) return Indeterminate{};
    double bd = detail::to_d(b);
    if (bd == 0.0) return Indeterminate{};
    return detail::to_d(a) / bd;
}

inline Value operator-(const Value& a) {
    if (a.is_indeterminate()) return Indeterminate{};
    if (a.is_int())  return static_cast<std::int64_t>(-a.as_int());
    if (a.is_real()) return -a.as_double();
    return Indeterminate{};
}

inline Value operator!(const Value& a) {
    if (a.is_indeterminate()) return Indeterminate{};
    return !a.truthy();
}

}  // namespace express
}  // namespace ifcapi

#endif  // IFCAPI_EXPRESS_VALUE_H
