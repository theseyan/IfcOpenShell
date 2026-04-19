// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

#include <cstdlib>
#include <deque>
#include <unordered_set>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

aggregate_of_instance::ptr get_inverse(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    try { return be->get_inverse(attr); } catch (...) { return nullptr; }
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
    } catch (...) { return nullptr; }
}

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

void collect_materials(IfcUtil::IfcBaseClass* mat, std::vector<IfcUtil::IfcBaseClass*>& out) {
    if (!mat) return;
    if (is_a(mat, "IfcMaterial")) { out.push_back(mat); return; }
    if (is_a(mat, "IfcMaterialLayerSet")) {
        for (auto* l : read_ref_list(mat, "MaterialLayers")) {
            auto* m = read_ref(l, "Material");
            if (m) out.push_back(m);
        }
        return;
    }
    if (is_a(mat, "IfcMaterialProfileSet")) {
        for (auto* p : read_ref_list(mat, "MaterialProfiles")) {
            auto* m = read_ref(p, "Material");
            if (m) out.push_back(m);
        }
        return;
    }
    if (is_a(mat, "IfcMaterialConstituentSet")) {
        for (auto* c : read_ref_list(mat, "MaterialConstituents")) {
            auto* m = read_ref(c, "Material");
            if (m) out.push_back(m);
        }
        return;
    }
    if (is_a(mat, "IfcMaterialList")) {
        for (auto* m : read_ref_list(mat, "Materials")) {
            if (m) out.push_back(m);
        }
        return;
    }
}

}  // namespace

extern "C" {

IFCAPI_EXPORT ifcopenshell_ifc_instance_t** ifcopenshell_util_element_get_styles(
    ifcopenshell_ifc_instance_t* element, uint32_t* out_count)
{
    if (out_count) *out_count = 0;
    auto* e = element ? element->ptr : nullptr;
    if (!e) return nullptr;

    std::vector<IfcUtil::IfcBaseClass*> styles;

    // 1. Styles from materials' representations.
    {
        auto* mat_h = ifcopenshell_element_get_material(element, /*should_skip_usage=*/true, /*should_inherit=*/true);
        std::vector<IfcUtil::IfcBaseClass*> materials;
        if (mat_h) {
            collect_materials(mat_h->ptr, materials);
            ifcopenshell_ifc_instance_destroy(mat_h);
        }
        for (auto* material : materials) {
            for (auto* mdr : read_ref_list(material, "HasRepresentation")) {
                for (auto* rep : read_ref_list(mdr, "Representations")) {
                    for (auto* item : read_ref_list(rep, "Items")) {
                        for (auto* style : read_ref_list(item, "Styles")) {
                            if (is_a(style, "IfcSurfaceStyle")) styles.push_back(style);
                        }
                    }
                }
            }
        }
    }

    // 2. Styles from the body representation, recursing through MappedItem and BooleanResult.
    auto* body_h = ifcopenshell_representation_get_product_representation(
        nullptr, element, nullptr, "Model", "Body", "MODEL_VIEW");
    if (body_h) {
        auto* body = body_h->ptr;
        std::deque<IfcUtil::IfcBaseClass*> queue;
        for (auto* item : read_ref_list(body, "Items")) queue.push_back(item);
        while (!queue.empty()) {
            auto* item = queue.back();
            queue.pop_back();
            if (is_a(item, "IfcMappedItem")) {
                auto* src = read_ref(item, "MappingSource");
                if (src) {
                    auto* mapped = read_ref(src, "MappedRepresentation");
                    if (mapped) {
                        for (auto* sub : read_ref_list(mapped, "Items")) queue.push_back(sub);
                    }
                }
            }
            if (is_a(item, "IfcBooleanResult")) {
                if (auto* a = read_ref(item, "FirstOperand")) queue.push_back(a);
                if (auto* b = read_ref(item, "SecondOperand")) queue.push_back(b);
            }
            auto styled_by = get_inverse(item, "StyledByItem");
            if (styled_by && styled_by->size() > 0) {
                auto* sbi = (*styled_by)[0];
                for (auto* style : read_ref_list(sbi, "Styles")) {
                    if (is_a(style, "IfcSurfaceStyle")) styles.push_back(style);
                }
            }
        }
        ifcopenshell_ifc_instance_destroy(body_h);
    }

    if (styles.empty()) return nullptr;

    auto** buf = static_cast<ifcopenshell_ifc_instance_t**>(
        std::malloc(styles.size() * sizeof(ifcopenshell_ifc_instance_t*)));
    if (!buf) return nullptr;
    for (size_t i = 0; i < styles.size(); ++i) {
        buf[i] = ifcopenshell::capi::wrap_instance(styles[i]);
    }
    if (out_count) *out_count = static_cast<uint32_t>(styles.size());
    return buf;
}

}  // extern "C"
