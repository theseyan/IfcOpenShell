// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_RESOURCE_H
#define IFCAPI_BINDINGS_RESOURCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

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
