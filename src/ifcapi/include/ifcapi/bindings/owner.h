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
    /// IfcPerson, IfcOrganization, or IfcPersonAndOrganization to act as the actor.
    express::Base actor;
    /// IFC class name for the actor entity, e.g. "IfcActor" or "IfcOccupant". Defaults to "IfcActor" when empty.
    std::string ifc_class;
    /// Owner history applied to the created entity. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/**
 * Options for adding a new IfcApplication.
 */
struct OwnerAddApplicationOptions {
    /// IfcOrganization for the ApplicationDeveloper attribute. When omitted, a default IfcOpenShell organisation is created.
    std::optional<express::Base> application_developer;
    /// Version string for the application.
    std::string version;
    /// Full human-readable name of the application.
    std::string application_full_name;
    /// Short identifier for the application.
    std::string application_identifier;
    /// Owner history used when creating the default application_developer organisation.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used when creating the default application_developer organisation.
    std::optional<express::Base> user;
    /// IfcApplication used when creating the default application_developer organisation.
    std::optional<express::Base> application;
};

/**
 * Options for assigning an actor to an object.
 */
struct OwnerAssignActorOptions {
    /// IfcActor (or IfcOccupant) that performs the role.
    express::Base relating_actor;
    /// Object to which the actor is assigned.
    express::Base related_object;
    /// Owner history applied to the IfcRelAssignsToActor relationship. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/**
 * Options for creating an IfcOwnerHistory.
 */
struct OwnerCreateOwnerHistoryOptions {
    /// IfcPersonAndOrganization for the OwningUser attribute. Required.
    std::optional<express::Base> user;
    /// IfcApplication for the OwningApplication attribute. Required.
    std::optional<express::Base> application;
};

/**
 * Options for unassigning an actor from an object.
 */
struct OwnerUnassignActorOptions {
    /// IfcActor to unassign from the object.
    express::Base relating_actor;
    /// Object from which to unassign the actor.
    express::Base related_object;
    /// IfcPersonAndOrganization used to update the relationship OwnerHistory.
    std::optional<express::Base> user;
    /// IfcApplication used to update the relationship OwnerHistory.
    std::optional<express::Base> application;
};

/**
 * Options for updating an IfcOwnerHistory on an element.
 */
struct OwnerUpdateOwnerHistoryOptions {
    /// IfcRoot element whose OwnerHistory to update. When omitted, no action is taken.
    std::optional<express::Base> element;
    /// IfcPersonAndOrganization used to set LastModifyingUser and for creating a new OwnerHistory if absent.
    std::optional<express::Base> user;
    /// IfcApplication used to set LastModifyingApplication and for creating a new OwnerHistory if absent.
    std::optional<express::Base> application;
};

/**
 * Create an IfcActor (or subclass) entity.
 *
 * @param file File that receives the new entity.
 * @param options Actor identity and class options.
 * @return Newly created IfcActor or subclass.
 */
IFCAPI_BINDING express::Base owner_add_actor(
    ifcopenshell::file* file,
    const OwnerAddActorOptions& options);
/**
 * Add an IfcAddress subclass to an IfcOrganization or IfcPerson.
 *
 * Creates an entity of the given IFC class (e.g. "IfcTelecomAddress",
 * "IfcPostalAddress") with Purpose set to OFFICE and appends it to the
 * assigned object's Addresses aggregate.
 *
 * @param file File that receives the new address.
 * @param assigned_object IfcOrganization or IfcPerson to receive the address.
 * @param ifc_class IFC class name of the address to create.
 * @return Newly created IfcAddress subclass.
 */
IFCAPI_BINDING express::Base owner_add_address(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& ifc_class);
/**
 * Create an IfcApplication entity.
 *
 * If application_developer is omitted, a default IfcOpenShell organisation
 * with a web-page address is created automatically.
 *
 * @param file File that receives the new entity.
 * @param options Application identity options.
 * @return Newly created IfcApplication.
 */
IFCAPI_BINDING express::Base owner_add_application(
    ifcopenshell::file* file,
    const OwnerAddApplicationOptions& options);
/**
 * Create an IfcOrganization entity.
 *
 * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
 *
 * @param file File that receives the new entity.
 * @param identification Organisation identifier.
 * @param name Organisation name.
 * @return Newly created IfcOrganization.
 */
IFCAPI_BINDING express::Base owner_add_organisation(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& name);
/**
 * Create an IfcPerson entity.
 *
 * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
 *
 * @param file File that receives the new entity.
 * @param identification Person identifier.
 * @param family_name Family (sur)name.
 * @param given_name Given (first) name.
 * @return Newly created IfcPerson.
 */
IFCAPI_BINDING express::Base owner_add_person(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& family_name,
    const std::string& given_name);
/**
 * Create an IfcPersonAndOrganization entity linking a person and an organisation.
 *
 * @param file File that receives the new entity.
 * @param person IfcPerson to link.
 * @param organisation IfcOrganization to link.
 * @return Newly created IfcPersonAndOrganization.
 */
IFCAPI_BINDING express::Base owner_add_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person,
    express::Base* organisation);
/**
 * Add an IfcActorRole to an IfcOrganization, IfcPerson, or IfcPersonAndOrganization.
 *
 * If the role string matches a valid IFC enum value (e.g. "ARCHITECT"), it is
 * set directly. Otherwise, Role is set to USERDEFINED and UserDefinedRole
 * receives the string. The role is appended to the assigned object's Roles aggregate.
 *
 * @param file File that receives the new role.
 * @param assigned_object Entity to receive the role.
 * @param role Role name or user-defined string.
 * @return Newly created IfcActorRole.
 */
IFCAPI_BINDING express::Base owner_add_role(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& role);
/**
 * Assign an actor to an object via IfcRelAssignsToActor.
 *
 * If an IfcRelAssignsToActor already exists for this actor and object, the
 * existing relationship is returned. If the actor already has a relationship,
 * the object is appended to its RelatedObjects. Otherwise a new relationship
 * is created.
 *
 * @param file File that receives the relationship.
 * @param options Actor, object, and ownership options.
 * @return The IfcRelAssignsToActor relationship.
 */
IFCAPI_BINDING express::Base owner_assign_actor(
    ifcopenshell::file* file,
    const OwnerAssignActorOptions& options);
/**
 * Create a new IfcOwnerHistory.
 *
 * Sets CreationDate and LastModifiedDate to the current time, State to
 * READWRITE, and ChangeAction to ADDED. Both user and application are
 * required; if either is omitted, no owner history is created.
 *
 * @param file File that receives the new entity.
 * @param options User and application for the owner history.
 * @return Newly created IfcOwnerHistory, or no result if creation fails.
 */
IFCAPI_BINDING express::Base owner_create_owner_history(
    ifcopenshell::file* file,
    const OwnerCreateOwnerHistoryOptions& options);
/**
 * Remove an IfcActor (or subclass) from the file.
 *
 * Removes the entity and its owner history.
 *
 * @param file File containing the actor.
 * @param actor IfcActor entity to remove.
 */
IFCAPI_BINDING void owner_remove_actor(ifcopenshell::file* file, express::Base* actor);
/**
 * Remove an IfcAddress subclass from the file.
 *
 * If the address is the only entry in its parent's Addresses aggregate,
 * that aggregate is cleared before the entity is removed.
 *
 * @param file File containing the address.
 * @param address IfcAddress entity to remove.
 */
IFCAPI_BINDING void owner_remove_address(ifcopenshell::file* file, express::Base* address);
/**
 * Remove an IfcApplication from the file.
 *
 * @param file File containing the application.
 * @param application IfcApplication entity to remove.
 */
IFCAPI_BINDING void owner_remove_application(ifcopenshell::file* file, express::Base* application);
/**
 * Remove an IfcOrganization from the file.
 *
 * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
 * IfcActor entities, IfcOrganizationRelationship entities, and IfcApplication
 * entities that reference this organisation as their sole member.
 *
 * @param file File containing the organisation.
 * @param organisation IfcOrganization entity to remove.
 */
IFCAPI_BINDING void owner_remove_organisation(ifcopenshell::file* file, express::Base* organisation);
/**
 * Remove an IfcPerson from the file.
 *
 * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
 * IfcActor entities, and IfcResourceLevelRelationship entities that reference
 * this person as their sole member.
 *
 * @param file File containing the person.
 * @param person IfcPerson entity to remove.
 */
IFCAPI_BINDING void owner_remove_person(ifcopenshell::file* file, express::Base* person);
/**
 * Remove an IfcPersonAndOrganization from the file.
 *
 * Cascades removal to IfcOwnerHistory, IfcActor, and
 * IfcResourceLevelRelationship entities that reference this entity as their
 * sole member.
 *
 * @param file File containing the person-and-organisation entity.
 * @param person_and_organisation IfcPersonAndOrganization entity to remove.
 */
IFCAPI_BINDING void owner_remove_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person_and_organisation);
/**
 * Remove an IfcActorRole from the file.
 *
 * If the role is the sole entry in its parent's Roles aggregate, that
 * aggregate is cleared before the entity is removed.
 *
 * @param file File containing the role.
 * @param role IfcActorRole entity to remove.
 */
IFCAPI_BINDING void owner_remove_role(ifcopenshell::file* file, express::Base* role);
/**
 * Remove an object assignment from an actor via IfcRelAssignsToActor.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * RelatedObjects aggregate and the OwnerHistory is updated.
 *
 * @param file File containing the relationship.
 * @param options Actor and object to unassign.
 */
IFCAPI_BINDING void owner_unassign_actor(
    ifcopenshell::file* file,
    const OwnerUnassignActorOptions& options);
/**
 * Update or create the OwnerHistory on an IfcRoot element.
 *
 * If the element already has an IfcOwnerHistory, its LastModifiedDate,
 * LastModifyingUser, and LastModifyingApplication are updated. If not,
 * a new IfcOwnerHistory is created and assigned. Requires both user and
 * application to be provided.
 *
 * @param file File containing the element.
 * @param options Element, user, and application.
 * @return The updated or newly created IfcOwnerHistory, or no result if the element is not an IfcRoot.
 */
IFCAPI_BINDING express::Base owner_update_owner_history(
    ifcopenshell::file* file,
    const OwnerUpdateOwnerHistoryOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_OWNER_H */
