/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IFCAPI_OWNED std::vector<express::Base> geometry_add_boolean(
    ifcopenshell::file* file,
    express::Base* first_item,
    const std::vector<express::Base>& second_items,
    const std::string& operator_type);
IFCAPI_BINDING express::Base geometry_add_axis_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<std::vector<double>>& axis);
IFCAPI_BINDING express::Base geometry_add_footprint_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<express::Base>& curves);
IFCAPI_BINDING express::Base geometry_add_mesh_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<std::vector<std::vector<double>>>& vertices,
    const std::vector<std::vector<std::vector<std::vector<int>>>>& faces,
    bool force_faceted_brep);
IFCAPI_BINDING express::Base geometry_add_shape_aspect(
    ifcopenshell::file* file,
    const std::string& name,
    const std::vector<express::Base>& items,
    express::Base* representation,
    express::Base* part_of_product,
    const char* description,
    bool has_description);
IFCAPI_BINDING express::Base geometry_add_topology_representation(
    ifcopenshell::file* file,
    express::Base* context,
    express::Base* item,
    const char* representation_identifier,
    bool has_representation_identifier,
    const char* representation_type,
    bool has_representation_type);
IFCAPI_BINDING express::Base geometry_add_wall_representation(
    ifcopenshell::file* file,
    express::Base* context,
    double length,
    double height,
    const std::string& direction_sense,
    double offset,
    double thickness,
    double x_angle,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<express::Base>& clipping_entities,
    const std::vector<express::Base>& booleans);
IFCAPI_BINDING express::Base geometry_add_slab_representation(
    ifcopenshell::file* file,
    express::Base* context,
    double depth,
    const std::string& direction_sense,
    double offset,
    double x_angle,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<express::Base>& clipping_entities,
    const std::vector<std::vector<double>>& polyline,
    bool has_polyline);
IFCAPI_BINDING express::Base geometry_create_2pt_wall(
    ifcopenshell::file* file,
    express::Base* element,
    express::Base* context,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    double elevation,
    double height,
    double thickness,
    bool is_si);
IFCAPI_BINDING express::Base geometry_connect_wall(
    ifcopenshell::file* file,
    express::Base* wall1,
    express::Base* wall2,
    bool is_atpath,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base geometry_regenerate_wall_representation(
    ifcopenshell::file* file,
    express::Base* wall,
    double length,
    double height,
    double angle,
    bool has_angle);
IFCAPI_BINDING express::Base geometry_add_window_representation(
    ifcopenshell::file* file,
    express::Base* context,
    double overall_height,
    double overall_width,
    const std::vector<std::vector<int>>& panel_schema,
    const std::vector<double>& lining_properties,
    const std::vector<std::vector<double>>& panel_properties,
    IFCAPI_NULLABLE express::Base* part_of_product,
    double glass_thickness);
IFCAPI_BINDING express::Base geometry_add_door_representation(
    ifcopenshell::file* file,
    express::Base* context,
    double overall_height,
    double overall_width,
    const std::string& operation_type,
    const std::vector<double>& lining_properties,
    const std::vector<double>& panel_properties,
    IFCAPI_NULLABLE express::Base* part_of_product,
    double unit_scale);
IFCAPI_BINDING express::Base geometry_add_railing_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<std::vector<double>>& railing_path,
    bool use_manual_supports,
    double support_spacing,
    double railing_diameter,
    double clear_width,
    const std::string& terminal_type,
    double height,
    bool looped_path,
    double unit_scale);
IFCAPI_BINDING express::Base geometry_clip_solid(
    ifcopenshell::file* file,
    express::Base* item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    IFCAPI_NULLABLE express::Base* element,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base geometry_clip_solid_bounded(
    ifcopenshell::file* file,
    express::Base* item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    const std::vector<std::vector<double>>& boundary_points,
    const std::vector<double>& boundary_position,
    IFCAPI_NULLABLE express::Base* element,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING bool geometry_validate_type(
    ifcopenshell::file* file,
    express::Base* representation,
    IFCAPI_NULLABLE express::Base* preferred_item);
IFCAPI_BINDING void geometry_remove_boolean(
    ifcopenshell::file* file,
    express::Base* item);
IFCAPI_BINDING express::Base geometry_assign_representation(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* representation);
IFCAPI_BINDING express::Base geometry_map_representation(
    ifcopenshell::file* file,
    express::Base* representation);
IFCAPI_BINDING void geometry_unassign_representation(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* representation);
IFCAPI_BINDING void geometry_remove_representation(
    ifcopenshell::file* file,
    express::Base* representation,
    bool should_keep_named_profiles);
IFCAPI_BINDING express::Base geometry_copy_representation(
    ifcopenshell::file* file,
    express::Base* source,
    express::Base* target,
    const char* context_identifier);
IFCAPI_BINDING std::vector<double> geometry_profile_extents(
    ifcopenshell::file* file,
    express::Base* profile);
IFCAPI_BINDING express::Base geometry_edit_object_placement(
    ifcopenshell::file* file,
    express::Base* product,
    const std::vector<double>& matrix,
    bool is_si,
    bool should_transform_children);
IFCAPI_BINDING express::Base geometry_connect_element(
    ifcopenshell::file* file,
    express::Base* relating_element,
    express::Base* related_element,
    const char* description,
    bool has_description,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void geometry_disconnect_element(
    ifcopenshell::file* file,
    express::Base* relating_element,
    express::Base* related_element);
IFCAPI_BINDING express::Base geometry_connect_path(
    ifcopenshell::file* file,
    express::Base* relating_element,
    express::Base* related_element,
    const std::string& relating_connection,
    const std::string& related_connection,
    const char* description,
    bool has_description,
    IFCAPI_NULLABLE express::Base* connection_geometry,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void geometry_disconnect_path(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* element,
    const char* connection_type,
    bool has_connection_type,
    IFCAPI_NULLABLE express::Base* relating_element,
    IFCAPI_NULLABLE express::Base* related_element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOMETRY_H */
