// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_RESOURCE_H
#define IFCAPI_BINDINGS_RESOURCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

struct ResourceAddResourceOptions {
    std::optional<express::Base> parent_resource;
    std::optional<std::string> ifc_class = std::string("IfcCrewResource");
    std::optional<std::string> name;
    std::optional<std::string> predefined_type = std::string("NOTDEFINED");
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/** Create a construction resource, nesting it below a parent when supplied or declaring it to the first IFC4+ context. */
IFCAPI_BINDING express::Base resource_add_resource(ifcopenshell::file* file, const ResourceAddResourceOptions& options);

/** Create and attach a schema-valid base quantity. Validation precedes replacement of any existing quantity. */
IFCAPI_BINDING express::Base resource_add_resource_quantity(
    ifcopenshell::file* file, express::Base* resource, const std::string& ifc_class = "IfcQuantityCount");

/** Create an IfcResourceTime and replace the resource Usage reference. */
IFCAPI_BINDING express::Base resource_add_resource_time(ifcopenshell::file* file, express::Base* resource);

struct ResourceAssignmentOptions {
    express::Base relating_resource;
    express::Base related_object;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/** Assign one product or actor to a resource, reusing its ordered relationship and suppressing duplicates. */
IFCAPI_BINDING express::Base resource_assign_resource(ifcopenshell::file* file, const ResourceAssignmentOptions& options);

/** Remove exactly one resource/object assignment pair, preserving other ordered members. */
IFCAPI_BINDING void resource_unassign_resource(ifcopenshell::file* file, const ResourceAssignmentOptions& options);

/** Calculate ScheduleWork from EPset_Productivity and the first applicable task/product assignments. */
IFCAPI_BINDING void resource_calculate_resource_work(ifcopenshell::file* file, express::Base* resource);

/** Calculate ScheduleUsage from ScheduleWork and the first applicable task duration. */
IFCAPI_BINDING void resource_calculate_resource_usage(ifcopenshell::file* file, express::Base* resource);

struct ResourceRemoveResourceOptions {
    express::Base resource;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/** Recursively remove a resource and clean its nesting, declaration, control, resource assignments, usage, quantity, and orphan history. */
IFCAPI_BINDING void resource_remove_resource(ifcopenshell::file* file, const ResourceRemoveResourceOptions& options);

/** Detach and deep-remove the current base quantity, or do nothing when absent. */
IFCAPI_BINDING void resource_remove_resource_quantity(ifcopenshell::file* file, express::Base* resource);

/**
 * Edit attributes of an IfcResourceTime entity.
 *
 * Applies attribute changes from the property bag to the resource time.
 * Attributes that are hard-constrained (ConstraintGrade HARD and Benchmark
 * EQUALTO) on the owning IfcResource are skipped. If ScheduleUsage is
 * changed and a ScheduleWork constraint exists, the associated IfcTask
 * duration is recalculated. Duration values use ISO 8601 format
 * (e.g. "PT8H", "P5D"). Date-time values use ISO 8601 extended format
 * (e.g. "2024-01-15T09:00:00").
 *
 * @param file File containing the resource time.
 * @param resource_time IfcResourceTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void resource_edit_resource_time(
    ifcopenshell::file* file,
    express::Base* resource_time,
    ifcopenshell_pset_props_t* attributes);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_RESOURCE_H */
