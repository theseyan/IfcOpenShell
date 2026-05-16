/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_OWNER_H
#define IFCAPI_BINDINGS_OWNER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* actor,
    const std::string& ifc_class,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_address(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* assigned_object,
    const std::string& ifc_class);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_application(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* application_developer,
    const std::string& version,
    const std::string& application_full_name,
    const std::string& application_identifier,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_organisation(
    IfcParse::IfcFile* file,
    const std::string& identification,
    const std::string& name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_person(
    IfcParse::IfcFile* file,
    const std::string& identification,
    const std::string& family_name,
    const std::string& given_name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_person_and_organisation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* person,
    IfcUtil::IfcBaseClass* organisation);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_add_role(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* assigned_object,
    const std::string& role);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_assign_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_actor,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_create_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void owner_remove_actor(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* actor);
IFCAPI_BINDING void owner_remove_address(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* address);
IFCAPI_BINDING void owner_remove_application(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void owner_remove_organisation(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* organisation);
IFCAPI_BINDING void owner_remove_person(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* person);
IFCAPI_BINDING void owner_remove_person_and_organisation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* person_and_organisation);
IFCAPI_BINDING void owner_remove_role(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* role);
IFCAPI_BINDING void owner_unassign_actor(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_actor,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* owner_update_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_OWNER_H */
