// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Shared 4x4 matrix math and IFC placement-tree evaluation helpers.

#ifndef IFCAPI_PLACEMENT_HELPERS_HPP
#define IFCAPI_PLACEMENT_HELPERS_HPP

#include "ifcparse/IfcBaseClass.h"

#include <cmath>
#include <cstring>

namespace ifcapi {

inline void identity4(double* m) {
    std::memset(m, 0, 16 * sizeof(double));
    m[0] = m[5] = m[10] = m[15] = 1.0;
}

inline void normalize3(double v[3]) {
    double n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (n > 0.0) { v[0] /= n; v[1] /= n; v[2] /= n; }
}

// Build a row-major 4x4 placement matrix from origin O, Z axis, X axis.
inline void a2p(const double o[3], const double z[3], const double x[3], double* m) {
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
    m[0] = xn[0]; m[1] = y[0]; m[2] = zn[0]; m[3] = o[0];
    m[4] = xn[1]; m[5] = y[1]; m[6] = zn[1]; m[7] = o[1];
    m[8] = xn[2]; m[9] = y[2]; m[10] = zn[2]; m[11] = o[2];
    m[12] = 0;    m[13] = 0;   m[14] = 0;    m[15] = 1;
}

inline void matmul4(const double* a, const double* b, double* out) {
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

// In-place 4x4 inverse via Gauss-Jordan with partial pivoting.
inline bool invert4(const double* m, double* out) {
    double a[4][8];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) a[i][j] = m[i * 4 + j];
        for (int j = 0; j < 4; ++j) a[i][4 + j] = (i == j) ? 1.0 : 0.0;
    }
    for (int col = 0; col < 4; ++col) {
        int piv = col;
        double best = std::fabs(a[col][col]);
        for (int r = col + 1; r < 4; ++r) {
            double v = std::fabs(a[r][col]);
            if (v > best) { best = v; piv = r; }
        }
        if (best < 1e-15) return false;
        if (piv != col) {
            for (int j = 0; j < 8; ++j) std::swap(a[col][j], a[piv][j]);
        }
        double d = a[col][col];
        for (int j = 0; j < 8; ++j) a[col][j] /= d;
        for (int r = 0; r < 4; ++r) {
            if (r == col) continue;
            double f = a[r][col];
            if (f == 0.0) continue;
            for (int j = 0; j < 8; ++j) a[r][j] -= f * a[col][j];
        }
    }
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) out[i * 4 + j] = a[i][4 + j];
    return true;
}

// Recursive evaluation of an IfcLocalPlacement chain. Returns identity for null /
// non-LocalPlacement input.
bool compute_local_placement(IfcUtil::IfcBaseClass* e, double* out);

// Evaluation of IfcAxis2Placement2D / 3D / Linear / IfcAxis1Placement.
// Returns false (identity output) for unsupported subtypes (e.g. LinearPlacement
// using IfcPointByDistanceExpression locations).
bool compute_axis2placement(IfcUtil::IfcBaseClass* e, double* out);

}  // namespace ifcapi

#endif
