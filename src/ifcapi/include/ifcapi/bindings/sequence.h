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

/**
 * Result of duplicating a task tree.
 */
struct SequenceDuplicateTaskResult {
    /// Original tasks in depth-first order.
    std::vector<express::Base> current;
    /// Corresponding duplicated tasks in the same order.
    std::vector<express::Base> duplicate;
};

struct SequenceDuplicateTaskOptions {
    /// Owner history applied to duplicated entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceCopyWorkScheduleOptions {
    /// Owner history applied to copied entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceCreateBaselineOptions {
    /// Name for the baseline schedule. When omitted, the baseline Name is left blank.
    std::optional<std::string> name;
    /// Owner history applied to baseline entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAddTaskTimeOptions {
    /// When true, creates an IfcTaskTimeRecurring instead of IfcTaskTime.
    std::optional<bool> is_recurring;
};

struct SequenceAddTaskOptions {
    /// IfcWorkSchedule to assign the task to via IfcRelAssignsToControl. Takes precedence over parent_task when both are provided.
    std::optional<express::Base> work_schedule;
    /// Parent IfcTask to nest under via IfcRelNests. Used only when work_schedule is omitted.
    std::optional<express::Base> parent_task;
    /// Task name.
    std::optional<std::string> name;
    /// Task description.
    std::optional<std::string> description;
    /// Task identification string. For nested tasks, auto-generated as "parent.N" when omitted.
    std::optional<std::string> identification;
    /// Predefined type enum value. Defaults to "NOTDEFINED".
    std::optional<std::string> predefined_type;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAddWorkCalendarOptions {
    /// Calendar name. Defaults to "Unnamed".
    std::optional<std::string> name;
    /// Predefined type enum value. Defaults to "NOTDEFINED".
    std::optional<std::string> predefined_type;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAddWorkPlanOptions {
    /// Work plan name.
    std::optional<std::string> name;
    /// Predefined type enum value. Defaults to "NOTDEFINED".
    std::optional<std::string> predefined_type;
    /// ISO 8601 date-time for CreationDate. Defaults to the current time.
    std::optional<std::string> creation_date;
    /// ISO 8601 date-time for StartTime. Defaults to creation_date.
    std::optional<std::string> start_time;
    /// IfcPerson to set as the creator. Appended to the Creators aggregate.
    std::optional<express::Base> creator_person;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAddWorkScheduleOptions {
    /// Schedule name. Defaults to "Unnamed".
    std::optional<std::string> name;
    /// Predefined type enum value. Defaults to "NOTDEFINED".
    std::optional<std::string> predefined_type;
    /// ObjectType string.
    std::optional<std::string> object_type;
    /// ISO 8601 date-time for CreationDate. Defaults to the current time.
    std::optional<std::string> creation_date;
    /// ISO 8601 date-time for StartTime. Defaults to creation_date.
    std::optional<std::string> start_time;
    /// IfcWorkPlan to nest the schedule under via IfcRelAggregates.
    std::optional<express::Base> work_plan;
    /// IfcPerson to set as the creator. Appended to the Creators aggregate.
    std::optional<express::Base> creator_person;
    /// Owner history applied to created entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAddTimePeriodOptions {
    /// ISO 8601 time string for the StartTime attribute (e.g. "08:00:00").
    std::optional<std::string> start_time;
    /// ISO 8601 time string for the EndTime attribute (e.g. "17:00:00").
    std::optional<std::string> end_time;
};

struct SequenceAssignSequenceOptions {
    /// Sequence type enum value. Defaults to "FINISH_START".
    std::optional<std::string> sequence_type;
    /// Owner history applied to the IfcRelSequence. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAssignLagTimeOptions {
    /// Duration type enum value for the lag. Defaults to "WORKTIME".
    std::optional<std::string> duration_type;
};

struct SequenceAssignProcessOptions {
    /// Owner history applied to the IfcRelAssignsToProcess. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAssignProductOptions {
    /// Owner history applied to the IfcRelAssignsToProduct. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceAssignWorkPlanOptions {
    /// Owner history applied to the aggregate relationship. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

struct SequenceRemoveOptions {
    /// IfcPersonAndOrganization used to update or remove relationships.
    std::optional<express::Base> user;
    /// IfcApplication used to update or remove relationships.
    std::optional<express::Base> application;
};

/**
 * Parse an ISO 8601 date-time string and return a schema-appropriate representation.
 *
 * For IFC4+, returns the string as-is. For IFC2X3, returns an IfcDateAndTime
 * entity constructed from the parsed components.
 *
 * @param file File to determine the schema version.
 * @param date_time ISO 8601 date-time string (e.g. "2024-01-15T09:00:00").
 * @return An IfcDateAndTime entity (IFC2X3) or the original string (IFC4+).
 */
IFCAPI_BINDING std::variant<express::Base, std::string> sequence_add_date_time(
    ifcopenshell::file* file,
    const std::string& date_time);

/**
 * Recalculate a task's ScheduleDuration from its assigned resources.
 *
 * Computes the maximum resource usage duration across all IfcConstructionResource
 * entities assigned to the task (directly or via IfcRelAssignsToProcess),
 * accounting for ScheduleWork and ScheduleUsage. The result is written to the
 * task's IfcTaskTime.ScheduleDuration. Creates an IfcTaskTime if absent.
 *
 * @param file File containing the task.
 * @param task IfcTask entity to recalculate.
 */
IFCAPI_BINDING void sequence_calculate_task_duration(
    ifcopenshell::file* file,
    express::Base* task);

/**
 * Edit attributes of an IfcTaskTime entity.
 *
 * Applies attribute changes from the property bag. Automatically derives
 * ScheduleFinish from ScheduleStart + ScheduleDuration, or ScheduleDuration
 * from ScheduleStart and ScheduleFinish, as appropriate. Dates are normalised
 * to the soonest working day based on the task's calendar. After editing,
 * cascades schedule changes to successor tasks.
 *
 * @param file File containing the task time.
 * @param task_time IfcTaskTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_task_time(
    ifcopenshell::file* file,
    express::Base* task_time,
    ifcopenshell_pset_props_t* attributes);

/**
 * Deep-copy a task and its subtasks, property sets, and sequence relationships.
 *
 * Creates duplicates of the task, its nested child tasks, property sets, and
 * IfcRelSequence relationships between duplicated tasks. Returns parallel
 * vectors of original and duplicated tasks in depth-first order.
 *
 * @param file File that receives the duplicated entities.
 * @param task IfcTask to duplicate.
 * @param options Ownership options for duplicated entities.
 * @return Parallel vectors of original and duplicated tasks.
 */
IFCAPI_BINDING SequenceDuplicateTaskResult sequence_duplicate_task(
    ifcopenshell::file* file,
    express::Base* task,
    const SequenceDuplicateTaskOptions& options);

/**
 * Deep-copy an IfcWorkSchedule and all its controlled tasks.
 *
 * Shallow-copies the schedule, then deep-copies each controlled IfcTask
 * (with its subtasks and relationships) and assigns the copies to the new
 * schedule.
 *
 * @param file File that receives the copied entities.
 * @param work_schedule IfcWorkSchedule to copy.
 * @param options Ownership options for the copied entities.
 * @return Newly created IfcWorkSchedule.
 */
IFCAPI_BINDING express::Base sequence_copy_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceCopyWorkScheduleOptions& options);

/**
 * Create a baseline snapshot of an IfcWorkSchedule.
 *
 * Only operates on schedules with PredefinedType PLANNED. Creates a new
 * IfcWorkSchedule with PredefinedType BASELINE, deep-copies all controlled
 * tasks into it, and creates IfcRelDefinesByObject references linking
 * original tasks to their baseline copies.
 *
 * @param file File that receives the baseline entities.
 * @param work_schedule IfcWorkSchedule to baseline.
 * @param options Name override and ownership options.
 */
IFCAPI_BINDING void sequence_create_baseline(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceCreateBaselineOptions& options);

/**
 * Create an IfcTaskTime (or IfcTaskTimeRecurring) and assign it to a task.
 *
 * Replaces any existing TaskTime on the task.
 *
 * @param file File that receives the new entity.
 * @param task IfcTask to receive the task time.
 * @param options Whether to create a recurring task time.
 * @return Newly created IfcTaskTime or IfcTaskTimeRecurring.
 */
IFCAPI_BINDING express::Base sequence_add_task_time(
    ifcopenshell::file* file,
    express::Base* task,
    const SequenceAddTaskTimeOptions& options);

/**
 * Create an IfcTask entity.
 *
 * When both work_schedule and parent_task are provided, work_schedule takes
 * precedence and the task is assigned to it via IfcRelAssignsToControl.
 * When only parent_task is provided, the task is nested under it via
 * IfcRelNests and its Identification is auto-generated as "parent.N" on IFC4+.
 *
 * @param file File that receives the new entity.
 * @param options Task placement and attribute options.
 * @return Newly created IfcTask.
 */
IFCAPI_BINDING express::Base sequence_add_task(
    ifcopenshell::file* file,
    const SequenceAddTaskOptions& options);

/**
 * Create an IfcWorkCalendar and declare it on the project.
 *
 * @param file File that receives the new entity.
 * @param options Calendar name, predefined type, and ownership options.
 * @return Newly created IfcWorkCalendar.
 */
IFCAPI_BINDING express::Base sequence_add_work_calendar(
    ifcopenshell::file* file,
    const SequenceAddWorkCalendarOptions& options);

/**
 * Create an IfcWorkPlan and declare it on the project.
 *
 * @param file File that receives the new entity.
 * @param options Work plan name, dates, creator, and ownership options.
 * @return Newly created IfcWorkPlan.
 */
IFCAPI_BINDING express::Base sequence_add_work_plan(
    ifcopenshell::file* file,
    const SequenceAddWorkPlanOptions& options);

/**
 * Create an IfcWorkSchedule.
 *
 * When work_plan is provided, the schedule is nested under it via
 * IfcRelAggregates. Otherwise it is declared directly on the project
 * context (IFC4+).
 *
 * @param file File that receives the new entity.
 * @param options Schedule name, type, dates, work plan, and ownership options.
 * @return Newly created IfcWorkSchedule.
 */
IFCAPI_BINDING express::Base sequence_add_work_schedule(
    ifcopenshell::file* file,
    const SequenceAddWorkScheduleOptions& options);

/**
 * Create an IfcWorkTime and add it to a work calendar.
 *
 * @param file File that receives the new entity.
 * @param work_calendar IfcWorkCalendar to receive the work time.
 * @param time_type "WorkingTimes" or "ExceptionTimes".
 * @return Newly created IfcWorkTime.
 */
IFCAPI_BINDING express::Base sequence_add_work_time(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    const std::string& time_type);

/**
 * Create an IfcTimePeriod and add it to a recurrence pattern.
 *
 * @param file File that receives the new entity.
 * @param recurrence_pattern IfcRecurrencePattern to receive the time period.
 * @param options Start and end time strings.
 * @return Newly created IfcTimePeriod.
 */
IFCAPI_BINDING express::Base sequence_add_time_period(
    ifcopenshell::file* file,
    express::Base* recurrence_pattern,
    const SequenceAddTimePeriodOptions& options);

/**
 * Create an IfcRelSequence linking two tasks.
 *
 * If a sequence relationship already exists between the two tasks, the
 * existing relationship is returned. After creating the relationship,
 * cascades schedule changes from the relating (predecessor) task.
 *
 * @param file File that receives the relationship.
 * @param relating_process Predecessor IfcTask.
 * @param related_process Successor IfcTask.
 * @param options Sequence type and ownership options.
 * @return The IfcRelSequence relationship.
 */
IFCAPI_BINDING express::Base sequence_assign_sequence(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_process,
    const SequenceAssignSequenceOptions& options);

/**
 * Cascade schedule dates from a task to all its successors.
 *
 * Propagates ScheduleStart and ScheduleFinish dates forward through the
 * task network, respecting sequence types (FS, SS, FF, SF), lag times,
 * duration types, and work calendars.
 *
 * @param file File containing the task network.
 * @param task IfcTask from which to cascade.
 */
IFCAPI_BINDING void sequence_cascade_schedule(
    ifcopenshell::file* file,
    express::Base* task);

/**
 * Recalculate an entire work schedule using the critical path method.
 *
 * Performs forward and backward passes through the task network to compute
 * early/late start/finish dates, total float, free float, and critical
 * path flags on each IfcTaskTime.
 *
 * @param file File containing the work schedule.
 * @param work_schedule IfcWorkSchedule to recalculate.
 */
IFCAPI_BINDING void sequence_recalculate_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule);

/**
 * Create an IfcLagTime and assign it to an IfcRelSequence.
 *
 * Replaces any existing TimeLag on the sequence (removing the old one if
 * it is sole-referenced).
 *
 * @param file File that receives the new entity.
 * @param rel_sequence IfcRelSequence to receive the lag time.
 * @param lag_value ISO 8601 duration string (e.g. "P2D", "PT8H").
 * @param options Duration type option. Defaults to "WORKTIME".
 * @return Newly created IfcLagTime.
 */
IFCAPI_BINDING express::Base sequence_assign_lag_time(
    ifcopenshell::file* file,
    express::Base* rel_sequence,
    const std::string& lag_value,
    const SequenceAssignLagTimeOptions& options);

/**
 * Assign a process (IfcTask) to an object via IfcRelAssignsToProcess.
 *
 * If a relationship already exists for this process and object, it is
 * returned. Otherwise a new relationship is created or the object is
 * appended to an existing relationship.
 *
 * @param file File that receives the relationship.
 * @param relating_process IfcTask (process) to assign.
 * @param related_object Object to assign to the process.
 * @param options Ownership options.
 * @return The IfcRelAssignsToProcess relationship.
 */
IFCAPI_BINDING express::Base sequence_assign_process(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_object,
    const SequenceAssignProcessOptions& options);

/**
 * Assign a product to an object via IfcRelAssignsToProduct.
 *
 * @param file File that receives the relationship.
 * @param relating_product IfcProduct to assign.
 * @param related_object Object to assign to the product.
 * @param options Ownership options.
 * @return The IfcRelAssignsToProduct relationship.
 */
IFCAPI_BINDING express::Base sequence_assign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    const SequenceAssignProductOptions& options);

/**
 * Assign an IfcWorkSchedule to an IfcWorkPlan via IfcRelAggregates.
 *
 * First unassigns the schedule from its current project declaration, then
 * nests it under the work plan.
 *
 * @param file File containing both entities.
 * @param work_schedule IfcWorkSchedule to assign.
 * @param work_plan IfcWorkPlan to assign to.
 * @param options Ownership options.
 * @return The IfcRelAggregates relationship.
 */
IFCAPI_BINDING express::Base sequence_assign_work_plan(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    express::Base* work_plan,
    const SequenceAssignWorkPlanOptions& options);

/**
 * Create an IfcRecurrencePattern and assign it to a parent entity.
 *
 * Assigns the pattern to an IfcWorkTime's RecurrencePattern or an
 * IfcTaskTimeRecurring's Recurrence attribute. Replaces any existing
 * pattern on the parent (removing the old one if sole-referenced).
 *
 * @param file File that receives the new entity.
 * @param parent IfcWorkTime or IfcTaskTimeRecurring to receive the pattern.
 * @param recurrence_type RecurrenceType enum value (e.g. "WEEKLY").
 * @return Newly created IfcRecurrencePattern.
 */
IFCAPI_BINDING express::Base sequence_assign_recurrence_pattern(
    ifcopenshell::file* file,
    express::Base* parent,
    const std::string& recurrence_type);

/**
 * Edit attributes of an IfcLagTime entity.
 *
 * @param lag_time IfcLagTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_lag_time(
    express::Base* lag_time,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcRelSequence entity.
 *
 * @param rel_sequence IfcRelSequence entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_sequence(
    express::Base* rel_sequence,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcTask entity.
 *
 * @param task IfcTask entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_task(
    express::Base* task,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcRecurrencePattern entity.
 *
 * @param recurrence_pattern IfcRecurrencePattern entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_recurrence_pattern(
    express::Base* recurrence_pattern,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcWorkCalendar entity.
 *
 * @param work_calendar IfcWorkCalendar entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_work_calendar(
    express::Base* work_calendar,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcWorkPlan entity.
 *
 * @param work_plan IfcWorkPlan entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_work_plan(
    express::Base* work_plan,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcWorkSchedule entity.
 *
 * @param work_schedule IfcWorkSchedule entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_work_schedule(
    express::Base* work_schedule,
    ifcopenshell_pset_props_t* attributes);

/**
 * Edit attributes of an IfcWorkTime entity.
 *
 * Date attributes "Start"/"StartDate" and "Finish"/"FinishDate" are
 * automatically mapped to the correct attribute name for the schema version.
 *
 * @param work_time IfcWorkTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
IFCAPI_BINDING void sequence_edit_work_time(
    express::Base* work_time,
    ifcopenshell_pset_props_t* attributes);

/**
 * Remove the TimeLag from an IfcRelSequence.
 *
 * If the IfcLagTime is only referenced by this sequence, it is deleted.
 * Otherwise the TimeLag attribute is cleared.
 *
 * @param file File containing the sequence.
 * @param rel_sequence IfcRelSequence to remove the lag from.
 */
IFCAPI_BINDING void sequence_unassign_lag_time(
    ifcopenshell::file* file,
    express::Base* rel_sequence);

/**
 * Remove an IfcRelSequence between two tasks.
 *
 * Removes the sequence relationship where relating_process is the
 * predecessor and related_process is the successor, then cascades
 * schedule changes from the successor.
 *
 * @param file File containing the tasks.
 * @param relating_process Predecessor IfcTask.
 * @param related_process Successor IfcTask.
 */
IFCAPI_BINDING void sequence_unassign_sequence(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_process);

/**
 * Remove a process assignment via IfcRelAssignsToProcess.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * aggregate.
 *
 * @param file File containing the relationship.
 * @param relating_process IfcTask to unassign from.
 * @param related_object Object to unassign.
 * @param options Ownership options.
 * @return The modified relationship, or a null handle if removed.
 */
IFCAPI_BINDING express::Base sequence_unassign_process(
    ifcopenshell::file* file,
    express::Base* relating_process,
    express::Base* related_object,
    const SequenceRemoveOptions& options);

/**
 * Remove a product assignment via IfcRelAssignsToProduct.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * aggregate.
 *
 * @param file File containing the relationship.
 * @param relating_product IfcProduct to unassign from.
 * @param related_object Object to unassign.
 * @param options Ownership options.
 * @return The modified relationship, or a null handle if removed.
 */
IFCAPI_BINDING express::Base sequence_unassign_product(
    ifcopenshell::file* file,
    express::Base* relating_product,
    express::Base* related_object,
    const SequenceRemoveOptions& options);

/**
 * Remove an IfcRecurrencePattern and its time periods from the file.
 *
 * Deletes all IfcTimePeriod entities in the pattern's TimePeriods aggregate,
 * then deletes the pattern itself.
 *
 * @param file File containing the recurrence pattern.
 * @param recurrence_pattern IfcRecurrencePattern to remove.
 */
IFCAPI_BINDING void sequence_unassign_recurrence_pattern(
    ifcopenshell::file* file,
    express::Base* recurrence_pattern);

/**
 * Remove an IfcTimePeriod from the file.
 *
 * @param file File containing the time period.
 * @param time_period IfcTimePeriod to remove.
 */
IFCAPI_BINDING void sequence_remove_time_period(
    ifcopenshell::file* file,
    express::Base* time_period);

/**
 * Remove an IfcWorkTime and its associated recurrence pattern from the file.
 *
 * If the work time has a RecurrencePattern, all its TimePeriods and the
 * pattern itself are deleted first.
 *
 * @param file File containing the work time.
 * @param work_time IfcWorkTime to remove.
 */
IFCAPI_BINDING void sequence_remove_work_time(
    ifcopenshell::file* file,
    express::Base* work_time);

/**
 * Remove an IfcTask and its subtasks from the file.
 *
 * Recursively removes nested child tasks, their IfcTaskTime entities,
 * sequence relationships, control assignments, process assignments,
 * product assignments, property sets, and project declarations.
 *
 * @param file File containing the task.
 * @param task IfcTask to remove.
 * @param options Ownership options for relationship cleanup.
 */
IFCAPI_BINDING void sequence_remove_task(
    ifcopenshell::file* file,
    express::Base* task,
    const SequenceRemoveOptions& options);

/**
 * Remove an IfcWorkCalendar from the file.
 *
 * Unassigns the calendar from all controlled tasks, removes all working
 * and exception times, and removes the project declaration.
 *
 * @param file File containing the calendar.
 * @param work_calendar IfcWorkCalendar to remove.
 * @param options Ownership options for relationship cleanup.
 */
IFCAPI_BINDING void sequence_remove_work_calendar(
    ifcopenshell::file* file,
    express::Base* work_calendar,
    const SequenceRemoveOptions& options);

/**
 * Remove an IfcWorkPlan from the file.
 *
 * Unassigns the plan from its project declaration and removes the aggregate
 * relationships to child work schedules (without removing the schedules).
 *
 * @param file File containing the work plan.
 * @param work_plan IfcWorkPlan to remove.
 * @param options Ownership options for relationship cleanup.
 */
IFCAPI_BINDING void sequence_remove_work_plan(
    ifcopenshell::file* file,
    express::Base* work_plan,
    const SequenceRemoveOptions& options);

/**
 * Remove an IfcWorkSchedule and its controlled tasks from the file.
 *
 * Recursively removes nested work schedules, all controlled IfcTask
 * entities (with their subtasks and relationships), aggregate and
 * declaration relationships, and the schedule itself.
 *
 * @param file File containing the work schedule.
 * @param work_schedule IfcWorkSchedule to remove.
 * @param options Ownership options for relationship cleanup.
 */
IFCAPI_BINDING void sequence_remove_work_schedule(
    ifcopenshell::file* file,
    express::Base* work_schedule,
    const SequenceRemoveOptions& options);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_SEQUENCE_H */
