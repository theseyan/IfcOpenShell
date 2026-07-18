/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_GEOMETRY_H
#define IFCAPI_BINDINGS_GEOMETRY_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/bindings/types.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ifcapi {
namespace bindings {

enum class GeometryDirectionSense { POSITIVE, NEGATIVE };
enum class GeometryBooleanOperator { DIFFERENCE, INTERSECTION, UNION };
enum class GeometryPathConnectionType { ATSTART, ATEND, ATPATH, NOTDEFINED };
enum class GeometryRailingTerminalType {
    RETURN_180 IFCAPI_LITERAL("180"),
    TO_END_POST,
    TO_WALL,
    TO_FLOOR,
    TO_END_POST_AND_FLOOR,
    NONE,
};

/** A clipping half-space plane in SI metres. */
struct GeometryPlaneClipping {
    /// Point on the clipping plane, in SI metres.
    std::array<double, 3> location;
    /// Plane normal pointing towards the material to discard.
    std::array<double, 3> normal;
};

/** A pre-existing boolean clipping entity that will be copied before use. */
struct GeometryEntityClipping {
    /// IfcBooleanClippingResult (or compatible boolean result) to copy.
    express::Base entity;
};

/** Ordered semantic clipping definition. */
using GeometryClipping = std::variant<GeometryPlaneClipping, GeometryEntityClipping>;

/** One polygonal mesh face, expressed with zero-based vertex indices. */
struct GeometryMeshFace {
    /// Outer boundary loop. At least three indices are required.
    std::vector<std::uint32_t> outer;
    /// Optional inner boundary loops. Each loop requires at least three indices.
    std::optional<std::vector<std::vector<std::uint32_t>>> inner_loops;
};

/** Vertices and faces for one IFC representation item. */
struct GeometryMeshItem {
    /// XYZ vertex coordinates in the units described by unit_scale.
    std::vector<std::array<double, 3>> vertices;
    /// Faces referencing this item's vertices.
    std::vector<GeometryMeshFace> faces;
};

/** Mesh representation input and coordinate conversion options. */
struct GeometryAddMeshRepresentationOptions {
    /// Ordered representation items. At least one item is required.
    std::vector<GeometryMeshItem> items;
    /// Optional XYZ offset applied after unit conversion, in project units.
    std::optional<std::array<double, 3>> coordinate_offset;
    /// Vertex-unit scale relative to SI. Defaults to the file length-unit scale.
    std::optional<double> unit_scale;
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
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Topology item (IfcVertex, IfcEdge, IfcFace, etc.).
    express::Base item;
    /// Optional representation identifier. Defaults to the context identifier when omitted.
    std::optional<std::string> representation_identifier;
    /// Optional representation type (e.g. "Vertex", "Edge"). Auto-detected when omitted.
    std::optional<std::string> representation_type;
};

/**
 * Options for slab-style swept solid representation creation.
 */
struct GeometryAddSlabRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Slab depth (thickness) in SI metres.
    std::optional<double> depth;
    /// Extrusion direction sense: "POSITIVE" or "NEGATIVE".
    std::optional<GeometryDirectionSense> direction_sense;
    /// Offset from the reference plane along the extrusion direction, in SI metres.
    std::optional<double> offset;
    /// Angle of the extrusion direction from vertical, in radians.
    std::optional<double> x_angle;
    /// Ordered plane or entity clippings. Defaults to no clippings.
    std::optional<std::vector<GeometryClipping>> clippings;
    /// Optional outer boundary polyline in SI metres. Defaults to a unit square when omitted.
    std::optional<std::vector<std::vector<double>>> polyline;
};

/**
 * Options for wall-style swept solid representation creation.
 */
struct GeometryAddWallRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Wall length in SI metres.
    std::optional<double> length;
    /// Wall height in SI metres.
    std::optional<double> height;
    /// Extrusion direction sense: "POSITIVE" or "NEGATIVE".
    std::optional<GeometryDirectionSense> direction_sense;
    /// Offset from the reference plane along the extrusion direction, in SI metres.
    std::optional<double> offset;
    /// Wall thickness in SI metres.
    std::optional<double> thickness;
    /// Angle of the extrusion direction from vertical, in radians.
    std::optional<double> x_angle;
    /// Ordered plane or entity clippings. Defaults to no clippings.
    std::optional<std::vector<GeometryClipping>> clippings;
    /// Pre-existing boolean operand entities applied before clippings. Defaults to empty.
    std::optional<std::vector<express::Base>> booleans;
};

/**
 * Options for creating a profile-based swept solid representation.
 */
struct GeometryAddProfileRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// IfcProfileDef to extrude.
    express::Base profile;
    /// Extrusion depth in SI metres.
    std::optional<double> depth;
    /// Canonical cardinal-point name. Defaults to "mid-depth centre".
    std::optional<std::string> cardinal_point;
    /// Optional placement Z axis; defaults to (0, 0, 1) when omitted.
    std::optional<std::array<double, 3>> placement_z_axis;
    /// Optional placement X axis; defaults to (1, 0, 0) when omitted.
    std::optional<std::array<double, 3>> placement_x_axis;
    /// Ordered plane or entity clippings. Defaults to no clippings.
    std::optional<std::vector<GeometryClipping>> clippings;
};

/**
 * Options for creating a wall from two endpoints.
 */
struct GeometryCreate2PtWallOptions {
    /// IfcWall (or similar) element to receive the geometry.
    express::Base element;
    /// IfcGeometricRepresentationContext for the body representation.
    express::Base context;
    /// XY start point of the wall baseline.
    std::array<double, 2> start;
    /// XY end point of the wall baseline.
    std::array<double, 2> end;
    /// Wall base elevation in SI metres (or model units when is_si is false).
    double elevation;
    /// Wall height in SI metres (or model units when is_si is false).
    double height;
    /// Wall thickness in SI metres (or model units when is_si is false).
    double thickness;
    /// If true, start/end/elevation/height/thickness are in SI metres. Defaults to true.
    std::optional<bool> is_si = true;
};

/**
 * Options for connecting two walls.
 */
struct GeometryConnectWallOptions {
    /// First wall in the connection.
    express::Base first_wall;
    /// Second wall in the connection.
    express::Base second_wall;
    /// If true, connect along the path (ATPATH) instead of at a terminal end.
    std::optional<bool> is_atpath = false;
    /// Optional existing IfcOwnerHistory for the relationship.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for regenerating a wall's body and axis representations.
 */
struct GeometryRegenerateWallRepresentationOptions {
    /// IfcWall entity whose representations to regenerate.
    express::Base wall;
    /// Wall length in SI metres.
    double length = 0.0;
    /// Wall height in SI metres.
    double height = 0.0;
    /// Optional extrusion angle in radians. Defaults to 0 when omitted.
    std::optional<double> angle;
};

/** Supported window panel partition layouts. */
enum class GeometryWindowPartitionType {
    SINGLE_PANEL,
    DOUBLE_PANEL_HORIZONTAL,
    DOUBLE_PANEL_VERTICAL,
    TRIPLE_PANEL_BOTTOM,
    TRIPLE_PANEL_HORIZONTAL,
    TRIPLE_PANEL_LEFT,
    TRIPLE_PANEL_RIGHT,
    TRIPLE_PANEL_TOP,
    TRIPLE_PANEL_VERTICAL,
};

/** Semantic window lining dimensions in model units. */
struct GeometryWindowLiningProperties {
    std::optional<double> lining_depth;
    std::optional<double> lining_thickness;
    std::optional<double> lining_offset;
    std::optional<double> lining_to_panel_offset_x;
    std::optional<double> lining_to_panel_offset_y;
    std::optional<double> mullion_thickness;
    std::optional<double> first_mullion_offset;
    std::optional<double> second_mullion_offset;
    std::optional<double> transom_thickness;
    std::optional<double> first_transom_offset;
    std::optional<double> second_transom_offset;
};

/** Semantic dimensions for one window panel in model units. */
struct GeometryWindowPanelProperties {
    std::optional<double> frame_depth;
    std::optional<double> frame_thickness;
};

/** Options for creating a window representation. */
struct GeometryAddWindowRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Overall window height in model units. Defaults to 0.9 metres.
    std::optional<double> overall_height = std::nullopt;
    /// Overall window width in model units. Defaults to 0.6 metres.
    std::optional<double> overall_width = std::nullopt;
    /// Window panel partition layout. Defaults to SINGLE_PANEL.
    std::optional<GeometryWindowPartitionType> partition_type = std::nullopt;
    /// Optional semantic lining property overrides.
    std::optional<GeometryWindowLiningProperties> lining_properties = std::nullopt;
    /// Optional semantic panel property overrides. Omission creates one default panel.
    std::optional<std::vector<GeometryWindowPanelProperties>> panel_properties = std::nullopt;
    /// Optional IfcProductDefinitionShape to attach shape aspects to.
    std::optional<express::Base> part_of_product;
    /// Scale factor from model units to SI metres. Calculated from the file when omitted.
    std::optional<double> unit_scale = std::nullopt;
};

/** Supported door panel operation layouts. */
enum class GeometryDoorOperationType {
    SINGLE_SWING_LEFT,
    SINGLE_SWING_RIGHT,
    DOUBLE_SWING_RIGHT,
    DOUBLE_SWING_LEFT,
    DOUBLE_DOOR_SINGLE_SWING,
    DOUBLE_DOOR_DOUBLE_SWING,
    SLIDING_TO_LEFT,
    SLIDING_TO_RIGHT,
    DOUBLE_DOOR_SLIDING,
};

/** Semantic door lining dimensions in model units. */
struct GeometryDoorLiningProperties {
    std::optional<double> lining_depth;
    std::optional<double> lining_thickness;
    std::optional<double> lining_offset;
    std::optional<double> lining_to_panel_offset_x;
    std::optional<double> lining_to_panel_offset_y;
    std::optional<double> transom_thickness;
    std::optional<double> transom_offset;
    std::optional<double> casing_depth;
    std::optional<double> casing_thickness;
    std::optional<double> threshold_depth;
    std::optional<double> threshold_thickness;
    std::optional<double> threshold_offset;
};

/** Semantic door panel dimensions in model units. */
struct GeometryDoorPanelProperties {
    std::optional<double> panel_depth;
    std::optional<double> panel_width;
    std::optional<double> frame_depth;
    std::optional<double> frame_thickness;
};

/** Options for creating a door representation. */
struct GeometryAddDoorRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Overall door height in model units. Defaults to 2 metres.
    std::optional<double> overall_height = std::nullopt;
    /// Overall door width in model units. Defaults to 0.9 metres.
    std::optional<double> overall_width = std::nullopt;
    /// Door operation layout. Defaults to SINGLE_SWING_LEFT.
    std::optional<GeometryDoorOperationType> operation_type = std::nullopt;
    /// Optional semantic lining property overrides.
    std::optional<GeometryDoorLiningProperties> lining_properties = std::nullopt;
    /// Optional semantic panel property overrides.
    std::optional<GeometryDoorPanelProperties> panel_properties = std::nullopt;
    /// Optional IfcProductDefinitionShape to attach a shape aspect to.
    std::optional<express::Base> part_of_product;
    /// Scale factor from model units to SI metres. Calculated from the file when omitted.
    std::optional<double> unit_scale = std::nullopt;
};

/** Pure geometry for one wall-mounted handrail support, in project units. */
struct GeometryRailingSupport {
    /// Ordered three-point support arc polyline.
    std::vector<std::vector<double>> arc_polyline;
    /// Radius swept along the support arc.
    double arc_radius;
    /// XYZ position of the wall attachment disk; equals the final arc point.
    std::vector<double> disk_position;
    /// Radius of the attachment disk.
    double disk_radius;
    /// Extrusion depth of the attachment disk.
    double disk_depth;
    /// Signed rotation around Z for the disk's local Y extrusion orientation.
    double disk_z_rotation;
};

/** Pure wall-mounted handrail geometry, with no IFC entities or file dependency. */
struct GeometryWallMountedHandrailResult {
    /// Ordered XYZ centerline points, including arc tangent and midpoint points.
    std::vector<std::vector<double>> handrail_polyline;
    /// Ordered zero-based indices of arc midpoint coordinates.
    std::vector<int> handrail_arc_point_indices;
    /// Radius swept along the handrail centerline.
    double handrail_radius;
    /// Supports ordered by straight run and then position along that run.
    std::vector<GeometryRailingSupport> supports;
};

/** Options for pure wall-mounted handrail computation. */
struct GeometryComputeWallMountedHandrailOptions {
    /// Required unclosed sequence of finite XYZ points in project units.
    std::vector<std::array<double, 3>> railing_path;
    /// Required automatic support spacing in project units; unused in manual mode.
    double support_spacing;
    /// Required positive handrail diameter in project units.
    double railing_diameter;
    /// Required positive clear gap between the wall and tube in project units.
    double clear_width;
    /// Required top-of-handrail height in project units.
    double height;
    /// When true, place supports only on collinear internal subdivision vertices.
    std::optional<bool> use_manual_supports;
    /// Terminal style. When omitted, uses "180".
    std::optional<GeometryRailingTerminalType> terminal_type;
    /// When true, treat the input as an unclosed loop and omit terminal caps.
    std::optional<bool> looped_path;
    /// Project-unit scale in SI metres, used only for fixed metric constants. Defaults to 1.0.
    std::optional<double> unit_scale;
};

/** Options for materializing a railing representation in an IFC file. */
struct GeometryAddRailingRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Optional finite XYZ path. When omitted, uses the documented three-point default path.
    std::optional<std::vector<std::array<double, 3>>> railing_path;
    /// Optional manual-support mode. Defaults to false.
    std::optional<bool> use_manual_supports;
    /// Optional automatic support spacing; defaults to 1000 mm in project units.
    std::optional<double> support_spacing;
    /// Optional tube diameter; defaults to 50 mm in project units.
    std::optional<double> railing_diameter;
    /// Optional clear wall gap; defaults to 40 mm in project units.
    std::optional<double> clear_width;
    /// Optional terminal style; defaults to "180".
    std::optional<GeometryRailingTerminalType> terminal_type;
    /// Optional total height; defaults to 1000 mm in project units.
    std::optional<double> height;
    /// Optional loop mode. Defaults to false.
    std::optional<bool> looped_path;
    /// Optional project-unit scale in SI metres. When omitted, it is read from the file.
    std::optional<double> unit_scale;
};

/**
 * Options for clipping a solid with an infinite half-space plane.
 */
struct GeometryClipSolidOptions {
    /// Solid item to clip (first operand).
    express::Base item;
    /// XYZ point on the clipping plane, in model units.
    std::array<double, 3> location;
    /// Direction ratios of the clipping plane normal.
    std::array<double, 3> normal;
    /// Optional owning element for BBIM_Boolean tracking.
    std::optional<express::Base> element;
    /// Optional existing IfcOwnerHistory.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for clipping a solid with a polygonal-bounded half-space.
 */
struct GeometryClipSolidBoundedOptions {
    /// Solid item to clip (first operand).
    express::Base item;
    /// XYZ point on the clipping plane, in model units.
    std::array<double, 3> location;
    /// Direction ratios of the clipping plane normal.
    std::array<double, 3> normal;
    /// XY points defining the polygonal boundary of the clipping region.
    std::vector<std::array<double, 2>> boundary_points;
    /// XYZ position of the boundary polygon's local origin.
    std::array<double, 3> boundary_position;
    /// Optional owning element for BBIM_Boolean tracking.
    std::optional<express::Base> element;
    /// Optional existing IfcOwnerHistory.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing a representation.
 */
struct GeometryRemoveRepresentationOptions {
    /// If true, IfcProfileDef entities with non-empty ProfileName are preserved. Defaults to true.
    std::optional<bool> should_keep_named_profiles;
};

/**
 * Options for copying a representation between products.
 */
struct GeometryCopyRepresentationOptions {
    /// Source product to copy the representation from.
    express::Base source;
    /// Target product to receive the copied representation.
    express::Base target;
    /// Representation identifier to copy (e.g. "Body"). Defaults to "Body" when omitted.
    std::optional<std::string> context_identifier;
};

/**
 * Options for editing a product's object placement.
 */
struct GeometryEditObjectPlacementOptions {
    /// Product whose ObjectPlacement to set.
    express::Base product;
    /// 16-element row-major 4x4 transformation matrix.
    std::optional<std::array<double, 16>> matrix;
    /// If true, translation components are in SI metres. Defaults to true.
    std::optional<bool> is_si = true;
    /// If true, child local placements are left unchanged so children move with the parent. If false (default), child world positions are preserved.
    std::optional<bool> should_transform_children = false;
};

/**
 * Options for creating an element-to-element connection.
 */
struct GeometryConnectElementOptions {
    /// The element initiating the connection.
    express::Base relating_element;
    /// The element being connected to.
    express::Base related_element;
    /// Optional description of the connection.
    std::optional<std::string> description;
    /// Optional existing IfcOwnerHistory for the relationship.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for creating a path-based element connection.
 */
struct GeometryConnectPathOptions {
    /// The element initiating the connection.
    express::Base relating_element;
    /// The element being connected to.
    express::Base related_element;
    /// Connection type on the relating side: "ATSTART", "ATEND", or "ATPATH".
    GeometryPathConnectionType relating_connection;
    /// Connection type on the related side: "ATSTART", "ATEND", or "ATPATH".
    GeometryPathConnectionType related_connection;
    /// Optional description of the connection.
    std::optional<std::string> description;
    /// Optional IfcConnectionGeometry for the relationship.
    std::optional<express::Base> connection_geometry;
    /// Optional existing IfcOwnerHistory for the relationship.
    std::optional<express::Base> owner_history;
    /// Optional IfcPersonAndOrganization for OwnerHistory creation.
    std::optional<express::Base> user;
    /// Optional IfcApplication for OwnerHistory creation.
    std::optional<express::Base> application;
};

/**
 * Options for disconnecting a path-based element connection.
 */
struct GeometryDisconnectPathOptions {
    /// Element whose connections to remove (used with connection_type).
    std::optional<express::Base> element;
    /// Connection type to match (e.g. "ATSTART"). Used with element.
    std::optional<GeometryPathConnectionType> connection_type;
    /// Relating element of the specific connection to remove.
    std::optional<express::Base> relating_element;
    /// Related element of the specific connection to remove.
    std::optional<express::Base> related_element;
};

/**
 * Options for validating and correcting a representation's type.
 */
struct GeometryValidateTypeOptions {
    /// Preferred first operand for boolean consolidation. Auto-selected when omitted.
    std::optional<express::Base> preferred_item;
};

/**
 * Add boolean operands to a solid representation item.
 *
 * Creates IfcBooleanResult entities (or IfcBooleanClippingResult for
 * DIFFERENCE with half-space solids) by combining the first item with each
 * additional operand. The returned entities are listed in creation order.
 *
 * @param file IFC file that receives the boolean entities.
 * @param first_item Base solid operand.
 * @param second_items Additional operands to apply.
 * @param operator_type Boolean operator: "DIFFERENCE", "UNION", or "INTERSECTION".
 * @return Created boolean result entities, or an empty list if creation fails.
 */
IFCAPI_BINDING std::vector<express::Base> geometry_add_boolean(
    ifcopenshell::file* file,
    express::Base* first_item,
    const std::vector<express::Base>& second_items,
    GeometryBooleanOperator operator_type);

/**
 * Create an axis representation (Curve2D or Curve3D) from a polyline.
 *
 * Points are converted from SI metres to project units.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * @param axis Ordered XY or XYZ points defining the axis curve.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_axis_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::variant<std::vector<std::array<double, 2>>, std::vector<std::array<double, 3>>>& axis);

/**
 * Create a footprint representation (GeometricCurveSet) from curves.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * @param curves IfcCurve entities to include in the footprint.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_footprint_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<express::Base>& curves);

/**
 * Create a mesh representation from vertices and face indices.
 *
 * Uses IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3). IFC2X3
 * always uses faceted BRep regardless of force_faceted_brep.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * Each item binds its vertex list to its faces. Face indices are zero-based.
 * Polygonal face sets support optional inner loops; IFC2X3 and forced faceted
 * BReps reject inner loops. Edges without faces are not supported upstream and
 * are intentionally absent from this native contract.
 *
 * Vertices are divided by unit_scale, then coordinate_offset (in project
 * units) is added. If omitted, unit_scale is calculated from the file.
 *
 * @param options Mesh items, coordinate conversion, and optional faceted BRep override.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_mesh_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const GeometryAddMeshRepresentationOptions& options);

/**
 * Create or update an IfcShapeAspect within a product definition shape.
 *
 * If an aspect with the same name already exists, its items are merged.
 * Items are removed from other aspects in the same product definition shape.
 *
 * @param file IFC file that receives the aspect.
 * @param options Aspect name, items, representation, and owning product.
 * @return IfcShapeAspect entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_shape_aspect(
    ifcopenshell::file* file,
    const GeometryAddShapeAspectOptions& options);

/**
 * Create an IfcTopologyRepresentation for a single topology item.
 *
 * @param file IFC file that receives the representation.
 * @param options Context, topology item, and optional identifier/type.
 * @return IfcTopologyRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_topology_representation(
    ifcopenshell::file* file,
    const GeometryAddTopologyRepresentationOptions& options);

/**
 * Create a wall-style swept solid representation (IfcExtrudedAreaSolid).
 *
 * @param file IFC file that receives the representation.
 * @param options Wall dimensions, direction, clippings, and booleans.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_wall_representation(
    ifcopenshell::file* file,
    const GeometryAddWallRepresentationOptions& options);

/**
 * Create a profile-based IfcExtrudedAreaSolid representation.
 *
 * Depth and plane locations are supplied in SI metres and converted to project
 * length units. Clipping kinds preserve input order but are applied from last
 * to first; entity clippings are copied before their FirstOperand is changed.
 * The placement defaults to Z=(0,0,1), X=(1,0,0), and the origin when no
 * cardinal point is supplied. The result is SweptSolid without clippings and
 * Clipping otherwise.
 *
 * @param file IFC file that receives the representation.
 * @param options Context, profile, extrusion, placement, cardinal point, and clippings.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_profile_representation(
    ifcopenshell::file* file,
    const GeometryAddProfileRepresentationOptions& options);

/**
 * Create a slab-style swept solid representation (IfcExtrudedAreaSolid).
 *
 * @param file IFC file that receives the representation.
 * @param options Slab dimensions, direction, clippings, and boundary polyline.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_slab_representation(
    ifcopenshell::file* file,
    const GeometryAddSlabRepresentationOptions& options);

/**
 * Create a wall from two XY endpoints with body representation and placement.
 *
 * Computes the wall length from the endpoint distance, creates a wall
 * representation, and sets the object placement with rotation to align the
 * wall along the baseline.
 *
 * @param file IFC file that receives the wall geometry.
 * @param options Element, context, endpoints, elevation, height, thickness, and unit flag.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_create_2pt_wall(
    ifcopenshell::file* file,
    const GeometryCreate2PtWallOptions& options);

/**
 * Connect two walls at their nearest endpoint.
 *
 * Computes the intersection of the second wall's axis with the first wall's
 * axis to determine ATSTART or ATEND. When is_atpath is true, the connection
 * type is ATPATH. Creates an IfcRelConnectsPathElements.
 *
 * @param file IFC file that receives the connection.
 * @param options Walls, connection mode, and optional owner history.
 * @return IfcRelConnectsPathElements entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_connect_wall(
    ifcopenshell::file* file,
    const GeometryConnectWallOptions& options);

/**
 * Regenerate a wall's body and axis representations from its material layers.
 *
 * Rebuilds the wall's body and axis representations using its material layers
 * and connected-wall geometry.
 *
 * @param file IFC file containing the wall.
 * @param options Wall entity, length, height, and optional angle.
 * @return New IfcShapeRepresentation for the body, or no result if regeneration fails.
 */
IFCAPI_BINDING express::Base geometry_regenerate_wall_representation(
    ifcopenshell::file* file,
    const GeometryRegenerateWallRepresentationOptions& options);

/**
 * Create a window representation with lining and panel geometry.
 *
 * @param file IFC file that receives the representation.
 * @param options Window dimensions, partition type, and semantic lining/panel properties.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_window_representation(
    ifcopenshell::file* file,
    const GeometryAddWindowRepresentationOptions& options);

/**
 * Create a door representation with lining and panel geometry.
 *
 * @param file IFC file that receives the representation.
 * @param options Door dimensions, operation type, and lining/panel properties.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_door_representation(
    ifcopenshell::file* file,
    const GeometryAddDoorRepresentationOptions& options);

/**
 * Compute wall-mounted handrail geometry without an IFC file or context.
 *
 * The input and output coordinates and dimensions use project units. Fixed
 * metric design constants are divided by the supplied unit scale. Manual mode
 * permits a non-positive unused support-spacing value; automatic mode requires
 * positive spacing. Degenerate edges retain finite sharp vertices and do not
 * produce support or fillet geometry with undefined directions.
 *
 * @param options Required dimensions and path plus optional terminal/support policy.
 * @return Pure handrail and nested support geometry owned by the caller.
 */
IFCAPI_BINDING GeometryWallMountedHandrailResult geometry_compute_wall_mounted_handrail_geometry(
    const GeometryComputeWallMountedHandrailOptions& options);

/**
 * Create a railing representation from the shared pure-compute result.
 *
 * @param file IFC file that receives the representation.
 * @param options Context plus optional path, dimensions, terminal policy, and unit scale.
 * @return IfcShapeRepresentation entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_add_railing_representation(
    ifcopenshell::file* file,
    const GeometryAddRailingRepresentationOptions& options);

/**
 * Clip a solid with an infinite half-space plane (IfcBooleanClippingResult).
 *
 * Creates a DIFFERENCE boolean between the item and an IfcHalfSpaceSolid.
 * If an owning element is provided, the clipping is registered in its
 * BBIM_Boolean property set.
 *
 * @param file IFC file that receives the clipping.
 * @param options Solid, plane point, normal, and optional element/history.
 * @return IfcBooleanClippingResult entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_clip_solid(
    ifcopenshell::file* file,
    const GeometryClipSolidOptions& options);

/**
 * Clip a solid with a polygonal-bounded half-space (IfcPolygonalBoundedHalfSpace).
 *
 * Creates a DIFFERENCE boolean between the item and an
 * IfcPolygonalBoundedHalfSpace. Locations are in model units.
 *
 * @param file IFC file that receives the clipping.
 * @param options Solid, plane, boundary polygon, and optional element/history.
 * @return IfcBooleanClippingResult entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_clip_solid_bounded(
    ifcopenshell::file* file,
    const GeometryClipSolidBoundedOptions& options);

/**
 * Validate and correct a representation's RepresentationType.
 *
 * When the representation contains multiple boolean operands, they are
 * consolidated into a single boolean chain via UNION. Returns true if the
 * final representation type is "CSG" or was successfully auto-detected.
 *
 * @param file IFC file containing the representation.
 * @param representation IfcShapeRepresentation to validate.
 * @param options Optional preferred operand for boolean consolidation.
 * @return True if the representation type is valid or was corrected.
 */
IFCAPI_BINDING bool geometry_validate_type(
    ifcopenshell::file* file,
    express::Base* representation,
    const GeometryValidateTypeOptions& options);

/**
 * Remove boolean operands from a solid representation.
 *
 * Removes boolean operations involving the given item, restores the primary
 * operand in its parent references, and exposes the other operands in the
 * owning representation.
 *
 * @param file IFC file to modify.
 * @param item Solid operand whose boolean chain to remove.
 */
IFCAPI_BINDING void geometry_remove_boolean(
    ifcopenshell::file* file,
    express::Base* item);

/**
 * Assign a representation to a product or type product.
 *
 * For IfcProduct, the representation is appended to the product's
 * IfcProductDefinitionShape. For IfcTypeProduct, an IfcRepresentationMap is
 * created and mapped representations are assigned to all related occurrences.
 * If the product has a type with existing maps and the representation is not
 * already mapped, the assignment is redirected to the type product.
 *
 * @param file IFC file to modify.
 * @param product IfcProduct or IfcTypeProduct entity.
 * @param representation IfcShapeRepresentation entity.
 * @return The product receiving the representation, or no result if assignment fails.
 */
IFCAPI_BINDING express::Base geometry_assign_representation(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* representation);

/**
 * Create a mapped representation (IfcRepresentationMap + IfcMappedItem).
 *
 * Wraps the given representation in an IfcRepresentationMap and returns a new
 * IfcShapeRepresentation of type "MappedRepresentation" containing one
 * IfcMappedItem.
 *
 * @param file IFC file that receives the mapped entities.
 * @param representation IfcShapeRepresentation to map.
 * @return New IfcShapeRepresentation with MappedRepresentation type.
 */
IFCAPI_BINDING express::Base geometry_map_representation(
    ifcopenshell::file* file,
    express::Base* representation);

/**
 * Unassign a representation from a product or type product.
 *
 * For IfcProduct, removes the representation from the
 * IfcProductDefinitionShape and removes an empty shape definition. For
 * IfcTypeProduct, removes the matching IfcRepresentationMap and unmaps
 * occurrences. Shape aspects referencing the representation are also removed.
 *
 * @param file IFC file to modify.
 * @param product IfcProduct or IfcTypeProduct entity.
 * @param representation IfcShapeRepresentation to unassign.
 */
IFCAPI_BINDING void geometry_unassign_representation(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* representation);

/**
 * Remove a representation and its unreferenced sub-entities.
 *
 * Cleans up styled items, presentation layer assignments, textures, and
 * colours. Geometric representation contexts are never deleted. Named
 * profiles are preserved when should_keep_named_profiles is true.
 *
 * @param file IFC file to modify.
 * @param representation IfcShapeRepresentation to remove.
 * @param options Whether to preserve named profiles.
 */
IFCAPI_BINDING void geometry_remove_representation(
    ifcopenshell::file* file,
    express::Base* representation,
    const GeometryRemoveRepresentationOptions& options);

/**
 * Copy a representation from one product to another.
 *
 * Copies the "Body" (or specified context) representation from the source
 * product, replaces any existing representation of the same context on the
 * target, and assigns the copy.
 *
 * @param file IFC file to modify.
 * @param options Source product, target product, and optional context identifier.
 * @return Newly created IfcShapeRepresentation, or no value when the source
 *         has no representation for the requested context identifier.
 */
IFCAPI_BINDING std::optional<express::Base> geometry_copy_representation(
    ifcopenshell::file* file,
    const GeometryCopyRepresentationOptions& options);

/**
 * Return the axis-aligned 2D bounding box extents of a profile.
 *
 * Computes the X and Y extents from the profile's parameterized attributes
 * (e.g. OverallWidth/OverallDepth for I-shaped profiles). When those values
 * are unavailable, geometry evaluation is used when available. Returns an
 * empty list if the extents cannot be determined.
 *
 * @param file IFC file containing the profile.
 * @param profile IfcProfileDef entity.
 * @return Two-element list {x_extent, y_extent} in model units, or an empty list.
 */
IFCAPI_BINDING std::vector<double> geometry_profile_extents(
    ifcopenshell::file* file,
    express::Base* profile);

/**
 * Set the ObjectPlacement of a product from a 4x4 transformation matrix.
 *
 * Creates a new IfcLocalPlacement relative to the product's parent
 * (determined from aggregation, nesting, containment, or void relationships).
 * The old placement is removed if unreferenced. When
 * should_transform_children is false, child placements are re-applied in their
 * previous world-space positions.
 *
 * @param file IFC file to modify.
 * @param options Product, matrix, SI flag, and child transform flag.
 * @return Newly created IfcLocalPlacement, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_edit_object_placement(
    ifcopenshell::file* file,
    const GeometryEditObjectPlacementOptions& options);

/**
 * Create an IfcRelConnectsElements between two elements.
 *
 * Removes any existing incompatible element connections between the two
 * elements before creating the new relationship.
 *
 * @param file IFC file that receives the relationship.
 * @param options Relating element, related element, and optional description/history.
 * @return IfcRelConnectsElements entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_connect_element(
    ifcopenshell::file* file,
    const GeometryConnectElementOptions& options);

/**
 * Remove all IfcRelConnectsElements relationships between two elements.
 *
 * Searches both ConnectedTo and ConnectedFrom inverse references on both
 * elements and removes matching relationships.
 *
 * @param file IFC file to modify.
 * @param relating_element First element.
 * @param related_element Second element.
 */
IFCAPI_BINDING void geometry_disconnect_element(
    ifcopenshell::file* file,
    express::Base* relating_element,
    express::Base* related_element);

/**
 * Create an IfcRelConnectsPathElements between two path-based elements.
 *
 * Removes incompatible existing path connections (same terminal end or same
 * pair) before creating the new relationship. Connection types are "ATSTART",
 * "ATEND", or "ATPATH".
 *
 * @param file IFC file that receives the relationship.
 * @param options Elements, connection types, and optional description/geometry/history.
 * @return IfcRelConnectsPathElements entity, or no result if creation fails.
 */
IFCAPI_BINDING express::Base geometry_connect_path(
    ifcopenshell::file* file,
    const GeometryConnectPathOptions& options);

/**
 * Remove path-based element connections.
 *
 * When element and connection_type are provided, removes all path connections
 * matching that type on the element. When relating_element and
 * related_element are provided, removes the specific connection between them.
 *
 * @param file IFC file to modify.
 * @param options Element, connection type, or relating/related element pair.
 */
IFCAPI_BINDING void geometry_disconnect_path(
    ifcopenshell::file* file,
    const GeometryDisconnectPathOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOMETRY_H */
