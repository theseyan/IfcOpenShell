// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_DRAWING_H
#define IFCAPI_BINDINGS_DRAWING_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>

namespace ifcapi {
namespace bindings {

/**
 * Options for assigning a product to an annotation object.
 */
struct DrawingAssignProductOptions {
    express::Base relating_product;
    express::Base related_object;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a product from an annotation object.
 */
struct DrawingUnassignProductOptions {
    express::Base relating_product;
    express::Base related_object;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING express::Base drawing_assign_product(
    ifcopenshell::file* file,
    const DrawingAssignProductOptions& options);

IFCAPI_BINDING void drawing_unassign_product(
    ifcopenshell::file* file,
    const DrawingUnassignProductOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DRAWING_H */
