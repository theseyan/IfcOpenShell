// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_ATTRIBUTE_H
#define IFCAPI_DETAIL_ATTRIBUTE_H

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcSchema.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace detail {

inline int find_attr_index(const IfcParse::entity* decl, const char* name) {
    if (!decl) {
        return -1;
    }
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline int attr_index_of(IfcUtil::IfcBaseClass* entity, const char* attr) {
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) {
        return -1;
    }
    return find_attr_index(base->declaration().as_entity(), attr);
}

inline bool entity_has_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    return attr_index_of(entity, attr) >= 0;
}

inline bool is_attr_null(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return true;
    }
    try {
        return entity->get_attribute_value(static_cast<size_t>(idx)).isNull();
    } catch (...) {
        return true;
    }
}

inline std::string read_string_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return std::string();
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return std::string();
        }
        return static_cast<std::string>(val);
    } catch (...) {
        return std::string();
    }
}

inline IfcUtil::IfcBaseClass* read_ref_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return nullptr;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) {
            return nullptr;
        }
        return static_cast<IfcUtil::IfcBaseClass*>(val);
    } catch (...) {
        return nullptr;
    }
}

inline std::vector<IfcUtil::IfcBaseClass*> get_ref_aggregate(IfcUtil::IfcBaseClass* entity, int attr_idx) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!entity || attr_idx < 0) {
        return result;
    }
    try {
        auto val = entity->get_attribute_value(static_cast<size_t>(attr_idx));
        if (val.isNull()) {
            return result;
        }
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& item : *agg) {
                result.push_back(item);
            }
        }
    } catch (...) {
    }
    return result;
}

inline std::vector<IfcUtil::IfcBaseClass*> read_ref_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr) {
    return get_ref_aggregate(entity, attr_index_of(entity, attr));
}

inline void set_ref_aggregate(
    IfcUtil::IfcBaseClass* entity,
    int attr_idx,
    const std::vector<IfcUtil::IfcBaseClass*>& refs)
{
    if (!entity || attr_idx < 0) {
        return;
    }
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* ref : refs) {
        agg->push(ref);
    }
    entity->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

inline bool write_ref_aggregate(
    IfcUtil::IfcBaseClass* entity,
    const char* attr,
    const std::vector<IfcUtil::IfcBaseClass*>& refs)
{
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    set_ref_aggregate(entity, idx, refs);
    return true;
}

inline void write_string_attr(IfcUtil::IfcBaseClass* entity, const char* attr, const std::string& value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

inline void write_blank_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

inline void set_ref(IfcUtil::IfcBaseClass* entity, int attr_idx, IfcUtil::IfcBaseClass* ref) {
    if (entity && attr_idx >= 0 && ref) {
        entity->set_attribute_value(static_cast<size_t>(attr_idx), ref);
    }
}

inline void write_ref_attr(IfcUtil::IfcBaseClass* entity, const char* attr, IfcUtil::IfcBaseClass* ref) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return;
    }
    if (ref) {
        entity->set_attribute_value(static_cast<size_t>(idx), ref);
    } else {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_ATTRIBUTE_H */
