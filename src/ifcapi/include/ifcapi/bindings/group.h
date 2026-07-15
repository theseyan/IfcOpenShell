/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GROUP_H
#define IFCAPI_BINDINGS_GROUP_H

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
 * Options for creating a new IfcGroup.
 */
struct GroupAddGroupOptions {
    /// Name for the group.
    std::string name;
    /// Optional description.
    std::optional<std::string> description;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for replacing the product list of a group.
 */
struct GroupUpdateGroupProductsOptions {
    /// The group whose product list is replaced.
    express::Base group;
    /// New product list. Replaces all existing products. Child IfcGroup entities
    /// from the existing relationship are preserved and merged.
    std::vector<express::Base> products;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for assigning products to a group.
 */
struct GroupAssignGroupOptions {
    /// Products to assign to the group.
    std::vector<express::Base> products;
    /// The group to assign products to.
    express::Base group;
    /// Optional owner history. A new one is created from user/application if not provided.
    std::optional<express::Base> owner_history;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning products from a group.
 */
struct GroupUnassignGroupOptions {
    /// Products to remove from the group.
    std::vector<express::Base> products;
    /// The group to remove products from.
    express::Base group;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Create a new IfcGroup with generated GlobalId and OwnerHistory.
 */
IFCAPI_BINDING express::Base group_add_group(
    ifcopenshell::file* file,
    const GroupAddGroupOptions& options);

/**
 * Replace the product list of an existing IfcRelAssignsToGroup relationship.
 *
 * If no relationship exists, a new one is created. Child IfcGroup entities
 * from the existing relationship are preserved and merged with the new list.
 * Multiple existing relationships are consolidated into one.
 */
IFCAPI_BINDING express::Base group_update_group_products(
    ifcopenshell::file* file,
    const GroupUpdateGroupProductsOptions& options);

/**
 * Assign products to a group via IfcRelAssignsToGroup.
 *
 * If an existing relationship is present, new products are merged into it.
 * Products already in the group are skipped.
 */
IFCAPI_BINDING express::Base group_assign_group(
    ifcopenshell::file* file,
    const GroupAssignGroupOptions& options);

/**
 * Remove products from an IfcRelAssignsToGroup relationship.
 *
 * If no products remain in the relationship after removal, the relationship
 * is deleted.
 */
IFCAPI_BINDING void group_unassign_group(
    ifcopenshell::file* file,
    const GroupUnassignGroupOptions& options);

/**
 * Remove a group and its relationships.
 *
 * Removes associated IfcRelDefinesByProperties (property sets) and
 * IfcRelAssignsToGroup relationships, then deletes the group entity.
 */
IFCAPI_BINDING void group_remove_group(
    ifcopenshell::file* file,
    express::Base* group);

/** Edit an IfcGroup using the shared attribute property writer. */
IFCAPI_BINDING void group_edit_group(
    ifcopenshell::file* file,
    express::Base group,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GROUP_H */
