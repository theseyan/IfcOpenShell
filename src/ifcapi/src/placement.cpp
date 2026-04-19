// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "entity_introspection.hpp"
#include "placement_helpers.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

using ifcapi::identity4;
using ifcapi::normalize3;
using ifcapi::a2p;
using ifcapi::matmul4;
using ifcapi::compute_local_placement;
using ifcapi::compute_axis2placement;

inline int attr_idx(IfcUtil::IfcBaseClass* e, const char* name) { return ifcapi::find_attr_idx(e, name); }
inline IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) { return ifcapi::get_entity_ref(e, attr); }
inline bool is_a(IfcUtil::IfcBaseClass* e, const char* name) { return ifcapi::entity_is_a(e, name); }
inline bool read_double_vec(IfcUtil::IfcBaseClass* e, const char* attr, double out[3]) { return ifcapi::read_double_vec3(e, attr, out); }
inline bool read_double_attr(IfcUtil::IfcBaseClass* e, const char* attr, double* out) { return ifcapi::get_double_attr(e, attr, out); }

bool compute_cart_xform_3d(IfcUtil::IfcBaseClass* e, double* out) {
    if (!e || !is_a(e, "IfcCartesianTransformationOperator3D")) {
        identity4(out);
        return false;
    }
    double origin[3] = {0, 0, 0};
    double a1[3] = {1, 0, 0};
    double a2[3] = {0, 1, 0};
    double a3[3] = {0, 0, 1};

    if (auto* loc = read_ref(e, "LocalOrigin")) read_double_vec(loc, "Coordinates", origin);
    if (auto* ax = read_ref(e, "Axis1")) read_double_vec(ax, "DirectionRatios", a1);
    if (auto* ax = read_ref(e, "Axis2")) read_double_vec(ax, "DirectionRatios", a2);
    if (auto* ax = read_ref(e, "Axis3")) read_double_vec(ax, "DirectionRatios", a3);

    a2p(origin, a3, a1, out);

    // Recover derived Y (column 1) and check orientation vs supplied axis2; if opposed, mirror.
    double derived_y[3] = {out[1], out[5], out[9]};
    double dot = derived_y[0] * a2[0] + derived_y[1] * a2[1] + derived_y[2] * a2[2];
    if (dot < 0.0) {
        out[1] = -out[1];
        out[5] = -out[5];
        out[9] = -out[9];
    }

    double s1 = 1.0, s2 = 1.0, s3 = 1.0;
    double scale;
    if (read_double_attr(e, "Scale", &scale)) {
        s1 = s2 = s3 = scale;
    }
    if (is_a(e, "IfcCartesianTransformationOperator3DnonUniform")) {
        double sc;
        if (read_double_attr(e, "Scale2", &sc)) s2 = sc;
        if (read_double_attr(e, "Scale3", &sc)) s3 = sc;
    }
    // Scale columns
    out[0] *= s1; out[4] *= s1; out[8] *= s1;
    out[1] *= s2; out[5] *= s2; out[9] *= s2;
    out[2] *= s3; out[6] *= s3; out[10] *= s3;
    return true;
}

}  // namespace

namespace ifcapi {

bool compute_local_placement(IfcUtil::IfcBaseClass* e, double* out) {
    if (!e || !entity_is_a(e, "IfcLocalPlacement")) {
        identity4(out);
        return true;
    }
    double parent[16];
    auto* rel_to = get_entity_ref(e, "PlacementRelTo");
    if (rel_to) {
        if (!compute_local_placement(rel_to, parent)) return false;
    } else {
        identity4(parent);
    }
    auto* rel = get_entity_ref(e, "RelativePlacement");
    if (!rel) {
        std::memcpy(out, parent, 16 * sizeof(double));
        return true;
    }
    double local[16];
    if (!compute_axis2placement(rel, local)) return false;
    matmul4(parent, local, out);
    return true;
}

bool compute_axis2placement(IfcUtil::IfcBaseClass* e, double* out) {
    if (!e) {
        identity4(out);
        return false;
    }
    double z[3] = {0, 0, 1};
    double x[3] = {1, 0, 0};
    double o[3] = {0, 0, 0};

    if (entity_is_a(e, "IfcAxis2Placement3D") || entity_is_a(e, "IfcAxis2PlacementLinear")) {
        if (auto* axis = get_entity_ref(e, "Axis")) read_double_vec3(axis, "DirectionRatios", z);
        if (auto* refd = get_entity_ref(e, "RefDirection")) read_double_vec3(refd, "DirectionRatios", x);
        auto* loc = get_entity_ref(e, "Location");
        if (!loc) {
            identity4(out);
            return false;
        }
        // IfcAxis2PlacementLinear with IfcPointByDistanceExpression locations
        // requires geometry evaluation and is not handled here.
        if (find_attr_idx(loc, "Coordinates") < 0) {
            identity4(out);
            return false;
        }
        read_double_vec3(loc, "Coordinates", o);
    } else if (entity_is_a(e, "IfcAxis2Placement2D")) {
        if (auto* refd = get_entity_ref(e, "RefDirection")) {
            read_double_vec3(refd, "DirectionRatios", x);
        }
        if (auto* loc = get_entity_ref(e, "Location")) {
            double c[3] = {0, 0, 0};
            read_double_vec3(loc, "Coordinates", c);
            o[0] = c[0];
            o[1] = c[1];
            o[2] = 0.0;
        }
    } else if (entity_is_a(e, "IfcAxis1Placement")) {
        if (auto* axis = get_entity_ref(e, "Axis")) read_double_vec3(axis, "DirectionRatios", z);
        if (auto* loc = get_entity_ref(e, "Location")) read_double_vec3(loc, "Coordinates", o);
    } else {
        identity4(out);
        return false;
    }

    a2p(o, z, x, out);
    return true;
}

}  // namespace ifcapi

extern "C" {

void ifcopenshell_placement_a2p(const double* o, const double* z, const double* x, double* out) {
    if (!o || !z || !x || !out) return;
    a2p(o, z, x, out);
}

bool ifcopenshell_placement_get_axis2placement(const ifcopenshell_ifc_instance_t* instance, double* out) {
    if (!out) return false;
    identity4(out);
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    return compute_axis2placement(e, out);
}

bool ifcopenshell_placement_get_local_placement(const ifcopenshell_ifc_instance_t* instance, double* out) {
    if (!out) return false;
    identity4(out);
    if (!instance) return true;  // NULL placement -> identity
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    return compute_local_placement(e, out);
}

bool ifcopenshell_placement_get_cartesian_xform_3d(const ifcopenshell_ifc_instance_t* instance, double* out) {
    if (!out) return false;
    identity4(out);
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    return compute_cart_xform_3d(e, out);
}

bool ifcopenshell_placement_get_mappeditem_xform(const ifcopenshell_ifc_instance_t* instance, double* out) {
    if (!out) return false;
    identity4(out);
    auto* e = instance ? instance->ptr : nullptr;
    if (!e || !is_a(e, "IfcMappedItem")) return false;
    auto* src = read_ref(e, "MappingSource");
    auto* tgt = read_ref(e, "MappingTarget");
    if (!src || !tgt) return false;
    auto* origin = read_ref(src, "MappingOrigin");
    if (!origin) return false;
    double source_m[16];
    if (!compute_axis2placement(origin, source_m)) return false;
    if (!is_a(tgt, "IfcCartesianTransformationOperator3D")) {
        // 2D case not supported here; return source only.
        std::memcpy(out, source_m, 16 * sizeof(double));
        return false;
    }
    double target_m[16];
    if (!compute_cart_xform_3d(tgt, target_m)) return false;
    matmul4(target_m, source_m, out);
    return true;
}

bool ifcopenshell_placement_get_storey_elevation(const ifcopenshell_ifc_instance_t* instance, double* out) {
    if (!out) return false;
    *out = 0.0;
    auto* e = instance ? instance->ptr : nullptr;
    if (!e) return false;
    auto* placement = read_ref(e, "ObjectPlacement");
    if (placement) {
        double m[16];
        if (compute_local_placement(placement, m)) {
            // Z translation is at m[11] (row 2, col 3 in row-major 4x4).
            *out = m[11];
            return true;
        }
    }
    double elev;
    if (read_double_attr(e, "Elevation", &elev)) {
        *out = elev;
        return true;
    }
    return true;
}

void ifcopenshell_placement_rotation(double angle_rad, char axis, double* out) {
    if (!out) return;
    identity4(out);
    double c = std::cos(angle_rad);
    double s = std::sin(angle_rad);
    if (axis == 'X' || axis == 'x') {
        out[5] = c;  out[6] = -s;
        out[9] = s;  out[10] = c;
    } else if (axis == 'Y' || axis == 'y') {
        out[0] = c;   out[2] = s;
        out[8] = -s;  out[10] = c;
    } else if (axis == 'Z' || axis == 'z') {
        out[0] = c;  out[1] = -s;
        out[4] = s;  out[5] = c;
    }
}

}  // extern "C"
