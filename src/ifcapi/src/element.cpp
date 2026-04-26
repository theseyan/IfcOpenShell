// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/element.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <set>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

namespace {

IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

IfcParse::IfcFile* as_file(const ifcopenshell_ifc_file_t* file) {
    return file->ptr;
}

bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

int32_t id_of(IfcUtil::IfcBaseClass* e) {
    return e ? static_cast<int32_t>(e->id()) : 0;
}

// Get all inverse entities for a given attribute name (e.g. "IsTypedBy").
aggregate_of_instance::ptr get_inverse(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    try {
        return be->get_inverse(attr);
    } catch (...) {
        return nullptr;
    }
}

// Read a direct forward attribute that is a reference.
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

// Read a direct forward attribute that is an aggregate of references.
std::vector<IfcUtil::IfcBaseClass*> read_ref_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return out;
    auto* d = be->declaration().as_entity();
    if (!d) return out;
    int idx = d->attribute_index(attr);
    if (idx < 0) return out;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return out;
        auto agg = (aggregate_of_instance::ptr)val;
        if (!agg) return out;
        for (auto& item : *agg) out.push_back(item);
    } catch (...) {}
    return out;
}

ifcopenshell_ifc_instance_t** alloc_id_handles(IfcParse::IfcFile* f, const std::vector<int32_t>& ids, uint32_t* out_count) {
    if (out_count) *out_count = static_cast<uint32_t>(ids.size());
    if (ids.empty()) return nullptr;
    auto* buf = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(ids.size() * sizeof(ifcopenshell_ifc_instance_t*)));
    if (!buf) {
        if (out_count) *out_count = 0;
        return nullptr;
    }
    for (size_t i = 0; i < ids.size(); ++i) {
        auto* e = f ? f->instance_by_id(ids[i]) : nullptr;
        buf[i] = ifcopenshell::capi::wrap_instance(e);
    }
    return buf;
}

// Forward decls
IfcUtil::IfcBaseClass* resolve_aggregate(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e);
IfcUtil::IfcBaseClass* resolve_nest(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e);

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
    // IFC2X3: walk IsDefinedBy looking for IfcRelDefinesByType
    auto inv = get_inverse(e, "IsDefinedBy");
    if (!inv) return nullptr;
    for (size_t i = 0; i < inv->size(); ++i) {
        if (is_a((*inv)[i], "IfcRelDefinesByType")) {
            return read_ref((*inv)[i], "RelatingType");
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* resolve_aggregate(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    auto inv = get_inverse(e, "Decomposes");
    if (!inv || inv->size() == 0) return nullptr;
    auto* rel = (*inv)[0];
    bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    if (is_ifc2x3 && !is_a(rel, "IfcRelAggregates")) return nullptr;
    return read_ref(rel, "RelatingObject");
}

IfcUtil::IfcBaseClass* resolve_nest(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    if (is_ifc2x3) {
        auto inv = get_inverse(e, "Decomposes");
        if (!inv || inv->size() == 0) return nullptr;
        auto* rel = (*inv)[0];
        if (!is_a(rel, "IfcRelNests")) return nullptr;
        return read_ref(rel, "RelatingObject");
    }
    auto inv = get_inverse(e, "Nests");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingObject");
}

IfcUtil::IfcBaseClass* resolve_filled_void(IfcUtil::IfcBaseClass* e) {
    // IfcElement.FillsVoids -> IfcRelFillsElement -> RelatingOpeningElement
    auto inv = get_inverse(e, "FillsVoids");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingOpeningElement");
}

IfcUtil::IfcBaseClass* resolve_voided_element(IfcUtil::IfcBaseClass* e) {
    // IfcOpeningElement.VoidsElements -> IfcRelVoidsElement -> RelatingBuildingElement
    auto inv = get_inverse(e, "VoidsElements");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingBuildingElement");
}

IfcUtil::IfcBaseClass* resolve_container_direct(IfcUtil::IfcBaseClass* e) {
    auto inv = get_inverse(e, "ContainedInStructure");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingStructure");
}

IfcUtil::IfcBaseClass* resolve_parent(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    if (auto* c = resolve_container_direct(e)) return c;
    if (auto* a = resolve_aggregate(file, e)) return a;
    if (auto* n = resolve_nest(file, e)) return n;
    if (auto* f = resolve_filled_void(e)) return f;
    if (auto* v = resolve_voided_element(e)) return v;
    return nullptr;
}

IfcUtil::IfcBaseClass* resolve_container(
    IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e,
    bool direct_only, const char* ifc_class)
{
    if (!e) return nullptr;
    if (direct_only) {
        auto* c = resolve_container_direct(e);
        if (!c) return nullptr;
        if (!ifc_class || *ifc_class == '\0') return c;
        return is_a(c, ifc_class) ? c : nullptr;
    }
    if (auto* c = resolve_container_direct(e)) {
        if (!ifc_class || *ifc_class == '\0') return c;
        while (c) {
            if (is_a(c, ifc_class)) return c;
            c = resolve_aggregate(file, c);
        }
        return nullptr;
    }
    if (auto* p = resolve_parent(file, e)) {
        return resolve_container(file, p, direct_only, ifc_class);
    }
    return nullptr;
}

}  // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* element_get_type(IfcUtil::IfcBaseClass* instance) {
    return instance ? resolve_type(instance->file_, instance) : nullptr;
}

IfcUtil::IfcBaseClass* element_get_aggregate(IfcUtil::IfcBaseClass* instance) {
    return instance ? resolve_aggregate(instance->file_, instance) : nullptr;
}

IfcUtil::IfcBaseClass* element_get_nest(IfcUtil::IfcBaseClass* instance) {
    return instance ? resolve_nest(instance->file_, instance) : nullptr;
}

IfcUtil::IfcBaseClass* element_get_container(
    IfcUtil::IfcBaseClass* instance,
    bool direct_only,
    const char* ifc_class)
{
    return instance ? resolve_container(instance->file_, instance, direct_only, ifc_class) : nullptr;
}

IfcUtil::IfcBaseClass* element_get_parent(IfcUtil::IfcBaseClass* instance) {
    return instance ? resolve_parent(instance->file_, instance) : nullptr;
}

IfcUtil::IfcBaseClass* element_get_material(
    IfcUtil::IfcBaseClass* instance,
    bool should_skip_usage,
    bool should_inherit)
{
    auto* f = instance ? instance->file_ : nullptr;
    if (!f || !instance) return nullptr;

    auto has_associations = get_inverse(instance, "HasAssociations");
    if (has_associations) {
        for (size_t i = 0; i < has_associations->size(); ++i) {
            auto* rel = (*has_associations)[i];
            if (!is_a(rel, "IfcRelAssociatesMaterial")) continue;
            auto* mat = read_ref(rel, "RelatingMaterial");
            if (!mat) continue;
            if (should_skip_usage) {
                if (is_a(mat, "IfcMaterialLayerSetUsage")) {
                    return read_ref(mat, "ForLayerSet");
                }
                if (is_a(mat, "IfcMaterialProfileSetUsage")) {
                    return read_ref(mat, "ForProfileSet");
                }
            }
            return mat;
        }
    }
    if (should_inherit) {
        auto* type_obj = resolve_type(f, instance);
        if (type_obj && type_obj != instance && get_inverse(type_obj, "HasAssociations")) {
            return element_get_material(type_obj, should_skip_usage, false);
        }
    }
    return nullptr;
}

}  // namespace bindings
}  // namespace ifcapi

extern "C" {

ifcopenshell_ifc_instance_t** ifcopenshell_element_get_decomposition(const ifcopenshell_ifc_instance_t* instance, bool is_recursive, uint32_t* out_count)
{
    if (out_count) *out_count = 0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return nullptr;

    std::set<int32_t> seen;
    std::vector<int32_t> result;
    std::deque<IfcUtil::IfcBaseClass*> queue;
    queue.push_back(e);

    auto push_all = [&](const std::vector<IfcUtil::IfcBaseClass*>& refs) {
        for (auto* r : refs) {
            int32_t rid = id_of(r);
            if (rid && seen.insert(rid).second) {
                result.push_back(rid);
                queue.push_back(r);
            }
        }
    };

    while (!queue.empty()) {
        auto* cur = queue.front();
        queue.pop_front();

        // ContainsElements: list of IfcRelContainedInSpatialStructure
        if (auto inv = get_inverse(cur, "ContainsElements")) {
            for (size_t i = 0; i < inv->size(); ++i) {
                push_all(read_ref_list((*inv)[i], "RelatedElements"));
            }
        }
        // IsDecomposedBy: list of IfcRelAggregates
        if (auto inv = get_inverse(cur, "IsDecomposedBy")) {
            for (size_t i = 0; i < inv->size(); ++i) {
                push_all(read_ref_list((*inv)[i], "RelatedObjects"));
            }
        }
        // HasOpenings: list of IfcRelVoidsElement
        if (auto inv = get_inverse(cur, "HasOpenings")) {
            for (size_t i = 0; i < inv->size(); ++i) {
                auto* opening = read_ref((*inv)[i], "RelatedOpeningElement");
                int32_t oid = id_of(opening);
                if (oid && seen.insert(oid).second) {
                    result.push_back(oid);
                    queue.push_back(opening);
                }
            }
        }
        // HasFillings: list of IfcRelFillsElement
        if (auto inv = get_inverse(cur, "HasFillings")) {
            for (size_t i = 0; i < inv->size(); ++i) {
                auto* filler = read_ref((*inv)[i], "RelatedBuildingElement");
                int32_t fid = id_of(filler);
                if (fid && seen.insert(fid).second) {
                    result.push_back(fid);
                    queue.push_back(filler);
                }
            }
        }
        // IsNestedBy: list of IfcRelNests (IFC4+), recursive-only per upstream
        if (is_recursive) {
            if (auto inv = get_inverse(cur, "IsNestedBy")) {
                for (size_t i = 0; i < inv->size(); ++i) {
                    push_all(read_ref_list((*inv)[i], "RelatedObjects"));
                }
            }
            // (loop continues)
        } else {
            break;
        }
    }

    return alloc_id_handles(instance ? instance->ptr->file_ : nullptr, result, out_count);
}

ifcopenshell_ifc_instance_t** ifcopenshell_element_get_pset_ids(const ifcopenshell_ifc_instance_t* instance,
    bool psets_only, bool qtos_only, bool should_inherit,
    uint32_t* out_count)
{
    if (out_count) *out_count = 0;
    auto* e_tmp = instance ? instance->ptr : nullptr; auto* f = e_tmp ? e_tmp->file_ : nullptr;
    auto* e = instance ? instance->ptr : nullptr;
    if (!f || !e) return nullptr;

    bool is_ifc2x3 = f->schema() && f->schema()->name() == "IFC2X3";
    std::vector<int32_t> result;
    std::set<int32_t> seen;

    auto push_def = [&](IfcUtil::IfcBaseClass* d) {
        if (!d) return;
        if (psets_only && !is_a(d, "IfcPropertySet") && !is_a(d, "IfcPreDefinedPropertySet")) return;
        if (qtos_only && !is_a(d, "IfcElementQuantity")) return;
        int32_t did = id_of(d);
        if (did && seen.insert(did).second) result.push_back(did);
    };

    // Case 1: IfcTypeObject -> HasPropertySets (forward aggregate).
    if (is_a(e, "IfcTypeObject")) {
        for (auto* d : read_ref_list(e, "HasPropertySets")) push_def(d);
        return alloc_id_handles(instance ? instance->ptr->file_ : nullptr, result, out_count);
    }

    // Case 2: Material/profile property containers.
    if ((is_ifc2x3 && is_a(e, "IfcMaterial")) ||
        is_a(e, "IfcMaterialDefinition") || is_a(e, "IfcProfileDef")) {
        if (qtos_only) return alloc_id_handles(instance ? instance->ptr->file_ : nullptr, result, out_count);
        if (is_ifc2x3 && is_a(e, "IfcMaterial")) {
            auto insts = f->instances_by_type(std::string("IfcExtendedMaterialProperties"));
            if (insts) {
                for (auto& inst : *insts) {
                    auto* mat = read_ref(inst, "Material");
                    if (mat == e) push_def(inst);
                }
            }
        } else if (!is_ifc2x3) {
            // HasProperties is an INVERSE attribute on IfcMaterial/IfcProfileDef.
            if (auto inv = get_inverse(e, "HasProperties")) {
                for (size_t i = 0; i < inv->size(); ++i) push_def((*inv)[i]);
            }
        }
        return alloc_id_handles(instance ? instance->ptr->file_ : nullptr, result, out_count);
    }

    // Case 3: Standard objects via IsDefinedBy. Inherited psets first so that
    // own psets with the same name override them in the caller's dict merge.
    auto is_defined_by = get_inverse(e, "IsDefinedBy");
    if (is_defined_by) {
        if (should_inherit) {
            auto* type_obj = resolve_type(f, e);
            if (type_obj) {
                uint32_t inherited_count = 0;
                auto* type_h = ifcopenshell::capi::wrap_instance(type_obj);
                ifcopenshell_ifc_instance_t** inherited = ifcopenshell_element_get_pset_ids(
                    type_h, psets_only, qtos_only, false, &inherited_count);
                if (inherited) {
                    for (uint32_t i = 0; i < inherited_count; ++i) {
                        push_def(inherited[i] ? inherited[i]->ptr : nullptr);
                    }
                    ifcopenshell_free_instance_array(inherited, inherited_count);
                }
                ifcopenshell_ifc_instance_destroy(type_h);
            }
        }
        for (size_t i = 0; i < is_defined_by->size(); ++i) {
            auto* rel = (*is_defined_by)[i];
            if (!is_a(rel, "IfcRelDefinesByProperties")) continue;
            push_def(read_ref(rel, "RelatingPropertyDefinition"));
        }
    }

    return alloc_id_handles(instance ? instance->ptr->file_ : nullptr, result, out_count);
}

}  // extern "C"
