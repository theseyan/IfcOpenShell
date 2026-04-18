// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

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

int attr_idx(IfcUtil::IfcBaseClass* e, const char* name) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return -1;
    auto* d = be->declaration().as_entity();
    if (!d) return -1;
    return d->attribute_index(name);
}

bool read_double_vec(IfcUtil::IfcBaseClass* e, const char* attr, double out[3]) {
    out[0] = out[1] = out[2] = 0.0;
    int idx = attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return false;
        auto vec = (std::vector<double>)val;
        for (size_t i = 0; i < std::min(vec.size(), size_t(3)); ++i) out[i] = vec[i];
        return true;
    } catch (...) {
        return false;
    }
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = attr_idx(e, attr);
    if (idx < 0) return nullptr;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)val;
    } catch (...) {
        return nullptr;
    }
}

bool read_double_attr(IfcUtil::IfcBaseClass* e, const char* attr, double* out) {
    int idx = attr_idx(e, attr);
    if (idx < 0) return false;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return false;
        *out = (double)val;
        return true;
    } catch (...) {
        return false;
    }
}

bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

void identity4(double* m) {
    std::memset(m, 0, 16 * sizeof(double));
    m[0] = m[5] = m[10] = m[15] = 1.0;
}

void normalize3(double v[3]) {
    double n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (n > 0.0) {
        v[0] /= n;
        v[1] /= n;
        v[2] /= n;
    }
}

// Computes a 4x4 placement matrix (row-major) from origin O, Z axis, X axis.
// Matches Python's a2p(o, z, x) semantics: places local axes into world.
void a2p(const double o[3], const double z[3], const double x[3], double* m) {
    double xn[3] = {x[0], x[1], x[2]};
    double zn[3] = {z[0], z[1], z[2]};
    normalize3(xn);
    normalize3(zn);
    double y[3] = {
        zn[1] * xn[2] - zn[2] * xn[1],
        zn[2] * xn[0] - zn[0] * xn[2],
        zn[0] * xn[1] - zn[1] * xn[0]
    };
    normalize3(y);
    // Row-major, matches numpy layout after .T:
    //   [ x0 y0 z0 ox ]
    //   [ x1 y1 z1 oy ]
    //   [ x2 y2 z2 oz ]
    //   [ 0  0  0  1  ]
    m[0] = xn[0]; m[1] = y[0]; m[2] = zn[0]; m[3] = o[0];
    m[4] = xn[1]; m[5] = y[1]; m[6] = zn[1]; m[7] = o[1];
    m[8] = xn[2]; m[9] = y[2]; m[10] = zn[2]; m[11] = o[2];
    m[12] = 0;    m[13] = 0;   m[14] = 0;    m[15] = 1;
}

void matmul4(const double* a, const double* b, double* out) {
    double t[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            double s = 0.0;
            for (int k = 0; k < 4; ++k) s += a[i * 4 + k] * b[k * 4 + j];
            t[i * 4 + j] = s;
        }
    }
    std::memcpy(out, t, 16 * sizeof(double));
}

bool compute_axis2placement(IfcUtil::IfcBaseClass* e, double* out);

bool compute_local_placement(IfcUtil::IfcBaseClass* e, double* out) {
    if (!e || !is_a(e, "IfcLocalPlacement")) {
        identity4(out);
        return true;
    }
    double parent[16];
    auto* rel_to = read_ref(e, "PlacementRelTo");
    if (rel_to) {
        if (!compute_local_placement(rel_to, parent)) return false;
    } else {
        identity4(parent);
    }
    auto* rel = read_ref(e, "RelativePlacement");
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

    if (is_a(e, "IfcAxis2Placement3D") || is_a(e, "IfcAxis2PlacementLinear")) {
        if (auto* axis = read_ref(e, "Axis")) read_double_vec(axis, "DirectionRatios", z);
        if (auto* refd = read_ref(e, "RefDirection")) read_double_vec(refd, "DirectionRatios", x);
        auto* loc = read_ref(e, "Location");
        if (!loc) {
            identity4(out);
            return false;
        }
        // For IfcCartesianPoint: Coordinates is vector of doubles.
        // For IfcAxis2PlacementLinear: Location is IfcPointByDistanceExpression (no Coordinates attribute)
        // -> that case requires geometry evaluation and is not supported natively; signal false.
        if (attr_idx(loc, "Coordinates") < 0) {
            identity4(out);
            return false;
        }
        read_double_vec(loc, "Coordinates", o);
    } else if (is_a(e, "IfcAxis2Placement2D")) {
        auto* refd = read_ref(e, "RefDirection");
        if (refd) {
            read_double_vec(refd, "DirectionRatios", x);
        }
        auto* loc = read_ref(e, "Location");
        if (loc) {
            double c[3] = {0, 0, 0};
            read_double_vec(loc, "Coordinates", c);
            o[0] = c[0];
            o[1] = c[1];
            o[2] = 0.0;
        }
    } else if (is_a(e, "IfcAxis1Placement")) {
        if (auto* axis = read_ref(e, "Axis")) read_double_vec(axis, "DirectionRatios", z);
        auto* loc = read_ref(e, "Location");
        if (loc) read_double_vec(loc, "Coordinates", o);
    } else {
        identity4(out);
        return false;
    }

    a2p(o, z, x, out);
    return true;
}

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
