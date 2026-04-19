// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "entity_introspection.hpp"
#include "placement_helpers.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"

#include <array>
#include <cstring>
#include <string>
#include <vector>

namespace {

using ifcapi::entity_is_a;
using ifcapi::find_attr_idx;
using ifcapi::has_attr;
using ifcapi::get_entity_ref;
using ifcapi::get_entity_list;
using ifcapi::get_all_inverses;
using ifcapi::set_entity_ref;
using ifcapi::identity4;
using ifcapi::a2p;
using ifcapi::matmul4;
using ifcapi::invert4;
using ifcapi::compute_local_placement;

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

void set_double_vec(IfcUtil::IfcBaseClass* e, const char* attr, const std::vector<double>& v) {
    int i = find_attr_idx(e, attr);
    if (i < 0) return;
    e->set_attribute_value(static_cast<size_t>(i), v);
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    return get_all_inverses(file, e).size();
}

// True iff `product` appears at attribute `attr` of `relation` (handles both
// single-reference and aggregate-of-references attributes).
bool relation_relates_to(IfcUtil::IfcBaseClass* relation, const char* attr,
                         IfcUtil::IfcBaseClass* product) {
    int i = find_attr_idx(relation, attr);
    if (i < 0) return false;
    try {
        auto v = relation->get_attribute_value(static_cast<size_t>(i));
        if (v.isNull()) return false;
        try {
            auto* single = (IfcUtil::IfcBaseClass*)v;
            if (single == product) return true;
        } catch (...) {}
        try {
            auto agg = (aggregate_of_instance::ptr)v;
            if (agg) {
                for (auto& it : *agg) if (it == product) return true;
            }
        } catch (...) {}
    } catch (...) {}
    return false;
}

IfcUtil::IfcBaseClass* find_relating(
    IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* product,
    const char* relation_class, const char* related_attr, const char* relating_attr)
{
    for (auto* inv : get_all_inverses(file, product)) {
        if (!entity_is_a(inv, relation_class)) continue;
        if (!relation_relates_to(inv, related_attr, product)) continue;
        return get_entity_ref(inv, relating_attr);
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* compute_placement_rel_to(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* product) {
    if (auto* r = find_relating(file, product, "IfcRelAggregates", "RelatedObjects", "RelatingObject"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelNests", "RelatedObjects", "RelatingObject"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelConnectsPortToElement", "RelatingPort", "RelatedElement"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelVoidsElement", "RelatedOpeningElement", "RelatingBuildingElement"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelFillsElement", "RelatedBuildingElement", "RelatingOpeningElement"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelProjectsElement", "RelatedFeatureElement", "RelatingElement"))
        return get_entity_ref(r, "ObjectPlacement");
    if (auto* r = find_relating(file, product, "IfcRelAdheresToElement", "RelatedSurfaceFeatures", "RelatingElement"))
        return get_entity_ref(r, "ObjectPlacement");
    // ContainedInStructure: relating side is the structure itself.
    for (auto* inv : get_all_inverses(file, product)) {
        if (!entity_is_a(inv, "IfcRelContainedInSpatialStructure")) continue;
        if (!relation_relates_to(inv, "RelatedElements", product)) continue;
        if (auto* s = get_entity_ref(inv, "RelatingStructure"))
            return get_entity_ref(s, "ObjectPlacement");
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* type_name) {
    return file->create(file->schema()->declaration_by_name(type_name));
}

struct ChildJob {
    IfcUtil::IfcBaseClass* product;
    std::array<double, 16> matrix;
};

// Walk old_placement.ReferencedByPlacements -> referenced IfcLocalPlacements ->
// products whose ObjectPlacement is that placement. Skip IfcDistributionPort
// (always travels with parent); for IfcFeatureElement descend one level so
// subchildren stay put while the feature itself follows the parent.
std::vector<ChildJob> get_children_settings(
    IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* old_placement)
{
    std::vector<ChildJob> out;
    if (!old_placement) return out;
    for (auto* ref_pl : get_all_inverses(file, old_placement)) {
        if (!entity_is_a(ref_pl, "IfcLocalPlacement")) continue;
        if (get_entity_ref(ref_pl, "PlacementRelTo") != old_placement) continue;
        std::array<double, 16> wm;
        if (!compute_local_placement(ref_pl, wm.data())) continue;
        for (auto* obj : get_all_inverses(file, ref_pl)) {
            if (!has_attr(obj, "ObjectPlacement")) continue;
            if (get_entity_ref(obj, "ObjectPlacement") != ref_pl) continue;
            if (entity_is_a(obj, "IfcDistributionPort")) continue;
            if (entity_is_a(obj, "IfcFeatureElement")) {
                auto* placement2 = get_entity_ref(obj, "ObjectPlacement");
                if (!placement2) continue;
                for (auto* ref_pl2 : get_all_inverses(file, placement2)) {
                    if (!entity_is_a(ref_pl2, "IfcLocalPlacement")) continue;
                    if (get_entity_ref(ref_pl2, "PlacementRelTo") != placement2) continue;
                    std::array<double, 16> wm2;
                    if (!compute_local_placement(ref_pl2, wm2.data())) continue;
                    for (auto* obj2 : get_all_inverses(file, ref_pl2)) {
                        if (!has_attr(obj2, "ObjectPlacement")) continue;
                        if (get_entity_ref(obj2, "ObjectPlacement") != ref_pl2) continue;
                        out.push_back({obj2, wm2});
                    }
                }
                continue;
            }
            out.push_back({obj, wm});
        }
    }
    return out;
}

IfcUtil::IfcBaseClass* edit_placement_impl(
    ifcopenshell_ifc_file_t* file_h,
    IfcUtil::IfcBaseClass* product,
    const std::array<double, 16>& matrix_in,
    bool is_si,
    bool should_transform_children)
{
    auto* file = file_h->ptr;
    if (!has_attr(product, "ObjectPlacement")) return nullptr;

    double unit_scale = ifcopenshell_util_unit_calculate_unit_scale(file_h, "LENGTHUNIT");
    if (unit_scale == 0.0) unit_scale = 1.0;

    std::array<double, 16> m = matrix_in;
    if (!is_si) {
        m[3] *= unit_scale;
        m[7] *= unit_scale;
        m[11] *= unit_scale;
    }

    IfcUtil::IfcBaseClass* old_placement = get_entity_ref(product, "ObjectPlacement");

    std::vector<ChildJob> children;
    if (!should_transform_children) {
        children = get_children_settings(file, old_placement);
    }

    IfcUtil::IfcBaseClass* placement_rel_to = compute_placement_rel_to(file, product);

    std::array<double, 16> rel_to_mat;
    if (placement_rel_to) {
        if (!compute_local_placement(placement_rel_to, rel_to_mat.data())) identity4(rel_to_mat.data());
        rel_to_mat[3] *= unit_scale;
        rel_to_mat[7] *= unit_scale;
        rel_to_mat[11] *= unit_scale;
    } else {
        identity4(rel_to_mat.data());
    }

    double o[3] = {m[3], m[7], m[11]};
    double z[3] = {m[2], m[6], m[10]};
    double x[3] = {m[0], m[4], m[8]};
    std::array<double, 16> obj_mat;
    a2p(o, z, x, obj_mat.data());

    std::array<double, 16> inv;
    if (!invert4(rel_to_mat.data(), inv.data())) identity4(inv.data());
    std::array<double, 16> rel_mat;
    matmul4(inv.data(), obj_mat.data(), rel_mat.data());

    std::vector<double> po = {rel_mat[3] / unit_scale, rel_mat[7] / unit_scale, rel_mat[11] / unit_scale};
    std::vector<double> pz = {rel_mat[2], rel_mat[6], rel_mat[10]};
    std::vector<double> px = {rel_mat[0], rel_mat[4], rel_mat[8]};

    auto* point = create_entity(file, "IfcCartesianPoint");
    set_double_vec(point, "Coordinates", po);
    auto* dz = create_entity(file, "IfcDirection");
    set_double_vec(dz, "DirectionRatios", pz);
    auto* dx = create_entity(file, "IfcDirection");
    set_double_vec(dx, "DirectionRatios", px);
    auto* axp = create_entity(file, "IfcAxis2Placement3D");
    set_entity_ref(axp, "Location", point);
    set_entity_ref(axp, "Axis", dz);
    set_entity_ref(axp, "RefDirection", dx);
    auto* new_placement = create_entity(file, "IfcLocalPlacement");
    set_entity_ref(new_placement, "RelativePlacement", axp);

    if (old_placement) {
        for (auto* inv_p : get_all_inverses(file, old_placement)) {
            if (!entity_is_a(inv_p, "IfcLocalPlacement")) continue;
            if (get_entity_ref(inv_p, "PlacementRelTo") == old_placement) {
                set_entity_ref(inv_p, "PlacementRelTo", new_placement);
            }
        }
        if (total_inverses(file, old_placement) == 1) {
            set_entity_ref(product, "ObjectPlacement", nullptr);
            set_entity_ref(old_placement, "PlacementRelTo", nullptr);
            auto* h = ifcopenshell::capi::wrap_instance(old_placement);
            ifcopenshell_util_remove_deep2(h);
            ifcopenshell_ifc_instance_destroy(h);
        }
    }

    set_entity_ref(new_placement, "PlacementRelTo", placement_rel_to);
    set_entity_ref(product, "ObjectPlacement", new_placement);

    for (auto& cj : children) {
        edit_placement_impl(file_h, cj.product, cj.matrix, false, true);
    }

    return new_placement;
}

}  // namespace

extern "C" {

IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_geometry_edit_object_placement(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* product_h,
    const double* matrix,
    bool is_si,
    bool should_transform_children)
{
    if (!file_h || !file_h->ptr || !product_h || !product_h->ptr) {
        set_error("ifcopenshell_api_geometry_edit_object_placement: missing argument");
        return nullptr;
    }
    auto* product = product_h->ptr;
    if (!has_attr(product, "ObjectPlacement")) return nullptr;

    std::array<double, 16> m;
    if (matrix) {
        std::memcpy(m.data(), matrix, 16 * sizeof(double));
    } else {
        identity4(m.data());
    }
    try {
        auto* result = edit_placement_impl(file_h, product, m, is_si, should_transform_children);
        if (!result) return nullptr;
        return ifcopenshell::capi::wrap_instance(result);
    } catch (const std::exception& ex) {
        set_error(std::string("edit_object_placement: ") + ex.what());
        return nullptr;
    } catch (...) {
        set_error("edit_object_placement: unknown error");
        return nullptr;
    }
}

}  // extern "C"
