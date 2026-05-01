// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/classification.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

inline bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

aggregate_of_instance::ptr get_inverse(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    try {
        return be->get_inverse(attr);
    } catch (...) {
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    auto* d = be->declaration().as_entity();
    if (!d) return nullptr;
    int idx = d->attribute_index(attr);
    if (idx < 0) return nullptr;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)val;
    } catch (...) {
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* resolve_type(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    if (is_a(e, "IfcTypeObject")) return e;
    bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    if (!is_ifc2x3) {
        auto inv = get_inverse(e, "IsTypedBy");
        if (inv && inv->size() > 0) {
            return read_ref((*inv)[0], "RelatingType");
        }
        return nullptr;
    }
    auto inv = get_inverse(e, "IsDefinedBy");
    if (!inv) return nullptr;
    for (size_t i = 0; i < inv->size(); ++i) {
        if (is_a((*inv)[i], "IfcRelDefinesByType")) {
            return read_ref((*inv)[i], "RelatingType");
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* get_classification(IfcUtil::IfcBaseClass* ref) {
    while (ref) {
        if (is_a(ref, "IfcClassification")) return ref;
        IfcUtil::IfcBaseClass* parent = read_ref(ref, "ReferencedSource");
        if (!parent) return nullptr;
        ref = parent;
    }
    return nullptr;
}

void collect_inherited(IfcUtil::IfcBaseClass* element, bool should_inherit,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& seen);

// Returns true if element is non-IfcRoot and we used the external-reference shortcut.
bool try_external_refs(IfcUtil::IfcBaseClass* element,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& seen) {
    if (is_a(element, "IfcRoot")) return false;
    if (auto refs = get_inverse(element, "HasExternalReferences")) {
        for (size_t i = 0; i < refs->size(); ++i) {
            auto* rr = read_ref((*refs)[i], "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    if (auto refs = get_inverse(element, "HasExternalReference")) {
        for (size_t i = 0; i < refs->size(); ++i) {
            auto* rr = read_ref((*refs)[i], "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    return false;
}

void collect_for_element(IfcUtil::IfcBaseClass* element, bool should_inherit,
                         std::vector<IfcUtil::IfcBaseClass*>& out) {
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;

    // Non-IfcRoot shortcut: HasExternalReferences / HasExternalReference.
    if (try_external_refs(element, out, seen)) return;

    // Inherited from type.
    std::vector<IfcUtil::IfcBaseClass*> inherited;
    auto* file = element ? element->file_ : nullptr;
    if (should_inherit && is_a(element, "IfcObject")) {
        auto* type_obj = resolve_type(file, element);
        if (type_obj && type_obj != element) {
            std::unordered_set<IfcUtil::IfcBaseClass*> _;
            collect_inherited(type_obj, /*should_inherit=*/true, inherited, _);
        }
    }

    // Occurrence references via HasAssociations.
    std::vector<IfcUtil::IfcBaseClass*> occurrence;
    {
        std::unordered_set<IfcUtil::IfcBaseClass*> occ_seen;
        auto rels = get_inverse(element, "HasAssociations");
        if (rels) {
            for (size_t i = 0; i < rels->size(); ++i) {
                auto* rel = (*rels)[i];
                if (!is_a(rel, "IfcRelAssociatesClassification")) continue;
                auto* rc = read_ref(rel, "RelatingClassification");
                if (rc && occ_seen.insert(rc).second) occurrence.push_back(rc);
            }
        }
    }

    if (!inherited.empty()) {
        // Group both lists by classification system; occurrence wins per system.
        std::unordered_map<IfcUtil::IfcBaseClass*, std::vector<IfcUtil::IfcBaseClass*>> per_system;
        std::vector<IfcUtil::IfcBaseClass*> system_order;
        auto add_to = [&](IfcUtil::IfcBaseClass* ref) {
            auto* cls = get_classification(ref);
            auto it = per_system.find(cls);
            if (it == per_system.end()) {
                per_system.emplace(cls, std::vector<IfcUtil::IfcBaseClass*>{ref});
                system_order.push_back(cls);
            } else {
                it->second.push_back(ref);
            }
        };
        for (auto* r : inherited) add_to(r);
        // Occurrence overrides per system.
        std::unordered_map<IfcUtil::IfcBaseClass*, std::vector<IfcUtil::IfcBaseClass*>> occ_per_system;
        std::vector<IfcUtil::IfcBaseClass*> occ_order;
        for (auto* r : occurrence) {
            auto* cls = get_classification(r);
            auto it = occ_per_system.find(cls);
            if (it == occ_per_system.end()) {
                occ_per_system.emplace(cls, std::vector<IfcUtil::IfcBaseClass*>{r});
                occ_order.push_back(cls);
            } else {
                it->second.push_back(r);
            }
        }
        for (auto* cls : occ_order) {
            per_system[cls] = occ_per_system[cls];
            if (std::find(system_order.begin(), system_order.end(), cls) == system_order.end()) {
                system_order.push_back(cls);
            }
        }
        std::unordered_set<IfcUtil::IfcBaseClass*> dedup;
        for (auto* cls : system_order) {
            for (auto* v : per_system[cls]) {
                if (dedup.insert(v).second) out.push_back(v);
            }
        }
        return;
    }

    for (auto* r : occurrence) out.push_back(r);
}

void collect_inherited(IfcUtil::IfcBaseClass* element, bool should_inherit,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& /*seen*/) {
    // Mirrors collect_for_element on the type element.
    collect_for_element(element, should_inherit, out);
}

}  // namespace

namespace ifcapi {
namespace bindings {

aggregate_of_instance::ptr classification_get_references(IfcUtil::IfcBaseClass* element, bool should_inherit) {
    aggregate_of_instance::ptr result(new aggregate_of_instance);
    if (!element) return result;

    std::vector<IfcUtil::IfcBaseClass*> items;
    collect_for_element(element, should_inherit, items);
    for (auto* item : items) {
        if (item) result->push(item);
    }
    return result;
}

} // namespace bindings
} // namespace ifcapi
