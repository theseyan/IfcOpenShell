// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/schema.h"
#include "ifcapi/detail/error.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/argument_type.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/logic/tribool.hpp>
#include <boost/dynamic_bitset.hpp>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline void set_error(const std::string& msg) { ifcapi::detail::set_error(msg); }
inline void set_value_error(const std::string& msg) { ifcapi::detail::set_error(ifcapi::detail::ERROR_VALUE, msg); }

const ifcopenshell::parameter_type* leaf_pt(const ifcopenshell::parameter_type* pt) {
    while (pt) {
        if (auto* nt = pt->as_named_type()) {
            auto* d = nt->declared_type();
            if (d && d->as_type_declaration()) {
                pt = d->as_type_declaration()->declared_type();
                continue;
            }
        }
        return pt;
    }
    return nullptr;
}

const ifcopenshell::enumeration_type* attr_enum_type(const ifcopenshell::attribute* attr) {
    auto* pt = leaf_pt(attr ? attr->type_of_attribute() : nullptr);
    auto* nt = pt ? pt->as_named_type() : nullptr;
    auto* d  = nt ? nt->declared_type() : nullptr;
    return d ? d->as_enumeration_type() : nullptr;
}

struct Stashed {
    ifcopenshell::argument_type type = ifcopenshell::Argument_NULL;
    int                                       int_v = 0;
    bool                                      bool_v = false;
    boost::logic::tribool                     log_v = boost::logic::indeterminate;
    double                                    dbl_v = 0.0;
    std::string                               str_v;
    boost::dynamic_bitset<>                   bin_v;
    express::Base                             inst_v;
    std::vector<int>                          agg_int;
    std::vector<double>                       agg_dbl;
    std::vector<std::string>                  agg_str;
    std::vector<boost::dynamic_bitset<>>      agg_bin;
    std::vector<express::Base>                agg_inst;
    std::vector<std::vector<int>>             agg_agg_int;
    std::vector<std::vector<double>>          agg_agg_dbl;
    std::vector<std::vector<express::Base>>   agg_agg_inst;
};

bool stash_attribute(express::Base e, size_t idx, Stashed& out) {
    attribute_value av;
    try { av = e.get_attribute_value(idx); }
    catch (...) { return false; }
    if (av.isNull()) return false;
    out.type = av.type();
    switch (out.type) {
        case ifcopenshell::Argument_DERIVED:
        case ifcopenshell::Argument_NULL:
            return false;
        case ifcopenshell::Argument_INT:               out.int_v = (int)av; break;
        case ifcopenshell::Argument_BOOL:              out.bool_v = (bool)av; break;
        case ifcopenshell::Argument_LOGICAL:           out.log_v = (boost::logic::tribool)av; break;
        case ifcopenshell::Argument_DOUBLE:            out.dbl_v = (double)av; break;
        case ifcopenshell::Argument_STRING:            out.str_v = (std::string)av; break;
        case ifcopenshell::Argument_BINARY:            out.bin_v = (boost::dynamic_bitset<>)av; break;
        case ifcopenshell::Argument_ENUMERATION: {
            enumeration_reference er = (enumeration_reference)av;
            out.str_v = er.value() ? std::string(er.value()) : std::string();
            break;
        }
        case ifcopenshell::Argument_ENTITY_INSTANCE:   out.inst_v = (express::Base)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_INT:    out.agg_int = (std::vector<int>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE: out.agg_dbl = (std::vector<double>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_STRING: out.agg_str = (std::vector<std::string>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_BINARY: out.agg_bin = (std::vector<boost::dynamic_bitset<>>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE:
            out.agg_inst = (std::vector<express::Base>)av;
            break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:    out.agg_agg_int = (std::vector<std::vector<int>>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE: out.agg_agg_dbl = (std::vector<std::vector<double>>)av; break;
        case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE:
            out.agg_agg_inst = (std::vector<std::vector<express::Base>>)av;
            break;
        case ifcopenshell::Argument_EMPTY_AGGREGATE:
        case ifcopenshell::Argument_AGGREGATE_OF_EMPTY_AGGREGATE:
        case ifcopenshell::Argument_UNKNOWN:
        default:
            return false;
    }
    return true;
}

bool apply_stashed(express::Base e, size_t idx, const ifcopenshell::attribute* new_attr,
                   const Stashed& s) {
    try {
        switch (s.type) {
            case ifcopenshell::Argument_INT:    e.set_attribute_value(idx, s.int_v); return true;
            case ifcopenshell::Argument_BOOL:   e.set_attribute_value(idx, s.bool_v); return true;
            case ifcopenshell::Argument_LOGICAL:e.set_attribute_value(idx, s.log_v); return true;
            case ifcopenshell::Argument_DOUBLE: e.set_attribute_value(idx, s.dbl_v); return true;
            case ifcopenshell::Argument_STRING: e.set_attribute_value(idx, s.str_v); return true;
            case ifcopenshell::Argument_BINARY: e.set_attribute_value(idx, s.bin_v); return true;
            case ifcopenshell::Argument_ENUMERATION: {
                auto* en = attr_enum_type(new_attr);
                if (!en) return false;
                const auto& items = en->enumeration_items();
                if (std::find(items.begin(), items.end(), s.str_v) == items.end()) return false;
                e.set_attribute_value(idx, enumeration_reference(en, en->lookup_enum_offset(s.str_v)));
                return true;
            }
            case ifcopenshell::Argument_ENTITY_INSTANCE: e.set_attribute_value(idx, s.inst_v); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_INT:    e.set_attribute_value(idx, s.agg_int); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE: e.set_attribute_value(idx, s.agg_dbl); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_STRING: e.set_attribute_value(idx, s.agg_str); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_BINARY: e.set_attribute_value(idx, s.agg_bin); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE: e.set_attribute_value(idx, s.agg_inst); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:    e.set_attribute_value(idx, s.agg_agg_int); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE: e.set_attribute_value(idx, s.agg_agg_dbl); return true;
            case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: e.set_attribute_value(idx, s.agg_agg_inst); return true;
            default:
                return false;
        }
    } catch (...) {
        return false;
    }
}

}  // namespace

express::Base reassign_class_impl(
    ifcopenshell::file* file,
    express::Base element,
    const std::string& new_class)
{
    if (!element) { set_error("element is NULL"); return {}; }
    file = file ? file : element.file();
    if (!file) { set_error("file is NULL"); return {}; }

    if (element.declaration().name() == new_class) {
        return element;
    }

    const auto* schema = file->schema();
    const ifcopenshell::declaration* new_decl_any = nullptr;
    try { new_decl_any = schema->declaration_by_name(new_class); }
    catch (...) {
        set_value_error(std::string("Class could not be changed to ") + new_class +
                        " as the class does not exist in schema " + (schema ? schema->name() : ""));
        return {};
    }
    const auto* new_entity_decl = new_decl_any ? new_decl_any->as_entity() : nullptr;
    if (!new_entity_decl) {
        set_error(std::string("new_class is not an entity declaration: ") + new_class);
        return {};
    }

    const auto* old_entity_decl = element.declaration().as_entity();
    int element_id = (int)element.id();

    std::unordered_map<std::string, Stashed> stash;
    if (old_entity_decl) {
        size_t n = old_entity_decl->attribute_count();
        for (size_t i = 0; i < n; ++i) {
            Stashed s;
            if (!stash_attribute(element, i, s)) continue;
            stash.emplace(old_entity_decl->attribute_by_index(i)->name(), std::move(s));
        }
    }

    struct InvPair { express::Base inst; int index; };
    std::vector<InvPair> inverse_pairs;
    try {
        auto inverses = file->instances_by_reference(element_id);
        auto idxs = file->get_inverse_indices_by_id(element_id);
        for (size_t i = 0; i < inverses.size(); ++i) {
            int idx = (i < idxs.size()) ? idxs[i] : -1;
            inverse_pairs.push_back({inverses[i], idx});
        }
    } catch (...) {}

    try {
        file->remove_entity(element);
    } catch (const std::exception& e) {
        set_error(std::string("Failed to remove old element: ") + e.what());
        return {};
    }

    express::Base new_inst;
    try {
        new_inst = file->create(new_entity_decl, element_id);

        const auto& all_attrs = new_entity_decl->all_attributes();
        for (size_t i = 0; i < all_attrs.size(); ++i) {
            const auto* a = all_attrs[i];
            auto it = stash.find(a->name());
            if (it == stash.end()) continue;
            apply_stashed(new_inst, i, a, it->second);
        }

    } catch (const std::exception& e) {
        set_error(std::string("Failed to create reassigned element: ") + e.what());
        return {};
    }
    if (!new_inst) {
        set_error("Failed to create reassigned element");
        return {};
    }

    for (auto& pair : inverse_pairs) {
        if (!pair.inst || pair.index < 0) continue;
        try {
            attribute_value cur = pair.inst.get_attribute_value((size_t)pair.index);
            auto t = cur.type();
            if (cur.isNull() || t == ifcopenshell::Argument_NULL) {
                pair.inst.set_attribute_value((size_t)pair.index, new_inst);
            } else if (t == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE ||
                       t == ifcopenshell::Argument_EMPTY_AGGREGATE) {
                std::vector<express::Base> agg;
                if (t == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                    agg = (std::vector<express::Base>)cur;
                }
                agg.push_back(new_inst);
                pair.inst.set_attribute_value((size_t)pair.index, agg);
            } else if (t == ifcopenshell::Argument_ENTITY_INSTANCE) {
                pair.inst.set_attribute_value((size_t)pair.index, new_inst);
            }
        } catch (...) {
            continue;
        }
    }

    return new_inst;
}

namespace ifcapi {
namespace bindings {

express::Base schema_reassign_class(
    ifcopenshell::file* file,
    express::Base* element,
    const std::string& new_class)
{
    return reassign_class_impl(file, element ? *element : express::Base(), new_class);
}

} // namespace bindings
} // namespace ifcapi
