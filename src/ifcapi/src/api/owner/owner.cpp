// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/owner.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_ifc4x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name().find("IFC4X3") != std::string::npos;
}

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

std::string exact_class_name(IfcUtil::IfcBaseClass* entity) {
    return entity ? entity->declaration().name() : std::string();
}

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const std::string& ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || entity->id() <= 0) return result;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* inverse : *inverses) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const char* attribute) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return result;
    try {
        auto inverses = base->get_inverse(attribute);
        if (!inverses) return result;
        for (size_t i = 0; i < inverses->size(); ++i) {
            if ((*inverses)[i]) result.push_back((*inverses)[i]);
        }
    } catch (...) {
    }
    return result;
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || entity->id() <= 0) return 0;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    return inverses ? inverses->size() : 0;
}

bool aggregate_is_singleton(IfcUtil::IfcBaseClass* entity, const char* attr, IfcUtil::IfcBaseClass* value) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    return values.size() == 1 && values.front() == value;
}

void append_ref(IfcUtil::IfcBaseClass* entity, const char* attr, IfcUtil::IfcBaseClass* value) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    values.push_back(value);
    ifcapi::detail::write_ref_aggregate(entity, attr, values);
}

void remove_resource_level_relationship_if_single(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relationship,
    IfcUtil::IfcBaseClass* object)
{
    if (is_a(relationship, "IfcResourceLevelRelationship")
        && aggregate_is_singleton(relationship, "RelatedResourceObjects", object)) {
        file->removeEntity(relationship);
    }
}

IfcUtil::IfcBaseClass* create_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* actor,
    const std::string& ifc_class,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const std::string entity_name = ifc_class.empty() ? "IfcActor" : ifc_class;
    auto* result = create_entity(file, entity_name);
    ifcapi::detail::write_string_attr(result, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        result,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_attr(result, "TheActor", actor);
    return result;
}

IfcUtil::IfcBaseClass* create_default_application_organisation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* organisation = create_entity(file, "IfcOrganization");
    ifcapi::detail::write_string_attr(organisation, is_ifc2x3(file) ? "Id" : "Identification", "IfcOpenShell");
    ifcapi::detail::write_string_attr(organisation, "Name", "IfcOpenShell");
    ifcapi::detail::write_string_attr(
        organisation,
        "Description",
        "IfcOpenShell is an open source software library that helps users and software developers to work with IFC data.");

    auto* role = create_entity(file, "IfcActorRole");
    ifcapi::detail::write_enum_attr(role, "Role", "USERDEFINED");
    ifcapi::detail::write_string_attr(role, "UserDefinedRole", "CONTRIBUTOR");
    ifcapi::detail::write_ref_aggregate(organisation, "Roles", {role});

    if (is_ifc4x3(file)) {
        auto* actor = create_actor(file, organisation, "IfcActor", owner_history, user, application);
        auto* pset = ifcapi::bindings::pset_add_pset(
            file, actor, "PEnum_AddressType", owner_history, user, application, nullptr);
        auto* props = ifcapi::bindings::pset_props_new();
        ifcapi::bindings::pset_props_set_string(props, "Purpose", "OTHER");
        ifcapi::bindings::pset_props_set_string(props, "UserDefinedPurpose", "WEBPAGE");
        ifcapi::bindings::pset_props_set_string(props, "WWWHomePageURL", "https://ifcopenshell.org");
        ifcapi::bindings::pset_edit_pset(file, pset, nullptr, props, nullptr, true);
        ifcapi::bindings::pset_props_free(props);
    } else {
        auto* address = create_entity(file, "IfcTelecomAddress");
        ifcapi::detail::write_enum_attr(address, "Purpose", "USERDEFINED");
        ifcapi::detail::write_string_attr(address, "UserDefinedPurpose", "WEBPAGE");
        ifcapi::detail::write_string_attr(address, "WWWHomePageURL", "https://ifcopenshell.org");
        ifcapi::detail::write_ref_aggregate(organisation, "Addresses", {address});
    }

    return organisation;
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* owner_add_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* actor,
    const std::string& ifc_class,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    return create_actor(file, actor, ifc_class, owner_history, user, application);
}

IfcUtil::IfcBaseClass* owner_add_address(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* assigned_object,
    const std::string& ifc_class)
{
    auto* address = create_entity(file, ifc_class);
    ifcapi::detail::write_enum_attr(address, "Purpose", "OFFICE");
    append_ref(assigned_object, "Addresses", address);
    return address;
}

IfcUtil::IfcBaseClass* owner_add_application(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* application_developer,
    const std::string& version,
    const std::string& application_full_name,
    const std::string& application_identifier,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!application_developer) {
        application_developer = create_default_application_organisation(file, owner_history, user, application);
    }
    auto* result = create_entity(file, "IfcApplication");
    ifcapi::detail::write_ref_attr(result, "ApplicationDeveloper", application_developer);
    ifcapi::detail::write_string_attr(result, "Version", version);
    ifcapi::detail::write_string_attr(result, "ApplicationFullName", application_full_name);
    ifcapi::detail::write_string_attr(result, "ApplicationIdentifier", application_identifier);
    return result;
}

IfcUtil::IfcBaseClass* owner_add_organisation(
    IfcParse::IfcFile* file,
    const std::string& identification,
    const std::string& name)
{
    auto* organisation = create_entity(file, "IfcOrganization");
    ifcapi::detail::write_string_attr(organisation, "Name", name);
    ifcapi::detail::write_string_attr(organisation, is_ifc2x3(file) ? "Id" : "Identification", identification);
    return organisation;
}

IfcUtil::IfcBaseClass* owner_add_person(
    IfcParse::IfcFile* file,
    const std::string& identification,
    const std::string& family_name,
    const std::string& given_name)
{
    auto* person = create_entity(file, "IfcPerson");
    ifcapi::detail::write_string_attr(person, is_ifc2x3(file) ? "Id" : "Identification", identification);
    ifcapi::detail::write_string_attr(person, "FamilyName", family_name);
    ifcapi::detail::write_string_attr(person, "GivenName", given_name);
    return person;
}

IfcUtil::IfcBaseClass* owner_add_person_and_organisation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* person,
    IfcUtil::IfcBaseClass* organisation)
{
    auto* result = create_entity(file, "IfcPersonAndOrganization");
    ifcapi::detail::write_ref_attr(result, "ThePerson", person);
    ifcapi::detail::write_ref_attr(result, "TheOrganization", organisation);
    return result;
}

IfcUtil::IfcBaseClass* owner_add_role(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* assigned_object,
    const std::string& role)
{
    auto* result = create_entity(file, "IfcActorRole");
    ifcapi::detail::write_enum_attr(result, "Role", "ARCHITECT");
    if (!role.empty() && !ifcapi::detail::write_enum_attr(result, "Role", role)) {
        ifcapi::detail::write_enum_attr(result, "Role", "USERDEFINED");
        ifcapi::detail::write_string_attr(result, "UserDefinedRole", role);
    }
    append_ref(assigned_object, "Roles", result);
    return result;
}

IfcUtil::IfcBaseClass* owner_assign_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_actor,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    for (auto* rel : inverse_entities(related_object, "HasAssignments")) {
        if (is_a(rel, "IfcRelAssignsToActor")
            && ifcapi::detail::read_ref_attr(rel, "RelatingActor") == relating_actor) {
            return rel;
        }
    }

    IfcUtil::IfcBaseClass* rel = nullptr;
    auto actor_rels = inverse_entities(relating_actor, "IsActingUpon");
    if (!actor_rels.empty()) {
        rel = actor_rels.front();
    }

    if (rel) {
        append_ref(rel, "RelatedObjects", related_object);
        ifcapi::detail::update_owner_history(file, rel, user, application);
        return rel;
    }

    rel = create_entity(file, "IfcRelAssignsToActor");
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object});
    ifcapi::detail::write_ref_attr(rel, "RelatingActor", relating_actor);
    return rel;
}

void owner_remove_actor(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* actor) {
    ifcapi::detail::remove_with_history(file, actor);
}

void owner_remove_address(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* address) {
    for (auto* inverse : inverse_entities(file, address)) {
        const auto type = exact_class_name(inverse);
        if ((type == "IfcOrganization" || type == "IfcPerson")
            && aggregate_is_singleton(inverse, "Addresses", address)) {
            ifcapi::detail::write_blank_attr(inverse, "Addresses");
        }
    }
    file->removeEntity(address);
}

void owner_remove_application(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* application) {
    file->removeEntity(application);
}

void owner_remove_organisation(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* organisation) {
    for (auto* role : ifcapi::detail::read_ref_aggregate(organisation, "Roles")) {
        if (total_inverses(file, role) == 1) owner_remove_role(file, role);
    }
    for (auto* address : ifcapi::detail::read_ref_aggregate(organisation, "Addresses")) {
        if (total_inverses(file, address) == 1) owner_remove_address(file, address);
    }
    for (auto* inverse : inverse_entities(file, organisation)) {
        if (is_a(inverse, "IfcOrganizationRelationship")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingOrganization") == organisation
                || aggregate_is_singleton(inverse, "RelatedOrganizations", organisation)) {
                file->removeEntity(inverse);
            }
        } else if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", organisation)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcPersonAndOrganization")) {
            owner_remove_person_and_organisation(file, inverse);
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")
            && !is_a(inverse, "IfcOrganizationRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, organisation);
        } else if (is_a(inverse, "IfcApplication")) {
            owner_remove_application(file, inverse);
        }
    }
    file->removeEntity(organisation);
}

void owner_remove_person(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* person) {
    for (auto* role : ifcapi::detail::read_ref_aggregate(person, "Roles")) {
        if (total_inverses(file, role) == 1) owner_remove_role(file, role);
    }
    for (auto* address : ifcapi::detail::read_ref_aggregate(person, "Addresses")) {
        if (total_inverses(file, address) == 1) owner_remove_address(file, address);
    }
    for (auto* inverse : inverse_entities(file, person)) {
        if (is_a(inverse, "IfcWorkControl")) {
            if (aggregate_is_singleton(inverse, "Creators", person)) {
                ifcapi::detail::write_blank_attr(inverse, "Creators");
            }
        } else if (is_a(inverse, "IfcInventory")) {
            if (aggregate_is_singleton(inverse, "ResponsiblePersons", person) && is_ifc2x3(file)) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", person)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcPersonAndOrganization")) {
            owner_remove_person_and_organisation(file, inverse);
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, person);
        }
    }
    file->removeEntity(person);
}

void owner_remove_person_and_organisation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* person_and_organisation)
{
    for (auto* inverse : inverse_entities(file, person_and_organisation)) {
        if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", person_and_organisation)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, person_and_organisation);
        } else if (is_a(inverse, "IfcOwnerHistory")) {
            file->removeEntity(inverse);
        }
    }
    file->removeEntity(person_and_organisation);
}

void owner_remove_role(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* role) {
    for (auto* inverse : inverse_entities(file, role)) {
        const auto type = exact_class_name(inverse);
        if ((type == "IfcOrganization" || type == "IfcPerson" || type == "IfcPersonAndOrganization")
            && aggregate_is_singleton(inverse, "Roles", role)) {
            ifcapi::detail::write_blank_attr(inverse, "Roles");
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, role);
        }
    }
    file->removeEntity(role);
}

void owner_unassign_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_actor,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    for (auto* rel : inverse_entities(related_object, "HasAssignments")) {
        if (!is_a(rel, "IfcRelAssignsToActor")
            || ifcapi::detail::read_ref_attr(rel, "RelatingActor") != relating_actor) {
            continue;
        }
        auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        if (related_objects.size() == 1) {
            ifcapi::detail::remove_with_history(file, rel);
            return;
        }
        related_objects.erase(std::remove(related_objects.begin(), related_objects.end(), related_object), related_objects.end());
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    }
}

} // namespace bindings
} // namespace ifcapi
