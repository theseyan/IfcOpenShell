/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<IfcUtil::IfcBaseClass*> geometry_add_boolean(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* first_item,
    const std::vector<const IfcUtil::IfcBaseClass*>& second_items,
    const std::string& operator_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_shape_aspect(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* representation,
    IfcUtil::IfcBaseClass* part_of_product,
    const char* description,
    bool has_description);
IFCAPI_BINDING void geometry_remove_boolean(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_assign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_map_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING void geometry_unassign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING void geometry_remove_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation,
    bool should_keep_named_profiles);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_copy_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* source,
    IfcUtil::IfcBaseClass* target,
    const char* context_identifier);
IFCAPI_BINDING std::vector<double> geometry_profile_extents(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_edit_object_placement(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    const std::vector<double>& matrix,
    bool is_si,
    bool should_transform_children);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_connect_element(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element,
    const char* description,
    bool has_description,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void geometry_disconnect_element(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_connect_path(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element,
    const std::string& relating_connection,
    const std::string& related_connection,
    const char* description,
    bool has_description,
    IfcUtil::IfcBaseClass* connection_geometry,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING void geometry_disconnect_path(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const char* connection_type,
    bool has_connection_type,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOMETRY_H */
