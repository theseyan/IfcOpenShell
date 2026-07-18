// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_TYPES_H
#define IFCAPI_BINDINGS_TYPES_H

#include <array>
#include <variant>
#include <vector>

namespace ifcapi {
namespace bindings {

using Vec2 = std::array<double, 2>;
using Vec3 = std::array<double, 3>;
using Mat3 = std::array<double, 9>;
using Mat4 = std::array<double, 16>;
using Dimensions7 = std::array<int, 7>;
using Vec2OrVec3 = std::variant<Vec2, Vec3>;
using Vec2OrVec3List = std::variant<std::vector<Vec2>, std::vector<Vec3>>;
using Mat3OrMat4 = std::variant<Mat3, Mat4>;

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_TYPES_H */
