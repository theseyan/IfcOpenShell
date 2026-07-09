/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

/**
 * Mesh item data for creating an IFC shape representation.
 */
struct GeometryAddMeshRepresentationOptions {
    /// Vertices for each mesh item. Each point is an XYZ coordinate.
    std::vector<std::vector<std::vector<double>>> vertices;
    /// Faces for each mesh item. Each face may contain one outer loop and optional inner loops.
    std::vector<std::vector<std::vector<std::vector<int>>>> faces;
    /// Force faceted BRep output instead of tessellated face sets. Defaults to false.
    std::optional<bool> force_faceted_brep;
};

/**
 * Options for creating or updating an IFC shape aspect.
 */
struct GeometryAddShapeAspectOptions {
    /// Shape aspect name.
    std::string name;
    /// Items that belong to the aspect representation.
    std::vector<express::Base> items;
    /// Representation containing the aspect items.
    express::Base representation;
    /// Product definition shape that owns the aspect.
    express::Base part_of_product;
    /// Optional shape aspect description.
    std::optional<std::string> description;
};

/**
 * Options for an IFC topology representation.
 */
struct GeometryAddTopologyRepresentationOptions {
    express::Base context;
    express::Base item;
    std::optional<std::string> representation_identifier;
    std::optional<std::string> representation_type;
};

/**
 * Options for slab-style swept solid representation creation.
 */
struct GeometryAddSlabRepresentationOptions {
    express::Base context;
    double depth = 0.0;
    std::string direction_sense;
    double offset = 0.0;
    double x_angle = 0.0;
    std::vector<int32_t> clipping_kinds;
    std::vector<std::vector<double>> clipping_locations;
    std::vector<std::vector<double>> clipping_normals;
    std::vector<express::Base> clipping_entities;
    std::optional<std::vector<std::vector<double>>> polyline;
};

struct GeometryAddWallRepresentationOptions {
    express::Base context;
    double length = 0.0;
    double height = 0.0;
    std::string direction_sense;
    double offset = 0.0;
    double thickness = 0.0;
    double x_angle = 0.0;
    std::vector<int32_t> clipping_kinds;
    std::vector<std::vector<double>> clipping_locations;
    std::vector<std::vector<double>> clipping_normals;
    std::vector<express::Base> clipping_entities;
    std::vector<express::Base> booleans;
};

struct GeometryCreate2PtWallOptions {
    express::Base element;
    express::Base context;
    std::vector<double> start;
    std::vector<double> end;
    double elevation = 0.0;
    double height = 0.0;
    double thickness = 0.0;
    bool is_si = true;
};

struct GeometryConnectWallOptions {
    express::Base first_wall;
    express::Base second_wall;
    bool is_atpath = false;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct GeometryRegenerateWallRepresentationOptions {
    express::Base wall;
    double length = 0.0;
    double height = 0.0;
    std::optional<double> angle;
};

struct GeometryAddWindowRepresentationOptions {
    express::Base context;
    double overall_height = 0.0;
    double overall_width = 0.0;
    std::vector<std::vector<int>> panel_schema;
    std::vector<double> lining_properties;
    std::vector<std::vector<double>> panel_properties;
    std::optional<express::Base> part_of_product;
    double glass_thickness = 0.01;
};

struct GeometryAddDoorRepresentationOptions {
    express::Base context;
    double overall_height = 0.0;
    double overall_width = 0.0;
    std::string operation_type;
    std::vector<double> lining_properties;
    std::vector<double> panel_properties;
    std::optional<express::Base> part_of_product;
    double unit_scale = 1.0;
};

struct GeometryAddRailingRepresentationOptions {
    express::Base context;
    std::vector<std::vector<double>> railing_path;
    bool use_manual_supports = false;
    double support_spacing = 1.0;
    double railing_diameter = 0.05;
    double clear_width = 0.05;
    std::string terminal_type;
    double height = 1.0;
    bool looped_path = false;
    double unit_scale = 1.0;
};

struct GeometryClipSolidOptions {
    express::Base item;
    std::vector<double> location;
    std::vector<double> normal;
    std::optional<express::Base> element;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct GeometryClipSolidBoundedOptions {
    express::Base item;
    std::vector<double> location;
    std::vector<double> normal;
    std::vector<std::vector<double>> boundary_points;
    std::vector<double> boundary_position;
    std::optional<express::Base> element;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct GeometryRemoveRepresentationOptions {
    std::optional<bool> should_keep_named_profiles;
};

struct GeometryCopyRepresentationOptions {
    express::Base source;
    express::Base target;
    std::optional<std::string> context_identifier;
};

struct GeometryEditObjectPlacementOptions {
    express::Base product;
    std::vector<double> matrix;
    bool is_si = true;
    bool should_transform_children = false;
};

struct GeometryConnectElementOptions {
    express::Base relating_element;
    express::Base related_element;
    std::optional<std::string> description;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct GeometryConnectPathOptions {
    express::Base relating_element;
    express::Base related_element;
    std::string relating_connection;
    std::string related_connection;
    std::optional<std::string> description;
    std::optional<express::Base> connection_geometry;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct GeometryDisconnectPathOptions {
    std::optional<express::Base> element;
    std::optional<std::string> connection_type;
    std::optional<express::Base> relating_element;
    std::optional<express::Base> related_element;
};

struct GeometryValidateTypeOptions {
    std::optional<express::Base> preferred_item;
};

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
    const GeometryAddMeshRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_shape_aspect(
    ifcopenshell::file* file,
    const GeometryAddShapeAspectOptions& options);
IFCAPI_BINDING express::Base geometry_add_topology_representation(
    ifcopenshell::file* file,
    const GeometryAddTopologyRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_wall_representation(
    ifcopenshell::file* file,
    const GeometryAddWallRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_slab_representation(
    ifcopenshell::file* file,
    const GeometryAddSlabRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_create_2pt_wall(
    ifcopenshell::file* file,
    const GeometryCreate2PtWallOptions& options);
IFCAPI_BINDING express::Base geometry_connect_wall(
    ifcopenshell::file* file,
    const GeometryConnectWallOptions& options);
IFCAPI_BINDING express::Base geometry_regenerate_wall_representation(
    ifcopenshell::file* file,
    const GeometryRegenerateWallRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_window_representation(
    ifcopenshell::file* file,
    const GeometryAddWindowRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_door_representation(
    ifcopenshell::file* file,
    const GeometryAddDoorRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_add_railing_representation(
    ifcopenshell::file* file,
    const GeometryAddRailingRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_clip_solid(
    ifcopenshell::file* file,
    const GeometryClipSolidOptions& options);
IFCAPI_BINDING express::Base geometry_clip_solid_bounded(
    ifcopenshell::file* file,
    const GeometryClipSolidBoundedOptions& options);
IFCAPI_BINDING bool geometry_validate_type(
    ifcopenshell::file* file,
    express::Base* representation,
    const GeometryValidateTypeOptions& options);
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
    const GeometryRemoveRepresentationOptions& options);
IFCAPI_BINDING express::Base geometry_copy_representation(
    ifcopenshell::file* file,
    const GeometryCopyRepresentationOptions& options);
IFCAPI_BINDING std::vector<double> geometry_profile_extents(
    ifcopenshell::file* file,
    express::Base* profile);
IFCAPI_BINDING express::Base geometry_edit_object_placement(
    ifcopenshell::file* file,
    const GeometryEditObjectPlacementOptions& options);
IFCAPI_BINDING express::Base geometry_connect_element(
    ifcopenshell::file* file,
    const GeometryConnectElementOptions& options);
IFCAPI_BINDING void geometry_disconnect_element(
    ifcopenshell::file* file,
    express::Base* relating_element,
    express::Base* related_element);
IFCAPI_BINDING express::Base geometry_connect_path(
    ifcopenshell::file* file,
    const GeometryConnectPathOptions& options);
IFCAPI_BINDING void geometry_disconnect_path(
    ifcopenshell::file* file,
    const GeometryDisconnectPathOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOMETRY_H */
