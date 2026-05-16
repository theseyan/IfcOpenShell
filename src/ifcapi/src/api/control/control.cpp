// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/control.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcSchema.h"

#include <algorithm>
#include <set>
#include <string>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

std::set<IfcUtil::IfcBaseClass*> to_mutable_set(const std::vector<const IfcUtil::IfcBaseClass*>& values) {
    std::set<IfcUtil::IfcBaseClass*> result;
    for (auto* value : values) {
        if (value) {
            result.insert(const_cast<IfcUtil::IfcBaseClass*>(value));
        }
    }
    return result;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* control_assign_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_control,
    const std::vector<const IfcUtil::IfcBaseClass*>& related_objects,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto related_set = to_mutable_set(related_objects);
        std::vector<IfcUtil::IfcBaseClass*> objects_to_assign;
        auto control_assignments = ifcapi::detail::read_inverse_aggregate(relating_control, "Controls");
        if (!control_assignments.empty()) {
            for (auto* object : related_set) {
                bool already_assigned = false;
                for (auto* assignment : ifcapi::detail::read_inverse_aggregate(object, "HasAssignments")) {
                    if (contains(control_assignments, assignment)) {
                        already_assigned = true;
                        break;
                    }
                }
                if (!already_assigned) objects_to_assign.push_back(object);
            }
        } else {
            objects_to_assign.assign(related_set.begin(), related_set.end());
        }

        if (objects_to_assign.empty()) return nullptr;

        auto* controls = control_assignments.empty() ? nullptr : control_assignments.front();
        if (controls) {
            auto related = ifcapi::detail::read_ref_aggregate(controls, "RelatedObjects");
            related.insert(related.end(), objects_to_assign.begin(), objects_to_assign.end());
            ifcapi::detail::write_ref_aggregate(controls, "RelatedObjects", related);
            ifcapi::detail::update_owner_history(file, controls, user, application);
            return controls;
        }

        auto* rel = file->create(file->schema()->declaration_by_name("IfcRelAssignsToControl"));
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            rel,
            "OwnerHistory",
            ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", objects_to_assign);
        ifcapi::detail::write_ref_attr(rel, "RelatingControl", relating_control);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void control_unassign_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_control,
    const std::vector<const IfcUtil::IfcBaseClass*>& related_objects,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto related_set = to_mutable_set(related_objects);
        auto control_assignments = ifcapi::detail::read_inverse_aggregate(relating_control, "Controls");
        std::set<IfcUtil::IfcBaseClass*> rels;
        for (auto* object : related_set) {
            for (auto* assignment : ifcapi::detail::read_inverse_aggregate(object, "HasAssignments")) {
                if (contains(control_assignments, assignment)) rels.insert(assignment);
            }
        }

        for (auto* rel : rels) {
            std::vector<IfcUtil::IfcBaseClass*> remaining;
            for (auto* object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                if (related_set.find(object) == related_set.end()) remaining.push_back(object);
            }
            if (remaining.empty()) {
                ifcapi::detail::remove_with_history(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
                ifcapi::detail::update_owner_history(file, rel, user, application);
            }
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
