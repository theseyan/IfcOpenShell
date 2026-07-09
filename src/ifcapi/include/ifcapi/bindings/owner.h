/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_OWNER_H
#define IFCAPI_BINDINGS_OWNER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a new actor (IfcActor or IfcOccupant).
 */
struct OwnerAddActorOptions {
    express::Base actor;
    std::string ifc_class;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for adding a new IfcApplication.
 */
struct OwnerAddApplicationOptions {
    std::optional<express::Base> application_developer;
    std::string version;
    std::string application_full_name;
    std::string application_identifier;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for assigning an actor to an object.
 */
struct OwnerAssignActorOptions {
    express::Base relating_actor;
    express::Base related_object;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for creating an IfcOwnerHistory.
 */
struct OwnerCreateOwnerHistoryOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for unassigning an actor from an object.
 */
struct OwnerUnassignActorOptions {
    express::Base relating_actor;
    express::Base related_object;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for updating an IfcOwnerHistory on an element.
 */
struct OwnerUpdateOwnerHistoryOptions {
    std::optional<express::Base> element;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING express::Base owner_add_actor(
    ifcopenshell::file* file,
    const OwnerAddActorOptions& options);
IFCAPI_BINDING express::Base owner_add_address(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& ifc_class);
IFCAPI_BINDING express::Base owner_add_application(
    ifcopenshell::file* file,
    const OwnerAddApplicationOptions& options);
IFCAPI_BINDING express::Base owner_add_organisation(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& name);
IFCAPI_BINDING express::Base owner_add_person(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& family_name,
    const std::string& given_name);
IFCAPI_BINDING express::Base owner_add_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person,
    express::Base* organisation);
IFCAPI_BINDING express::Base owner_add_role(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& role);
IFCAPI_BINDING express::Base owner_assign_actor(
    ifcopenshell::file* file,
    const OwnerAssignActorOptions& options);
IFCAPI_BINDING express::Base owner_create_owner_history(
    ifcopenshell::file* file,
    const OwnerCreateOwnerHistoryOptions& options);
IFCAPI_BINDING void owner_remove_actor(ifcopenshell::file* file, express::Base* actor);
IFCAPI_BINDING void owner_remove_address(ifcopenshell::file* file, express::Base* address);
IFCAPI_BINDING void owner_remove_application(ifcopenshell::file* file, express::Base* application);
IFCAPI_BINDING void owner_remove_organisation(ifcopenshell::file* file, express::Base* organisation);
IFCAPI_BINDING void owner_remove_person(ifcopenshell::file* file, express::Base* person);
IFCAPI_BINDING void owner_remove_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person_and_organisation);
IFCAPI_BINDING void owner_remove_role(ifcopenshell::file* file, express::Base* role);
IFCAPI_BINDING void owner_unassign_actor(
    ifcopenshell::file* file,
    const OwnerUnassignActorOptions& options);
IFCAPI_BINDING express::Base owner_update_owner_history(
    ifcopenshell::file* file,
    const OwnerUpdateOwnerHistoryOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_OWNER_H */
