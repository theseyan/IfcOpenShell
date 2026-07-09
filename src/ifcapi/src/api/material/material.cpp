// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/material.h"
#include "ifcapi/bindings/owner.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/geometry.h"
#include "ifcapi/detail/relationship.h"
#include "../pset/attribute_props.hpp"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, express::Base entity);
std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute);

int read_optional_int_from_props(ifcopenshell_pset_props_t* props, const char* key, int fallback, bool& found) {
    found = false;
    if (!props) return fallback;
    for (const auto& entry : props->entries) {
        if (entry.key != key) continue;
        found = true;
        if (entry.kind == ifcapi_pset::Kind::INT || entry.kind == ifcapi_pset::Kind::TYPED_INT) {
            return static_cast<int>(entry.i_val);
        }
        if (entry.kind == ifcapi_pset::Kind::DOUBLE || entry.kind == ifcapi_pset::Kind::TYPED_DOUBLE) {
            return static_cast<int>(entry.d_val);
        }
        return fallback;
    }
    return fallback;
}

express::Base usage_profile(express::Base usage) {
    auto material_set = ifcapi::detail::read_ref_attr(usage, "ForProfileSet");
    auto profile = ifcapi::detail::read_ref_attr(material_set, "CompositeProfile");
    if (profile) {
        return profile;
    }
    auto profiles = ifcapi::detail::read_ref_aggregate(material_set, "MaterialProfiles");
    if (!profiles.empty()) {
        return ifcapi::detail::read_ref_attr(profiles.front(), "Profile");
    }
    return {};
}

express::Base cardinal_point_position(ifcopenshell::file* file, int cardinal_point, double width, double height) {
    std::vector<double> location{0.0, 0.0, 0.0};
    if (cardinal_point == 1) location = {-width / 2.0, height / 2.0, 0.0};
    else if (cardinal_point == 2) location = {0.0, height / 2.0, 0.0};
    else if (cardinal_point == 3) location = {width / 2.0, height / 2.0, 0.0};
    else if (cardinal_point == 4) location = {-width / 2.0, 0.0, 0.0};
    else if (cardinal_point == 5) location = {0.0, 0.0, 0.0};
    else if (cardinal_point == 6) location = {width / 2.0, 0.0, 0.0};
    else if (cardinal_point == 7) location = {-width / 2.0, -height / 2.0, 0.0};
    else if (cardinal_point == 8) location = {0.0, -height / 2.0, 0.0};
    else if (cardinal_point == 9) location = {width / 2.0, -height / 2.0, 0.0};
    else return {};
    auto placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    ifcapi::detail::write_ref_attr(placement, "Location", ifcapi::detail::create_cartesian_point(file, location));
    return placement;
}

void update_profile_usage_representation(
    ifcopenshell::file* file,
    express::Base element,
    express::Base profile,
    express::Base position)
{
    auto representation = ifcapi::bindings::representation_get_product_representation(
        &element,
        {{}, "Model", "Body", "MODEL_VIEW"});
    if (!representation) {
        return;
    }
    auto traversed = file->traverse(representation, -1);
    if (traversed.empty()) {
        return;
    }
    for (auto subelement : traversed) {
        if (is_a(subelement, "IfcSweptAreaSolid") && ifcapi::detail::read_ref_attr(subelement, "SweptArea") == profile) {
            ifcapi::detail::write_ref_attr(subelement, "Position", position);
        }
    }
}

void update_profile_usage_cardinal_point(
    ifcopenshell::file* file,
    express::Base usage,
    express::Base profile,
    int cardinal_point,
    double profile_width,
    double profile_height)
{
    auto position = cardinal_point_position(file, cardinal_point, profile_width, profile_height);
    if (is_ifc2x3(file)) {
        for (auto rel : inverse_entities(file, usage)) {
            if (!is_a(rel, "IfcRelAssociatesMaterial")) continue;
            for (auto element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                update_profile_usage_representation(file, element, profile, position);
            }
        }
    } else {
        for (auto rel : inverse_entities(usage, "AssociatedTo")) {
            for (auto element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                update_profile_usage_representation(file, element, profile, position);
            }
        }
    }
}

std::string exact_class_name(express::Base entity) {
    return entity ? entity.declaration().name() : std::string();
}

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() <= 0) return result;
    for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

std::vector<express::Base> mutable_entities(
    const std::vector<express::Base>& entities)
{
    std::vector<express::Base> result;
    result.reserve(entities.size());
    for (auto entity : entities) if (entity) result.push_back(entity);
    return result;
}

std::vector<express::Base> dedupe_entities(std::vector<express::Base> entities) {
    std::vector<express::Base> result;
    std::unordered_set<express::Base> seen;
    result.reserve(entities.size());
    for (auto entity : entities) {
        if (entity && seen.insert(entity).second) {
            result.push_back(entity);
        }
    }
    return result;
}

bool contains(const std::vector<express::Base>& values, express::Base value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

void remove_first(std::vector<express::Base>& values, express::Base value) {
    auto it = std::find(values.begin(), values.end(), value);
    if (it != values.end()) {
        values.erase(it);
    }
}

void write_optional_string(express::Base entity, const char* attr, const char* value) {
    if (value) {
        ifcapi::detail::write_string_attr(entity, attr, value);
    }
}

express::Base create_material_association(
    ifcopenshell::file* file,
    express::Base relating_material,
    const std::vector<express::Base>& products,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto rel = create_entity(file, "IfcRelAssociatesMaterial");
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", products);
    ifcapi::detail::write_ref_attr(rel, "RelatingMaterial", relating_material);
    return rel;
}

express::Base get_rel_associates_material(
    ifcopenshell::file* file,
    express::Base material,
    express::Base original_material)
{
    auto target = original_material ? original_material : material;
    if (is_ifc2x3(file) || is_a(material, "IfcMaterialList")) {
        auto rels = ifcapi::detail::instances_by_type(file, "IfcRelAssociatesMaterial");
        for (auto rel : rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingMaterial") == target) {
                return rel;
            }
        }
        return {};
    }
    auto associated = inverse_entities(material, "AssociatedTo");
    return associated.empty() ? express::Base() : associated.front();
}

express::Base material_for_product(express::Base product) {
    return ifcapi::bindings::element_get_material(&product, {false, true}).value_or(express::Base());
}

express::Base type_for_product(express::Base product) {
    return ifcapi::bindings::element_get_type(&product).value_or(express::Base());
}

std::vector<express::Base> association_rels_for_product(ifcopenshell::file* file, express::Base product) {
    std::vector<express::Base> result;
    for (auto inverse : inverse_entities(file, product)) {
        if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            result.push_back(inverse);
        }
    }
    return result;
}

void remove_with_history(ifcopenshell::file* file, express::Base rel) {
    ifcapi::detail::remove_with_history(file, rel);
}

bool is_axis3_class(express::Base product) {
    static const std::unordered_set<std::string> axis3 = {
        "IfcSlab",
        "IfcSlabStandardCase",
        "IfcSlabElementedCase",
        "IfcRoof",
        "IfcRamp",
        "IfcPlate",
        "IfcPlateStandardCase",
        "IfcCovering",
        "IfcFurniture",
    };
    return axis3.find(exact_class_name(product)) != axis3.end();
}

express::Base find_material_set_for_type(
    ifcopenshell::file* file,
    express::Base product,
    const char* material_set_class,
    const char* fallback_class)
{
    auto element_type = type_for_product(product);
    auto type_material = element_type ? material_for_product(element_type) : express::Base();
    if (is_a(type_material, material_set_class)) {
        return type_material;
    }
    return file ? create_entity(file, fallback_class) : express::Base();
}

void update_representation_profile(
    ifcopenshell::file* file,
    express::Base material_set,
    const std::vector<express::Base>& products)
{
    auto profile = ifcapi::detail::read_ref_attr(material_set, "CompositeProfile");
    if (!profile) {
        auto profiles = ifcapi::detail::read_ref_aggregate(material_set, "MaterialProfiles");
        if (!profiles.empty()) {
            profile = ifcapi::detail::read_ref_attr(profiles.front(), "Profile");
        }
    }
    if (!profile) return;
    for (auto product : products) {
        auto representation = ifcapi::bindings::representation_get_product_representation(
            &product,
            {{}, "Model", "Body", "MODEL_VIEW"});
        if (!representation) return;
        auto traversed = file->traverse(representation, -1);
        if (!traversed.empty()) {
            for (auto subelement : traversed) {
                if (is_a(subelement, "IfcSweptAreaSolid")) {
                    ifcapi::detail::write_ref_attr(subelement, "SweptArea", profile);
                }
            }
        }
    }
}

void remove_material_usages_from_types(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products)
{
    for (auto product : products) {
        if (!is_a(product, "IfcTypeObject")) continue;
        auto material = material_for_product(product);
        if (!is_a(material, "IfcMaterialLayerSet") && !is_a(material, "IfcMaterialProfileSet")) continue;
        for (auto inverse : inverse_entities(file, material)) {
            if (is_ifc2x3(file)) {
                if (!is_a(inverse, "IfcMaterialLayerSetUsage")) continue;
                for (auto rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) {
                        remove_with_history(file, rel);
                    }
                }
            } else {
                if (!is_a(inverse, "IfcMaterialUsageDefinition")) continue;
                for (auto rel : inverse_entities(inverse, "AssociatedTo")) {
                    remove_with_history(file, rel);
                }
            }
            file->remove_entity(inverse);
        }
    }
}

void unassign_materials(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base user,
    express::Base application)
{
    std::vector<express::Base> associations;
    for (auto product : products) {
        for (auto rel : association_rels_for_product(file, product)) {
            if (!contains(associations, rel)) associations.push_back(rel);
        }
    }

    while (!associations.empty()) {
        auto rel = associations.back();
        if (!is_a(rel, "IfcRelAssociatesMaterial")) {
            associations.pop_back();
            continue;
        }
        auto material = ifcapi::detail::read_ref_attr(rel, "RelatingMaterial");
        auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        for (auto product : products) {
            remove_first(related_objects, product);
        }

        if (is_a(material, "IfcMaterialLayerSetUsage") || is_a(material, "IfcMaterialProfileSetUsage")) {
            auto material_inverses = inverse_entities(file, material);
            bool subset = true;
            for (auto inverse : material_inverses) {
                if (!contains(associations, inverse)) {
                    subset = false;
                    break;
                }
            }
            if (subset && related_objects.empty()) {
                file->remove_entity(material);
            }
        }
        associations.pop_back();

        if (related_objects.empty()) {
            remove_with_history(file, rel);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            ifcapi::detail::update_owner_history(file, rel, user, application);
        }
    }
}

void remove_properties(ifcopenshell::file* file, express::Base properties, bool ifc2x3_extended_only) {
    const char* attr = "Properties";
    if (is_ifc2x3(file)) {
        if (!ifc2x3_extended_only || is_a(properties, "IfcExtendedMaterialProperties")) {
            attr = "ExtendedProperties";
        } else {
            attr = nullptr;
        }
    }
    if (attr) {
        for (auto prop : ifcapi::detail::read_ref_aggregate(properties, attr)) {
            file->remove_entity(prop);
        }
    }
    file->remove_entity(properties);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base material_add_material(
    ifcopenshell::file* file,
    const MaterialAddMaterialOptions& options)
{
    auto material = create_entity(file, "IfcMaterial");
    const auto name = options.name.value_or("");
    ifcapi::detail::write_string_attr(material, "Name", !name.empty() ? name : "Unnamed");
    if (options.category && !options.category->empty()) {
        ifcapi::detail::write_string_attr(material, "Category", *options.category);
    }
    if (options.description && !options.description->empty()) {
        ifcapi::detail::write_string_attr(material, "Description", *options.description);
    }
    return material;
}

express::Base material_add_material_set(
    ifcopenshell::file* file,
    const MaterialAddMaterialSetOptions& options)
{
    const auto name = options.name.value_or("Unnamed");
    const auto set_type = options.set_type.value_or("IfcMaterialConstituentSet");
    auto set = create_entity(file, set_type.c_str());
    if (set_type == "IfcMaterialLayerSet") {
        ifcapi::detail::write_string_attr(set, "LayerSetName", name.empty() ? "Unnamed" : name);
    } else if (set_type != "IfcMaterialList") {
        ifcapi::detail::write_string_attr(set, "Name", name.empty() ? "Unnamed" : name);
    }
    return set;
}

express::Base material_add_constituent(
    ifcopenshell::file* file,
    express::Base* constituent_set,
    const MaterialAddConstituentOptions& options)
{
    auto constituent_set_value = detail::deref_or_empty(constituent_set);
    auto constituents = ifcapi::detail::read_ref_aggregate(constituent_set_value, "MaterialConstituents");
    auto constituent = create_entity(file, "IfcMaterialConstituent");
    ifcapi::detail::write_ref_attr(constituent, "Material", options.material);
    if (options.name) {
        write_optional_string(constituent, "Name", options.name->c_str());
    }
    constituents.push_back(constituent);
    ifcapi::detail::write_ref_aggregate(constituent_set_value, "MaterialConstituents", constituents);
    return constituent;
}

express::Base material_add_layer(
    ifcopenshell::file* file,
    express::Base* layer_set,
    const MaterialAddLayerOptions& options)
{
    double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    auto layer_set_value = detail::deref_or_empty(layer_set);
    auto layers = ifcapi::detail::read_ref_aggregate(layer_set_value, "MaterialLayers");
    auto layer = create_entity(file, "IfcMaterialLayer");
    ifcapi::detail::write_ref_attr(layer, "Material", options.material);
    int idx = ifcapi::detail::attr_index_of(layer, "LayerThickness");
    if (idx >= 0) layer.set_attribute_value(static_cast<size_t>(idx), 0.1 / unit_scale);
    if (!is_ifc2x3(file) && options.name) write_optional_string(layer, "Name", options.name->c_str());
    layers.push_back(layer);
    ifcapi::detail::write_ref_aggregate(layer_set_value, "MaterialLayers", layers);
    return layer;
}

express::Base material_add_profile(
    ifcopenshell::file* file,
    express::Base* profile_set,
    const MaterialAddProfileOptions& options)
{
    auto profile_set_value = detail::deref_or_empty(profile_set);
    auto profiles = ifcapi::detail::read_ref_aggregate(profile_set_value, "MaterialProfiles");
    auto mat_profile = create_entity(file, "IfcMaterialProfile");
    if (options.name) write_optional_string(mat_profile, "Name", options.name->c_str());
    if (options.material) ifcapi::detail::write_ref_attr(mat_profile, "Material", *options.material);
    if (options.profile) ifcapi::detail::write_ref_attr(mat_profile, "Profile", *options.profile);
    profiles.push_back(mat_profile);
    ifcapi::detail::write_ref_aggregate(profile_set_value, "MaterialProfiles", profiles);
    return mat_profile;
}

void material_add_list_item(
    ifcopenshell::file*,
    express::Base* material_list,
    express::Base* material)
{
    auto material_list_value = detail::deref_or_empty(material_list);
    auto material_value = detail::deref_or_empty(material);
    auto materials = ifcapi::detail::read_ref_aggregate(material_list_value, "Materials");
    materials.push_back(material_value);
    ifcapi::detail::write_ref_aggregate(material_list_value, "Materials", materials);
}

std::vector<express::Base> material_assign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& input_products,
    const MaterialAssignMaterialOptions& options)
{
    auto material_value = options.material.value_or(express::Base());
    auto owner_history_value = options.owner_history.value_or(express::Base());
    auto user_value = options.user.value_or(express::Base());
    auto application_value = options.application.value_or(express::Base());
    const auto type = options.type.value_or("IfcMaterial");
    auto products = dedupe_entities(mutable_entities(input_products));
    if (products.empty()) return {};
    std::vector<express::Base> to_unassign;
    for (auto product : products) {
        if (material_for_product(product)) to_unassign.push_back(product);
    }
    if (!to_unassign.empty()) {
        material_unassign_material(file, to_unassign, {user_value, application_value});
    }

    if (type == "IfcMaterial" || (material_value && !is_a(material_value, "IfcMaterial") && type.rfind("Usage") == std::string::npos)) {
        auto actual_material = material_value ? material_value : create_entity(file, "IfcMaterial");
        auto rel = get_rel_associates_material(file, actual_material, material_value);
        if (!rel) {
            return {create_material_association(file, actual_material, products, owner_history_value, user_value, application_value)};
        }
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        for (auto product : products) {
            if (!contains(related, product)) related.push_back(product);
        }
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
        return {rel};
    }

    if (type == "IfcMaterialConstituentSet" || type == "IfcMaterialLayerSet" || type == "IfcMaterialProfileSet") {
        auto material_set = create_entity(file, type.c_str());
        return {create_material_association(file, material_set, products, owner_history_value, user_value, application_value)};
    }

    if (type == "IfcMaterialLayerSetUsage") {
        if (material_value && !is_a(material_value, "IfcMaterialLayerSet")) {
            throw std::runtime_error(exact_class_name(material_value) + " cannot be assiged as a IfcMaterialLayerSetUsage.");
        }
        struct Group {
            express::Base set;
            std::string direction;
            std::vector<express::Base> products;
        };
        std::vector<Group> groups;
        for (auto product : products) {
            auto material_set = material_value ? material_value : find_material_set_for_type(file, product, "IfcMaterialLayerSet", "IfcMaterialLayerSet");
            std::string direction = is_axis3_class(product) ? "AXIS3" : "AXIS2";
            auto it = std::find_if(groups.begin(), groups.end(), [&](const Group& g) {
                return g.set == material_set && g.direction == direction;
            });
            if (it == groups.end()) {
                groups.push_back({material_set, direction, {product}});
            } else {
                it->products.push_back(product);
            }
        }
        std::vector<express::Base> rels;
        for (auto& group : groups) {
            auto usage = create_entity(file, "IfcMaterialLayerSetUsage");
            ifcapi::detail::write_ref_attr(usage, "ForLayerSet", group.set);
            ifcapi::detail::write_enum_attr(usage, "LayerSetDirection", group.direction);
            ifcapi::detail::write_enum_attr(usage, "DirectionSense", "POSITIVE");
            int offset_idx = ifcapi::detail::attr_index_of(usage, "OffsetFromReferenceLine");
            if (offset_idx >= 0) usage.set_attribute_value(static_cast<size_t>(offset_idx), 0.0);
            rels.push_back(create_material_association(file, usage, group.products, owner_history_value, user_value, application_value));
        }
        return rels;
    }

    if (type == "IfcMaterialProfileSetUsage") {
        if (material_value && !is_a(material_value, "IfcMaterialProfileSet")) {
            throw std::runtime_error(exact_class_name(material_value) + " cannot be assiged as a IfcMaterialProfileSetUsage.");
        }
        struct Group {
            express::Base set;
            std::vector<express::Base> products;
        };
        std::vector<Group> groups;
        for (auto product : products) {
            auto material_set = material_value ? material_value : find_material_set_for_type(file, product, "IfcMaterialProfileSet", "IfcMaterialProfileSet");
            auto it = std::find_if(groups.begin(), groups.end(), [&](const Group& g) { return g.set == material_set; });
            if (it == groups.end()) {
                groups.push_back({material_set, {product}});
            } else {
                it->products.push_back(product);
            }
        }
        std::vector<express::Base> rels;
        for (auto& group : groups) {
            update_representation_profile(file, group.set, group.products);
            auto usage = create_entity(file, "IfcMaterialProfileSetUsage");
            ifcapi::detail::write_ref_attr(usage, "ForProfileSet", group.set);
            rels.push_back(create_material_association(file, usage, group.products, owner_history_value, user_value, application_value));
        }
        return rels;
    }

    if (type == "IfcMaterialList") {
        auto material_set = create_entity(file, "IfcMaterialList");
        if (material_value) ifcapi::detail::write_ref_aggregate(material_set, "Materials", {material_value});
        return {create_material_association(file, material_set, products, owner_history_value, user_value, application_value)};
    }

    return {};
}

void material_unassign_material(
    ifcopenshell::file* file,
    const std::vector<express::Base>& input_products,
    const MaterialUnassignMaterialOptions& options)
{
    auto user_value = options.user.value_or(express::Base());
    auto application_value = options.application.value_or(express::Base());
    auto products = dedupe_entities(mutable_entities(input_products));
    if (products.empty()) return;
    remove_material_usages_from_types(file, products);
    unassign_materials(file, products, user_value, application_value);
}

void material_edit_profile_usage(
    ifcopenshell::file* file,
    express::Base* usage,
    const MaterialEditProfileUsageOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto usage_value = detail::deref_or_empty(usage);
        if (!file || !usage_value) {
            throw std::runtime_error("material_edit_profile_usage requires a file and usage");
        }
        bool found_cardinal_point = false;
        int old_cardinal_point = ifcapi::detail::read_int_attr(usage_value, "CardinalPoint");
        int cardinal_point = read_optional_int_from_props(options.attributes, "CardinalPoint", old_cardinal_point, found_cardinal_point);
        if (found_cardinal_point && cardinal_point && cardinal_point != old_cardinal_point &&
            options.profile_width && options.profile_height) {
            if (auto profile = usage_profile(usage_value)) {
                update_profile_usage_cardinal_point(
                    file,
                    usage_value,
                    profile,
                    cardinal_point,
                    *options.profile_width,
                    *options.profile_height);
            }
        }
        ifcapi::detail::apply_attribute_props(usage_value, options.attributes);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void material_assign_profile(
    ifcopenshell::file* file,
    express::Base* material_profile,
    express::Base* profile)
{
    auto material_profile_value = detail::deref_or_empty(material_profile);
    auto profile_value = detail::deref_or_empty(profile);
    auto old_profile = ifcapi::detail::read_ref_attr(material_profile_value, "Profile");
    ifcapi::detail::write_ref_attr(material_profile_value, "Profile", profile_value);
    for (auto profile_set : inverse_entities(material_profile_value, "ToMaterialProfileSet")) {
        for (auto inverse : inverse_entities(file, profile_set)) {
            if (!is_a(inverse, "IfcMaterialProfileSetUsage")) continue;
            std::vector<express::Base> rels;
            if (is_ifc2x3(file)) {
                for (auto rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) rels.push_back(rel);
                }
            } else {
                rels = inverse_entities(inverse, "AssociatedTo");
            }
            for (auto rel : rels) {
                for (auto element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                    auto representation = ifcapi::bindings::representation_get_product_representation(
                        &element,
                        {{}, "Model", "Body", "MODEL_VIEW"});
                    if (!representation) continue;
                    auto traversed = file->traverse(representation, -1);
                    if (!traversed.empty()) {
                        for (auto subelement : traversed) {
                            if (is_a(subelement, "IfcSweptAreaSolid")) {
                                ifcapi::detail::write_ref_attr(subelement, "SweptArea", profile_value);
                            }
                        }
                    }
                }
            }
        }
    }
    if (old_profile && inverse_entities(file, old_profile).empty()) {
        file->remove_entity(old_profile);
    }
}

void material_remove_constituent(
    ifcopenshell::file* file,
    express::Base* constituent,
    const MaterialRemoveItemOptions& options)
{
    auto constituent_value = detail::deref_or_empty(constituent);
    auto material = ifcapi::detail::read_ref_attr(constituent_value, "Material");
    file->remove_entity(constituent_value);
    if (material && options.should_remove_material.value_or(false)) ifcapi::bindings::entity_remove_deep2(&material);
}

void material_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer,
    const MaterialRemoveItemOptions& options)
{
    auto layer_value = detail::deref_or_empty(layer);
    auto material = ifcapi::detail::read_ref_attr(layer_value, "Material");
    file->remove_entity(layer_value);
    if (material && options.should_remove_material.value_or(false)) ifcapi::bindings::entity_remove_deep2(&material);
}

void material_remove_profile(
    ifcopenshell::file* file,
    express::Base* profile,
    const MaterialRemoveProfileOptions& options)
{
    auto profile_value = detail::deref_or_empty(profile);
    auto material = ifcapi::detail::read_ref_attr(profile_value, "Material");
    auto profile_def = ifcapi::detail::read_ref_attr(profile_value, "Profile");
    file->remove_entity(profile_value);
    if (material && options.should_remove_material.value_or(false)) ifcapi::bindings::entity_remove_deep2(&material);
    if (profile_def && options.should_remove_profile_def.value_or(false)) ifcapi::bindings::entity_remove_deep2(&profile_def);
}

void material_remove_list_item(
    ifcopenshell::file*,
    express::Base* material_list,
    const MaterialRemoveListItemOptions& options)
{
    auto material_list_value = detail::deref_or_empty(material_list);
    auto materials = ifcapi::detail::read_ref_aggregate(material_list_value, "Materials");
    const auto material_index = options.material_index.value_or(0);
    materials.erase(materials.begin() + material_index);
    ifcapi::detail::write_ref_aggregate(material_list_value, "Materials", materials);
}

void material_reorder_set_item(
    ifcopenshell::file*,
    express::Base* material_set,
    const MaterialReorderSetItemOptions& options)
{
    auto material_set_value = detail::deref_or_empty(material_set);
    const auto old_index = options.old_index.value_or(0);
    const auto new_index = options.new_index.value_or(0);
    const char* set_name = nullptr;
    if (is_a(material_set_value, "IfcMaterialConstituentSet")) {
        set_name = "MaterialConstituents";
    } else if (is_a(material_set_value, "IfcMaterialLayerSet")) {
        set_name = "MaterialLayers";
    } else if (is_a(material_set_value, "IfcMaterialProfileSet")) {
        set_name = "MaterialProfiles";
    } else if (is_a(material_set_value, "IfcMaterialList")) {
        set_name = "Materials";
    } else {
        throw std::runtime_error("Unexpected material set type: '" + exact_class_name(material_set_value) + "'.");
    }
    auto items = ifcapi::detail::read_ref_aggregate(material_set_value, set_name);
    auto item = items.at(static_cast<size_t>(old_index));
    items.erase(items.begin() + old_index);
    items.insert(items.begin() + new_index, item);
    ifcapi::detail::write_ref_aggregate(material_set_value, set_name, items);
}

void material_remove_material(ifcopenshell::file* file, express::Base* material) {
    auto material_value = detail::deref_or_empty(material);
    auto inverse_elements = inverse_entities(file, material_value);
    file->remove_entity(material_value);
    for (auto inverse : inverse_elements) {
        if (is_a(inverse, "IfcMaterialConstituent") ||
            is_a(inverse, "IfcMaterialLayer") ||
            is_a(inverse, "IfcMaterialProfile")) {
            file->remove_entity(inverse);
        } else if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcMaterialProperties")) {
            remove_properties(file, inverse, true);
        } else if (is_a(inverse, "IfcMaterialDefinitionRepresentation")) {
            for (auto representation : ifcapi::detail::read_ref_aggregate(inverse, "Representations")) {
                for (auto item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                    file->remove_entity(item);
                }
                file->remove_entity(representation);
            }
            file->remove_entity(inverse);
        }
    }
}

void material_remove_material_set(ifcopenshell::file* file, express::Base* material) {
    auto material_value = detail::deref_or_empty(material);
    bool has_usages = is_a(material_value, "IfcMaterialLayerSet") || is_a(material_value, "IfcMaterialProfileSet");
    if (has_usages) {
        for (auto inverse : inverse_entities(file, material_value)) {
            if (!is_a(inverse, "IfcMaterialLayerSetUsage") && !is_a(inverse, "IfcMaterialProfileSetUsage")) continue;
            if (is_ifc2x3(file)) {
                for (auto rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) remove_with_history(file, rel);
                }
            } else {
                for (auto rel : inverse_entities(inverse, "AssociatedTo")) {
                    remove_with_history(file, rel);
                }
            }
            file->remove_entity(inverse);
        }
    }

    std::vector<express::Base> set_items;
    if (is_a(material_value, "IfcMaterialLayerSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material_value, "MaterialLayers");
    } else if (is_a(material_value, "IfcMaterialProfileSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material_value, "MaterialProfiles");
    } else if (is_a(material_value, "IfcMaterialConstituentSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material_value, "MaterialConstituents");
    } else if (!is_a(material_value, "IfcMaterialList")) {
        throw std::runtime_error("Unknown material set type: " + exact_class_name(material_value));
    }
    for (auto set_item : set_items) {
        file->remove_entity(set_item);
    }

    auto inverse_elements = inverse_entities(file, material_value);
    file->remove_entity(material_value);
    for (auto inverse : inverse_elements) {
        if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcMaterialProperties")) {
            remove_properties(file, inverse, false);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
