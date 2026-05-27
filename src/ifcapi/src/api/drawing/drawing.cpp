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

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

IfcUtil::IfcBaseClass* first_inverse(IfcUtil::IfcBaseClass* entity, const char* inverse_name) {
    auto values = ifcapi::detail::read_inverse_aggregate(entity, inverse_name);
    return values.empty() ? nullptr : values.front();
}

IfcUtil::IfcBaseClass* grid_for_axis(IfcUtil::IfcBaseClass* axis) {
    for (const char* inverse_name : {"PartOfW", "PartOfV", "PartOfU"}) {
        if (auto* grid = first_inverse(axis, inverse_name)) {
            return grid;
        }
    }
    return nullptr;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

IfcUtil::IfcBaseClass* create_assigns_to_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    const std::string& name,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = file->create(file->schema()->declaration_by_name("IfcRelAssignsToProduct"));
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

IfcUtil::IfcBaseClass* drawing_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        if (is_a(relating_product, "IfcGridAxis")) {
            auto* axis = relating_product;
            auto* grid = grid_for_axis(axis);
            if (!grid) {
                throw std::runtime_error("IfcGridAxis is not assigned to an IfcGrid");
            }
            const auto axis_tag = ifcapi::detail::read_string_attr(axis, "AxisTag");
            for (auto* rel : ifcapi::detail::read_inverse_aggregate(grid, "ReferencedBy")) {
                if (ifcapi::detail::read_string_attr(rel, "Name") == axis_tag &&
                    contains(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), related_object)) {
                    return nullptr;
                }
            }
            return create_assigns_to_product(
                file,
                grid,
                related_object,
                axis_tag,
                owner_history,
                user,
                application);
        }

        for (auto* rel : ifcapi::detail::read_inverse_aggregate(related_object, "HasAssignments")) {
            if (is_a(rel, "IfcRelAssignsToProduct") &&
                ifcapi::detail::read_ref_attr(rel, "RelatingProduct") == relating_product) {
                return nullptr;
            }
        }

        auto referenced_by = ifcapi::detail::read_inverse_aggregate(relating_product, "ReferencedBy");
        auto* rel = referenced_by.empty() ? nullptr : referenced_by.front();
        if (rel) {
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            related_objects.push_back(related_object);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            ifcapi::detail::update_owner_history(file, rel, user, application);
            return rel;
        }

        return create_assigns_to_product(
            file,
            relating_product,
            related_object,
            std::string(),
            owner_history,
            user,
            application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void drawing_unassign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        if (is_a(relating_product, "IfcGridAxis")) {
            relating_product = grid_for_axis(relating_product);
        }

        for (auto* rel : ifcapi::detail::read_inverse_aggregate(related_object, "HasAssignments")) {
            if (!is_a(rel, "IfcRelAssignsToProduct") ||
                ifcapi::detail::read_ref_attr(rel, "RelatingProduct") != relating_product) {
                continue;
            }
            auto related_objects = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            if (related_objects.size() == 1) {
                ifcapi::detail::remove_with_history(file, rel);
                return;
            }
            related_objects.erase(
                std::remove(related_objects.begin(), related_objects.end(), related_object),
                related_objects.end());
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
            ifcapi::detail::update_owner_history(file, rel, user, application);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
