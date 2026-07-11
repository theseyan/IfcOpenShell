// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Native port of the high-level representation-mapping APIs from
// `ifcopenshell.api.geometry` and parts of `ifcopenshell.util.element`.
//
// Functions implemented:
//   - geometry_map_representation
//   - geometry_assign_representation
//   - geometry_unassign_representation
//   - geometry_copy_representation
//   - (file-local) deep_copy_entity, get_representation_for_product
//
// These are used internally by type assignment to propagate IfcRepresentationMaps
// onto related occurrences when a typed object is assigned.

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/geometry.h"
#include "ifcapi/detail/matrix.h"
#include "ifcapi/detail/relationship.h"
#include "ifcapi/detail/representation.h"
#include "ifcapi/detail/shape_builder.h"
#include "ifcapi/detail/vector.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/argument_type.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cctype>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }

inline express::Entity as_entity(express::Base e) {
    return e.as<express::Entity>();
}

inline int attr_index(const ifcopenshell::entity* d, const char* name) {
    if (!d) return -1;
    return d->attribute_index(name);
}

inline int attr_index_of(express::Base e, const char* name) {
    try {
        auto be = as_entity(e);
        if (!be) return -1;
        return attr_index(be.declaration().as_entity(), name);
    } catch (...) {
        return -1;
    }
}

express::Base read_ref(express::Base e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        return (express::Base)v;
    } catch (...) { return {}; }
}

std::string read_string(express::Base e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        return (std::string)v;
    } catch (...) { return {}; }
}

std::vector<express::Base> read_ref_list(express::Base e, const char* attr) {
    std::vector<express::Base> out;
    int idx = attr_index_of(e, attr);
    if (idx < 0) return out;
    try {
        auto v = e.get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return out;
        auto agg = (std::vector<express::Base>)v;
        for (auto it : agg) out.push_back(it);
    } catch (...) {}
    return out;
}

void write_ref_list(express::Base e, const char* attr,
                    const std::vector<express::Base>& refs) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    if (refs.empty()) {
        e.set_attribute_value(static_cast<size_t>(idx), blank{});
        return;
    }
    e.set_attribute_value(static_cast<size_t>(idx), refs);
}

void write_double_list(express::Base e, const char* attr,
                       const std::vector<double>& values) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
                       e.set_attribute_value(static_cast<size_t>(idx), values);
}

void write_int(express::Base e, const char* attr, int v) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    e.set_attribute_value(static_cast<size_t>(idx), v);
}

void write_string(express::Base e, const char* attr, const std::string& v) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    ifcapi::detail::write_string_attr(e, attr, v);
}

void write_ref(express::Base e, const char* attr, express::Base ref) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    if (!ref) {
        e.set_attribute_value(static_cast<size_t>(idx), blank{});
        return;
    }
    e.set_attribute_value(static_cast<size_t>(idx), ref);
}

inline bool is_a(express::Base e, const char* name) {
    auto d = ifcopenshell::lock_data(e.data_weak());
    return d && d->declaration()->is(name);
}

inline express::Base* entity_ptr(express::Base& entity) {
    return entity ? &entity : nullptr;
}

inline express::Base* nullable_entity_ptr(express::Base& entity) {
    return entity ? &entity : nullptr;
}

bool is_terminal_connection(const std::string& connection_type) {
    return connection_type == "ATSTART" || connection_type == "ATEND";
}

std::vector<express::Base> inverse_refs(express::Base entity, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(entity, attr);
}

std::vector<express::Base> all_inverse_refs(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || !entity.id()) {
        return result;
    }
    try {
        auto inverses = file->instances_by_reference(static_cast<int>(entity.id()));
        for (auto inverse : inverses) {
            if (inverse) {
                result.push_back(inverse);
            }
        }
    } catch (...) {
    }
    return result;
}

void append_unique_connection(
    std::vector<express::Base>& connections,
    std::unordered_set<express::Base>& seen,
    express::Base connection)
{
    if (connection && seen.insert(connection).second) {
        connections.push_back(connection);
    }
}

void remove_connections_with_history(ifcopenshell::file* file, const std::vector<express::Base>& connections) {
    for (auto connection : connections) {
        ifcapi::detail::remove_with_history(file, connection);
    }
}

void write_optional_string(express::Base entity, const char* attr, const char* value, bool has_value) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return;
    }
    if (has_value) {
        entity.set_attribute_value(static_cast<size_t>(idx), std::string(value ? value : ""));
    } else {
        entity.set_attribute_value(static_cast<size_t>(idx), blank{});
    }
}

void write_bool(express::Base entity, const char* attr, bool value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity.set_attribute_value(static_cast<size_t>(idx), value);
    }
}

void write_empty_int_aggregate(express::Base entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity.set_attribute_value(static_cast<size_t>(idx), std::vector<int>());
    }
}

express::Base create_shape_representation_like(
    ifcopenshell::file* file,
    express::Base representation,
    const std::vector<express::Base>& items)
{
    auto aspect_rep = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
    write_ref(aspect_rep, "ContextOfItems", read_ref(representation, "ContextOfItems"));
    write_string(aspect_rep, "RepresentationIdentifier", read_string(representation, "RepresentationIdentifier"));
    write_string(aspect_rep, "RepresentationType", read_string(representation, "RepresentationType"));
    write_ref_list(aspect_rep, "Items", items);
    return aspect_rep;
}

bool is_boolean_operand(express::Base item) {
    return is_a(item, "IfcBooleanResult") || is_a(item, "IfcCsgPrimitive3D") || is_a(item, "IfcHalfSpaceSolid") ||
        is_a(item, "IfcSolidModel") || is_a(item, "IfcTessellatedFaceSet");
}

std::string topology_representation_type(express::Base item) {
    static const std::vector<std::pair<const char*, const char*>> type_map = {
        {"IfcVertex", "Vertex"},
        {"IfcVertexPoint", "Vertex"},
        {"IfcEdge", "Edge"},
        {"IfcOrientedEdge", "Edge"},
        {"IfcEdgeCurve", "Edge"},
        {"IfcEdgeLoop", "Edge"},
        {"IfcPath", "Edge"},
        {"IfcFace", "Face"},
        {"IfcFaceSurface", "Face"},
        {"IfcAdvancedFace", "Face"},
        {"IfcClosedShell", "Face"},
        {"IfcOpenShell", "Face"},
        {"IfcConnectedFaceSet", "Face"},
    };
    for (const auto& entry : type_map) {
        if (is_a(item, entry.first)) {
            return entry.second;
        }
    }
    return "Undefined";
}

template <typename Predicate>
bool all_guess_items_are(const std::vector<express::Base>& items, Predicate predicate) {
    return std::all_of(items.begin(), items.end(), [&](auto item) { return item && predicate(item); });
}

int read_dim_attr(express::Base item) {
    int idx = attr_index_of(item, "Dim");
    if (idx >= 0) {
        try {
            auto value = item.get_attribute_value(static_cast<size_t>(idx));
            if (!value.isNull()) return static_cast<int>(value);
        } catch (...) {
        }
    }
    if (is_a(item, "IfcIndexedPolyCurve")) {
        auto points = read_ref(item, "Points");
        if (is_a(points, "IfcCartesianPointList2D")) return 2;
        if (is_a(points, "IfcCartesianPointList3D")) return 3;
    }
    if (is_a(item, "IfcPolyline")) {
        auto points = read_ref_list(item, "Points");
        if (!points.empty()) return static_cast<int>(ifcapi::detail::read_double_aggregate(points.front(), "Coordinates").size());
    }
    if (is_a(item, "IfcCompositeCurve")) {
        auto segments = read_ref_list(item, "Segments");
        if (!segments.empty()) return read_dim_attr(read_ref(segments.front(), "ParentCurve"));
    }
    if (is_a(item, "IfcTrimmedCurve") || is_a(item, "IfcOffsetCurve2D") || is_a(item, "IfcOffsetCurve3D")) {
        return read_dim_attr(read_ref(item, "BasisCurve"));
    }
    return 0;
}

std::string guess_representation_type(const std::vector<express::Base>& items) {
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcMappedItem"); })) return "MappedRepresentation";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcPoint") || is_a(item, "IfcCartesianPointList"); })) return "Point";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcCartesianPointList3D"); })) return "PointCloud";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcCurve") && read_dim_attr(item) == 2; })) return "Curve2D";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcCurve") && read_dim_attr(item) == 3; })) return "Curve3D";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcCurve"); })) return "Curve";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSegment"); })) return "Segment";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSurface") && read_dim_attr(item) == 2; })) return "Surface2D";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSurface") && read_dim_attr(item) == 3; })) return "Surface3D";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSurface"); })) return "Surface";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSectionedSurface"); })) return "SectionedSurface";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcAnnotationFillArea"); })) return "FillArea";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcTextLiteral"); })) return "Text";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcBSplineSurface"); })) return "AdvancedSurface";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcGeometricSet") || is_a(item, "IfcPoint") || is_a(item, "IfcCurve") ||
                is_a(item, "IfcSurface");
        })) return "GeometricSet";
    if (all_guess_items_are(items, [](auto item) {
            if (is_a(item, "IfcGeometricCurveSet") || is_a(item, "IfcPoint") || is_a(item, "IfcCurve")) return true;
            if (!is_a(item, "IfcGeometricSet")) return false;
            return all_guess_items_are(read_ref_list(item, "Elements"), [](auto element) { return is_a(element, "IfcSurface"); });
        })) return "GeometricCurveSet";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcPoint") || is_a(item, "IfcCurve") || is_a(item, "IfcGeometricCurveSet") ||
                is_a(item, "IfcAnnotationFillArea") || is_a(item, "IfcTextLiteral");
        })) return "Annotation2D";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcTessellatedItem"); })) return "Tessellation";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcTessellatedItem") || is_a(item, "IfcShellBasedSurfaceModel") ||
                is_a(item, "IfcFaceBasedSurfaceModel");
        })) return "SurfaceModel";
    if (all_guess_items_are(items, [](auto item) {
            return item && (item.declaration().name() == "IfcExtrudedAreaSolid" || item.declaration().name() == "IfcRevolvedAreaSolid");
        })) return "SweptSolid";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSolidModel"); })) return "SolidModel";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcTessellatedItem") || is_a(item, "IfcShellBasedSurfaceModel") ||
                is_a(item, "IfcFaceBasedSurfaceModel") || is_a(item, "IfcSolidModel");
        })) return "SurfaceOrSolidModel";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcSweptAreaSolid") || is_a(item, "IfcSweptDiskSolid") ||
                is_a(item, "IfcSectionedSolidHorizontal");
        })) return "AdvancedSweptSolid";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcCsgSolid") || is_a(item, "IfcBooleanClippingResult"); })) return "Clipping";
    if (all_guess_items_are(items, [](auto item) {
            return is_a(item, "IfcBooleanResult") || is_a(item, "IfcCsgPrimitive3D") || is_a(item, "IfcCsgSolid");
        })) return "CSG";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcFacetedBrep"); })) return "Brep";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcManifoldSolidBrep"); })) return "AdvancedBrep";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcBoundingBox"); })) return "BoundingBox";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcSectionedSpine"); })) return "SectionedSpine";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcLightSource"); })) return "LightSource";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcVertex"); })) return "Vertex";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcEdge"); })) return "Edge";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcPath"); })) return "Path";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcFace"); })) return "Face";
    if (all_guess_items_are(items, [](auto item) { return is_a(item, "IfcOpenShell"); })) return "Shell";
    return "";
}

express::Base create_boolean_result(
    ifcopenshell::file* file,
    const std::string& operator_type,
    express::Base first,
    express::Base second)
{
    const bool should_clip = operator_type == "DIFFERENCE" && is_a(second, "IfcHalfSpaceSolid") &&
        (is_a(first, "IfcSweptAreaSolid") || is_a(first, "IfcSweptDiskSolid") || is_a(first, "IfcBooleanClippingResult"));
    auto boolean = file->create(file->schema()->declaration_by_name(should_clip ? "IfcBooleanClippingResult" : "IfcBooleanResult"));
    ifcapi::detail::write_enum_attr(boolean, "Operator", operator_type);
    write_ref(boolean, "FirstOperand", first);
    write_ref(boolean, "SecondOperand", second);
    return boolean;
}

void geometry_remove_boolean_impl(ifcopenshell::file* file, express::Base item) {
    if (!is_a(item, "IfcBooleanResult")) {
        for (auto inverse : all_inverse_refs(file, item)) {
            if (is_a(inverse, "IfcBooleanResult")) {
                geometry_remove_boolean_impl(file, inverse);
            }
        }
        return;
    }

    std::vector<express::Base> representations;
    std::vector<express::Base> queue = all_inverse_refs(file, item);
    while (!queue.empty()) {
        auto inverse = queue.back();
        queue.pop_back();
        if (is_a(inverse, "IfcShapeRepresentation")) {
            if (!ifcapi::detail::contains_ref(representations, inverse)) {
                representations.push_back(inverse);
            }
        } else if (is_a(inverse, "IfcBooleanResult") || is_a(inverse, "IfcCsgSolid")) {
            auto more = all_inverse_refs(file, inverse);
            queue.insert(queue.end(), more.begin(), more.end());
        }
    }

    auto first = read_ref(item, "FirstOperand");
    auto second = read_ref(item, "SecondOperand");
    for (auto inverse : all_inverse_refs(file, item)) {
        ifcapi::detail::replace_attribute_reference(inverse, item, first);
    }

    for (auto representation : representations) {
        auto items = read_ref_list(representation, "Items");
        items.push_back(second);
        write_ref_list(representation, "Items", items);
    }

    file->remove_entity(item);
}

// Construct standard geometry primitives reusable across representation
// assignment / mapping operations.

express::Base make_cartesian_point(ifcopenshell::file* file,
                                            double x, double y, double z) {
    auto decl = file->schema()->declaration_by_name("IfcCartesianPoint");
    auto p = file->create(decl);
    write_double_list(p, "Coordinates", {x, y, z});
    return p;
}

express::Base make_direction(ifcopenshell::file* file,
                                      double x, double y, double z) {
    auto decl = file->schema()->declaration_by_name("IfcDirection");
    auto p = file->create(decl);
    write_double_list(p, "DirectionRatios", {x, y, z});
    return p;
}

express::Base make_axis2_placement_3d(ifcopenshell::file* file) {
    auto decl = file->schema()->declaration_by_name("IfcAxis2Placement3D");
    auto p = file->create(decl);
    write_ref(p, "Location",     make_cartesian_point(file, 0.0, 0.0, 0.0));
    write_ref(p, "Axis",         make_direction(file, 0.0, 0.0, 1.0));
    write_ref(p, "RefDirection", make_direction(file, 1.0, 0.0, 0.0));
    return p;
}

bool allclose3(const std::vector<double>& value, const std::vector<double>& expected, double atol = 1e-2) {
    if (value.size() != 3 || expected.size() != 3) return false;
    for (size_t i = 0; i < 3; ++i) {
        if (std::fabs(value[i] - expected[i]) > atol) return false;
    }
    return true;
}

std::vector<double> cross3(const std::vector<double>& a, const std::vector<double>& b) {
    return {
        a.at(1) * b.at(2) - a.at(2) * b.at(1),
        a.at(2) * b.at(0) - a.at(0) * b.at(2),
        a.at(0) * b.at(1) - a.at(1) * b.at(0),
    };
}

void normalize_in_place(std::vector<double>& value) {
    double length = std::sqrt(value.at(0) * value.at(0) + value.at(1) * value.at(1) + value.at(2) * value.at(2));
    for (double& component : value) {
        component /= length;
    }
}

express::Base make_axis2_placement_3d(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    write_ref(placement, "Location", ifcapi::detail::create_cartesian_point(file, location));
    write_ref(placement, "Axis", make_direction(file, axis.at(0), axis.at(1), axis.at(2)));
    write_ref(placement, "RefDirection", make_direction(file, ref_direction.at(0), ref_direction.at(1), ref_direction.at(2)));
    return placement;
}

express::Base make_axis2_placement_3d_location_only(
    ifcopenshell::file* file,
    const std::vector<double>& location)
{
    auto placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    write_ref(placement, "Location", ifcapi::detail::create_cartesian_point(file, location));
    return placement;
}

std::vector<double> clipping_x_axis(const std::vector<double>& normal) {
    std::vector<double> arbitrary =
        (allclose3(normal, {0.0, 0.0, 1.0}) || allclose3(normal, {0.0, 0.0, -1.0}))
            ? std::vector<double>{0.0, 1.0, 0.0}
            : std::vector<double>{0.0, 0.0, 1.0};
    auto result = cross3(normal, arbitrary);
    normalize_in_place(result);
    return result;
}

express::Base make_clipping_plane(
    ifcopenshell::file* file,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    double unit_scale)
{
    auto scaled_location = location;
    for (auto& coordinate : scaled_location) coordinate /= unit_scale;
    auto plane = file->create(file->schema()->declaration_by_name("IfcPlane"));
    write_ref(plane, "Position", make_axis2_placement_3d(file, scaled_location, normal, clipping_x_axis(normal)));
    return plane;
}

express::Base make_clipping_result(
    ifcopenshell::file* file,
    express::Base first_operand,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    double unit_scale)
{
    auto half_space = file->create(file->schema()->declaration_by_name("IfcHalfSpaceSolid"));
    write_ref(half_space, "BaseSurface", make_clipping_plane(file, location, normal, unit_scale));
    write_bool(half_space, "AgreementFlag", false);

    auto result = file->create(file->schema()->declaration_by_name("IfcBooleanClippingResult"));
    write_string(result, "Operator", "DIFFERENCE");
    write_ref(result, "FirstOperand", first_operand);
    write_ref(result, "SecondOperand", half_space);
    return result;
}

express::Base copy_boolean_clipping(
    ifcopenshell::file* file,
    express::Base clipping,
    express::Base first_operand)
{
    auto copy = ifcapi::detail::copy_single(file, clipping);
    if (!copy) throw std::runtime_error("Unable to copy clipping entity");
    write_ref(copy, "FirstOperand", first_operand);
    return copy;
}

express::Base apply_ordered_clippings(
    ifcopenshell::file* file,
    express::Base first_operand,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<express::Base>& clipping_entities,
    double unit_scale)
{
    size_t plane_cursor = clipping_locations.size();
    size_t entity_cursor = clipping_entities.size();
    if (clipping_locations.size() != clipping_normals.size()) {
        throw std::runtime_error("Clipping location/normal count mismatch");
    }
    for (auto it = clipping_kinds.rbegin(); it != clipping_kinds.rend(); ++it) {
        if (*it == 0) {
            if (plane_cursor == 0) throw std::runtime_error("Missing clipping plane data");
            --plane_cursor;
            first_operand = make_clipping_result(
                file, first_operand, clipping_locations[plane_cursor], clipping_normals[plane_cursor], unit_scale);
        } else if (*it == 1) {
            if (entity_cursor == 0) throw std::runtime_error("Missing clipping entity data");
            --entity_cursor;
            first_operand = copy_boolean_clipping(file, clipping_entities[entity_cursor], first_operand);
        } else {
            throw std::runtime_error("Unknown clipping kind");
        }
    }
    if (plane_cursor != 0 || entity_cursor != 0) {
        throw std::runtime_error("Unused clipping data");
    }
    return first_operand;
}

express::Base make_closed_profile(ifcopenshell::file* file, express::Base curve) {
    auto profile = file->create(file->schema()->declaration_by_name("IfcArbitraryClosedProfileDef"));
    write_string(profile, "ProfileType", "AREA");
    write_ref(profile, "OuterCurve", curve);
    return profile;
}

express::Base make_shape_representation(
    ifcopenshell::file* file,
    express::Base context,
    const std::string& representation_type,
    express::Base item)
{
    auto representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
    write_ref(representation, "ContextOfItems", context);
    ifcapi::detail::copy_string_attr_preserving_null(representation, "RepresentationIdentifier", context, "ContextIdentifier");
    write_string(representation, "RepresentationType", representation_type);
    write_ref_list(representation, "Items", {item});
    return representation;
}

express::Base make_axis2_placement_3d_optional(
    ifcopenshell::file* file,
    const std::vector<double>& location)
{
    return make_axis2_placement_3d(file, location, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0});
}

std::vector<int> parse_json_int_array(const std::string& json) {
    std::vector<int> result;
    size_t i = 0;
    auto skip_ws = [&]() {
        while (i < json.size() && std::isspace(static_cast<unsigned char>(json[i]))) ++i;
    };
    skip_ws();
    if (i >= json.size() || json[i++] != '[') throw std::runtime_error("Expected JSON array");
    skip_ws();
    if (i < json.size() && json[i] == ']') {
        ++i;
        skip_ws();
        if (i != json.size()) throw std::runtime_error("Unexpected trailing JSON data");
        return result;
    }
    while (i < json.size()) {
        skip_ws();
        size_t start = i;
        if (i < json.size() && json[i] == '-') ++i;
        if (i >= json.size() || !std::isdigit(static_cast<unsigned char>(json[i]))) {
            throw std::runtime_error("Expected integer in JSON array");
        }
        while (i < json.size() && std::isdigit(static_cast<unsigned char>(json[i]))) ++i;
        result.push_back(std::stoi(json.substr(start, i - start)));
        skip_ws();
        if (i < json.size() && json[i] == ',') {
            ++i;
            continue;
        }
        if (i < json.size() && json[i] == ']') {
            ++i;
            break;
        }
        throw std::runtime_error("Expected comma or closing bracket in JSON array");
    }
    skip_ws();
    if (i != json.size()) throw std::runtime_error("Unexpected trailing JSON data");
    return result;
}

std::string dump_json_int_array(const std::vector<int>& values) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) out << ", ";
        out << values[i];
    }
    out << "]";
    return out.str();
}

std::string read_property_single_value_string(express::Base property) {
    auto value = read_ref(property, "NominalValue");
    if (!value) throw std::runtime_error("BBIM_Boolean Data property has no NominalValue");
    try {
        auto wrapped = value.get_attribute_value(0);
        if (wrapped.isNull()) throw std::runtime_error("BBIM_Boolean Data property has null NominalValue");
        return static_cast<std::string>(wrapped);
    } catch (const std::exception&) {
        throw;
    } catch (...) {
        throw std::runtime_error("BBIM_Boolean Data property is not a string value");
    }
}

std::string bbim_boolean_data(express::Base pset) {
    for (auto prop : read_ref_list(pset, "HasProperties")) {
        if (prop && is_a(prop, "IfcPropertySingleValue") && read_string(prop, "Name") == "Data") {
            return read_property_single_value_string(prop);
        }
    }
    throw std::runtime_error("BBIM_Boolean pset has no Data property");
}

express::Base find_bbim_boolean_pset(express::Base element) {
    auto psets = ifcapi::bindings::element_get_pset_ids(&element, {true, false, true});
    for (auto pset : psets) {
        if (pset && is_a(pset, "IfcPropertySet") && read_string(pset, "Name") == "BBIM_Boolean") {
            return pset;
        }
    }
    return {};
}

double read_double(express::Base entity, const char* attr, double fallback = 0.0) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) return fallback;
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(idx));
        if (value.isNull()) return fallback;
        return static_cast<double>(value);
    } catch (...) {
        return fallback;
    }
}

int read_int(express::Base entity, const char* attr, int fallback = 0) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) return fallback;
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(idx));
        if (value.isNull()) return fallback;
        return static_cast<int>(value);
    } catch (...) {
        return fallback;
    }
}

bool allclose(const std::vector<double>& a, const std::vector<double>& b, double rtol = 1e-5, double atol = 1e-8) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::fabs(a[i] - b[i]) > atol + rtol * std::fabs(b[i])) return false;
    }
    return true;
}

std::vector<double> add2(const std::vector<double>& a, const std::vector<double>& b) {
    return {a[0] + b[0], a[1] + b[1]};
}

std::vector<double> mul2(const std::vector<double>& a, double factor) {
    return {a[0] * factor, a[1] * factor};
}

double intersect_x_or_throw(const std::vector<double>& p1, const std::vector<double>& p2, double y) {
    double x = 0.0;
    if (!ifcapi::detail::intersect_x_axis_2d(p1, p2, y, x)) {
        throw std::runtime_error("Unable to intersect wall axes");
    }
    return x;
}

struct PrioritisedLayer {
    int priority = 0;
    double thickness = 0.0;
};

struct WallVectors {
    std::vector<double> z = {0.0, 0.0, 1.0};
    std::vector<double> y = {0.0, 1.0, 0.0};
    double a = 0.0;
    double d = 1.0;
    double h = 1.0;
};

std::vector<PrioritisedLayer> get_wall_layers(express::Base wall) {
    std::vector<PrioritisedLayer> result;
    auto material = ifcapi::bindings::element_get_material(&wall, {true, false});
    if (!material || !is_a(*material, "IfcMaterialLayerSet")) return result;
    for (auto layer : read_ref_list(*material, "MaterialLayers")) {
        result.push_back({read_int(layer, "Priority", 0), read_double(layer, "LayerThickness", 0.0)});
    }
    return result;
}

std::vector<PrioritisedLayer> combine_wall_layers(
    std::vector<PrioritisedLayer> layers,
    const std::vector<int>& override_priorities)
{
    for (size_t i = 0; i < override_priorities.size() && i < layers.size(); ++i) {
        layers[i].priority = override_priorities[i];
    }
    std::vector<PrioritisedLayer> result;
    if (layers.empty()) return result;
    result.push_back(layers.front());
    for (size_t i = 1; i < layers.size(); ++i) {
        const auto& layer = layers[i];
        if (layer.thickness == 0.0) continue;
        if (layer.priority == result.back().priority) {
            result.back().thickness += layer.thickness;
        } else {
            result.push_back(layer);
        }
    }
    return result;
}

std::vector<int> read_int_vector_attr(express::Base entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) return {};
    try {
        auto value = entity.get_attribute_value(static_cast<size_t>(idx));
        if (value.isNull()) return {};
        return static_cast<std::vector<int>>(value);
    } catch (...) {
        return {};
    }
}

std::vector<std::vector<double>> get_reference_line_with_fallback(
    ifcopenshell::file* file,
    express::Base wall,
    double fallback_length)
{
    if (auto axis = ifcapi::bindings::representation_get_product_representation(
        &wall, {{}, "Plan", "Axis", "GRAPH_VIEW"})) {
    if (auto resolved = ifcapi::bindings::representation_resolve(&axis)) {
            for (auto item : read_ref_list(resolved, "Items")) {
                std::vector<double> p0, p1;
                if (ifcapi::detail::read_curve_axis_points(item, p0, p1)) {
                    if (p0[0] < p1[0]) return {{p0[0], p0[1]}, {p1[0], p1[1]}};
                    return {{p1[0], p1[1]}, {p0[0], p0[1]}};
                }
            }
        }
    }
    auto definition = read_ref(wall, "Representation");
    for (auto representation : read_ref_list(definition, "Representations")) {
        for (auto item : read_ref_list(representation, "Items")) {
            std::vector<express::Base> candidates = {item};
            if (file && item) {
                auto traversed = file->traverse(item, -1);
                candidates.assign(traversed.begin(), traversed.end());
            }
            for (auto candidate : candidates) {
                if (!is_a(candidate, "IfcExtrudedAreaSolid")) continue;
                auto profile = read_ref(candidate, "SweptArea");
                auto curve = read_ref(profile, "OuterCurve");
                std::vector<double> x_values;
                if (is_a(curve, "IfcPolyline")) {
                    for (auto point : read_ref_list(curve, "Points")) {
                        auto coords = ifcapi::detail::read_double_aggregate(point, "Coordinates");
                        if (!coords.empty()) x_values.push_back(coords[0]);
                    }
                } else if (is_a(curve, "IfcIndexedPolyCurve")) {
                    auto points = read_ref(curve, "Points");
                    int idx = attr_index_of(points, "CoordList");
                    if (idx >= 0) {
                        try {
                            auto value = points.get_attribute_value(static_cast<size_t>(idx));
                            if (!value.isNull()) {
                                for (const auto& coords : static_cast<std::vector<std::vector<double>>>(value)) {
                                    if (!coords.empty()) x_values.push_back(coords[0]);
                                }
                            }
                        } catch (...) {
                        }
                    }
                }
                if (!x_values.empty()) {
                    auto bounds = std::minmax_element(x_values.begin(), x_values.end());
                    return {{*bounds.first, 0.0}, {*bounds.second, 0.0}};
                }
            }
        }
    }
    return {{0.0, 0.0}, {fallback_length, 0.0}};
}

struct WallRegenerator {
    ifcopenshell::file* file = {};
    express::Base body = {};
    express::Base axis = {};
    double unit_scale = 1.0;
    bool is_angled = false;
    double fallback_length = 1.0;
    double fallback_height = 1.0;
    bool has_fallback_angle = false;
    double fallback_angle = 0.0;
    std::vector<double> reference_p1;
    std::vector<double> reference_p2;
    WallVectors wall_vectors;
    std::vector<std::vector<double>> start_points;
    std::vector<double> start_vector = {0.0, 0.0, 1.0};
    double start_offset = 0.0;
    std::vector<std::pair<std::vector<double>, std::vector<std::vector<double>>>> atpath_points;
    std::vector<std::vector<std::vector<double>>> split_points;
    std::vector<std::vector<std::vector<double>>> maxpath_points;
    std::vector<std::vector<std::vector<double>>> minpath_points;
    std::vector<std::vector<double>> end_points;
    std::vector<double> end_vector = {0.0, 0.0, 1.0};
    double end_offset = 0.0;

    explicit WallRegenerator(ifcopenshell::file* f) : file(f) {
        body = ifcapi::bindings::representation_get_context(file, "Model", "Body", "MODEL_VIEW");
        axis = ifcapi::bindings::representation_get_context(file, "Plan", "Axis", "GRAPH_VIEW");
        unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        if (!axis) {
            auto plan = ifcapi::bindings::representation_get_context(file, "Plan", {}, {});
            if (!plan) {
                plan = ifcapi::bindings::context_add_context(file, ifcapi::bindings::ContextAddContextOptions{"Plan"});
            }
            axis = ifcapi::bindings::context_add_context(file, ifcapi::bindings::ContextAddContextOptions{"Plan", "Axis", "GRAPH_VIEW", {}, plan});
        }
    }

    WallVectors get_wall_vectors(express::Base wall) {
        if (auto body_rep = ifcapi::bindings::representation_get_product_representation(
                &wall, {{}, "Model", "Body", "MODEL_VIEW"})) {
            if (auto resolved = ifcapi::bindings::representation_resolve(&body_rep)) {
                for (auto item : read_ref_list(resolved, "Items")) {
                    while (is_a(item, "IfcBooleanResult")) {
                        item = read_ref(item, "FirstOperand");
                    }
                    if (!is_a(item, "IfcExtrudedAreaSolid")) continue;
                    auto z = ifcapi::detail::read_double_aggregate(read_ref(item, "ExtrudedDirection"), "DirectionRatios");
                    z = ifcapi::detail::np_normalized(z);
                    auto y = ifcapi::detail::vec_cross3(z, {1.0, 0.0, 0.0});
                    const double d = read_double(item, "Depth", fallback_height);
                    const double h = (z.size() > 2 ? z[2] : 0.0) * d;
                    const double a = ifcapi::detail::np_angle_signed({0.0, 1.0}, {z.size() > 1 ? z[1] : 0.0, z.size() > 2 ? z[2] : 0.0});
                    if (!ifcapi::detail::is_x(a, 0.0)) is_angled = true;
                    return {z, y, a, d, h};
                }
            }
        } else if (has_fallback_angle && fallback_angle != 0.0) {
            const double a = fallback_angle;
            std::vector<double> z = {0.0, std::sin(a), std::cos(a)};
            auto y = ifcapi::detail::vec_cross3(z, {1.0, 0.0, 0.0});
            const double h = fallback_height;
            const double d = ifcapi::detail::vec_norm(ifcapi::detail::vec_mul(z, h / z[2]));
            if (!ifcapi::detail::is_x(a, 0.0)) is_angled = true;
            return {z, y, a, d, h};
        }
        return {{0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, 0.0, fallback_height, fallback_height};
    }

    std::vector<double> get_join_vector(const std::vector<double>& y1, const std::vector<double>& y2) {
        auto result = ifcapi::detail::vec_cross3(y1, y2);
        if (result[2] < 0.0) result = ifcapi::detail::vec_mul(result, -1.0);
        return result;
    }

    std::vector<std::vector<std::vector<double>>> get_axes(
        express::Base wall,
        const std::vector<std::vector<double>>& reference,
        const std::vector<PrioritisedLayer>& layers,
        double angle)
    {
        std::vector<std::vector<std::vector<double>>> axes = {{reference[0], reference[1]}};
        int sense_factor = 1;
        auto usage = ifcapi::bindings::element_get_material(&wall, {false, false});
        if (usage && is_a(*usage, "IfcMaterialLayerSetUsage")) {
            const double offset = read_double(*usage, "OffsetFromReferenceLine", 0.0);
            for (auto& point : axes[0]) point[1] += offset;
            sense_factor = read_string(*usage, "DirectionSense") == "NEGATIVE" ? -1 : 1;
        }
        for (const auto& layer : layers) {
            const double y_offset = (layer.thickness * sense_factor) / std::cos(angle);
            axes.push_back({add2(axes.back()[0], {0.0, y_offset}), add2(axes.back()[1], {0.0, y_offset})});
        }
        return axes;
    }

    std::vector<express::Base> get_manual_booleans(express::Base element) {
        try {
            auto pset = find_bbim_boolean_pset(element);
            if (!pset) return {};
            std::vector<express::Base> result;
            for (int id : parse_json_int_array(bbim_boolean_data(pset))) {
                if (auto boolean = file->instance_by_id(static_cast<unsigned>(id))) result.push_back(boolean);
            }
            return result;
        } catch (...) {
            return {};
        }
    }

    void join(
        express::Base wall1,
        express::Base wall2,
        std::vector<PrioritisedLayer> layers1,
        std::vector<PrioritisedLayer> layers2,
        const std::string& connection1,
        const std::string& connection2)
    {
        if (connection1 == "NOTDEFINED" || connection2 == "NOTDEFINED") return;
        if (connection1 == "ATPATH" && connection2 == "ATPATH") return;

        auto reference1 = get_reference_line_with_fallback(file, wall1, fallback_length);
        auto reference2 = get_reference_line_with_fallback(file, wall2, fallback_length);
        auto wall_vectors2 = get_wall_vectors(wall2);
        auto axes1 = get_axes(wall1, reference1, layers1, wall_vectors.a);
        auto axes2 = get_axes(wall2, reference2, layers2, wall_vectors2.a);
        auto placement1 = read_ref(wall1, "ObjectPlacement");
        auto placement2 = read_ref(wall2, "ObjectPlacement");
        auto matrix1i = ifcapi::detail::invert_rigid4(ifcapi::bindings::placement_get_local_placement(placement1));
        auto matrix2 = ifcapi::bindings::placement_get_local_placement(placement2);
        auto transform = ifcapi::detail::matmul4(matrix1i, matrix2);

        for (auto& axis_pair : axes2) {
            axis_pair[0] = ifcapi::detail::transform_point_2d(transform, axis_pair[0]);
            axis_pair[1] = ifcapi::detail::transform_point_2d(transform, axis_pair[1]);
        }
        reference2[0] = ifcapi::detail::transform_point_2d(transform, reference2[0]);
        reference2[1] = ifcapi::detail::transform_point_2d(transform, reference2[1]);
        wall_vectors2.z = ifcapi::detail::transform_vector_3d(transform, wall_vectors2.z);
        wall_vectors2.y = ifcapi::detail::transform_vector_3d(transform, wall_vectors2.y);

        auto axis2 = axes2[0];
        if (ifcapi::detail::is_x(axis2[0][1], axis2[1][1])) return;

        if (connection1 == "ATEND") {
            if (axes2[0][0][0] > axes2.back()[0][0]) {
                std::reverse(axes2.begin(), axes2.end());
                std::reverse(layers2.begin(), layers2.end());
            }
        } else if (connection1 == "ATSTART") {
            if (axes2.back()[0][0] > axes2[0][0][0]) {
                std::reverse(axes2.begin(), axes2.end());
                std::reverse(layers2.begin(), layers2.end());
            }
        }

        axis2 = axes2[0];
        if (connection2 == "ATSTART") std::swap(axis2[0], axis2[1]);
        if (axis2[0][1] < axis2[1][1]) {
            if (axes1.back()[0][1] < axes1[0][0][1]) {
                std::reverse(axes1.begin(), axes1.end());
                std::reverse(layers1.begin(), layers1.end());
            }
        } else {
            if (axes1[0][0][1] < axes1.back()[0][1]) {
                std::reverse(axes1.begin(), axes1.end());
                std::reverse(layers1.begin(), layers1.end());
            }
        }

        if (connection1 == "ATPATH") {
            const auto first_axis2 = axes2.front();
            const auto last_axis2 = axes2.back();
            const double first_y = axes1[0][0][1];
            const double last_y = axes1.back()[0][1];
            std::vector<double> p0 = {intersect_x_or_throw(first_axis2[0], first_axis2[1], first_y), first_y};
            std::vector<double> pN = {intersect_x_or_throw(last_axis2[0], last_axis2[1], first_y), first_y};
            std::vector<std::vector<double>> points = {p0};
            size_t axis_index = 0;
            for (const auto& layer2 : layers2) {
                size_t y_index = 0;
                double y = axes1[y_index++][0][1];
                for (const auto& layer1 : layers1) {
                    if (layer2.priority <= layer1.priority) break;
                    y = axes1[y_index++][0][1];
                }
                std::vector<double> p1 = {intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y), y};
                ++axis_index;
                std::vector<double> p2 = {intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y), y};
                if (!points.empty() && allclose(points.back(), p1)) points.back() = p2;
                else {
                    points.push_back(p1);
                    points.push_back(p2);
                }
            }
            if (!allclose(points.back(), pN)) points.push_back(pN);

            const double split_a = first_y;
            const double split_b = last_y;
            std::vector<std::vector<double>> segment;
            auto atpath_vector = get_join_vector(wall_vectors.y, wall_vectors2.y);
            atpath_points.push_back({atpath_vector, points});
            for (const auto& point : points) {
                segment.push_back(point);
                if (segment.size() == 1) continue;
                if ((segment.front()[1] == split_a && segment.back()[1] == split_b) ||
                    (segment.front()[1] == split_b && segment.back()[1] == split_a)) {
                    if (segment.front()[1] > segment.back()[1]) std::reverse(segment.begin(), segment.end());
                    split_points.push_back(segment);
                    segment.clear();
                } else if (segment.front()[1] == segment.back()[1]) {
                    if (segment.front()[1] == maxy()) {
                        if (segment.front()[0] > segment.back()[0]) std::reverse(segment.begin(), segment.end());
                        maxpath_points.push_back(segment);
                    } else if (segment.front()[1] == miny()) {
                        if (segment.back()[0] > segment.front()[0]) std::reverse(segment.begin(), segment.end());
                        minpath_points.push_back(segment);
                    }
                    segment.clear();
                }
            }
        } else if (connection2 == "ATPATH") {
            std::vector<std::vector<double>> points;
            size_t y_index = 0;
            double y = axes1[y_index++][0][1];
            for (const auto& layer1 : layers1) {
                size_t axis_index = 0;
                for (const auto& layer2 : layers2) {
                    if (layer1.priority <= layer2.priority) break;
                    ++axis_index;
                }
                double x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
                std::vector<double> p1 = {x, y};
                y = axes1[y_index++][0][1];
                x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
                std::vector<double> p2 = {x, y};
                if (!points.empty() && allclose(points.back(), p1)) points.push_back(p2);
                else {
                    points.push_back(p1);
                    points.push_back(p2);
                }
            }
            apply_terminal_join(connection1, points, reference1, reference2, wall_vectors2);
        } else {
            const double last_y = axes1.back()[0][1];
            const auto last_axis2 = axes2.back();
            size_t axis_index = 0;
            size_t y_index = 0;
            double y = axes1[y_index++][0][1];
            double x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
            std::vector<std::vector<double>> points = {{x, y}};
            size_t layer1_index = 0;
            size_t layer2_index = 0;
            while (layer1_index < layers1.size() && layer2_index < layers2.size()) {
                if (layers1[layer1_index].priority > layers2[layer2_index].priority) {
                    ++axis_index;
                    x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
                    ++layer2_index;
                } else if (layers2[layer2_index].priority > layers1[layer1_index].priority) {
                    y = axes1[y_index++][0][1];
                    x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
                    ++layer1_index;
                } else {
                    y = axes1[y_index++][0][1];
                    ++axis_index;
                    x = intersect_x_or_throw(axes2[axis_index][0], axes2[axis_index][1], y);
                    ++layer1_index;
                    ++layer2_index;
                }
                points.push_back({x, y});
            }
            if (points.back()[1] != last_y) {
                points.push_back({intersect_x_or_throw(last_axis2[0], last_axis2[1], last_y), last_y});
            }
            apply_terminal_join(connection1, points, reference1, reference2, wall_vectors2);
        }
    }

    double miny() const { return axes_miny; }
    double maxy() const { return axes_maxy; }

    void apply_terminal_join(
        const std::string& connection1,
        const std::vector<std::vector<double>>& points,
        const std::vector<std::vector<double>>& reference1,
        const std::vector<std::vector<double>>& reference2,
        const WallVectors& wall_vectors2)
    {
        if (connection1 == "ATSTART") {
            start_points = points;
            start_vector = get_join_vector(wall_vectors.y, wall_vectors2.y);
            start_offset = ifcapi::detail::vec_mul(start_vector, wall_vectors.h / start_vector[2])[0];
            reference_p1[0] = intersect_x_or_throw(reference2[0], reference2[1], reference1[0][1]);
        } else if (connection1 == "ATEND") {
            end_points = points;
            end_vector = get_join_vector(wall_vectors.y, wall_vectors2.y);
            end_offset = ifcapi::detail::vec_mul(end_vector, wall_vectors.h / end_vector[2])[0];
            reference_p2[0] = intersect_x_or_throw(reference2[0], reference2[1], reference1[0][1]);
        }
    }

    double axes_miny = 0.0;
    double axes_maxy = 0.0;

    express::Base polyline(const std::vector<std::vector<double>>& points, bool closed, bool has_offset) {
        return ifcapi::bindings::shape_builder_polyline(
            file,
            ifcapi::bindings::ShapeBuilderPolylineOptions{
                points, closed, has_offset ? std::optional<std::vector<double>>(mul2(reference_p1, -1.0)) : std::nullopt, {}});
    }

    express::Base profile_from_points(const std::vector<std::vector<double>>& points, bool has_offset) {
        auto outer_curve = polyline(points, true, has_offset);
        return ifcapi::bindings::shape_builder_profile(
            file,
            ifcapi::bindings::ShapeBuilderProfileOptions{outer_curve, {}, {}, "AREA"});
    }

    express::Base extrude(express::Base profile, double magnitude, const std::vector<double>& vector) {
        return ifcapi::bindings::shape_builder_extrude(
            file,
            ifcapi::bindings::ShapeBuilderExtrudeOptions{profile, magnitude, {}, vector, vector, {1.0, 0.0, 0.0}, {}});
    }

    express::Base regenerate(express::Base wall, double length, double height, bool has_angle, double angle) {
        fallback_length = length / unit_scale;
        fallback_height = height / unit_scale;
        has_fallback_angle = has_angle;
        fallback_angle = angle;
        auto layers = get_wall_layers(wall);
        if (layers.empty()) return {};
        auto reference = get_reference_line_with_fallback(file, wall, fallback_length);
        reference_p1 = reference[0];
        reference_p2 = reference[1];
        wall_vectors = get_wall_vectors(wall);
        auto axes = get_axes(wall, reference, layers, wall_vectors.a);
        axes_miny = axes[0][0][1];
        axes_maxy = axes.back()[0][1];

        auto manual_booleans = get_manual_booleans(wall);
        for (auto rel : inverse_refs(wall, "ConnectedTo")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) continue;
            auto wall2 = read_ref(rel, "RelatedElement");
            auto layers1 = combine_wall_layers(layers, read_int_vector_attr(rel, "RelatingPriorities"));
            auto layers2 = combine_wall_layers(get_wall_layers(wall2), read_int_vector_attr(rel, "RelatedPriorities"));
            if (!layers1.empty() && !layers2.empty()) join(wall, wall2, layers1, layers2, read_string(rel, "RelatingConnectionType"), read_string(rel, "RelatedConnectionType"));
        }
        for (auto rel : inverse_refs(wall, "ConnectedFrom")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) continue;
            auto wall2 = read_ref(rel, "RelatingElement");
            auto layers1 = combine_wall_layers(layers, read_int_vector_attr(rel, "RelatedPriorities"));
            auto layers2 = combine_wall_layers(get_wall_layers(wall2), read_int_vector_attr(rel, "RelatingPriorities"));
            if (!layers1.empty() && !layers2.empty()) join(wall, wall2, layers1, layers2, read_string(rel, "RelatedConnectionType"), read_string(rel, "RelatingConnectionType"));
        }

        const double miny_value = axes[axes.size() - 2][0][1];
        const double maxy_value = axes.back()[0][1];
        if (start_points.empty()) {
            const double minx = axes[0][0][0];
            start_points = {{minx, axes[0][0][1]}, {minx, axes.back()[0][1]}};
        }
        if (end_points.empty()) {
            const double maxx = axes[0][1][0];
            end_points = {{maxx, axes[0][0][1]}, {maxx, axes.back()[0][1]}};
        }
        if (start_points.front()[1] > start_points.back()[1]) std::reverse(start_points.begin(), start_points.end());
        if (end_points.front()[1] > end_points.back()[1]) std::reverse(end_points.begin(), end_points.end());

        const bool has_offset = manual_booleans.empty();
        express::Base item = {};
        if (is_angled) {
            auto start = start_points;
            auto end = end_points;
            if (end_offset > 0.0) for (auto& point : end) point[0] += end_offset;
            if (start_offset < 0.0) for (auto& point : start) point[0] += start_offset;
            std::vector<std::vector<double>> points = start;
            std::reverse(end.begin(), end.end());
            points.insert(points.end(), end.begin(), end.end());
            item = extrude(polyline(points, true, has_offset), wall_vectors.d, wall_vectors.z);

            std::vector<express::Base> operands;
            add_angled_operand(operands, start_points, start_vector, start_offset, true, has_offset);
            add_angled_operand(operands, end_points, end_vector, end_offset, false, has_offset);
            for (const auto& atpath : atpath_points) {
                if (atpath.second.size() <= 2) continue;
                const double magnitude = ifcapi::detail::vec_norm(ifcapi::detail::vec_mul(atpath.first, wall_vectors.h / atpath.first[2]));
                operands.push_back(extrude(polyline(atpath.second, true, has_offset), magnitude, atpath.first));
            }
            if (!operands.empty()) {
                auto booleans = ifcapi::bindings::geometry_add_boolean(file, &item, operands, "DIFFERENCE");
                if (!booleans.empty()) item = booleans.back();
            }
        } else {
            std::vector<express::Base> profiles;
            const double minx = std::max_element(start_points.begin(), start_points.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; })->at(0);
            const double maxx = std::min_element(end_points.begin(), end_points.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; })->at(0);
            std::vector<std::vector<std::vector<double>>> ordered_splits;
            std::sort(split_points.begin(), split_points.end(), [](const auto& a, const auto& b) { return a[0][0] < b[0][0]; });
            for (const auto& points : split_points) {
                bool outside = false;
                for (const auto& point : points) {
                    if (point[0] > maxx || point[0] < minx) outside = true;
                }
                if (!outside) ordered_splits.push_back(points);
            }
            ordered_splits.insert(ordered_splits.begin(), start_points);
            ordered_splits.push_back(end_points);
            if (maxy_value < miny_value) {
                std::swap(maxpath_points, minpath_points);
                if (!maxpath_points.empty()) std::reverse(maxpath_points[0].begin(), maxpath_points[0].end());
                if (!minpath_points.empty()) std::reverse(minpath_points[0].begin(), minpath_points[0].end());
            }
            for (size_t i = 0; i + 1 < ordered_splits.size(); i += 2) {
                auto points = ordered_splits[i];
                const auto& end_split = ordered_splits[i + 1];
                const double maxy_minx = points.back()[0];
                const double maxy_maxx = end_split.back()[0];
                const double miny_minx = points.front()[0];
                const double miny_maxx = end_split.front()[0];
                std::vector<std::vector<std::vector<double>>> remaining;
                for (const auto& maxpath : maxpath_points) {
                    if (maxpath.front()[0] > maxy_minx && maxpath.back()[0] < maxy_maxx) points.insert(points.end(), maxpath.begin(), maxpath.end());
                    else remaining.push_back(maxpath);
                }
                maxpath_points = remaining;
                points.insert(points.end(), end_split.rbegin(), end_split.rend());
                remaining.clear();
                for (const auto& minpath : minpath_points) {
                    if (minpath.front()[0] < miny_maxx && minpath.back()[0] > miny_minx) points.insert(points.end(), minpath.begin(), minpath.end());
                    else remaining.push_back(minpath);
                }
                minpath_points = remaining;
                profiles.push_back(profile_from_points(points, has_offset));
            }
            for (const auto& points : maxpath_points) profiles.push_back(profile_from_points(points, has_offset));
            for (const auto& points : minpath_points) profiles.push_back(profile_from_points(points, has_offset));
            express::Base profile = {};
            if (profiles.size() > 1) {
                profile = file->create(file->schema()->declaration_by_name("IfcCompositeProfileDef"));
                write_string(profile, "ProfileType", "AREA");
                write_ref_list(profile, "Profiles", profiles);
            } else {
                if (profiles.empty()) throw std::runtime_error("Unable to build wall profile");
                profile = profiles.front();
            }
            item = extrude(profile, wall_vectors.d, wall_vectors.z);
        }

        for (auto boolean : get_manual_booleans(wall)) {
            write_ref(boolean, "FirstOperand", item);
            item = boolean;
        }

        auto body_rep = ifcapi::bindings::shape_builder_representation(
            file,
            ifcapi::bindings::ShapeBuilderRepresentationOptions{body, {item}, {}});
        if (auto old_rep = ifcapi::bindings::representation_get_product_representation(&wall, {body, {}, {}, {}})) {
            ifcapi::bindings::element_replace_element(&old_rep, &body_rep);
            ifcapi::bindings::entity_remove_deep(&old_rep);
        } else {
            ifcapi::bindings::geometry_assign_representation(file, &wall, &body_rep);
        }

        auto axis_curve = ifcapi::bindings::shape_builder_polyline(
            file,
            ifcapi::bindings::ShapeBuilderPolylineOptions{
                {reference_p1, reference_p2},
                false,
                has_offset ? std::optional<std::vector<double>>(mul2(reference_p1, -1.0)) : std::nullopt,
                {}});
        auto axis_rep = ifcapi::bindings::shape_builder_representation(
            file,
            ifcapi::bindings::ShapeBuilderRepresentationOptions{axis, {axis_curve}, {}});
        if (auto old_rep = ifcapi::bindings::representation_get_product_representation(&wall, {axis, {}, {}, {}})) {
            ifcapi::bindings::element_replace_element(&old_rep, &axis_rep);
            ifcapi::bindings::entity_remove_deep(&old_rep);
        } else {
            ifcapi::bindings::geometry_assign_representation(file, &wall, &axis_rep);
        }

        if (!allclose(reference_p1, {0.0, 0.0}) && manual_booleans.empty()) {
            restore_wall_placement(wall);
        }

        return body_rep;
    }

    void add_angled_operand(
        std::vector<express::Base>& operands,
        std::vector<std::vector<double>> points,
        const std::vector<double>& vector,
        double offset,
        bool is_start,
        bool has_offset)
    {
        if (allclose(vector, {0.0, 0.0, 1.0})) return;
        while (points.size() > 1 && ifcapi::detail::is_x(points[0][1], points[1][1])) points.erase(points.begin());
        while (points.size() > 1 && ifcapi::detail::is_x(points.back()[1], points[points.size() - 2][1])) points.pop_back();
        const auto bounds = std::minmax_element(points.begin(), points.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; });
        const double newx = is_start ? bounds.first->at(0) - std::fabs(offset) : bounds.second->at(0) + std::fabs(offset);
        auto p1 = points.back();
        p1[0] = newx;
        auto p2 = p1;
        p2[1] = points.front()[1];
        points.push_back(p1);
        points.push_back(p2);
        const double magnitude = ifcapi::detail::vec_norm(ifcapi::detail::vec_mul(vector, wall_vectors.h / vector[2]));
        operands.push_back(extrude(polyline(points, true, has_offset), magnitude, vector));
    }

    void restore_wall_placement(express::Base wall) {
        struct ChildPlacement {
            std::vector<double> matrix;
            std::vector<express::Base> elements;
        };
        std::vector<ChildPlacement> children;
        auto placement = read_ref(wall, "ObjectPlacement");
        for (auto referenced_placement : inverse_refs(placement, "ReferencedByPlacements")) {
            children.push_back({
                ifcapi::bindings::placement_get_local_placement(referenced_placement),
                inverse_refs(referenced_placement, "PlacesObject"),
            });
        }
        auto matrix = ifcapi::bindings::placement_get_local_placement(placement);
        const double x = reference_p1[0];
        const double y = reference_p1[1];
        matrix[3] = matrix[0] * x + matrix[1] * y + matrix[3];
        matrix[7] = matrix[4] * x + matrix[5] * y + matrix[7];
        matrix[11] = matrix[8] * x + matrix[9] * y + matrix[11];
        ifcapi::bindings::geometry_edit_object_placement(
            file,
            ifcapi::bindings::GeometryEditObjectPlacementOptions{wall, matrix, false, true});
        for (const auto& child : children) {
            for (auto element : child.elements) {
                ifcapi::bindings::geometry_edit_object_placement(
                    file,
                    ifcapi::bindings::GeometryEditObjectPlacementOptions{element, child.matrix, false, true});
            }
        }
    }
};

void register_bbim_boolean(
    ifcopenshell::file* file,
    express::Base element,
    express::Base result,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    if (!element) return;
    auto pset = find_bbim_boolean_pset(element);
    std::vector<int> ids;
    if (pset) {
        ids = parse_json_int_array(bbim_boolean_data(pset));
    } else {
        pset = ifcapi::bindings::pset_add_pset(
            file,
            ifcapi::bindings::PsetAddPsetOptions{
                element,
                "BBIM_Boolean",
                owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt,
                {},
            });
        if (!pset) throw std::runtime_error("Unable to create BBIM_Boolean pset");
    }
    int result_id = static_cast<int>(result.id());
    if (std::find(ids.begin(), ids.end(), result_id) == ids.end()) {
        ids.push_back(result_id);
    }

    auto props = ifcapi::bindings::pset_props_new();
    if (!props) throw std::runtime_error("Unable to create pset property container");
    try {
        ifcapi::bindings::pset_props_set_string(props, "Data", dump_json_int_array(ids));
        if (!ifcapi::bindings::pset_edit_pset(file, ifcapi::bindings::PsetEditPsetOptions{pset, {}, props, {}, true})) {
            throw std::runtime_error("pset_edit_pset failed");
        }
        ifcapi::bindings::pset_props_free(props);
    } catch (...) {
        ifcapi::bindings::pset_props_free(props);
        throw;
    }
}

express::Base make_cartesian_transformation_op_3d(ifcopenshell::file* file) {
    auto decl = file->schema()->declaration_by_name("IfcCartesianTransformationOperator3D");
    auto p = file->create(decl);
    write_ref(p, "Axis1",       make_direction(file, 1.0, 0.0, 0.0));
    write_ref(p, "Axis2",       make_direction(file, 0.0, 1.0, 0.0));
    write_ref(p, "LocalOrigin", make_cartesian_point(file, 0.0, 0.0, 0.0));
    write_int(p, "Scale",       1);
    write_ref(p, "Axis3",       make_direction(file, 0.0, 0.0, 1.0));
    return p;
}

express::Base make_representation_map(ifcopenshell::file* file,
                                               express::Base mapped_rep) {
    auto decl = file->schema()->declaration_by_name("IfcRepresentationMap");
    auto p = file->create(decl);
    write_ref(p, "MappingOrigin",       make_axis2_placement_3d(file));
    write_ref(p, "MappedRepresentation", mapped_rep);
    return p;
}

express::Base make_mapped_item(ifcopenshell::file* file,
                                        express::Base source,
                                        express::Base target) {
    auto decl = file->schema()->declaration_by_name("IfcMappedItem");
    auto p = file->create(decl);
    write_ref(p, "MappingSource", source);
    write_ref(p, "MappingTarget", target);
    return p;
}

express::Base find_existing_representation_map(ifcopenshell::file* file,
                                                       express::Base representation) {
    auto inverses = file->instances_by_reference(static_cast<int>(representation.id()));
    for (auto inv : inverses) {
        if (is_a(inv, "IfcRepresentationMap")) return inv;
    }
    return {};
}

// ---- copy_deep ----------------------------------------------------------

bool excluded(express::Base e, const std::vector<std::string>& exclude) {
    if (!e) return false;
    for (const auto& name : exclude) {
        if (e.declaration().is(name)) return true;
    }
    return false;
}

express::Base deep_copy_entity(ifcopenshell::file* file,
                                       express::Base element,
                                       const std::vector<std::string>& exclude,
                                       std::unordered_map<unsigned, express::Base>& memo) {
    if (!element) return {};
    auto id = element.id();
    if (id) {
        auto it = memo.find(id);
        if (it != memo.end()) return it->second;
    }

    auto be = as_entity(element);
    if (!be) return element; // simple types: return as-is

    const auto decl = be.declaration().as_entity();
    if (!decl) return element;

    auto dst = file->create(decl);
    if (id) memo[id] = dst;

    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto v = element.get_attribute_value(i);
            if (v.isNull()) continue;
            auto t = v.type();

            // Replace GlobalIds with fresh GUIDs.
            if (attrs[i]->name() == "GlobalId") {
                dst.set_attribute_value(i, ifcapi::guid_new());
                continue;
            }

            switch (t) {
                case ifcopenshell::Argument_INT:
                    dst.set_attribute_value(i, (int)v); break;
                case ifcopenshell::Argument_BOOL:
                    dst.set_attribute_value(i, (bool)v); break;
                case ifcopenshell::Argument_LOGICAL:
                    dst.set_attribute_value(i, (boost::logic::tribool)v); break;
                case ifcopenshell::Argument_DOUBLE:
                    dst.set_attribute_value(i, (double)v); break;
                case ifcopenshell::Argument_STRING:
                    dst.set_attribute_value(i, (std::string)v); break;
                case ifcopenshell::Argument_BINARY:
                    dst.set_attribute_value(i, (boost::dynamic_bitset<>)v); break;
                case ifcopenshell::Argument_ENUMERATION:
                    dst.set_attribute_value(i, (std::string)v); break;
                case ifcopenshell::Argument_ENTITY_INSTANCE: {
                    auto ref = (express::Base)v;
                    if (excluded(ref, exclude)) {
                        dst.set_attribute_value(i, ref);
                    } else {
                        dst.set_attribute_value(i, deep_copy_entity(file, ref, exclude, memo));
                    }
                    break;
                }
                case ifcopenshell::Argument_AGGREGATE_OF_INT:
                    dst.set_attribute_value(i, (std::vector<int>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE:
                    dst.set_attribute_value(i, (std::vector<double>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_STRING:
                    dst.set_attribute_value(i, (std::vector<std::string>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_BINARY:
                    dst.set_attribute_value(i, (std::vector<boost::dynamic_bitset<>>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto agg = (std::vector<express::Base>)v;
                    std::vector<express::Base> out;
                    for (auto item : agg) {
                        if (excluded(item, exclude)) {
                            out.push_back(item);
                        } else {
                            out.push_back(deep_copy_entity(file, item, exclude, memo));
                        }
                    }
                    dst.set_attribute_value(i, out);
                    break;
                }
                case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
                    dst.set_attribute_value(i, (std::vector<std::vector<int>>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
                    dst.set_attribute_value(i, (std::vector<std::vector<double>>)v); break;
                case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto agg = (std::vector<std::vector<express::Base>>)v;
                    std::vector<std::vector<express::Base>> out;
                    for (const auto& outer : agg) {
                        std::vector<express::Base> inner;
                        for (auto item : outer) {
                            if (excluded(item, exclude)) inner.push_back(item);
                            else inner.push_back(deep_copy_entity(file, item, exclude, memo));
                        }
                        out.push_back(inner);
                    }
                    dst.set_attribute_value(i, out);
                    break;
                }
                default:
                    break;
            }
        } catch (...) {
            // skip unsupported attribute
        }
    }
    return dst;
}

// ---- get_representation_for_product ------------------------------------

express::Base get_representation_for_product(express::Base product,
                                                     const std::string& context_identifier) {
    if (!product) return {};
    if (is_a(product, "IfcProduct")) {
        auto def = read_ref(product, "Representation");
        if (!def) return {};
        for (auto rep : read_ref_list(def, "Representations")) {
            if (read_string(rep, "RepresentationIdentifier") == context_identifier) {
                return rep;
            }
        }
    } else if (is_a(product, "IfcTypeProduct")) {
        for (auto m : read_ref_list(product, "RepresentationMaps")) {
            auto mapped = read_ref(m, "MappedRepresentation");
            if (mapped && read_string(mapped, "RepresentationIdentifier") == context_identifier) {
                return mapped;
            }
        }
    }
    return {};
}

// ---- representation removal --------------------------------------------

// Simplified port of ifcopenshell.api.geometry.remove_representation.
// Walks the representation graph, and for each visited entity that has no
// remaining inverses (after the rep is removed) deletes it.  Excludes
// IfcGeometricRepresentationContext from deletion.
void remove_representation_simple(ifcopenshell::file* file,
                                  express::Base representation) {
    if (!representation) return;
    auto subs = file->traverse(representation, -1);
    if (subs.empty()) {
        file->remove_entity(representation);
        return;
    }

    // Collect every entity to potentially delete, excluding contexts.
    std::vector<express::Base> candidates;
    candidates.reserve(subs.size());
    for (auto s : subs) {
        if (!s) continue;
        if (is_a(s, "IfcGeometricRepresentationContext")) continue;
        candidates.push_back(s);
    }

    std::set<unsigned> deletable;
    for (auto c : candidates) {
        if (c.id() != 0) deletable.insert(c.id());
    }

    // Only delete a node if every inverse is also deleted. If a candidate is
    // retained because of an external inverse, anything it references must be
    // retained too.
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto e : candidates) {
            unsigned id = e.id();
            if (id == 0 || deletable.find(id) == deletable.end()) continue;
            auto invs = file->instances_by_reference(static_cast<int>(id));
            for (auto inv : invs) {
                if (!inv) continue;
                if (deletable.find(inv.id()) == deletable.end()) {
                    deletable.erase(id);
                    changed = true;
                    break;
                }
            }
        }
    }

    for (auto e : candidates) {
        unsigned id = e.id();
        if (id == 0 || deletable.find(id) == deletable.end()) continue;
        if (file->instance_by_id(id)) file->remove_entity(e);
    }
}

void unassign_product_representation(ifcopenshell::file* file,
                                     express::Base product,
                                     express::Base representation) {
    auto def = read_ref(product, "Representation");
    if (!def) return;
    auto reps = read_ref_list(def, "Representations");
    auto found = std::find(reps.begin(), reps.end(), representation);
    if (found == reps.end()) return;
    reps.erase(found);
    if (reps.empty()) {
        if (auto be = as_entity(def)) {
            try {
                auto aspects = be.get_inverse("HasShapeAspects");
                std::vector<express::Base> shape_aspects;
                for (auto aspect : aspects) {
                    if (aspect) {
                        shape_aspects.push_back(aspect);
                    }
                }
                for (auto aspect : shape_aspects) {
                    auto shape_reps = read_ref_list(aspect, "ShapeRepresentations");
                    file->remove_entity(aspect);
                    for (auto shape_rep : shape_reps) {
                        remove_representation_simple(file, shape_rep);
                    }
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to process shape aspects: ") + e.what());
            }
        }
        write_ref(product, "Representation", {});
        file->remove_entity(def);
    } else {
        write_ref_list(def, "Representations", reps);
    }
}

// ---- map_representation -------------------------------------------------

express::Base map_representation_impl(ifcopenshell::file* file,
                                               express::Base representation) {
    auto mapping_source = find_existing_representation_map(file, representation);
    if (!mapping_source) {
        mapping_source = make_representation_map(file, representation);
    }
    auto mapping_target = make_cartesian_transformation_op_3d(file);
    auto mapped_item = make_mapped_item(file, mapping_source, mapping_target);

    auto sr_decl = file->schema()->declaration_by_name("IfcShapeRepresentation");
    auto sr = file->create(sr_decl);
    write_ref(sr, "ContextOfItems", read_ref(representation, "ContextOfItems"));
    auto rid = read_string(representation, "RepresentationIdentifier");
    if (!rid.empty()) write_string(sr, "RepresentationIdentifier", rid);
    write_string(sr, "RepresentationType", "MappedRepresentation");
    write_ref_list(sr, "Items", {mapped_item});
    return sr;
}

// ---- assign_representation ---------------------------------------------

// Get the type of a product (IFC4: IsTypedBy, IFC2X3: IsDefinedBy).
express::Base get_product_type(ifcopenshell::file* file,
                                        express::Base product) {
    auto be = as_entity(product);
    if (!be) return {};
    try {
        auto invs = be.get_inverse("IsTypedBy");
        if (!invs.empty()) {
            auto rel = invs[0];
            return read_ref(rel, "RelatingType");
        }
    } catch (...) {}
    try {
        auto invs = be.get_inverse("IsDefinedBy");
        const auto rdt = file->schema()->declaration_by_name("IfcRelDefinesByType");
        for (auto inv : invs) {
            if (inv.declaration().is(*rdt)) {
                return read_ref(inv, "RelatingType");
            }
        }
    } catch (...) {}
    return {};
}

// Get the material association of an element (IfcRelAssociatesMaterial.RelatingMaterial).
express::Base get_material_simple(express::Base element) {
    auto be = as_entity(element);
    if (!be) return {};
    try {
        auto invs = be.get_inverse("HasAssociations");
        for (auto inv : invs) {
            if (is_a(inv, "IfcRelAssociatesMaterial")) {
                return read_ref(inv, "RelatingMaterial");
            }
        }
    } catch (...) {}
    return {};
}

void assign_product_representation(ifcopenshell::file* file,
                                   express::Base product,
                                   express::Base representation) {
    auto def = read_ref(product, "Representation");
    if (!def) {
        auto pds_decl = file->schema()->declaration_by_name("IfcProductDefinitionShape");
        def = file->create(pds_decl);
        write_ref(product, "Representation", def);
    }
    auto reps = read_ref_list(def, "Representations");
    reps.push_back(representation);
    write_ref_list(def, "Representations", reps);
}

void assign_representation_impl(ifcopenshell::file* file,
                                express::Base& product /*may be re-routed*/,
                                express::Base representation) {
    if (is_a(product, "IfcProduct")) {
        auto product_type = get_product_type(file, product);
        if (product_type) {
            auto maps = read_ref_list(product_type, "RepresentationMaps");
            std::string rep_type = read_string(representation, "RepresentationType");
            if (!maps.empty() && rep_type != "MappedRepresentation") {
                // Skip re-routing for profile/layer-based types.
                auto mat = get_material_simple(product_type);
                bool is_profile_or_layer =
                    mat && (is_a(mat, "IfcMaterialProfileSet") ||
                            is_a(mat, "IfcMaterialLayerSet"));
                if (!is_profile_or_layer) {
                    product = product_type;
                }
            }
        }
    }

    if (is_a(product, "IfcProduct")) {
        assign_product_representation(file, product, representation);
    } else if (is_a(product, "IfcTypeProduct")) {
        auto maps = read_ref_list(product, "RepresentationMaps");
        maps.push_back(make_representation_map(file, representation));
        write_ref_list(product, "RepresentationMaps", maps);

        // Resolve related occurrences for the type and assign mapped reps.
        auto be = as_entity(product);
        if (be) {
            std::vector<express::Base> related_objects;
            // IFC4+: Types ; IFC2X3: ObjectTypeOf
            for (const char* inv_name : {"Types", "ObjectTypeOf"}) {
                try {
                    auto invs = be.get_inverse(inv_name);
                    if (!invs.empty()) {
                        for (auto obj : read_ref_list(invs[0], "RelatedObjects")) {
                            related_objects.push_back(obj);
                        }
                        break;
                    }
                } catch (...) {}
            }
            for (auto element : related_objects) {
                auto mapped = map_representation_impl(file, representation);
                assign_product_representation(file, element, mapped);
            }
        }
    }
    // OwnerHistory updating is handled by the caller / Python wrapper.
}

}  // namespace

namespace ifcapi {
namespace bindings {

std::vector<express::Base> geometry_add_boolean(
    ifcopenshell::file* file,
    express::Base first_item,
    const std::vector<express::Base>& second_items,
    const std::string& operator_type)
{
    ifcopenshell_clear_error();
    std::vector<express::Base> booleans;
    if (!file || !is_boolean_operand(first_item)) {
        return booleans;
    }

    try {
        auto original_first_item = first_item;
        auto filtered_second_items = ifcapi::detail::to_mutable_refs(second_items);
        filtered_second_items.erase(
            std::remove_if(
                filtered_second_items.begin(),
                filtered_second_items.end(),
                [&](express::Base item) { return item == first_item || !is_boolean_operand(item); }),
            filtered_second_items.end());

        while (true) {
            bool is_part_of_boolean = false;
            for (auto inverse : all_inverse_refs(file, first_item)) {
                if (!is_a(inverse, "IfcBooleanResult")) {
                    continue;
                }
                is_part_of_boolean = true;
                first_item = inverse;
                auto first_operand = read_ref(inverse, "FirstOperand");
                auto second_operand = read_ref(inverse, "SecondOperand");
                if (first_operand == original_first_item) {
                    filtered_second_items.erase(
                        std::remove(filtered_second_items.begin(), filtered_second_items.end(), second_operand),
                        filtered_second_items.end());
                } else if (second_operand == original_first_item) {
                    filtered_second_items.erase(
                        std::remove(filtered_second_items.begin(), filtered_second_items.end(), first_operand),
                        filtered_second_items.end());
                }
                break;
            }
            if (!is_part_of_boolean) {
                break;
            }
        }

        if (filtered_second_items.empty()) {
            return booleans;
        }

        std::vector<express::Base> to_replace;
        for (auto inverse : all_inverse_refs(file, first_item)) {
            if (is_a(inverse, "IfcShapeRepresentation") || is_a(inverse, "IfcBooleanResult")) {
                if (!ifcapi::detail::contains_ref(to_replace, inverse)) {
                    to_replace.push_back(inverse);
                }
            }
        }

        auto first = first_item;
        for (auto second_item : filtered_second_items) {
            first = create_boolean_result(file, operator_type, first, second_item);
            booleans.push_back(first);
        }

        for (auto inverse : to_replace) {
            ifcapi::detail::replace_attribute_reference(inverse, first_item, first);
        }
        return booleans;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_axis_representation(
    ifcopenshell::file* file,
    express::Base context,
    const std::vector<std::vector<double>>& axis)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        size_t dimensions = axis.at(0).size();
        auto points = ifcapi::detail::convert_si_to_project_units(file, axis);
        auto curve = ifcapi::detail::create_polyline_or_indexed_polycurve(file, points, dimensions);
        auto representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        write_ref(representation, "ContextOfItems", context);
        write_string(representation, "RepresentationIdentifier", read_string(context, "ContextIdentifier"));
        write_string(representation, "RepresentationType", dimensions == 2 ? "Curve2D" : "Curve3D");
        write_ref_list(representation, "Items", {curve});
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_footprint_representation(
    ifcopenshell::file* file,
    express::Base context,
    const std::vector<express::Base>& curves)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto curve_set = file->create(file->schema()->declaration_by_name("IfcGeometricCurveSet"));
        write_ref_list(curve_set, "Elements", ifcapi::detail::to_mutable_refs(curves));

        auto representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        write_ref(representation, "ContextOfItems", context);
        write_string(representation, "RepresentationIdentifier", read_string(context, "ContextIdentifier"));
        write_string(representation, "RepresentationType", "GeometricCurveSet");
        write_ref_list(representation, "Items", {curve_set});
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_mesh_representation(
    ifcopenshell::file* file,
    express::Base context,
    const std::vector<std::vector<std::vector<double>>>& vertices,
    const std::vector<std::vector<std::vector<std::vector<int>>>>& faces,
    bool force_faceted_brep)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        if (vertices.size() != faces.size()) {
            throw std::invalid_argument("vertices and faces item counts must match");
        }

        const bool use_faceted_brep = force_faceted_brep || file->schema()->name() == "IFC2X3";
        std::vector<express::Base> items;
        items.reserve(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (use_faceted_brep) {
                std::vector<std::vector<int>> brep_faces;
                brep_faces.reserve(faces[i].size());
                for (const auto& face : faces[i]) {
                    if (face.size() != 1) {
                        throw std::invalid_argument("IfcFacetedBrep mesh faces cannot contain inner loops");
                    }
                    brep_faces.push_back(face.front());
                }
                items.push_back(shape_builder_faceted_brep(file, vertices[i], brep_faces));
            } else {
                items.push_back(shape_builder_polygonal_face_set(file, vertices[i], faces[i]));
            }
        }

        auto representation = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
        write_ref(representation, "ContextOfItems", context);
        ifcapi::detail::copy_string_attr_preserving_null(
            representation, "RepresentationIdentifier", context, "ContextIdentifier");
        write_string(representation, "RepresentationType", use_faceted_brep ? "Brep" : "Tessellation");
        write_ref_list(representation, "Items", items);
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_shape_aspect(
    ifcopenshell::file* file,
    const std::string& name,
    const std::vector<express::Base>& items,
    express::Base representation,
    express::Base part_of_product,
    const char* description,
    bool has_description)
{
    ifcopenshell_clear_error();
    if (!file || !representation || !part_of_product) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto shape_items = ifcapi::detail::to_mutable_refs(items);
        auto context = read_ref(representation, "ContextOfItems");
        express::Base shape_aspect = {};

        for (auto aspect : ifcapi::detail::read_inverse_aggregate(part_of_product, "HasShapeAspects")) {
            if (read_string(aspect, "Name") == name) {
                shape_aspect = aspect;
                break;
            }
        }

        if (shape_aspect) {
            write_optional_string(shape_aspect, "Description", description, has_description);
            bool has_context_representation = false;
            for (auto aspect_rep : read_ref_list(shape_aspect, "ShapeRepresentations")) {
                if (read_ref(aspect_rep, "ContextOfItems") == context) {
                    auto existing_items = read_ref_list(aspect_rep, "Items");
                    ifcapi::detail::append_unique(existing_items, shape_items);
                    write_ref_list(aspect_rep, "Items", existing_items);
                    has_context_representation = true;
                    break;
                }
            }
            if (!has_context_representation) {
                auto aspect_reps = read_ref_list(shape_aspect, "ShapeRepresentations");
                aspect_reps.push_back(create_shape_representation_like(file, representation, shape_items));
                write_ref_list(shape_aspect, "ShapeRepresentations", aspect_reps);
            }
        } else {
            auto aspect_rep = create_shape_representation_like(file, representation, shape_items);
            shape_aspect = file->create(file->schema()->declaration_by_name("IfcShapeAspect"));
            write_ref_list(shape_aspect, "ShapeRepresentations", {aspect_rep});
            write_string(shape_aspect, "Name", name);
            write_optional_string(shape_aspect, "Description", description, has_description);
            write_bool(shape_aspect, "ProductDefinitional", true);
            write_ref(shape_aspect, "PartOfProductDefinitionShape", part_of_product);
        }

        for (auto aspect : ifcapi::detail::read_inverse_aggregate(part_of_product, "HasShapeAspects")) {
            if (aspect == shape_aspect) {
                continue;
            }
            auto aspect_reps = read_ref_list(aspect, "ShapeRepresentations");
            for (auto it = aspect_reps.begin(); it != aspect_reps.end();) {
                auto aspect_rep = *it;
                if (read_ref(aspect_rep, "ContextOfItems") != context) {
                    ++it;
                    continue;
                }
                auto aspect_items = read_ref_list(aspect_rep, "Items");
                aspect_items.erase(
                    std::remove_if(
                        aspect_items.begin(),
                        aspect_items.end(),
                        [&](express::Base item) {
                            return ifcapi::detail::contains_ref(shape_items, item);
                        }),
                    aspect_items.end());
                if (aspect_items.empty()) {
                    file->remove_entity(aspect_rep);
                    it = aspect_reps.erase(it);
                } else {
                    write_ref_list(aspect_rep, "Items", aspect_items);
                    ++it;
                }
            }
            write_ref_list(aspect, "ShapeRepresentations", aspect_reps);
            if (aspect_reps.empty()) {
                file->remove_entity(aspect);
            }
        }

        return shape_aspect;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_topology_representation(
    ifcopenshell::file* file,
    express::Base context,
    express::Base item,
    const char* representation_identifier,
    bool has_representation_identifier,
    const char* representation_type,
    bool has_representation_type)
{
    ifcopenshell_clear_error();
    if (!file || !context || !item) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto representation = file->create(file->schema()->declaration_by_name("IfcTopologyRepresentation"));
        write_ref(representation, "ContextOfItems", context);
        write_string(
            representation,
            "RepresentationIdentifier",
            has_representation_identifier ? std::string(representation_identifier ? representation_identifier : "")
                                          : read_string(context, "ContextIdentifier"));
        write_string(
            representation,
            "RepresentationType",
            has_representation_type ? std::string(representation_type ? representation_type : "")
                                    : topology_representation_type(item));
        write_ref_list(representation, "Items", {item});
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_wall_representation(
    ifcopenshell::file* file,
    express::Base context,
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
    const std::vector<express::Base>& booleans)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        double length_units = length / unit_scale;
        double thickness_units = (thickness / unit_scale) * (1.0 / std::cos(x_angle));
        if (direction_sense == "NEGATIVE") {
            thickness_units *= -1.0;
        }
        const std::vector<std::vector<double>> points = {
            {0.0, 0.0},
            {0.0, thickness_units},
            {length_units, thickness_units},
            {length_units, 0.0},
            {0.0, 0.0},
        };
        auto curve = ifcapi::detail::create_polyline_or_indexed_polycurve(file, points, 2, true);
        auto profile = make_closed_profile(file, curve);
        const std::vector<double> extrusion_ratios = x_angle ? std::vector<double>{0.0, std::sin(x_angle), std::cos(x_angle)}
                                                            : std::vector<double>{0.0, 0.0, 1.0};

        auto extrusion = file->create(file->schema()->declaration_by_name("IfcExtrudedAreaSolid"));
        write_ref(extrusion, "SweptArea", profile);
        write_ref(extrusion, "Position", make_axis2_placement_3d_optional(file, {0.0, offset / unit_scale, 0.0}));
        write_ref(
            extrusion,
            "ExtrudedDirection",
            make_direction(file, extrusion_ratios[0], extrusion_ratios[1], extrusion_ratios[2]));
        int depth_idx = attr_index_of(extrusion, "Depth");
        if (depth_idx >= 0) {
            extrusion.set_attribute_value(static_cast<size_t>(depth_idx), (height / unit_scale) * std::abs(1.0 / std::cos(x_angle)));
        }

        express::Base item = extrusion;
        for (auto it = booleans.rbegin(); it != booleans.rend(); ++it) {
            auto boolean = *it;
            if (!boolean) continue;
            write_ref(boolean, "FirstOperand", item);
            item = boolean;
        }
        item = apply_ordered_clippings(file, item, clipping_kinds, clipping_locations, clipping_normals, clipping_entities, unit_scale);
        return make_shape_representation(
            file, context, (!clipping_kinds.empty() || !booleans.empty()) ? "Clipping" : "SweptSolid", item);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_add_slab_representation(
    ifcopenshell::file* file,
    express::Base context,
    double depth,
    const std::string& direction_sense,
    double offset,
    double x_angle,
    const std::vector<int32_t>& clipping_kinds,
    const std::vector<std::vector<double>>& clipping_locations,
    const std::vector<std::vector<double>>& clipping_normals,
    const std::vector<express::Base>& clipping_entities,
    const std::vector<std::vector<double>>& polyline,
    bool has_polyline)
{
    ifcopenshell_clear_error();
    if (!file || !context) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        std::vector<std::vector<double>> points;
        if (has_polyline) {
            points.reserve(polyline.size());
            for (const auto& p : polyline) {
                points.push_back({p.at(0) / unit_scale, p.at(1) * std::abs(1.0 / std::cos(x_angle)) / unit_scale});
            }
        } else {
            const double size = 1.0 / unit_scale;
            points = {{0.0, 0.0}, {size, 0.0}, {size, size}, {0.0, size}, {0.0, 0.0}};
        }
        auto curve = ifcapi::detail::create_polyline_or_indexed_polycurve(file, points, 2, false);
        auto profile = make_closed_profile(file, curve);
        std::vector<double> direction_ratios = x_angle ? std::vector<double>{0.0, std::sin(x_angle), std::cos(x_angle)}
                                                       : std::vector<double>{0.0, 0.0, 1.0};
        const std::vector<double> offset_direction = direction_ratios;
        if (direction_sense == "NEGATIVE") {
            for (auto& ratio : direction_ratios) {
                ratio *= -1.0;
            }
        }

        const double perpendicular_offset = (offset / unit_scale) * std::abs(1.0 / std::cos(x_angle));
        const double perpendicular_depth = (depth / unit_scale) * std::abs(1.0 / std::cos(x_angle));
        express::Base position = {};
        if (file->schema()->name() == "IFC2X3" || offset != 0.0) {
            position = make_axis2_placement_3d_optional(
                file,
                {
                    offset_direction[0] * perpendicular_offset,
                    offset_direction[1] * perpendicular_offset,
                    offset_direction[2] * perpendicular_offset,
                });
        }

        auto extrusion = file->create(file->schema()->declaration_by_name("IfcExtrudedAreaSolid"));
        write_ref(extrusion, "SweptArea", profile);
        write_ref(extrusion, "Position", position);
        write_ref(
            extrusion,
            "ExtrudedDirection",
            make_direction(file, direction_ratios[0], direction_ratios[1], direction_ratios[2]));
        int depth_idx = attr_index_of(extrusion, "Depth");
        if (depth_idx >= 0) {
            extrusion.set_attribute_value(static_cast<size_t>(depth_idx), perpendicular_depth);
        }

        auto item = apply_ordered_clippings(file, extrusion, clipping_kinds, clipping_locations, clipping_normals, clipping_entities, unit_scale);
        return make_shape_representation(file, context, clipping_kinds.empty() ? "SweptSolid" : "Clipping", item);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_create_2pt_wall(
    ifcopenshell::file* file,
    express::Base element,
    express::Base context,
    const std::vector<double>& p1,
    const std::vector<double>& p2,
    double elevation,
    double height,
    double thickness,
    bool is_si)
{
    ifcopenshell_clear_error();
    if (!file || !element || !context || p1.size() < 2 || p2.size() < 2) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        std::vector<double> p1_si = {p1[0], p1[1]};
        const double dx = p2[0] - p1[0];
        const double dy = p2[1] - p1[1];
        double length = std::sqrt(dx * dx + dy * dy);
        if (!is_si) {
            length *= unit_scale;
            height *= unit_scale;
            thickness *= unit_scale;
            p1_si[0] *= unit_scale;
            p1_si[1] *= unit_scale;
            elevation *= unit_scale;
        }
        auto representation = geometry_add_wall_representation(
            file, context, length, height, "POSITIVE", 0.0, thickness, 0.0, {}, {}, {}, {}, {});
        if (!representation) {
            throw std::runtime_error("Unable to create wall representation");
        }

        const double norm = std::sqrt(dx * dx + dy * dy);
        const double vx = dx / norm;
        const double vy = dy / norm;
        const std::vector<double> matrix = {
            vx, -vy, 0.0, p1_si[0],
            vy,  vx, 0.0, p1_si[1],
            0.0, 0.0, 1.0, elevation,
            0.0, 0.0, 0.0, 1.0,
        };
        if (!geometry_edit_object_placement(
                file,
                GeometryEditObjectPlacementOptions{element, matrix, true, false})) {
            throw std::runtime_error("Unable to edit wall placement");
        }
        return representation;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_connect_wall(
    ifcopenshell::file* file,
    express::Base wall1,
    express::Base wall2,
    bool is_atpath,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || !wall1 || !wall2) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto placement1 = read_ref(wall1, "ObjectPlacement");
        auto placement2 = read_ref(wall2, "ObjectPlacement");
        auto matrix1i = ifcapi::detail::invert_rigid4(ifcapi::bindings::placement_get_local_placement(placement1));
        auto matrix2 = ifcapi::bindings::placement_get_local_placement(placement2);
        auto transform = ifcapi::detail::matmul4(matrix1i, matrix2);
        auto axis1 = ifcapi::detail::get_reference_line(file, wall1);
        auto axis2 = ifcapi::detail::get_reference_line(file, wall2);
        axis2[0] = ifcapi::detail::transform_point_2d(transform, axis2[0]);
        axis2[1] = ifcapi::detail::transform_point_2d(transform, axis2[1]);

        const double midx = (axis1[0][0] + axis1[1][0]) / 2.0;
        const double starty = axis2[0][1];
        const double endy = axis2[1][1];
        const double y = axis1[0][1];
        double x = 0.0;
        if (!ifcapi::detail::intersect_x_axis_2d(axis2[0], axis2[1], y, x)) {
            return {};
        }

        const std::string wall1_end = x > midx ? "ATEND" : "ATSTART";
        const std::string wall2_end = is_atpath ? "ATPATH" : (std::fabs(y - starty) < std::fabs(y - endy) ? "ATSTART" : "ATEND");
        return geometry_connect_path(
            file,
            GeometryConnectPathOptions{
                wall1,
                wall2,
                wall1_end,
                wall2_end,
                {},
                {},
                owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt});
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_clip_solid(
    ifcopenshell::file* file,
    express::Base item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    express::Base element,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || !item) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        auto half_space = file->create(file->schema()->declaration_by_name("IfcHalfSpaceSolid"));
        write_ref(half_space, "BaseSurface", make_clipping_plane(file, location, normal, unit_scale));
        write_bool(half_space, "AgreementFlag", false);

        auto result = file->create(file->schema()->declaration_by_name("IfcBooleanClippingResult"));
        write_string(result, "Operator", "DIFFERENCE");
        write_ref(result, "FirstOperand", item);
        write_ref(result, "SecondOperand", half_space);
        register_bbim_boolean(file, element, result, owner_history, user, application);
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_clip_solid_bounded(
    ifcopenshell::file* file,
    express::Base item,
    const std::vector<double>& location,
    const std::vector<double>& normal,
    const std::vector<std::vector<double>>& boundary_points,
    const std::vector<double>& boundary_position,
    express::Base element,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || !item) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
        auto scaled_boundary_position = boundary_position;
        for (auto& coordinate : scaled_boundary_position) coordinate /= unit_scale;

        std::vector<std::vector<double>> scaled_boundary_points;
        scaled_boundary_points.reserve(boundary_points.size() + 1);
        for (const auto& point : boundary_points) {
            scaled_boundary_points.push_back({point.at(0) / unit_scale, point.at(1) / unit_scale});
        }
        scaled_boundary_points.push_back(scaled_boundary_points.at(0));

        std::vector<express::Base> ifc_points;
        ifc_points.reserve(scaled_boundary_points.size());
        for (const auto& point : scaled_boundary_points) {
            ifc_points.push_back(ifcapi::detail::create_cartesian_point(file, point));
        }
        auto boundary = file->create(file->schema()->declaration_by_name("IfcPolyline"));
        write_ref_list(boundary, "Points", ifc_points);

        auto half_space = file->create(file->schema()->declaration_by_name("IfcPolygonalBoundedHalfSpace"));
        write_ref(half_space, "BaseSurface", make_clipping_plane(file, location, normal, unit_scale));
        write_bool(half_space, "AgreementFlag", false);
        write_ref(half_space, "Position", make_axis2_placement_3d_location_only(file, scaled_boundary_position));
        write_ref(half_space, "PolygonalBoundary", boundary);

        auto result = file->create(file->schema()->declaration_by_name("IfcBooleanClippingResult"));
        write_string(result, "Operator", "DIFFERENCE");
        write_ref(result, "FirstOperand", item);
        write_ref(result, "SecondOperand", half_space);
        register_bbim_boolean(file, element, result, owner_history, user, application);
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

bool geometry_validate_type(
    ifcopenshell::file* file,
    express::Base representation,
    express::Base preferred_item)
{
    ifcopenshell_clear_error();
    if (!file || !representation) {
        set_error("Invalid arguments");
        return false;
    }

    try {
        auto items = read_ref_list(representation, "Items");
        bool has_boolean = false;
        std::vector<express::Base> remaining_items;
        for (auto item : items) {
            if (is_a(item, "IfcBooleanResult")) {
                has_boolean = true;
            }
            if ((!preferred_item || item != preferred_item) &&
                (is_a(item, "IfcBooleanResult") || is_a(item, "IfcCsgPrimitive3D") ||
                 is_a(item, "IfcHalfSpaceSolid") || is_a(item, "IfcSolidModel") ||
                 is_a(item, "IfcTessellatedFaceSet"))) {
                remaining_items.push_back(item);
            }
        }

        if (!has_boolean) {
            std::string result = guess_representation_type(items);
            if (!result.empty()) {
                if (read_string(representation, "RepresentationType") != result) {
                    write_string(representation, "RepresentationType", result);
                }
                return true;
            }
            return false;
        }

        if (!preferred_item) {
            for (auto item : remaining_items) {
                if (is_a(item, "IfcBooleanResult")) {
                    preferred_item = item;
                    break;
                }
            }
            if (!preferred_item && !remaining_items.empty()) {
                preferred_item = remaining_items.front();
            }
        }

        if (!remaining_items.empty()) {
            geometry_add_boolean(file, preferred_item, ifcapi::detail::to_const_refs(remaining_items), "UNION");
            items = read_ref_list(representation, "Items");
            items.erase(
                std::remove_if(
                    items.begin(),
                    items.end(),
                    [&](express::Base item) {
                        return !ifcapi::detail::same_instance(item, preferred_item) &&
                            ifcapi::detail::contains_ref(remaining_items, item);
                    }),
                items.end());
            write_ref_list(representation, "Items", items);
        }

        std::string representation_type = guess_representation_type(read_ref_list(representation, "Items"));
        if (representation_type.empty()) {
            int idx = attr_index_of(representation, "RepresentationType");
            if (idx >= 0) representation.set_attribute_value(static_cast<size_t>(idx), blank{});
            return false;
        }
        if (read_string(representation, "RepresentationType") != representation_type) {
            write_string(representation, "RepresentationType", representation_type);
        }
        return representation_type == "CSG";
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

void geometry_remove_boolean(ifcopenshell::file* file, express::Base item) {
    ifcopenshell_clear_error();
    if (!file || !item) {
        return;
    }

    try {
        geometry_remove_boolean_impl(file, item);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

express::Base geometry_map_representation(
    ifcopenshell::file* file,
    express::Base representation)
{
    ifcopenshell_clear_error();
    if (!file || !representation) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        return map_representation_impl(file, representation);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_assign_representation(
    ifcopenshell::file* file,
    express::Base product,
    express::Base representation)
{
    ifcopenshell_clear_error();
    if (!file || !product || !representation) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        assign_representation_impl(file, product, representation);
        return product;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_regenerate_wall_representation(
    ifcopenshell::file* file,
    express::Base wall,
    double length,
    double height,
    double angle,
    bool has_angle)
{
    ifcopenshell_clear_error();
    if (!file || !wall) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        WallRegenerator regenerator(file);
        return regenerator.regenerate(wall, length, height, has_angle, angle);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base geometry_connect_element(
    ifcopenshell::file* file,
    express::Base relating_element,
    express::Base related_element,
    const char* description,
    bool has_description,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        std::vector<express::Base> incompatible_connections;
        std::unordered_set<express::Base> seen;
        for (auto rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatingElement"), related_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(related_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), relating_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);

        for (auto rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), related_element)) {
                write_optional_string(rel, "Description", description, has_description);
                return rel;
            }
        }

        const auto rel_decl = file->schema()->declaration_by_name("IfcRelConnectsElements");
        auto rel = file->create(rel_decl);
        if (!rel) {
            set_error("Failed to create IfcRelConnectsElements");
            return {};
        }
        write_string(rel, "GlobalId", ifcapi::guid_new());
        write_ref(rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        write_optional_string(rel, "Description", description, has_description);
        write_ref(rel, "RelatingElement", relating_element);
        write_ref(rel, "RelatedElement", related_element);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void geometry_disconnect_element(
    ifcopenshell::file* file,
    express::Base relating_element,
    express::Base related_element)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        return;
    }

    try {
        std::vector<express::Base> incompatible_connections;
        std::unordered_set<express::Base> seen;
        for (auto rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), related_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatingElement"), related_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(related_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), relating_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(related_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatingElement"), relating_element)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

express::Base geometry_connect_path(
    ifcopenshell::file* file,
    express::Base relating_element,
    express::Base related_element,
    const std::string& relating_connection,
    const std::string& related_connection,
    const char* description,
    bool has_description,
    express::Base connection_geometry,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        const std::string& relating_type = relating_connection;
        const std::string& related_type = related_connection;
        std::vector<express::Base> incompatible_connections;
        std::unordered_set<express::Base> seen;

        for (auto rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) {
                continue;
            }
            if (ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), related_element) ||
                (is_terminal_connection(read_string(rel, "RelatingConnectionType")) &&
                    read_string(rel, "RelatingConnectionType") == relating_type)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsPathElements") &&
                is_terminal_connection(read_string(rel, "RelatedConnectionType")) &&
                read_string(rel, "RelatedConnectionType") == relating_type) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(related_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsPathElements") &&
                is_terminal_connection(read_string(rel, "RelatedConnectionType")) &&
                read_string(rel, "RelatedConnectionType") == related_type) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto rel : inverse_refs(related_element, "ConnectedTo")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) {
                continue;
            }
            if (ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), relating_element) ||
                (is_terminal_connection(read_string(rel, "RelatingConnectionType")) &&
                    read_string(rel, "RelatingConnectionType") == related_type)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);

        const auto rel_decl = file->schema()->declaration_by_name("IfcRelConnectsPathElements");
        auto rel = file->create(rel_decl);
        if (!rel) {
            set_error("Failed to create IfcRelConnectsPathElements");
            return {};
        }
        write_string(rel, "GlobalId", ifcapi::guid_new());
        write_ref(rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        write_optional_string(rel, "Description", description, has_description);
        write_ref(rel, "ConnectionGeometry", connection_geometry);
        write_ref(rel, "RelatingElement", relating_element);
        write_ref(rel, "RelatedElement", related_element);
        ifcapi::detail::write_enum_attr(rel, "RelatingConnectionType", relating_type);
        ifcapi::detail::write_enum_attr(rel, "RelatedConnectionType", related_type);
        write_empty_int_aggregate(rel, "RelatingPriorities");
        write_empty_int_aggregate(rel, "RelatedPriorities");
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void geometry_disconnect_path(
    ifcopenshell::file* file,
    express::Base element,
    const char* connection_type,
    bool has_connection_type,
    express::Base relating_element,
    express::Base related_element)
{
    ifcopenshell_clear_error();
    if (!file) {
        return;
    }

    try {
        std::vector<express::Base> connections;
        std::unordered_set<express::Base> seen;
        if (has_connection_type && element) {
            const std::string type = connection_type ? connection_type : "";
            for (auto rel : inverse_refs(element, "ConnectedTo")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && read_string(rel, "RelatingConnectionType") == type) {
                    append_unique_connection(connections, seen, rel);
                }
            }
            for (auto rel : inverse_refs(element, "ConnectedFrom")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && read_string(rel, "RelatedConnectionType") == type) {
                    append_unique_connection(connections, seen, rel);
                }
            }
        } else if (related_element) {
            for (auto rel : inverse_refs(relating_element, "ConnectedTo")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && ifcapi::detail::same_instance(read_ref(rel, "RelatedElement"), related_element)) {
                    append_unique_connection(connections, seen, rel);
                }
            }
        } else {
            set_error("cannot access local variable 'connections' where it is not associated with a value");
            return;
        }
        remove_connections_with_history(file, connections);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

// ---- IfcTypeProduct unassignment ---------------------------------------

} // namespace bindings
} // namespace ifcapi

namespace {

void process_shape_aspects_for_rep_map(ifcopenshell::file* file,
                                       express::Base rep_map) {
    std::vector<express::Base> shape_aspects;
    bool is_2x3 = file->schema() && std::string(file->schema()->name()) == "IFC2X3";
    if (is_2x3) {
        // IFC2X3: scan all IfcShapeAspect, filter by PartOfProductDefinitionShape.
        try {
            auto all = ifcapi::detail::instances_by_type(file, "IfcShapeAspect");
            for (auto a : all) {
                if (read_ref(a, "PartOfProductDefinitionShape") == rep_map) {
                    shape_aspects.push_back(a);
                }
            }
        } catch (...) {}
    } else {
        auto be = as_entity(rep_map);
        if (be) {
            try {
                auto invs = be.get_inverse("HasShapeAspects");
                for (auto a : invs) shape_aspects.push_back(a);
            } catch (...) {}
        }
    }
    for (auto sa : shape_aspects) {
        auto reps = read_ref_list(sa, "ShapeRepresentations");
        file->remove_entity(sa);
        for (auto r : reps) {
            remove_representation_simple(file, r);
        }
    }
}

void unassign_products_using_mapped_representation(ifcopenshell::file* file,
                                                   express::Base rep_map) {
    struct PR { express::Base product; express::Base rep; };
    std::vector<PR> mapped_pairs;
    std::vector<express::Base> just_reps;

    auto be = as_entity(rep_map);
    if (!be) return;
    std::vector<express::Base> map_usages;
    try { map_usages = ifcapi::detail::read_inverse_aggregate(rep_map, "MapUsage"); } catch (...) {}
    if (map_usages.empty()) return;
    for (auto mu : map_usages) {
        if (!mu) continue;
        auto invs = file->instances_by_reference(static_cast<int>(mu.id()));
        for (auto inv : invs) {
            if (!inv || !is_a(inv, "IfcShapeRepresentation")) continue;
            auto inv_be = as_entity(inv);
            if (!inv_be) continue;
            std::vector<express::Base> defs;
            try { defs = ifcapi::detail::read_inverse_aggregate(inv, "OfProductRepresentation"); } catch (...) {}
            if (defs.empty()) continue;
            for (auto def : defs) {
                auto def_be = as_entity(def);
                if (!def_be) continue;
                std::vector<express::Base> products;
                try { products = ifcapi::detail::read_inverse_aggregate(def, "ShapeOfProduct"); } catch (...) {}
                if (products.empty()) continue;
                for (auto product : products) {
                    mapped_pairs.push_back({product, inv});
                    just_reps.push_back(inv);
                }
            }
        }
    }
    for (auto& pr : mapped_pairs) {
        unassign_product_representation(file, pr.product, pr.rep);
    }
    for (auto r : just_reps) {
        remove_representation_simple(file, r);
    }
}

void unassign_type_representation(ifcopenshell::file* file,
                                  express::Base type_product,
                                  express::Base representation) {
    auto rep_maps = read_ref_list(type_product, "RepresentationMaps");
    express::Base matching = {};
    for (auto rm : rep_maps) {
        if (read_ref(rm, "MappedRepresentation") == representation) {
            matching = rm; break;
        }
    }
    if (!matching) return;

    unassign_products_using_mapped_representation(file, matching);

    std::vector<express::Base> remaining;
    for (auto rm : rep_maps) if (rm != matching) remaining.push_back(rm);
    if (remaining.empty()) {
        int idx = attr_index_of(type_product, "RepresentationMaps");
        if (idx >= 0) type_product.set_attribute_value(static_cast<size_t>(idx), blank{});
    } else {
        write_ref_list(type_product, "RepresentationMaps", remaining);
    }

    process_shape_aspects_for_rep_map(file, matching);

    // Replace MappedRepresentation with a fresh empty IfcShapeRepresentation
    // so the rep_map subgraph (which still references the *old* representation
    // shared by other things) becomes safely deletable.
    auto sr_decl = file->schema()->declaration_by_name("IfcShapeRepresentation");
    auto placeholder = file->create(sr_decl);
    write_ref(matching, "MappedRepresentation", placeholder);

    ifcapi::bindings::entity_remove_deep(&matching);
}

// ---- profile extents (axis-aligned 2D bbox of an IfcProfileDef) ---------

bool profile_extents(ifcopenshell::file* file,
                     express::Base profile,
                     double* x_out, double* y_out) {
    if (!profile || !x_out || !y_out) return false;

    auto try_attr = [&](const char* name) -> double {
        int idx = attr_index_of(profile, name);
        if (idx < 0) return 0.0;
        try {
            auto v = profile.get_attribute_value(idx);
            return (double)v;
        } catch (...) { return 0.0; }
    };

    if (is_a(profile, "IfcAsymmetricIShapeProfileDef")) {
        *x_out = try_attr("OverallWidth"); *y_out = try_attr("OverallDepth"); return true;
    }
    if (is_a(profile, "IfcCShapeProfileDef")) {
        *x_out = try_attr("Width"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcCircleProfileDef")) {
        double r = try_attr("Radius");
        *x_out = r * 2; *y_out = r * 2; return true;
    }
    if (is_a(profile, "IfcEllipseProfileDef")) {
        *x_out = try_attr("SemiAxis1") * 2; *y_out = try_attr("SemiAxis2") * 2; return true;
    }
    if (is_a(profile, "IfcIShapeProfileDef")) {
        *x_out = try_attr("OverallWidth"); *y_out = try_attr("OverallDepth"); return true;
    }
    if (is_a(profile, "IfcLShapeProfileDef")) {
        *x_out = try_attr("Width"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcRectangleProfileDef")) {
        *x_out = try_attr("XDim"); *y_out = try_attr("YDim"); return true;
    }
    if (is_a(profile, "IfcTShapeProfileDef")) {
        *x_out = try_attr("FlangeWidth"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcUShapeProfileDef")) {
        *x_out = try_attr("FlangeWidth"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcZShapeProfileDef")) {
        *x_out = (try_attr("FlangeWidth") * 2) - try_attr("WebThickness");
        *y_out = try_attr("Depth"); return true;
    }
    auto entity = profile.as<express::Entity>();
    if (!file || !entity) return false;

#if IFCOPENSHELL_WITH_IFCGEOM
    ifcopenshell::geometry::Settings settings;
    settings.get<ifcopenshell::geometry::settings::OutputDimensionality>().value =
        ifcopenshell::geometry::settings::CURVES_SURFACES_AND_SOLIDS;
    settings.get<ifcopenshell::geometry::settings::IteratorOutput>().value =
        ifcopenshell::geometry::settings::TRIANGULATED;

    ifcopenshell::geometry::Converter kernel(
        ifcopenshell::geometry::kernels::construct(file, "opencascade", settings),
        file,
        settings);

    IfcGeom::ConversionResults shapes = kernel.convert(profile);
    if (shapes.empty()) return false;

    auto brep = boost::shared_ptr<IfcGeom::Representation::BRep>(
        new IfcGeom::Representation::BRep(
            kernel.settings(),
            entity.declaration().name(),
            std::to_string(entity.id()),
            shapes));
    auto identity = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>();
    IfcGeom::BRepElement brep_element(
        entity.id(),
        -1,
        entity.declaration().name(),
        entity.declaration().name(),
        std::string(),
        std::string(),
        identity,
        brep,
        {});
    IfcGeom::TriangulationElement triangulated(brep_element);
    const auto& verts = triangulated.geometry().verts();
    if (verts.size() < 3) return false;

    double xmin = std::numeric_limits<double>::infinity();
    double xmax = -std::numeric_limits<double>::infinity();
    double ymin = std::numeric_limits<double>::infinity();
    double ymax = -std::numeric_limits<double>::infinity();
    for (size_t i = 0; i + 2 < verts.size(); i += 3) {
        xmin = std::min(xmin, verts[i]);
        xmax = std::max(xmax, verts[i]);
        ymin = std::min(ymin, verts[i + 1]);
        ymax = std::max(ymax, verts[i + 1]);
    }

    double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    if (unit_scale == 0.0) unit_scale = 1.0;
    *x_out = (xmax - xmin) / unit_scale;
    *y_out = (ymax - ymin) / unit_scale;
    return true;
#else
    (void)file;
    (void)entity;
    return false;
#endif
}

}  // namespace

namespace ifcapi {
namespace bindings {

void geometry_unassign_representation(
    ifcopenshell::file* file,
    express::Base product,
    express::Base representation)
{
    if (!file || !product || !representation) return;
    try {
        if (is_a(product, "IfcProduct")) {
            unassign_product_representation(file, product, representation);
        } else if (is_a(product, "IfcTypeProduct")) {
            unassign_type_representation(file, product, representation);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    } catch (...) {
        set_error("geometry_unassign_representation: unknown exception");
    }
}

void geometry_remove_representation(
    ifcopenshell::file* file,
    express::Base representation,
    bool should_keep_named_profiles)
{
    if (!file || !representation) return;
    bool is_ifc2x3 = file->schema() && file->schema()->name() == "IFC2X3";
    std::vector<express::Base> styled_items;
    std::vector<express::Base> presentation_layer_assignments_items;
    std::vector<express::Base> presentation_layer_assignments_reps;
    std::vector<express::Base> textures;
    std::vector<express::Base> colours;
    std::vector<express::Base> named_profiles;

    auto traversed = file->traverse(representation, -1);
    if (!traversed.empty()) {
        for (auto subelement : traversed) {
            if (is_a(subelement, "IfcRepresentationItem")) {
                ifcapi::detail::append_unique(
                    styled_items,
                    ifcapi::detail::read_inverse_aggregate(subelement, "StyledByItem"));
                ifcapi::detail::append_unique(
                    presentation_layer_assignments_items,
                    ifcapi::detail::read_inverse_aggregate(
                        subelement,
                        is_ifc2x3 ? "LayerAssignments" : "LayerAssignment"));
                if (is_a(subelement, "IfcTessellatedFaceSet")) {
                    ifcapi::detail::append_unique(
                        textures,
                        ifcapi::detail::read_inverse_aggregate(subelement, "HasTextures"));
                    ifcapi::detail::append_unique(
                        colours,
                        ifcapi::detail::read_inverse_aggregate(subelement, "HasColours"));
                }
            } else if (is_a(subelement, "IfcRepresentation")) {
                ifcapi::detail::append_unique(
                    presentation_layer_assignments_reps,
                    ifcapi::detail::read_inverse_aggregate(subelement, "LayerAssignments"));
            } else if (is_a(subelement, "IfcProfileDef") && !read_string(subelement, "ProfileName").empty()) {
                ifcapi::detail::append_unique(named_profiles, subelement);
            }
        }
    }

    auto do_not_delete = ifcapi::detail::instances_by_type(file, "IfcGeometricRepresentationContext");
    if (should_keep_named_profiles) {
        ifcapi::detail::append_unique(do_not_delete, named_profiles);
    }

    std::vector<express::Base> also_consider = presentation_layer_assignments_reps;
    for (auto layer : presentation_layer_assignments_items) {
        if (!ifcapi::detail::contains_ref(presentation_layer_assignments_reps, layer)) {
            also_consider.push_back(layer);
        }
    }
    ifcapi::detail::append_unique(also_consider, styled_items);
    ifcapi::detail::append_unique(also_consider, textures);

    entity_remove_deep_with_options(
        &representation,
        {ifcapi::detail::to_const_refs(also_consider),
         ifcapi::detail::to_const_refs(do_not_delete)});

    for (auto texture : textures) {
        entity_remove_deep(&texture);
    }
    for (auto colour : colours) {
        entity_remove_deep(&colour);
    }

    for (auto styled_item : styled_items) {
        if (!ifcapi::detail::exists_in_file(file, styled_item)) continue;
        auto item = read_ref(styled_item, "Item");
        if (!item || !ifcapi::detail::exists_in_file(file, item)) {
            file->remove_entity(styled_item);
        }
    }

    std::vector<express::Base> presentation_layer_assignments = presentation_layer_assignments_reps;
    ifcapi::detail::append_unique(presentation_layer_assignments, presentation_layer_assignments_items);
    for (auto layer : presentation_layer_assignments) {
        if (!ifcapi::detail::exists_in_file(file, layer)) continue;
        auto assigned_items = read_ref_list(layer, "AssignedItems");
        bool all_deleted = true;
        for (auto item : assigned_items) {
            if (ifcapi::detail::exists_in_file(file, item)) {
                all_deleted = false;
                break;
            }
        }
        if (all_deleted) {
            file->remove_entity(layer);
        }
    }
}

express::Base geometry_copy_representation(
    ifcopenshell::file* file,
    express::Base source,
    express::Base target,
    const char* context_identifier)
{
    ifcopenshell_clear_error();
    if (!file || !source || !target) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        std::string ctx = context_identifier ? context_identifier : "Body";
        auto source_rep = get_representation_for_product(source, ctx);
        if (!source_rep) return {};

        std::unordered_map<unsigned, express::Base> memo;
        auto new_rep = deep_copy_entity(file, source_rep,
                                         {"IfcGeometricRepresentationContext"}, memo);

        auto existing = get_representation_for_product(target, ctx);
        if (existing) {
            if (is_a(target, "IfcProduct")) {
                unassign_product_representation(file, target, existing);
            }
            remove_representation_simple(file, existing);
        }

        assign_representation_impl(file, target, new_rep);
        return new_rep;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

bool type_map_type_representations(
    ifcopenshell::file* file,
    express::Base related_object,
    express::Base relating_type)
{
    ifcopenshell_clear_error();
    if (!file || !related_object || !relating_type) {
        set_error("Invalid arguments");
        return false;
    }
    try {
        auto maps = read_ref_list(relating_type, "RepresentationMaps");
        if (maps.empty()) return true;

        auto def = read_ref(related_object, "Representation");
        if (def) {
            auto reps = read_ref_list(def, "Representations");
            for (auto rep : reps) {
                unassign_product_representation(file, related_object, rep);
                remove_representation_simple(file, rep);
            }
        }

        for (auto rmap : maps) {
            auto rep = read_ref(rmap, "MappedRepresentation");
            if (!rep) continue;
            auto mapped = map_representation_impl(file, rep);
            assign_product_representation(file, related_object, mapped);
        }
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

std::vector<double> geometry_profile_extents(
    ifcopenshell::file* file,
    express::Base profile)
{
    ifcopenshell_clear_error();
    if (!file || !profile) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        double x = 0.0;
        double y = 0.0;
        if (!profile_extents(file, profile, &x, &y)) {
            return {};
        }
        return {x, y};
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi

namespace ifcapi {
namespace bindings {

std::vector<express::Base> geometry_add_boolean(
    ifcopenshell::file* file,
    express::Base* first_item,
    const std::vector<express::Base>& second_items,
    const std::string& operator_type)
{
    auto first = ifcapi::detail::deref_or_empty(first_item);
    return geometry_add_boolean(file, first, second_items, operator_type);
}

express::Base geometry_add_axis_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<std::vector<double>>& axis)
{
    auto context_value = ifcapi::detail::deref_or_empty(context);
    return geometry_add_axis_representation(file, context_value, axis);
}

express::Base geometry_add_footprint_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const std::vector<express::Base>& curves)
{
    auto context_value = ifcapi::detail::deref_or_empty(context);
    return geometry_add_footprint_representation(file, context_value, curves);
}

express::Base geometry_add_mesh_representation(
    ifcopenshell::file* file,
    express::Base* context,
    const GeometryAddMeshRepresentationOptions& options)
{
    auto context_value = ifcapi::detail::deref_or_empty(context);
    return geometry_add_mesh_representation(
        file, context_value, options.vertices, options.faces, options.force_faceted_brep.value_or(false));
}

express::Base geometry_add_shape_aspect(
    ifcopenshell::file* file,
    const GeometryAddShapeAspectOptions& options)
{
    return geometry_add_shape_aspect(
        file,
        options.name,
        options.items,
        options.representation,
        options.part_of_product,
        options.description ? options.description->c_str() : nullptr,
        static_cast<bool>(options.description));
}

express::Base geometry_add_topology_representation(
    ifcopenshell::file* file,
    const GeometryAddTopologyRepresentationOptions& options)
{
    return geometry_add_topology_representation(
        file,
        options.context,
        options.item,
        options.representation_identifier ? options.representation_identifier->c_str() : nullptr,
        static_cast<bool>(options.representation_identifier),
        options.representation_type ? options.representation_type->c_str() : nullptr,
        static_cast<bool>(options.representation_type));
}

express::Base geometry_add_wall_representation(
    ifcopenshell::file* file,
    const GeometryAddWallRepresentationOptions& options)
{
    return geometry_add_wall_representation(
        file,
        options.context,
        options.length,
        options.height,
        options.direction_sense,
        options.offset,
        options.thickness,
        options.x_angle,
        options.clipping_kinds,
        options.clipping_locations,
        options.clipping_normals,
        options.clipping_entities,
        options.booleans);
}

express::Base geometry_add_slab_representation(
    ifcopenshell::file* file,
    const GeometryAddSlabRepresentationOptions& options)
{
    static const std::vector<std::vector<double>> empty_polyline;
    return geometry_add_slab_representation(
        file,
        options.context,
        options.depth,
        options.direction_sense,
        options.offset,
        options.x_angle,
        options.clipping_kinds,
        options.clipping_locations,
        options.clipping_normals,
        options.clipping_entities,
        options.polyline ? *options.polyline : empty_polyline,
        static_cast<bool>(options.polyline));
}

express::Base geometry_create_2pt_wall(
    ifcopenshell::file* file,
    const GeometryCreate2PtWallOptions& options)
{
    return geometry_create_2pt_wall(
        file,
        options.element,
        options.context,
        options.start,
        options.end,
        options.elevation,
        options.height,
        options.thickness,
        options.is_si);
}

express::Base geometry_connect_wall(
    ifcopenshell::file* file,
    const GeometryConnectWallOptions& options)
{
    return geometry_connect_wall(
        file,
        options.first_wall,
        options.second_wall,
        options.is_atpath,
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

express::Base geometry_clip_solid(
    ifcopenshell::file* file,
    const GeometryClipSolidOptions& options)
{
    return geometry_clip_solid(
        file,
        options.item,
        options.location,
        options.normal,
        options.element.value_or(express::Base{}),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

express::Base geometry_clip_solid_bounded(
    ifcopenshell::file* file,
    const GeometryClipSolidBoundedOptions& options)
{
    return geometry_clip_solid_bounded(
        file,
        options.item,
        options.location,
        options.normal,
        options.boundary_points,
        options.boundary_position,
        options.element.value_or(express::Base{}),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

bool geometry_validate_type(
    ifcopenshell::file* file,
    express::Base* representation,
    const GeometryValidateTypeOptions& options)
{
    auto representation_value = ifcapi::detail::deref_or_empty(representation);
    auto preferred_value = options.preferred_item.value_or(express::Base{});
    return geometry_validate_type(file, representation_value, preferred_value);
}

void geometry_remove_boolean(ifcopenshell::file* file, express::Base* item) {
    auto item_value = ifcapi::detail::deref_or_empty(item);
    geometry_remove_boolean(file, item_value);
}

express::Base geometry_map_representation(ifcopenshell::file* file, express::Base* representation) {
    auto representation_value = ifcapi::detail::deref_or_empty(representation);
    return geometry_map_representation(file, representation_value);
}

express::Base geometry_assign_representation(ifcopenshell::file* file, express::Base* product, express::Base* representation) {
    auto product_value = ifcapi::detail::deref_or_empty(product);
    auto representation_value = ifcapi::detail::deref_or_empty(representation);
    return geometry_assign_representation(file, product_value, representation_value);
}

express::Base geometry_regenerate_wall_representation(
    ifcopenshell::file* file,
    const GeometryRegenerateWallRepresentationOptions& options)
{
    return geometry_regenerate_wall_representation(
        file,
        options.wall,
        options.length,
        options.height,
        options.angle.value_or(0.0),
        static_cast<bool>(options.angle));
}

express::Base geometry_connect_element(
    ifcopenshell::file* file,
    const GeometryConnectElementOptions& options)
{
    return geometry_connect_element(
        file,
        options.relating_element,
        options.related_element,
        options.description ? options.description->c_str() : nullptr,
        static_cast<bool>(options.description),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

void geometry_disconnect_element(ifcopenshell::file* file, express::Base* relating_element, express::Base* related_element) {
    auto relating_value = ifcapi::detail::deref_or_empty(relating_element);
    auto related_value = ifcapi::detail::deref_or_empty(related_element);
    geometry_disconnect_element(file, relating_value, related_value);
}

express::Base geometry_connect_path(
    ifcopenshell::file* file,
    const GeometryConnectPathOptions& options)
{
    return geometry_connect_path(
        file,
        options.relating_element,
        options.related_element,
        options.relating_connection,
        options.related_connection,
        options.description ? options.description->c_str() : nullptr,
        static_cast<bool>(options.description),
        options.connection_geometry.value_or(express::Base{}),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

void geometry_disconnect_path(
    ifcopenshell::file* file,
    const GeometryDisconnectPathOptions& options)
{
    geometry_disconnect_path(
        file,
        options.element.value_or(express::Base{}),
        options.connection_type ? options.connection_type->c_str() : nullptr,
        static_cast<bool>(options.connection_type),
        options.relating_element.value_or(express::Base{}),
        options.related_element.value_or(express::Base{}));
}

void geometry_unassign_representation(ifcopenshell::file* file, express::Base* product, express::Base* representation) {
    auto product_value = ifcapi::detail::deref_or_empty(product);
    auto representation_value = ifcapi::detail::deref_or_empty(representation);
    geometry_unassign_representation(file, product_value, representation_value);
}

void geometry_remove_representation(
    ifcopenshell::file* file,
    express::Base* representation,
    const GeometryRemoveRepresentationOptions& options)
{
    auto representation_value = ifcapi::detail::deref_or_empty(representation);
    geometry_remove_representation(file, representation_value, options.should_keep_named_profiles.value_or(true));
}

std::optional<express::Base> geometry_copy_representation(
    ifcopenshell::file* file,
    const GeometryCopyRepresentationOptions& options)
{
    auto result = geometry_copy_representation(
        file,
        options.source,
        options.target,
        options.context_identifier ? options.context_identifier->c_str() : nullptr);
    if (!result) return std::nullopt;
    return result;
}

bool type_map_type_representations(ifcopenshell::file* file, express::Base* related_object, express::Base* relating_type) {
    auto related_value = ifcapi::detail::deref_or_empty(related_object);
    auto type_value = ifcapi::detail::deref_or_empty(relating_type);
    return type_map_type_representations(file, related_value, type_value);
}

std::vector<double> geometry_profile_extents(ifcopenshell::file* file, express::Base* profile) {
    auto profile_value = ifcapi::detail::deref_or_empty(profile);
    return geometry_profile_extents(file, profile_value);
}

} // namespace bindings
} // namespace ifcapi
