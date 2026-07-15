// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/alignment.h"
#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"
#include "guid.h"

#include <stdexcept>

namespace {

using namespace ifcapi::detail;
using namespace ifcapi::detail::alignment;

express::Base rooted(ifcopenshell::file* file, const char* type) {
    auto result = create(file, type);
    write_string_attr(result, "GlobalId", ifcapi::guid_new());
    return result;
}

void nest(ifcopenshell::file* file, express::Base parent, express::Base child) {
    ifcapi::bindings::NestAssignObjectOptions options;
    options.products = {child}; options.relating_object = parent;
    ifcapi::bindings::nest_assign_object(file, options);
}

void aggregate(ifcopenshell::file* file, express::Base parent, express::Base child) {
    ifcapi::bindings::AggregateAssignObjectOptions options;
    options.products = {child}; options.relating_object = parent;
    ifcapi::bindings::aggregate_assign_object(file, options);
}

express::Base curve(ifcopenshell::file* file, const char* type, express::Base base = {}) {
    auto result = create(file, type);
    write_ref_aggregate(result, "Segments", {});
    entity_view(result).set("SelfIntersect", false);
    if (base) write_ref_attr(result, "BaseCurve", base);
    return result;
}

express::Base shape(
    ifcopenshell::file* file,
    express::Base context,
    const char* identifier,
    const char* type,
    express::Base item)
{
    auto result = create(file, "IfcShapeRepresentation");
    write_ref_attr(result, "ContextOfItems", context);
    write_string_attr(result, "RepresentationIdentifier", identifier);
    write_string_attr(result, "RepresentationType", type);
    write_ref_aggregate(result, "Items", {item});
    return result;
}

void assign(ifcopenshell::file* file, express::Base product, express::Base representation) {
    ifcapi::bindings::geometry_assign_representation(file, &product, &representation);
}

express::Base ensure_referent_nest(ifcopenshell::file* file, express::Base alignment) {
    auto existing = ifcapi::bindings::alignment_get_referent_nest(alignment);
    if (existing) return *existing;
    auto relation = rooted(file, "IfcRelNests");
    write_ref_attr(relation, "RelatingObject", alignment);
    write_ref_aggregate(relation, "RelatedObjects", {});
    return relation;
}

void move_vertical_to_child(ifcopenshell::file* file, express::Base parent, express::Base vertical) {
    ifcapi::bindings::NestUnassignObjectOptions unassign;
    unassign.products = {vertical};
    ifcapi::bindings::nest_unassign_object(file, unassign);
    auto child = rooted(file, "IfcAlignment");
    write_string_attr(child, "Name", "Child of " + read_string_attr(parent, "Name"));
    nest(file, child, vertical);
    aggregate(file, parent, child);
    auto parent_referents = ifcapi::bindings::alignment_get_referent_nest(parent);
    if (parent_referents) {
        auto parent_objects = refs(*parent_referents, "RelatedObjects");
        std::vector<express::Base> keep;
        std::vector<express::Base> child_objects;
        for (auto referent : parent_objects) {
            bool belongs = false;
            for (auto rel : inverses(referent, "Positions")) {
                for (auto product : refs(rel, "RelatedProducts")) {
                    if (!product.declaration().is("IfcAlignmentSegment")) continue;
                    auto layout = ifcapi::bindings::alignment_get_layout(product);
                    if (layout && *layout == vertical) belongs = true;
                }
            }
            (belongs ? child_objects : keep).push_back(referent);
        }
        write_ref_aggregate(*parent_referents, "RelatedObjects", keep);
        if (!child_objects.empty()) {
            auto child_referents = ensure_referent_nest(file, child);
            write_ref_aggregate(child_referents, "RelatedObjects", child_objects);
        }
    }
    auto basis = ifcapi::bindings::alignment_get_basis_curve(parent);
    if (basis) {
        for (auto representation : representations(parent)) {
            if (read_string_attr(representation, "RepresentationIdentifier") == "Axis" &&
                read_string_attr(representation, "RepresentationType") == "Curve3D") {
                ifcapi::bindings::geometry_unassign_representation(file, &parent, &representation);
                assign(file, child, representation);
                write_ref_attr(child, "ObjectPlacement", read_ref_attr(parent, "ObjectPlacement"));
                break;
            }
        }
    }
}

} // namespace

namespace ifcapi::bindings {

express::Base alignment_get_axis_subcontext(ifcopenshell::file* file) {
    require_ifc4x3(file);
    auto existing = representation_get_context(file, "Model", "Axis", "MODEL_VIEW");
    if (existing) return existing;
    ContextAddContextOptions parent_options;
    parent_options.context_type = "Model";
    auto parent = context_add_context(file, parent_options);
    ContextAddContextOptions child_options;
    child_options.context_type = "Model";
    child_options.context_identifier = "Axis";
    child_options.target_view = "MODEL_VIEW";
    child_options.parent = parent;
    return context_add_context(file, child_options);
}

} // namespace ifcapi::bindings

namespace ifcapi::detail::alignment {

using namespace ifcapi::bindings;

void create_geometric_representation(ifcopenshell::file* file, express::Base alignment) {
    require_owned(file, alignment, "alignment");
    require_type(alignment, "IfcAlignment", "alignment");
    auto context = alignment_get_axis_subcontext(file);
    auto layouts = alignment_get_alignment_layouts(alignment);
    auto children = alignment_get_child_alignments(alignment);
    if (layouts.size() == 1 && children.empty()) {
        auto horizontal = curve(file, "IfcCompositeCurve");
        assign(file, alignment, shape(file, context, "Axis", "Curve2D", horizontal));
    } else if (layouts.size() == 2 && children.empty()) {
        auto horizontal = curve(file, "IfcCompositeCurve");
        assign(file, alignment, shape(file, context, "FootPrint", "Curve2D", horizontal));
        auto vertical = curve(file, "IfcGradientCurve", horizontal);
        assign(file, alignment, shape(file, context, "Axis", "Curve3D", vertical));
    } else if (layouts.size() == 3 && children.empty()) {
        auto horizontal = curve(file, "IfcCompositeCurve");
        assign(file, alignment, shape(file, context, "FootPrint", "Curve2D", horizontal));
        auto vertical = curve(file, "IfcGradientCurve", horizontal);
        auto cant = curve(file, "IfcSegmentedReferenceCurve", vertical);
        assign(file, alignment, shape(file, context, "Axis", "Curve3D", cant));
    } else {
        auto horizontal = curve(file, "IfcCompositeCurve");
        assign(file, alignment, shape(file, context, "FootPrint", "Curve2D", horizontal));
    }
    for (auto child : children) {
        write_ref_attr(child, "ObjectPlacement", read_ref_attr(alignment, "ObjectPlacement"));
        auto child_layouts = alignment_get_alignment_layouts(child);
        auto basis = alignment_get_basis_curve(alignment);
        if (!basis) throw std::runtime_error("Parent alignment has no horizontal basis curve");
        if (child_layouts.size() == 1 && child_layouts.front().declaration().is("IfcAlignmentVertical")) {
            auto vertical = curve(file, "IfcGradientCurve", *basis);
            assign(file, child, shape(file, context, "Axis", "Curve3D", vertical));
        } else if (child_layouts.size() == 2 &&
                   child_layouts[0].declaration().is("IfcAlignmentVertical") &&
                   child_layouts[1].declaration().is("IfcAlignmentCant")) {
            auto vertical = curve(file, "IfcGradientCurve", *basis);
            auto cant = curve(file, "IfcSegmentedReferenceCurve", vertical);
            // Upstream calls the misspelled creatIfcShapeRepresentation here; create the intended entity.
            assign(file, child, shape(file, context, "Axis", "Curve3D", cant));
        } else {
            throw std::runtime_error("Child alignment must contain vertical and optional cant layouts");
        }
    }
}

} // namespace ifcapi::detail::alignment

namespace ifcapi::bindings {

void alignment_create_representation(ifcopenshell::file* file, express::Base alignment) {
    require_ifc4x3(file);
    require_owned(file, alignment, "alignment");
    require_type(alignment, "IfcAlignment", "alignment");
    if (read_ref_attr(alignment, "Representation")) return;
    ifcapi::detail::alignment::create_geometric_representation(file, alignment);
    auto populate = [&](express::Base target) {
        for (auto layout : alignment_get_alignment_layouts(target)) {
            auto target_curve = alignment_get_layout_curve(layout);
            if (!target_curve) continue;
            for (auto segment : alignment_get_layout_segments(layout)) append_segment_to_curve(file, segment, *target_curve, nullptr);
        }
    };
    populate(alignment);
    for (auto child : alignment_get_child_alignments(alignment)) populate(child);
    auto referent_nest = alignment_get_referent_nest(alignment);
    if (referent_nest) {
        auto referents = refs(*referent_nest, "RelatedObjects");
        if (!referents.empty()) {
            auto placement = read_ref_attr(referents.front(), "ObjectPlacement");
            if (placement && !placement.declaration().is("IfcLinearPlacement")) {
                auto basis = alignment_get_basis_curve(alignment);
                auto linear = create(file, "IfcLinearPlacement");
                auto relative = create(file, "IfcAxis2PlacementLinear");
                auto location = create(file, "IfcPointByDistanceExpression");
                write_ref_attr(location, "DistanceAlong", length_measure(file, 0.0));
                write_ref_attr(location, "BasisCurve", basis ? *basis : express::Base());
                write_ref_attr(relative, "Location", location);
                write_ref_attr(linear, "RelativePlacement", relative);
                alignment_update_fallback_position(file, linear);
                write_ref_attr(referents.front(), "ObjectPlacement", linear);
                auto old = placement;
                entity_remove_deep(&old);
            }
        }
    }
}

void alignment_create_segment_representations(ifcopenshell::file* file, express::Base alignment) {
    require_ifc4x3(file);
    require_owned(file, alignment, "alignment");
    for (auto layout : alignment_get_alignment_layouts(alignment)) {
        for (auto segment : alignment_get_layout_segments(layout)) {
            auto p = read_ref_attr(segment, "DesignParameters");
            if (enum_value(p, "PredefinedType") == "HELMERTCURVE") {
                throw std::runtime_error("Segment representations do not support Helmert 1:2 mappings");
            }
        }
    }
    auto context = alignment_get_axis_subcontext(file);
    for (auto layout : alignment_get_alignment_layouts(alignment)) {
        auto target_curve = alignment_get_layout_curve(layout);
        if (!target_curve) continue;
        auto mapped = refs(*target_curve, "Segments");
        auto semantic = alignment_get_layout_segments(layout);
        if (mapped.size() != semantic.size()) throw std::runtime_error("Segment representation mapping cardinality mismatch");
        for (size_t i = 0; i < mapped.size(); ++i) {
            auto rep = shape(file, context, "Axis", "Segment", mapped[i]);
            auto product_shape = create(file, "IfcProductDefinitionShape");
            write_ref_aggregate(product_shape, "Representations", {rep});
            write_ref_attr(semantic[i], "ObjectPlacement", read_ref_attr(alignment, "ObjectPlacement"));
            write_ref_attr(semantic[i], "Representation", product_shape);
        }
    }
}

express::Base alignment_add_vertical_layout(ifcopenshell::file* file, express::Base parent) {
    require_ifc4x3(file);
    require_owned(file, parent, "parent_alignment");
    require_type(parent, "IfcAlignment", "parent_alignment");
    auto horizontal = alignment_get_horizontal_layout(parent);
    if (!horizontal) throw std::runtime_error("parent alignment requires a horizontal layout");
    auto existing_children = alignment_get_child_alignments(parent);
    auto current_vertical = alignment_get_vertical_layout(parent);
    if (current_vertical) move_vertical_to_child(file, parent, *current_vertical);
    auto vertical = rooted(file, "IfcAlignmentVertical");
    auto basis = alignment_get_basis_curve(parent);
    if (existing_children.empty() && !current_vertical) {
        nest(file, parent, vertical);
        if (basis) {
            for (auto representation : representations(parent)) {
                if (read_string_attr(representation, "RepresentationIdentifier") == "Axis" &&
                    read_string_attr(representation, "RepresentationType") == "Curve2D") {
                    write_string_attr(representation, "RepresentationIdentifier", "FootPrint");
                    break;
                }
            }
            auto gradient = curve(file, "IfcGradientCurve", *basis);
            assign(file, parent, shape(file, alignment_get_axis_subcontext(file), "Axis", "Curve3D", gradient));
        }
    } else {
        auto child = rooted(file, "IfcAlignment");
        write_string_attr(child, "Name", "Child of " + read_string_attr(parent, "Name"));
        aggregate(file, parent, child);
        nest(file, child, vertical);
        write_ref_attr(child, "ObjectPlacement", read_ref_attr(parent, "ObjectPlacement"));
        if (basis) {
            auto gradient = curve(file, "IfcGradientCurve", *basis);
            assign(file, child, shape(file, alignment_get_axis_subcontext(file), "Axis", "Curve3D", gradient));
        }
    }
    alignment_add_zero_length_segment(file, vertical);
    auto layout_curve = alignment_get_layout_curve(vertical);
    if (layout_curve) alignment_add_zero_length_segment(file, *layout_curve);
    return vertical;
}

} // namespace ifcapi::bindings
