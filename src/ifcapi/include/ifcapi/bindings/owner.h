/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_OWNER_H
#define IFCAPI_BINDINGS_OWNER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base owner_add_actor(
    ifcopenshell::file* file,
    express::Base* actor,
    const std::string& ifc_class,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base owner_add_address(
    ifcopenshell::file* file,
    express::Base* assigned_object,
    const std::string& ifc_class);
IFCAPI_BINDING express::Base owner_add_application(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* application_developer,
    const std::string& version,
    const std::string& application_full_name,
    const std::string& application_identifier,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
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
    express::Base* relating_actor,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base owner_create_owner_history(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
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
    express::Base* relating_actor,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base owner_update_owner_history(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* element,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_OWNER_H */
