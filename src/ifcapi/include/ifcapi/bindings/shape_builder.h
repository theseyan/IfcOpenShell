/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_SHAPE_BUILDER_H
#define IFCAPI_BINDINGS_SHAPE_BUILDER_H

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

struct ShapeBuilderLineSegment {
    /// Zero-based point indices forming one continuous line segment.
    std::vector<std::uint32_t> line_indices;
};

struct ShapeBuilderArcSegment {
    /// Zero-based start, midpoint, and end point indices.
    std::array<std::uint32_t, 3> arc_indices;
};

using ShapeBuilderCurveSegment = std::variant<ShapeBuilderLineSegment, ShapeBuilderArcSegment>;

struct ShapeBuilderEllipsePointTrim {
    /// Explicit start and end trim points.
    std::array<std::array<double, 2>, 2> points;
};

struct ShapeBuilderEllipseCardinalTrim {
    /// Cardinal indices (0=+X, 1=+Y, 2=-X, 3=-Y).
    std::array<std::uint32_t, 2> cardinal_points;
};

using ShapeBuilderEllipseTrimValue =
    std::variant<ShapeBuilderEllipsePointTrim, ShapeBuilderEllipseCardinalTrim>;

struct ShapeBuilderEllipseTrim {
    /// Explicit point or cardinal trim mode.
    ShapeBuilderEllipseTrimValue value;
};

struct ShapeBuilderMepProfileHalfDimensions {
    double half_x = 0.0;
    double half_y = 0.0;
    double depth = 0.0;
};

struct ShapeBuilderMepOffset {
    double x = 0.0;
    double y = 0.0;
};

struct ShapeBuilderMepBendDirection {
    double x = 0.0;
    double y = 0.0;
};

struct ShapeBuilderMepTransitionFromLength {
    double length = 0.0;
};

struct ShapeBuilderMepTransitionFromAngle {
    double angle = 0.0;
};

using ShapeBuilderMepTransitionCalculation =
    std::variant<ShapeBuilderMepTransitionFromLength, ShapeBuilderMepTransitionFromAngle>;

/**
 * Result of an MEP transition shape computation.
 */
struct ShapeBuilderMepTransitionShapeResult {
    /// IfcShapeRepresentation containing the transition geometry.
    express::Base representation;
    /// Length of the start segment in model units.
    double start_length = 0.0;
    /// Length of the end segment in model units.
    double end_length = 0.0;
    /// Transition angle in degrees.
    double angle = 0.0;
    /// XY offset between the two profile centres.
    std::vector<double> profile_offset;
    /// Computed transition length in model units.
    double transition_length = 0.0;
    /// Total length (start + transition + end) in model units.
    double full_transition_length = 0.0;
};

/**
 * Result of an MEP bend shape computation.
 */
struct ShapeBuilderMepBendShapeResult {
    /// IfcShapeRepresentation containing the bend geometry.
    express::Base representation;
    /// Length of the start straight section in model units.
    double start_length = 0.0;
    /// Length of the end straight section in model units.
    double end_length = 0.0;
    /// Bend radius in model units.
    double radius = 0.0;
    /// Bend angle in degrees.
    double angle = 0.0;
    /// Index of the lateral axis (0 = X, 1 = Y).
    int lateral_axis = 0;
    /// Sign of the lateral direction (-1.0 or 1.0).
    double lateral_sign = 0.0;
    /// Sign of the Z axis direction (-1 or 1).
    int z_axis_sign = 1;
    /// Dimension of the main profile along the lateral axis.
    double main_profile_dimension = 0.0;
};

/**
 * Options for creating a 2D/3D polyline or indexed polycurve.
 */
struct ShapeBuilderPolylineOptions {
    /// Ordered coordinate points (each XY or XYZ).
    std::variant<std::vector<std::array<double, 2>>, std::vector<std::array<double, 3>>> points;
    /// Optional offset added to every point before storage.
    std::optional<std::variant<std::array<double, 2>, std::array<double, 3>>> position_offset;
    /// Explicit line and arc segments. Omit for an open, unsegmented polyline.
    std::optional<std::vector<ShapeBuilderCurveSegment>> segments;
};

struct ShapeBuilderIndexedPolycurve2dOptions {
    /// Ordered XY coordinate points.
    std::vector<std::array<double, 2>> points;
    /// Explicit line and arc segments using zero-based indices.
    std::vector<ShapeBuilderCurveSegment> segments;
};

/**
 * Options for extruding a profile or curve into a swept solid.
 */
struct ShapeBuilderExtrudeOptions {
    /// IfcProfileDef or IfcCurve to extrude. Curves are wrapped in an IfcArbitraryClosedProfileDef.
    express::Base profile_or_curve;
    /// Extrusion depth in model units. Must be greater than zero.
    std::optional<double> magnitude = 1.0;
    /// XYZ position of the solid's local origin.
    std::optional<std::array<double, 3>> position = std::array<double, 3>{0.0, 0.0, 0.0};
    /// Direction ratios for the extrusion direction.
    std::optional<std::array<double, 3>> extrusion_vector = std::array<double, 3>{0.0, 0.0, 1.0};
    /// Z axis direction ratios for the solid's placement.
    std::optional<std::array<double, 3>> position_z_axis = std::array<double, 3>{0.0, 0.0, 1.0};
    /// X axis direction ratios for the solid's placement.
    std::optional<std::array<double, 3>> position_x_axis = std::array<double, 3>{1.0, 0.0, 0.0};
    /// Optional Y axis; when provided, the Z axis is computed as cross(X, Y).
    std::optional<std::array<double, 3>> position_y_axis;
};

/**
 * Options for creating an IfcAxis2Placement2D.
 */
struct ShapeBuilderAxis2Placement2dOptions {
    /// XY coordinates of the placement origin.
    std::optional<std::array<double, 2>> position = std::array<double, 2>{0.0, 0.0};
    /// Optional reference direction. Defaults to (1, 0) when omitted.
    std::optional<std::array<double, 2>> x_direction;
};

/**
 * Options for creating an IfcAxis2Placement3D.
 */
struct ShapeBuilderAxis2Placement3dOptions {
    /// XYZ coordinates of the placement origin.
    std::optional<std::array<double, 3>> position = std::array<double, 3>{0.0, 0.0, 0.0};
    /// Direction ratios for the Z axis.
    std::optional<std::array<double, 3>> z_axis = std::array<double, 3>{0.0, 0.0, 1.0};
    /// Direction ratios for the X axis (Y is derived).
    std::optional<std::array<double, 3>> x_axis = std::array<double, 3>{1.0, 0.0, 0.0};
};

/**
 * Options for creating an IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
 */
struct ShapeBuilderProfileOptions {
    /// Outer boundary curve (IfcCurve entity).
    express::Base outer_curve;
    /// Optional profile name.
    std::optional<std::string> name;
    /// Inner void curves. If non-empty, creates IfcArbitraryProfileDefWithVoids.
    std::optional<std::vector<express::Base>> inner_curves = std::vector<express::Base>{};
    /// Profile type string (e.g. "AREA"). Defaults to "AREA".
    std::optional<std::string> profile_type = std::string("AREA");
};

/**
 * Options for creating an IfcShapeRepresentation or IfcTopologyRepresentation.
 */
struct ShapeBuilderRepresentationOptions {
    /// IfcGeometricRepresentationContext for the representation.
    express::Base context;
    /// Representation items (geometric or topological entities).
    std::vector<express::Base> items;
    /// Representation type (e.g. "Tessellation", "Curve2D"). Auto-detected if omitted.
    std::optional<std::string> representation_type;
};

/**
 * Options for creating an IfcEllipse, optionally trimmed to an IfcTrimmedCurve.
 */
struct ShapeBuilderEllipseCurveOptions {
    /// First semi-axis radius (X direction).
    double x_axis_radius = 0.0;
    /// Second semi-axis radius (Y direction).
    double y_axis_radius = 0.0;
    /// XY centre of the ellipse.
    std::optional<std::array<double, 2>> position = std::array<double, 2>{0.0, 0.0};
    /// Optional reference X direction for the ellipse placement.
    std::optional<std::array<double, 2>> ref_x_direction = std::array<double, 2>{1.0, 0.0};
    /// Optional explicit point or cardinal trim mode.
    std::optional<ShapeBuilderEllipseTrim> trim;
};

/**
 * Options for translating a geometry item.
 */
struct ShapeBuilderTranslateOptions {
    /// Geometry item to translate (polyline, circle, extruded solid, or shape representation).
    express::Base item;
    /// Translation (XY or XYZ).
    std::variant<std::array<double, 2>, std::array<double, 3>> translation;
    /// If true, translate an independent copy instead of the supplied item. Defaults to false.
    std::optional<bool> create_copy = false;
};

/**
 * Options for rotating a geometry item in 2D.
 */
struct ShapeBuilderRotateOptions {
    /// Geometry item to rotate (polyline, circle, or extruded solid).
    express::Base item;
    /// Rotation angle in degrees.
    std::optional<double> angle = 90.0;
    /// XY pivot point for the rotation.
    std::optional<std::array<double, 2>> pivot_point = std::array<double, 2>{0.0, 0.0};
    /// If true, rotate counter-clockwise; if false, clockwise. Defaults to false.
    std::optional<bool> counter_clockwise = false;
    /// If true, rotate an independent copy instead of the supplied item. Defaults to false.
    std::optional<bool> create_copy = false;
};

/**
 * Options for mirroring a geometry item across 2D axes.
 */
struct ShapeBuilderMirrorOptions {
    /// Geometry item to mirror (polyline, circle, ellipse, trimmed curve, or extruded solid).
    express::Base item;
    /// Mirror axes: non-zero values flip the corresponding axis (index 0 = X, index 1 = Y).
    std::optional<std::array<double, 2>> mirror_axes = std::array<double, 2>{1.0, 1.0};
    /// XY point through which the mirror plane passes.
    std::optional<std::array<double, 2>> mirror_point = std::array<double, 2>{0.0, 0.0};
    /// If true, mirror an independent copy instead of the supplied item. Defaults to false.
    std::optional<bool> create_copy = false;
    /// Optional 3x3 or 4x4 placement matrix for local-space mirroring.
    std::optional<std::variant<std::array<double, 9>, std::array<double, 16>>> placement_matrix;
};

/**
 * Options for computing an MEP transition angle or length.
 */
struct ShapeBuilderMepTransitionCalculateOptions {
    /// Half-dimensions of the start profile (X, Y).
    ShapeBuilderMepProfileHalfDimensions start_half_dim;
    /// Half-dimensions of the end profile (X, Y).
    ShapeBuilderMepProfileHalfDimensions end_half_dim;
    /// XY offset between the two profile centres.
    ShapeBuilderMepOffset offset;
    /// Optional override for the absolute XY difference of half-dimensions.
    std::optional<ShapeBuilderMepOffset> diff;
    /// If true, swap X/Y axes for the end profile. Defaults to false.
    std::optional<bool> end_profile = false;
    /// Known transition length or angle used to solve for the other value.
    ShapeBuilderMepTransitionCalculation calculation;
};

/**
 * Options for creating MEP transition shape geometry between two segments.
 */
struct ShapeBuilderMepTransitionShapeOptions {
    /// Start MEP segment (IfcFlowSegment with material profile).
    express::Base start_segment;
    /// End MEP segment (IfcFlowSegment with material profile).
    express::Base end_segment;
    /// Length of the start straight section in model units.
    double start_length = 0.0;
    /// Length of the end straight section in model units.
    double end_length = 0.0;
    /// Transition angle in degrees.
    std::optional<double> angle = 30.0;
    /// XY offset between the two profile centres.
    std::optional<ShapeBuilderMepOffset> profile_offset = ShapeBuilderMepOffset{};
};

/**
 * Options for creating an IfcSphere.
 */
struct ShapeBuilderSphereOptions {
    /// Sphere radius in model units. Defaults to 1.0.
    std::optional<double> radius = 1.0;
    /// XYZ centre of the sphere.
    std::optional<std::array<double, 3>> center = std::array<double, 3>{0.0, 0.0, 0.0};
};

/**
 * Options for creating an IfcBlock.
 */
struct ShapeBuilderBlockOptions {
    /// XYZ position of the block's local origin (corner from which X/Y/Z lengths extend).
    std::optional<std::array<double, 3>> position = std::array<double, 3>{0.0, 0.0, 0.0};
    /// Block length along X in model units. Defaults to 1.0.
    std::optional<double> x_length = 1.0;
    /// Block length along Y in model units. Defaults to 1.0.
    std::optional<double> y_length = 1.0;
    /// Block length along Z in model units. Defaults to 1.0.
    std::optional<double> z_length = 1.0;
};

/**
 * Options for creating an IfcHalfSpaceSolid.
 */
struct ShapeBuilderHalfSpaceSolidOptions {
    /// IfcPlane defining the bounding surface.
    express::Base plane;
    /// If true, the half-space is on the side of the plane normal.
    std::optional<bool> agreement_flag = false;
};

/**
 * Options for computing the required length of an MEP transition.
 */
struct ShapeBuilderMepTransitionLengthOptions {
    /// Half-dimensions of the start profile (X, Y).
    ShapeBuilderMepProfileHalfDimensions start_half_dim;
    /// Half-dimensions of the end profile (X, Y).
    ShapeBuilderMepProfileHalfDimensions end_half_dim;
    /// Transition angle in degrees.
    double angle = 0.0;
    /// XY offset between the two profile centres.
    std::optional<ShapeBuilderMepOffset> profile_offset = ShapeBuilderMepOffset{};
};

/**
 * Options for creating MEP bend shape geometry.
 */
struct ShapeBuilderMepBendShapeOptions {
    /// MEP segment entity (must have a single material profile).
    express::Base segment;
    /// Length of the start straight section in model units.
    double start_length = 0.0;
    /// Length of the end straight section in model units.
    double end_length = 0.0;
    /// Bend angle in radians.
    double angle = 0.0;
    /// Bend radius in model units.
    double radius = 0.0;
    /// XY direction indicating the bend plane.
    ShapeBuilderMepBendDirection bend_vector;
    /// If true, flip the Z axis direction. Defaults to false.
    bool flip_z_axis = false;
};

/**
 * Create a mesh representation using IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3).
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Face index lists (zero-based).
 * @return IfcPolygonalFaceSet or IfcFacetedBrep entity.
 */
IFCAPI_BINDING express::Base shape_builder_mesh(
    ifcopenshell::file* file,
    const std::vector<std::array<double, 3>>& points,
    const std::vector<std::vector<int>>& faces);

/**
 * Create an IfcFacetedBrep from vertices and face index lists.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Face index lists (zero-based). Each face is a single outer loop.
 * @return IfcFacetedBrep entity with an IfcClosedShell.
 */
IFCAPI_BINDING express::Base shape_builder_faceted_brep(
    ifcopenshell::file* file,
    const std::vector<std::array<double, 3>>& points,
    const std::vector<std::vector<int>>& faces);

/**
 * Create an IfcTriangulatedFaceSet from vertices and triangle index lists.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Triangle index lists (zero-based, truncated to 3 vertices each).
 * @return IfcTriangulatedFaceSet entity.
 */
IFCAPI_BINDING express::Base shape_builder_triangulated_face_set(
    ifcopenshell::file* file,
    const std::vector<std::array<double, 3>>& points,
    const std::vector<std::array<int, 3>>& faces);

/**
 * Create an IfcPolygonalFaceSet from vertices and polygonal face loops.
 *
 * Each face may contain one outer loop and optional inner loops (voids).
 * Faces with multiple loops create IfcIndexedPolygonalFaceWithVoids entities.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Per-face loop index lists (zero-based).
 * @return IfcPolygonalFaceSet entity.
 */
IFCAPI_BINDING express::Base shape_builder_polygonal_face_set(
    ifcopenshell::file* file,
    const std::vector<std::array<double, 3>>& points,
    const std::vector<std::vector<std::vector<int>>>& faces);

/**
 * Create an IfcVertexPoint at the given position.
 *
 * @param file IFC file that receives the geometry.
 * @param position XYZ coordinates of the vertex.
 * @return IfcVertexPoint entity.
 */
IFCAPI_BINDING express::Base shape_builder_vertex(
    ifcopenshell::file* file,
    const std::array<double, 3>& position);

/**
 * Create an IfcEdge between two vertex positions.
 *
 * @param file IFC file that receives the geometry.
 * @param start XYZ coordinates of the edge start.
 * @param end XYZ coordinates of the edge end.
 * @return IfcEdge entity.
 */
IFCAPI_BINDING express::Base shape_builder_edge(
    ifcopenshell::file* file,
    const std::array<double, 3>& start,
    const std::array<double, 3>& end);

/**
 * Create an IfcFace from an ordered list of polygon vertices.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ coordinates defining the face outer boundary.
 * @return IfcFace entity with an IfcFaceOuterBound.
 */
IFCAPI_BINDING express::Base shape_builder_face(
    ifcopenshell::file* file,
    const std::vector<std::array<double, 3>>& points);

/**
 * Create a 2D or 3D polyline (IfcPolyline for IFC2X3, IfcIndexedPolyCurve otherwise).
 *
 * @param file IFC file that receives the geometry.
 * @param options Points, optional offset, and explicit semantic segments.
 * @return IfcPolyline or IfcIndexedPolyCurve entity.
 */
IFCAPI_BINDING express::Base shape_builder_polyline(
    ifcopenshell::file* file,
    const ShapeBuilderPolylineOptions& options);

/**
 * Create an IfcAxis2Placement3D from position and axes.
 *
 * @param file IFC file that receives the placement.
 * @param options Position, Z axis, and X axis direction ratios.
 * @return IfcAxis2Placement3D entity.
 */
IFCAPI_BINDING express::Base shape_builder_axis2_placement_3d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement3dOptions& options);

/**
 * Create an IfcAxis2Placement2D from position and optional reference direction.
 *
 * @param file IFC file that receives the placement.
 * @param options Position and optional X direction.
 * @return IfcAxis2Placement2D entity.
 */
IFCAPI_BINDING express::Base shape_builder_axis2_placement_2d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement2dOptions& options);

/**
 * Create an IfcCircle centred at the given position.
 *
 * @param file IFC file that receives the geometry.
 * @param center XY coordinates of the circle centre.
 * @param radius Circle radius in model units.
 * @return IfcCircle entity.
 */
IFCAPI_BINDING express::Base shape_builder_circle(
    ifcopenshell::file* file,
    const std::array<double, 2>& center,
    double radius);

/**
 * Create an IfcPlane from a point and normal direction.
 *
 * The reference direction is computed automatically from the normal.
 *
 * @param file IFC file that receives the geometry.
 * @param location XYZ point on the plane.
 * @param normal Direction ratios of the plane normal.
 * @return IfcPlane entity.
 */
IFCAPI_BINDING express::Base shape_builder_plane(
    ifcopenshell::file* file,
    const std::array<double, 3>& location,
    const std::array<double, 3>& normal);

/**
 * Create an IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve, optional inner curves, name, and profile type.
 * @return IfcProfileDef entity.
 */
IFCAPI_BINDING express::Base shape_builder_profile(
    ifcopenshell::file* file,
    const ShapeBuilderProfileOptions& options);

/**
 * Create an IfcSphere.
 *
 * @param file IFC file that receives the geometry.
 * @param options Radius and centre position.
 * @return IfcSphere entity.
 */
IFCAPI_BINDING express::Base shape_builder_sphere(
    ifcopenshell::file* file,
    const ShapeBuilderSphereOptions& options);

/**
 * Create an IfcBlock (rectangular parallelepiped).
 *
 * @param file IFC file that receives the geometry.
 * @param options Position and XYZ dimensions.
 * @return IfcBlock entity.
 */
IFCAPI_BINDING express::Base shape_builder_block(
    ifcopenshell::file* file,
    const ShapeBuilderBlockOptions& options);

/**
 * Create an IfcHalfSpaceSolid bounded by a plane.
 *
 * @param file IFC file that receives the geometry.
 * @param options Bounding plane and agreement flag.
 * @return IfcHalfSpaceSolid entity.
 */
IFCAPI_BINDING express::Base shape_builder_half_space_solid(
    ifcopenshell::file* file,
    const ShapeBuilderHalfSpaceSolidOptions& options);

/**
 * Create an IfcExtrudedAreaSolid by extruding a profile along a direction.
 *
 * If the profile_or_curve is a bare IfcCurve (not an IfcProfileDef), it is
 * wrapped in an IfcArbitraryClosedProfileDef automatically.
 *
 * @param file IFC file that receives the geometry.
 * @param options Profile, depth, position, and extrusion direction.
 * @return IfcExtrudedAreaSolid entity.
 */
IFCAPI_BINDING express::Base shape_builder_extrude(
    ifcopenshell::file* file,
    const ShapeBuilderExtrudeOptions& options);

/**
 * Create an IfcSweptDiskSolid along a directrix curve.
 *
 * @param file IFC file that receives the geometry.
 * @param path_curve IfcCurve entity defining the sweep path.
 * @param radius Disk radius in model units.
 * @return IfcSweptDiskSolid entity.
 */
IFCAPI_BINDING express::Base shape_builder_swept_disk_solid(
    ifcopenshell::file* file,
    express::Base* path_curve,
    double radius);

/**
 * Create an IfcShapeRepresentation or IfcTopologyRepresentation.
 *
 * The representation type is auto-detected from the items when omitted.
 * Topology types (Vertex, Edge, Path, Face, Shell) produce IfcTopologyRepresentation;
 * all others produce IfcShapeRepresentation.
 *
 * @param file IFC file that receives the representation.
 * @param options Context, items, and optional representation type.
 * @return IfcShapeRepresentation or IfcTopologyRepresentation entity.
 */
IFCAPI_BINDING express::Base shape_builder_representation(
    ifcopenshell::file* file,
    const ShapeBuilderRepresentationOptions& options);

/**
 * Create an independent copy of an IFC entity and the entities it references.
 *
 * New GlobalId attributes are generated for the copied entities.
 *
 * @param file IFC file that receives the copy.
 * @param element Entity to copy.
 * @return Root entity of the copied subgraph.
 */
IFCAPI_BINDING express::Base shape_builder_deep_copy(
    ifcopenshell::file* file,
    express::Base* element);

/**
 * Create a 2D IfcIndexedPolyCurve arc between two points.
 *
 * Inserts a midpoint slightly offset along the dominant axis to form a
 * three-point arc segment.
 *
 * @param file IFC file that receives the geometry.
 * @param points Two XY endpoints.
 * @return IfcIndexedPolyCurve entity with one arc segment.
 */
IFCAPI_BINDING express::Base shape_builder_curve_between_two_points(
    ifcopenshell::file* file,
    const std::array<std::array<double, 2>, 2>& points);

/**
 * Create an IfcEllipse, optionally trimmed to an IfcTrimmedCurve.
 *
 * @param file IFC file that receives the geometry.
 * @param options Radii, position, trim points, and reference direction.
 * @return IfcEllipse or IfcTrimmedCurve entity.
 */
IFCAPI_BINDING express::Base shape_builder_ellipse_curve(
    ifcopenshell::file* file,
    const ShapeBuilderEllipseCurveOptions& options);

/**
 * Create a 2D IfcIndexedPolyCurve from explicit points and segment indices.
 *
 * @param file IFC file that receives the geometry.
 * @param options XY points and explicit zero-based line or arc segments.
 * @return IfcIndexedPolyCurve entity.
 */
IFCAPI_BINDING express::Base shape_builder_indexed_polycurve_2d(
    ifcopenshell::file* file,
    const ShapeBuilderIndexedPolycurve2dOptions& options);

/**
 * Translate a geometry item by a direction and distance.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
 * IfcExtrudedAreaSolid, IfcTessellatedFaceSet, IfcShapeRepresentation,
 * and IfcTrimmedCurve.
 *
 * @param file IFC file containing the item.
 * @param options Item, translation, and copy flag.
 * @return The translated item, either the supplied entity or an independent copy.
 */
IFCAPI_BINDING express::Base shape_builder_translate(
    ifcopenshell::file* file,
    const ShapeBuilderTranslateOptions& options);

/**
 * Rotate a geometry item around a 2D pivot point.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, and
 * IfcExtrudedAreaSolid. The angle is in degrees.
 *
 * @param file IFC file containing the item.
 * @param options Item, angle, pivot, direction, and copy flag.
 * @return The rotated item, either the supplied entity or an independent copy.
 */
IFCAPI_BINDING express::Base shape_builder_rotate(
    ifcopenshell::file* file,
    const ShapeBuilderRotateOptions& options);

/**
 * Mirror a geometry item across 2D axes.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
 * IfcTrimmedCurve, and IfcExtrudedAreaSolid. A non-zero value in
 * mirror_axes at index 0 or 1 flips the corresponding coordinate.
 *
 * @param file IFC file containing the item.
 * @param options Item, axes, point, copy flag, and optional placement matrix.
 * @return The mirrored item, either the supplied entity or an independent copy.
 */
IFCAPI_BINDING express::Base shape_builder_mirror(
    ifcopenshell::file* file,
    const ShapeBuilderMirrorOptions& options);

/**
 * Read the coordinate list from an IfcPolyline or IfcIndexedPolyCurve.
 *
 * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
 * @return Ordered XY or XYZ coordinate sequences.
 */
IFCAPI_BINDING std::vector<std::vector<double>> shape_builder_get_polyline_coords(
    express::Base* polyline);

/**
 * Replace the coordinate list of an IfcPolyline or IfcIndexedPolyCurve.
 *
 * For IfcPolyline, the new coordinate count must match the existing point count.
 *
 * @param file IFC file containing the polyline.
 * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
 * @param coords Replacement coordinates.
 * @return The modified polyline entity.
 */
IFCAPI_BINDING express::Base shape_builder_set_polyline_coords(
    ifcopenshell::file* file,
    express::Base* polyline,
    const std::variant<std::vector<std::array<double, 2>>, std::vector<std::array<double, 3>>>& coords);

/**
 * Compute the transition angle or length between two MEP duct profiles.
 *
 * Provide either length or angle (not both) to solve for the other.
 * Returns NaN when no valid solution exists.
 *
 * @param options Profile half-dimensions, offset, and one of length or angle.
 * @return The solved angle (degrees) or length (model units), or NaN.
 */
IFCAPI_BINDING double shape_builder_mep_transition_calculate(
    const ShapeBuilderMepTransitionCalculateOptions& options);

/**
 * Compute the minimum transition length between two MEP duct profiles at a given angle.
 *
 * @param options Profile half-dimensions, angle in degrees, and profile offset.
 * @return Required transition length in model units, or NaN if no valid solution.
 */
IFCAPI_BINDING double shape_builder_mep_transition_length(
    const ShapeBuilderMepTransitionLengthOptions& options);

/**
 * Build MEP transition geometry between two duct segments.
 *
 * Generates start/end extrusions and a connecting transition mesh.
 * Returns no result when the segments lack material profiles or the
 * transition cannot be computed.
 *
 * @param file IFC file that receives the geometry.
 * @param options Start/end segments, lengths, angle, and profile offset.
 * @return Transition result with representation and dimensions, or no result.
 */
IFCAPI_BINDING std::optional<ShapeBuilderMepTransitionShapeResult> shape_builder_mep_transition_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepTransitionShapeOptions& options);

/**
 * Build MEP bend geometry for a duct segment.
 *
 * Generates a swept disk solid (for circular profiles) or an extruded bend
 * shape (for rectangular profiles), plus optional start and end straight
 * extensions.
 *
 * @param file IFC file that receives the geometry.
 * @param options Segment, lengths, angle, radius, bend direction, and Z flip.
 * @return Bend result with representation and computed parameters.
 */
IFCAPI_BINDING ShapeBuilderMepBendShapeResult shape_builder_mep_bend_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepBendShapeOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SHAPE_BUILDER_H */
