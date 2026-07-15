// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/style.h"

#include "../pset/attribute_props.hpp"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/detail/attribute.h"
#include "ifcopenshell_api_internal.hpp"
#include "ifcparse/file.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_ifc4x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name().rfind("IFC4X3", 0) == 0;
}

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() <= 0) {
        return result;
    }
    for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
        if (inverse) {
            result.push_back(inverse);
        }
    }
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

bool contains(const std::vector<express::Base>& values, express::Base value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool contains_name(const std::vector<std::string>& values, const std::string& value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool same_single_style(const std::vector<express::Base>& styles, express::Base style) {
    return styles.size() == 1 && styles.front() == style;
}

const ifcapi_pset::Entry* nested_entry(const ifcapi_pset::Entry& entry, const std::string& key) {
    if (!entry.nested) {
        return {};
    }
    for (const auto& nested : entry.nested->entries) {
        if (nested.key == key) {
            return &nested;
        }
    }
    return {};
}

bool entry_truthy(const ifcapi_pset::Entry* entry) {
    if (!entry) {
        return false;
    }
    switch (entry->kind) {
    case ifcapi_pset::Kind::NONE:
        return false;
    case ifcapi_pset::Kind::BOOL:
    case ifcapi_pset::Kind::TYPED_BOOL:
        return entry->b_val;
    case ifcapi_pset::Kind::INT:
    case ifcapi_pset::Kind::TYPED_INT:
        return entry->i_val != 0;
    case ifcapi_pset::Kind::DOUBLE:
    case ifcapi_pset::Kind::TYPED_DOUBLE:
        return entry->d_val != 0.0;
    case ifcapi_pset::Kind::STRING:
    case ifcapi_pset::Kind::TYPED_STRING:
        return !entry->s_val.empty();
    case ifcapi_pset::Kind::INSTANCE:
        return static_cast<bool>(entry->inst);
    default:
        return true;
    }
}

double entry_double(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
    case ifcapi_pset::Kind::DOUBLE:
    case ifcapi_pset::Kind::TYPED_DOUBLE:
        return entry.d_val;
    case ifcapi_pset::Kind::INT:
    case ifcapi_pset::Kind::TYPED_INT:
        return static_cast<double>(entry.i_val);
    case ifcapi_pset::Kind::BOOL:
    case ifcapi_pset::Kind::TYPED_BOOL:
        return entry.b_val ? 1.0 : 0.0;
    default:
        throw std::runtime_error("Expected a numeric colour value");
    }
}

void write_optional_colour_name(express::Base colour, const ifcapi_pset::Entry* name) {
    if (!name || name->kind == ifcapi_pset::Kind::NONE) {
        ifcapi::detail::write_blank_attr(colour, "Name");
    } else {
        ifcapi::detail::write_string_attr(colour, "Name", name->s_val);
    }
}

void write_colour_components(express::Base colour, const ifcapi_pset::Entry& value, bool include_name) {
    if (!value.nested) {
        throw std::runtime_error("Colour attributes require a dictionary value");
    }
    auto* red = nested_entry(value, "Red");
    auto* green = nested_entry(value, "Green");
    auto* blue = nested_entry(value, "Blue");
    if (!red || !green || !blue) {
        throw std::runtime_error("Colour attributes require Red, Green, and Blue");
    }
    if (include_name) {
        write_optional_colour_name(colour, nested_entry(value, "Name"));
    }
    ifcapi::detail::write_double_attr(colour, "Red", entry_double(*red));
    ifcapi::detail::write_double_attr(colour, "Green", entry_double(*green));
    ifcapi::detail::write_double_attr(colour, "Blue", entry_double(*blue));
}

std::string declared_attribute_type(express::Base entity, const std::string& name) {
    auto* decl = entity ? entity.declaration().as_entity() : nullptr;
    if (!decl) {
        return std::string();
    }
    auto attrs = decl->all_attributes();
    for (auto attr : attrs) {
        if (attr->name() != name) {
            continue;
        }
        const ifcopenshell::parameter_type* pt = attr->type_of_attribute();
        if (auto* aggregate = pt ? pt->as_aggregation_type() : nullptr) {
            pt = aggregate->type_of_element();
        }
        auto* named = pt ? pt->as_named_type() : nullptr;
        auto* declared = named ? named->declared_type() : nullptr;
        return declared ? declared->name() : std::string();
    }
    return std::string();
}

express::Base create_typed_double(ifcopenshell::file* file, const char* ifc_type, double value) {
    auto result = ifcapi::detail::create_typed_double(file, ifc_type, value);
    if (!result) {
        throw std::runtime_error(std::string("Unable to create ") + ifc_type);
    }
    return result;
}

void edit_colour_rgb(ifcopenshell::file* file, express::Base style, const ifcapi_pset::Entry& entry) {
    auto colour = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
    if (!colour) {
        colour = create_entity(file, "IfcColourRgb");
        ifcapi::detail::write_ref_attr(style, entry.key.c_str(), colour);
    }
    write_colour_components(colour, entry, true);
}

void edit_colour_or_factor(ifcopenshell::file* file, express::Base style, const ifcapi_pset::Entry& entry) {
    if (entry.kind == ifcapi_pset::Kind::DICT) {
        auto colour = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
        if (!is_a(colour, "IfcColourRgb")) {
            colour = create_entity(file, "IfcColourRgb");
            ifcapi::detail::write_blank_attr(colour, "Name");
            ifcapi::detail::write_double_attr(colour, "Red", 0.0);
            ifcapi::detail::write_double_attr(colour, "Green", 0.0);
            ifcapi::detail::write_double_attr(colour, "Blue", 0.0);
            ifcapi::detail::write_ref_attr(style, entry.key.c_str(), colour);
        }
        write_colour_components(colour, entry, false);
        return;
    }

    auto existing = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
    if (existing && existing.id()) {
        file->remove_entity(existing);
    }
    if (entry.kind == ifcapi_pset::Kind::NONE) {
        ifcapi::detail::write_blank_attr(style, entry.key.c_str());
    } else {
        ifcapi::detail::write_ref_attr(
            style,
            entry.key.c_str(),
            create_typed_double(file, "IfcNormalisedRatioMeasure", entry_double(entry)));
    }
}

void edit_specular_highlight(ifcopenshell::file* file, express::Base style, const ifcapi_pset::Entry& entry) {
    if (entry.kind == ifcapi_pset::Kind::NONE) {
        ifcapi::detail::write_blank_attr(style, "SpecularHighlight");
        return;
    }
    if (entry.kind != ifcapi_pset::Kind::DICT || !entry.nested) {
        ifcapi::detail::apply_attribute_prop(style, entry);
        return;
    }
    if (auto* exponent = nested_entry(entry, "IfcSpecularExponent"); entry_truthy(exponent)) {
        ifcapi::detail::write_ref_attr(
            style,
            "SpecularHighlight",
            create_typed_double(file, "IfcSpecularExponent", entry_double(*exponent)));
    } else if (auto* roughness = nested_entry(entry, "IfcSpecularRoughness"); entry_truthy(roughness)) {
        ifcapi::detail::write_ref_attr(
            style,
            "SpecularHighlight",
            create_typed_double(file, "IfcSpecularRoughness", entry_double(*roughness)));
    }
}

std::vector<express::Base> mutable_entities(
    const std::vector<express::Base>& entities) {
    std::vector<express::Base> result;
    result.reserve(entities.size());
    for (auto entity : entities) {
        if (entity) {
            result.push_back(entity);
        }
    }
    return result;
}

void remove_styles(
    ifcopenshell::file* file,
    express::Base item,
    const std::vector<express::Base>& styles) {
    auto current = ifcapi::detail::read_ref_aggregate(item, "Styles");
    std::vector<express::Base> retained;
    retained.reserve(current.size());
    for (auto style : current) {
        if (!contains(styles, style)) {
            retained.push_back(style);
        }
    }
    if (retained.empty()) {
        file->remove_entity(item);
    } else if (retained.size() != current.size()) {
        ifcapi::detail::write_ref_aggregate(item, "Styles", retained);
    }
}

express::Base create_presentation_style_assignment(ifcopenshell::file* file, express::Base style) {
    auto assignment = create_entity(file, "IfcPresentationStyleAssignment");
    ifcapi::detail::write_ref_aggregate(assignment, "Styles", {style});
    return assignment;
}

express::Base create_styled_item(
    ifcopenshell::file* file,
    express::Base item,
    const std::vector<express::Base>& styles,
    const std::string& name) {
    auto styled_item = create_entity(file, "IfcStyledItem");
    if (item) {
        ifcapi::detail::write_ref_attr(styled_item, "Item", item);
    }
    ifcapi::detail::write_ref_aggregate(styled_item, "Styles", styles);
    ifcapi::detail::write_string_attr(styled_item, "Name", name);
    return styled_item;
}

std::vector<express::Base> append_ref(
    std::vector<express::Base> values,
    express::Base value) {
    values.push_back(value);
    return values;
}

void remove_same_type_styles(
    ifcopenshell::file* file,
    express::Base style_item,
    const std::string& current_style_type,
    bool remove_item) {
    if (!style_item) {
        return;
    }
    auto current = ifcapi::detail::read_ref_aggregate(style_item, "Styles");
    std::vector<express::Base> retained;
    for (auto style : current) {
        if (!is_a(style, current_style_type.c_str())) {
            retained.push_back(style);
        }
    }
    if (remove_item && retained.empty()) {
        file->remove_entity(style_item);
    } else {
        ifcapi::detail::write_ref_aggregate(style_item, "Styles", retained);
    }
}

bool has_proposed_style(ifcopenshell::file* file, express::Base styled_item, express::Base style) {
    auto styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
    if (contains(styles, style)) {
        return true;
    }
    if (!is_ifc4x3(file)) {
        for (auto s : styles) {
            if (is_a(s, "IfcPresentationStyleAssignment") &&
                contains(ifcapi::detail::read_ref_aggregate(s, "Styles"), style)) {
                return true;
            }
        }
    }
    return false;
}

bool has_same_style_type(express::Base styled_item, express::Base style) {
    auto style_class = style ? style.declaration().name() : std::string();
    for (auto s : ifcapi::detail::read_ref_aggregate(styled_item, "Styles")) {
        if (s.declaration().name() == style_class) {
            return true;
        }
        if (is_a(s, "IfcPresentationStyleAssignment")) {
            for (auto ss : ifcapi::detail::read_ref_aggregate(s, "Styles")) {
                if (ss.declaration().name() == style_class) {
                    return true;
                }
            }
        }
    }
    return false;
}

} // namespace

namespace ifcapi {
namespace bindings {

void remove_surface_style_impl(ifcopenshell::file* file, express::Base style);
void remove_style_impl(
    ifcopenshell::file* file,
    express::Base style,
    const std::vector<express::Base>& do_not_delete);
void style_unassign_representation_styles_impl(
    ifcopenshell::file* file,
    express::Base shape_representation,
    const std::vector<express::Base>& input_styles,
    bool should_use_presentation_style_assignment);

void style_edit_surface_style_impl(
    ifcopenshell::file* file,
    express::Base style,
    ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        if (!file || !style) {
            throw std::runtime_error("style_edit_surface_style requires a file and style");
        }
        if (!attributes) {
            return;
        }
        for (const auto& entry : attributes->entries) {
            auto attribute_class = declared_attribute_type(style, entry.key);
            if (attribute_class == "IfcColourRgb") {
                edit_colour_rgb(file, style, entry);
            } else if (entry.key == "SpecularHighlight") {
                edit_specular_highlight(file, style, entry);
            } else if (attribute_class == "IfcColourOrFactor") {
                edit_colour_or_factor(file, style, entry);
            } else {
                ifcapi::detail::apply_attribute_prop(style, entry);
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    }
}

express::Base create_styled_item_for_material(
    ifcopenshell::file* file,
    express::Base& active_style,
    express::Base public_style,
    express::Base reuse_item) {
    if (!reuse_item) {
        return create_styled_item(file, {}, {active_style}, ifcapi::detail::read_string_attr(public_style, "Name"));
    }
    if (is_a(reuse_item, "IfcPresentationStyleAssignment") && is_a(active_style, "IfcPresentationStyleAssignment")) {
        file->remove_entity(active_style);
        active_style = reuse_item;
    }
    ifcapi::detail::write_ref_aggregate(reuse_item, "Styles", {public_style});
    ifcapi::detail::write_string_attr(reuse_item, "Name", ifcapi::detail::read_string_attr(public_style, "Name"));
    return reuse_item;
}

express::Base get_styled_representation(
    express::Base definition_representation,
    express::Base context) {
    for (auto representation : ifcapi::detail::read_ref_aggregate(definition_representation, "Representations")) {
        if (is_a(representation, "IfcStyledRepresentation") &&
            ifcapi::detail::read_ref_attr(representation, "ContextOfItems") == context) {
            return representation;
        }
    }
    return {};
}

express::Base create_styled_representation(
    ifcopenshell::file* file,
    express::Base context,
    express::Base& active_style,
    express::Base public_style) {
    auto representation = create_entity(file, "IfcStyledRepresentation");
    ifcapi::detail::write_ref_attr(representation, "ContextOfItems", context);
    ifcapi::detail::write_string_attr(
        representation,
        "RepresentationIdentifier",
        ifcapi::detail::read_string_attr(context, "ContextIdentifier"));
    ifcapi::detail::write_ref_aggregate(
        representation,
        "Items",
        {create_styled_item_for_material(file, active_style, public_style, {})});
    return representation;
}

express::Base style_add_style(
    ifcopenshell::file* file,
    const char* name,
    const std::string& ifc_class) {
    auto style = create_entity(file, ifc_class.c_str());
    if (name) {
        ifcapi::detail::write_string_attr(style, "Name", name);
    }
    if (ifc_class == "IfcSurfaceStyle") {
        ifcapi::detail::write_enum_attr(style, "Side", "BOTH");
    }
    return style;
}

express::Base style_add_surface_style(
    ifcopenshell::file* file,
    express::Base* style,
    const char* ifc_class,
    ifcopenshell_pset_props_t* attributes) {
    auto parent = detail::deref_or_empty(style);
    if (!file || !parent || !is_a(parent, "IfcSurfaceStyle")) {
        throw std::runtime_error("style_add_surface_style requires an IfcSurfaceStyle");
    }
    const std::string requested = ifc_class ? ifc_class : "IfcSurfaceStyleShading";
    static const std::vector<std::string> supported = {
        "IfcSurfaceStyleShading",
        "IfcSurfaceStyleRendering",
        "IfcSurfaceStyleWithTextures",
        "IfcSurfaceStyleLighting",
        "IfcSurfaceStyleRefraction",
        "IfcExternallyDefinedSurfaceStyle",
    };
    if (std::find(supported.begin(), supported.end(), requested) == supported.end()) {
        throw std::runtime_error("Unsupported surface style class: '" + requested + "'.");
    }

    auto component = create_entity(file, requested.c_str());
    try {
        if (attributes) {
            for (const auto& entry : attributes->entries) {
                auto attribute_class = declared_attribute_type(component, entry.key);
                if (attribute_class == "IfcColourRgb") {
                    edit_colour_rgb(file, component, entry);
                } else if (entry.key == "SpecularHighlight") {
                    edit_specular_highlight(file, component, entry);
                } else if (attribute_class == "IfcColourOrFactor") {
                    edit_colour_or_factor(file, component, entry);
                } else {
                    ifcapi::detail::apply_attribute_prop(component, entry);
                }
            }
        }
    } catch (...) {
        remove_surface_style_impl(file, component);
        throw;
    }

    const bool shading_family = requested == "IfcSurfaceStyleShading" || requested == "IfcSurfaceStyleRendering";
    for (auto existing : ifcapi::detail::read_ref_aggregate(parent, "Styles")) {
        const bool conflicts = shading_family
                                   ? is_a(existing, "IfcSurfaceStyleShading")
                                   : existing.declaration().name() == requested;
        if (conflicts) {
            remove_surface_style_impl(file, existing);
        }
    }
    auto styles = ifcapi::detail::read_ref_aggregate(parent, "Styles");
    styles.push_back(component);
    ifcapi::detail::write_ref_aggregate(parent, "Styles", styles);
    return component;
}

std::vector<express::Base> style_add_surface_textures(
    ifcopenshell::file* file,
    const std::vector<StyleSurfaceTextureOptions>& descriptors,
    const std::vector<express::Base>& uv_maps) {
    if (!file) {
        throw std::runtime_error("style_add_surface_textures requires a file");
    }
    if (is_ifc2x3(file)) {
        return {};
    }
    std::vector<express::Base> result;
    result.reserve(descriptors.size());
    for (const auto& descriptor : descriptors) {
        auto texture = create_entity(file, "IfcImageTexture");
        ifcapi::detail::entity_view(texture).set("RepeatS", descriptor.repeat_s);
        ifcapi::detail::entity_view(texture).set("RepeatT", descriptor.repeat_t);
        if (descriptor.mode) {
            ifcapi::detail::write_string_attr(texture, "Mode", *descriptor.mode);
        }
        ifcapi::detail::write_string_attr(texture, "URLReference", descriptor.url_reference);
        if (descriptor.texture_transform) {
            ifcapi::detail::write_ref_attr(texture, "TextureTransform", *descriptor.texture_transform);
        }
        if (descriptor.parameter) {
            ifcapi::detail::entity_view(texture).set("Parameter", *descriptor.parameter);
        }

        const auto mapping = descriptor.uv_mode.value_or("");
        if (mapping == "Generated" || mapping == "Camera") {
            auto coordinates = create_entity(file, "IfcTextureCoordinateGenerator");
            ifcapi::detail::write_ref_aggregate(coordinates, "Maps", {texture});
            ifcapi::detail::write_string_attr(coordinates, "Mode", mapping == "Generated" ? "COORD" : "COORD-EYE");
        } else if (mapping == "UV") {
            for (auto uv_map : uv_maps) {
                auto maps = ifcapi::detail::read_ref_aggregate(uv_map, "Maps");
                if (!contains(maps, texture)) {
                    maps.push_back(texture);
                }
                ifcapi::detail::write_ref_aggregate(uv_map, "Maps", maps);
            }
        }
        result.push_back(texture);
    }
    return result;
}

express::Base style_assign_item_style_impl(
    ifcopenshell::file* file,
    express::Base item,
    express::Base style,
    bool should_use_presentation_style_assignment) {
    auto styled_items = inverse_entities(item, "StyledByItem");
    if (styled_items.empty()) {
        if (!style) {
            return {};
        }
        auto assigned_style = (is_ifc2x3(file) || should_use_presentation_style_assignment)
                                  ? create_presentation_style_assignment(file, style)
                                  : style;
        return create_styled_item(file, item, {assigned_style}, std::string());
    }

    auto styled_item = styled_items.front();
    auto styled_item_styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
    if (style && same_single_style(styled_item_styles, style)) {
        return styled_item;
    }

    if (is_ifc4x3(file)) {
        if (!style) {
            file->remove_entity(styled_item);
            return {};
        }
        ifcapi::detail::write_ref_aggregate(styled_item, "Styles", {style});
        return styled_item;
    }

    express::Base assignment = {};
    for (auto style_ : styled_item_styles) {
        if (!is_a(style_, "IfcPresentationStyleAssignment")) {
            continue;
        }
        if (!style || assignment) {
            file->remove_entity(style_);
        } else {
            assignment = style_;
            if (!same_single_style(ifcapi::detail::read_ref_aggregate(assignment, "Styles"), style)) {
                ifcapi::detail::write_ref_aggregate(assignment, "Styles", {style});
            }
        }
    }

    if (!style) {
        file->remove_entity(styled_item);
        return {};
    }

    if (assignment) {
        if (same_single_style(styled_item_styles, assignment)) {
            return styled_item;
        }
        ifcapi::detail::write_ref_aggregate(styled_item, "Styles", {assignment});
        return styled_item;
    }

    ifcapi::detail::write_ref_aggregate(styled_item, "Styles", {style});
    return styled_item;
}

std::vector<express::Base> style_assign_representation_styles_impl(
    ifcopenshell::file* file,
    express::Base shape_representation,
    const std::vector<express::Base>& input_styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style) {
    auto styles = mutable_entities(input_styles);
    if (styles.empty()) {
        return {};
    }
    std::vector<express::Base> remaining_styles = styles;
    std::vector<express::Base> results;
    bool use_style_assignment = is_ifc2x3(file) || should_use_presentation_style_assignment;
    express::Base style = {};
    auto traversed = file->traverse(shape_representation, -1);
    if (traversed.empty()) {
        return results;
    }
    for (auto element : traversed) {
        if (!is_a(element, "IfcShapeModel")) {
            continue;
        }
        for (auto item : ifcapi::detail::read_ref_aggregate(element, "Items")) {
            if (!is_a(item, "IfcGeometricRepresentationItem") && !is_a(item, "IfcTopologicalRepresentationItem")) {
                continue;
            }
            if (!remaining_styles.empty()) {
                style = remaining_styles.front();
                remaining_styles.erase(remaining_styles.begin());
            }
            if (!style) {
                continue;
            }
            std::string name = ifcapi::detail::read_string_attr(style, "Name");
            std::string current_style_type = style.declaration().name();
            auto styled_items = inverse_entities(item, "StyledByItem");
            auto prev_styled_item = styled_items.empty() ? express::Base() : styled_items.front();
            express::Base style_assignment = {};

            if (!prev_styled_item) {
                if (use_style_assignment) {
                    style_assignment = create_presentation_style_assignment(file, style);
                    results.push_back(create_styled_item(file, item, {style_assignment}, name));
                } else {
                    results.push_back(create_styled_item(file, item, {style}, name));
                }
                continue;
            }

            if (replace_previous_same_type_style) {
                remove_same_type_styles(file, prev_styled_item, current_style_type, false);
                auto prev_styles = ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles");
                for (auto style_ : prev_styles) {
                    if (is_a(style_, "IfcPresentationStyleAssignment")) {
                        if (use_style_assignment && !style_assignment) {
                            style_assignment = style_;
                            remove_same_type_styles(file, style_assignment, current_style_type, false);
                        } else {
                            remove_same_type_styles(file, style_assignment, current_style_type, true);
                        }
                    }
                }
                if (use_style_assignment) {
                    if (style_assignment) {
                        ifcapi::detail::write_ref_aggregate(
                            style_assignment,
                            "Styles",
                            append_ref(ifcapi::detail::read_ref_aggregate(style_assignment, "Styles"), style));
                    } else {
                        style_assignment = create_presentation_style_assignment(file, style);
                        ifcapi::detail::write_ref_aggregate(
                            prev_styled_item,
                            "Styles",
                            append_ref(ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles"), style_assignment));
                    }
                } else {
                    ifcapi::detail::write_ref_aggregate(
                        prev_styled_item,
                        "Styles",
                        append_ref(ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles"), style));
                }
                continue;
            }

            std::vector<express::Base> assigned_styles;
            for (auto style_ : ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles")) {
                if (is_a(style_, "IfcPresentationStyleAssignment")) {
                    if (!style_assignment) {
                        style_assignment = style_;
                    }
                    auto nested = ifcapi::detail::read_ref_aggregate(style_, "Styles");
                    assigned_styles.insert(assigned_styles.end(), nested.begin(), nested.end());
                } else {
                    assigned_styles.push_back(style_);
                }
            }
            if (contains(assigned_styles, style)) {
                continue;
            }
            if (use_style_assignment) {
                if (style_assignment) {
                    ifcapi::detail::write_ref_aggregate(
                        style_assignment,
                        "Styles",
                        append_ref(ifcapi::detail::read_ref_aggregate(style_assignment, "Styles"), style));
                } else {
                    style_assignment = create_presentation_style_assignment(file, style);
                    ifcapi::detail::write_ref_aggregate(
                        prev_styled_item,
                        "Styles",
                        append_ref(ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles"), style_assignment));
                }
            } else {
                ifcapi::detail::write_ref_aggregate(
                    prev_styled_item,
                    "Styles",
                    append_ref(ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles"), style));
            }
        }
    }
    return results;
}

void style_assign_material_style_impl(
    ifcopenshell::file* file,
    express::Base material,
    express::Base style,
    express::Base context,
    bool should_use_presentation_style_assignment) {
    auto active_style = (is_ifc2x3(file) || should_use_presentation_style_assignment)
                            ? create_presentation_style_assignment(file, style)
                            : style;

    auto definitions = inverse_entities(material, "HasRepresentation");
    if (!definitions.empty()) {
        auto definition_representation = definitions.front();
        auto representation = get_styled_representation(definition_representation, context);
        if (representation) {
            std::vector<express::Base> new_items;
            std::vector<express::Base> same_style_items;
            for (auto item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                if (!is_a(item, "IfcStyledItem")) {
                    continue;
                }
                if (has_proposed_style(file, item, style)) {
                    return;
                }
                if (has_same_style_type(item, style)) {
                    same_style_items.push_back(item);
                } else {
                    new_items.push_back(item);
                }
            }
            express::Base item_to_reuse = {};
            if (!same_style_items.empty()) {
                item_to_reuse = same_style_items.front();
                same_style_items.erase(same_style_items.begin());
            }
            new_items.push_back(create_styled_item_for_material(file, active_style, style, item_to_reuse));
            ifcapi::detail::write_ref_aggregate(representation, "Items", new_items);
            for (auto item : same_style_items) {
                if (file->get_total_inverses(static_cast<int>(item.id())) == 0) {
                    file->remove_entity(item);
                }
            }
        } else {
            auto representations = ifcapi::detail::read_ref_aggregate(definition_representation, "Representations");
            representations.push_back(create_styled_representation(file, context, active_style, style));
            ifcapi::detail::write_ref_aggregate(definition_representation, "Representations", representations);
        }
    } else {
        auto representation = create_styled_representation(file, context, active_style, style);
        auto definition = create_entity(file, "IfcMaterialDefinitionRepresentation");
        ifcapi::detail::write_ref_aggregate(definition, "Representations", {representation});
        ifcapi::detail::write_ref_attr(definition, "RepresentedMaterial", material);
    }

    std::vector<std::string> constituent_names;
    for (auto inverse : inverse_entities(file, material)) {
        if (is_a(inverse, "IfcMaterialConstituent")) {
            std::string name = ifcapi::detail::read_string_attr(inverse, "Name");
            if (!name.empty()) {
                constituent_names.push_back(name);
            }
        }
    }
    if (constituent_names.empty()) {
        return;
    }
    auto elements = ifcapi::bindings::element_get_elements_by_material(&material);
    for (auto element : elements) {
        auto shape_aspects = ifcapi::bindings::element_get_shape_aspects(&element, {true});
        for (auto shape_aspect : shape_aspects) {
            if (!contains_name(constituent_names, ifcapi::detail::read_string_attr(shape_aspect, "Name"))) {
                continue;
            }
            for (auto rep : ifcapi::detail::read_ref_aggregate(shape_aspect, "ShapeRepresentations")) {
                style_assign_representation_styles_impl(file, rep, {active_style}, false, true);
            }
        }
    }
}

void style_unassign_material_style_impl(
    ifcopenshell::file* file,
    express::Base material,
    express::Base style,
    express::Base context) {
    for (auto definition : inverse_entities(material, "HasRepresentation")) {
        for (auto representation : ifcapi::detail::read_ref_aggregate(definition, "Representations")) {
            if (!is_a(representation, "IfcStyledRepresentation") ||
                ifcapi::detail::read_ref_attr(representation, "ContextOfItems") != context) {
                continue;
            }
            for (auto item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                if (!is_a(item, "IfcStyledItem")) {
                    continue;
                }
                auto current = ifcapi::detail::read_ref_aggregate(item, "Styles");
                std::vector<express::Base> retained;
                for (auto item_style : current) {
                    if (item_style == style) {
                        continue;
                    }
                    if (is_a(item_style, "IfcPresentationStyleAssignment") &&
                        same_single_style(ifcapi::detail::read_ref_aggregate(item_style, "Styles"), style)) {
                        continue;
                    }
                    retained.push_back(item_style);
                }
                if (retained.empty()) {
                    file->remove_entity(item);
                } else if (retained.size() != current.size()) {
                    ifcapi::detail::write_ref_aggregate(item, "Styles", retained);
                }
            }
            if (ifcapi::detail::read_ref_aggregate(representation, "Items").empty()) {
                file->remove_entity(representation);
            }
        }
        if (ifcapi::detail::read_ref_aggregate(definition, "Representations").empty()) {
            file->remove_entity(definition);
        }
    }

    std::vector<std::string> constituent_names;
    for (auto inverse : inverse_entities(file, material)) {
        if (is_a(inverse, "IfcMaterialConstituent")) {
            std::string name = ifcapi::detail::read_string_attr(inverse, "Name");
            if (!name.empty()) {
                constituent_names.push_back(name);
            }
        }
    }
    if (constituent_names.empty()) {
        return;
    }
    auto elements = ifcapi::bindings::element_get_elements_by_material(&material);
    for (auto element : elements) {
        auto shape_aspects = ifcapi::bindings::element_get_shape_aspects(&element, {true});
        for (auto shape_aspect : shape_aspects) {
            if (!contains_name(constituent_names, ifcapi::detail::read_string_attr(shape_aspect, "Name"))) {
                continue;
            }
            for (auto rep : ifcapi::detail::read_ref_aggregate(shape_aspect, "ShapeRepresentations")) {
                style_unassign_representation_styles_impl(file, rep, {style}, false);
            }
        }
    }
}

void style_unassign_representation_styles_impl(
    ifcopenshell::file* file,
    express::Base shape_representation,
    const std::vector<express::Base>& input_styles,
    bool should_use_presentation_style_assignment) {
    auto styles = mutable_entities(input_styles);
    if (styles.empty()) {
        return;
    }
    bool use_style_assignment = is_ifc2x3(file) || should_use_presentation_style_assignment;
    auto traversed = file->traverse(shape_representation, -1);
    if (traversed.empty()) {
        return;
    }
    for (auto element : traversed) {
        if (!is_a(element, "IfcShapeRepresentation")) {
            continue;
        }
        for (auto item : ifcapi::detail::read_ref_aggregate(element, "Items")) {
            if (!is_a(item, "IfcGeometricRepresentationItem")) {
                continue;
            }
            auto styled_items = inverse_entities(item, "StyledByItem");
            if (styled_items.empty()) {
                continue;
            }
            auto styled_item = styled_items.front();
            if (use_style_assignment) {
                auto assigned_styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
                for (auto style : assigned_styles) {
                    if (is_a(style, "IfcPresentationStyleAssignment")) {
                        remove_styles(file, style, styles);
                    }
                }
            }
            remove_styles(file, styled_item, styles);
        }
    }
}

void style_remove_styled_representation_impl(
    ifcopenshell::file* file,
    express::Base representation) {
    for (auto inverse : inverse_entities(file, representation)) {
        if (is_a(inverse, "IfcMaterialDefinitionRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Representations").size() == 1) {
            file->remove_entity(inverse);
        }
    }

    for (auto item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
        if (is_a(item, "IfcStyledItem") && file->get_total_inverses(static_cast<int>(item.id())) == 1) {
            for (auto style : ifcapi::detail::read_ref_aggregate(item, "Styles")) {
                if (is_a(style, "IfcPresentationStyleAssignment")) {
                    file->remove_entity(style);
                }
            }
            file->remove_entity(item);
        }
    }
    file->remove_entity(representation);
}

void remove_surface_style_impl(ifcopenshell::file* file, express::Base style) {
    std::vector<express::Base> to_delete;
    if (is_a(style, "IfcSurfaceStyleWithTextures")) {
        auto textures = ifcapi::detail::read_ref_aggregate(style, "Textures");
        if (is_ifc2x3(file)) {
            to_delete.insert(to_delete.end(), textures.begin(), textures.end());
        } else {
            for (auto texture : textures) {
                auto coords = inverse_entities(texture, "IsMappedBy");
                if (!coords.empty()) {
                    to_delete.insert(to_delete.end(), coords.begin(), coords.end());
                } else {
                    to_delete.push_back(texture);
                }
            }
        }
    }

    if (style) {
        auto attrs = style.declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attrs.size(); ++i) {
            try {
                auto value = style.get_attribute_value(i);
                if (!value.isNull()) {
                    if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                        auto entity = static_cast<express::Base>(value);
                        if (entity && entity.id()) {
                            to_delete.push_back(entity);
                        }
                    } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                        auto aggregate = static_cast<std::vector<express::Base>>(value);
                        if (!aggregate.empty()) {
                            for (auto entity : aggregate) {
                                if (entity && entity.id()) {
                                    to_delete.push_back(entity);
                                }
                            }
                        }
                    }
                }
            } catch (...) {
            }
        }
    }

    file->remove_entity(style);
    for (auto element : to_delete) {
        if (element.id()) {
            try {
                file->instance_by_id(static_cast<int>(element.id()));
            } catch (const ifcopenshell::exception&) {
                continue;
            }
        }
        ifcapi::bindings::entity_remove_deep(&element);
    }
}

void style_remove_surface_style_impl_public(ifcopenshell::file* file, express::Base style) {
    remove_surface_style_impl(file, style);
}

void purge_material_definition_representations(ifcopenshell::file* file, express::Base styled_representation) {
    for (auto inverse : inverse_entities(file, styled_representation)) {
        if (is_a(inverse, "IfcMaterialDefinitionRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Representations").size() == 1) {
            file->remove_entity(inverse);
        }
    }
}

void purge_styled_representations(ifcopenshell::file* file, express::Base styled_item) {
    for (auto inverse : inverse_entities(file, styled_item)) {
        if (is_a(inverse, "IfcStyledRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Items").size() == 1) {
            purge_material_definition_representations(file, inverse);
            file->remove_entity(inverse);
        }
    }
}

void purge_fill_area_style_hatching(
    ifcopenshell::file* file,
    express::Base fill_area_style_hatching,
    express::Base style) {
    for (auto inverse : inverse_entities(file, fill_area_style_hatching)) {
        if (is_a(inverse, "IfcFillAreaStyle")) {
            remove_style_impl(file, inverse, {fill_area_style_hatching, style});
        }
    }
    ifcapi::bindings::entity_remove_deep_with_options(&fill_area_style_hatching, {{}, {style}});
}

void purge_inverses(ifcopenshell::file* file, express::Base style) {
    for (auto inverse : inverse_entities(file, style)) {
        if (is_a(inverse, "IfcStyledItem")) {
            if (ifcapi::detail::read_ref_aggregate(inverse, "Styles").size() == 1) {
                purge_styled_representations(file, inverse);
                file->remove_entity(inverse);
            }
        } else if (is_a(inverse, "IfcFillAreaStyleHatching")) {
            purge_fill_area_style_hatching(file, inverse, style);
        }
    }
}

void remove_style_impl(
    ifcopenshell::file* file,
    express::Base style,
    const std::vector<express::Base>& do_not_delete) {
    purge_inverses(file, style);
    if (is_a(style, "IfcSurfaceStyle")) {
        auto nested = ifcapi::detail::read_ref_aggregate(style, "Styles");
        for (auto style_item : nested) {
            remove_surface_style_impl(file, style_item);
        }
    } else if (is_a(style, "IfcFillAreaStyle")) {
        for (auto style_item : ifcapi::detail::read_ref_aggregate(style, "FillStyles")) {
            ifcapi::bindings::entity_remove_deep_with_options(&style_item, {{style}, do_not_delete});
        }
    }
    file->remove_entity(style);
}

void style_remove_style_impl_public(ifcopenshell::file* file, express::Base style) {
    remove_style_impl(file, style, {});
}

void style_edit_surface_style(
    ifcopenshell::file* file,
    express::Base* style,
    ifcopenshell_pset_props_t* attributes) {
    style_edit_surface_style_impl(file, ifcapi::detail::deref_or_empty(style), attributes);
}

void style_edit_presentation_style(
    ifcopenshell::file* file,
    express::Base presentation_style,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, presentation_style, attributes);
}

express::Base style_assign_item_style(
    ifcopenshell::file* file,
    const StyleAssignItemStyleOptions& options) {
    return style_assign_item_style_impl(
        file,
        options.item,
        options.style.value_or(express::Base()),
        options.should_use_presentation_style_assignment);
}

std::vector<express::Base> style_assign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style) {
    return style_assign_representation_styles_impl(
        file,
        ifcapi::detail::deref_or_empty(shape_representation),
        styles,
        should_use_presentation_style_assignment,
        replace_previous_same_type_style);
}

void style_assign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context,
    bool should_use_presentation_style_assignment) {
    style_assign_material_style_impl(
        file,
        ifcapi::detail::deref_or_empty(material),
        ifcapi::detail::deref_or_empty(style),
        ifcapi::detail::deref_or_empty(context),
        should_use_presentation_style_assignment);
}

void style_unassign_material_style(
    ifcopenshell::file* file,
    express::Base* material,
    express::Base* style,
    express::Base* context) {
    style_unassign_material_style_impl(
        file,
        ifcapi::detail::deref_or_empty(material),
        ifcapi::detail::deref_or_empty(style),
        ifcapi::detail::deref_or_empty(context));
}

void style_unassign_representation_styles(
    ifcopenshell::file* file,
    express::Base* shape_representation,
    const std::vector<express::Base>& styles,
    bool should_use_presentation_style_assignment) {
    style_unassign_representation_styles_impl(
        file,
        ifcapi::detail::deref_or_empty(shape_representation),
        styles,
        should_use_presentation_style_assignment);
}

void style_remove_styled_representation(ifcopenshell::file* file, express::Base* representation) {
    style_remove_styled_representation_impl(file, ifcapi::detail::deref_or_empty(representation));
}

void style_remove_surface_style(ifcopenshell::file* file, express::Base* style) {
    remove_surface_style_impl(file, ifcapi::detail::deref_or_empty(style));
}

void style_remove_style(ifcopenshell::file* file, express::Base* style) {
    remove_style_impl(file, ifcapi::detail::deref_or_empty(style), {});
}

} // namespace bindings
} // namespace ifcapi
