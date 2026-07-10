/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SHAPE_H
#define IFCAPI_BINDINGS_SHAPE_H

#include "ifcapi/bindings/contract.h"

namespace ifcapi {
namespace bindings {

/**
 * Check whether two floating-point values are approximately equal.
 *
 * Uses strict less-than comparison. When tolerance is 0.0, it defaults
 * to 1e-6.
 *
 * @param value First value.
 * @param x Second value.
 * @param tolerance Maximum absolute difference (exclusive). Defaults to 1e-6 when 0.
 * @return True if |value - x| < tolerance.
 */
IFCAPI_BINDING bool shape_is_almost_equal(double value, double x, double tolerance);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SHAPE_H */
