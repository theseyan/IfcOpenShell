// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/style.h"
#include "ifcapi/detail/attribute.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"
#include "ifcparse/IfcFile.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_ifc4x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name().rfind("IFC4X3", 0) == 0;
}

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || entity->id() <= 0) return result;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* inverse : *inverses) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const char* attribute) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return result;
    try {
        auto inverses = base->get_inverse(attribute);
        if (!inverses) return result;
        for (size_t i = 0; i < inverses->size(); ++i) {
            if ((*inverses)[i]) result.push_back((*inverses)[i]);
        }
    } catch (...) {
    }
    return result;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool contains_name(const std::vector<std::string>& values, const std::string& value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool same_single_style(const std::vector<IfcUtil::IfcBaseClass*>& styles, IfcUtil::IfcBaseClass* style) {
    return styles.size() == 1 && styles.front() == style;
}

const ifcapi_pset::Entry* nested_entry(const ifcapi_pset::Entry& entry, const std::string& key) {
    if (!entry.nested) return nullptr;
    for (const auto& nested : entry.nested->entries) {
        if (nested.key == key) return &nested;
    }
    return nullptr;
}

bool entry_truthy(const ifcapi_pset::Entry* entry) {
    if (!entry) return false;
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
            return entry->inst != nullptr;
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

void write_optional_colour_name(IfcUtil::IfcBaseClass* colour, const ifcapi_pset::Entry* name) {
    if (!name || name->kind == ifcapi_pset::Kind::NONE) {
        ifcapi::detail::write_blank_attr(colour, "Name");
    } else {
        ifcapi::detail::write_string_attr(colour, "Name", name->s_val);
    }
}

void write_colour_components(IfcUtil::IfcBaseClass* colour, const ifcapi_pset::Entry& value, bool include_name) {
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

std::string declared_attribute_type(IfcUtil::IfcBaseClass* entity, const std::string& name) {
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return std::string();
    auto* decl = base->declaration().as_entity();
    if (!decl) return std::string();
    auto attrs = decl->all_attributes();
    for (auto* attr : attrs) {
        if (attr->name() != name) continue;
        const IfcParse::parameter_type* pt = attr->type_of_attribute();
        if (auto* aggregate = pt ? pt->as_aggregation_type() : nullptr) {
            pt = aggregate->type_of_element();
        }
        auto* named = pt ? pt->as_named_type() : nullptr;
        auto* declared = named ? named->declared_type() : nullptr;
        return declared ? declared->name() : std::string();
    }
    return std::string();
}

IfcUtil::IfcBaseClass* create_typed_double(IfcParse::IfcFile* file, const char* ifc_type, double value) {
    auto* result = ifcapi::detail::create_typed_double(file, ifc_type, value);
    if (!result) {
        throw std::runtime_error(std::string("Unable to create ") + ifc_type);
    }
    return result;
}

void edit_colour_rgb(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style, const ifcapi_pset::Entry& entry) {
    auto* colour = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
    if (!colour) {
        colour = create_entity(file, "IfcColourRgb");
        ifcapi::detail::write_ref_attr(style, entry.key.c_str(), colour);
    }
    write_colour_components(colour, entry, true);
}

void edit_colour_or_factor(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style, const ifcapi_pset::Entry& entry) {
    if (entry.kind == ifcapi_pset::Kind::DICT) {
        auto* colour = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
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

    auto* existing = ifcapi::detail::read_ref_attr(style, entry.key.c_str());
    if (existing && existing->id()) {
        file->removeEntity(existing);
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

void edit_specular_highlight(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style, const ifcapi_pset::Entry& entry) {
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

std::vector<IfcUtil::IfcBaseClass*> mutable_entities(
    const std::vector<const IfcUtil::IfcBaseClass*>& entities)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(entities.size());
    for (auto* entity : entities) {
        if (entity) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(entity));
    }
    return result;
}

void remove_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    const std::vector<IfcUtil::IfcBaseClass*>& styles)
{
    auto current = ifcapi::detail::read_ref_aggregate(item, "Styles");
    std::vector<IfcUtil::IfcBaseClass*> retained;
    retained.reserve(current.size());
    for (auto* style : current) {
        if (!contains(styles, style)) retained.push_back(style);
    }
    if (retained.empty()) {
        file->removeEntity(item);
    } else if (retained.size() != current.size()) {
        ifcapi::detail::write_ref_aggregate(item, "Styles", retained);
    }
}

IfcUtil::IfcBaseClass* create_presentation_style_assignment(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style) {
    auto* assignment = create_entity(file, "IfcPresentationStyleAssignment");
    ifcapi::detail::write_ref_aggregate(assignment, "Styles", {style});
    return assignment;
}

IfcUtil::IfcBaseClass* create_styled_item(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    const std::vector<IfcUtil::IfcBaseClass*>& styles,
    const std::string& name)
{
    auto* styled_item = create_entity(file, "IfcStyledItem");
    if (item) ifcapi::detail::write_ref_attr(styled_item, "Item", item);
    ifcapi::detail::write_ref_aggregate(styled_item, "Styles", styles);
    ifcapi::detail::write_string_attr(styled_item, "Name", name);
    return styled_item;
}

std::vector<IfcUtil::IfcBaseClass*> append_ref(
    std::vector<IfcUtil::IfcBaseClass*> values,
    IfcUtil::IfcBaseClass* value)
{
    values.push_back(value);
    return values;
}

void remove_same_type_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* style_item,
    const std::string& current_style_type,
    bool remove_item)
{
    if (!style_item) return;
    auto current = ifcapi::detail::read_ref_aggregate(style_item, "Styles");
    std::vector<IfcUtil::IfcBaseClass*> retained;
    for (auto* style : current) {
        if (!is_a(style, current_style_type.c_str())) retained.push_back(style);
    }
    if (remove_item && retained.empty()) {
        file->removeEntity(style_item);
    } else {
        ifcapi::detail::write_ref_aggregate(style_item, "Styles", retained);
    }
}

bool has_proposed_style(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* styled_item, IfcUtil::IfcBaseClass* style) {
    auto styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
    if (contains(styles, style)) return true;
    if (!is_ifc4x3(file)) {
        for (auto* s : styles) {
            if (is_a(s, "IfcPresentationStyleAssignment") &&
                contains(ifcapi::detail::read_ref_aggregate(s, "Styles"), style)) {
                return true;
            }
        }
    }
    return false;
}

bool has_same_style_type(IfcUtil::IfcBaseClass* styled_item, IfcUtil::IfcBaseClass* style) {
    auto style_class = style ? style->declaration().name() : std::string();
    for (auto* s : ifcapi::detail::read_ref_aggregate(styled_item, "Styles")) {
        if (s->declaration().name() == style_class) return true;
        if (is_a(s, "IfcPresentationStyleAssignment")) {
            for (auto* ss : ifcapi::detail::read_ref_aggregate(s, "Styles")) {
                if (ss->declaration().name() == style_class) return true;
            }
        }
    }
    return false;
}

} // namespace

namespace ifcapi {
namespace bindings {

void remove_surface_style_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style);
void remove_style_impl(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* style,
    const std::vector<const IfcUtil::IfcBaseClass*>& do_not_delete);
void style_unassign_representation_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* shape_representation,
    const std::vector<const IfcUtil::IfcBaseClass*>& input_styles,
    bool should_use_presentation_style_assignment);

void style_edit_surface_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* style,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !style) {
            throw std::runtime_error("style_edit_surface_style requires a file and style");
        }
        if (!attributes) return;
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
        ifcopenshell::capi::set_last_error(e.what());
    }
}

IfcUtil::IfcBaseClass* create_styled_item_for_material(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass*& active_style,
    IfcUtil::IfcBaseClass* public_style,
    IfcUtil::IfcBaseClass* reuse_item)
{
    if (!reuse_item) {
        return create_styled_item(file, nullptr, {active_style}, ifcapi::detail::read_string_attr(public_style, "Name"));
    }
    if (is_a(reuse_item, "IfcPresentationStyleAssignment") && is_a(active_style, "IfcPresentationStyleAssignment")) {
        file->removeEntity(active_style);
        active_style = reuse_item;
    }
    ifcapi::detail::write_ref_aggregate(reuse_item, "Styles", {public_style});
    ifcapi::detail::write_string_attr(reuse_item, "Name", ifcapi::detail::read_string_attr(public_style, "Name"));
    return reuse_item;
}

IfcUtil::IfcBaseClass* get_styled_representation(
    IfcUtil::IfcBaseClass* definition_representation,
    IfcUtil::IfcBaseClass* context)
{
    for (auto* representation : ifcapi::detail::read_ref_aggregate(definition_representation, "Representations")) {
        if (is_a(representation, "IfcStyledRepresentation") &&
            ifcapi::detail::read_ref_attr(representation, "ContextOfItems") == context) {
            return representation;
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* create_styled_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* context,
    IfcUtil::IfcBaseClass*& active_style,
    IfcUtil::IfcBaseClass* public_style)
{
    auto* representation = create_entity(file, "IfcStyledRepresentation");
    ifcapi::detail::write_ref_attr(representation, "ContextOfItems", context);
    ifcapi::detail::write_string_attr(
        representation,
        "RepresentationIdentifier",
        ifcapi::detail::read_string_attr(context, "ContextIdentifier"));
    ifcapi::detail::write_ref_aggregate(
        representation,
        "Items",
        {create_styled_item_for_material(file, active_style, public_style, nullptr)});
    return representation;
}

IfcUtil::IfcBaseClass* style_add_style(
    IfcParse::IfcFile* file,
    const char* name,
    const std::string& ifc_class)
{
    auto* style = create_entity(file, ifc_class.c_str());
    if (name) ifcapi::detail::write_string_attr(style, "Name", name);
    if (ifc_class == "IfcSurfaceStyle") {
        ifcapi::detail::write_enum_attr(style, "Side", "BOTH");
    }
    return style;
}

IfcUtil::IfcBaseClass* style_assign_item_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* item,
    IfcUtil::IfcBaseClass* style,
    bool should_use_presentation_style_assignment)
{
    auto styled_items = inverse_entities(item, "StyledByItem");
    if (styled_items.empty()) {
        if (!style) return nullptr;
        auto* assigned_style = (is_ifc2x3(file) || should_use_presentation_style_assignment)
            ? create_presentation_style_assignment(file, style)
            : style;
        return create_styled_item(file, item, {assigned_style}, std::string());
    }

    auto* styled_item = styled_items.front();
    auto styled_item_styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
    if (style && same_single_style(styled_item_styles, style)) {
        return styled_item;
    }

    if (is_ifc4x3(file)) {
        if (!style) {
            file->removeEntity(styled_item);
            return nullptr;
        }
        ifcapi::detail::write_ref_aggregate(styled_item, "Styles", {style});
        return styled_item;
    }

    IfcUtil::IfcBaseClass* assignment = nullptr;
    for (auto* style_ : styled_item_styles) {
        if (!is_a(style_, "IfcPresentationStyleAssignment")) continue;
        if (!style || assignment) {
            file->removeEntity(style_);
        } else {
            assignment = style_;
            if (!same_single_style(ifcapi::detail::read_ref_aggregate(assignment, "Styles"), style)) {
                ifcapi::detail::write_ref_aggregate(assignment, "Styles", {style});
            }
        }
    }

    if (!style) {
        file->removeEntity(styled_item);
        return nullptr;
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

std::vector<IfcUtil::IfcBaseClass*> style_assign_representation_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* shape_representation,
    const std::vector<const IfcUtil::IfcBaseClass*>& input_styles,
    bool should_use_presentation_style_assignment,
    bool replace_previous_same_type_style)
{
    auto styles = mutable_entities(input_styles);
    if (styles.empty()) return {};
    std::vector<IfcUtil::IfcBaseClass*> remaining_styles = styles;
    std::vector<IfcUtil::IfcBaseClass*> results;
    bool use_style_assignment = is_ifc2x3(file) || should_use_presentation_style_assignment;
    IfcUtil::IfcBaseClass* style = nullptr;
    auto traversed = file->traverse(shape_representation, -1);
    if (!traversed) return results;
    for (auto* element : *traversed) {
        if (!is_a(element, "IfcShapeModel")) continue;
        for (auto* item : ifcapi::detail::read_ref_aggregate(element, "Items")) {
            if (!is_a(item, "IfcGeometricRepresentationItem") && !is_a(item, "IfcTopologicalRepresentationItem")) {
                continue;
            }
            if (!remaining_styles.empty()) {
                style = remaining_styles.front();
                remaining_styles.erase(remaining_styles.begin());
            }
            if (!style) continue;
            std::string name = ifcapi::detail::read_string_attr(style, "Name");
            std::string current_style_type = style->declaration().name();
            auto styled_items = inverse_entities(item, "StyledByItem");
            auto* prev_styled_item = styled_items.empty() ? nullptr : styled_items.front();
            IfcUtil::IfcBaseClass* style_assignment = nullptr;

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
                for (auto* style_ : prev_styles) {
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

            std::vector<IfcUtil::IfcBaseClass*> assigned_styles;
            for (auto* style_ : ifcapi::detail::read_ref_aggregate(prev_styled_item, "Styles")) {
                if (is_a(style_, "IfcPresentationStyleAssignment")) {
                    if (!style_assignment) style_assignment = style_;
                    auto nested = ifcapi::detail::read_ref_aggregate(style_, "Styles");
                    assigned_styles.insert(assigned_styles.end(), nested.begin(), nested.end());
                } else {
                    assigned_styles.push_back(style_);
                }
            }
            if (contains(assigned_styles, style)) continue;
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

void style_assign_material_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* style,
    IfcUtil::IfcBaseClass* context,
    bool should_use_presentation_style_assignment)
{
    auto* active_style = (is_ifc2x3(file) || should_use_presentation_style_assignment)
        ? create_presentation_style_assignment(file, style)
        : style;

    auto definitions = inverse_entities(material, "HasRepresentation");
    if (!definitions.empty()) {
        auto* definition_representation = definitions.front();
        auto* representation = get_styled_representation(definition_representation, context);
        if (representation) {
            std::vector<IfcUtil::IfcBaseClass*> new_items;
            std::vector<IfcUtil::IfcBaseClass*> same_style_items;
            for (auto* item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                if (!is_a(item, "IfcStyledItem")) continue;
                if (has_proposed_style(file, item, style)) return;
                if (has_same_style_type(item, style)) {
                    same_style_items.push_back(item);
                } else {
                    new_items.push_back(item);
                }
            }
            IfcUtil::IfcBaseClass* item_to_reuse = nullptr;
            if (!same_style_items.empty()) {
                item_to_reuse = same_style_items.front();
                same_style_items.erase(same_style_items.begin());
            }
            new_items.push_back(create_styled_item_for_material(file, active_style, style, item_to_reuse));
            ifcapi::detail::write_ref_aggregate(representation, "Items", new_items);
            for (auto* item : same_style_items) {
                if (file->getTotalInverses(item->id()) == 0) file->removeEntity(item);
            }
        } else {
            auto representations = ifcapi::detail::read_ref_aggregate(definition_representation, "Representations");
            representations.push_back(create_styled_representation(file, context, active_style, style));
            ifcapi::detail::write_ref_aggregate(definition_representation, "Representations", representations);
        }
    } else {
        auto* representation = create_styled_representation(file, context, active_style, style);
        auto* definition = create_entity(file, "IfcMaterialDefinitionRepresentation");
        ifcapi::detail::write_ref_aggregate(definition, "Representations", {representation});
        ifcapi::detail::write_ref_attr(definition, "RepresentedMaterial", material);
    }

    std::vector<std::string> constituent_names;
    for (auto* inverse : inverse_entities(file, material)) {
        if (is_a(inverse, "IfcMaterialConstituent")) {
            std::string name = ifcapi::detail::read_string_attr(inverse, "Name");
            if (!name.empty()) constituent_names.push_back(name);
        }
    }
    if (constituent_names.empty()) return;
    auto elements = ifcapi::bindings::element_get_elements_by_material(material);
    if (!elements) return;
    for (auto* element : *elements) {
        auto shape_aspects = ifcapi::bindings::element_get_shape_aspects(element, true);
        if (!shape_aspects) continue;
        for (auto* shape_aspect : *shape_aspects) {
            if (!contains_name(constituent_names, ifcapi::detail::read_string_attr(shape_aspect, "Name"))) continue;
            for (auto* rep : ifcapi::detail::read_ref_aggregate(shape_aspect, "ShapeRepresentations")) {
                style_assign_representation_styles(file, rep, {active_style}, false, true);
            }
        }
    }
}

void style_unassign_material_style(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* style,
    IfcUtil::IfcBaseClass* context)
{
    for (auto* definition : inverse_entities(material, "HasRepresentation")) {
        for (auto* representation : ifcapi::detail::read_ref_aggregate(definition, "Representations")) {
            if (!is_a(representation, "IfcStyledRepresentation") ||
                ifcapi::detail::read_ref_attr(representation, "ContextOfItems") != context) {
                continue;
            }
            for (auto* item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                if (!is_a(item, "IfcStyledItem")) continue;
                auto current = ifcapi::detail::read_ref_aggregate(item, "Styles");
                std::vector<IfcUtil::IfcBaseClass*> retained;
                for (auto* item_style : current) {
                    if (item_style == style) continue;
                    if (is_a(item_style, "IfcPresentationStyleAssignment") &&
                        same_single_style(ifcapi::detail::read_ref_aggregate(item_style, "Styles"), style)) {
                        continue;
                    }
                    retained.push_back(item_style);
                }
                if (retained.empty()) {
                    file->removeEntity(item);
                } else if (retained.size() != current.size()) {
                    ifcapi::detail::write_ref_aggregate(item, "Styles", retained);
                }
            }
            if (ifcapi::detail::read_ref_aggregate(representation, "Items").empty()) {
                file->removeEntity(representation);
            }
        }
        if (ifcapi::detail::read_ref_aggregate(definition, "Representations").empty()) {
            file->removeEntity(definition);
        }
    }

    std::vector<std::string> constituent_names;
    for (auto* inverse : inverse_entities(file, material)) {
        if (is_a(inverse, "IfcMaterialConstituent")) {
            std::string name = ifcapi::detail::read_string_attr(inverse, "Name");
            if (!name.empty()) constituent_names.push_back(name);
        }
    }
    if (constituent_names.empty()) return;
    auto elements = ifcapi::bindings::element_get_elements_by_material(material);
    if (!elements) return;
    for (auto* element : *elements) {
        auto shape_aspects = ifcapi::bindings::element_get_shape_aspects(element, true);
        if (!shape_aspects) continue;
        for (auto* shape_aspect : *shape_aspects) {
            if (!contains_name(constituent_names, ifcapi::detail::read_string_attr(shape_aspect, "Name"))) continue;
            for (auto* rep : ifcapi::detail::read_ref_aggregate(shape_aspect, "ShapeRepresentations")) {
                style_unassign_representation_styles(file, rep, {style}, false);
            }
        }
    }
}

void style_unassign_representation_styles(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* shape_representation,
    const std::vector<const IfcUtil::IfcBaseClass*>& input_styles,
    bool should_use_presentation_style_assignment)
{
    auto styles = mutable_entities(input_styles);
    if (styles.empty()) return;
    bool use_style_assignment = is_ifc2x3(file) || should_use_presentation_style_assignment;
    auto traversed = file->traverse(shape_representation, -1);
    if (!traversed) return;
    for (auto* element : *traversed) {
        if (!is_a(element, "IfcShapeRepresentation")) continue;
        for (auto* item : ifcapi::detail::read_ref_aggregate(element, "Items")) {
            if (!is_a(item, "IfcGeometricRepresentationItem")) continue;
            auto styled_items = inverse_entities(item, "StyledByItem");
            if (styled_items.empty()) continue;
            auto* styled_item = styled_items.front();
            if (use_style_assignment) {
                auto assigned_styles = ifcapi::detail::read_ref_aggregate(styled_item, "Styles");
                for (auto* style : assigned_styles) {
                    if (is_a(style, "IfcPresentationStyleAssignment")) {
                        remove_styles(file, style, styles);
                    }
                }
            }
            remove_styles(file, styled_item, styles);
        }
    }
}

void style_remove_styled_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation)
{
    for (auto* inverse : inverse_entities(file, representation)) {
        if (is_a(inverse, "IfcMaterialDefinitionRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Representations").size() == 1) {
            file->removeEntity(inverse);
        }
    }

    for (auto* item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
        if (is_a(item, "IfcStyledItem") && file->getTotalInverses(item->id()) == 1) {
            for (auto* style : ifcapi::detail::read_ref_aggregate(item, "Styles")) {
                if (is_a(style, "IfcPresentationStyleAssignment")) {
                    file->removeEntity(style);
                }
            }
            file->removeEntity(item);
        }
    }
    file->removeEntity(representation);
}

void remove_surface_style_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style) {
    std::vector<IfcUtil::IfcBaseClass*> to_delete;
    if (is_a(style, "IfcSurfaceStyleWithTextures")) {
        auto textures = ifcapi::detail::read_ref_aggregate(style, "Textures");
        if (is_ifc2x3(file)) {
            to_delete.insert(to_delete.end(), textures.begin(), textures.end());
        } else {
            for (auto* texture : textures) {
                auto coords = inverse_entities(texture, "IsMappedBy");
                if (!coords.empty()) {
                    to_delete.insert(to_delete.end(), coords.begin(), coords.end());
                } else {
                    to_delete.push_back(texture);
                }
            }
        }
    }

    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(style);
    if (base) {
        auto attrs = base->declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attrs.size(); ++i) {
            try {
                auto value = style->get_attribute_value(i);
                if (!value.isNull()) {
                    if (value.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
                        auto* entity = static_cast<IfcUtil::IfcBaseClass*>(value);
                        if (entity && entity->id()) to_delete.push_back(entity);
                    } else if (value.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                        auto aggregate = static_cast<aggregate_of_instance::ptr>(value);
                        if (aggregate) {
                            for (auto* entity : *aggregate) {
                                if (entity && entity->id()) to_delete.push_back(entity);
                            }
                        }
                    }
                }
            } catch (...) {
            }
        }
    }

    file->removeEntity(style);
    for (auto* element : to_delete) {
        ifcapi::bindings::entity_remove_deep2(element);
    }
}

void style_remove_surface_style(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style) {
    remove_surface_style_impl(file, style);
}

void purge_material_definition_representations(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* styled_representation) {
    for (auto* inverse : inverse_entities(file, styled_representation)) {
        if (is_a(inverse, "IfcMaterialDefinitionRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Representations").size() == 1) {
            file->removeEntity(inverse);
        }
    }
}

void purge_styled_representations(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* styled_item) {
    for (auto* inverse : inverse_entities(file, styled_item)) {
        if (is_a(inverse, "IfcStyledRepresentation") &&
            ifcapi::detail::read_ref_aggregate(inverse, "Items").size() == 1) {
            purge_material_definition_representations(file, inverse);
            file->removeEntity(inverse);
        }
    }
}

void purge_fill_area_style_hatching(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* fill_area_style_hatching,
    IfcUtil::IfcBaseClass* style)
{
    for (auto* inverse : inverse_entities(file, fill_area_style_hatching)) {
        if (is_a(inverse, "IfcFillAreaStyle")) {
            remove_style_impl(file, inverse, {fill_area_style_hatching, style});
        }
    }
    ifcapi::bindings::entity_remove_deep2_ex(fill_area_style_hatching, {}, {style});
}

void purge_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style) {
    for (auto* inverse : inverse_entities(file, style)) {
        if (is_a(inverse, "IfcStyledItem")) {
            if (ifcapi::detail::read_ref_aggregate(inverse, "Styles").size() == 1) {
                purge_styled_representations(file, inverse);
                file->removeEntity(inverse);
            }
        } else if (is_a(inverse, "IfcFillAreaStyleHatching")) {
            purge_fill_area_style_hatching(file, inverse, style);
        }
    }
}

void remove_style_impl(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* style,
    const std::vector<const IfcUtil::IfcBaseClass*>& do_not_delete)
{
    purge_inverses(file, style);
    if (is_a(style, "IfcSurfaceStyle")) {
        auto nested = ifcapi::detail::read_ref_aggregate(style, "Styles");
        for (auto* style_item : nested) {
            remove_surface_style_impl(file, style_item);
        }
    } else if (is_a(style, "IfcFillAreaStyle")) {
        for (auto* style_item : ifcapi::detail::read_ref_aggregate(style, "FillStyles")) {
            ifcapi::bindings::entity_remove_deep2_ex(style_item, {style}, do_not_delete);
        }
    }
    file->removeEntity(style);
}

void style_remove_style(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* style) {
    remove_style_impl(file, style, {});
}

} // namespace bindings
} // namespace ifcapi
