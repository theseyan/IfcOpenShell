// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_VECTOR_H
#define IFCAPI_DETAIL_VECTOR_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace ifcapi {
namespace detail {

inline std::vector<double> vec_pad3(const std::vector<double>& value) {
    std::vector<double> result = value;
    result.resize(3, 0.0);
    return result;
}

inline std::vector<double> vec_add(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> result = a;
    const size_t n = std::min(result.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        result[i] += b[i];
    }
    return result;
}

inline std::vector<double> vec_sub(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> result = a;
    const size_t n = std::min(result.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        result[i] -= b[i];
    }
    return result;
}

inline std::vector<double> vec_mul(const std::vector<double>& value, double factor) {
    std::vector<double> result = value;
    for (double& component : result) {
        component *= factor;
    }
    return result;
}

inline double vec_dot(const std::vector<double>& a, const std::vector<double>& b) {
    double result = 0.0;
    const size_t n = std::min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

inline double vec_norm(const std::vector<double>& value) {
    return std::sqrt(vec_dot(value, value));
}

inline std::vector<double> np_normalized(const std::vector<double>& value) {
    const double length = vec_norm(value);
    if (length == 0.0) {
        return value;
    }
    return vec_mul(value, 1.0 / length);
}

inline std::vector<double> np_to_3d(const std::vector<double>& value) {
    return vec_pad3(value);
}

inline std::vector<double> vec_cross3(const std::vector<double>& a, const std::vector<double>& b) {
    const auto aa = vec_pad3(a);
    const auto bb = vec_pad3(b);
    return {
        aa[1] * bb[2] - aa[2] * bb[1],
        aa[2] * bb[0] - aa[0] * bb[2],
        aa[0] * bb[1] - aa[1] * bb[0],
    };
}

inline std::vector<double> np_normal(const std::vector<std::vector<double>>& points) {
    if (points.size() < 3) {
        return {0.0, 0.0, 0.0};
    }
    return np_normalized(vec_cross3(vec_sub(points[1], points[0]), vec_sub(points[2], points[0])));
}

inline std::vector<double> np_lerp(const std::vector<double>& a, const std::vector<double>& b, double factor) {
    return vec_add(a, vec_mul(vec_sub(b, a), factor));
}

inline double np_angle(const std::vector<double>& a, const std::vector<double>& b) {
    const double denom = vec_norm(a) * vec_norm(b);
    if (denom == 0.0) {
        return 0.0;
    }
    const double c = std::clamp(vec_dot(a, b) / denom, -1.0, 1.0);
    return std::acos(c);
}

inline double np_angle_signed(const std::vector<double>& a, const std::vector<double>& b) {
    const double angle = np_angle(a, b);
    const double cross_z = a.at(0) * b.at(1) - a.at(1) * b.at(0);
    return cross_z < 0.0 ? -angle : angle;
}

inline bool is_x(double value, double target, double tolerance = 1e-6) {
    return std::abs(value - target) <= tolerance;
}

inline std::vector<std::vector<double>> np_intersect_line_line(
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    const std::vector<double>& p3,
    const std::vector<double>& p4)
{
    const auto a = vec_pad3(p1);
    const auto b = vec_pad3(p2);
    const auto c = vec_pad3(p3);
    const auto d = vec_pad3(p4);
    const auto u = vec_sub(b, a);
    const auto v = vec_sub(d, c);
    const auto w = vec_sub(a, c);
    const double uu = vec_dot(u, u);
    const double uv = vec_dot(u, v);
    const double vv = vec_dot(v, v);
    const double uw = vec_dot(u, w);
    const double vw = vec_dot(v, w);
    const double denom = uu * vv - uv * uv;
    if (std::abs(denom) < 1e-12) {
        return {a, c};
    }
    const double s = (uv * vw - vv * uw) / denom;
    const double t = (uu * vw - uv * uw) / denom;
    return {vec_add(a, vec_mul(u, s)), vec_add(c, vec_mul(v, t))};
}

inline bool vec_allclose(const std::vector<double>& a, const std::vector<double>& b, double tolerance = 1e-8) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

inline std::vector<double> rotate_xy(const std::vector<double>& value, double angle) {
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    auto result = vec_pad3(value);
    result[0] = value[0] * c - value[1] * s;
    result[1] = value[0] * s + value[1] * c;
    return result;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_VECTOR_H */
