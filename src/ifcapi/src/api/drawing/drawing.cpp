// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/drawing.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/schema.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

express::Base first_inverse(express::Base entity, const char* inverse_name) {
    auto values = ifcapi::detail::read_inverse_aggregate(entity, inverse_name);
    return values.empty() ? express::Base() : values.front();
}

express::Base grid_for_axis(express::Base axis) {
    for (const char* inverse_name : {"PartOfW", "PartOfV", "PartOfU"}) {
        if (auto grid = first_inverse(axis, inverse_name)) {
            return grid;
        }
    }
    return {};
}

bool contains(const std::vector<express::Base>& values, express::Base value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

express::Base create_assigns_to_product(
    ifcopenshell::file* file,
    express::Base relating_product,
    express::Base related_object,
    const std::string& name,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto rel = file->create(file->schema()->declaration_by_name("IfcRelAssignsToProduct"));
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    if (!name.empty()) {
        ifcapi::detail::write_string_attr(rel, "Name", name);
    }
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object});
    ifcapi::detail::write_ref_attr(rel, "RelatingProduct", relating_product);
    return rel;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base drawing_assign_product(
    ifcopenshell::file* file,
    const DrawingAssignProductOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto relating_product_value = options.relating_product;
        auto related_object_value = options.related_object;
        auto owner_history_value = options.owner_history.value_or(express::Base());
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        if (is_a(relating_product_value, "IfcGridAxis")) {
            auto axis = relating_product_value;
            auto grid = grid_for_axis(axis);
            if (!grid) {
                throw std::runtime_error("IfcGridAxis is not assigned to an IfcGrid");
            }
            const auto axis_tag = ifcapi::detail::read_string_attr(axis, "AxisTag");
            for (auto rel : ifcapi::detail::read_inverse_aggregate(grid, "ReferencedBy")) {
                if (ifcapi::detail::read_string_attr(rel, "Name") == axis_tag &&
                    contains(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), related_object_value)) {
                    return {};
                }
            }
            return create_assigns_to_product(
                file,
                grid,
                related_object_value,
                axis_tag,
                owner_history_value,
                user_value,
                application_value);
        }

        for (auto rel : ifcapi::detail::read_inverse_aggregate(related_object_value, "HasAssignments")) {
            if (is_a(rel, "IfcRelAssignsToProduct") &&
                ifcapi::detail::read_ref_attr(rel, "RelatingProduct") == relating_product_value) {
                return {};
            }
        }

        auto referenced_by = ifcapi::detail::read_inverse_aggregate(relating_product_value, "ReferencedBy");
        auto rel = referenced_by.empty() ? express::Base() : referenced_by.front();
        if (rel) {
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            related_objects.push_back(related_object_value);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
            return rel;
        }

        return create_assigns_to_product(
            file,
            relating_product_value,
            related_object_value,
            std::string(),
            owner_history_value,
            user_value,
            application_value);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void drawing_unassign_product(
    ifcopenshell::file* file,
    const DrawingUnassignProductOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto relating_product_value = options.relating_product;
        auto related_object_value = options.related_object;
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        if (is_a(relating_product_value, "IfcGridAxis")) {
            relating_product_value = grid_for_axis(relating_product_value);
        }

        for (auto rel : ifcapi::detail::read_inverse_aggregate(related_object_value, "HasAssignments")) {
            if (!is_a(rel, "IfcRelAssignsToProduct") ||
                ifcapi::detail::read_ref_attr(rel, "RelatingProduct") != relating_product_value) {
                continue;
            }
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            if (related_objects.size() == 1) {
                ifcapi::detail::remove_with_history(file, rel);
                return;
            }
            related_objects.erase(
                std::remove(related_objects.begin(), related_objects.end(), related_object_value),
                related_objects.end());
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
