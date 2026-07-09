/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ROOT_H
#define IFCAPI_BINDINGS_ROOT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

/**
 * Options for creating an IFC rooted entity.
 */
struct RootCreateEntityOptions {
    /// IFC entity class to instantiate, for example `IfcWall` or `IfcProject`.
    std::string ifc_class;
    /// Optional predefined type. Unsupported enum values are stored as user-defined type labels when possible.
    std::optional<std::string> predefined_type;
    /// Optional entity name.
    std::optional<std::string> name;
    /// Optional owner history to assign when the target schema supports it.
    std::optional<express::Base> owner_history;
};

struct RootRemoveProductOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Create an IFC entity with generated identity, ownership metadata, optional
 * name, and optional predefined type.
 */
IFCAPI_BINDING express::Base root_create_entity(
    ifcopenshell::file* file,
    const RootCreateEntityOptions& options);
IFCAPI_BINDING void root_remove_product(
    ifcopenshell::file* file,
    express::Base* product,
    const RootRemoveProductOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ROOT_H */
