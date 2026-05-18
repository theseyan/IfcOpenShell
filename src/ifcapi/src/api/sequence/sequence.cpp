// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/sequence.h"
#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/control.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/project.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/relationship.h"
#include "ifcapi/detail/sequence_time.h"
#include "../pset/attribute_props.hpp"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <string>
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

ifcapi::detail::sequence::Duration read_task_duration(IfcUtil::IfcBaseClass* task) {
    ifcapi::detail::sequence::Duration result;
    auto* task_time = ifcapi::detail::read_ref_attr(task, "TaskTime");
    auto duration = task_time ? ifcapi::detail::read_optional_string_attr(task_time, "ScheduleDuration") : ifcapi::detail::OptionalString{};
    if (duration.has_value) {
        ifcapi::detail::sequence::parse_duration(duration.value, result);
    }
    return result;
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
        try {
            auto arg = value->get_attribute_value(static_cast<size_t>(idx));
            if (!arg.isNull() && arg.type() == IfcUtil::Argument_STRING) {
                text.has_value = true;
                text.value = static_cast<std::string>(arg);
            }
        } catch (...) {
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
    try {
        auto arg = value->get_attribute_value(static_cast<size_t>(idx));
        if (!arg.isNull()) {
            return static_cast<double>(arg);
        }
    } catch (...) {
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

} // namespace bindings
} // namespace ifcapi
