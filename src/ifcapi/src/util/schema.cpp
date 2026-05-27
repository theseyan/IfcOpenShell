// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/schema.h"
#include "ifcapi/detail/error.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/argument_type.h"
#include "ifcparse/aggregate_of_instance.h"

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

const IfcParse::parameter_type* leaf_pt(const IfcParse::parameter_type* pt) {
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

const IfcParse::enumeration_type* attr_enum_type(const IfcParse::attribute* attr) {
    auto* pt = leaf_pt(attr ? attr->type_of_attribute() : nullptr);
    auto* nt = pt ? pt->as_named_type() : nullptr;
    auto* d  = nt ? nt->declared_type() : nullptr;
    return d ? d->as_enumeration_type() : nullptr;
}

struct Stashed {
    IfcUtil::ArgumentType type = IfcUtil::Argument_NULL;
    int                                       int_v = 0;
    bool                                      bool_v = false;
    boost::logic::tribool                     log_v = boost::logic::indeterminate;
    double                                    dbl_v = 0.0;
    std::string                               str_v;
    boost::dynamic_bitset<>                   bin_v;
    IfcUtil::IfcBaseClass*                    inst_v = nullptr;
    std::vector<int>                          agg_int;
    std::vector<double>                       agg_dbl;
    std::vector<std::string>                  agg_str;
    std::vector<boost::dynamic_bitset<>>      agg_bin;
    std::vector<IfcUtil::IfcBaseClass*>       agg_inst;
    std::vector<std::vector<int>>             agg_agg_int;
    std::vector<std::vector<double>>          agg_agg_dbl;
    std::vector<std::vector<IfcUtil::IfcBaseClass*>> agg_agg_inst;
};

bool stash_attribute(IfcUtil::IfcBaseClass* e, size_t idx, Stashed& out) {
    AttributeValue av;
    try { av = e->get_attribute_value(idx); }
    catch (...) { return false; }
    if (av.isNull()) return false;
    out.type = av.type();
    switch (out.type) {
        case IfcUtil::Argument_DERIVED:
        case IfcUtil::Argument_NULL:
            return false;
        case IfcUtil::Argument_INT:               out.int_v = (int)av; break;
        case IfcUtil::Argument_BOOL:              out.bool_v = (bool)av; break;
        case IfcUtil::Argument_LOGICAL:           out.log_v = (boost::logic::tribool)av; break;
        case IfcUtil::Argument_DOUBLE:            out.dbl_v = (double)av; break;
        case IfcUtil::Argument_STRING:            out.str_v = (std::string)av; break;
        case IfcUtil::Argument_BINARY:            out.bin_v = (boost::dynamic_bitset<>)av; break;
        case IfcUtil::Argument_ENUMERATION: {
            EnumerationReference er = (EnumerationReference)av;
            out.str_v = er.value() ? std::string(er.value()) : std::string();
            break;
        }
        case IfcUtil::Argument_ENTITY_INSTANCE:   out.inst_v = (IfcUtil::IfcBaseClass*)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_INT:    out.agg_int = (std::vector<int>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: out.agg_dbl = (std::vector<double>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_STRING: out.agg_str = (std::vector<std::string>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_BINARY: out.agg_bin = (std::vector<boost::dynamic_bitset<>>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
            auto agg = (boost::shared_ptr<aggregate_of_instance>)av;
            if (agg) {
                for (size_t i = 0; i < agg->size(); ++i) out.agg_inst.push_back((*agg)[i]);
            }
            break;
        }
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:    out.agg_agg_int = (std::vector<std::vector<int>>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE: out.agg_agg_dbl = (std::vector<std::vector<double>>)av; break;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
            auto agg = (boost::shared_ptr<aggregate_of_aggregate_of_instance>)av;
            if (agg) {
                int n = agg->size();
                auto it = agg->begin();
                for (int i = 0; i < n && it != agg->end(); ++i, ++it) {
                    std::vector<IfcUtil::IfcBaseClass*> row(it->begin(), it->end());
                    out.agg_agg_inst.push_back(std::move(row));
                }
            }
            break;
        }
        case IfcUtil::Argument_EMPTY_AGGREGATE:
        case IfcUtil::Argument_AGGREGATE_OF_EMPTY_AGGREGATE:
        case IfcUtil::Argument_UNKNOWN:
        default:
            return false;
    }
    return true;
}

bool apply_stashed(IfcUtil::IfcBaseClass* e, size_t idx, const IfcParse::attribute* new_attr,
                   const Stashed& s) {
    try {
        switch (s.type) {
            case IfcUtil::Argument_INT:    e->set_attribute_value(idx, s.int_v); return true;
            case IfcUtil::Argument_BOOL:   e->set_attribute_value(idx, s.bool_v); return true;
            case IfcUtil::Argument_LOGICAL:e->set_attribute_value(idx, s.log_v); return true;
            case IfcUtil::Argument_DOUBLE: e->set_attribute_value(idx, s.dbl_v); return true;
            case IfcUtil::Argument_STRING: e->set_attribute_value(idx, s.str_v); return true;
            case IfcUtil::Argument_BINARY: e->set_attribute_value(idx, s.bin_v); return true;
            case IfcUtil::Argument_ENUMERATION: {
                auto* en = attr_enum_type(new_attr);
                if (!en) return false;
                const auto& items = en->enumeration_items();
                if (std::find(items.begin(), items.end(), s.str_v) == items.end()) return false;
                e->set_attribute_value(idx, EnumerationReference(en, en->lookup_enum_offset(s.str_v)));
                return true;
            }
            case IfcUtil::Argument_ENTITY_INSTANCE: e->set_attribute_value(idx, s.inst_v); return true;
            case IfcUtil::Argument_AGGREGATE_OF_INT:    e->set_attribute_value(idx, s.agg_int); return true;
            case IfcUtil::Argument_AGGREGATE_OF_DOUBLE: e->set_attribute_value(idx, s.agg_dbl); return true;
            case IfcUtil::Argument_AGGREGATE_OF_STRING: e->set_attribute_value(idx, s.agg_str); return true;
            case IfcUtil::Argument_AGGREGATE_OF_BINARY: e->set_attribute_value(idx, s.agg_bin); return true;
            case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
                aggregate_of_instance::ptr agg(new aggregate_of_instance);
                for (auto* p : s.agg_inst) agg->push(p);
                e->set_attribute_value(idx, agg);
                return true;
            }
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:    e->set_attribute_value(idx, s.agg_agg_int); return true;
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE: e->set_attribute_value(idx, s.agg_agg_dbl); return true;
            case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
                aggregate_of_aggregate_of_instance::ptr agg(new aggregate_of_aggregate_of_instance);
                for (const auto& row : s.agg_agg_inst) {
                    std::vector<IfcUtil::IfcBaseClass*> r(row.begin(), row.end());
                    agg->push(r);
                }
                e->set_attribute_value(idx, agg);
                return true;
            }
            default:
                return false;
        }
    } catch (...) {
        return false;
    }
}

}  // namespace

IfcUtil::IfcBaseClass* reassign_class_impl(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const std::string& new_class)
{
    if (!element) { set_error("element is NULL"); return nullptr; }
    file = file ? file : element->file_;
    if (!file) { set_error("file is NULL"); return nullptr; }

    if (element->declaration().name() == new_class) {
        return element;
    }

    const auto* schema = file->schema();
    const IfcParse::declaration* new_decl_any = nullptr;
    try { new_decl_any = schema->declaration_by_name(new_class); }
    catch (...) {
        set_value_error(std::string("Class could not be changed to ") + new_class +
                        " as the class does not exist in schema " + (schema ? schema->name() : ""));
        return nullptr;
    }
    const auto* new_entity_decl = new_decl_any ? new_decl_any->as_entity() : nullptr;
    if (!new_entity_decl) {
        set_error(std::string("new_class is not an entity declaration: ") + new_class);
        return nullptr;
    }

    const auto* old_entity_decl = element->declaration().as_entity();
    int element_id = (int)element->id();

    std::unordered_map<std::string, Stashed> stash;
    if (old_entity_decl) {
        size_t n = old_entity_decl->attribute_count();
        for (size_t i = 0; i < n; ++i) {
            Stashed s;
            if (!stash_attribute(element, i, s)) continue;
            stash.emplace(old_entity_decl->attribute_by_index(i)->name(), std::move(s));
        }
    }

    struct InvPair { IfcUtil::IfcBaseClass* inst; int index; };
    std::vector<InvPair> inverse_pairs;
    try {
        auto inverses = file->getInverse(element_id, nullptr, -1);
        auto idxs = file->get_inverse_indices(element_id);
        if (inverses) {
            size_t n = inverses->size();
            for (size_t i = 0; i < n; ++i) {
                int idx = (i < idxs.size()) ? idxs[i] : -1;
                inverse_pairs.push_back({(*inverses)[i], idx});
            }
        }
    } catch (...) {}

    try {
        file->removeEntity(element);
    } catch (const std::exception& e) {
        set_error(std::string("Failed to remove old element: ") + e.what());
        return nullptr;
    }

    IfcUtil::IfcBaseClass* new_inst = nullptr;
    try {
        new_inst = schema->instantiate(new_entity_decl, in_memory_attribute_storage(new_entity_decl->attribute_count()));
        new_inst->file_ = nullptr;

        const auto& all_attrs = new_entity_decl->all_attributes();
        for (size_t i = 0; i < all_attrs.size(); ++i) {
            const auto* a = all_attrs[i];
            auto it = stash.find(a->name());
            if (it == stash.end()) continue;
            apply_stashed(new_inst, i, a, it->second);
        }

        new_inst = file->addEntity(new_inst, element_id);
    } catch (const std::exception& e) {
        set_error(std::string("Failed to create reassigned element: ") + e.what());
        return nullptr;
    }
    if (!new_inst) {
        set_error("Failed to create reassigned element");
        return nullptr;
    }

    for (const auto& pair : inverse_pairs) {
        if (!pair.inst || pair.index < 0) continue;
        try {
            AttributeValue cur = pair.inst->get_attribute_value((size_t)pair.index);
            auto t = cur.type();
            if (cur.isNull() || t == IfcUtil::Argument_NULL) {
                pair.inst->set_attribute_value((size_t)pair.index, new_inst);
            } else if (t == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE ||
                       t == IfcUtil::Argument_EMPTY_AGGREGATE) {
                aggregate_of_instance::ptr agg(new aggregate_of_instance);
                if (t == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                    auto src = (boost::shared_ptr<aggregate_of_instance>)cur;
                    if (src) {
                        for (size_t i = 0; i < src->size(); ++i) agg->push((*src)[i]);
                    }
                }
                agg->push(new_inst);
                pair.inst->set_attribute_value((size_t)pair.index, agg);
            } else if (t == IfcUtil::Argument_ENTITY_INSTANCE) {
                pair.inst->set_attribute_value((size_t)pair.index, new_inst);
            }
        } catch (...) {
            continue;
        }
    }

    return new_inst;
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* schema_reassign_class(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const std::string& new_class)
{
    return reassign_class_impl(file, element, new_class);
}

} // namespace bindings
} // namespace ifcapi
