// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_SEQUENCE_H
#define IFCAPI_BINDINGS_SEQUENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

struct SequenceDateTimeResult {
    express::Base date_time;
    std::string date_time_string;
    bool is_entity = false;
};

struct SequenceDuplicateTaskResult {
    std::vector<express::Base> current;
    std::vector<express::Base> duplicate;
};

IFCAPI_BINDING SequenceDateTimeResult sequence_add_date_time(
    ifcopenshell::file* file,
    const std::string& date_time);

IFCAPI_BINDING void sequence_calculate_task_duration(
    ifcopenshell::file* file,
    express::Base* task);

IFCAPI_BINDING void sequence_edit_task_time(
    ifcopenshell::file* file,
    express::Base* task_time,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING SequenceDuplicateTaskResult sequence_duplicate_task(
    ifcopenshell::file* file,
    express::Base* task,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_copy_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_create_baseline(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const char* name,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_add_task_time(
    ifcopenshell::file* file,
    express::Base* task,
    bool is_recurring);

IFCAPI_BINDING express::Base sequence_add_task(
    ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* work_schedule,
    IFCAPI_NULLABLE express::Base* parent_task,
    const char* name,
    const char* description,
    const char* identification,
    const std::string& predefined_type,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_add_work_calendar(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& predefined_type,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_add_work_plan(
    ifcopenshell::file* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& creation_date,
    const std::string& start_time,
    IFCAPI_NULLABLE express::Base* creator_person,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_add_work_schedule(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& predefined_type,
    const char* object_type,
    const std::string& creation_date,
    const std::string& start_time,
    IFCAPI_NULLABLE express::Base* work_plan,
    IFCAPI_NULLABLE express::Base* creator_person,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_add_work_time(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    const std::string& time_type);

IFCAPI_BINDING express::Base sequence_add_time_period(
    ifcopenshell::file* file,
    express::Base* recurrence_pattern,
    const char* start_time,
    const char* end_time);

IFCAPI_BINDING express::Base sequence_assign_sequence(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_process,
    const std::string& sequence_type,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_cascade_schedule(
    ifcopenshell::file* file,
    express::Base* task);

IFCAPI_BINDING void sequence_recalculate_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule);

IFCAPI_BINDING express::Base sequence_assign_lag_time(
    ifcopenshell::file* file,
    express::Base* rel_sequence,
    const std::string& lag_value,
    const std::string& duration_type);

IFCAPI_BINDING express::Base sequence_assign_process(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_assign_work_plan(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    express::Base* work_plan,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_assign_recurrence_pattern(
    ifcopenshell::file* file,
    express::Base* parent,
    const std::string& recurrence_type);

IFCAPI_BINDING void sequence_edit_lag_time(
    express::Base* lag_time,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_sequence(
    express::Base* rel_sequence,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_task(
    express::Base* task,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_recurrence_pattern(
    express::Base* recurrence_pattern,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_calendar(
    express::Base* work_calendar,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_plan(
    express::Base* work_plan,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_schedule(
    express::Base* work_schedule,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_edit_work_time(
    express::Base* work_time,
    ifcopenshell_pset_props_t* attributes);

IFCAPI_BINDING void sequence_unassign_lag_time(
    ifcopenshell::file* file,
    express::Base* rel_sequence);

IFCAPI_BINDING void sequence_unassign_sequence(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_process);

IFCAPI_BINDING express::Base sequence_unassign_process(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING express::Base sequence_unassign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_unassign_recurrence_pattern(
    ifcopenshell::file* file,
    express::Base* recurrence_pattern);

IFCAPI_BINDING void sequence_remove_time_period(
    ifcopenshell::file* file,
    express::Base* time_period);

IFCAPI_BINDING void sequence_remove_work_time(
    ifcopenshell::file* file,
    express::Base* work_time);

IFCAPI_BINDING void sequence_remove_task(
    ifcopenshell::file* file,
    express::Base* task,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_remove_work_calendar(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_remove_work_plan(
    ifcopenshell::file* file,
    express::Base* work_plan,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

IFCAPI_BINDING void sequence_remove_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SEQUENCE_H */
