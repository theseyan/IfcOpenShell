// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/shape.h"

#include <cmath>

namespace ifcapi {
namespace bindings {

bool shape_is_almost_equal(double value, double x, double tolerance) {
    if (tolerance == 0.0) tolerance = 1e-6;
    return std::fabs(x - value) < tolerance;
}

} // namespace bindings
} // namespace ifcapi
