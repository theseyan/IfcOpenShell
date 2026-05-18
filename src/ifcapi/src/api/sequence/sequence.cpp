// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/sequence.h"
#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/control.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/project.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/relationship.h"
#include "ifcapi/detail/sequence_time.h"
#include "../pset/attribute_props.hpp"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

IfcUtil::IfcBaseClass* first_context(IfcParse::IfcFile* file) {
    auto* context = ifcapi::detail::first_instance_by_type(file, "IfcContext");
    if (!context) {
        throw std::runtime_error("No IfcContext found");
    }
    return context;
}

std::vector<const IfcUtil::IfcBaseClass*> one_const(IfcUtil::IfcBaseClass* item) {
    return item ? std::vector<const IfcUtil::IfcBaseClass*>{item} : std::vector<const IfcUtil::IfcBaseClass*>{};
}

IfcUtil::IfcBaseClass* create_root_entity(
    IfcParse::IfcFile* file,
    const char* ifc_class,
    const char* predefined_type,
    const char* name,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* history = ifcapi::detail::ensure_owner_history(file, owner_history, user, application);
    auto* entity = ifcapi::bindings::root_create_entity(file, ifc_class, predefined_type, name, history);
    if (!entity) {
        throw std::runtime_error(std::string("Failed to create ") + ifc_class);
    }
    return entity;
}

IfcUtil::IfcBaseClass* create_ifc_date_time(IfcParse::IfcFile* file, const std::string& value) {
    ifcapi::detail::sequence::DateTime parsed;
    if (!ifcapi::detail::sequence::parse_date_time(value, parsed)) {
        throw std::runtime_error("Invalid IfcDateTime value");
    }
    auto* calendar_date = create_entity(file, "IfcCalendarDate");
    ifcapi::detail::write_int_attr(calendar_date, "DayComponent", parsed.day);
    ifcapi::detail::write_int_attr(calendar_date, "MonthComponent", parsed.month);
    ifcapi::detail::write_int_attr(calendar_date, "YearComponent", parsed.year);

    auto* local_time = create_entity(file, "IfcLocalTime");
    ifcapi::detail::write_int_attr(local_time, "HourComponent", parsed.hour);
    ifcapi::detail::write_int_attr(local_time, "MinuteComponent", parsed.minute);
    int second_idx = ifcapi::detail::attr_index_of(local_time, "SecondComponent");
    if (second_idx >= 0) {
        local_time->set_attribute_value(static_cast<size_t>(second_idx), static_cast<double>(parsed.second));
    }

    auto* date_time = create_entity(file, "IfcDateAndTime");
    ifcapi::detail::write_ref_attr(date_time, "DateComponent", calendar_date);
    ifcapi::detail::write_ref_attr(date_time, "TimeComponent", local_time);
    return date_time;
}

std::string current_ifc_datetime() {
    std::time_t now = std::time(nullptr);
    std::tm local_time{};
#ifdef _WIN32
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d",
        local_time.tm_year + 1900,
        local_time.tm_mon + 1,
        local_time.tm_mday,
        local_time.tm_hour,
        local_time.tm_min,
        local_time.tm_sec);
    return buffer;
}

ifcapi::bindings::SequenceDateTimeResult make_date_time_result(
    IfcParse::IfcFile* file,
    const std::string& value)
{
    if (is_ifc2x3(file)) {
        return ifcapi::bindings::SequenceDateTimeResult{create_ifc_date_time(file, value), std::string(), true};
    }
    return ifcapi::bindings::SequenceDateTimeResult{nullptr, value, false};
}

void write_date_time_attr(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* entity,
    const char* attribute,
    const std::string& value)
{
    if (is_ifc2x3(file)) {
        ifcapi::detail::write_ref_attr(entity, attribute, create_ifc_date_time(file, value));
    } else {
        ifcapi::detail::write_string_attr(entity, attribute, value);
    }
}

IfcUtil::IfcBaseClass* find_assignment(
    IfcUtil::IfcBaseClass* related_object,
    const char* ifc_class,
    const char* relating_attr,
    IfcUtil::IfcBaseClass* relating_object)
{
    for (auto* assignment : ifcapi::detail::read_inverse_aggregate(related_object, "HasAssignments")) {
        if (assignment && assignment->declaration().is(ifc_class) &&
            ifcapi::detail::read_ref_attr(assignment, relating_attr) == relating_object) {
            return assignment;
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* find_sequence(
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_process)
{
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(related_process, "IsSuccessorFrom")) {
        if (rel && ifcapi::detail::read_ref_attr(rel, "RelatingProcess") == relating_process) {
            return rel;
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* assign_object_relationship(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_object,
    IfcUtil::IfcBaseClass* related_object,
    const char* relationship_class,
    const char* relating_attr,
    const char* relating_inverse,
    bool return_existing,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (auto* existing = find_assignment(related_object, relationship_class, relating_attr, relating_object)) {
        return return_existing ? existing : nullptr;
    }

    auto relationships = ifcapi::detail::read_inverse_aggregate(relating_object, relating_inverse);
    auto* rel = relationships.empty() ? nullptr : relationships.front();
    if (rel) {
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        related.push_back(related_object);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
        return rel;
    }

    rel = create_entity(file, relationship_class);
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {related_object});
    ifcapi::detail::write_ref_attr(rel, relating_attr, relating_object);
    return rel;
}

void apply_work_time_props(IfcUtil::IfcBaseClass* work_time, ifcopenshell_pset_props_t* props) {
    if (!props) return;
    for (const auto& entry : props->entries) {
        if (entry.key == "Start" || entry.key == "StartDate" || entry.key == "Finish" || entry.key == "FinishDate") {
            auto mapped = entry;
            const bool is_start = entry.key == "Start" || entry.key == "StartDate";
            const char* short_name = is_start ? "Start" : "Finish";
            const char* date_name = is_start ? "StartDate" : "FinishDate";
            mapped.key = ifcapi::detail::attr_index_of(work_time, short_name) >= 0 ? short_name : date_name;
            ifcapi::detail::apply_attribute_prop(work_time, mapped);
        } else {
            ifcapi::detail::apply_attribute_prop(work_time, entry);
        }
    }
}

IfcUtil::IfcBaseClass* unassign_object_relationship(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_object,
    IfcUtil::IfcBaseClass* related_object,
    const char* relationship_class,
    const char* relating_attr,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = find_assignment(related_object, relationship_class, relating_attr, relating_object);
    if (!rel) {
        return nullptr;
    }
    auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
    if (related.size() == 1) {
        ifcapi::detail::remove_with_history(file, rel);
        return nullptr;
    }
    related.erase(std::remove(related.begin(), related.end(), related_object), related.end());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
    ifcapi::detail::update_owner_history(file, rel, user, application);
    return rel;
}

std::vector<IfcUtil::IfcBaseClass*> sequence_assignment(IfcUtil::IfcBaseClass* task, const char* sequence) {
    const char* relationship_attr = nullptr;
    if (std::string(sequence) == "successor") {
        relationship_attr = "IsPredecessorTo";
    } else if (std::string(sequence) == "predecessor") {
        relationship_attr = "IsSuccessorFrom";
    } else {
        return {};
    }
    auto relationships = ifcapi::detail::read_inverse_aggregate(task, relationship_attr);
    if (!relationships.empty()) {
        return relationships;
    }
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "Nests")) {
        if (auto* parent = ifcapi::detail::read_ref_attr(rel, "RelatingObject")) {
            auto result = sequence_assignment(parent, sequence);
            if (!result.empty()) {
                return result;
            }
        }
    }
    return {};
}

bool read_task_time_date(IfcUtil::IfcBaseClass* task, const char* attribute, ifcapi::detail::sequence::DateTime& result) {
    auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime");
    return task_time && ifcapi::detail::sequence::read_date_attribute(task_time, attribute, result);
}

IfcUtil::IfcBaseClass* task_from_task_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* task_time) {
    if (!file || !task_time) {
        return nullptr;
    }
    auto inverses = file->getInverse(task_time->id(), nullptr, -1);
    if (!inverses) {
        return nullptr;
    }
    for (auto* inverse : *inverses) {
        if (inverse && inverse->declaration().is("IfcTask")) {
            return inverse;
        }
    }
    return nullptr;
}

ifcapi::detail::sequence::Duration read_task_duration(IfcUtil::IfcBaseClass* task) {
    ifcapi::detail::sequence::Duration result;
    auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime");
    auto duration = task_time ? ifcapi::detail::read_optional_string_attr(task_time, "ScheduleDuration") : ifcapi::detail::OptionalString{};
    if (duration.has_value) {
        ifcapi::detail::sequence::parse_duration(duration.value, result);
    }
    return result;
}

std::optional<double> read_double_like_attr(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = ifcapi::detail::attr_index_of(entity, attr);
    if (idx < 0) {
        return std::nullopt;
    }
    auto value = entity->get_attribute_value(static_cast<size_t>(idx));
    if (value.isNull()) {
        return std::nullopt;
    }
    if (value.type() == IfcUtil::Argument_DOUBLE || value.type() == IfcUtil::Argument_INT) {
        return static_cast<double>(value);
    }
    return std::nullopt;
}

std::optional<std::string> read_typed_string_value(IfcUtil::IfcBaseClass* value) {
    if (!value) {
        return std::nullopt;
    }
    auto text = ifcapi::detail::read_optional_string_attr(value, "wrappedValue");
    if (!text.has_value) {
        text = ifcapi::detail::read_optional_string_attr(value, "Value");
    }
    if (text.has_value) {
        return text.value;
    }
    auto* declaration = value->declaration().as_entity();
    if (!declaration || !declaration->all_attributes().empty()) {
        auto arg = value->get_attribute_value(0);
        if (!arg.isNull() && arg.type() == IfcUtil::Argument_STRING) {
            return static_cast<std::string>(arg);
        }
    }
    return std::nullopt;
}

std::optional<double> duration_to_seconds(
    const std::string& value,
    double seconds_per_workday,
    bool hourly_work)
{
    if (value.empty() || value[0] != 'P') {
        return std::nullopt;
    }
    bool in_time = false;
    double days = 0.0;
    double seconds = 0.0;
    size_t i = 1;
    while (i < value.size()) {
        if (value[i] == 'T') {
            in_time = true;
            ++i;
            continue;
        }
        char* end = nullptr;
        double amount = std::strtod(value.c_str() + i, &end);
        if (end == value.c_str() + i || end >= value.c_str() + value.size()) {
            return std::nullopt;
        }
        char designator = *end;
        if (designator == 'Y') {
            days += amount * 365.0;
        } else if (designator == 'M' && !in_time) {
            days += amount * 30.0;
        } else if (designator == 'W') {
            days += amount * 7.0;
        } else if (designator == 'D') {
            days += amount;
        } else if (designator == 'H') {
            seconds += amount * 60.0 * 60.0;
        } else if (designator == 'M' && in_time) {
            seconds += amount * 60.0;
        } else if (designator == 'S') {
            seconds += amount;
        } else {
            return std::nullopt;
        }
        i = static_cast<size_t>(end - value.c_str()) + 1;
    }
    if (hourly_work) {
        return days * 24.0 * 60.0 * 60.0 + seconds;
    }
    double partial_days = seconds / (24.0 * 60.0 * 60.0);
    return (days + partial_days) * seconds_per_workday;
}

IfcUtil::IfcBaseClass* task_work_schedule(IfcUtil::IfcBaseClass* task) {
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "HasAssignments")) {
        auto* control = ifcapi::detail::read_ref_attr(rel, "RelatingControl");
        if (rel && rel->declaration().is("IfcRelAssignsToControl") && control && control->declaration().is("IfcWorkSchedule")) {
            return control;
        }
    }
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "Nests")) {
        if (auto* parent = ifcapi::detail::read_ref_attr(rel, "RelatingObject")) {
            return task_work_schedule(parent);
        }
    }
    return nullptr;
}

std::optional<std::string> pset_property_string(
    IfcUtil::IfcBaseClass* product,
    const std::string& pset_name,
    const std::string& property_name)
{
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(product, "IsDefinedBy")) {
        if (!rel || !rel->declaration().is("IfcRelDefinesByProperties")) {
            continue;
        }
        auto* pset = ifcapi::detail::read_ref_attr(rel, "RelatingPropertyDefinition");
        if (!pset || ifcapi::detail::read_string_attr(pset, "Name") != pset_name) {
            continue;
        }
        for (auto* property : ifcapi::detail::read_ref_aggregate(pset, "HasProperties")) {
            if (!property || ifcapi::detail::read_string_attr(property, "Name") != property_name) {
                continue;
            }
            auto* nominal = ifcapi::detail::read_ref_attr(property, "NominalValue");
            if (!nominal) {
                continue;
            }
            auto value = read_typed_string_value(nominal);
            if (value.has_value()) {
                return *value;
            }
        }
    }
    return std::nullopt;
}

double seconds_per_workday(IfcUtil::IfcBaseClass* task) {
    constexpr double default_seconds = 8.0 * 60.0 * 60.0;
    auto* schedule = task_work_schedule(task);
    if (!schedule) {
        return default_seconds;
    }
    auto workday = pset_property_string(schedule, "Pset_WorkControlCommon", "WorkDayDuration");
    if (!workday.has_value()) {
        return default_seconds;
    }
    auto seconds = duration_to_seconds(*workday, default_seconds, true);
    return seconds.value_or(default_seconds);
}

double resource_duration_days(IfcUtil::IfcBaseClass* resource, double seconds_per_day) {
    auto* usage = ifcapi::detail::read_ref_attr(resource, "Usage");
    auto schedule_work = usage ? ifcapi::detail::read_optional_string_attr(usage, "ScheduleWork") : ifcapi::detail::OptionalString{};
    if (!schedule_work.has_value) {
        return 0.0;
    }
    double schedule_usage = read_double_like_attr(usage, "ScheduleUsage").value_or(1.0);
    if (schedule_usage == 0.0) {
        schedule_usage = 1.0;
    }
    bool hourly = schedule_work.value.find('T') != std::string::npos;
    auto schedule_seconds = duration_to_seconds(schedule_work.value, seconds_per_day, hourly);
    if (!schedule_seconds.has_value()) {
        return 0.0;
    }
    return std::ceil((*schedule_seconds / seconds_per_day) / schedule_usage);
}

double max_resource_usage_duration(IfcUtil::IfcBaseClass* task, double seconds_per_day) {
    double max_duration = 0.0;
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "OperatesOn")) {
        for (auto* related : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (related && related->declaration().is("IfcConstructionResource")) {
                double duration = resource_duration_days(related, seconds_per_day);
                if (duration > max_duration) {
                    max_duration = duration;
                }
            }
        }
    }
    return max_duration;
}

IfcUtil::IfcBaseClass* ensure_task_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* task) {
    if (auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime")) {
        return task_time;
    }
    auto* task_time = create_entity(file, "IfcTaskTime");
    ifcapi::detail::write_ref_attr(task, "TaskTime", task_time);
    return task_time;
}

bool prop_has_key(ifcopenshell_pset_props_t* props, const std::string& key) {
    if (!props) {
        return false;
    }
    return std::any_of(props->entries.begin(), props->entries.end(), [&](const auto& entry) {
        return entry.key == key;
    });
}

const ifcapi_pset::Entry* prop_entry(ifcopenshell_pset_props_t* props, const std::string& key) {
    if (!props) {
        return nullptr;
    }
    auto it = std::find_if(props->entries.begin(), props->entries.end(), [&](const auto& entry) {
        return entry.key == key;
    });
    return it == props->entries.end() ? nullptr : &*it;
}

bool prop_is_truthy(ifcopenshell_pset_props_t* props, const std::string& key) {
    const auto* entry = prop_entry(props, key);
    if (!entry || entry->kind == ifcapi_pset::Kind::NONE) {
        return false;
    }
    if (entry->kind == ifcapi_pset::Kind::STRING || entry->kind == ifcapi_pset::Kind::TYPED_STRING) {
        return !entry->s_val.empty();
    }
    if (entry->kind == ifcapi_pset::Kind::BOOL || entry->kind == ifcapi_pset::Kind::TYPED_BOOL) {
        return entry->b_val;
    }
    if (entry->kind == ifcapi_pset::Kind::INT || entry->kind == ifcapi_pset::Kind::TYPED_INT) {
        return entry->i_val != 0;
    }
    if (entry->kind == ifcapi_pset::Kind::DOUBLE || entry->kind == ifcapi_pset::Kind::TYPED_DOUBLE) {
        return entry->d_val != 0.0;
    }
    return true;
}

std::optional<ifcapi::detail::sequence::DateTime> entry_date(const ifcapi_pset::Entry* entry) {
    if (!entry || entry->kind == ifcapi_pset::Kind::NONE) {
        return std::nullopt;
    }
    ifcapi::detail::sequence::DateTime result;
    if (entry->kind == ifcapi_pset::Kind::STRING || entry->kind == ifcapi_pset::Kind::TYPED_STRING) {
        if (ifcapi::detail::sequence::parse_date_time(entry->s_val, result)) {
            return result;
        }
    }
    return std::nullopt;
}

void erase_prop_key(ifcopenshell_pset_props_t* props, const std::string& key) {
    if (!props) {
        return;
    }
    props->entries.erase(std::remove_if(props->entries.begin(), props->entries.end(), [&](const auto& entry) {
        return entry.key == key;
    }), props->entries.end());
}

void normalize_task_time_dates(
    IfcUtil::IfcBaseClass* task_time,
    IfcUtil::IfcBaseClass* calendar,
    ifcopenshell_pset_props_t* props)
{
    auto duration_type_entry = prop_entry(props, "DurationType");
    std::string duration_type = duration_type_entry &&
            (duration_type_entry->kind == ifcapi_pset::Kind::STRING || duration_type_entry->kind == ifcapi_pset::Kind::TYPED_STRING)
        ? duration_type_entry->s_val
        : ifcapi::detail::read_string_attr(task_time, "DurationType");

    if (auto finish = entry_date(prop_entry(props, "ScheduleFinish"))) {
        auto soonest = ifcapi::detail::sequence::get_soonest_working_day(*finish, duration_type, calendar);
        for (auto& entry : props->entries) {
            if (entry.key == "ScheduleFinish") {
                entry.kind = ifcapi_pset::Kind::STRING;
                entry.s_val = ifcapi::detail::sequence::format_date_time(ifcapi::detail::sequence::with_time(soonest, 17));
            }
        }
    }
    if (auto start = entry_date(prop_entry(props, "ScheduleStart"))) {
        auto soonest = ifcapi::detail::sequence::get_soonest_working_day(*start, duration_type, calendar);
        for (auto& entry : props->entries) {
            if (entry.key == "ScheduleStart") {
                entry.kind = ifcapi_pset::Kind::STRING;
                entry.s_val = ifcapi::detail::sequence::format_date_time(ifcapi::detail::sequence::with_time(soonest, 9));
            }
        }
    }
}

void calculate_task_time_finish(IfcUtil::IfcBaseClass* task_time, IfcUtil::IfcBaseClass* calendar) {
    ifcapi::detail::sequence::DateTime start;
    if (!ifcapi::detail::sequence::read_date_attribute(task_time, "ScheduleStart", start)) {
        return;
    }
    auto duration_text = ifcapi::detail::read_optional_string_attr(task_time, "ScheduleDuration");
    if (!duration_text.has_value) {
        return;
    }
    ifcapi::detail::sequence::Duration duration;
    if (!ifcapi::detail::sequence::parse_duration(duration_text.value, duration)) {
        return;
    }
    auto duration_type = ifcapi::detail::read_string_attr(task_time, "DurationType");
    if (duration_type.empty()) {
        duration_type = "ELAPSEDTIME";
    }
    auto finish = ifcapi::detail::sequence::get_start_or_finish_date(start, duration, duration_type, calendar, "FINISH");
    ifcapi::detail::write_string_attr(task_time, "ScheduleFinish", ifcapi::detail::sequence::format_date_time(finish));
}

void calculate_task_time_duration(IfcUtil::IfcBaseClass* task_time, IfcUtil::IfcBaseClass* calendar) {
    ifcapi::detail::sequence::DateTime start;
    ifcapi::detail::sequence::DateTime finish;
    if (!ifcapi::detail::sequence::read_date_attribute(task_time, "ScheduleStart", start) ||
        !ifcapi::detail::sequence::read_date_attribute(task_time, "ScheduleFinish", finish)) {
        return;
    }
    auto duration_type = ifcapi::detail::read_string_attr(task_time, "DurationType");
    int current = ifcapi::detail::sequence::date_serial(start);
    const int finish_day = ifcapi::detail::sequence::date_serial(finish);
    int days = 1;
    while (current < finish_day) {
        if (duration_type == "ELAPSEDTIME" || !calendar ||
            ifcapi::detail::sequence::is_working_day(ifcapi::detail::sequence::civil_from_days(current), calendar)) {
            ++days;
        }
        ++current;
    }
    ifcapi::detail::sequence::Duration duration;
    duration.days = days;
    ifcapi::detail::write_string_attr(task_time, "ScheduleDuration", ifcapi::detail::sequence::format_duration(duration));
}

void remove_task_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

void remove_work_schedule_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

void remove_task_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* task_time) {
    if (!task_time) {
        return;
    }
    if (task_time->declaration().is("IfcTaskTimeRecurring")) {
        if (auto* recurrence = ifcapi::detail::read_ref_attr(task_time, "Recurrence")) {
            for (auto* time_period : ifcapi::detail::read_ref_aggregate(recurrence, "TimePeriods")) {
                file->removeEntity(time_period);
            }
            file->removeEntity(recurrence);
        }
    }
    file->removeEntity(task_time);
}

IfcUtil::IfcBaseClass* deep_copy_entity(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    std::unordered_map<unsigned, IfcUtil::IfcBaseClass*>& memo)
{
    if (!element) {
        return nullptr;
    }
    auto id = element->id();
    if (id) {
        auto it = memo.find(id);
        if (it != memo.end()) {
            return it->second;
        }
    }
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(element);
    if (!base) {
        return element;
    }
    const auto* declaration = base->declaration().as_entity();
    if (!declaration) {
        return element;
    }
    auto* result = file->create(declaration);
    if (id) {
        memo[id] = result;
    }
    auto attributes = declaration->all_attributes();
    for (size_t i = 0; i < attributes.size(); ++i) {
        auto value = element->get_attribute_value(i);
        if (value.isNull()) {
            continue;
        }
        if (attributes[i]->name() == "GlobalId") {
            result->set_attribute_value(i, ifcapi::guid_new());
            continue;
        }
        ifcapi::detail::copy_attribute_value(result, i, value);
        if (value.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
            result->set_attribute_value(i, deep_copy_entity(file, static_cast<IfcUtil::IfcBaseClass*>(value), memo));
        } else if (value.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            auto aggregate = static_cast<aggregate_of_instance::ptr>(value);
            auto copied = aggregate_of_instance::ptr(new aggregate_of_instance());
            if (aggregate) {
                for (auto& item : *aggregate) {
                    copied->push(deep_copy_entity(file, item, memo));
                }
            }
            result->set_attribute_value(i, copied);
        } else if (value.type() == IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
            auto aggregate = static_cast<aggregate_of_aggregate_of_instance::ptr>(value);
            auto copied = aggregate_of_aggregate_of_instance::ptr(new aggregate_of_aggregate_of_instance());
            if (aggregate) {
                for (const auto& row : *aggregate) {
                    std::vector<IfcUtil::IfcBaseClass*> copied_row;
                    copied_row.reserve(row.size());
                    for (auto* item : row) {
                        copied_row.push_back(deep_copy_entity(file, item, memo));
                    }
                    copied->push(copied_row);
                }
            }
            result->set_attribute_value(i, copied);
        }
    }
    return result;
}

IfcUtil::IfcBaseClass* deep_copy_entity(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* element) {
    std::unordered_map<unsigned, IfcUtil::IfcBaseClass*> memo;
    return deep_copy_entity(file, element, memo);
}

void create_object_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_object,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    IfcUtil::IfcBaseClass* referenced_by = nullptr;
    auto declares = ifcapi::detail::read_inverse_aggregate(relating_object, "Declares");
    if (!declares.empty()) {
        referenced_by = declares.front();
    }
    if (referenced_by) {
        auto related = ifcapi::detail::read_ref_aggregate(referenced_by, "RelatedObjects");
        related.push_back(related_object);
        ifcapi::detail::write_ref_aggregate(referenced_by, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, referenced_by, user, application);
        return;
    }
    referenced_by = create_entity(file, "IfcRelDefinesByObject");
    ifcapi::detail::write_string_attr(referenced_by, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        referenced_by, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(referenced_by, "RelatedObjects", {related_object});
    ifcapi::detail::write_ref_attr(referenced_by, "RelatingObject", relating_object);
}

class DuplicateTask {
public:
    DuplicateTask(
        IfcParse::IfcFile* file,
        IfcUtil::IfcBaseClass* owner_history,
        IfcUtil::IfcBaseClass* user,
        IfcUtil::IfcBaseClass* application)
        : file_(file), owner_history_(owner_history), user_(user), application_(application) {}

    ifcapi::bindings::SequenceDuplicateTaskResult execute(IfcUtil::IfcBaseClass* task) {
        duplicate_task(task);
        copy_sequence_relationships();
        return {current_, duplicate_};
    }

private:
    IfcParse::IfcFile* file_;
    IfcUtil::IfcBaseClass* owner_history_;
    IfcUtil::IfcBaseClass* user_;
    IfcUtil::IfcBaseClass* application_;
    std::vector<IfcUtil::IfcBaseClass*> current_;
    std::vector<IfcUtil::IfcBaseClass*> duplicate_;

    IfcUtil::IfcBaseClass* duplicate_task(IfcUtil::IfcBaseClass* task) {
        auto* new_task = deep_copy_entity(file_, task);
        current_.push_back(task);
        duplicate_.push_back(new_task);
        copy_indirect_attributes(task, new_task);
        return new_task;
    }

    int current_index(IfcUtil::IfcBaseClass* task) const {
        auto it = std::find(current_.begin(), current_.end(), task);
        return it == current_.end() ? -1 : static_cast<int>(std::distance(current_.begin(), it));
    }

    void copy_indirect_attributes(IfcUtil::IfcBaseClass* from_element, IfcUtil::IfcBaseClass* to_element) {
        auto inverses = file_->getInverse(from_element->id(), nullptr, -1);
        std::vector<IfcUtil::IfcBaseClass*> snapshot;
        if (inverses) {
            for (auto* inverse : *inverses) {
                if (inverse) {
                    snapshot.push_back(inverse);
                }
            }
        }
        for (auto* inverse : snapshot) {
            if (!ifcapi::detail::exists_in_file(file_, inverse)) {
                continue;
            }
            if (inverse->declaration().is("IfcRelDefinesByProperties")) {
                auto* rel = ifcapi::detail::shallow_copy(file_, inverse);
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {to_element});
                ifcapi::detail::write_ref_attr(
                    rel, "RelatingPropertyDefinition",
                    deep_copy_entity(file_, ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition")));
            } else if (inverse->declaration().is("IfcRelNests") &&
                ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == from_element) {
                std::vector<IfcUtil::IfcBaseClass*> new_tasks;
                for (auto* nested_task : ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects")) {
                    new_tasks.push_back(duplicate_task(nested_task));
                }
                if (!new_tasks.empty()) {
                    ifcapi::bindings::nest_assign_object(
                        file_, ifcapi::detail::to_const_refs(new_tasks), to_element, owner_history_, user_, application_);
                }
            } else if (inverse->declaration().is("IfcRelSequence") ||
                (inverse->declaration().is("IfcRelAssignsToControl") &&
                    ifcapi::detail::read_ref_attr(inverse, "RelatingControl") &&
                    ifcapi::detail::read_ref_attr(inverse, "RelatingControl")->declaration().is("IfcWorkSchedule")) ||
                inverse->declaration().is("IfcRelDefinesByObject")) {
                continue;
            } else {
                copy_or_extend_inverse(inverse, from_element, to_element);
            }
        }
    }

    void copy_or_extend_inverse(
        IfcUtil::IfcBaseClass* inverse,
        IfcUtil::IfcBaseClass* from_element,
        IfcUtil::IfcBaseClass* to_element)
    {
        auto* decl = inverse->declaration().as_entity();
        if (!decl) {
            return;
        }
        auto attributes = decl->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {
            auto value = inverse->get_attribute_value(i);
            if (value.isNull()) {
                continue;
            }
            if (value.type() == IfcUtil::Argument_ENTITY_INSTANCE &&
                static_cast<IfcUtil::IfcBaseClass*>(value) == from_element) {
                auto* copied = ifcapi::detail::shallow_copy(file_, inverse);
                copied->set_attribute_value(i, to_element);
            } else if (value.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto aggregate = static_cast<aggregate_of_instance::ptr>(value);
                if (!aggregate) {
                    continue;
                }
                bool contains = false;
                std::vector<IfcUtil::IfcBaseClass*> refs;
                for (auto& item : *aggregate) {
                    refs.push_back(item);
                    if (item == from_element) {
                        contains = true;
                    }
                }
                if (contains) {
                    refs.push_back(to_element);
                    ifcapi::detail::set_ref_aggregate(inverse, static_cast<int>(i), refs);
                }
            }
        }
    }

    void copy_sequence_relationships() {
        for (size_t i = 0; i < current_.size(); ++i) {
            auto* original_task = current_[i];
            auto* duplicated_task = duplicate_[i];
            auto inverses = file_->getInverse(original_task->id(), nullptr, -1);
            if (!inverses) {
                continue;
            }
            for (auto* inverse : *inverses) {
                if (!inverse || !inverse->declaration().is("IfcRelSequence")) {
                    continue;
                }
                auto* relating_process = ifcapi::detail::read_ref_attr(inverse, "RelatingProcess");
                auto* related_process = ifcapi::detail::read_ref_attr(inverse, "RelatedProcess");
                if (relating_process == original_task) {
                    relating_process = duplicated_task;
                }
                if (related_process == original_task) {
                    related_process = duplicated_task;
                }
                int related_index = current_index(ifcapi::detail::read_ref_attr(inverse, "RelatedProcess"));
                if (related_index >= 0) {
                    related_process = duplicate_[static_cast<size_t>(related_index)];
                }
                int relating_index = current_index(ifcapi::detail::read_ref_attr(inverse, "RelatingProcess"));
                if (relating_index >= 0) {
                    relating_process = duplicate_[static_cast<size_t>(relating_index)];
                }
                if (relating_process && related_process) {
                    auto* rel = ifcapi::bindings::sequence_assign_sequence(
                        file_,
                        relating_process,
                        related_process,
                        ifcapi::detail::read_string_attr(inverse, "SequenceType"),
                        owner_history_,
                        user_,
                        application_);
                    if (auto* lag = ifcapi::detail::read_ref_attr(inverse, "TimeLag")) {
                        ifcapi::detail::write_ref_attr(rel, "TimeLag", deep_copy_entity(file_, lag));
                    }
                }
            }
        }
    }
};

void remove_from_related_objects_or_relation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* rel,
    IfcUtil::IfcBaseClass* object)
{
    auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
    if (related.size() <= 1) {
        ifcapi::detail::remove_with_history(file, rel);
        return;
    }
    related.erase(std::remove(related.begin(), related.end(), object), related.end());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
}

void remove_rel_defines_by_object_or_unassign(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* rel,
    IfcUtil::IfcBaseClass* object)
{
    if (ifcapi::detail::read_ref_attr(rel, "RelatingObject") == object ||
        ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects").size() <= 1) {
        ifcapi::detail::remove_with_history(file, rel);
        return;
    }
    auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
    related.erase(std::remove(related.begin(), related.end(), object), related.end());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
}

void remove_task_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!ifcapi::detail::exists_in_file(file, task)) {
        return;
    }
    ifcapi::bindings::project_unassign_declaration(file, one_const(task), first_context(file), user, application);

    remove_task_time(file, ifcapi::detail::read_ref_attr(task, "TaskTime"));

    auto nested_by = ifcapi::detail::read_inverse_aggregate(task, "IsNestedBy");
    if (!nested_by.empty()) {
        auto subtasks = ifcapi::detail::read_ref_aggregate(nested_by.front(), "RelatedObjects");
        if (!subtasks.empty()) {
            ifcapi::bindings::nest_unassign_object(file, ifcapi::detail::to_const_refs(subtasks), user, application);
            for (auto* subtask : subtasks) {
                remove_task_internal(file, subtask, user, application);
            }
        }
    }
    if (!ifcapi::detail::read_inverse_aggregate(task, "Nests").empty()) {
        ifcapi::bindings::nest_unassign_object(file, one_const(task), user, application);
    }

    auto inverses = file->getInverse(task->id(), nullptr, -1);
    std::vector<IfcUtil::IfcBaseClass*> snapshot;
    if (inverses) {
        for (auto* inverse : *inverses) {
            if (inverse) {
                snapshot.push_back(inverse);
            }
        }
    }
    for (auto* inverse : snapshot) {
        if (!ifcapi::detail::exists_in_file(file, inverse)) {
            continue;
        }
        if (inverse->declaration().is("IfcRelSequence")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (inverse->declaration().is("IfcRelAssignsToControl")) {
            auto* control = ifcapi::detail::read_ref_attr(inverse, "RelatingControl");
            if (control == task || ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() <= 1) {
                ifcapi::detail::remove_with_history(file, inverse);
            } else {
                remove_from_related_objects_or_relation(file, inverse, task);
            }
        } else if (inverse->declaration().is("IfcRelDefinesByProperties")) {
            ifcapi::bindings::pset_remove_pset(file, task, ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition"));
        } else if (inverse->declaration().is("IfcRelAssignsToProcess")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingProcess") == task ||
                ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() <= 1) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (inverse->declaration().is("IfcRelAssignsToProduct")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingProduct") == task) {
                ifcapi::detail::remove_with_history(file, inverse);
            } else {
                remove_from_related_objects_or_relation(file, inverse, task);
            }
        } else if (inverse->declaration().is("IfcRelAssignsToObject")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == task) {
                ifcapi::detail::remove_with_history(file, inverse);
            } else {
                remove_from_related_objects_or_relation(file, inverse, task);
            }
        }
    }

    ifcapi::detail::remove_with_history(file, task);
}

void remove_work_calendar_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_calendar,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!ifcapi::detail::exists_in_file(file, work_calendar)) {
        return;
    }
    ifcapi::bindings::project_unassign_declaration(file, one_const(work_calendar), first_context(file), user, application);
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_calendar, "Controls")) {
        if (!ifcapi::detail::exists_in_file(file, rel)) {
            continue;
        }
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        ifcapi::bindings::control_unassign_control(
            file, work_calendar, ifcapi::detail::to_const_refs(related), user, application);
    }
    auto working_times = ifcapi::detail::read_ref_aggregate(work_calendar, "WorkingTimes");
    auto exception_times = ifcapi::detail::read_ref_aggregate(work_calendar, "ExceptionTimes");
    for (auto* work_time : working_times) {
        if (ifcapi::detail::exists_in_file(file, work_time)) {
            ifcapi::bindings::sequence_remove_work_time(file, work_time);
        }
    }
    for (auto* work_time : exception_times) {
        if (ifcapi::detail::exists_in_file(file, work_time)) {
            ifcapi::bindings::sequence_remove_work_time(file, work_time);
        }
    }
    ifcapi::detail::remove_with_history(file, work_calendar);
}

void remove_work_plan_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_plan,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!ifcapi::detail::exists_in_file(file, work_plan)) {
        return;
    }
    ifcapi::bindings::project_unassign_declaration(file, one_const(work_plan), first_context(file), user, application);
    std::vector<IfcUtil::IfcBaseClass*> related_objects;
    for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_plan, "IsDecomposedBy")) {
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        related_objects.insert(related_objects.end(), related.begin(), related.end());
    }
    if (!related_objects.empty()) {
        ifcapi::bindings::aggregate_unassign_object(file, ifcapi::detail::to_const_refs(related_objects), user, application);
    }
    ifcapi::detail::remove_with_history(file, work_plan);
}

void remove_work_schedule_internal(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!ifcapi::detail::exists_in_file(file, work_schedule)) {
        return;
    }
    ifcapi::bindings::project_unassign_declaration(file, one_const(work_schedule), first_context(file), user, application);

    for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_schedule, "Declares")) {
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        for (auto* nested_schedule : related) {
            if (nested_schedule && nested_schedule->declaration().is("IfcWorkSchedule")) {
                remove_work_schedule_internal(file, nested_schedule, user, application);
            }
        }
    }

    if (!ifcapi::detail::read_inverse_aggregate(work_schedule, "Decomposes").empty()) {
        ifcapi::bindings::aggregate_unassign_object(file, one_const(work_schedule), user, application);
    }

    auto inverses = file->getInverse(work_schedule->id(), nullptr, -1);
    std::vector<IfcUtil::IfcBaseClass*> snapshot;
    if (inverses) {
        for (auto* inverse : *inverses) {
            if (inverse) {
                snapshot.push_back(inverse);
            }
        }
    }
    for (auto* inverse : snapshot) {
        if (!ifcapi::detail::exists_in_file(file, inverse)) {
            continue;
        }
        if (inverse->declaration().is("IfcRelDefinesByObject")) {
            remove_rel_defines_by_object_or_unassign(file, inverse, work_schedule);
        } else if (inverse->declaration().is("IfcRelAssignsToControl")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            for (auto* related_object : related) {
                if (related_object && related_object->declaration().is("IfcTask")) {
                    remove_task_internal(file, related_object, user, application);
                }
            }
        }
    }

    ifcapi::detail::remove_with_history(file, work_schedule);
}

int lag_time_days(IfcUtil::IfcBaseClass* lag_time) {
    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
    if (!value) {
        return 0;
    }
    auto text = ifcapi::detail::read_optional_string_attr(value, "wrappedValue");
    if (!text.has_value) {
        text = ifcapi::detail::read_optional_string_attr(value, "Value");
    }
    if (!text.has_value) {
        int idx = ifcapi::detail::attr_index_of(value, "wrappedValue");
        if (idx < 0) idx = 0;
        auto* declaration = value->declaration().as_entity();
        if (!declaration || static_cast<size_t>(idx) < declaration->all_attributes().size()) {
            auto arg = value->get_attribute_value(static_cast<size_t>(idx));
            if (!arg.isNull() && arg.type() == IfcUtil::Argument_STRING) {
                text.has_value = true;
                text.value = static_cast<std::string>(arg);
            }
        }
    }
    if (!text.has_value) {
        return 0;
    }
    ifcapi::detail::sequence::Duration duration;
    ifcapi::detail::sequence::parse_duration(text.value, duration);
    return duration.days;
}

double lag_time_ratio(IfcUtil::IfcBaseClass* lag_time) {
    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
    if (!value) {
        return 0.0;
    }
    int idx = ifcapi::detail::attr_index_of(value, "wrappedValue");
    if (idx < 0) idx = 0;
    auto* declaration = value->declaration().as_entity();
    if (!declaration || static_cast<size_t>(idx) < declaration->all_attributes().size()) {
        auto arg = value->get_attribute_value(static_cast<size_t>(idx));
        if (!arg.isNull()) {
            return static_cast<double>(arg);
        }
    }
    return 0.0;
}

struct ScheduleNode {
    int id = 0;
    IfcUtil::IfcBaseClass* task = nullptr;
    int duration = 0;
    std::string duration_type = "ELAPSEDTIME";
    IfcUtil::IfcBaseClass* calendar = nullptr;
    std::optional<ifcapi::detail::sequence::DateTime> early_start;
    std::optional<ifcapi::detail::sequence::DateTime> early_finish;
    std::optional<ifcapi::detail::sequence::DateTime> late_start;
    std::optional<ifcapi::detail::sequence::DateTime> late_finish;
    ifcapi::detail::sequence::Duration total_float;
    std::optional<ifcapi::detail::sequence::Duration> free_float;
};

struct ScheduleEdge {
    int from = 0;
    int to = 0;
    int lag_time = 0;
    std::string type = "FS";
};

bool date_greater(const ifcapi::detail::sequence::DateTime& a, const ifcapi::detail::sequence::DateTime& b) {
    return ifcapi::detail::sequence::less(b, a);
}

ifcapi::detail::sequence::DateTime min_date(const std::vector<ifcapi::detail::sequence::DateTime>& values) {
    return *std::min_element(values.begin(), values.end(), ifcapi::detail::sequence::less);
}

ifcapi::detail::sequence::DateTime max_date(const std::vector<ifcapi::detail::sequence::DateTime>& values) {
    return *std::max_element(values.begin(), values.end(), ifcapi::detail::sequence::less);
}

class RecalculateSchedule {
public:
    explicit RecalculateSchedule(IfcParse::IfcFile*) {}

    void execute(IfcUtil::IfcBaseClass* work_schedule) {
        work_schedule_ = work_schedule;
        build_network_graph();
        if (start_dates_.empty()) {
            return;
        }
        bool is_cyclic = false;
        int attempts = 0;
        std::set<int> pending;
        for (const auto& item : nodes_) pending.insert(item.first);
        int max_worst_case_attempts = static_cast<int>(pending.size() * pending.size());
        while (!pending.empty()) {
            ++attempts;
            std::set<int> remaining;
            for (int node : pending) {
                if (!forward_pass(node)) {
                    remaining.insert(node);
                }
            }
            pending = remaining;
            int max_remaining_attempts = static_cast<int>(pending.size() * pending.size());
            if (max_remaining_attempts < max_worst_case_attempts) {
                max_worst_case_attempts = max_remaining_attempts;
                attempts = 0;
            }
            if (attempts > max_worst_case_attempts) {
                is_cyclic = true;
                break;
            }
        }
        if (is_cyclic) {
            throw std::runtime_error("Task graph is cyclic and so critical path method cannot be performed.");
        }
        pending.clear();
        for (const auto& item : nodes_) pending.insert(item.first);
        while (!pending.empty()) {
            std::set<int> remaining;
            for (int node : pending) {
                if (!backward_pass(node)) {
                    remaining.insert(node);
                }
            }
            pending = remaining;
        }
        update_task_times();
    }

private:
    static constexpr int start_id = -1;
    static constexpr int finish_id = -2;
    IfcUtil::IfcBaseClass* work_schedule_ = nullptr;
    std::map<int, ScheduleNode> nodes_;
    std::vector<ScheduleEdge> edges_;
    std::vector<ifcapi::detail::sequence::DateTime> start_dates_;

    std::vector<int> predecessors(int id) const {
        std::vector<int> result;
        for (const auto& edge : edges_) if (edge.to == id) result.push_back(edge.from);
        return result;
    }

    std::vector<int> successors(int id) const {
        std::vector<int> result;
        for (const auto& edge : edges_) if (edge.from == id) result.push_back(edge.to);
        return result;
    }

    const ScheduleEdge* edge_between(int from, int to) const {
        for (const auto& edge : edges_) if (edge.from == from && edge.to == to) return &edge;
        return nullptr;
    }

    ifcapi::detail::sequence::DateTime offset_date(
        const ifcapi::detail::sequence::DateTime& date,
        int days,
        const ScheduleNode& node)
    {
        ifcapi::detail::sequence::Duration duration;
        duration.days = days;
        return ifcapi::detail::sequence::offset_date(date, duration, node.duration_type, node.calendar);
    }

    void build_network_graph() {
        ScheduleNode start_node;
        start_node.id = start_id;
        nodes_[start_id] = start_node;
        ScheduleNode finish_node;
        finish_node.id = finish_id;
        nodes_[finish_id] = finish_node;
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_schedule_, "Controls")) {
            for (auto* related : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                if (related && related->declaration().is("IfcTask")) {
                    add_node(related);
                }
            }
        }
    }

    void add_node(IfcUtil::IfcBaseClass* task) {
        auto nested_by = ifcapi::detail::read_inverse_aggregate(task, "IsNestedBy");
        if (!nested_by.empty()) {
            for (auto* rel : nested_by) {
                for (auto* child : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                    add_node(child);
                }
            }
            return;
        }
        int id = task->id();
        ScheduleNode node;
        node.id = id;
        node.task = task;
        auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime");
        if (task_time) {
            auto duration_text = ifcapi::detail::read_optional_string_attr(task_time, "ScheduleDuration");
            if (duration_text.has_value) {
                ifcapi::detail::sequence::Duration duration;
                ifcapi::detail::sequence::parse_duration(duration_text.value, duration);
                node.duration = duration.days;
            }
            node.duration_type = ifcapi::detail::read_string_attr(task_time, "DurationType");
            if (node.duration_type.empty()) node.duration_type = "ELAPSEDTIME";
        }
        node.calendar = ifcapi::detail::sequence::derive_calendar(task);
        nodes_[id] = node;

        std::vector<std::string> predecessor_types;
        for (auto* rel : sequence_assignment(task, "predecessor")) {
            auto* predecessor = ifcapi::detail::read_ref_attr(rel, "RelatingProcess");
            if (!predecessor) continue;
            auto type = ifcapi::detail::read_string_attr(rel, "SequenceType");
            predecessor_types.push_back(type);
            edges_.push_back(ScheduleEdge{static_cast<int>(predecessor->id()), id,
                ifcapi::detail::read_ref_attr(rel, "TimeLag") ? lag_time_days(ifcapi::detail::read_ref_attr(rel, "TimeLag")) : 0,
                sequence_type(type)});
        }
        std::vector<std::string> successor_types;
        for (auto* rel : sequence_assignment(task, "successor")) {
            successor_types.push_back(ifcapi::detail::read_string_attr(rel, "SequenceType"));
        }
        if (predecessor_types.empty()) {
            edges_.push_back(ScheduleEdge{start_id, id, 0, "FS"});
            ifcapi::detail::sequence::DateTime start;
            if (task_time && ifcapi::detail::sequence::read_date_attribute(task_time, "ScheduleStart", start)) {
                start_dates_.push_back(start);
                nodes_[id].early_start = start;
            }
        }
        if (successor_types.empty()) {
            edges_.push_back(ScheduleEdge{id, finish_id, 0, "FF"});
        }
    }

    std::string sequence_type(const std::string& type) const {
        if (type == "START_START") return "SS";
        if (type == "START_FINISH") return "SF";
        if (type == "FINISH_FINISH") return "FF";
        return "FS";
    }

    bool forward_pass(int id) {
        auto& data = nodes_[id];
        if (id == start_id) {
            data.early_start = min_date(start_dates_);
            return true;
        }
        std::vector<ifcapi::detail::sequence::DateTime> finishes;
        std::vector<ifcapi::detail::sequence::DateTime> starts;
        if (data.early_start.has_value()) {
            ifcapi::detail::sequence::Duration duration;
            duration.days = data.duration;
            data.early_finish = ifcapi::detail::sequence::get_start_or_finish_date(
                *data.early_start, duration, data.duration_type, data.calendar, "FINISH");
            return true;
        }
        for (int predecessor : predecessors(id)) {
            const auto& pred = nodes_[predecessor];
            const auto* edge = edge_between(predecessor, id);
            if (!edge) continue;
            if (edge->type == "FS") {
                if (!pred.early_finish) return false;
                int days = pred.duration == 0 ? 0 : 1;
                if (edge->lag_time) days += edge->lag_time;
                if (days) {
                    starts.push_back(ifcapi::detail::sequence::with_time(offset_date(*pred.early_finish, days, data), 9));
                    starts.push_back(ifcapi::detail::sequence::with_time(offset_date(*pred.early_finish, days, pred), 9));
                } else starts.push_back(*pred.early_finish);
            } else if (edge->type == "SS") {
                if (!pred.early_start) return false;
                if (edge->lag_time) {
                    starts.push_back(offset_date(*pred.early_start, edge->lag_time, data));
                    starts.push_back(offset_date(*pred.early_start, edge->lag_time, pred));
                } else starts.push_back(*pred.early_start);
            } else if (edge->type == "FF") {
                if (!pred.early_finish) return false;
                if (edge->lag_time) {
                    finishes.push_back(offset_date(*pred.early_finish, edge->lag_time, data));
                    finishes.push_back(offset_date(*pred.early_finish, edge->lag_time, pred));
                } else finishes.push_back(*pred.early_finish);
            } else if (edge->type == "SF") {
                if (!pred.early_start) return false;
                int days = -1;
                if (edge->lag_time) days += edge->lag_time;
                if (days || edge->lag_time) {
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(*pred.early_start, days, data), 17));
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(*pred.early_start, days, pred), 17));
                } else finishes.push_back(*pred.early_start);
            }
        }
        if (!starts.empty() && !finishes.empty()) {
            data.early_start = max_date(starts);
            data.early_finish = max_date(finishes);
            ifcapi::detail::sequence::Duration duration;
            duration.days = data.duration;
            auto potential_finish = ifcapi::detail::sequence::get_start_or_finish_date(
                *data.early_start, duration, data.duration_type, data.calendar, "FINISH");
            if (date_greater(potential_finish, *data.early_finish)) data.early_finish = potential_finish;
            else data.early_start = ifcapi::detail::sequence::get_start_or_finish_date(
                *data.early_finish, duration, data.duration_type, data.calendar, "START");
        } else if (!finishes.empty()) data.early_finish = max_date(finishes);
        else if (!starts.empty()) data.early_start = max_date(starts);
        if (!data.early_finish) {
            if (!data.early_start) return false;
            ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
            data.early_finish = ifcapi::detail::sequence::get_start_or_finish_date(
                *data.early_start, duration, data.duration_type, data.calendar, "FINISH");
        } else if (!data.early_start) {
            ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
            data.early_start = ifcapi::detail::sequence::get_start_or_finish_date(
                *data.early_finish, duration, data.duration_type, data.calendar, "START");
        }
        return true;
    }

    bool backward_pass(int id) {
        auto& data = nodes_[id];
        std::vector<ifcapi::detail::sequence::DateTime> finishes;
        std::vector<ifcapi::detail::sequence::DateTime> starts;
        std::vector<ifcapi::detail::sequence::Duration> free_floats;
        if (id == finish_id) {
            data.late_finish = data.early_finish;
        } else {
            for (int successor : successors(id)) {
                const auto& succ = nodes_[successor];
                const auto* edge = edge_between(id, successor);
                if (!edge) continue;
                if (edge->type == "FS") {
                    if (!succ.late_start) return false;
                    int days = 1;
                    if (edge->lag_time) days += edge->lag_time;
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(*succ.late_start, -days, data), 17));
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(*succ.late_start, -days, succ), 17));
                    free_floats.push_back(calculate_free_float(
                        ifcapi::detail::sequence::with_time(ifcapi::detail::sequence::add_days(*data.early_finish, 1), 0),
                        ifcapi::detail::sequence::with_time(*succ.early_start, 0),
                        edge->lag_time, data, succ));
                } else if (edge->type == "SS") {
                    if (!succ.late_start) return false;
                    if (edge->lag_time) {
                        starts.push_back(offset_date(*succ.late_start, -edge->lag_time, data));
                        starts.push_back(offset_date(*succ.late_start, -edge->lag_time, succ));
                    } else starts.push_back(*succ.late_start);
                    free_floats.push_back(calculate_free_float(*data.early_start, *succ.early_start, edge->lag_time, data, succ));
                } else if (edge->type == "FF") {
                    if (!succ.late_finish) return false;
                    if (edge->lag_time) {
                        finishes.push_back(offset_date(*succ.late_finish, -edge->lag_time, data));
                        finishes.push_back(offset_date(*succ.late_finish, -edge->lag_time, succ));
                    } else finishes.push_back(*succ.late_finish);
                    free_floats.push_back(calculate_free_float(*data.early_finish, *succ.early_finish, edge->lag_time, data, succ));
                } else if (edge->type == "SF") {
                    if (!succ.late_finish) return false;
                    int days = succ.duration == 0 ? 0 : -1;
                    if (edge->lag_time) days += edge->lag_time;
                    if (days) {
                        starts.push_back(ifcapi::detail::sequence::with_time(offset_date(*succ.late_finish, -days, data), 9));
                        starts.push_back(ifcapi::detail::sequence::with_time(offset_date(*succ.late_finish, -days, succ), 9));
                    } else starts.push_back(*succ.late_finish);
                    free_floats.push_back(calculate_free_float(*data.early_start, *succ.early_finish, edge->lag_time, data, succ));
                }
            }
            if (!starts.empty() && !finishes.empty()) {
                data.late_start = min_date(starts);
                data.late_finish = min_date(finishes);
                if (ifcapi::detail::sequence::less(offset_date(*data.late_start, data.duration, data), *data.late_finish)) {
                    ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
                    data.late_finish = ifcapi::detail::sequence::get_start_or_finish_date(*data.late_start, duration, data.duration_type, data.calendar, "FINISH");
                } else {
                    ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
                    data.late_start = ifcapi::detail::sequence::get_start_or_finish_date(*data.late_finish, duration, data.duration_type, data.calendar, "START");
                }
            } else if (!finishes.empty()) data.late_finish = min_date(finishes);
            else if (!starts.empty()) data.late_start = min_date(starts);
        }
        if (!data.late_finish) {
            if (!data.late_start) return false;
            ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
            data.late_finish = ifcapi::detail::sequence::get_start_or_finish_date(*data.late_start, duration, data.duration_type, data.calendar, "FINISH");
        } else if (!data.late_start) {
            ifcapi::detail::sequence::Duration duration; duration.days = data.duration;
            data.late_start = ifcapi::detail::sequence::get_start_or_finish_date(*data.late_finish, duration, data.duration_type, data.calendar, "START");
        }
        if (data.duration_type == "WORKTIME") {
            data.total_float.days = ifcapi::detail::sequence::count_working_days(*data.early_finish, *data.late_finish, data.calendar);
        } else {
            data.total_float = ifcapi::detail::sequence::subtract(*data.late_finish, *data.early_finish);
            if (data.total_float.seconds == 60 * 60 * 8) {
                data.total_float.days += 1;
                data.total_float.seconds = 0;
            }
        }
        if (!free_floats.empty()) {
            data.free_float = *std::min_element(free_floats.begin(), free_floats.end(), [](const auto& a, const auto& b) {
                return a.days == b.days ? a.seconds < b.seconds : a.days < b.days;
            });
            if (data.free_float->seconds == 60 * 60 * 8) {
                data.free_float->days += 1;
                data.free_float->seconds = 0;
            }
        }
        return true;
    }

    ifcapi::detail::sequence::Duration calculate_free_float(
        const ifcapi::detail::sequence::DateTime& predecessor_date,
        const ifcapi::detail::sequence::DateTime& successor_date,
        int lag_time,
        const ScheduleNode& predecessor_data,
        const ScheduleNode& successor_data)
    {
        auto min_successor_date = successor_date;
        if (lag_time) {
            min_successor_date = min_date({
                offset_date(successor_date, -lag_time, predecessor_data),
                offset_date(successor_date, -lag_time, successor_data),
            });
        }
        if (predecessor_data.duration_type == "WORKTIME") {
            ifcapi::detail::sequence::Duration result;
            result.days = ifcapi::detail::sequence::count_working_days(predecessor_date, min_successor_date, predecessor_data.calendar);
            return result;
        }
        return ifcapi::detail::sequence::subtract(min_successor_date, predecessor_date);
    }

    void update_task_times() {
        for (auto& item : nodes_) {
            int id = item.first;
            if (id == start_id || id == finish_id) continue;
            auto& data = item.second;
            auto* task_time = ifcapi::detail::read_ref_attr(data.task, "TaskTime");
            if (!task_time) continue;
            ifcapi::detail::write_string_attr(task_time, "FreeFloat", ifcapi::detail::sequence::format_duration(data.free_float.value_or(ifcapi::detail::sequence::Duration{})));
            ifcapi::detail::write_string_attr(task_time, "TotalFloat", ifcapi::detail::sequence::format_duration(data.total_float));
            int critical_idx = ifcapi::detail::attr_index_of(task_time, "IsCritical");
            if (critical_idx >= 0) task_time->set_attribute_value(static_cast<size_t>(critical_idx), data.total_float.days == 0);
            if (data.early_start) ifcapi::detail::write_string_attr(task_time, "EarlyStart", ifcapi::detail::sequence::format_date_time(*data.early_start));
            if (data.early_finish) ifcapi::detail::write_string_attr(task_time, "EarlyFinish", ifcapi::detail::sequence::format_date_time(*data.early_finish));
            if (data.late_start) ifcapi::detail::write_string_attr(task_time, "LateStart", ifcapi::detail::sequence::format_date_time(*data.late_start));
            if (data.late_finish) ifcapi::detail::write_string_attr(task_time, "LateFinish", ifcapi::detail::sequence::format_date_time(*data.late_finish));
        }
    }
};

class CascadeSchedule {
public:
    explicit CascadeSchedule(IfcParse::IfcFile*) {}

    void execute(IfcUtil::IfcBaseClass* task) { cascade_task(task, true, {}); }

private:
    IfcUtil::IfcBaseClass* calendar(IfcUtil::IfcBaseClass* task) {
        return ifcapi::detail::sequence::derive_calendar(task);
    }

    ifcapi::detail::sequence::DateTime offset_date(
        const ifcapi::detail::sequence::DateTime& date,
        int days,
        const std::string& duration_type,
        IfcUtil::IfcBaseClass* calendar)
    {
        ifcapi::detail::sequence::Duration duration;
        duration.days = days;
        return ifcapi::detail::sequence::offset_date(date, duration, duration_type, calendar);
    }

    void cascade_task(IfcUtil::IfcBaseClass* task, bool is_first_task, std::vector<IfcUtil::IfcBaseClass*> task_sequence) {
        if (std::find(task_sequence.begin(), task_sequence.end(), task) != task_sequence.end()) {
            throw std::runtime_error("Recursive tasks found. Could not cascade schedule.");
        }
        auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime");
        if (!task_time) {
            return;
        }

        auto duration = read_task_duration(task);
        std::vector<ifcapi::detail::sequence::DateTime> finishes;
        std::vector<ifcapi::detail::sequence::DateTime> starts;

        for (auto* rel : sequence_assignment(task, "predecessor")) {
            auto* predecessor = ifcapi::detail::read_ref_attr(rel, "RelatingProcess");
            if (!predecessor) {
                continue;
            }
            auto predecessor_duration = read_task_duration(predecessor);
            auto type = ifcapi::detail::read_string_attr(rel, "SequenceType");
            auto* lag_time = ifcapi::detail::read_ref_attr(rel, "TimeLag");
            auto duration_type = std::string("WORKTIME");
            if (lag_time) {
                duration_type = ifcapi::detail::read_string_attr(lag_time, "DurationType");
            }

            if (type == "FINISH_START" || type.empty()) {
                ifcapi::detail::sequence::DateTime finish;
                if (!read_task_time_date(predecessor, "ScheduleFinish", finish)) continue;
                int days = predecessor_duration.days == 0 ? 0 : 1;
                if (lag_time) {
                    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
                    days += value && value->declaration().is("IfcDuration")
                        ? lag_time_days(lag_time)
                        : static_cast<int>(predecessor_duration.days * lag_time_ratio(lag_time));
                }
                if (days) {
                    starts.push_back(ifcapi::detail::sequence::with_time(offset_date(finish, days, duration_type, calendar(task)), 9));
                    starts.push_back(ifcapi::detail::sequence::with_time(offset_date(finish, days, duration_type, calendar(predecessor)), 9));
                } else {
                    starts.push_back(finish);
                }
            } else if (type == "START_START") {
                ifcapi::detail::sequence::DateTime start;
                if (!read_task_time_date(predecessor, "ScheduleStart", start)) continue;
                if (lag_time) {
                    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
                    int days = value && value->declaration().is("IfcDuration")
                        ? lag_time_days(lag_time)
                        : static_cast<int>(predecessor_duration.days * lag_time_ratio(lag_time));
                    starts.push_back(offset_date(start, days, duration_type, calendar(task)));
                    starts.push_back(offset_date(start, days, duration_type, calendar(predecessor)));
                } else {
                    starts.push_back(start);
                }
            } else if (type == "FINISH_FINISH") {
                ifcapi::detail::sequence::DateTime finish;
                if (!read_task_time_date(predecessor, "ScheduleFinish", finish)) continue;
                if (lag_time) {
                    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
                    int days = value && value->declaration().is("IfcDuration")
                        ? lag_time_days(lag_time)
                        : static_cast<int>(predecessor_duration.days * lag_time_ratio(lag_time));
                    finishes.push_back(offset_date(finish, days, duration_type, calendar(task)));
                    finishes.push_back(offset_date(finish, days, duration_type, calendar(predecessor)));
                } else {
                    finishes.push_back(finish);
                }
            } else if (type == "START_FINISH") {
                ifcapi::detail::sequence::DateTime start;
                if (!read_task_time_date(predecessor, "ScheduleStart", start)) continue;
                int days = -1;
                if (lag_time) {
                    auto* value = ifcapi::detail::read_ref_attr(lag_time, "LagValue");
                    days += value && value->declaration().is("IfcDuration")
                        ? lag_time_days(lag_time)
                        : static_cast<int>(predecessor_duration.days * lag_time_ratio(lag_time));
                }
                if (days || lag_time) {
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(start, days, duration_type, calendar(task)), 17));
                    finishes.push_back(ifcapi::detail::sequence::with_time(offset_date(start, days, duration_type, calendar(predecessor)), 17));
                } else {
                    finishes.push_back(start);
                }
            }
        }

        apply_dates(task, task_time, duration, starts, finishes, is_first_task);

        task_sequence.push_back(task);
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "IsPredecessorTo")) {
            if (auto* related = ifcapi::detail::read_ref_attr(rel, "RelatedProcess")) {
                cascade_task(related, false, task_sequence);
            }
        }
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(task, "IsNestedBy")) {
            for (auto* nested_task : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                cascade_task(nested_task, false, task_sequence);
            }
        }
    }

    void apply_dates(
        IfcUtil::IfcBaseClass* task,
        IfcUtil::IfcBaseClass* task_time,
        const ifcapi::detail::sequence::Duration& duration,
        const std::vector<ifcapi::detail::sequence::DateTime>& starts,
        const std::vector<ifcapi::detail::sequence::DateTime>& finishes,
        bool is_first_task)
    {
        auto max_date = [](const std::vector<ifcapi::detail::sequence::DateTime>& values) {
            return *std::max_element(values.begin(), values.end(), [](const auto& a, const auto& b) {
                return ifcapi::detail::sequence::date_serial(a) == ifcapi::detail::sequence::date_serial(b)
                    ? std::tie(a.hour, a.minute, a.second) < std::tie(b.hour, b.minute, b.second)
                    : ifcapi::detail::sequence::date_serial(a) < ifcapi::detail::sequence::date_serial(b);
            });
        };
        auto duration_type = ifcapi::detail::read_string_attr(task_time, "DurationType");
        if (duration_type.empty()) duration_type = "ELAPSEDTIME";
        if (!starts.empty() && !finishes.empty()) {
            auto start = max_date(starts);
            auto finish = max_date(finishes);
            auto potential_finish = ifcapi::detail::sequence::get_start_or_finish_date(
                start, duration, duration_type, calendar(task), "FINISH");
            if (ifcapi::detail::sequence::date_serial(potential_finish) > ifcapi::detail::sequence::date_serial(finish) ||
                (ifcapi::detail::sequence::date_serial(potential_finish) == ifcapi::detail::sequence::date_serial(finish) &&
                    std::tie(potential_finish.hour, potential_finish.minute, potential_finish.second) >
                        std::tie(finish.hour, finish.minute, finish.second))) {
                auto start_ifc = ifcapi::detail::sequence::format_date_time(start);
                if (ifcapi::detail::read_string_attr(task_time, "ScheduleStart") == start_ifc && !is_first_task) return;
                ifcapi::detail::write_string_attr(task_time, "ScheduleStart", start_ifc);
                ifcapi::detail::write_string_attr(task_time, "ScheduleFinish", ifcapi::detail::sequence::format_date_time(potential_finish));
            } else {
                auto finish_ifc = ifcapi::detail::sequence::format_date_time(finish);
                if (ifcapi::detail::read_string_attr(task_time, "ScheduleFinish") == finish_ifc && !is_first_task) return;
                ifcapi::detail::write_string_attr(task_time, "ScheduleFinish", finish_ifc);
                ifcapi::detail::write_string_attr(task_time, "ScheduleStart",
                    ifcapi::detail::sequence::format_date_time(ifcapi::detail::sequence::get_start_or_finish_date(
                        finish, duration, duration_type, calendar(task), "START")));
            }
        } else if (!finishes.empty()) {
            auto finish = max_date(finishes);
            auto finish_ifc = ifcapi::detail::sequence::format_date_time(finish);
            if (ifcapi::detail::read_string_attr(task_time, "ScheduleFinish") == finish_ifc && !is_first_task) return;
            ifcapi::detail::write_string_attr(task_time, "ScheduleFinish", finish_ifc);
            ifcapi::detail::write_string_attr(task_time, "ScheduleStart",
                ifcapi::detail::sequence::format_date_time(ifcapi::detail::sequence::get_start_or_finish_date(
                    finish, duration, duration_type, calendar(task), "START")));
        } else if (!starts.empty()) {
            auto start = max_date(starts);
            auto start_ifc = ifcapi::detail::sequence::format_date_time(start);
            if (ifcapi::detail::read_string_attr(task_time, "ScheduleStart") == start_ifc && !is_first_task) return;
            ifcapi::detail::write_string_attr(task_time, "ScheduleStart", start_ifc);
            ifcapi::detail::write_string_attr(task_time, "ScheduleFinish",
                ifcapi::detail::sequence::format_date_time(ifcapi::detail::sequence::get_start_or_finish_date(
                    start, duration, duration_type, calendar(task), "FINISH")));
        }
    }
};

} // namespace

namespace ifcapi {
namespace bindings {

SequenceDateTimeResult sequence_add_date_time(IfcParse::IfcFile* file, const std::string& date_time) {
    ifcopenshell_clear_error();
    return make_date_time_result(file, date_time);
}

void sequence_calculate_task_duration(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* task) {
    ifcopenshell_clear_error();
    try {
        double duration = max_resource_usage_duration(task, seconds_per_workday(task));
        if (duration <= 0.0) {
            return;
        }
        auto* task_time = ensure_task_time(file, task);
        ifcapi::detail::write_string_attr(task_time, "ScheduleDuration", "P" + std::to_string(static_cast<int>(duration)) + "D");
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_edit_task_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task_time,
    ifcopenshell_pset_props_t* attributes)
{
    ifcopenshell_clear_error();
    try {
        auto* task = task_from_task_time(file, task_time);
        auto* calendar = task ? ifcapi::detail::sequence::derive_calendar(task) : nullptr;
        if (prop_is_truthy(attributes, "ScheduleDuration") && prop_has_key(attributes, "ScheduleFinish")) {
            erase_prop_key(attributes, "ScheduleFinish");
        }
        normalize_task_time_dates(task_time, calendar, attributes);

        ifcapi::detail::apply_attribute_props(task_time, attributes);

        if (prop_has_key(attributes, "ScheduleDuration") &&
            !ifcapi::detail::is_attr_null(task_time, "ScheduleDuration") &&
            !ifcapi::detail::is_attr_null(task_time, "ScheduleStart")) {
            calculate_task_time_finish(task_time, calendar);
        } else if (prop_is_truthy(attributes, "ScheduleStart") &&
            !ifcapi::detail::is_attr_null(task_time, "ScheduleDuration")) {
            calculate_task_time_finish(task_time, calendar);
        } else if (prop_is_truthy(attributes, "ScheduleFinish") &&
            !ifcapi::detail::is_attr_null(task_time, "ScheduleStart")) {
            calculate_task_time_duration(task_time, calendar);
        }

        if (task && !ifcapi::detail::is_attr_null(task_time, "ScheduleDuration") &&
            (prop_has_key(attributes, "ScheduleStart") ||
                prop_has_key(attributes, "ScheduleFinish") ||
                prop_has_key(attributes, "ScheduleDuration"))) {
            CascadeSchedule(file).execute(task);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

SequenceDuplicateTaskResult sequence_duplicate_task(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        return DuplicateTask(file, owner_history, user, application).execute(task);
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

IfcUtil::IfcBaseClass* sequence_copy_work_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto* new_schedule = ifcapi::detail::shallow_copy(file, work_schedule);
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_schedule, "Controls")) {
            for (auto* task : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                auto duplicated = DuplicateTask(file, owner_history, user, application).execute(task).duplicate;
                if (!duplicated.empty()) {
                    ifcapi::bindings::control_assign_control(
                        file, new_schedule, one_const(duplicated.front()), owner_history, user, application);
                }
            }
        }
        return new_schedule;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void sequence_create_baseline(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    const char* name,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        if (ifcapi::detail::read_string_attr(work_schedule, "PredefinedType") != "PLANNED") {
            return;
        }
        const auto now = current_ifc_datetime();
        auto* baseline = sequence_add_work_schedule(
            file,
            ifcapi::detail::read_string_attr(work_schedule, "Name"),
            "BASELINE",
            nullptr,
            now,
            now,
            nullptr,
            nullptr,
            owner_history,
            user,
            application);
        if (name) {
            ifcapi::detail::write_string_attr(baseline, "Name", name);
        } else {
            ifcapi::detail::write_blank_attr(baseline, "Name");
        }
        create_object_reference(file, work_schedule, baseline, owner_history, user, application);
        for (auto* rel : ifcapi::detail::read_inverse_aggregate(work_schedule, "Controls")) {
            for (auto* task : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                if (!task || !task->declaration().is("IfcTask")) {
                    continue;
                }
                auto duplicated = DuplicateTask(file, owner_history, user, application).execute(task);
                if (!duplicated.duplicate.empty()) {
                    ifcapi::bindings::control_assign_control(
                        file, baseline, one_const(duplicated.duplicate.front()), owner_history, user, application);
                }
                for (size_t i = 0; i < duplicated.current.size() && i < duplicated.duplicate.size(); ++i) {
                    create_object_reference(
                        file, duplicated.current[i], duplicated.duplicate[i], owner_history, user, application);
                }
            }
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

IfcUtil::IfcBaseClass* sequence_add_task_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    bool is_recurring)
{
    ifcopenshell_clear_error();
    try {
        auto* task_time = create_entity(file, is_recurring ? "IfcTaskTimeRecurring" : "IfcTaskTime");
        ifcapi::detail::write_ref_attr(task, "TaskTime", task_time);
        return task_time;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_task(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* parent_task,
    const char* name,
    const char* description,
    const char* identification,
    const std::string& predefined_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto* task = create_root_entity(
            file, "IfcTask", predefined_type.c_str(), name, owner_history, user, application);
        if (description) {
            ifcapi::detail::write_string_attr(task, "Description", description);
        }
        if (identification) {
            ifcapi::detail::write_string_attr(task, "Identification", identification);
        }
        int milestone_idx = ifcapi::detail::attr_index_of(task, "IsMilestone");
        if (milestone_idx >= 0) {
            task->set_attribute_value(static_cast<size_t>(milestone_idx), false);
        }
        if (work_schedule) {
            ifcapi::bindings::control_assign_control(
                file, work_schedule, one_const(task), owner_history, user, application);
        } else if (parent_task) {
            auto* rel = ifcapi::bindings::nest_assign_object(
                file, one_const(task), parent_task, owner_history, user, application);
            auto parent_identification = ifcapi::detail::read_optional_string_attr(parent_task, "Identification");
            if (!is_ifc2x3(file) && rel && parent_identification.has_value) {
                auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
                ifcapi::detail::write_string_attr(
                    task, "Identification", parent_identification.value + "." + std::to_string(related.size()));
            }
        }
        return task;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_work_calendar(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::string& predefined_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto* work_calendar = create_root_entity(
            file, "IfcWorkCalendar", predefined_type.c_str(), name.c_str(), owner_history, user, application);
        ifcapi::bindings::project_assign_declaration(
            file, one_const(work_calendar), first_context(file), owner_history, user, application);
        return work_calendar;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_work_plan(
    IfcParse::IfcFile* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& creation_date,
    const std::string& start_time,
    IfcUtil::IfcBaseClass* creator_person,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto* work_plan = create_root_entity(
            file, "IfcWorkPlan", predefined_type.c_str(), name, owner_history, user, application);
        write_date_time_attr(file, work_plan, "CreationDate", creation_date);
        if (creator_person) {
            ifcapi::detail::write_ref_aggregate(work_plan, "Creators", {creator_person});
        }
        write_date_time_attr(file, work_plan, "StartTime", start_time);
        if (!is_ifc2x3(file)) {
            ifcapi::bindings::project_assign_declaration(
                file, one_const(work_plan), first_context(file), owner_history, user, application);
        }
        return work_plan;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_work_schedule(
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
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto* work_schedule = create_root_entity(
            file, "IfcWorkSchedule", predefined_type.c_str(), name.c_str(), owner_history, user, application);
        write_date_time_attr(file, work_schedule, "CreationDate", creation_date);
        if (creator_person) {
            ifcapi::detail::write_ref_aggregate(work_schedule, "Creators", {creator_person});
        }
        write_date_time_attr(file, work_schedule, "StartTime", start_time);
        if (object_type) {
            ifcapi::detail::write_string_attr(work_schedule, "ObjectType", object_type);
        }
        if (work_plan) {
            ifcapi::bindings::aggregate_assign_object(
                file, one_const(work_schedule), work_plan, owner_history, user, application);
        } else if (!is_ifc2x3(file)) {
            ifcapi::bindings::project_assign_declaration(
                file, one_const(work_schedule), first_context(file), owner_history, user, application);
        }
        return work_schedule;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_work_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_calendar,
    const std::string& time_type)
{
    ifcopenshell_clear_error();
    try {
        auto* work_time = create_entity(file, "IfcWorkTime");
        if (time_type == "WorkingTimes") {
            auto working_times = ifcapi::detail::read_ref_aggregate(work_calendar, "WorkingTimes");
            working_times.push_back(work_time);
            ifcapi::detail::write_ref_aggregate(work_calendar, "WorkingTimes", working_times);
        } else if (time_type == "ExceptionTimes") {
            auto exception_times = ifcapi::detail::read_ref_aggregate(work_calendar, "ExceptionTimes");
            exception_times.push_back(work_time);
            ifcapi::detail::write_ref_aggregate(work_calendar, "ExceptionTimes", exception_times);
        }
        return work_time;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_add_time_period(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* recurrence_pattern,
    const char* start_time,
    const char* end_time)
{
    ifcopenshell_clear_error();
    try {
        auto* time_period = create_entity(file, "IfcTimePeriod");
        if (start_time) {
            ifcapi::detail::write_string_attr(time_period, "StartTime", start_time);
        }
        if (end_time) {
            ifcapi::detail::write_string_attr(time_period, "EndTime", end_time);
        }
        auto time_periods = ifcapi::detail::read_ref_aggregate(recurrence_pattern, "TimePeriods");
        time_periods.push_back(time_period);
        ifcapi::detail::write_ref_aggregate(recurrence_pattern, "TimePeriods", time_periods);
        return time_period;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_sequence(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_process,
    const std::string& sequence_type,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        if (auto* existing = find_sequence(relating_process, related_process)) {
            return existing;
        }
        auto* rel = create_entity(file, "IfcRelSequence");
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        ifcapi::detail::write_ref_attr(rel, "RelatingProcess", relating_process);
        ifcapi::detail::write_ref_attr(rel, "RelatedProcess", related_process);
        ifcapi::detail::write_enum_attr(rel, "SequenceType", sequence_type);
        CascadeSchedule(file).execute(relating_process);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_lag_time(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* rel_sequence,
    const std::string& lag_value,
    const std::string& duration_type)
{
    ifcopenshell_clear_error();
    try {
        auto* duration = ifcapi::detail::create_typed_string(file, "IfcDuration", lag_value);
        auto* lag_time = create_entity(file, "IfcLagTime");
        ifcapi::detail::write_enum_attr(lag_time, "DurationType", duration_type);
        ifcapi::detail::write_ref_attr(lag_time, "LagValue", duration);
        if (rel_sequence && rel_sequence->declaration().is("IfcRelSequence")) {
            auto* current_lag_time = ifcapi::detail::read_ref_attr(rel_sequence, "TimeLag");
            if (current_lag_time && ifcapi::detail::total_inverses(file, current_lag_time) == 1) {
                file->removeEntity(current_lag_time);
            }
        }
        ifcapi::detail::write_ref_attr(rel_sequence, "TimeLag", lag_time);
        return lag_time;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_process(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        return assign_object_relationship(
            file,
            relating_process,
            related_object,
            "IfcRelAssignsToProcess",
            "RelatingProcess",
            "OperatesOn",
            false,
            owner_history,
            user,
            application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        return assign_object_relationship(
            file,
            relating_product,
            related_object,
            "IfcRelAssignsToProduct",
            "RelatingProduct",
            "ReferencedBy",
            true,
            owner_history,
            user,
            application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_work_plan(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* work_plan,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        ifcapi::bindings::project_unassign_declaration(
            file, one_const(work_schedule), first_context(file), user, application);
        return ifcapi::bindings::aggregate_assign_object(
            file, one_const(work_schedule), work_plan, owner_history, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_assign_recurrence_pattern(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    const std::string& recurrence_type)
{
    ifcopenshell_clear_error();
    try {
        auto* recurrence = create_entity(file, "IfcRecurrencePattern");
        ifcapi::detail::write_enum_attr(recurrence, "RecurrenceType", recurrence_type);
        if (parent && parent->declaration().is("IfcWorkTime")) {
            auto* old_recurrence = ifcapi::detail::read_ref_attr(parent, "RecurrencePattern");
            if (old_recurrence && ifcapi::detail::total_inverses(file, old_recurrence) == 1) {
                file->removeEntity(old_recurrence);
            }
            ifcapi::detail::write_ref_attr(parent, "RecurrencePattern", recurrence);
        } else if (parent && parent->declaration().is("IfcTaskTimeRecurring")) {
            auto* old_recurrence = ifcapi::detail::read_ref_attr(parent, "Recurrence");
            if (old_recurrence && ifcapi::detail::total_inverses(file, old_recurrence) == 1) {
                file->removeEntity(old_recurrence);
            }
            ifcapi::detail::write_ref_attr(parent, "Recurrence", recurrence);
        }
        return recurrence;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void sequence_cascade_schedule(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* task) {
    ifcopenshell_clear_error();
    try {
        CascadeSchedule(file).execute(task);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_recalculate_schedule(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* work_schedule) {
    ifcopenshell_clear_error();
    try {
        RecalculateSchedule(file).execute(work_schedule);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_edit_lag_time(IfcUtil::IfcBaseClass* lag_time, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(lag_time, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_sequence(IfcUtil::IfcBaseClass* rel_sequence, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(rel_sequence, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_task(IfcUtil::IfcBaseClass* task, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(task, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_recurrence_pattern(IfcUtil::IfcBaseClass* recurrence_pattern, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(recurrence_pattern, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_work_calendar(IfcUtil::IfcBaseClass* work_calendar, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(work_calendar, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_work_plan(IfcUtil::IfcBaseClass* work_plan, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(work_plan, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_work_schedule(IfcUtil::IfcBaseClass* work_schedule, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        ifcapi::detail::apply_attribute_props(work_schedule, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_edit_work_time(IfcUtil::IfcBaseClass* work_time, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        apply_work_time_props(work_time, attributes);
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void sequence_unassign_lag_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* rel_sequence) {
    ifcopenshell_clear_error();
    try {
        auto* current_lag_time = ifcapi::detail::read_ref_attr(rel_sequence, "TimeLag");
        if (!current_lag_time) return;
        if (ifcapi::detail::total_inverses(file, current_lag_time) == 1) {
            file->removeEntity(current_lag_time);
        } else {
            ifcapi::detail::write_ref_attr(rel_sequence, "TimeLag", nullptr);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_unassign_sequence(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_process)
{
    ifcopenshell_clear_error();
    try {
        auto rels = ifcapi::detail::read_inverse_aggregate(related_process, "IsSuccessorFrom");
        for (auto* rel : rels) {
            if (rel && ifcapi::detail::read_ref_attr(rel, "RelatingProcess") == relating_process) {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }
        CascadeSchedule(file).execute(related_process);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

IfcUtil::IfcBaseClass* sequence_unassign_process(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_process,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        return unassign_object_relationship(
            file, relating_process, related_object, "IfcRelAssignsToProcess", "RelatingProcess", user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* sequence_unassign_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_product,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        return unassign_object_relationship(
            file, relating_product, related_object, "IfcRelAssignsToProduct", "RelatingProduct", user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void sequence_unassign_recurrence_pattern(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* recurrence_pattern) {
    ifcopenshell_clear_error();
    try {
        auto time_periods = ifcapi::detail::read_ref_aggregate(recurrence_pattern, "TimePeriods");
        for (auto* time_period : time_periods) {
            file->removeEntity(time_period);
        }
        file->removeEntity(recurrence_pattern);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_time_period(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* time_period) {
    ifcopenshell_clear_error();
    try {
        file->removeEntity(time_period);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_work_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* work_time) {
    ifcopenshell_clear_error();
    try {
        if (auto* recurrence_pattern = ifcapi::detail::read_ref_attr(work_time, "RecurrencePattern")) {
            auto time_periods = ifcapi::detail::read_ref_aggregate(recurrence_pattern, "TimePeriods");
            for (auto* time_period : time_periods) {
                file->removeEntity(time_period);
            }
            file->removeEntity(recurrence_pattern);
        }
        file->removeEntity(work_time);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_task(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* task,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        remove_task_internal(file, task, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_work_calendar(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_calendar,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        remove_work_calendar_internal(file, work_calendar, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_work_plan(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_plan,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        remove_work_plan_internal(file, work_plan, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void sequence_remove_work_schedule(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* work_schedule,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        remove_work_schedule_internal(file, work_schedule, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
