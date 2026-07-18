// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

template <typename T, std::size_t N>
std::vector<T> dynamic_array(const std::array<T, N>& values) {
    return {values.begin(), values.end()};
}

template <typename T, std::size_t N>
std::vector<std::vector<T>> dynamic_arrays(const std::vector<std::array<T, N>>& values) {
    std::vector<std::vector<T>> result;
    result.reserve(values.size());
    for (const auto& value : values) result.push_back(dynamic_array(value));
    return result;
}

std::vector<double> dynamic_vector(const ifcapi::bindings::Vec2OrVec3& value) {
    return std::visit([](const auto& item) { return dynamic_array(item); }, value);
}

std::vector<std::vector<double>> dynamic_vectors(const ifcapi::bindings::Vec2OrVec3List& value) {
    return std::visit([](const auto& items) { return dynamic_arrays(items); }, value);
}

std::vector<double> dynamic_matrix(const ifcapi::bindings::Mat3OrMat4& value) {
    return std::visit([](const auto& item) { return dynamic_array(item); }, value);
}

express::Base create_entity(ifcopenshell::file* file, const std::string& name) {
    if (!file) {
        throw std::invalid_argument("missing file");
    }
    const auto* declaration = file->schema()->declaration_by_name(name);
    if (declaration == nullptr) {
        throw std::invalid_argument("schema does not contain " + name);
    }
    return file->create(declaration);
}

int attr_index(express::Base entity, const std::string& name) {
    auto declaration = entity ? entity.declaration().as_entity() : nullptr;
    if (!declaration) {
        return -1;
    }
    ptrdiff_t index = declaration->attribute_index(name.c_str());
    return index < 0 ? -1 : static_cast<int>(index);
}

template <typename T>
void set_attr(express::Base entity, const std::string& name, const T& value) {
    int index = attr_index(entity, name);
    if (index < 0) {
        throw std::invalid_argument(entity.declaration().name() + " has no attribute " + name);
    }
    entity.set_attribute_value(static_cast<size_t>(index), value);
}

void set_ref(express::Base entity, const std::string& name, express::Base value) {
    int index = attr_index(entity, name);
    if (index < 0) {
        throw std::invalid_argument(entity.declaration().name() + " has no attribute " + name);
    }
    if (value) {
        entity.set_attribute_value(static_cast<size_t>(index), value);
    } else {
        entity.set_attribute_value(static_cast<size_t>(index), blank{});
    }
}

void set_refs(express::Base entity, const std::string& name, const std::vector<express::Base>& values)
{
    set_attr(entity, name, values);
}

std::vector<int> one_based(const std::vector<int>& values) {
    std::vector<int> result;
    result.reserve(values.size());
    for (int value : values) {
        result.push_back(value + 1);
    }
    return result;
}

std::vector<std::vector<int>> one_based(const std::vector<std::vector<int>>& values) {
    std::vector<std::vector<int>> result;
    result.reserve(values.size());
    for (const auto& row : values) {
        result.push_back(one_based(row));
    }
    return result;
}

std::vector<double> add_vectors(const std::vector<double>& a, const std::vector<double>& b) {
    std::vector<double> result = a;
    const size_t n = std::min(result.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        result[i] += b[i];
    }
    return result;
}

std::vector<double> scale_vector(const std::vector<double>& v, double scale) {
    std::vector<double> result = v;
    for (double& value : result) {
        value *= scale;
    }
    return result;
}

std::vector<double> cross3(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() < 3 || b.size() < 3) {
        return {0.0, 0.0, 0.0};
    }
    return {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    };
}

std::vector<double> normalized(const std::vector<double>& v) {
    double length_sq = 0.0;
    for (double value : v) {
        length_sq += value * value;
    }
    if (length_sq == 0.0) {
        return v;
    }
    return scale_vector(v, 1.0 / std::sqrt(length_sq));
}

express::Base cartesian_point(ifcopenshell::file* file, const std::vector<double>& coordinates) {
    auto point = create_entity(file, "IfcCartesianPoint");
    set_attr(point, "Coordinates", coordinates);
    return point;
}

express::Base direction(ifcopenshell::file* file, const std::vector<double>& ratios) {
    auto result = create_entity(file, "IfcDirection");
    set_attr(result, "DirectionRatios", ratios);
    return result;
}

express::Base line_index(ifcopenshell::file* file, const std::vector<int>& indices) {
    auto result = create_entity(file, "IfcLineIndex");
    result.set_attribute_value(0, indices);
    return result;
}

express::Base arc_index(ifcopenshell::file* file, const std::vector<int>& indices) {
    auto result = create_entity(file, "IfcArcIndex");
    result.set_attribute_value(0, indices);
    return result;
}

express::Base axis2_placement_3d(
    ifcopenshell::file* file,
    const std::vector<double>& position,
    const std::vector<double>& z_axis,
    const std::vector<double>& x_axis)
{
    auto result = create_entity(file, "IfcAxis2Placement3D");
    set_ref(result, "Location", cartesian_point(file, position));
    set_ref(result, "Axis", direction(file, z_axis));
    set_ref(result, "RefDirection", direction(file, x_axis));
    return result;
}

express::Base axis2_placement_2d(
    ifcopenshell::file* file,
    const std::vector<double>& position,
    const std::vector<double>& x_direction,
    bool has_x_direction)
{
    auto result = create_entity(file, "IfcAxis2Placement2D");
    set_ref(result, "Location", cartesian_point(file, position));
    set_ref(result, "RefDirection", has_x_direction ? direction(file, x_direction) : express::Base());
    return result;
}

express::Base vertex_point(ifcopenshell::file* file, const std::vector<double>& position) {
    auto vertex = create_entity(file, "IfcVertexPoint");
    set_ref(vertex, "VertexGeometry", cartesian_point(file, position));
    return vertex;
}

express::Base face_impl(ifcopenshell::file* file, const std::vector<std::vector<double>>& points) {
    std::vector<express::Base> vertices;
    vertices.reserve(points.size());
    for (const auto& point : points) {
        vertices.push_back(cartesian_point(file, point));
    }
    auto loop = create_entity(file, "IfcPolyLoop");
    set_refs(loop, "Polygon", vertices);
    auto bound = create_entity(file, "IfcFaceOuterBound");
    set_ref(bound, "Bound", loop);
    set_attr(bound, "Orientation", true);
    auto face = create_entity(file, "IfcFace");
    set_refs(face, "Bounds", {bound});
    return face;
}

express::Base polyline_impl(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& input_points,
    bool closed,
    const std::vector<double>& position_offset,
    bool has_position_offset,
    const std::vector<int>& arc_points)
{
    if (!arc_points.empty() && file->schema()->name() == "IFC2X3") {
        throw std::invalid_argument("Arcs are not supported for IFC2X3.");
    }

    std::vector<std::vector<double>> points;
    points.reserve(input_points.size());
    for (const auto& point : input_points) {
        points.push_back(has_position_offset ? add_vectors(point, position_offset) : point);
    }

    if (file->schema()->name() == "IFC2X3") {
        std::vector<express::Base> ifc_points;
        ifc_points.reserve(points.size() + (closed && !points.empty() ? 1 : 0));
        for (const auto& point : points) {
            ifc_points.push_back(cartesian_point(file, point));
        }
        if (closed && !ifc_points.empty()) {
            ifc_points.push_back(ifc_points.front());
        }
        auto result = create_entity(file, "IfcPolyline");
        set_refs(result, "Points", ifc_points);
        return result;
    }

    if (points.empty()) {
        throw std::invalid_argument("polyline requires at least one point");
    }
    const size_t dimensions = points.front().size();
    auto ifc_points = create_entity(file, dimensions == 2 ? "IfcCartesianPointList2D" : "IfcCartesianPointList3D");
    if (dimensions != 2 && dimensions != 3) {
        throw std::invalid_argument("Point has unexpected number of dimensions - " + std::to_string(dimensions) + ".");
    }
    set_attr(ifc_points, "CoordList", points);

    auto result = create_entity(file, "IfcIndexedPolyCurve");
    set_ref(result, "Points", ifc_points);
    if (!closed && arc_points.empty()) {
        return result;
    }

    std::vector<std::vector<int>> segments;
    int cur_i = 0;
    bool closed_by_arc = false;
    auto is_arc = [&](int index) {
        return std::find(arc_points.begin(), arc_points.end(), index) != arc_points.end();
    };
    while (cur_i < static_cast<int>(points.size()) - 1) {
        const int cur_i_ifc = cur_i + 1;
        if (is_arc(cur_i + 1)) {
            if (cur_i_ifc + 1 < static_cast<int>(points.size())) {
                segments.push_back({cur_i_ifc, cur_i_ifc + 1, cur_i_ifc + 2});
            } else {
                segments.push_back({cur_i_ifc, cur_i_ifc + 1, 1});
                closed_by_arc = true;
            }
            cur_i += 2;
        } else {
            segments.push_back({cur_i_ifc, cur_i_ifc + 1});
            cur_i += 1;
        }
    }
    if (closed && !closed_by_arc) {
        segments.push_back({static_cast<int>(points.size()), 1});
    }

    std::vector<express::Base> ifc_segments;
    std::vector<int> current_line_segment;
    const size_t last_segment = segments.empty() ? 0 : segments.size() - 1;
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        if (segment.size() == 2) {
            if (current_line_segment.empty()) {
                current_line_segment = segment;
            } else {
                current_line_segment.push_back(segment[1]);
            }
        }
        if (!current_line_segment.empty() && (segment.size() == 3 || i == last_segment)) {
            ifc_segments.push_back(line_index(file, current_line_segment));
            current_line_segment.clear();
        }
        if (segment.size() == 3) {
            ifc_segments.push_back(arc_index(file, segment));
        }
    }
    set_refs(result, "Segments", ifc_segments);
    return result;
}

express::Base indexed_polycurve_2d_impl(
    ifcopenshell::file* file,
    const ifcapi::bindings::ShapeBuilderIndexedPolycurve2dOptions& options)
{
    const auto& points = options.points;
    if (points.empty()) {
        throw std::invalid_argument("indexed polycurve requires at least one point");
    }
    std::vector<std::vector<double>> coordinates;
    coordinates.reserve(points.size());
    for (const auto& point : points) {
        if (!std::isfinite(point[0]) || !std::isfinite(point[1])) {
            throw std::invalid_argument("indexed polycurve points must be finite");
        }
        coordinates.push_back({point[0], point[1]});
    }
    std::vector<std::pair<bool, std::vector<int>>> segments;
    segments.reserve(options.segments.size());
    for (const auto& segment : options.segments) {
        std::visit(
            [&](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                std::vector<std::uint32_t> indices;
                bool is_arc = false;
                if constexpr (std::is_same_v<T, ifcapi::bindings::ShapeBuilderLineSegment>) {
                    if (value.line_indices.size() < 2) {
                        throw std::invalid_argument("line segment requires at least two point indices");
                    }
                    indices = value.line_indices;
                } else {
                    indices.assign(value.arc_indices.begin(), value.arc_indices.end());
                    is_arc = true;
                }
                std::vector<int> converted;
                converted.reserve(indices.size());
                for (std::uint32_t index : indices) {
                    if (index >= points.size()) {
                        throw std::out_of_range("curve segment point index out of range");
                    }
                    converted.push_back(static_cast<int>(index) + 1);
                }
                segments.emplace_back(is_arc, std::move(converted));
            },
            segment);
    }
    auto point_list = create_entity(file, "IfcCartesianPointList2D");
    set_attr(point_list, "CoordList", coordinates);
    std::vector<express::Base> ifc_segments;
    ifc_segments.reserve(segments.size());
    for (const auto& segment : segments) {
        ifc_segments.push_back(segment.first ? arc_index(file, segment.second) : line_index(file, segment.second));
    }
    auto result = create_entity(file, "IfcIndexedPolyCurve");
    set_ref(result, "Points", point_list);
    set_refs(result, "Segments", ifc_segments);
    return result;
}

express::Base semantic_polyline_impl(
    ifcopenshell::file* file,
    const ifcapi::bindings::ShapeBuilderPolylineOptions& options)
{
    const auto input_points = dynamic_vectors(options.points);
    const auto position_offset = options.position_offset
        ? std::optional<std::vector<double>>(dynamic_vector(*options.position_offset))
        : std::nullopt;
    if (input_points.empty()) {
        throw std::invalid_argument("polyline requires at least one point");
    }
    const size_t dimensions = input_points.front().size();
    if (dimensions != 2 && dimensions != 3) {
        throw std::invalid_argument("polyline points must be XY or XYZ");
    }
    for (const auto& point : input_points) {
        if (point.size() != dimensions ||
            std::any_of(point.begin(), point.end(), [](double value) { return !std::isfinite(value); })) {
            throw std::invalid_argument("polyline points must have consistent finite coordinates");
        }
    }
    if (position_offset) {
        if (position_offset->size() != dimensions ||
            std::any_of(position_offset->begin(), position_offset->end(), [](double value) {
                return !std::isfinite(value);
            })) {
            throw std::invalid_argument("polyline offset must match the point dimension and be finite");
        }
    }

    std::vector<std::pair<bool, std::vector<int>>> segments;
    for (const auto& segment : options.segments.value_or(std::vector<ifcapi::bindings::ShapeBuilderCurveSegment>{})) {
        std::visit(
            [&](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                std::vector<std::uint32_t> indices;
                bool is_arc = false;
                if constexpr (std::is_same_v<T, ifcapi::bindings::ShapeBuilderLineSegment>) {
                    if (value.line_indices.size() < 2) {
                        throw std::invalid_argument("line segment requires at least two point indices");
                    }
                    indices = value.line_indices;
                } else {
                    indices.assign(value.arc_indices.begin(), value.arc_indices.end());
                    is_arc = true;
                }
                std::vector<int> converted;
                for (std::uint32_t index : indices) {
                    if (index >= input_points.size()) {
                        throw std::out_of_range("curve segment point index out of range");
                    }
                    converted.push_back(static_cast<int>(index) + 1);
                }
                segments.emplace_back(is_arc, std::move(converted));
            },
            segment);
    }

    std::vector<std::vector<double>> points;
    points.reserve(input_points.size());
    for (const auto& point : input_points) {
        points.push_back(position_offset ? add_vectors(point, *position_offset) : point);
    }
    if (file->schema()->name() == "IFC2X3") {
        if (std::any_of(segments.begin(), segments.end(), [](const auto& value) { return value.first; }) ||
            segments.size() > 1) {
            throw std::invalid_argument("IFC2X3 polylines cannot represent explicit arc or multiple segments");
        }
        std::vector<std::uint32_t> indices;
        if (!segments.empty()) {
            for (int index : segments.front().second) indices.push_back(static_cast<std::uint32_t>(index - 1));
        } else {
            for (std::uint32_t index = 0; index < points.size(); ++index) indices.push_back(index);
        }
        std::vector<express::Base> ifc_points;
        for (std::uint32_t index : indices) ifc_points.push_back(cartesian_point(file, points[index]));
        auto result = create_entity(file, "IfcPolyline");
        set_refs(result, "Points", ifc_points);
        return result;
    }

    auto point_list = create_entity(file, dimensions == 2 ? "IfcCartesianPointList2D" : "IfcCartesianPointList3D");
    set_attr(point_list, "CoordList", points);
    auto result = create_entity(file, "IfcIndexedPolyCurve");
    set_ref(result, "Points", point_list);
    if (!options.segments) return result;
    std::vector<express::Base> ifc_segments;
    for (const auto& segment : segments) {
        ifc_segments.push_back(segment.first ? arc_index(file, segment.second) : line_index(file, segment.second));
    }
    set_refs(result, "Segments", ifc_segments);
    return result;
}

express::Base cartesian_point_list_3d(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points)
{
    auto point_list = create_entity(file, "IfcCartesianPointList3D");
    set_attr(point_list, "CoordList", points);
    return point_list;
}

express::Base indexed_polygonal_face(ifcopenshell::file* file, const std::vector<int>& face) {
    if (face.empty()) {
        throw std::invalid_argument("polygonal face loop must contain at least one index");
    }
    auto ifc_face = create_entity(file, "IfcIndexedPolygonalFace");
    set_attr(ifc_face, "CoordIndex", one_based(face));
    return ifc_face;
}

express::Base indexed_polygonal_face_with_voids(
    ifcopenshell::file* file,
    const std::vector<std::vector<int>>& loops)
{
    if (loops.empty()) {
        throw std::invalid_argument("polygonal face with voids requires at least one loop");
    }
    for (const auto& loop : loops) {
        if (loop.empty()) {
            throw std::invalid_argument("polygonal face loop must contain at least one index");
        }
    }
    auto ifc_face = create_entity(file, "IfcIndexedPolygonalFaceWithVoids");
    set_attr(ifc_face, "CoordIndex", one_based(loops.front()));
    std::vector<std::vector<int>> inner;
    inner.reserve(loops.size() - 1);
    for (size_t i = 1; i < loops.size(); ++i) {
        inner.push_back(one_based(loops[i]));
    }
    set_attr(ifc_face, "InnerCoordIndices", inner);
    return ifc_face;
}

express::Base faceted_brep_impl(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& faces)
{
    std::vector<express::Base> vertices;
    vertices.reserve(points.size());
    for (const auto& point : points) {
        vertices.push_back(cartesian_point(file, point));
    }

    std::vector<express::Base> ifc_faces;
    ifc_faces.reserve(faces.size());
    for (const auto& face : faces) {
        std::vector<express::Base> loop_vertices;
        loop_vertices.reserve(face.size());
        for (int index : face) {
            if (index < 0 || static_cast<size_t>(index) >= vertices.size()) {
                throw std::out_of_range("face index out of range");
            }
            loop_vertices.push_back(vertices[static_cast<size_t>(index)]);
        }
        auto loop = create_entity(file, "IfcPolyLoop");
        set_refs(loop, "Polygon", loop_vertices);
        auto bound = create_entity(file, "IfcFaceOuterBound");
        set_ref(bound, "Bound", loop);
        set_attr(bound, "Orientation", true);
        auto ifc_face = create_entity(file, "IfcFace");
        set_refs(ifc_face, "Bounds", {bound});
        ifc_faces.push_back(ifc_face);
    }

    auto shell = create_entity(file, "IfcClosedShell");
    set_refs(shell, "CfsFaces", ifc_faces);
    auto brep = create_entity(file, "IfcFacetedBrep");
    set_ref(brep, "Outer", shell);
    return brep;
}

express::Base triangulated_face_set_impl(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<int>>& faces)
{
    auto point_list = cartesian_point_list_3d(file, points);
    std::vector<std::vector<int>> coord_index;
    coord_index.reserve(faces.size());
    for (const auto& face : faces) {
        auto converted = one_based(face);
        if (converted.size() > 3) {
            converted.resize(3);
        }
        coord_index.push_back(std::move(converted));
    }
    auto result = create_entity(file, "IfcTriangulatedFaceSet");
    set_ref(result, "Coordinates", point_list);
    set_attr(result, "CoordIndex", coord_index);
    return result;
}

express::Base polygonal_face_set_impl(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<std::vector<int>>>& faces)
{
    auto point_list = cartesian_point_list_3d(file, points);
    std::vector<express::Base> ifc_faces;
    ifc_faces.reserve(faces.size());
    for (const auto& face : faces) {
        if (face.empty()) {
            throw std::invalid_argument("polygonal face must contain at least one loop");
        }
        ifc_faces.push_back(
            face.size() == 1 ? indexed_polygonal_face(file, face.front()) : indexed_polygonal_face_with_voids(file, face)
        );
    }
    auto result = create_entity(file, "IfcPolygonalFaceSet");
    set_ref(result, "Coordinates", point_list);
    set_refs(result, "Faces", ifc_faces);
    return result;
}

bool is_a(express::Base entity, const char* name) {
    return entity && entity.declaration().is(name);
}

template <typename Predicate>
bool all_items_are(const std::vector<express::Base>& items, Predicate predicate) {
    return !items.empty() && std::all_of(items.begin(), items.end(), [&](auto item) {
        return item && predicate(item);
    });
}

int read_dim(express::Base entity) {
    auto decl = entity ? entity.declaration().as_entity() : nullptr;
    if (!decl) {
        return 0;
    }
    int dim_idx = decl->attribute_index("Dim");
    if (dim_idx < 0) {
        return 0;
    }
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(dim_idx));
        return value.isNull() ? 0 : static_cast<int>(value);
    } catch (...) {
        return 0;
    }
}

std::string guess_representation_type(const std::vector<express::Base>& items) {
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcVertexPoint"); })) return "Vertex";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcEdge"); })) return "Edge";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcPath"); })) return "Path";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcFace"); })) return "Face";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcShell"); })) return "Shell";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcMappedItem"); })) return "MappedRepresentation";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcExtrudedAreaSolid") || is_a(item, "IfcRevolvedAreaSolid"); })) return "SweptSolid";
    if (all_items_are(items, [](auto item) {
            return is_a(item, "IfcSweptAreaSolid") || is_a(item, "IfcSweptDiskSolid") ||
                is_a(item, "IfcSectionedSolidHorizontal");
        })) return "AdvancedSweptSolid";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcTessellatedItem"); })) return "Tessellation";
    if (all_items_are(items, [](auto item) {
            return is_a(item, "IfcBooleanResult") || is_a(item, "IfcCsgPrimitive3D") || is_a(item, "IfcCsgSolid");
        })) return "CSG";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcCurve") && read_dim(item) == 2; })) return "Curve2D";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcCurve") && read_dim(item) == 3; })) return "Curve3D";
    if (all_items_are(items, [](auto item) { return is_a(item, "IfcCurve"); })) return "Curve";
    return "";
}


std::string read_string(express::Base entity, const char* attr);

express::Base representation_impl(
    ifcopenshell::file* file,
    express::Base context,
    const std::vector<express::Base>& items,
    const std::string& representation_type)
{
    std::string rep_type = representation_type.empty() ? guess_representation_type(items) : representation_type;
    auto result = create_entity(
        file,
        rep_type == "Vertex" || rep_type == "Edge" || rep_type == "Path" || rep_type == "Face" || rep_type == "Shell"
            ? "IfcTopologyRepresentation"
            : "IfcShapeRepresentation"
    );
    set_ref(result, "ContextOfItems", context);
    if (context) {
        auto context_identifier = ifcapi::detail::read_optional_string_attr(context, "ContextIdentifier");
        if (context_identifier.has_value) {
            set_attr(result, "RepresentationIdentifier", context_identifier.value);
        }
    }
    if (!rep_type.empty()) {
        set_attr(result, "RepresentationType", rep_type);
    }
    set_refs(result, "Items", items);
    return result;
}


express::Base read_ref(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return {};
        return static_cast<express::Base>(value);
    } catch (...) {
        return {};
    }
}

std::string read_string(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return {};
        return static_cast<std::string>(value);
    } catch (...) {
        return {};
    }
}

double read_double(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return 0.0;
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return 0.0;
        return static_cast<double>(value);
    } catch (...) {
        return 0.0;
    }
}

std::vector<double> read_double_list(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return {};
        return static_cast<std::vector<double>>(value);
    } catch (...) {
        return {};
    }
}

std::vector<std::vector<double>> read_double_list_list(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return {};
        return static_cast<std::vector<std::vector<double>>>(value);
    } catch (...) {
        return {};
    }
}

std::vector<express::Base> read_refs(express::Base entity, const char* attr) {
    int index = attr_index(entity, attr);
    if (index < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(index));
        if (value.isNull()) return {};
        return static_cast<std::vector<express::Base>>(value);
    } catch (...) {
        return {};
    }
}

std::vector<double> read_point_coordinates(express::Base point) {
    return read_double_list(point, "Coordinates");
}

void set_point_coordinates(express::Base point, const std::vector<double>& coordinates) {
    set_attr(point, "Coordinates", coordinates);
}

std::vector<std::vector<double>> polyline_coords(express::Base polyline) {
    if (is_a(polyline, "IfcIndexedPolyCurve")) {
        return read_double_list_list(read_ref(polyline, "Points"), "CoordList");
    }
    if (is_a(polyline, "IfcPolyline")) {
        std::vector<std::vector<double>> result;
        for (auto point : read_refs(polyline, "Points")) {
            result.push_back(read_point_coordinates(point));
        }
        return result;
    }
    throw std::invalid_argument(std::string(polyline ? polyline.declaration().name() : "<null>") + " is not a polyline");
}

void set_polyline_coords_impl(express::Base polyline, const std::vector<std::vector<double>>& coords) {
    if (is_a(polyline, "IfcIndexedPolyCurve")) {
        set_attr(read_ref(polyline, "Points"), "CoordList", coords);
        return;
    }
    if (is_a(polyline, "IfcPolyline")) {
        auto points = read_refs(polyline, "Points");
        if (points.size() != coords.size()) {
            throw std::invalid_argument("coordinate count does not match IfcPolyline point count");
        }
        for (size_t i = 0; i < coords.size(); ++i) {
            set_point_coordinates(points[i], coords[i]);
        }
        return;
    }
    throw std::invalid_argument(std::string(polyline ? polyline.declaration().name() : "<null>") + " is not a polyline");
}

std::vector<double> rotate_point_2d(
    const std::vector<double>& point,
    double angle,
    const std::vector<double>& pivot,
    bool counter_clockwise)
{
    constexpr double pi = 3.14159265358979323846;
    const double angle_rad = angle * pi / 180.0 * (counter_clockwise ? 1.0 : -1.0);
    const double px = pivot.size() > 0 ? pivot[0] : 0.0;
    const double py = pivot.size() > 1 ? pivot[1] : 0.0;
    const double x = (point.size() > 0 ? point[0] : 0.0) - px;
    const double y = (point.size() > 1 ? point[1] : 0.0) - py;
    std::vector<double> result = point;
    if (result.size() < 2) result.resize(2, 0.0);
    result[0] = std::cos(angle_rad) * x - std::sin(angle_rad) * y + px;
    result[1] = std::sin(angle_rad) * x + std::cos(angle_rad) * y + py;
    return result;
}

std::vector<double> mirror_point_2d(
    const std::vector<double>& point,
    const std::vector<double>& mirror_axes,
    const std::vector<double>& mirror_point)
{
    const double mx = mirror_axes.size() > 0 && mirror_axes[0] > 0.0 ? -1.0 : 1.0;
    const double my = mirror_axes.size() > 1 && mirror_axes[1] > 0.0 ? -1.0 : 1.0;
    const double px = mirror_point.size() > 0 ? mirror_point[0] : 0.0;
    const double py = mirror_point.size() > 1 ? mirror_point[1] : 0.0;
    std::vector<double> result = point;
    if (result.size() < 2) result.resize(2, 0.0);
    result[0] = (result[0] - px) * mx + px;
    result[1] = (result[1] - py) * my + py;
    return result;
}

std::vector<double> mat3_from_matrix(const std::vector<double>& matrix) {
    if (matrix.size() == 9) {
        return matrix;
    }
    if (matrix.size() == 16) {
        return {
            matrix[0], matrix[1], matrix[2],
            matrix[4], matrix[5], matrix[6],
            matrix[8], matrix[9], matrix[10],
        };
    }
    return {};
}

std::vector<double> mat3_mul_vec3(const std::vector<double>& matrix, const std::vector<double>& value) {
    return {
        matrix[0] * value[0] + matrix[1] * value[1] + matrix[2] * value[2],
        matrix[3] * value[0] + matrix[4] * value[1] + matrix[5] * value[2],
        matrix[6] * value[0] + matrix[7] * value[1] + matrix[8] * value[2],
    };
}

std::vector<double> mat3_inverse(const std::vector<double>& m) {
    const double det =
        m[0] * (m[4] * m[8] - m[5] * m[7]) -
        m[1] * (m[3] * m[8] - m[5] * m[6]) +
        m[2] * (m[3] * m[7] - m[4] * m[6]);
    if (std::fabs(det) < 1.0e-12) {
        throw std::invalid_argument("placement matrix is singular");
    }
    const double inv_det = 1.0 / det;
    return {
        (m[4] * m[8] - m[5] * m[7]) * inv_det,
        (m[2] * m[7] - m[1] * m[8]) * inv_det,
        (m[1] * m[5] - m[2] * m[4]) * inv_det,
        (m[5] * m[6] - m[3] * m[8]) * inv_det,
        (m[0] * m[8] - m[2] * m[6]) * inv_det,
        (m[2] * m[3] - m[0] * m[5]) * inv_det,
        (m[3] * m[7] - m[4] * m[6]) * inv_det,
        (m[1] * m[6] - m[0] * m[7]) * inv_det,
        (m[0] * m[4] - m[1] * m[3]) * inv_det,
    };
}

std::vector<double> mirror_point_2d_in_placement(
    const std::vector<double>& point,
    const std::vector<double>& mirror_axes,
    const std::vector<double>& mirror_point,
    const std::vector<double>& placement_matrix)
{
    const auto matrix = mat3_from_matrix(placement_matrix);
    if (matrix.empty()) {
        return mirror_point_2d(point, mirror_axes, mirror_point);
    }
    const auto inverse = mat3_inverse(matrix);
    std::vector<double> local = {point.size() > 0 ? point[0] : 0.0, point.size() > 1 ? point[1] : 0.0, 0.0};
    const auto placed = mat3_mul_vec3(matrix, local);
    auto mirrored = mirror_point_2d(placed, mirror_axes, mirror_point);
    mirrored.resize(3, 0.0);
    mirrored[2] = placed[2];
    const auto restored = mat3_mul_vec3(inverse, mirrored);
    return {restored[0], restored[1]};
}

bool close_to(double value, double target) {
    constexpr double precision = 1.0e-5;
    return (target + precision) > value && value > (target - precision);
}

double no_mep_result() {
    return std::numeric_limits<double>::quiet_NaN();
}

bool has_mep_result(double value) {
    return !std::isnan(value);
}

std::vector<double> abs_xy_diff(const std::vector<double>& start_half_dim, const std::vector<double>& end_half_dim) {
    return {
        std::fabs((start_half_dim.size() > 0 ? start_half_dim[0] : 0.0) - (end_half_dim.size() > 0 ? end_half_dim[0] : 0.0)),
        std::fabs((start_half_dim.size() > 1 ? start_half_dim[1] : 0.0) - (end_half_dim.size() > 1 ? end_half_dim[1] : 0.0)),
    };
}

std::vector<double> mep_dimensions(const ifcapi::bindings::ShapeBuilderMepProfileHalfDimensions& value) {
    return {value.half_x, value.half_y, value.depth};
}

std::vector<double> mep_offset(const ifcapi::bindings::ShapeBuilderMepOffset& value) {
    return {value.x, value.y};
}

std::vector<double> mep_bend_direction(const ifcapi::bindings::ShapeBuilderMepBendDirection& value) {
    return {value.x, value.y};
}

std::vector<double> rounded_mep_offset(const std::vector<double>& profile_offset) {
    std::vector<double> result = profile_offset;
    if (result.size() < 2) result.resize(2, 0.0);
    const double scale = 100000.0;
    result[0] = std::round(result[0] * scale) / scale;
    result[1] = std::round(result[1] * scale) / scale;
    return result;
}

double mep_transition_calculate_impl(
    const std::vector<double>& start_half_dim,
    const std::vector<double>& end_half_dim,
    const std::vector<double>& raw_offset,
    const std::vector<double>& raw_diff,
    bool has_diff,
    bool end_profile,
    double length,
    bool has_length,
    double angle,
    bool has_angle)
{
    std::vector<double> diff = has_diff ? raw_diff : abs_xy_diff(start_half_dim, end_half_dim);
    std::vector<double> offset = raw_offset;
    if (diff.size() < 2) diff.resize(2, 0.0);
    if (offset.size() < 2) offset.resize(2, 0.0);
    if (end_profile) {
        std::swap(diff[0], diff[1]);
        std::swap(offset[0], offset[1]);
    }
    const bool same_dimension = close_to(diff[0], 0.0);
    const double a = diff[0] + offset[0];
    const double b = diff[0] - offset[0];
    constexpr double pi = 3.14159265358979323846;
    if (!has_length) {
        if (!has_angle) return no_mep_result();
        if (!same_dimension) {
            const double t = std::tan(angle * pi / 180.0);
            const double h0 = a * a + 4.0 * a * b * t * t + 2.0 * a * b + b * b;
            if (h0 < 0.0) return no_mep_result();
            const double h = (a + b + std::sqrt(h0)) / (2.0 * t);
            const double length_squared = h * h - offset[1] * offset[1];
            if (length_squared <= 0.0) return no_mep_result();
            return std::sqrt(length_squared);
        }
        if (close_to(offset[0], 0.0)) {
            const double fixed_angle = 90.0;
            const double h = start_half_dim[0] / std::tan((fixed_angle / 2.0) * pi / 180.0);
            const double length_squared = h * h - offset[1] * offset[1];
            if (length_squared <= 0.0) return no_mep_result();
            return std::sqrt(length_squared);
        }
        const double h = offset[0] / std::tan(angle * pi / 180.0);
        const double length_squared = h * h - offset[1] * offset[1];
        if (length_squared <= 0.0) return no_mep_result();
        return std::sqrt(length_squared);
    }
    if (has_angle) return no_mep_result();
    if (!same_dimension) {
        if (length == 0.0) return 0.0;
        const double h = std::sqrt(length * length + offset[1] * offset[1]);
        const double t = -h * (a + b) / (a * b - h * h);
        return std::atan(t) * 180.0 / pi;
    }
    const double h = std::sqrt(length * length + offset[1] * offset[1]);
    if (close_to(offset[0], 0.0)) {
        return (2.0 * std::atan(start_half_dim[0] / h)) * 180.0 / pi;
    }
    return std::atan(offset[0] / h) * 180.0 / pi;
}

double mep_transition_length_impl(
    const std::vector<double>& start_half_dim,
    const std::vector<double>& end_half_dim,
    double angle,
    const std::vector<double>& profile_offset)
{
    const std::vector<double> offset = rounded_mep_offset(profile_offset);
    const std::vector<double> diff = abs_xy_diff(start_half_dim, end_half_dim);
    auto check_transition = [&](bool end_profile) {
        const double length = mep_transition_calculate_impl(
            start_half_dim, end_half_dim, offset, diff, true, end_profile, 0.0, false, angle, true);
        if (!has_mep_result(length)) return no_mep_result();
        const double other_side_angle = mep_transition_calculate_impl(
            start_half_dim, end_half_dim, offset, diff, true, !end_profile, length, true, 0.0, false);
        if (!has_mep_result(other_side_angle)) return no_mep_result();
        const bool same_dimension = close_to(diff[end_profile ? 0 : 1], 0.0);
        const double requested_angle =
            same_dimension && close_to(offset[end_profile ? 0 : 1], 0.0) ? 90.0 : angle;
        if (other_side_angle < requested_angle || close_to(other_side_angle, requested_angle)) {
            return length;
        }
        return no_mep_result();
    };
    const double start_length = check_transition(false);
    if (has_mep_result(start_length)) return start_length;
    return check_transition(true);
}

express::Base mep_profile(express::Base element) {
    auto material = ifcapi::bindings::element_get_material(&element, {true, false});
    if (!material || !is_a(*material, "IfcMaterialProfileSet")) return {};
    auto profiles = read_refs(*material, "MaterialProfiles");
    if (profiles.size() != 1) return {};
    return read_ref(profiles.front(), "Profile");
}

std::vector<double> mep_dim(express::Base profile, double depth) {
    if (is_a(profile, "IfcRectangleProfileDef")) {
        return {read_double(profile, "XDim") / 2.0, read_double(profile, "YDim") / 2.0, depth};
    }
    if (is_a(profile, "IfcCircleProfileDef")) {
        const double radius = read_double(profile, "Radius");
        return {radius, radius, depth};
    }
    return {};
}

std::vector<std::vector<double>> circle_points(double radius, size_t segments = 16) {
    constexpr double pi = 3.14159265358979323846;
    std::vector<std::vector<double>> result;
    result.reserve(segments);
    for (size_t i = 0; i < segments; ++i) {
        const double angle = 2.0 * pi * static_cast<double>(i) / static_cast<double>(segments);
        result.push_back({std::cos(angle) * radius, std::sin(angle) * radius, 0.0});
    }
    return result;
}

std::vector<std::vector<double>> bend_circle_points(
    const std::vector<double>& angles,
    double radius,
    int lateral_axis,
    double lateral_sign,
    double z_sign)
{
    constexpr double pi = 3.14159265358979323846;
    std::vector<std::vector<double>> result;
    result.reserve(angles.size());
    for (double angle : angles) {
        angle -= pi / 2.0;
        std::vector<double> point = {0.0, 0.0, 0.0};
        point[2] = z_sign * std::cos(angle) * radius;
        point[static_cast<size_t>(lateral_axis)] = lateral_sign * std::sin(angle) * radius;
        result.push_back(point);
    }
    return result;
}

std::vector<double> bend_circle_tangent(double angle, int lateral_axis, double lateral_sign, double z_sign) {
    std::vector<double> tangent = {0.0, 0.0, 0.0};
    tangent[2] = std::cos(angle) * z_sign;
    tangent[static_cast<size_t>(lateral_axis)] = std::sin(angle) * lateral_sign;
    return tangent;
}

std::vector<std::vector<double>> select_axes(
    const std::vector<std::vector<double>>& points,
    int first_axis,
    int second_axis)
{
    std::vector<std::vector<double>> result;
    result.reserve(points.size());
    for (const auto& point : points) {
        result.push_back({point[static_cast<size_t>(first_axis)], point[static_cast<size_t>(second_axis)]});
    }
    return result;
}

std::vector<std::vector<double>> rectangle_points(double x_dim, double y_dim) {
    const double x = x_dim / 2.0;
    const double y = y_dim / 2.0;
    return {{x, y, 0.0}, {-x, y, 0.0}, {-x, -y, 0.0}, {x, -y, 0.0}};
}

std::vector<double> mul_vec(const std::vector<double>& values, const std::vector<double>& factors) {
    std::vector<double> result = values;
    for (size_t i = 0; i < result.size() && i < factors.size(); ++i) {
        result[i] *= factors[i];
    }
    return result;
}

std::vector<std::vector<double>> add_offset(
    std::vector<std::vector<double>> points,
    const std::vector<double>& offset)
{
    for (auto& point : points) {
        point = add_vectors(point, offset);
    }
    return points;
}

express::Base extrude_face_set_impl(
    ifcopenshell::file* file,
    const std::vector<std::vector<double>>& points,
    double magnitude,
    const std::vector<double>& offset,
    bool start_cap,
    bool end_cap)
{
    auto bottom_points = add_offset(points, offset);
    auto top_points = add_offset(points, add_vectors(offset, {0.0, 0.0, magnitude}));
    std::vector<std::vector<double>> all_points = bottom_points;
    all_points.insert(all_points.end(), top_points.begin(), top_points.end());
    const int n = static_cast<int>(points.size());
    std::vector<std::vector<std::vector<int>>> faces;
    for (int i = 0; i < n; ++i) {
        const int next = (i + 1) % n;
        faces.push_back({{i, next, next + n, i + n}});
    }
    if (start_cap) {
        std::vector<int> cap;
        for (int i = n - 1; i >= 0; --i) cap.push_back(i);
        faces.push_back({cap});
    }
    if (end_cap) {
        std::vector<int> cap;
        for (int i = 0; i < n; ++i) cap.push_back(i + n);
        faces.push_back({cap});
    }
    return polygonal_face_set_impl(file, all_points, faces);
}

express::Base placement_location(express::Base entity) {
    auto position = read_ref(entity, "Position");
    return read_ref(position, "Location");
}

std::vector<double> location_coordinates(express::Base entity) {
    return read_point_coordinates(placement_location(entity));
}

void set_location_coordinates(express::Base entity, const std::vector<double>& coordinates) {
    set_point_coordinates(placement_location(entity), coordinates);
}

express::Base deep_copy_entity(
    ifcopenshell::file* file,
    express::Base element,
    std::unordered_map<unsigned, express::Base>& memo)
{
    if (!element) {
        return {};
    }
    auto id = element.id();
    if (id) {
        auto it = memo.find(id);
        if (it != memo.end()) {
            return it->second;
        }
    }
    const auto* declaration = element.declaration().as_entity();
    if (!declaration) {
        return element;
    }
    auto result = file->create(declaration);
    if (id) {
        memo[id] = result;
    }
    auto attributes = declaration->all_attributes();
    for (size_t i = 0; i < attributes.size(); ++i) {
        try {
            auto value = element.get_attribute_value(i);
            if (value.isNull()) {
                continue;
            }
            if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                result.set_attribute_value(i, deep_copy_entity(file, static_cast<express::Base>(value), memo));
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto aggregate = static_cast<std::vector<express::Base>>(value);
                std::vector<express::Base> copied;
                copied.reserve(aggregate.size());
                for (auto item : aggregate) {
                    copied.push_back(deep_copy_entity(file, item, memo));
                }
                result.set_attribute_value(i, copied);
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto aggregate = static_cast<std::vector<std::vector<express::Base>>>(value);
                std::vector<std::vector<express::Base>> copied;
                copied.reserve(aggregate.size());
                for (const auto& row : aggregate) {
                    std::vector<express::Base> copied_row;
                    copied_row.reserve(row.size());
                    for (auto item : row) {
                        copied_row.push_back(deep_copy_entity(file, item, memo));
                    }
                    copied.push_back(copied_row);
                }
                result.set_attribute_value(i, copied);
            } else {
                ifcapi::detail::copy_attribute_value(result, i, value);
            }
        } catch (...) {
        }
    }
    return result;
}

express::Base translate_impl(ifcopenshell::file* file, express::Base item, const std::vector<double>& translation, bool create_copy) {
    std::unordered_map<unsigned, express::Base> memo;
    auto result = create_copy ? deep_copy_entity(file, item, memo) : item;
    if (is_a(result, "IfcIndexedPolyCurve") || is_a(result, "IfcPolyline")) {
        auto coords = polyline_coords(result);
        for (auto& coord : coords) {
            coord = add_vectors(coord, translation);
        }
        set_polyline_coords_impl(result, coords);
    } else if (is_a(result, "IfcCircle") || is_a(result, "IfcExtrudedAreaSolid") || is_a(result, "IfcEllipse")) {
        set_location_coordinates(result, add_vectors(location_coordinates(result), translation));
    } else if (is_a(result, "IfcTessellatedFaceSet")) {
        auto coordinates = read_ref(result, "Coordinates");
        auto coords = read_double_list_list(coordinates, "CoordList");
        for (auto& coord : coords) {
            coord = add_vectors(coord, translation);
        }
        set_attr(coordinates, "CoordList", coords);
    } else if (is_a(result, "IfcShapeRepresentation")) {
        for (auto child : read_refs(result, "Items")) {
            translate_impl(file, child, translation, false);
        }
    } else if (is_a(result, "IfcTrimmedCurve")) {
        auto trim1 = read_refs(result, "Trim1");
        auto trim2 = read_refs(result, "Trim2");
        if (!trim1.empty()) set_point_coordinates(trim1.front(), add_vectors(read_point_coordinates(trim1.front()), translation));
        if (!trim2.empty()) set_point_coordinates(trim2.front(), add_vectors(read_point_coordinates(trim2.front()), translation));
        translate_impl(file, read_ref(result, "BasisCurve"), translation, false);
    } else {
        throw std::invalid_argument(result ? result.declaration().name() + " is not supported for translate() method." : "null is not supported for translate() method.");
    }
    return result;
}

express::Base rotate_impl(
    ifcopenshell::file* file,
    express::Base item,
    double angle,
    const std::vector<double>& pivot_point,
    bool counter_clockwise,
    bool create_copy)
{
    std::unordered_map<unsigned, express::Base> memo;
    auto result = create_copy ? deep_copy_entity(file, item, memo) : item;
    if (is_a(result, "IfcIndexedPolyCurve") || is_a(result, "IfcPolyline")) {
        auto coords = polyline_coords(result);
        for (auto& coord : coords) {
            coord = rotate_point_2d(coord, angle, pivot_point, counter_clockwise);
        }
        set_polyline_coords_impl(result, coords);
    } else if (is_a(result, "IfcCircle")) {
        set_location_coordinates(result, rotate_point_2d(location_coordinates(result), angle, pivot_point, counter_clockwise));
    } else if (is_a(result, "IfcExtrudedAreaSolid")) {
        auto position = location_coordinates(result);
        auto rotated = rotate_point_2d(position, angle, pivot_point, counter_clockwise);
        if (position.size() > 2) {
            rotated.resize(3, 0.0);
            rotated[2] = position[2];
        }
        set_location_coordinates(result, rotated);
        auto swept_area = read_ref(result, "SweptArea");
        rotate_impl(file, read_ref(swept_area, "OuterCurve"), angle, pivot_point, counter_clockwise, false);
    } else {
        throw std::invalid_argument(result ? result.declaration().name() + " is not supported for rotate() method." : "null is not supported for rotate() method.");
    }
    return result;
}

express::Base mirror_impl(
    ifcopenshell::file* file,
    express::Base item,
    const std::vector<double>& mirror_axes,
    const std::vector<double>& mirror_point,
    bool create_copy,
    const std::vector<double>& placement_matrix)
{
    std::unordered_map<unsigned, express::Base> memo;
    auto result = create_copy ? deep_copy_entity(file, item, memo) : item;
    if (is_a(result, "IfcIndexedPolyCurve") || is_a(result, "IfcPolyline")) {
        auto coords = polyline_coords(result);
        for (auto& coord : coords) {
            coord = mirror_point_2d_in_placement(coord, mirror_axes, mirror_point, placement_matrix);
        }
        set_polyline_coords_impl(result, coords);
    } else if (is_a(result, "IfcCircle") || is_a(result, "IfcEllipse")) {
        set_location_coordinates(result, mirror_point_2d(location_coordinates(result), mirror_axes, mirror_point));
    } else if (is_a(result, "IfcTrimmedCurve")) {
        auto trim1 = read_refs(result, "Trim1");
        auto trim2 = read_refs(result, "Trim2");
        if (!trim1.empty()) set_point_coordinates(trim1.front(), mirror_point_2d(read_point_coordinates(trim1.front()), mirror_axes, mirror_point));
        if (!trim2.empty()) set_point_coordinates(trim2.front(), mirror_point_2d(read_point_coordinates(trim2.front()), mirror_axes, mirror_point));
        if (std::find(mirror_axes.begin(), mirror_axes.end(), 0.0) != mirror_axes.end() && !trim1.empty() && !trim2.empty()) {
            const auto first = read_point_coordinates(trim1.front());
            set_point_coordinates(trim1.front(), read_point_coordinates(trim2.front()));
            set_point_coordinates(trim2.front(), first);
        }
        mirror_impl(file, read_ref(result, "BasisCurve"), mirror_axes, mirror_point, false, {});
    } else if (is_a(result, "IfcExtrudedAreaSolid")) {
        auto result_position = read_ref(result, "Position");
        const auto placement_matrix = ifcapi::bindings::placement_get_axis2_placement(&result_position);
        const auto placement = mat3_from_matrix(dynamic_array(placement_matrix));
        auto position = location_coordinates(result);
        auto mirrored = mirror_point_2d(position, mirror_axes, mirror_point);
        if (position.size() > 2) {
            mirrored.resize(3, 0.0);
            mirrored[2] = position[2];
        }
        set_location_coordinates(result, mirrored);
        auto swept_area = read_ref(result, "SweptArea");
        auto outer_curve = read_ref(swept_area, "OuterCurve");
        translate_impl(file, outer_curve, std::vector<double>{position[0], position.size() > 1 ? position[1] : 0.0}, false);
        mirror_impl(file, outer_curve, mirror_axes, mirror_point, false, placement);
        translate_impl(file, outer_curve, std::vector<double>{-mirrored[0], mirrored.size() > 1 ? -mirrored[1] : 0.0}, false);
        for (auto inner_curve : read_refs(swept_area, "InnerCurves")) {
            translate_impl(file, inner_curve, std::vector<double>{position[0], position.size() > 1 ? position[1] : 0.0}, false);
            mirror_impl(file, inner_curve, mirror_axes, mirror_point, false, placement);
            translate_impl(file, inner_curve, std::vector<double>{-mirrored[0], mirrored.size() > 1 ? -mirrored[1] : 0.0}, false);
        }
        auto direction_entity = read_ref(result, "ExtrudedDirection");
        auto direction = read_double_list(direction_entity, "DirectionRatios");
        if (placement.empty()) {
            set_attr(direction_entity, "DirectionRatios", mirror_point_2d(direction, mirror_axes, {0.0, 0.0}));
        } else {
            direction.resize(3, 0.0);
            const auto placed_direction = mat3_mul_vec3(placement, direction);
            auto mirrored_direction = mirror_point_2d(placed_direction, mirror_axes, {0.0, 0.0});
            mirrored_direction.resize(3, 0.0);
            mirrored_direction[2] = placed_direction[2];
            set_attr(direction_entity, "DirectionRatios", mat3_mul_vec3(mat3_inverse(placement), mirrored_direction));
        }
    } else {
        throw std::invalid_argument(result ? result.declaration().name() + " is not supported for mirror() method." : "null is not supported for mirror() method.");
    }
    return result;
}

template <typename Fn>
express::Base wrap_shape_builder_errors(const char* name, Fn&& fn) {
    try {
        return fn();
    } catch (const ifcapi::detail::Error& ex) {
        throw ifcapi::detail::Error(ex.kind(), ex.code(), std::string(name) + ": " + ex.what());
    } catch (const std::invalid_argument& ex) {
        throw std::invalid_argument(std::string(name) + ": " + ex.what());
    } catch (const std::domain_error& ex) {
        throw std::domain_error(std::string(name) + ": " + ex.what());
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string(name) + ": " + ex.what());
    } catch (...) {
        throw std::runtime_error(std::string(name) + ": unknown error");
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base shape_builder_mesh(
    ifcopenshell::file* file,
    const std::vector<Vec3>& points,
    const std::vector<std::vector<int>>& faces)
{
    return wrap_shape_builder_errors("shape_builder_mesh", [&]() -> express::Base {
        if (file && file->schema()->name() == "IFC2X3") {
            return faceted_brep_impl(file, dynamic_arrays(points), faces);
        }
        std::vector<std::vector<std::vector<int>>> polygon_faces;
        polygon_faces.reserve(faces.size());
        for (const auto& face : faces) {
            polygon_faces.push_back({face});
        }
        return polygonal_face_set_impl(file, dynamic_arrays(points), polygon_faces);
    });
}

express::Base shape_builder_faceted_brep(
    ifcopenshell::file* file,
    const std::vector<Vec3>& points,
    const std::vector<std::vector<int>>& faces)
{
    return wrap_shape_builder_errors("shape_builder_faceted_brep", [&]() {
        return faceted_brep_impl(file, dynamic_arrays(points), faces);
    });
}

express::Base shape_builder_triangulated_face_set(
    ifcopenshell::file* file,
    const std::vector<Vec3>& points,
    const std::vector<std::array<int, 3>>& faces)
{
    return wrap_shape_builder_errors("shape_builder_triangulated_face_set", [&]() {
        return triangulated_face_set_impl(file, dynamic_arrays(points), dynamic_arrays(faces));
    });
}

express::Base shape_builder_polygonal_face_set(
    ifcopenshell::file* file,
    const std::vector<Vec3>& points,
    const std::vector<std::vector<std::vector<int>>>& faces)
{
    return wrap_shape_builder_errors("shape_builder_polygonal_face_set", [&]() {
        return polygonal_face_set_impl(file, dynamic_arrays(points), faces);
    });
}

express::Base shape_builder_vertex(ifcopenshell::file* file, const Vec3& position)
{
    return wrap_shape_builder_errors("shape_builder_vertex", [&]() {
        return vertex_point(file, dynamic_array(position));
    });
}

express::Base shape_builder_edge(
    ifcopenshell::file* file,
    const Vec3& start,
    const Vec3& end)
{
    return wrap_shape_builder_errors("shape_builder_edge", [&]() {
        auto edge = create_entity(file, "IfcEdge");
        set_ref(edge, "EdgeStart", vertex_point(file, dynamic_array(start)));
        set_ref(edge, "EdgeEnd", vertex_point(file, dynamic_array(end)));
        return edge;
    });
}

express::Base shape_builder_face(ifcopenshell::file* file, const std::vector<Vec3>& points)
{
    return wrap_shape_builder_errors("shape_builder_face", [&]() {
        return face_impl(file, dynamic_arrays(points));
    });
}

express::Base shape_builder_polyline(
    ifcopenshell::file* file,
    const ShapeBuilderPolylineOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_polyline", [&]() {
        return semantic_polyline_impl(file, options);
    });
}

express::Base shape_builder_axis2_placement_3d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement3dOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_axis2_placement_3d", [&]() {
        return axis2_placement_3d(
            file,
            dynamic_array(options.position.value_or(Vec3{0.0, 0.0, 0.0})),
            dynamic_array(options.z_axis.value_or(Vec3{0.0, 0.0, 1.0})),
            dynamic_array(options.x_axis.value_or(Vec3{1.0, 0.0, 0.0})));
    });
}

express::Base shape_builder_axis2_placement_2d(
    ifcopenshell::file* file,
    const ShapeBuilderAxis2Placement2dOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_axis2_placement_2d", [&]() {
        static const std::vector<double> empty_direction;
        return axis2_placement_2d(
            file,
            dynamic_array(options.position.value_or(Vec2{0.0, 0.0})),
            options.x_direction ? dynamic_array(*options.x_direction) : empty_direction,
            static_cast<bool>(options.x_direction));
    });
}

express::Base shape_builder_circle(
    ifcopenshell::file* file,
    const Vec2& center,
    double radius)
{
    return wrap_shape_builder_errors("shape_builder_circle", [&]() {
        auto result = create_entity(file, "IfcCircle");
        set_ref(result, "Position", axis2_placement_2d(file, dynamic_array(center), {}, false));
        set_attr(result, "Radius", radius);
        return result;
    });
}

express::Base shape_builder_plane(
    ifcopenshell::file* file,
    const Vec3& location,
    const Vec3& normal)
{
    return wrap_shape_builder_errors("shape_builder_plane", [&]() {
        const auto location_values = dynamic_array(location);
        const auto normal_values = dynamic_array(normal);
        std::vector<double> arbitrary = {0.0, 0.0, 1.0};
        if (std::fabs(std::round(normal[0] * 100.0) / 100.0) == 0.0 &&
            std::fabs(std::round(normal[1] * 100.0) / 100.0) == 0.0 &&
            std::fabs(std::round(normal[2] * 100.0) / 100.0 - 1.0) == 0.0) {
            arbitrary = {0.0, 1.0, 0.0};
        }
        auto result = create_entity(file, "IfcPlane");
        set_ref(result, "Position", axis2_placement_3d(file, location_values, normal_values, normalized(cross3(normal_values, arbitrary))));
        return result;
    });
}

express::Base shape_builder_profile(
    ifcopenshell::file* file,
    const ShapeBuilderProfileOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_profile", [&]() {
        const auto inner_curves = options.inner_curves.value_or(std::vector<express::Base>{});
        auto result = create_entity(file, inner_curves.empty() ? "IfcArbitraryClosedProfileDef" : "IfcArbitraryProfileDefWithVoids");
        set_attr(result, "ProfileType", options.profile_type.value_or("AREA"));
        if (options.name && !options.name->empty()) {
            set_attr(result, "ProfileName", *options.name);
        }
        set_ref(result, "OuterCurve", options.outer_curve);
        if (!inner_curves.empty()) {
            set_refs(result, "InnerCurves", inner_curves);
        }
        return result;
    });
}

express::Base shape_builder_sphere(ifcopenshell::file* file, const ShapeBuilderSphereOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_sphere", [&]() {
        auto result = create_entity(file, "IfcSphere");
        set_ref(result, "Position", axis2_placement_3d(file, dynamic_array(options.center.value_or(Vec3{0.0, 0.0, 0.0})), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}));
        set_attr(result, "Radius", options.radius.value_or(1.0));
        return result;
    });
}

express::Base shape_builder_block(
    ifcopenshell::file* file,
    const ShapeBuilderBlockOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_block", [&]() {
        auto result = create_entity(file, "IfcBlock");
        set_ref(result, "Position", axis2_placement_3d(file, dynamic_array(options.position.value_or(Vec3{0.0, 0.0, 0.0})), {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}));
        set_attr(result, "XLength", options.x_length.value_or(1.0));
        set_attr(result, "YLength", options.y_length.value_or(1.0));
        set_attr(result, "ZLength", options.z_length.value_or(1.0));
        return result;
    });
}

express::Base shape_builder_half_space_solid(
    ifcopenshell::file* file,
    const ShapeBuilderHalfSpaceSolidOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_half_space_solid", [&]() {
        auto result = create_entity(file, "IfcHalfSpaceSolid");
        set_ref(result, "BaseSurface", options.plane);
        set_attr(result, "AgreementFlag", options.agreement_flag.value_or(false));
        return result;
    });
}

express::Base shape_builder_extrude(
    ifcopenshell::file* file,
    const ShapeBuilderExtrudeOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_extrude", [&]() {
        const double magnitude = options.magnitude.value_or(1.0);
        const auto position = dynamic_array(options.position.value_or(Vec3{0.0, 0.0, 0.0}));
        const auto extrusion_vector = dynamic_array(options.extrusion_vector.value_or(Vec3{0.0, 0.0, 1.0}));
        const auto position_z_axis = dynamic_array(options.position_z_axis.value_or(Vec3{0.0, 0.0, 1.0}));
        const auto position_x_axis = dynamic_array(options.position_x_axis.value_or(Vec3{1.0, 0.0, 0.0}));
        if (!std::isfinite(magnitude) || magnitude <= 0.0) {
            throw std::invalid_argument(
                "Extrusion magnitude must be greater than 0 to be valid.\n"
                "Ref: https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcPositiveLengthMeasure.htm#8.11.2.71.3-Formal-representation"
            );
        }
        express::Base profile = options.profile_or_curve;
        if (!is_a(profile, "IfcProfileDef")) {
            profile = shape_builder_profile(file, ShapeBuilderProfileOptions{profile, {}, {}, "AREA"});
        }
        std::vector<double> z_axis = options.position_y_axis ? cross3(position_x_axis, dynamic_array(*options.position_y_axis))
                                                             : position_z_axis;
        auto result = create_entity(file, "IfcExtrudedAreaSolid");
        set_ref(result, "SweptArea", profile);
        set_ref(result, "Position", axis2_placement_3d(file, position, z_axis, position_x_axis));
        set_ref(result, "ExtrudedDirection", direction(file, extrusion_vector));
        set_attr(result, "Depth", magnitude);
        return result;
    });
}

express::Base shape_builder_swept_disk_solid(
    ifcopenshell::file* file,
    express::Base* path_curve,
    double radius)
{
    return wrap_shape_builder_errors("shape_builder_swept_disk_solid", [&]() {
        auto result = create_entity(file, "IfcSweptDiskSolid");
        set_ref(result, "Directrix", ifcapi::detail::deref_or_empty(path_curve));
        set_attr(result, "Radius", radius);
        return result;
    });
}

express::Base shape_builder_representation(
    ifcopenshell::file* file,
    const ShapeBuilderRepresentationOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_representation", [&]() {
        return representation_impl(file, options.context, options.items, options.representation_type.value_or(""));
    });
}

express::Base shape_builder_deep_copy(ifcopenshell::file* file, express::Base* element)
{
    return wrap_shape_builder_errors("shape_builder_deep_copy", [&]() {
        std::unordered_map<unsigned, express::Base> memo;
        return deep_copy_entity(file, ifcapi::detail::deref_or_empty(element), memo);
    });
}

express::Base shape_builder_curve_between_two_points(
    ifcopenshell::file* file,
    const std::array<Vec2, 2>& points)
{
    return wrap_shape_builder_errors("shape_builder_curve_between_two_points", [&]() {
        std::vector<double> diff = {points[1][0] - points[0][0], points[1][1] - points[0][1]};
        const size_t max_i = std::fabs(diff[0]) >= std::fabs(diff[1]) ? 0 : 1;
        std::vector<double> middle(points[0].begin(), points[0].end());
        middle[max_i] += (diff[max_i] < 0.0 ? -0.01 : 0.01);
        return indexed_polycurve_2d_impl(
            file,
            ShapeBuilderIndexedPolycurve2dOptions{
                {{{points[0][0], points[0][1]}}, {{middle[0], middle[1]}}, {{points[1][0], points[1][1]}}},
                {ShapeBuilderArcSegment{{0, 1, 2}}}});
    });
}

express::Base shape_builder_ellipse_curve(
    ifcopenshell::file* file,
    const ShapeBuilderEllipseCurveOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_ellipse_curve", [&]() {
        static const std::vector<double> empty_direction;
        const double x_axis_radius = options.x_axis_radius;
        const double y_axis_radius = options.y_axis_radius;
        const auto position = dynamic_array(options.position.value_or(Vec2{0.0, 0.0}));
        if (!std::isfinite(x_axis_radius) || !std::isfinite(y_axis_radius) || x_axis_radius <= 0.0 ||
            y_axis_radius <= 0.0) {
            throw std::invalid_argument("ellipse radii must be finite and greater than zero");
        }
        std::vector<std::vector<double>> trims;
        if (options.trim) {
            std::visit(
                [&](const auto& trim) {
                    using Trim = std::decay_t<decltype(trim)>;
                    if constexpr (std::is_same_v<Trim, ShapeBuilderEllipsePointTrim>) {
                        for (const auto& point : trim.points) trims.push_back({point[0], point[1]});
                    } else {
                        const std::vector<std::vector<double>> cardinal = {
                            {x_axis_radius, 0.0},
                            {0.0, y_axis_radius},
                            {-x_axis_radius, 0.0},
                            {0.0, -y_axis_radius},
                        };
                        for (std::uint32_t index : trim.cardinal_points) {
                            if (index >= cardinal.size()) {
                                throw std::out_of_range("ellipse cardinal trim index out of range");
                            }
                            trims.push_back(add_vectors(cardinal[index], position));
                        }
                    }
                },
                options.trim->value);
        }
        auto ellipse = create_entity(file, "IfcEllipse");
        set_ref(
            ellipse,
            "Position",
            axis2_placement_2d(
                file,
                position,
                options.ref_x_direction ? dynamic_array(*options.ref_x_direction) : empty_direction,
                static_cast<bool>(options.ref_x_direction)));
        set_attr(ellipse, "SemiAxis1", x_axis_radius);
        set_attr(ellipse, "SemiAxis2", y_axis_radius);
        if (trims.empty()) {
            return ellipse;
        }
        if (trims.size() < 2) {
            throw std::invalid_argument("ellipse trimming requires two trim points");
        }
        auto result = create_entity(file, "IfcTrimmedCurve");
        set_ref(result, "BasisCurve", ellipse);
        set_refs(result, "Trim1", {cartesian_point(file, trims[0])});
        set_refs(result, "Trim2", {cartesian_point(file, trims[1])});
        set_attr(result, "SenseAgreement", true);
        set_attr(result, "MasterRepresentation", std::string("CARTESIAN"));
        return result;
    });
}

express::Base shape_builder_indexed_polycurve_2d(
    ifcopenshell::file* file,
    const ShapeBuilderIndexedPolycurve2dOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_indexed_polycurve_2d", [&]() {
        return indexed_polycurve_2d_impl(file, options);
    });
}

express::Base shape_builder_translate(
    ifcopenshell::file* file,
    const ShapeBuilderTranslateOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_translate", [&]() {
        return translate_impl(file, options.item, dynamic_vector(options.translation), options.create_copy.value_or(false));
    });
}

express::Base shape_builder_rotate(
    ifcopenshell::file* file,
    const ShapeBuilderRotateOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_rotate", [&]() {
        return rotate_impl(
            file,
            options.item,
            options.angle.value_or(90.0),
            dynamic_array(options.pivot_point.value_or(Vec2{0.0, 0.0})),
            options.counter_clockwise.value_or(false),
            options.create_copy.value_or(false));
    });
}

express::Base shape_builder_mirror(
    ifcopenshell::file* file,
    const ShapeBuilderMirrorOptions& options)
{
    return wrap_shape_builder_errors("shape_builder_mirror", [&]() {
        return mirror_impl(
            file,
            options.item,
            dynamic_array(options.mirror_axes.value_or(Vec2{1.0, 1.0})),
            dynamic_array(options.mirror_point.value_or(Vec2{0.0, 0.0})),
            options.create_copy.value_or(false),
            options.placement_matrix ? dynamic_matrix(*options.placement_matrix) : std::vector<double>{});
    });
}

std::vector<std::vector<double>> shape_builder_get_polyline_coords(express::Base* polyline)
{
    return polyline_coords(ifcapi::detail::deref_or_empty(polyline));
}

express::Base shape_builder_set_polyline_coords(
    ifcopenshell::file*,
    express::Base* polyline,
    const Vec2OrVec3List& coords)
{
    auto polyline_value = ifcapi::detail::deref_or_empty(polyline);
    set_polyline_coords_impl(polyline_value, dynamic_vectors(coords));
    return polyline_value;
}

double shape_builder_mep_transition_calculate(
    const ShapeBuilderMepTransitionCalculateOptions& options)
{
    static const std::vector<double> empty_diff;
    const auto start = mep_dimensions(options.start_half_dim);
    const auto end = mep_dimensions(options.end_half_dim);
    const auto offset = mep_offset(options.offset);
    const auto diff = options.diff ? mep_offset(*options.diff) : empty_diff;
    if (const auto* from_length = std::get_if<ShapeBuilderMepTransitionFromLength>(&options.calculation)) {
        return mep_transition_calculate_impl(
            start, end, offset, diff, static_cast<bool>(options.diff), options.end_profile.value_or(false),
            from_length->length, true, 0.0, false);
    }
    const auto& from_angle = std::get<ShapeBuilderMepTransitionFromAngle>(options.calculation);
    return mep_transition_calculate_impl(
        start, end, offset, diff, static_cast<bool>(options.diff), options.end_profile.value_or(false),
        0.0, false, from_angle.angle, true);
}

double shape_builder_mep_transition_length(
    const ShapeBuilderMepTransitionLengthOptions& options)
{
    return mep_transition_length_impl(
        mep_dimensions(options.start_half_dim),
        mep_dimensions(options.end_half_dim),
        options.angle,
        mep_offset(options.profile_offset.value_or(ShapeBuilderMepOffset{})));
}

std::optional<ShapeBuilderMepTransitionShapeResult> shape_builder_mep_transition_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepTransitionShapeOptions& options)
{
    ShapeBuilderMepTransitionShapeResult result;
    result.start_length = options.start_length;
    result.end_length = options.end_length;
    result.angle = options.angle.value_or(30.0);
    result.profile_offset = mep_offset(options.profile_offset.value_or(ShapeBuilderMepOffset{}));
    const double start_length = options.start_length;
    const double end_length = options.end_length;
    const double angle = options.angle.value_or(30.0);
    const auto profile_offset = mep_offset(options.profile_offset.value_or(ShapeBuilderMepOffset{}));
    auto start_profile = mep_profile(options.start_segment);
    auto end_profile = mep_profile(options.end_segment);
    if (!start_profile || !end_profile) return std::nullopt;
    auto start_half_dim = mep_dim(start_profile, start_length);
    auto end_half_dim = mep_dim(end_profile, end_length);
    if (start_half_dim.empty() || end_half_dim.empty()) return std::nullopt;
    double transition_length = mep_transition_length_impl(start_half_dim, end_half_dim, angle, profile_offset);
    if (!has_mep_result(transition_length)) return std::nullopt;
    std::vector<express::Base> items;
    std::vector<double> start_offset = {0.0, 0.0, start_length};
    std::vector<double> end_offset = start_offset;
    end_offset[2] += transition_length;
    end_offset = add_vectors(end_offset, {profile_offset.size() > 0 ? profile_offset[0] : 0.0, profile_offset.size() > 1 ? profile_offset[1] : 0.0, 0.0});
    std::vector<std::vector<double>> points;
    std::vector<std::vector<std::vector<int>>> faces;
    if (is_a(start_profile, "IfcRectangleProfileDef") && is_a(end_profile, "IfcRectangleProfileDef")) {
        if (transition_length == 0.0) return std::nullopt;
        std::vector<std::vector<int>> rect_faces = {
            {3, 4, 7, 0}, {11, 8, 15, 12}, {3, 11, 12, 4}, {7, 15, 8, 0},
            {0, 1, 2, 3}, {8, 11, 10, 9}, {0, 8, 9, 1}, {1, 9, 10, 2}, {2, 10, 11, 3},
            {4, 5, 6, 7}, {12, 15, 14, 13}, {4, 12, 13, 5}, {5, 13, 14, 6}, {6, 14, 15, 7},
        };
        for (auto& face : rect_faces) faces.push_back({face});
        points = {
            mul_vec(start_half_dim, {-1, -1, 1}), mul_vec(start_half_dim, {-1, -1, 0}),
            mul_vec(start_half_dim, {1, -1, 0}), mul_vec(start_half_dim, {1, -1, 1}),
            add_vectors(mul_vec(end_half_dim, {1, -1, 0}), end_offset),
            add_vectors(mul_vec(end_half_dim, {1, -1, 1}), end_offset),
            add_vectors(mul_vec(end_half_dim, {-1, -1, 1}), end_offset),
            add_vectors(mul_vec(end_half_dim, {-1, -1, 0}), end_offset),
            mul_vec(start_half_dim, {-1, 1, 1}), mul_vec(start_half_dim, {-1, 1, 0}),
            mul_vec(start_half_dim, {1, 1, 0}), mul_vec(start_half_dim, {1, 1, 1}),
            add_vectors(mul_vec(end_half_dim, {1, 1, 0}), end_offset),
            add_vectors(mul_vec(end_half_dim, {1, 1, 1}), end_offset),
            add_vectors(mul_vec(end_half_dim, {-1, 1, 1}), end_offset),
            add_vectors(mul_vec(end_half_dim, {-1, 1, 0}), end_offset),
        };
    } else if (is_a(start_profile, "IfcCircleProfileDef") && is_a(end_profile, "IfcCircleProfileDef")) {
        if (transition_length == 0.0) return std::nullopt;
        constexpr int n = 16;
        auto first = circle_points(read_double(start_profile, "Radius"), n);
        auto second = circle_points(read_double(end_profile, "Radius"), n);
        items.push_back(extrude_face_set_impl(file, first, start_length, {}, true, false));
        items.push_back(extrude_face_set_impl(file, second, end_length, end_offset, false, true));
        first = add_offset(first, start_offset);
        second = add_offset(second, end_offset);
        points = first;
        points.insert(points.end(), second.begin(), second.end());
        for (int i = 0; i < n; ++i) {
            int next = (i + 1) % n;
            faces.push_back({{i, next, next + n, i + n}});
        }
    } else {
        if (transition_length == 0.0) {
            transition_length = (start_length + end_length) / 2.0;
            end_offset[2] += transition_length;
        }
        const bool starting_with_circle = is_a(start_profile, "IfcCircleProfileDef");
        auto circle_profile = starting_with_circle ? start_profile : end_profile;
        auto rect_profile = starting_with_circle ? end_profile : start_profile;
        auto circle = circle_points(read_double(circle_profile, "Radius"));
        auto rect = rectangle_points(read_double(rect_profile, "XDim"), read_double(rect_profile, "YDim"));
        items.push_back(extrude_face_set_impl(file, starting_with_circle ? circle : rect, start_length, {}, true, false));
        items.push_back(extrude_face_set_impl(file, starting_with_circle ? rect : circle, end_length, end_offset, false, true));
        if (starting_with_circle) {
            circle = add_offset(circle, start_offset);
            rect = add_offset(rect, end_offset);
        } else {
            rect = add_offset(rect, start_offset);
            circle = add_offset(circle, end_offset);
        }
        points = circle;
        points.insert(points.end(), rect.begin(), rect.end());
        std::vector<std::vector<int>> transition_faces = {
            {0, 19, 16}, {0, 16, 1}, {1, 16, 2}, {2, 16, 3}, {3, 16, 4},
            {4, 16, 17}, {4, 17, 5}, {5, 17, 6}, {6, 17, 7}, {7, 17, 8},
            {8, 17, 18}, {8, 18, 9}, {9, 18, 10}, {10, 18, 11}, {11, 18, 12},
            {12, 18, 19}, {12, 19, 13}, {13, 19, 14}, {14, 19, 15}, {15, 19, 0},
        };
        if (starting_with_circle) {
            for (auto& face : transition_faces) std::reverse(face.begin(), face.end());
        }
        for (auto& face : transition_faces) faces.push_back({face});
    }
    items.push_back(polygonal_face_set_impl(file, points, faces));
    auto body = ifcapi::bindings::representation_get_context(file, "Model", "Body", "MODEL_VIEW");
    result.representation = representation_impl(file, body, items, "Tesselation");
    result.transition_length = transition_length;
    result.full_transition_length = start_length + transition_length + end_length;
    return result;
}

ShapeBuilderMepBendShapeResult shape_builder_mep_bend_shape(
    ifcopenshell::file* file,
    const ShapeBuilderMepBendShapeOptions& options)
{
    ShapeBuilderMepBendShapeResult result;
    const double start_length = options.start_length;
    const double end_length = options.end_length;
    const double angle = options.angle;
    const double radius = options.radius;
    const auto bend_vector = mep_bend_direction(options.bend_vector);
    const bool flip_z_axis = options.flip_z_axis;
    result.start_length = start_length;
    result.end_length = end_length;
    result.radius = radius;
    result.angle = angle * 180.0 / 3.14159265358979323846;
    result.z_axis_sign = flip_z_axis ? -1 : 1;
    auto profile = mep_profile(options.segment);
    if (!profile) {
        throw std::invalid_argument("segment must have a single material profile");
    }
    auto profile_dim = mep_dim(profile, start_length);
    if (profile_dim.empty()) {
        throw std::invalid_argument("segment material profile must be IfcRectangleProfileDef or IfcCircleProfileDef");
    }
    const bool is_circular_profile = is_a(profile, "IfcCircleProfileDef");
    const double si_conversion = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    std::vector<double> rounded = bend_vector;
    if (rounded.size() < 2) rounded.resize(2, 0.0);
    for (double& value : rounded) {
        value = std::round(value * si_conversion * 100000.0) / 100000.0 / si_conversion;
    }
    if (close_to(rounded[0], 0.0) && close_to(rounded[1], 0.0)) {
        throw std::invalid_argument("bend_vector must have a non-zero X or Y component");
    }
    int lateral_axis = close_to(rounded[0], 0.0) ? 1 : 0;
    int non_lateral_axis = lateral_axis == 0 ? 1 : 0;
    const double lateral_value =
        bend_vector.size() > static_cast<size_t>(lateral_axis) ? bend_vector[static_cast<size_t>(lateral_axis)] : 0.0;
    const double lateral_sign = (lateral_value < 0.0) ? -1.0 : 1.0;
    const double z_sign = flip_z_axis ? -1.0 : 1.0;
    result.lateral_axis = lateral_axis;
    result.lateral_sign = lateral_sign;
    result.main_profile_dimension = profile_dim[static_cast<size_t>(lateral_axis)];
    std::vector<express::Base> items;
    std::vector<double> center = {0.0, 0.0, 0.0};
    center[static_cast<size_t>(lateral_axis)] = (radius + profile_dim[static_cast<size_t>(lateral_axis)]) * lateral_sign;
    const std::vector<double> theta_segments = {0.0, angle / 2.0, angle};
    express::Base bend = {};
    if (is_circular_profile) {
        auto points = bend_circle_points(theta_segments, radius + profile_dim[static_cast<size_t>(lateral_axis)], lateral_axis, lateral_sign, z_sign);
        points = add_offset(points, center);
        std::vector<double> offset = {0.0, 0.0, z_sign * start_length};
        auto bend_path = polyline_impl(file, points, false, offset, true, {1});
        bend = shape_builder_swept_disk_solid(file, &bend_path, profile_dim[static_cast<size_t>(lateral_axis)]);
    } else {
        std::vector<std::vector<double>> points;
        std::vector<int> arc_points;
        const double outer_r = radius + 2.0 * profile_dim[static_cast<size_t>(lateral_axis)];
        std::vector<double> reversed = {angle, angle / 2.0, 0.0};
        auto outer_points = bend_circle_points(reversed, outer_r, lateral_axis, lateral_sign, z_sign);
        if (close_to(radius, 0.0)) {
            points = bend_circle_points({angle}, radius, lateral_axis, lateral_sign, z_sign);
            points.insert(points.end(), outer_points.begin(), outer_points.end());
            arc_points = {2};
        } else {
            points = bend_circle_points(theta_segments, radius, lateral_axis, lateral_sign, z_sign);
            points.insert(points.end(), outer_points.begin(), outer_points.end());
            arc_points = {1, 4};
        }
        points = add_offset(points, center);
        auto offset = std::vector<double>{0.0, 0.0, z_sign * start_length};
        offset[static_cast<size_t>(non_lateral_axis)] = -profile_dim[static_cast<size_t>(non_lateral_axis)];
        auto profile_curve = polyline_impl(file, select_axes(points, lateral_axis, 2), true, {}, false, arc_points);
        auto bend_profile = shape_builder_profile(file, ShapeBuilderProfileOptions{profile_curve, {}, {}, "AREA"});
        std::vector<double> z_axis = non_lateral_axis == 0 ? std::vector<double>{1.0, 0.0, 0.0} : std::vector<double>{0.0, 1.0, 0.0};
        std::vector<double> x_axis = non_lateral_axis == 0 ? std::vector<double>{0.0, 1.0, 0.0} : std::vector<double>{1.0, 0.0, 0.0};
        bend = shape_builder_extrude(
            file,
            ShapeBuilderExtrudeOptions{
                bend_profile,
                profile_dim[static_cast<size_t>(non_lateral_axis)] * 2.0,
                Vec3{offset.at(0), offset.at(1), offset.at(2)},
                Vec3{0.0, 0.0, 1.0},
                Vec3{z_axis.at(0), z_axis.at(1), z_axis.at(2)},
                Vec3{x_axis.at(0), x_axis.at(1), x_axis.at(2)},
                {}});
    }
    if (bend) items.push_back(bend);
    if (start_length != 0.0) {
        items.push_back(shape_builder_extrude(
            file,
            ShapeBuilderExtrudeOptions{
                profile,
                start_length,
                {},
                Vec3{0.0, 0.0, z_sign},
                Vec3{0.0, 0.0, 1.0},
                Vec3{1.0, 0.0, 0.0},
                {}}));
    }
    if (end_length != 0.0) {
        auto endpoint = bend_circle_points({angle}, radius + profile_dim[static_cast<size_t>(lateral_axis)], lateral_axis, lateral_sign, z_sign)[0];
        endpoint = add_vectors(center, endpoint);
        endpoint[2] += start_length * z_sign;
        auto z_axis = bend_circle_tangent(angle, lateral_axis, lateral_sign, z_sign);
        std::vector<double> x_axis = lateral_axis == 0 ? cross3(z_axis, {0.0, 1.0, 0.0}) : std::vector<double>{1.0, 0.0, 0.0};
        items.push_back(shape_builder_extrude(
            file,
            ShapeBuilderExtrudeOptions{
                profile,
                end_length,
                Vec3{endpoint.at(0), endpoint.at(1), endpoint.at(2)},
                Vec3{0.0, 0.0, 1.0},
                Vec3{z_axis.at(0), z_axis.at(1), z_axis.at(2)},
                Vec3{x_axis.at(0), x_axis.at(1), x_axis.at(2)},
                {}}));
    }
    auto body = ifcapi::bindings::representation_get_context(file, "Model", "Body", "MODEL_VIEW");
    result.representation = representation_impl(file, body, items, "");
    return result;
}

} // namespace bindings
} // namespace ifcapi
