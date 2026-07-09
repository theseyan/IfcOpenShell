// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_SEQUENCE_H
#define IFCAPI_BINDINGS_SEQUENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>
#include <variant>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

struct SequenceDuplicateTaskResult {
    std::vector<express::Base> current;
    std::vector<express::Base> duplicate;
};

struct SequenceDuplicateTaskOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceCopyWorkScheduleOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceCreateBaselineOptions {
    std::optional<std::string> name;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAddTaskTimeOptions {
    std::optional<bool> is_recurring;
};

struct SequenceAddTaskOptions {
    std::optional<express::Base> work_schedule;
    std::optional<express::Base> parent_task;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> identification;
    std::optional<std::string> predefined_type;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAddWorkCalendarOptions {
    std::optional<std::string> name;
    std::optional<std::string> predefined_type;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAddWorkPlanOptions {
    std::optional<std::string> name;
    std::optional<std::string> predefined_type;
    std::optional<std::string> creation_date;
    std::optional<std::string> start_time;
    std::optional<express::Base> creator_person;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAddWorkScheduleOptions {
    std::optional<std::string> name;
    std::optional<std::string> predefined_type;
    std::optional<std::string> object_type;
    std::optional<std::string> creation_date;
    std::optional<std::string> start_time;
    std::optional<express::Base> work_plan;
    std::optional<express::Base> creator_person;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAddTimePeriodOptions {
    std::optional<std::string> start_time;
    std::optional<std::string> end_time;
};

struct SequenceAssignSequenceOptions {
    std::optional<std::string> sequence_type;
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAssignLagTimeOptions {
    std::optional<std::string> duration_type;
};

struct SequenceAssignProcessOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAssignProductOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceAssignWorkPlanOptions {
    std::optional<express::Base> owner_history;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

struct SequenceRemoveOptions {
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING std::variant<express::Base, std::string> sequence_add_date_time(
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
    const SequenceDuplicateTaskOptions& options);

IFCAPI_BINDING express::Base sequence_copy_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceCopyWorkScheduleOptions& options);

IFCAPI_BINDING void sequence_create_baseline(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceCreateBaselineOptions& options);

IFCAPI_BINDING express::Base sequence_add_task_time(
    ifcopenshell::file* file,
    express::Base* task,
    const SequenceAddTaskTimeOptions& options);

IFCAPI_BINDING express::Base sequence_add_task(
    ifcopenshell::file* file,
    const SequenceAddTaskOptions& options);

IFCAPI_BINDING express::Base sequence_add_work_calendar(
    ifcopenshell::file* file,
    const SequenceAddWorkCalendarOptions& options);

IFCAPI_BINDING express::Base sequence_add_work_plan(
    ifcopenshell::file* file,
    const SequenceAddWorkPlanOptions& options);

IFCAPI_BINDING express::Base sequence_add_work_schedule(
    ifcopenshell::file* file,
    const SequenceAddWorkScheduleOptions& options);

IFCAPI_BINDING express::Base sequence_add_work_time(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    const std::string& time_type);

IFCAPI_BINDING express::Base sequence_add_time_period(
    ifcopenshell::file* file,
    express::Base* recurrence_pattern,
    const SequenceAddTimePeriodOptions& options);

IFCAPI_BINDING express::Base sequence_assign_sequence(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_process,
    const SequenceAssignSequenceOptions& options);

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
    const SequenceAssignLagTimeOptions& options);

IFCAPI_BINDING express::Base sequence_assign_process(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_object,
    const SequenceAssignProcessOptions& options);

IFCAPI_BINDING express::Base sequence_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    const SequenceAssignProductOptions& options);

IFCAPI_BINDING express::Base sequence_assign_work_plan(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    express::Base* work_plan,
    const SequenceAssignWorkPlanOptions& options);

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
    const SequenceRemoveOptions& options);

IFCAPI_BINDING express::Base sequence_unassign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    const SequenceRemoveOptions& options);

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
    const SequenceRemoveOptions& options);

IFCAPI_BINDING void sequence_remove_work_calendar(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    const SequenceRemoveOptions& options);

IFCAPI_BINDING void sequence_remove_work_plan(
    ifcopenshell::file* file,
    express::Base* work_plan,
    const SequenceRemoveOptions& options);

IFCAPI_BINDING void sequence_remove_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceRemoveOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SEQUENCE_H */
