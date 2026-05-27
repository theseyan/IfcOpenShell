// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/exception.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

express::Base first_inverse_of_type(express::Base entity, const char* inverse_name, const char* ifc_class) {
    for (auto inverse : ifcapi::detail::read_inverse_aggregate(entity, inverse_name)) {
        if (inverse && inverse.declaration().is(ifc_class)) {
            return inverse;
        }
    }
    return {};
}

express::Base find_is_decomposed_by(express::Base entity) {
    return first_inverse_of_type(entity, "IsDecomposedBy", "IfcRelAggregates");
}

express::Base find_decomposes(express::Base entity) {
    return first_inverse_of_type(entity, "Decomposes", "IfcRelAggregates");
}

express::Base find_contained_in_structure(express::Base entity) {
    auto result = ifcapi::detail::read_inverse_aggregate(entity, "ContainedInStructure");
    return result.empty() ? express::Base() : result.front();
}

express::Base find_contains_elements(express::Base entity) {
    auto result = ifcapi::detail::read_inverse_aggregate(entity, "ContainsElements");
    return result.empty() ? express::Base() : result.front();
}

std::set<express::Base> to_ref_set_filtered(const std::vector<express::Base>& products) {
    std::set<express::Base> result;
    for (const auto& product : products) {
        if (product) {
            result.insert(product);
        }
    }
    return result;
}

std::vector<express::Base> without_set(
    const std::vector<express::Base>& values,
    const std::set<express::Base>& removed)
{
    std::vector<express::Base> result;
    for (const auto& value : values) {
        if (removed.find(value) == removed.end()) {
            result.push_back(value);
        }
    }
    return result;
}

express::Base create_relationship(
    ifcopenshell::file* file,
    const char* ifc_class,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* rel_decl = file->schema()->declaration_by_name(ifc_class);
    auto rel = file->create(rel_decl);
    if (!rel) {
        return {};
    }
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    auto history = ifcapi::detail::ensure_owner_history(file, owner_history, user, application);
    ifcapi::detail::write_ref_attr(rel, "OwnerHistory", history);
    return rel;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base aggregate_assign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* relating_object,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    if (!file || products.empty()) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto relating = relating_object ? *relating_object : express::Base();
        if (!relating) {
            set_error("Relating object not found");
            return {};
        }

        auto products_set = to_ref_set_filtered(products);
        if (products_set.empty()) {
            return {};
        }

        auto existing_rel = find_is_decomposed_by(relating);
        std::set<express::Base> previous_rels;
        std::vector<express::Base> products_without_aggregates;
        std::vector<express::Base> products_to_change;

        for (const auto& product : products_set) {
            auto cur_rel = find_decomposes(product);
            if (!cur_rel) {
                products_without_aggregates.push_back(product);
                products_to_change.push_back(product);
            } else if (cur_rel != existing_rel) {
                previous_rels.insert(cur_rel);
                products_to_change.push_back(product);
            }
        }

        if (products_to_change.empty()) {
            return existing_rel;
        }

        std::set<express::Base> products_without_aggregates_set(
            products_without_aggregates.begin(), products_without_aggregates.end());
        for (const auto& product : products_without_aggregates) {
            auto container_rel = find_contained_in_structure(product);
            if (!container_rel) {
                continue;
            }
            auto remaining = without_set(
                ifcapi::detail::read_ref_aggregate(container_rel, "RelatedElements"),
                products_without_aggregates_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, container_rel);
            } else {
                ifcapi::detail::write_ref_aggregate(container_rel, "RelatedElements", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    container_rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }

        for (auto prev_rel : previous_rels) {
            auto remaining = without_set(ifcapi::detail::read_ref_aggregate(prev_rel, "RelatedObjects"), products_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, prev_rel);
            } else {
                ifcapi::detail::write_ref_aggregate(prev_rel, "RelatedObjects", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    prev_rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }

        if (existing_rel) {
            auto current = ifcapi::detail::read_ref_aggregate(existing_rel, "RelatedObjects");
            auto current_set = ifcapi::detail::to_ref_set(current);
            current_set.insert(products_set.begin(), products_set.end());
            auto merged = ifcapi::detail::to_ref_vector(current_set);
            ifcapi::detail::write_ref_aggregate(existing_rel, "RelatedObjects", merged);
            ifcapi::detail::update_owner_history(
                file,
                existing_rel,
                user ? *user : express::Base(),
                application ? *application : express::Base());
            return existing_rel;
        }

        auto rel = create_relationship(
            file,
            "IfcRelAggregates",
            owner_history ? *owner_history : express::Base(),
            user ? *user : express::Base(),
            application ? *application : express::Base());
        if (!rel) {
            set_error("Failed to create IfcRelAggregates");
            return {};
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingObject", relating);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", ifcapi::detail::to_ref_vector(products_set));
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void aggregate_unassign_object(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* user,
    express::Base* application)
{
    if (!file || products.empty()) {
        return;
    }

    try {
        auto products_set = to_ref_set_filtered(products);
        std::set<express::Base> rels;
        for (const auto& product : products_set) {
            auto rel = find_decomposes(product);
            if (rel) {
                rels.insert(rel);
            }
        }

        for (auto rel : rels) {
            auto remaining = without_set(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), products_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }
    } catch (...) {
    }
}

express::Base spatial_assign_container(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* relating_structure,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    if (!file || products.empty()) {
        set_error("Invalid arguments");
        return {};
    }

    try {
        auto structure = relating_structure ? *relating_structure : express::Base();
        if (!structure) {
            set_error("Relating structure not found");
            return {};
        }

        auto products_set = to_ref_set_filtered(products);
        if (products_set.empty()) {
            return {};
        }

        auto existing_rel = find_contains_elements(structure);
        std::set<express::Base> previous_rels;
        std::vector<express::Base> products_without_containers;
        std::vector<express::Base> products_to_change;

        for (const auto& product : products_set) {
            auto cur_rel = find_contained_in_structure(product);
            if (!cur_rel) {
                products_without_containers.push_back(product);
                products_to_change.push_back(product);
            } else if (cur_rel != existing_rel) {
                previous_rels.insert(cur_rel);
                products_to_change.push_back(product);
            }
        }

        if (products_to_change.empty()) {
            return existing_rel;
        }

        std::set<express::Base> products_without_containers_set(
            products_without_containers.begin(), products_without_containers.end());
        for (const auto& product : products_without_containers) {
            auto agg_rel = find_decomposes(product);
            if (!agg_rel) {
                continue;
            }
            auto remaining = without_set(
                ifcapi::detail::read_ref_aggregate(agg_rel, "RelatedObjects"),
                products_without_containers_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, agg_rel);
            } else {
                ifcapi::detail::write_ref_aggregate(agg_rel, "RelatedObjects", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    agg_rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }

        for (auto prev_rel : previous_rels) {
            auto remaining = without_set(ifcapi::detail::read_ref_aggregate(prev_rel, "RelatedElements"), products_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, prev_rel);
            } else {
                ifcapi::detail::write_ref_aggregate(prev_rel, "RelatedElements", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    prev_rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }

        if (existing_rel) {
            auto current = ifcapi::detail::read_ref_aggregate(existing_rel, "RelatedElements");
            auto current_set = ifcapi::detail::to_ref_set(current);
            current_set.insert(products_set.begin(), products_set.end());
            ifcapi::detail::write_ref_aggregate(existing_rel, "RelatedElements", ifcapi::detail::to_ref_vector(current_set));
            ifcapi::detail::update_owner_history(
                file,
                existing_rel,
                user ? *user : express::Base(),
                application ? *application : express::Base());
            return existing_rel;
        }

        auto rel = create_relationship(
            file,
            "IfcRelContainedInSpatialStructure",
            owner_history ? *owner_history : express::Base(),
            user ? *user : express::Base(),
            application ? *application : express::Base());
        if (!rel) {
            set_error("Failed to create IfcRelContainedInSpatialStructure");
            return {};
        }
        ifcapi::detail::write_ref_attr(rel, "RelatingStructure", structure);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedElements", ifcapi::detail::to_ref_vector(products_set));
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void spatial_unassign_container(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* user,
    express::Base* application)
{
    if (!file || products.empty()) {
        return;
    }

    try {
        auto products_set = to_ref_set_filtered(products);
        std::set<express::Base> rels;
        for (const auto& product : products_set) {
            auto rel = find_contained_in_structure(product);
            if (rel) {
                rels.insert(rel);
            }
        }

        for (auto rel : rels) {
            auto remaining = without_set(ifcapi::detail::read_ref_aggregate(rel, "RelatedElements"), products_set);
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedElements", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }
    } catch (...) {
    }
}

express::Base spatial_reference_structure(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* relating_structure,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    if (!file || products.empty()) {
        return {};
    }

    try {
        auto structure = relating_structure ? *relating_structure : express::Base();
        if (!structure) {
            set_error("Relating structure not found");
            return {};
        }

        auto products_set = to_ref_set_filtered(products);
        if (products_set.empty()) {
            return {};
        }

        std::set<express::Base> referenced_set;
        auto structure_ref = structure;
        auto referenced = element_get_structure_referenced_elements(&structure_ref);
        referenced_set.insert(referenced.begin(), referenced.end());

        std::vector<express::Base> products_to_assign;
        for (const auto& product : products_set) {
            if (referenced_set.find(product) == referenced_set.end()) {
                products_to_assign.push_back(product);
            }
        }

        auto references = ifcapi::detail::read_inverse_aggregate(structure, "ReferencesElements");
        auto rel = references.empty() ? express::Base() : references.front();
        if (products_to_assign.empty()) {
            return rel;
        }

        if (rel) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedElements");
            ifcapi::detail::append_unique(related, products_to_assign);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedElements", related);
            ifcapi::detail::update_owner_history(
                file,
                rel,
                user ? *user : express::Base(),
                application ? *application : express::Base());
            return rel;
        }

        rel = create_relationship(
            file,
            "IfcRelReferencedInSpatialStructure",
            owner_history ? *owner_history : express::Base(),
            user ? *user : express::Base(),
            application ? *application : express::Base());
        if (!rel) {
            set_error("Failed to create IfcRelReferencedInSpatialStructure");
            return {};
        }
        ifcapi::detail::write_ref_aggregate(rel, "RelatedElements", products_to_assign);
        ifcapi::detail::write_ref_attr(rel, "RelatingStructure", structure);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void spatial_dereference_structure(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* relating_structure,
    express::Base* user,
    express::Base* application)
{
    ifcopenshell_clear_error();
    if (!file || products.empty() || !relating_structure || !*relating_structure) {
        return;
    }

    try {
        auto products_set = to_ref_set_filtered(products);
        if (products_set.empty()) {
            return;
        }

        auto references = ifcapi::detail::read_inverse_aggregate(*relating_structure, "ReferencesElements");
        for (auto rel : references) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedElements");
            bool intersects = false;
            std::vector<express::Base> remaining;
            for (const auto& element : related) {
                if (products_set.find(element) != products_set.end()) {
                    intersects = true;
                } else {
                    remaining.push_back(element);
                }
            }
            if (!intersects) {
                continue;
            }
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedElements", remaining);
                ifcapi::detail::update_owner_history(
                    file,
                    rel,
                    user ? *user : express::Base(),
                    application ? *application : express::Base());
            }
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
