/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GROUP_H
#define IFCAPI_BINDINGS_GROUP_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Options for creating a new IfcGroup.
 */
struct GroupAddGroupOptions {
    std::string name;
    std::optional<std::string> description;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for replacing the product list of a group.
 */
struct GroupUpdateGroupProductsOptions {
    express::Base group;
    std::vector<express::Base> products;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for assigning products to a group.
 */
struct GroupAssignGroupOptions {
    std::vector<express::Base> products;
    express::Base group;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning products from a group.
 */
struct GroupUnassignGroupOptions {
    std::vector<express::Base> products;
    express::Base group;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Create a new IfcGroup with generated identity and ownership metadata.
 */
IFCAPI_BINDING express::Base group_add_group(
    ifcopenshell::file* file,
    const GroupAddGroupOptions& options);

/**
 * Replace the product list of an existing group relationship.
 */
IFCAPI_BINDING express::Base group_update_group_products(
    ifcopenshell::file* file,
    const GroupUpdateGroupProductsOptions& options);

/**
 * Assign products to a group, merging into an existing relationship when present.
 */
IFCAPI_BINDING express::Base group_assign_group(
    ifcopenshell::file* file,
    const GroupAssignGroupOptions& options);

/**
 * Remove products from a group relationship.
 */
IFCAPI_BINDING void group_unassign_group(
    ifcopenshell::file* file,
    const GroupUnassignGroupOptions& options);

/**
 * Remove a group and its relationships.
 */
IFCAPI_BINDING void group_remove_group(
    ifcopenshell::file* file,
    express::Base* group);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GROUP_H */
