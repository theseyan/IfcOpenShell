// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_MATRIX_H
#define IFCAPI_DETAIL_MATRIX_H

#include <stdexcept>
#include <vector>

namespace ifcapi {
namespace detail {

inline std::vector<double> matmul4(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> out(16, 0.0);
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            for (size_t k = 0; k < 4; ++k) {
                out[r * 4 + c] += a[r * 4 + k] * b[k * 4 + c];
            }
        }
    }
    return out;
}

inline std::vector<double> invert_rigid4(const std::vector<double>& m) {
    if (m.size() != 16) {
        throw std::runtime_error("Expected 4x4 matrix");
    }
    std::vector<double> out(16, 0.0);
    out[0] = m[0];
    out[1] = m[4];
    out[2] = m[8];
    out[4] = m[1];
    out[5] = m[5];
    out[6] = m[9];
    out[8] = m[2];
    out[9] = m[6];
    out[10] = m[10];
    out[15] = 1.0;
    out[3] = -(out[0] * m[3] + out[1] * m[7] + out[2] * m[11]);
    out[7] = -(out[4] * m[3] + out[5] * m[7] + out[6] * m[11]);
    out[11] = -(out[8] * m[3] + out[9] * m[7] + out[10] * m[11]);
    return out;
}

inline std::vector<double> transform_point_2d(const std::vector<double>& matrix, const std::vector<double>& point) {
    return {
        matrix[0] * point[0] + matrix[1] * point[1] + matrix[3],
        matrix[4] * point[0] + matrix[5] * point[1] + matrix[7],
    };
}

inline std::vector<double> transform_vector_3d(const std::vector<double>& matrix, const std::vector<double>& vector) {
    return {
        matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2],
        matrix[4] * vector[0] + matrix[5] * vector[1] + matrix[6] * vector[2],
        matrix[8] * vector[0] + matrix[9] * vector[1] + matrix[10] * vector[2],
    };
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_MATRIX_H */
