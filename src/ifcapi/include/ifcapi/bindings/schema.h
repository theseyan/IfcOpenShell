// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.

#pragma once

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

/**
 * Change the IFC class of an entity.
 *
 * Removes the element and recreates it as the new class with the same
 * STEP id. Compatible attributes are preserved; incompatible attributes
 * are dropped. Inverse references to the old element are updated to
 * point to the new element.
 *
 * If the element is already of the requested class, returns it unchanged.
 *
 * @param file The IFC file. If empty, uses the element's file.
 * @param element The entity to reassign.
 * @param new_class The target IFC class name (e.g. "IfcWall").
 * @return The new entity of the requested class, or empty on failure.
 */
IFCAPI_BINDING express::Base schema_reassign_class(
    std::optional<ifcopenshell::file*> file,
    express::Base* element,
    const std::string& new_class);

} // namespace bindings
} // namespace ifcapi
