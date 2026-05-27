// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/detail/attribute.h"
#include "entity_introspection.hpp"
#include "placement_helpers.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/exception.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
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

inline int attr_idx(express::Base e, const char* name) { return ifcapi::find_attr_idx(e, name); }
inline express::Base read_ref(express::Base e, const char* attr) { return ifcapi::get_entity_ref(e, attr); }
inline bool is_a(express::Base e, const char* name) { return ifcapi::entity_is_a(e, name); }
inline bool read_double_vec(express::Base e, const char* attr, double out[3]) { return ifcapi::read_double_vec3(e, attr, out); }
inline bool read_double_attr(express::Base e, const char* attr, double* out) { return ifcapi::get_double_attr(e, attr, out); }

bool compute_cart_xform_3d(express::Base e, double* out) {
    if (!e || !is_a(e, "IfcCartesianTransformationOperator3D")) {
        identity4(out);
        return false;
    }
    double origin[3] = {0, 0, 0};
    double a1[3] = {1, 0, 0};
    double a2[3] = {0, 1, 0};
    double a3[3] = {0, 0, 1};

    if (auto loc = read_ref(e, "LocalOrigin")) read_double_vec(loc, "Coordinates", origin);
    if (auto ax = read_ref(e, "Axis1")) read_double_vec(ax, "DirectionRatios", a1);
    if (auto ax = read_ref(e, "Axis2")) read_double_vec(ax, "DirectionRatios", a2);
    if (auto ax = read_ref(e, "Axis3")) read_double_vec(ax, "DirectionRatios", a3);

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

bool compute_mappeditem_xform(express::Base e, double* out) {
    if (!e || !is_a(e, "IfcMappedItem")) return false;
    auto src = read_ref(e, "MappingSource");
    auto tgt = read_ref(e, "MappingTarget");
    if (!src || !tgt) return false;
    auto origin = read_ref(src, "MappingOrigin");
    if (!origin) return false;
    double source_m[16];
    if (!compute_axis2placement(origin, source_m)) return false;
    if (!is_a(tgt, "IfcCartesianTransformationOperator3D")) {
        std::memcpy(out, source_m, 16 * sizeof(double));
        return false;
    }
    double target_m[16];
    if (!compute_cart_xform_3d(tgt, target_m)) return false;
    matmul4(target_m, source_m, out);
    return true;
}

}  // namespace

namespace ifcapi {

bool compute_local_placement(express::Base e, double* out) {
    if (!e || !entity_is_a(e, "IfcLocalPlacement")) {
        identity4(out);
        return true;
    }
    double parent[16];
    auto rel_to = get_entity_ref(e, "PlacementRelTo");
    if (rel_to) {
        if (!compute_local_placement(rel_to, parent)) return false;
    } else {
        identity4(parent);
    }
    auto rel = get_entity_ref(e, "RelativePlacement");
    if (!rel) {
        std::memcpy(out, parent, 16 * sizeof(double));
        return true;
    }
    double local[16];
    if (!compute_axis2placement(rel, local)) return false;
    matmul4(parent, local, out);
    return true;
}

bool compute_axis2placement(express::Base e, double* out) {
    if (!e) {
        identity4(out);
        return false;
    }
    double z[3] = {0, 0, 1};
    double x[3] = {1, 0, 0};
    double o[3] = {0, 0, 0};

    if (entity_is_a(e, "IfcAxis2Placement3D") || entity_is_a(e, "IfcAxis2PlacementLinear")) {
        if (auto axis = get_entity_ref(e, "Axis")) read_double_vec3(axis, "DirectionRatios", z);
        if (auto refd = get_entity_ref(e, "RefDirection")) read_double_vec3(refd, "DirectionRatios", x);
        auto loc = get_entity_ref(e, "Location");
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
        if (auto refd = get_entity_ref(e, "RefDirection")) {
            read_double_vec3(refd, "DirectionRatios", x);
        }
        if (auto loc = get_entity_ref(e, "Location")) {
            double c[3] = {0, 0, 0};
            read_double_vec3(loc, "Coordinates", c);
            o[0] = c[0];
            o[1] = c[1];
            o[2] = 0.0;
        }
    } else if (entity_is_a(e, "IfcAxis1Placement")) {
        if (auto axis = get_entity_ref(e, "Axis")) read_double_vec3(axis, "DirectionRatios", z);
        if (auto loc = get_entity_ref(e, "Location")) read_double_vec3(loc, "Coordinates", o);
    } else {
        identity4(out);
        return false;
    }

    a2p(o, z, x, out);
    return true;
}

}  // namespace ifcapi

namespace {

std::vector<double> matrix_to_vector(const double* matrix) {
    return std::vector<double>(matrix, matrix + 16);
}

void vector_to_vec3(const std::vector<double>& values, const char* name, double out[3]) {
    if (values.size() != 3) {
        throw std::invalid_argument(std::string(name) + " must contain exactly 3 values");
    }
    out[0] = values[0];
    out[1] = values[1];
    out[2] = values[2];
}

std::vector<double> matrix_result(bool ok, const double* matrix) {
    return ok ? matrix_to_vector(matrix) : std::vector<double>();
}

} // namespace

namespace ifcapi {
namespace bindings {

std::vector<double> placement_a2p(
    const std::vector<double>& origin,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis)
{
    double o[3], z[3], x[3], out[16];
    vector_to_vec3(origin, "origin", o);
    vector_to_vec3(z_axis, "z_axis", z);
    vector_to_vec3(x_axis, "x_axis", x);
    a2p(o, z, x, out);
    return matrix_to_vector(out);
}

std::vector<double> placement_get_axis2placement(express::Base* instance) {
    double out[16];
    identity4(out);
    return matrix_result(compute_axis2placement(ifcapi::detail::deref_or_empty(instance), out), out);
}

std::vector<double> placement_get_local_placement(express::Base* instance) {
    double out[16];
    identity4(out);
    auto instance_value = ifcapi::detail::deref_or_empty(instance);
    if (!instance_value) return matrix_to_vector(out);
    return matrix_result(compute_local_placement(instance_value, out), out);
}

std::vector<double> placement_get_cartesian_xform_3d(express::Base* instance) {
    double out[16];
    identity4(out);
    return matrix_result(compute_cart_xform_3d(ifcapi::detail::deref_or_empty(instance), out), out);
}

std::vector<double> placement_get_mappeditem_xform(express::Base* instance) {
    double out[16];
    identity4(out);
    return matrix_result(compute_mappeditem_xform(ifcapi::detail::deref_or_empty(instance), out), out);
}

double placement_get_storey_elevation(express::Base* instance) {
    double out = 0.0;
    auto e = ifcapi::detail::deref_or_empty(instance);
    if (!e) return out;
    auto placement = read_ref(e, "ObjectPlacement");
    if (placement) {
        double m[16];
        if (compute_local_placement(placement, m)) {
            return m[11];
        }
    }
    double elev;
    if (read_double_attr(e, "Elevation", &elev)) {
        return elev;
    }
    return out;
}

std::vector<double> placement_rotation(double angle_rad, const std::string& axis) {
    double out[16];
    identity4(out);
    const char axis_char = !axis.empty() ? axis[0] : '\0';
    double c = std::cos(angle_rad);
    double s = std::sin(angle_rad);
    if (axis_char == 'X' || axis_char == 'x') {
        out[5] = c;  out[6] = -s;
        out[9] = s;  out[10] = c;
    } else if (axis_char == 'Y' || axis_char == 'y') {
        out[0] = c;   out[2] = s;
        out[8] = -s;  out[10] = c;
    } else if (axis_char == 'Z' || axis_char == 'z') {
        out[0] = c;  out[1] = -s;
        out[4] = s;  out[5] = c;
    }
    return matrix_to_vector(out);
}

} // namespace bindings
} // namespace ifcapi
