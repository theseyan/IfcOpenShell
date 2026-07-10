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
    /// Product to assign (e.g. IfcGridAxis, IfcProduct).
    express::Base relating_product;
    /// Annotation or object receiving the assignment.
    express::Base related_object;
    /// Optional existing IfcOwnerHistory; created from user/application if omitted.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a product from an annotation object.
 */
struct DrawingUnassignProductOptions {
    /// Product to unassign (e.g. IfcGridAxis, IfcProduct).
    express::Base relating_product;
    /// Annotation or object losing the assignment.
    express::Base related_object;
    /// Optional IfcPersonAndOrganization for OwnerHistory update.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory update.
    std::optional<express::Base> application;
};

/**
 * Assign a product to an annotation object via IfcRelAssignsToProduct.
 *
 * When the relating product is an IfcGridAxis, the assignment is created
 * against the owning IfcGrid with the axis tag as the relationship name;
 * a new IfcRelAssignsToProduct is always created for grid axes even if one
 * already exists for the same axis tag. For non-grid products, if an
 * existing IfcRelAssignsToProduct already references the relating product,
 * the related object is appended to its RelatedObjects aggregate instead
 * of creating a new relationship. Returns a null handle on exact duplicate.
 *
 * @param file IFC file to modify.
 * @param options Assignment parameters.
 * @return IfcRelAssignsToProduct relationship, or null handle on duplicate or failure.
 */
IFCAPI_BINDING express::Base drawing_assign_product(
    ifcopenshell::file* file,
    const DrawingAssignProductOptions& options);

/**
 * Remove a product assignment from an annotation object.
 *
 * Finds and removes the IfcRelAssignsToProduct relating the product to the
 * object. If the relationship contains only this one related object, the
 * entire relationship is removed; otherwise only the object is removed from
 * the RelatedObjects aggregate.
 *
 * @param file IFC file to modify.
 * @param options Unassignment parameters.
 */
IFCAPI_BINDING void drawing_unassign_product(
    ifcopenshell::file* file,
    const DrawingUnassignProductOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_DRAWING_H */
