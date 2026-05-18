// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_SEQUENCE_H
#define IFCAPI_BINDINGS_SEQUENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_task_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    bool is_recurring);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_task(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* parent_task,
    const char* name,
    const char* description,
    const char* identification,
    const std::string& predefined_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_work_calendar(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& predefined_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_work_plan(
    IfcParse::IfcFile* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& creation_date,
    const std::string& start_time,
    IfcUtil::IfcBaseClass* creator_person,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_work_schedule(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& predefined_type,
    const char* object_type,
    const std::string& creation_date,
    const std::string& start_time,
    IfcUtil::IfcBaseClass* work_plan,
    IfcUtil::IfcBaseClass* creator_person,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_work_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_calendar,
    const std::string& time_type);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_add_time_period(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* recurrence_pattern,
    const char* start_time,
    const char* end_time);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_sequence(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_process,
    const std::string& sequence_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING void sequence_cascade_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task);

IFCAPI_BINDING void sequence_recalculate_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_lag_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* rel_sequence,
    const std::string& lag_value,
    const std::string& duration_type);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_process(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_work_plan(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* work_plan,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_assign_recurrence_pattern(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    const std::string& recurrence_type);

IFCAPI_BINDING void sequence_edit_lag_time(
    IfcUtil::IfcBaseClass* lag_time,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_sequence(
    IfcUtil::IfcBaseClass* rel_sequence,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_task(
    IfcUtil::IfcBaseClass* task,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_recurrence_pattern(
    IfcUtil::IfcBaseClass* recurrence_pattern,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_calendar(
    IfcUtil::IfcBaseClass* work_calendar,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_plan(
    IfcUtil::IfcBaseClass* work_plan,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_schedule(
    IfcUtil::IfcBaseClass* work_schedule,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_time(
    IfcUtil::IfcBaseClass* work_time,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_unassign_lag_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* rel_sequence);

IFCAPI_BINDING void sequence_unassign_sequence(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_process);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_unassign_process(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING IfcUtil::IfcBaseClass* sequence_unassign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

IFCAPI_BINDING void sequence_unassign_recurrence_pattern(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* recurrence_pattern);

IFCAPI_BINDING void sequence_remove_time_period(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* time_period);

IFCAPI_BINDING void sequence_remove_work_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_time);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SEQUENCE_H */
