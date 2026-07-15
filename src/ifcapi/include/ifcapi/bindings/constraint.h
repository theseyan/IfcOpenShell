// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_CONSTRAINT_H
#define IFCAPI_BINDINGS_CONSTRAINT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

/**
 * Create a new IfcObjective constraint with default attributes.
 *
 * Sets Name to "Unnamed", ConstraintGrade and ObjectiveQualifier to "NOTDEFINED".
 */
IFCAPI_BINDING express::Base constraint_add_objective(ifcopenshell::file* file);

/**
 * Add a new IfcMetric constraint. If an objective is provided, the metric
 * is appended to the objective's benchmark values.
 */
IFCAPI_BINDING express::Base constraint_add_metric(
    ifcopenshell::file* file,
    std::optional<express::Base> objective);

/**
 * Create a chain of IfcReference entities from a dot-separated path and
 * assign it to a metric's ReferencePath attribute.
 *
 * For example, a path "Pset_WallCommon.FireRating" produces two IfcReference
 * entities linked by InnerReference, with AttributeIdentifier set to
 * "Pset_WallCommon" and "FireRating" respectively.
 *
 * @return The created IfcReference chain in order from outermost to innermost.
 */
IFCAPI_BINDING std::vector<express::Base> constraint_add_metric_reference(
    ifcopenshell::file* file,
    express::Base* metric,
    const std::string& reference_path);

/**
 * Options for assigning a constraint to products.
 */
struct ConstraintAssignConstraintOptions {
    /// Products to associate with the constraint.
    std::vector<express::Base> products;
    /// The constraint to assign.
    express::Base constraint;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning a constraint from products.
 */
struct ConstraintUnassignConstraintOptions {
    /// Products to disassociate from the constraint.
    std::vector<express::Base> products;
    /// The constraint to unassign.
    express::Base constraint;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
 *
 * If the constraint is already associated with some of the products, only
 * new products are added to the existing relationship.
 */
IFCAPI_BINDING express::Base constraint_assign_constraint(
    ifcopenshell::file* file,
    const ConstraintAssignConstraintOptions& options);

/**
 * Remove constraint associations from products.
 *
 * Products are removed from existing IfcRelAssociatesConstraint
 * relationships. If no products remain, the relationship is removed.
 */
IFCAPI_BINDING void constraint_unassign_constraint(
    ifcopenshell::file* file,
    const ConstraintUnassignConstraintOptions& options);

/**
 * Remove a constraint entity and clean up orphaned IfcRelAssociatesConstraint
 * relationships that reference it.
 */
IFCAPI_BINDING void constraint_remove_constraint(
    ifcopenshell::file* file,
    express::Base* constraint);

/**
 * Remove an IfcMetric and its reference path chain.
 *
 * Deletes the metric's IfcReference chain (via InnerReference), then removes
 * the metric entity. Orphaned IfcRelAssociatesConstraint and
 * IfcResourceConstraintRelationship entities are also deleted.
 */
IFCAPI_BINDING void constraint_remove_metric(
    ifcopenshell::file* file,
    express::Base* metric);

/** Edit an IfcMetric using the shared attribute property writer. */
IFCAPI_BINDING void constraint_edit_metric(
    ifcopenshell::file* file,
    express::Base metric,
    ifcopenshell_pset_props_t* attributes);

/** Edit an IfcObjective using the shared attribute property writer. */
IFCAPI_BINDING void constraint_edit_objective(
    ifcopenshell::file* file,
    express::Base objective,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CONSTRAINT_H */
