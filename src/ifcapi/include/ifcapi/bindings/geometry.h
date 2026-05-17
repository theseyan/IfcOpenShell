/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<IfcUtil::IfcBaseClass*> geometry_add_boolean(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* first_item,
    const std::vector<const IfcUtil::IfcBaseClass*>& second_items,
    const std::string& operator_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_axis_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    const std::vector<std::vector<double>>& axis);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_footprint_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    const std::vector<const IfcUtil::IfcBaseClass*>& curves);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_mesh_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    const std::vector<std::vector<std::vector<double>>>& vertices,
    const std::vector<std::vector<std::vector<std::vector<int>>>>& faces,
    bool force_faceted_brep);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_shape_aspect(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* representation,
    IfcUtil::IfcBaseClass* part_of_product,
    const char* description,
    bool has_description);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_topology_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    IfcUtil::IfcBaseClass* item,
    const char* representation_identifier,
    bool has_representation_identifier,
    const char* representation_type,
    bool has_representation_type);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_wall_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    double length,
    double height,
    const std::string& direction_sense,
    double offset,
    double thickness,
    double x_angle,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<const IfcUtil::IfcBaseClass*>& clipping_entities,
    const std::vector<const IfcUtil::IfcBaseClass*>& booleans);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_slab_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    double depth,
    const std::string& direction_sense,
    double offset,
    double x_angle,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<const IfcUtil::IfcBaseClass*>& clipping_entities,
    const std::vector<std::vector<double>>& polyline,
    bool has_polyline);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_create_2pt_wall(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* context,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    double elevation,
    double height,
    double thickness,
    bool is_si);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_connect_wall(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* wall1,
    IfcUtil::IfcBaseClass* wall2,
    bool is_atpath,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_add_window_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    double overall_height,
    double overall_width,
    const std::vector<std::vector<int>>& panel_schema,
    const std::vector<double>& lining_properties,
    const std::vector<std::vector<double>>& panel_properties,
    IfcUtil::IfcBaseClass* part_of_product,
    double glass_thickness);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_clip_solid(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING IfcUtil::IfcBaseClass* geometry_clip_solid_bounded(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    const std::vector<std::vector<double>>& boundary_points,
    const std::vector<double>& boundary_position,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);
IFCAPI_BINDING bool geometry_validate_type(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation,
    IfcUtil::IfcBaseClass* preferred_item);
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
