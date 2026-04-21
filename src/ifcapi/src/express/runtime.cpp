// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.
//
// Implementation of the EXPRESS DERIVE-rule runtime helpers declared in
// ifcapi/express/runtime.h.

#include "ifcapi/express/runtime.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_set>

namespace ifcapi {
namespace express {

/* ================================================================== */
/*  Aggregate inspection                                              */
/* ================================================================== */

Value sizeof_(const Value& v) {
    if (v.is_indeterminate()) return Indeterminate{};
    if (v.is_list())   return static_cast<std::int64_t>(v.as_list().size());
    if (v.is_set())    return static_cast<std::int64_t>(v.as_set().size());
    if (v.is_string()) return static_cast<std::int64_t>(v.as_string().size());
    return Indeterminate{};
}

Value hiindex(const Value& v) { return sizeof_(v); }

Value loindex(const Value& v) {
    if (v.is_indeterminate()) return Indeterminate{};
    if (v.is_list() && !v.as_list().empty())  return static_cast<std::int64_t>(1);
    if (v.is_set()  && !v.as_set().empty())   return static_cast<std::int64_t>(1);
    if (v.is_string() && !v.as_string().empty()) return static_cast<std::int64_t>(1);
    return Indeterminate{};
}

Value blength(const Value& v) {
    if (v.is_indeterminate()) return Indeterminate{};
    if (v.is_string()) return static_cast<std::int64_t>(v.as_string().size());
    return Indeterminate{};
}

/* ================================================================== */
/*  Set operations                                                    */
/* ================================================================== */

namespace {

// O(n*m) but EXPRESS sets are tiny (cartesian-point coordinates etc.).
bool contains_value(const SetData& s, const Value& v) {
    for (const auto& it : s) {
        Value eq = (it == v);
        if (!eq.is_indeterminate() && eq.as_bool()) return true;
    }
    return false;
}

std::shared_ptr<SetData> to_set(const Value& v) {
    auto out = std::make_shared<SetData>();
    if (v.is_set()) {
        *out = v.as_set();
        return out;
    }
    if (v.is_list()) {
        for (const auto& item : v.as_list())
            if (!contains_value(*out, item)) out->push_back(item);
        return out;
    }
    return out;  // empty set for scalar/INDETERMINATE handled by caller
}

}  // namespace

Value set_union(const Value& a, const Value& b) {
    if (a.is_indeterminate() || b.is_indeterminate()) return Indeterminate{};
    auto out = to_set(a);
    auto rhs = to_set(b);
    for (const auto& it : *rhs)
        if (!contains_value(*out, it)) out->push_back(it);
    return Value(SetPtr{std::move(out)});
}

Value set_intersect(const Value& a, const Value& b) {
    if (a.is_indeterminate() || b.is_indeterminate()) return Indeterminate{};
    auto lhs = to_set(a);
    auto rhs = to_set(b);
    auto out = std::make_shared<SetData>();
    for (const auto& it : *lhs)
        if (contains_value(*rhs, it)) out->push_back(it);
    return Value(SetPtr{std::move(out)});
}

/* ================================================================== */
/*  Indexing                                                          */
/* ================================================================== */

Value express_getitem(const Value& container, const Value& idx) {
    if (container.is_indeterminate() || idx.is_indeterminate()) return Indeterminate{};
    if (!idx.is_int()) return Indeterminate{};
    auto i = idx.as_int();  // 1-based per EXPRESS
    if (i < 1) return Indeterminate{};
    auto pos = static_cast<size_t>(i - 1);
    if (container.is_list()) {
        const auto& l = container.as_list();
        if (pos >= l.size()) return Indeterminate{};
        return l[pos];
    }
    if (container.is_set()) {
        const auto& s = container.as_set();
        if (pos >= s.size()) return Indeterminate{};
        return s[pos];
    }
    if (container.is_string()) {
        const auto& s = container.as_string();
        if (pos >= s.size()) return Indeterminate{};
        return std::string(1, s[pos]);
    }
    return Indeterminate{};
}

/* ================================================================== */
/*  Attribute & type queries                                          */
/* ================================================================== */

namespace {

IfcUtil::IfcBaseClass* as_baseclass(const Value& v) {
    if (!v.is_entity()) return nullptr;
    return static_cast<IfcUtil::IfcBaseClass*>(v.as_entity().ptr);
}

// Convert an IfcParse Argument to a Value. Recurses for aggregates.
Value argument_to_value(const AttributeValue& a) {
    if (a.isNull()) return Indeterminate{};
    switch (a.type()) {
        case IfcUtil::Argument_BOOL:    return (bool)a;
        case IfcUtil::Argument_LOGICAL: {
            // EXPRESS LOGICAL: TRUE / FALSE / UNKNOWN. UNKNOWN -> INDETERMINATE.
            // The C++ argument exposes it as an enum string in some kernels;
            // fall back to bool extraction otherwise.
            try { return (bool)a; } catch (...) { return Indeterminate{}; }
        }
        case IfcUtil::Argument_INT:     return static_cast<std::int64_t>((int)a);
        case IfcUtil::Argument_DOUBLE:  return (double)a;
        case IfcUtil::Argument_STRING:
        case IfcUtil::Argument_ENUMERATION:
            return (std::string)a;
        case IfcUtil::Argument_ENTITY_INSTANCE: {
            EntityRef ref;
            ref.ptr = static_cast<void*>((IfcUtil::IfcBaseClass*)a);
            return ref;
        }
        case IfcUtil::Argument_AGGREGATE_OF_INT: {
            auto out = std::make_shared<ListData>();
            for (auto i : (std::vector<int>)a) out->emplace_back(static_cast<std::int64_t>(i));
            return Value(ListPtr{std::move(out)});
        }
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: {
            auto out = std::make_shared<ListData>();
            for (auto d : (std::vector<double>)a) out->emplace_back(d);
            return Value(ListPtr{std::move(out)});
        }
        case IfcUtil::Argument_AGGREGATE_OF_STRING: {
            auto out = std::make_shared<ListData>();
            for (auto& s : (std::vector<std::string>)a) out->emplace_back(s);
            return Value(ListPtr{std::move(out)});
        }
        case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
            auto out = std::make_shared<ListData>();
            auto agg = (aggregate_of_instance::ptr)a;
            if (agg) for (auto& it : *agg) {
                EntityRef ref; ref.ptr = static_cast<void*>(it);
                out->emplace_back(ref);
            }
            return Value(ListPtr{std::move(out)});
        }
        default:
            return Indeterminate{};
    }
}

}  // namespace

Value express_getattr(const Value& v, std::string_view attr_name) {
    auto* e = as_baseclass(v);
    if (!e) return Indeterminate{};
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return Indeterminate{};
    auto* d = be->declaration().as_entity();
    if (!d) return Indeterminate{};
    std::string name(attr_name);
    int idx;
    try {
        idx = static_cast<int>(d->attribute_index(name));
    } catch (...) {
        return Indeterminate{};
    }
    if (idx < 0) return Indeterminate{};
    try {
        return argument_to_value(e->get_attribute_value(static_cast<size_t>(idx)));
    } catch (...) {
        return Indeterminate{};
    }
}

Value typeof_(const Value& v) {
    if (v.is_indeterminate()) return Indeterminate{};
    auto* e = as_baseclass(v);
    if (!e) return Indeterminate{};
    auto out = std::make_shared<SetData>();
    const IfcParse::declaration* d = &e->declaration();
    std::string schema = d->schema() ? d->schema()->name() : std::string{};
    // Lower-case schema for matching: EXPRESS typeof yields names like
    // "ifc4.ifccartesianpoint".
    std::string schema_lc = schema;
    std::transform(schema_lc.begin(), schema_lc.end(), schema_lc.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    while (d) {
        std::string name = d->name();
        std::transform(name.begin(), name.end(), name.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::string qual = schema_lc + "." + name;
        // Dedupe.
        bool dup = false;
        for (auto& it : *out) if (it.is_string() && it.as_string() == qual) { dup = true; break; }
        if (!dup) out->emplace_back(qual);
        if (auto* ent = d->as_entity()) d = ent->supertype();
        else d = nullptr;
    }
    return Value(SetPtr{std::move(out)});
}

Value usedin(const Value& v, std::string_view qualified_attr) {
    if (v.is_indeterminate()) return Indeterminate{};
    auto* e = as_baseclass(v);
    if (!e) return Indeterminate{};
    auto* file = e->file_;
    if (!file) {
        auto out = std::make_shared<SetData>();
        return Value(SetPtr{std::move(out)});
    }
    // qualified_attr is "schema.entity.attr". Match "entity.attr" suffix.
    std::string qa(qualified_attr);
    auto first_dot  = qa.find('.');
    auto second_dot = qa.find('.', first_dot == std::string::npos ? 0 : first_dot + 1);
    std::string entity_name, attr_name;
    if (second_dot != std::string::npos) {
        entity_name = qa.substr(first_dot + 1, second_dot - first_dot - 1);
        attr_name   = qa.substr(second_dot + 1);
    } else if (first_dot != std::string::npos) {
        entity_name = qa.substr(0, first_dot);
        attr_name   = qa.substr(first_dot + 1);
    } else {
        attr_name = qa;
    }
    auto out = std::make_shared<SetData>();
    try {
        auto inverses = file->getInverse(e->id(), nullptr, -1);
        if (!inverses) return Value(SetPtr{std::move(out)});
        for (auto& cand : *inverses) {
            if (!cand) continue;
            // Filter by entity type if specified.
            if (!entity_name.empty() && !cand->declaration().is(entity_name)) continue;
            // Verify the named attribute references our entity.
            if (!attr_name.empty()) {
                auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(cand);
                if (!be) continue;
                auto* d = be->declaration().as_entity();
                if (!d) continue;
                int idx;
                try { idx = static_cast<int>(d->attribute_index(attr_name)); }
                catch (...) { continue; }
                if (idx < 0) continue;
                bool matches = false;
                try {
                    auto av = cand->get_attribute_value(static_cast<size_t>(idx));
                    if (av.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
                        matches = ((IfcUtil::IfcBaseClass*)av) == e;
                    } else if (av.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                        auto agg = (aggregate_of_instance::ptr)av;
                        if (agg) for (auto& m : *agg) if (m == e) { matches = true; break; }
                    }
                } catch (...) {}
                if (!matches) continue;
            }
            EntityRef ref; ref.ptr = static_cast<void*>(cand);
            // Dedupe on raw pointer.
            bool dup = false;
            for (auto& it : *out)
                if (it.is_entity() && it.as_entity().ptr == ref.ptr) { dup = true; break; }
            if (!dup) out->emplace_back(ref);
        }
    } catch (...) {}
    return Value(SetPtr{std::move(out)});
}

/* ================================================================== */
/*  Range                                                             */
/* ================================================================== */

Value express_range(const Value& lo, const Value& hi) {
    if (lo.is_indeterminate() || hi.is_indeterminate()) return Value::make_list();
    if (!lo.is_int() || !hi.is_int()) return Value::make_list();
    auto out = std::make_shared<ListData>();
    for (auto i = lo.as_int(); i <= hi.as_int(); ++i)
        out->emplace_back(i);
    return Value(ListPtr{std::move(out)});
}

/* ================================================================== */
/*  Entity construction                                               */
/* ================================================================== */

namespace {

// Per-thread, per-schema scratch IfcFile. Rule invocations need to build
// intermediate entities (IfcDirection / IfcVector / ...) for math; those
// must outlive the call but cleaning them up between invocations is not
// strictly necessary because rule code typically only allocates a few
// per call.
std::unordered_map<std::string, std::unique_ptr<IfcParse::IfcFile>>&
scratch_files_for_thread() {
    thread_local std::unordered_map<std::string, std::unique_ptr<IfcParse::IfcFile>> map;
    return map;
}

IfcParse::IfcFile* get_scratch_file(std::string_view schema_name) {
    std::string key(schema_name);
    auto& map = scratch_files_for_thread();
    auto it = map.find(key);
    if (it != map.end()) return it->second.get();
    const auto* schema = IfcParse::schema_by_name(key);
    if (!schema) return nullptr;
    auto file = std::make_unique<IfcParse::IfcFile>(schema);
    auto* raw = file.get();
    map.emplace(std::move(key), std::move(file));
    return raw;
}

// Set a single attribute on `e` from a Value, dispatching on the
// declared parameter type. Returns true on success. Unsupported / type
// mismatched assignments silently fail (return false) — a rule that
// needs the attribute will simply observe INDETERMINATE downstream.
bool set_attr_from_value(IfcUtil::IfcBaseClass* e, size_t idx, const Value& v) {
    if (v.is_indeterminate()) return false;
    try {
        switch (v.tag()) {
            case Value::Tag::Bool:   e->set_attribute_value(idx, v.as_bool()); return true;
            case Value::Tag::Int:    e->set_attribute_value(idx, static_cast<int>(v.as_int())); return true;
            case Value::Tag::Real:   e->set_attribute_value(idx, v.as_double()); return true;
            case Value::Tag::Str:    e->set_attribute_value(idx, v.as_string()); return true;
            case Value::Tag::Entity: {
                auto* p = static_cast<IfcUtil::IfcBaseClass*>(v.as_entity().ptr);
                if (p) e->set_attribute_value(idx, p);
                return p != nullptr;
            }
            case Value::Tag::List: {
                const auto& l = v.as_list();
                if (l.empty()) {
                    // Empty aggregate type-agnostic; default to vector<double>.
                    e->set_attribute_value(idx, std::vector<double>{});
                    return true;
                }
                // Dispatch on element type; assume homogeneous.
                switch (l[0].tag()) {
                    case Value::Tag::Real:
                    case Value::Tag::Int: {
                        std::vector<double> out;
                        out.reserve(l.size());
                        for (const auto& it : l) {
                            if (it.is_indeterminate()) return false;
                            out.push_back(it.is_int() ? static_cast<double>(it.as_int())
                                                      : it.as_double());
                        }
                        e->set_attribute_value(idx, out);
                        return true;
                    }
                    case Value::Tag::Str: {
                        std::vector<std::string> out;
                        out.reserve(l.size());
                        for (const auto& it : l) out.push_back(it.as_string());
                        e->set_attribute_value(idx, out);
                        return true;
                    }
                    case Value::Tag::Entity: {
                        auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
                        for (const auto& it : l) {
                            if (!it.is_entity()) return false;
                            agg->push(static_cast<IfcUtil::IfcBaseClass*>(it.as_entity().ptr));
                        }
                        e->set_attribute_value(idx, agg);
                        return true;
                    }
                    default: return false;
                }
            }
            default: return false;
        }
    } catch (...) {
        return false;
    }
}

}  // namespace

Value make_entity(std::string_view schema_name,
                  std::string_view type_name,
                  std::initializer_list<std::pair<std::string_view, Value>> attrs) {
    auto* file = get_scratch_file(schema_name);
    if (!file) return Indeterminate{};
    const auto* schema = file->schema();
    if (!schema) return Indeterminate{};
    std::string tn(type_name);
    const IfcParse::declaration* decl;
    try { decl = schema->declaration_by_name(tn); }
    catch (...) { return Indeterminate{}; }
    if (!decl || !decl->as_entity()) return Indeterminate{};
    IfcUtil::IfcBaseClass* e;
    try { e = file->create(decl); }
    catch (...) { return Indeterminate{}; }
    if (!e) return Indeterminate{};
    auto* ent = decl->as_entity();
    for (const auto& kv : attrs) {
        std::string name(kv.first);
        int idx;
        try { idx = static_cast<int>(ent->attribute_index(name)); }
        catch (...) { continue; }
        if (idx < 0) continue;
        set_attr_from_value(e, static_cast<size_t>(idx), kv.second);
    }
    EntityRef ref;
    ref.ptr = static_cast<void*>(e);
    return ref;
}

/* ================================================================== */
/*  Math                                                              */
/* ================================================================== */

#define IFCAPI_DEFINE_MATH1(NAME, FN)                                    \
    Value NAME(const Value& v) {                                         \
        if (v.is_indeterminate() || !v.is_number()) return Indeterminate{}; \
        return FN(v.as_double());                                        \
    }
IFCAPI_DEFINE_MATH1(math_sqrt, std::sqrt)
IFCAPI_DEFINE_MATH1(math_sin,  std::sin)
IFCAPI_DEFINE_MATH1(math_cos,  std::cos)
IFCAPI_DEFINE_MATH1(math_tan,  std::tan)
IFCAPI_DEFINE_MATH1(math_asin, std::asin)
IFCAPI_DEFINE_MATH1(math_acos, std::acos)
IFCAPI_DEFINE_MATH1(math_atan, std::atan)
IFCAPI_DEFINE_MATH1(math_log,  std::log)
IFCAPI_DEFINE_MATH1(math_exp,  std::exp)
#undef IFCAPI_DEFINE_MATH1

Value math_atan2(const Value& y, const Value& x) {
    if (y.is_indeterminate() || x.is_indeterminate()) return Indeterminate{};
    if (!y.is_number() || !x.is_number()) return Indeterminate{};
    return std::atan2(y.as_double(), x.as_double());
}

Value math_pow(const Value& b, const Value& e) {
    if (b.is_indeterminate() || e.is_indeterminate()) return Indeterminate{};
    if (!b.is_number() || !e.is_number()) return Indeterminate{};
    return std::pow(b.as_double(), e.as_double());
}

Value math_abs(const Value& v) {
    if (v.is_indeterminate()) return Indeterminate{};
    if (v.is_int())  return static_cast<std::int64_t>(std::llabs(v.as_int()));
    if (v.is_real()) return std::fabs(v.as_double());
    return Indeterminate{};
}

}  // namespace express
}  // namespace ifcapi
