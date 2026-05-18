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

#include "ifcparse/IfcFile.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity);
std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const char* attribute);

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

IfcUtil::IfcBaseClass* usage_profile(IfcUtil::IfcBaseClass* usage) {
    auto* material_set = ifcapi::detail::read_ref_attr(usage, "ForProfileSet");
    auto* profile = ifcapi::detail::read_ref_attr(material_set, "CompositeProfile");
    if (profile) {
        return profile;
    }
    auto profiles = ifcapi::detail::read_ref_aggregate(material_set, "MaterialProfiles");
    if (!profiles.empty()) {
        return ifcapi::detail::read_ref_attr(profiles.front(), "Profile");
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* cardinal_point_position(IfcParse::IfcFile* file, int cardinal_point, double width, double height) {
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
    else return nullptr;
    auto* placement = file->create(file->schema()->declaration_by_name("IfcAxis2Placement3D"));
    ifcapi::detail::write_ref_attr(placement, "Location", ifcapi::detail::create_cartesian_point(file, location));
    return placement;
}

void update_profile_usage_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* profile,
    IfcUtil::IfcBaseClass* position)
{
    auto* representation = ifcapi::bindings::representation_get_product_representation(
        element,
        nullptr,
        "Model",
        "Body",
        "MODEL_VIEW");
    if (!representation) {
        return;
    }
    auto traversed = file->traverse(representation, -1);
    if (!traversed) {
        return;
    }
    for (auto* subelement : *traversed) {
        if (is_a(subelement, "IfcSweptAreaSolid") && ifcapi::detail::read_ref_attr(subelement, "SweptArea") == profile) {
            ifcapi::detail::write_ref_attr(subelement, "Position", position);
        }
    }
}

void update_profile_usage_cardinal_point(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* usage,
    IfcUtil::IfcBaseClass* profile,
    int cardinal_point,
    double profile_width,
    double profile_height)
{
    auto* position = cardinal_point_position(file, cardinal_point, profile_width, profile_height);
    if (is_ifc2x3(file)) {
        for (auto* rel : inverse_entities(file, usage)) {
            if (!is_a(rel, "IfcRelAssociatesMaterial")) continue;
            for (auto* element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                update_profile_usage_representation(file, element, profile, position);
            }
        }
    } else {
        for (auto* rel : inverse_entities(usage, "AssociatedTo")) {
            for (auto* element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                update_profile_usage_representation(file, element, profile, position);
            }
        }
    }
}

std::string exact_class_name(IfcUtil::IfcBaseClass* entity) {
    return entity ? entity->declaration().name() : std::string();
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

std::vector<IfcUtil::IfcBaseClass*> mutable_entities(
    const std::vector<const IfcUtil::IfcBaseClass*>& entities)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(entities.size());
    for (auto* entity : entities) {
        auto* mutable_entity = const_cast<IfcUtil::IfcBaseClass*>(entity);
        if (mutable_entity) result.push_back(mutable_entity);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> dedupe_entities(std::vector<IfcUtil::IfcBaseClass*> entities) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;
    result.reserve(entities.size());
    for (auto* entity : entities) {
        if (entity && seen.insert(entity).second) {
            result.push_back(entity);
        }
    }
    return result;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

void remove_first(std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    auto it = std::find(values.begin(), values.end(), value);
    if (it != values.end()) {
        values.erase(it);
    }
}

void write_optional_string(IfcUtil::IfcBaseClass* entity, const char* attr, const char* value) {
    if (value) {
        ifcapi::detail::write_string_attr(entity, attr, value);
    }
}

IfcUtil::IfcBaseClass* create_material_association(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_material,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = create_entity(file, "IfcRelAssociatesMaterial");
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", products);
    ifcapi::detail::write_ref_attr(rel, "RelatingMaterial", relating_material);
    return rel;
}

IfcUtil::IfcBaseClass* get_rel_associates_material(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* original_material)
{
    auto* target = original_material ? original_material : material;
    if (is_ifc2x3(file) || is_a(material, "IfcMaterialList")) {
        auto rels = file->instances_by_type("IfcRelAssociatesMaterial");
        if (rels) {
            for (auto* rel : *rels) {
                if (ifcapi::detail::read_ref_attr(rel, "RelatingMaterial") == target) {
                    return rel;
                }
            }
        }
        return nullptr;
    }
    auto associated = inverse_entities(material, "AssociatedTo");
    return associated.empty() ? nullptr : associated.front();
}

IfcUtil::IfcBaseClass* material_for_product(IfcUtil::IfcBaseClass* product) {
    return ifcapi::bindings::element_get_material(product, false, true);
}

IfcUtil::IfcBaseClass* type_for_product(IfcUtil::IfcBaseClass* product) {
    return ifcapi::bindings::element_get_type(product);
}

std::vector<IfcUtil::IfcBaseClass*> association_rels_for_product(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* product) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (auto* inverse : inverse_entities(file, product)) {
        if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            result.push_back(inverse);
        }
    }
    return result;
}

void remove_with_history(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* rel) {
    ifcapi::detail::remove_with_history(file, rel);
}

bool is_axis3_class(IfcUtil::IfcBaseClass* product) {
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

IfcUtil::IfcBaseClass* find_material_set_for_type(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    const char* material_set_class,
    const char* fallback_class)
{
    auto* element_type = type_for_product(product);
    auto* type_material = element_type ? material_for_product(element_type) : nullptr;
    if (is_a(type_material, material_set_class)) {
        return type_material;
    }
    return file ? create_entity(file, fallback_class) : nullptr;
}

void update_representation_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_set,
    const std::vector<IfcUtil::IfcBaseClass*>& products)
{
    auto* profile = ifcapi::detail::read_ref_attr(material_set, "CompositeProfile");
    if (!profile) {
        auto profiles = ifcapi::detail::read_ref_aggregate(material_set, "MaterialProfiles");
        if (!profiles.empty()) {
            profile = ifcapi::detail::read_ref_attr(profiles.front(), "Profile");
        }
    }
    if (!profile) return;
    for (auto* product : products) {
        auto* representation = ifcapi::bindings::representation_get_product_representation(
            product,
            nullptr,
            "Model",
            "Body",
            "MODEL_VIEW");
        if (!representation) return;
        auto traversed = file->traverse(representation, -1);
        if (traversed) {
            for (auto* subelement : *traversed) {
                if (is_a(subelement, "IfcSweptAreaSolid")) {
                    ifcapi::detail::write_ref_attr(subelement, "SweptArea", profile);
                }
            }
        }
    }
}

void remove_material_usages_from_types(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products)
{
    for (auto* product : products) {
        if (!is_a(product, "IfcTypeObject")) continue;
        auto* material = material_for_product(product);
        if (!is_a(material, "IfcMaterialLayerSet") && !is_a(material, "IfcMaterialProfileSet")) continue;
        for (auto* inverse : inverse_entities(file, material)) {
            if (is_ifc2x3(file)) {
                if (!is_a(inverse, "IfcMaterialLayerSetUsage")) continue;
                for (auto* rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) {
                        remove_with_history(file, rel);
                    }
                }
            } else {
                if (!is_a(inverse, "IfcMaterialUsageDefinition")) continue;
                for (auto* rel : inverse_entities(inverse, "AssociatedTo")) {
                    remove_with_history(file, rel);
                }
            }
            file->removeEntity(inverse);
        }
    }
}

void unassign_materials(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    std::vector<IfcUtil::IfcBaseClass*> associations;
    for (auto* product : products) {
        for (auto* rel : association_rels_for_product(file, product)) {
            if (!contains(associations, rel)) associations.push_back(rel);
        }
    }

    while (!associations.empty()) {
        auto* rel = associations.back();
        if (!is_a(rel, "IfcRelAssociatesMaterial")) {
            associations.pop_back();
            continue;
        }
        auto* material = ifcapi::detail::read_ref_attr(rel, "RelatingMaterial");
        auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        for (auto* product : products) {
            remove_first(related_objects, product);
        }

        if (is_a(material, "IfcMaterialLayerSetUsage") || is_a(material, "IfcMaterialProfileSetUsage")) {
            auto material_inverses = inverse_entities(file, material);
            bool subset = true;
            for (auto* inverse : material_inverses) {
                if (!contains(associations, inverse)) {
                    subset = false;
                    break;
                }
            }
            if (subset && related_objects.empty()) {
                file->removeEntity(material);
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

void remove_properties(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* properties, bool ifc2x3_extended_only) {
    const char* attr = "Properties";
    if (is_ifc2x3(file)) {
        if (ifc2x3_extended_only && !is_a(properties, "IfcExtendedMaterialProperties")) return;
        attr = "ExtendedProperties";
    }
    for (auto* prop : ifcapi::detail::read_ref_aggregate(properties, attr)) {
        file->removeEntity(prop);
    }
    file->removeEntity(properties);
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* material_add_material(
    IfcParse::IfcFile* file,
    const char* name,
    const char* category,
    const char* description)
{
    auto* material = create_entity(file, "IfcMaterial");
    ifcapi::detail::write_string_attr(material, "Name", (name && *name) ? name : "Unnamed");
    if (category && *category) ifcapi::detail::write_string_attr(material, "Category", category);
    if (description && *description) ifcapi::detail::write_string_attr(material, "Description", description);
    return material;
}

IfcUtil::IfcBaseClass* material_add_material_set(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& set_type)
{
    auto* set = create_entity(file, set_type.c_str());
    if (set_type == "IfcMaterialLayerSet") {
        ifcapi::detail::write_string_attr(set, "LayerSetName", name.empty() ? "Unnamed" : name);
    } else if (set_type != "IfcMaterialList") {
        ifcapi::detail::write_string_attr(set, "Name", name.empty() ? "Unnamed" : name);
    }
    return set;
}

IfcUtil::IfcBaseClass* material_add_constituent(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constituent_set,
    IfcUtil::IfcBaseClass* material,
    const char* name)
{
    auto constituents = ifcapi::detail::read_ref_aggregate(constituent_set, "MaterialConstituents");
    auto* constituent = create_entity(file, "IfcMaterialConstituent");
    ifcapi::detail::write_ref_attr(constituent, "Material", material);
    write_optional_string(constituent, "Name", name);
    constituents.push_back(constituent);
    ifcapi::detail::write_ref_aggregate(constituent_set, "MaterialConstituents", constituents);
    return constituent;
}

IfcUtil::IfcBaseClass* material_add_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer_set,
    IfcUtil::IfcBaseClass* material,
    const char* name)
{
    double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    auto layers = ifcapi::detail::read_ref_aggregate(layer_set, "MaterialLayers");
    auto* layer = create_entity(file, "IfcMaterialLayer");
    ifcapi::detail::write_ref_attr(layer, "Material", material);
    int idx = ifcapi::detail::attr_index_of(layer, "LayerThickness");
    if (idx >= 0) layer->set_attribute_value(static_cast<size_t>(idx), 0.1 / unit_scale);
    if (!is_ifc2x3(file)) write_optional_string(layer, "Name", name);
    layers.push_back(layer);
    ifcapi::detail::write_ref_aggregate(layer_set, "MaterialLayers", layers);
    return layer;
}

IfcUtil::IfcBaseClass* material_add_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile_set,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* profile,
    const char* name)
{
    auto profiles = ifcapi::detail::read_ref_aggregate(profile_set, "MaterialProfiles");
    auto* mat_profile = create_entity(file, "IfcMaterialProfile");
    write_optional_string(mat_profile, "Name", name);
    if (material) ifcapi::detail::write_ref_attr(mat_profile, "Material", material);
    if (profile) ifcapi::detail::write_ref_attr(mat_profile, "Profile", profile);
    profiles.push_back(mat_profile);
    ifcapi::detail::write_ref_aggregate(profile_set, "MaterialProfiles", profiles);
    return mat_profile;
}

void material_add_list_item(
    IfcParse::IfcFile*,
    IfcUtil::IfcBaseClass* material_list,
    IfcUtil::IfcBaseClass* material)
{
    auto materials = ifcapi::detail::read_ref_aggregate(material_list, "Materials");
    materials.push_back(material);
    ifcapi::detail::write_ref_aggregate(material_list, "Materials", materials);
}

std::vector<IfcUtil::IfcBaseClass*> material_assign_material(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& input_products,
    const std::string& type,
    IfcUtil::IfcBaseClass* material,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto products = dedupe_entities(mutable_entities(input_products));
    if (products.empty()) return {};
    std::vector<const IfcUtil::IfcBaseClass*> to_unassign;
    for (auto* product : products) {
        if (material_for_product(product)) to_unassign.push_back(product);
    }
    if (!to_unassign.empty()) {
        material_unassign_material(file, to_unassign, user, application);
    }

    if (type == "IfcMaterial" || (material && !is_a(material, "IfcMaterial") && type.rfind("Usage") == std::string::npos)) {
        auto* actual_material = material ? material : create_entity(file, "IfcMaterial");
        auto* rel = get_rel_associates_material(file, actual_material, material);
        if (!rel) {
            return {create_material_association(file, actual_material, products, owner_history, user, application)};
        }
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        for (auto* product : products) {
            if (!contains(related, product)) related.push_back(product);
        }
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
        return {rel};
    }

    if (type == "IfcMaterialConstituentSet" || type == "IfcMaterialLayerSet" || type == "IfcMaterialProfileSet") {
        auto* material_set = create_entity(file, type.c_str());
        return {create_material_association(file, material_set, products, owner_history, user, application)};
    }

    if (type == "IfcMaterialLayerSetUsage") {
        if (material && !is_a(material, "IfcMaterialLayerSet")) {
            throw std::runtime_error(exact_class_name(material) + " cannot be assiged as a IfcMaterialLayerSetUsage.");
        }
        struct Group {
            IfcUtil::IfcBaseClass* set;
            std::string direction;
            std::vector<IfcUtil::IfcBaseClass*> products;
        };
        std::vector<Group> groups;
        for (auto* product : products) {
            auto* material_set = material ? material : find_material_set_for_type(file, product, "IfcMaterialLayerSet", "IfcMaterialLayerSet");
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
        std::vector<IfcUtil::IfcBaseClass*> rels;
        for (auto& group : groups) {
            auto* usage = create_entity(file, "IfcMaterialLayerSetUsage");
            ifcapi::detail::write_ref_attr(usage, "ForLayerSet", group.set);
            ifcapi::detail::write_enum_attr(usage, "LayerSetDirection", group.direction);
            ifcapi::detail::write_enum_attr(usage, "DirectionSense", "POSITIVE");
            int offset_idx = ifcapi::detail::attr_index_of(usage, "OffsetFromReferenceLine");
            if (offset_idx >= 0) usage->set_attribute_value(static_cast<size_t>(offset_idx), 0.0);
            rels.push_back(create_material_association(file, usage, group.products, owner_history, user, application));
        }
        return rels;
    }

    if (type == "IfcMaterialProfileSetUsage") {
        if (material && !is_a(material, "IfcMaterialProfileSet")) {
            throw std::runtime_error(exact_class_name(material) + " cannot be assiged as a IfcMaterialProfileSetUsage.");
        }
        struct Group {
            IfcUtil::IfcBaseClass* set;
            std::vector<IfcUtil::IfcBaseClass*> products;
        };
        std::vector<Group> groups;
        for (auto* product : products) {
            auto* material_set = material ? material : find_material_set_for_type(file, product, "IfcMaterialProfileSet", "IfcMaterialProfileSet");
            auto it = std::find_if(groups.begin(), groups.end(), [&](const Group& g) { return g.set == material_set; });
            if (it == groups.end()) {
                groups.push_back({material_set, {product}});
            } else {
                it->products.push_back(product);
            }
        }
        std::vector<IfcUtil::IfcBaseClass*> rels;
        for (auto& group : groups) {
            update_representation_profile(file, group.set, group.products);
            auto* usage = create_entity(file, "IfcMaterialProfileSetUsage");
            ifcapi::detail::write_ref_attr(usage, "ForProfileSet", group.set);
            rels.push_back(create_material_association(file, usage, group.products, owner_history, user, application));
        }
        return rels;
    }

    if (type == "IfcMaterialList") {
        auto* material_set = create_entity(file, "IfcMaterialList");
        if (material) ifcapi::detail::write_ref_aggregate(material_set, "Materials", {material});
        return {create_material_association(file, material_set, products, owner_history, user, application)};
    }

    return {};
}

void material_unassign_material(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& input_products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto products = dedupe_entities(mutable_entities(input_products));
    if (products.empty()) return;
    remove_material_usages_from_types(file, products);
    unassign_materials(file, products, user, application);
}

void material_edit_profile_usage(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* usage,
    ifcopenshell_pset_props_t* attributes,
    bool has_profile_dimensions,
    double profile_width,
    double profile_height)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !usage) {
            throw std::runtime_error("material_edit_profile_usage requires a file and usage");
        }
        bool found_cardinal_point = false;
        int old_cardinal_point = ifcapi::detail::read_int_attr(usage, "CardinalPoint");
        int cardinal_point = read_optional_int_from_props(attributes, "CardinalPoint", old_cardinal_point, found_cardinal_point);
        if (found_cardinal_point && cardinal_point && cardinal_point != old_cardinal_point && has_profile_dimensions) {
            if (auto* profile = usage_profile(usage)) {
                update_profile_usage_cardinal_point(file, usage, profile, cardinal_point, profile_width, profile_height);
            }
        }
        ifcapi::detail::apply_attribute_props(usage, attributes);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void material_assign_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* material_profile,
    IfcUtil::IfcBaseClass* profile)
{
    auto* old_profile = ifcapi::detail::read_ref_attr(material_profile, "Profile");
    ifcapi::detail::write_ref_attr(material_profile, "Profile", profile);
    for (auto* profile_set : inverse_entities(material_profile, "ToMaterialProfileSet")) {
        for (auto* inverse : inverse_entities(file, profile_set)) {
            if (!is_a(inverse, "IfcMaterialProfileSetUsage")) continue;
            std::vector<IfcUtil::IfcBaseClass*> rels;
            if (is_ifc2x3(file)) {
                for (auto* rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) rels.push_back(rel);
                }
            } else {
                rels = inverse_entities(inverse, "AssociatedTo");
            }
            for (auto* rel : rels) {
                for (auto* element : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                    auto* representation = ifcapi::bindings::representation_get_product_representation(
                        element,
                        nullptr,
                        "Model",
                        "Body",
                        "MODEL_VIEW");
                    if (!representation) continue;
                    auto traversed = file->traverse(representation, -1);
                    if (traversed) {
                        for (auto* subelement : *traversed) {
                            if (is_a(subelement, "IfcSweptAreaSolid")) {
                                ifcapi::detail::write_ref_attr(subelement, "SweptArea", profile);
                            }
                        }
                    }
                }
            }
        }
    }
    if (old_profile && file->getTotalInverses(old_profile->id()) == 0) {
        file->removeEntity(old_profile);
    }
}

void material_remove_constituent(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constituent,
    bool should_remove_material)
{
    auto* material = ifcapi::detail::read_ref_attr(constituent, "Material");
    file->removeEntity(constituent);
    if (material && should_remove_material) ifcapi::bindings::entity_remove_deep2(material);
}

void material_remove_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer,
    bool should_remove_material)
{
    auto* material = ifcapi::detail::read_ref_attr(layer, "Material");
    file->removeEntity(layer);
    if (material && should_remove_material) ifcapi::bindings::entity_remove_deep2(material);
}

void material_remove_profile(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile,
    bool should_remove_profile_def,
    bool should_remove_material)
{
    auto* material = ifcapi::detail::read_ref_attr(profile, "Material");
    auto* profile_def = ifcapi::detail::read_ref_attr(profile, "Profile");
    file->removeEntity(profile);
    if (material && should_remove_material) ifcapi::bindings::entity_remove_deep2(material);
    if (profile_def && should_remove_profile_def) ifcapi::bindings::entity_remove_deep2(profile_def);
}

void material_remove_list_item(
    IfcParse::IfcFile*,
    IfcUtil::IfcBaseClass* material_list,
    int material_index)
{
    auto materials = ifcapi::detail::read_ref_aggregate(material_list, "Materials");
    materials.erase(materials.begin() + material_index);
    ifcapi::detail::write_ref_aggregate(material_list, "Materials", materials);
}

void material_reorder_set_item(
    IfcParse::IfcFile*,
    IfcUtil::IfcBaseClass* material_set,
    int old_index,
    int new_index)
{
    const char* set_name = nullptr;
    if (is_a(material_set, "IfcMaterialConstituentSet")) {
        set_name = "MaterialConstituents";
    } else if (is_a(material_set, "IfcMaterialLayerSet")) {
        set_name = "MaterialLayers";
    } else if (is_a(material_set, "IfcMaterialProfileSet")) {
        set_name = "MaterialProfiles";
    } else if (is_a(material_set, "IfcMaterialList")) {
        set_name = "Materials";
    } else {
        throw std::runtime_error("Unexpected material set type: '" + exact_class_name(material_set) + "'.");
    }
    auto items = ifcapi::detail::read_ref_aggregate(material_set, set_name);
    auto* item = items.at(static_cast<size_t>(old_index));
    items.erase(items.begin() + old_index);
    items.insert(items.begin() + new_index, item);
    ifcapi::detail::write_ref_aggregate(material_set, set_name, items);
}

void material_remove_material(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* material) {
    auto inverse_elements = inverse_entities(file, material);
    file->removeEntity(material);
    for (auto* inverse : inverse_elements) {
        if (is_a(inverse, "IfcMaterialConstituent") ||
            is_a(inverse, "IfcMaterialLayer") ||
            is_a(inverse, "IfcMaterialProfile")) {
            file->removeEntity(inverse);
        } else if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcMaterialProperties")) {
            remove_properties(file, inverse, true);
        } else if (is_a(inverse, "IfcMaterialDefinitionRepresentation")) {
            for (auto* representation : ifcapi::detail::read_ref_aggregate(inverse, "Representations")) {
                for (auto* item : ifcapi::detail::read_ref_aggregate(representation, "Items")) {
                    file->removeEntity(item);
                }
                file->removeEntity(representation);
            }
            file->removeEntity(inverse);
        }
    }
}

void material_remove_material_set(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* material) {
    bool has_usages = is_a(material, "IfcMaterialLayerSet") || is_a(material, "IfcMaterialProfileSet");
    if (has_usages) {
        for (auto* inverse : inverse_entities(file, material)) {
            if (!is_a(inverse, "IfcMaterialLayerSetUsage") && !is_a(inverse, "IfcMaterialProfileSetUsage")) continue;
            if (is_ifc2x3(file)) {
                for (auto* rel : inverse_entities(file, inverse)) {
                    if (is_a(rel, "IfcRelAssociatesMaterial")) remove_with_history(file, rel);
                }
            } else {
                for (auto* rel : inverse_entities(inverse, "AssociatedTo")) {
                    remove_with_history(file, rel);
                }
            }
            file->removeEntity(inverse);
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> set_items;
    if (is_a(material, "IfcMaterialLayerSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material, "MaterialLayers");
    } else if (is_a(material, "IfcMaterialProfileSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material, "MaterialProfiles");
    } else if (is_a(material, "IfcMaterialConstituentSet")) {
        set_items = ifcapi::detail::read_ref_aggregate(material, "MaterialConstituents");
    } else if (!is_a(material, "IfcMaterialList")) {
        throw std::runtime_error("Unknown material set type: " + exact_class_name(material));
    }
    for (auto* set_item : set_items) {
        file->removeEntity(set_item);
    }

    auto inverse_elements = inverse_entities(file, material);
    file->removeEntity(material);
    for (auto* inverse : inverse_elements) {
        if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            remove_with_history(file, inverse);
        } else if (is_a(inverse, "IfcMaterialProperties")) {
            remove_properties(file, inverse, false);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
