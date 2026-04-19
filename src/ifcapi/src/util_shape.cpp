// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include <cmath>

extern "C" {

IFCAPI_EXPORT bool ifcopenshell_util_shape_is_x(double value, double x, double tolerance) {
    if (tolerance == 0.0) tolerance = 1e-6;
    return std::fabs(x - value) < tolerance;
}

}  // extern "C"
