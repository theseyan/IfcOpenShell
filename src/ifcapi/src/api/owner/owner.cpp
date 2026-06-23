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

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_ifc4x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name().find("IFC4X3") != std::string::npos;
}

bool is_a(express::Base entity, const char* ifc_class) {
    auto d = ifcopenshell::lock_data(entity.data_weak());
    return d && d->declaration()->is(ifc_class);
}

std::string exact_class_name(express::Base entity) {
    auto d = ifcopenshell::lock_data(entity.data_weak());
    return d ? d->declaration()->name() : std::string();
}

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

express::Base create_entity(ifcopenshell::file* file, const std::string& ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() <= 0) return result;
    for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

size_t total_inverses(ifcopenshell::file* file, express::Base entity) {
    return inverse_entities(file, entity).size();
}

bool aggregate_is_singleton(express::Base entity, const char* attr, express::Base value) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    return values.size() == 1 && ifcapi::detail::same_instance(values.front(), value);
}

void append_ref(express::Base entity, const char* attr, express::Base value) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    values.push_back(value);
    ifcapi::detail::write_ref_aggregate(entity, attr, values);
}

void remove_resource_level_relationship_if_single(
    ifcopenshell::file* file,
    express::Base relationship,
    express::Base object)
{
    if (is_a(relationship, "IfcResourceLevelRelationship")
        && aggregate_is_singleton(relationship, "RelatedResourceObjects", object)) {
        file->remove_entity(relationship);
    }
}

express::Base create_actor(
    ifcopenshell::file* file,
    express::Base actor,
    const std::string& ifc_class,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const std::string entity_name = ifc_class.empty() ? "IfcActor" : ifc_class;
    auto result = create_entity(file, entity_name);
    ifcapi::detail::write_string_attr(result, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        result,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_attr(result, "TheActor", actor);
    return result;
}

express::Base create_default_application_organisation(
    ifcopenshell::file* file,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto organisation = create_entity(file, "IfcOrganization");
    ifcapi::detail::write_string_attr(organisation, is_ifc2x3(file) ? "Id" : "Identification", "IfcOpenShell");
    ifcapi::detail::write_string_attr(organisation, "Name", "IfcOpenShell");
    ifcapi::detail::write_string_attr(
        organisation,
        "Description",
        "IfcOpenShell is an open source software library that helps users and software developers to work with IFC data.");

    auto role = create_entity(file, "IfcActorRole");
    ifcapi::detail::write_enum_attr(role, "Role", "USERDEFINED");
    ifcapi::detail::write_string_attr(role, "UserDefinedRole", "CONTRIBUTOR");
    ifcapi::detail::write_ref_aggregate(organisation, "Roles", {role});

    if (is_ifc4x3(file)) {
        auto actor = create_actor(file, organisation, "IfcActor", owner_history, user, application);
        auto pset = ifcapi::bindings::pset_add_pset(
            file, &actor, "PEnum_AddressType", &owner_history, &user, &application, nullptr);
        auto* props = ifcapi::bindings::pset_props_new();
        ifcapi::bindings::pset_props_set_string(props, "Purpose", "OTHER");
        ifcapi::bindings::pset_props_set_string(props, "UserDefinedPurpose", "WEBPAGE");
        ifcapi::bindings::pset_props_set_string(props, "WWWHomePageURL", "https://ifcopenshell.org");
        ifcapi::bindings::pset_edit_pset(file, &pset, nullptr, props, nullptr, true);
        ifcapi::bindings::pset_props_free(props);
    } else {
        auto address = create_entity(file, "IfcTelecomAddress");
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

express::Base owner_add_actor(
    ifcopenshell::file* file,
    express::Base* actor,
    const std::string& ifc_class,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    return create_actor(
        file, detail::deref_or_empty(actor), ifc_class, detail::deref_or_empty(owner_history),
        detail::deref_or_empty(user), detail::deref_or_empty(application));
}

express::Base owner_add_address(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& ifc_class)
{
    auto assigned_object_value = detail::deref_or_empty(assigned_object);
    auto address = create_entity(file, ifc_class);
    ifcapi::detail::write_enum_attr(address, "Purpose", "OFFICE");
    append_ref(assigned_object_value, "Addresses", address);
    return address;
}

express::Base owner_add_application(
    ifcopenshell::file* file,
    express::Base* application_developer,
    const std::string& version,
    const std::string& application_full_name,
    const std::string& application_identifier,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    auto application_developer_value = detail::deref_or_empty(application_developer);
    auto owner_history_value = detail::deref_or_empty(owner_history);
    auto user_value = detail::deref_or_empty(user);
    auto application_value = detail::deref_or_empty(application);
    if (!application_developer_value) {
        application_developer_value = create_default_application_organisation(file, owner_history_value, user_value, application_value);
    }
    auto result = create_entity(file, "IfcApplication");
    ifcapi::detail::write_ref_attr(result, "ApplicationDeveloper", application_developer_value);
    ifcapi::detail::write_string_attr(result, "Version", version);
    ifcapi::detail::write_string_attr(result, "ApplicationFullName", application_full_name);
    ifcapi::detail::write_string_attr(result, "ApplicationIdentifier", application_identifier);
    return result;
}

express::Base owner_add_organisation(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& name)
{
    auto organisation = create_entity(file, "IfcOrganization");
    ifcapi::detail::write_string_attr(organisation, "Name", name);
    ifcapi::detail::write_string_attr(organisation, is_ifc2x3(file) ? "Id" : "Identification", identification);
    return organisation;
}

express::Base owner_add_person(
    ifcopenshell::file* file,
    const std::string& identification,
    const std::string& family_name,
    const std::string& given_name)
{
    auto person = create_entity(file, "IfcPerson");
    ifcapi::detail::write_string_attr(person, is_ifc2x3(file) ? "Id" : "Identification", identification);
    ifcapi::detail::write_string_attr(person, "FamilyName", family_name);
    ifcapi::detail::write_string_attr(person, "GivenName", given_name);
    return person;
}

express::Base owner_add_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person,
    express::Base* organisation)
{
    auto result = create_entity(file, "IfcPersonAndOrganization");
    ifcapi::detail::write_ref_attr(result, "ThePerson", detail::deref_or_empty(person));
    ifcapi::detail::write_ref_attr(result, "TheOrganization", detail::deref_or_empty(organisation));
    return result;
}

express::Base owner_add_role(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& role)
{
    auto assigned_object_value = detail::deref_or_empty(assigned_object);
    auto result = create_entity(file, "IfcActorRole");
    ifcapi::detail::write_enum_attr(result, "Role", "ARCHITECT");
    if (!role.empty() && !ifcapi::detail::write_enum_attr(result, "Role", role)) {
        ifcapi::detail::write_enum_attr(result, "Role", "USERDEFINED");
        ifcapi::detail::write_string_attr(result, "UserDefinedRole", role);
    }
    append_ref(assigned_object_value, "Roles", result);
    return result;
}

express::Base owner_assign_actor(
    ifcopenshell::file* file,
    express::Base* relating_actor,
    express::Base* related_object,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    auto relating_actor_value = detail::deref_or_empty(relating_actor);
    auto related_object_value = detail::deref_or_empty(related_object);
    auto owner_history_value = detail::deref_or_empty(owner_history);
    auto user_value = detail::deref_or_empty(user);
    auto application_value = detail::deref_or_empty(application);
    for (auto rel : inverse_entities(related_object_value, "HasAssignments")) {
        if (is_a(rel, "IfcRelAssignsToActor")
            && ifcapi::detail::read_ref_attr(rel, "RelatingActor") == relating_actor_value) {
            return rel;
        }
    }

    express::Base rel;
    auto actor_rels = inverse_entities(relating_actor_value, "IsActingUpon");
    if (!actor_rels.empty()) {
        rel = actor_rels.front();
    }

    if (rel) {
        append_ref(rel, "RelatedObjects", related_object_value);
        ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
        return rel;
    }

    rel = create_entity(file, "IfcRelAssignsToActor");
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history_value, user_value, application_value));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object_value});
    ifcapi::detail::write_ref_attr(rel, "RelatingActor", relating_actor_value);
    return rel;
}

void owner_remove_actor(ifcopenshell::file* file, express::Base* actor) {
    ifcapi::detail::remove_with_history(file, detail::deref_or_empty(actor));
}

void owner_remove_address(ifcopenshell::file* file, express::Base* address) {
    auto address_value = detail::deref_or_empty(address);
    for (auto inverse : inverse_entities(file, address_value)) {
        const auto type = exact_class_name(inverse);
        if ((type == "IfcOrganization" || type == "IfcPerson")
            && aggregate_is_singleton(inverse, "Addresses", address_value)) {
            ifcapi::detail::write_blank_attr(inverse, "Addresses");
        }
    }
    file->remove_entity(address_value);
}

void owner_remove_application(ifcopenshell::file* file, express::Base* application) {
    file->remove_entity(detail::deref_or_empty(application));
}

void owner_remove_organisation(ifcopenshell::file* file, express::Base* organisation) {
    auto organisation_value = detail::deref_or_empty(organisation);
    for (auto role : ifcapi::detail::read_ref_aggregate(organisation_value, "Roles")) {
        if (total_inverses(file, role) == 1) owner_remove_role(file, &role);
    }
    for (auto address : ifcapi::detail::read_ref_aggregate(organisation_value, "Addresses")) {
        if (total_inverses(file, address) == 1) owner_remove_address(file, &address);
    }
    for (auto inverse : inverse_entities(file, organisation_value)) {
        if (is_a(inverse, "IfcOrganizationRelationship")) {
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingOrganization"), organisation_value)
                || aggregate_is_singleton(inverse, "RelatedOrganizations", organisation_value)) {
                file->remove_entity(inverse);
            }
        } else if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", organisation_value)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcPersonAndOrganization")) {
            owner_remove_person_and_organisation(file, &inverse);
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")
            && !is_a(inverse, "IfcOrganizationRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, organisation_value);
        } else if (is_a(inverse, "IfcApplication")) {
            owner_remove_application(file, &inverse);
        }
    }
    file->remove_entity(organisation_value);
}

void owner_remove_person(ifcopenshell::file* file, express::Base* person) {
    auto person_value = detail::deref_or_empty(person);
    for (auto role : ifcapi::detail::read_ref_aggregate(person_value, "Roles")) {
        if (total_inverses(file, role) == 1) owner_remove_role(file, &role);
    }
    for (auto address : ifcapi::detail::read_ref_aggregate(person_value, "Addresses")) {
        if (total_inverses(file, address) == 1) owner_remove_address(file, &address);
    }
    for (auto inverse : inverse_entities(file, person_value)) {
        if (is_a(inverse, "IfcWorkControl")) {
            if (aggregate_is_singleton(inverse, "Creators", person_value)) {
                ifcapi::detail::write_blank_attr(inverse, "Creators");
            }
        } else if (is_a(inverse, "IfcInventory")) {
            if (aggregate_is_singleton(inverse, "ResponsiblePersons", person_value) && is_ifc2x3(file)) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", person_value)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcPersonAndOrganization")) {
            owner_remove_person_and_organisation(file, &inverse);
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, person_value);
        }
    }
    file->remove_entity(person_value);
}

void owner_remove_person_and_organisation(
    ifcopenshell::file* file,
    express::Base* person_and_organisation)
{
    auto person_and_organisation_value = detail::deref_or_empty(person_and_organisation);
    for (auto inverse : inverse_entities(file, person_and_organisation_value)) {
        if (is_a(inverse, "IfcDocumentInformation")) {
            if (aggregate_is_singleton(inverse, "Editors", person_and_organisation_value)) {
                ifcapi::detail::write_blank_attr(inverse, "Editors");
            }
        } else if (is_a(inverse, "IfcActor")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, person_and_organisation_value);
        } else if (is_a(inverse, "IfcOwnerHistory")) {
            file->remove_entity(inverse);
        }
    }
    file->remove_entity(person_and_organisation_value);
}

void owner_remove_role(ifcopenshell::file* file, express::Base* role) {
    auto role_value = detail::deref_or_empty(role);
    for (auto inverse : inverse_entities(file, role_value)) {
        const auto type = exact_class_name(inverse);
        if ((type == "IfcOrganization" || type == "IfcPerson" || type == "IfcPersonAndOrganization")
            && aggregate_is_singleton(inverse, "Roles", role_value)) {
            ifcapi::detail::write_blank_attr(inverse, "Roles");
        } else if (is_a(inverse, "IfcResourceLevelRelationship")) {
            remove_resource_level_relationship_if_single(file, inverse, role_value);
        }
    }
    file->remove_entity(role_value);
}

void owner_unassign_actor(
    ifcopenshell::file* file,
    express::Base* relating_actor,
    express::Base* related_object,
    express::Base* user,
    express::Base* application)
{
    auto relating_actor_value = detail::deref_or_empty(relating_actor);
    auto related_object_value = detail::deref_or_empty(related_object);
    auto user_value = detail::deref_or_empty(user);
    auto application_value = detail::deref_or_empty(application);
    for (auto rel : inverse_entities(related_object_value, "HasAssignments")) {
        if (!is_a(rel, "IfcRelAssignsToActor")
            || ifcapi::detail::read_ref_attr(rel, "RelatingActor") != relating_actor_value) {
            continue;
        }
        auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        if (related_objects.size() == 1) {
            ifcapi::detail::remove_with_history(file, rel);
            return;
        }
        related_objects.erase(std::remove(related_objects.begin(), related_objects.end(), related_object_value), related_objects.end());
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
        ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
    }
}

} // namespace bindings
} // namespace ifcapi
