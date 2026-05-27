// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/control.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <vector>

namespace {

bool contains(const std::vector<express::Base>& values, express::Base value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

std::vector<express::Base> unique_valid(const std::vector<express::Base>& values) {
    std::vector<express::Base> result;
    for (auto value : values) {
        if (value && !contains(result, value)) {
            result.push_back(value);
        }
    }
    return result;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base control_assign_control(
    ifcopenshell::file* file,
    express::Base* relating_control,
    const std::vector<express::Base>& related_objects,
    express::Base* owner_history,
    express::Base* user,
    express::Base* application)
{
    auto control = detail::deref_or_empty(relating_control);
    auto related_set = unique_valid(related_objects);
    if (!file || !control || related_set.empty()) {
        return {};
    }

    std::vector<express::Base> objects_to_assign;
    auto control_assignments = detail::read_inverse_aggregate(control, "Controls");
    if (!control_assignments.empty()) {
        for (auto object : related_set) {
            bool already_assigned = false;
            for (auto assignment : detail::read_inverse_aggregate(object, "HasAssignments")) {
                if (contains(control_assignments, assignment)) {
                    already_assigned = true;
                    break;
                }
            }
            if (!already_assigned) {
                objects_to_assign.push_back(object);
            }
        }
    } else {
        objects_to_assign = related_set;
    }

    if (objects_to_assign.empty()) {
        return {};
    }

    auto controls = control_assignments.empty() ? express::Base() : control_assignments.front();
    if (controls) {
        auto related = detail::read_ref_aggregate(controls, "RelatedObjects");
        for (auto object : objects_to_assign) {
            if (!contains(related, object)) {
                related.push_back(object);
            }
        }
        detail::write_ref_aggregate(controls, "RelatedObjects", related);
        detail::update_owner_history(file, controls, detail::deref_or_empty(user), detail::deref_or_empty(application));
        return controls;
    }

    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssignsToControl");
    auto rel = file->create(declaration);
    detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    detail::write_ref_attr(
        rel,
        "OwnerHistory",
        detail::ensure_owner_history(
            file, detail::deref_or_empty(owner_history), detail::deref_or_empty(user), detail::deref_or_empty(application)));
    detail::write_ref_aggregate(rel, "RelatedObjects", objects_to_assign);
    detail::write_ref_attr(rel, "RelatingControl", control);
    return rel;
}

void control_unassign_control(
    ifcopenshell::file* file,
    express::Base* relating_control,
    const std::vector<express::Base>& related_objects,
    express::Base* user,
    express::Base* application)
{
    auto control = detail::deref_or_empty(relating_control);
    auto related_set = unique_valid(related_objects);
    if (!file || !control || related_set.empty()) {
        return;
    }

    auto control_assignments = detail::read_inverse_aggregate(control, "Controls");
    std::vector<express::Base> rels;
    for (auto object : related_set) {
        for (auto assignment : detail::read_inverse_aggregate(object, "HasAssignments")) {
            if (contains(control_assignments, assignment) && !contains(rels, assignment)) {
                rels.push_back(assignment);
            }
        }
    }

    for (auto rel : rels) {
        std::vector<express::Base> remaining;
        for (auto object : detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (!contains(related_set, object)) {
                remaining.push_back(object);
            }
        }
        if (remaining.empty()) {
            detail::remove_with_history(file, rel);
        } else {
            detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            detail::update_owner_history(file, rel, detail::deref_or_empty(user), detail::deref_or_empty(application));
        }
    }
}

} // namespace bindings
} // namespace ifcapi
