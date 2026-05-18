// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_SEQUENCE_TIME_H
#define IFCAPI_DETAIL_SEQUENCE_TIME_H

#include "ifcapi/detail/attribute.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <tuple>

namespace ifcapi {
namespace detail {
namespace sequence {

struct Duration {
    int days = 0;
    int seconds = 0;
    int months = 0;
    int years = 0;
};

struct DateTime {
    int year = 1970;
    int month = 1;
    int day = 1;
    int hour = 0;
    int minute = 0;
    int second = 0;
};

inline int days_from_civil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<int>(doe) - 719468;
}

inline DateTime civil_from_days(int z) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = static_cast<int>(yoe) + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    const unsigned d = doy - (153 * mp + 2) / 5 + 1;
    const unsigned m = mp + (mp < 10 ? 3 : -9);
    y += m <= 2;
    return DateTime{y, static_cast<int>(m), static_cast<int>(d), 0, 0, 0};
}

inline int date_serial(const DateTime& value) {
    return days_from_civil(value.year, static_cast<unsigned>(value.month), static_cast<unsigned>(value.day));
}

inline DateTime add_days(DateTime value, int days) {
    auto result = civil_from_days(date_serial(value) + days);
    result.hour = value.hour;
    result.minute = value.minute;
    result.second = value.second;
    return result;
}

inline DateTime with_time(DateTime value, int hour, int minute = 0, int second = 0) {
    value.hour = hour;
    value.minute = minute;
    value.second = second;
    return value;
}

inline bool less(const DateTime& a, const DateTime& b) {
    return date_serial(a) == date_serial(b) ? std::tie(a.hour, a.minute, a.second) < std::tie(b.hour, b.minute, b.second)
                                            : date_serial(a) < date_serial(b);
}

inline int seconds_since_epoch(const DateTime& value) {
    return date_serial(value) * 24 * 60 * 60 + value.hour * 60 * 60 + value.minute * 60 + value.second;
}

inline Duration subtract(const DateTime& finish, const DateTime& start) {
    int seconds = seconds_since_epoch(finish) - seconds_since_epoch(start);
    Duration result;
    result.days = seconds / (24 * 60 * 60);
    result.seconds = seconds % (24 * 60 * 60);
    if (result.seconds < 0) {
        --result.days;
        result.seconds += 24 * 60 * 60;
    }
    return result;
}

inline bool parse_date_time(const std::string& value, DateTime& result) {
    if (value.size() < 10) {
        return false;
    }
    int matched = std::sscanf(value.c_str(), "%d-%d-%dT%d:%d:%d", &result.year, &result.month, &result.day,
        &result.hour, &result.minute, &result.second);
    if (matched >= 3) {
        if (matched < 6) {
            result.hour = result.minute = result.second = 0;
        }
        return true;
    }
    return false;
}

inline std::string format_date(const DateTime& value) {
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", value.year, value.month, value.day);
    return std::string(buffer);
}

inline std::string format_date_time(const DateTime& value) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d", value.year, value.month, value.day,
        value.hour, value.minute, value.second);
    return std::string(buffer);
}

inline std::string format_time(const DateTime& value) {
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", value.hour, value.minute, value.second);
    return std::string(buffer);
}

inline bool parse_duration(const std::string& value, Duration& result) {
    if (value.empty() || value[0] != 'P') {
        return false;
    }
    bool in_time = false;
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
            return false;
        }
        char designator = *end;
        if (designator == 'Y') {
            result.years += static_cast<int>(amount);
        } else if (designator == 'M' && !in_time) {
            result.months += static_cast<int>(amount);
        } else if (designator == 'W') {
            result.days += static_cast<int>(amount * 7);
        } else if (designator == 'D') {
            result.days += static_cast<int>(amount);
        } else if (designator == 'H') {
            result.seconds += static_cast<int>(amount * 60 * 60);
        } else if (designator == 'M' && in_time) {
            result.seconds += static_cast<int>(amount * 60);
        } else if (designator == 'S') {
            result.seconds += static_cast<int>(amount);
        } else {
            return false;
        }
        i = static_cast<size_t>(end - value.c_str()) + 1;
    }
    return true;
}

inline std::string format_duration(const Duration& duration) {
    int days = duration.days;
    int seconds = duration.seconds;
    if (!duration.years && !duration.months && !days && !seconds) {
        return "P0D";
    }
    std::string result = "P";
    if (duration.years) result += std::to_string(duration.years) + "Y";
    if (duration.months) result += std::to_string(duration.months) + "M";
    if (days) result += std::to_string(days) + "D";
    if (seconds) {
        result += "T";
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;
        if (hours) result += std::to_string(hours) + "H";
        if (minutes) result += std::to_string(minutes) + "M";
        if (seconds) result += std::to_string(seconds) + "S";
    }
    return result;
}

inline bool contains_int(const std::vector<int>& values, int value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

inline std::vector<int> read_int_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr) {
    std::vector<int> result;
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return result;
    }
    try {
        auto value = entity->get_attribute_value(static_cast<size_t>(idx));
        if (!value.isNull()) {
            result = static_cast<std::vector<int>>(value);
        }
    } catch (...) {
    }
    return result;
}

inline bool read_ifc_date(IfcUtil::IfcBaseClass* value, DateTime& result) {
    if (!value) {
        return false;
    }
    if (value->declaration().is("IfcCalendarDate")) {
        result.year = read_int_attr(value, "YearComponent");
        result.month = read_int_attr(value, "MonthComponent");
        result.day = read_int_attr(value, "DayComponent");
        return true;
    }
    if (value->declaration().is("IfcDateAndTime")) {
        auto* date = read_ref_attr(value, "DateComponent");
        auto* time = read_ref_attr(value, "TimeComponent");
        if (!read_ifc_date(date, result)) {
            return false;
        }
        result.hour = read_int_attr(time, "HourComponent");
        result.minute = read_int_attr(time, "MinuteComponent");
        result.second = read_int_attr(time, "SecondComponent");
        return true;
    }
    return false;
}

inline bool read_date_attribute(IfcUtil::IfcBaseClass* entity, const char* attr, DateTime& result) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return false;
    }
    try {
        auto value = entity->get_attribute_value(static_cast<size_t>(idx));
        if (value.isNull()) {
            return false;
        }
        if (value.type() == IfcUtil::Argument_STRING) {
            return parse_date_time(static_cast<std::string>(value), result);
        }
        if (value.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
            return read_ifc_date(static_cast<IfcUtil::IfcBaseClass*>(value), result);
        }
    } catch (...) {
    }
    return false;
}

inline IfcUtil::IfcBaseClass* derive_calendar(IfcUtil::IfcBaseClass* task) {
    for (auto* rel : read_inverse_aggregate(task, "HasAssignments")) {
        auto* control = read_ref_attr(rel, "RelatingControl");
        if (rel && rel->declaration().is("IfcRelAssignsToControl") && control && control->declaration().is("IfcWorkCalendar")) {
            return control;
        }
    }
    for (auto* rel : read_inverse_aggregate(task, "Nests")) {
        if (auto* parent = read_ref_attr(rel, "RelatingObject")) {
            return derive_calendar(parent);
        }
    }
    return nullptr;
}

inline bool is_day_in_work_time(const DateTime& day, IfcUtil::IfcBaseClass* work_time) {
    bool applicable = true;
    DateTime start;
    if (read_date_attribute(work_time, "Start", start) || read_date_attribute(work_time, "StartDate", start)) {
        applicable = date_serial(day) > date_serial(start);
    }
    DateTime finish;
    if (read_date_attribute(work_time, "Finish", finish) || read_date_attribute(work_time, "FinishDate", finish)) {
        applicable = date_serial(day) < date_serial(finish);
    }
    return applicable;
}

inline bool is_work_time_applicable_to_day(IfcUtil::IfcBaseClass* work_time, const DateTime& day) {
    if (!is_day_in_work_time(day, work_time)) {
        return false;
    }
    auto* recurrence = read_ref_attr(work_time, "RecurrencePattern");
    if (!recurrence) {
        return true;
    }
    std::string type = read_string_attr(recurrence, "RecurrenceType");
    bool has_interval = !is_attr_null(recurrence, "Interval");
    bool has_occurrences = !is_attr_null(recurrence, "Occurrences");
    if (type == "DAILY") {
        return !has_interval && !has_occurrences;
    }
    if (type == "WEEKLY") {
        if (has_interval || has_occurrences) return false;
        int weekday = (date_serial(day) + 3) % 7 + 1;
        return contains_int(read_int_aggregate(recurrence, "WeekdayComponent"), weekday);
    }
    if (type == "MONTHLY_BY_DAY_OF_MONTH") {
        if (has_interval || has_occurrences) return false;
        return contains_int(read_int_aggregate(recurrence, "DayComponent"), day.day);
    }
    if (type == "YEARLY_BY_DAY_OF_MONTH") {
        if (has_interval || has_occurrences) return false;
        return contains_int(read_int_aggregate(recurrence, "MonthComponent"), day.month) &&
            contains_int(read_int_aggregate(recurrence, "DayComponent"), day.day);
    }
    int weekday = (date_serial(day) + 3) % 7 + 1;
    int position = static_cast<int>(std::floor(day.day / 7.0)) + 1;
    if (type == "MONTHLY_BY_POSITION") {
        if (has_interval || has_occurrences) return false;
        return contains_int(read_int_aggregate(recurrence, "WeekdayComponent"), weekday) &&
            read_int_attr(recurrence, "Position") == position;
    }
    if (type == "YEARLY_BY_POSITION") {
        if (has_interval || has_occurrences) return false;
        return contains_int(read_int_aggregate(recurrence, "MonthComponent"), day.month) &&
            contains_int(read_int_aggregate(recurrence, "WeekdayComponent"), weekday) &&
            read_int_attr(recurrence, "Position") == position;
    }
    return false;
}

inline bool is_calendar_applicable(const DateTime& day, IfcUtil::IfcBaseClass* calendar) {
    if (!calendar || read_ref_aggregate(calendar, "WorkingTimes").empty()) {
        return false;
    }
    for (auto* work_time : read_ref_aggregate(calendar, "WorkingTimes")) {
        if (is_day_in_work_time(day, work_time)) {
            return true;
        }
    }
    return false;
}

inline bool is_working_day(const DateTime& day, IfcUtil::IfcBaseClass* calendar) {
    bool working = false;
    for (auto* work_time : read_ref_aggregate(calendar, "WorkingTimes")) {
        if (is_work_time_applicable_to_day(work_time, day)) {
            working = true;
            break;
        }
    }
    if (!working) {
        return false;
    }
    for (auto* work_time : read_ref_aggregate(calendar, "ExceptionTimes")) {
        if (is_work_time_applicable_to_day(work_time, day)) {
            return false;
        }
    }
    return true;
}

inline DateTime get_soonest_working_day(DateTime start, const std::string& duration_type, IfcUtil::IfcBaseClass* calendar) {
    if (duration_type == "ELAPSEDTIME" || !is_calendar_applicable(start, calendar)) {
        return start;
    }
    while (!is_working_day(start, calendar)) {
        if (!is_calendar_applicable(start, calendar)) {
            break;
        }
        start = add_days(start, 1);
    }
    return start;
}

inline DateTime get_recent_working_day(DateTime start, const std::string& duration_type, IfcUtil::IfcBaseClass* calendar) {
    if (duration_type == "ELAPSEDTIME" || !is_calendar_applicable(start, calendar)) {
        return start;
    }
    while (!is_working_day(start, calendar)) {
        if (!is_calendar_applicable(start, calendar)) {
            break;
        }
        start = add_days(start, -1);
    }
    return start;
}

inline DateTime offset_date(DateTime start, const Duration& duration, const std::string& duration_type, IfcUtil::IfcBaseClass* calendar) {
    int total_days = duration.days + duration.months * 30 + duration.years * 12 * 30;
    int remaining = std::abs(total_days);
    int direction = duration.days > 0 ? 1 : -1;
    while (remaining > 0) {
        if (duration_type == "ELAPSEDTIME" || !is_calendar_applicable(start, calendar)) {
            --remaining;
        } else if (is_working_day(start, calendar)) {
            --remaining;
        }
        start = add_days(start, direction);
    }
    return duration.days > 0 ? get_soonest_working_day(start, duration_type, calendar)
                             : get_recent_working_day(start, duration_type, calendar);
}

inline DateTime get_start_or_finish_date(
    DateTime start,
    const Duration& duration,
    const std::string& duration_type,
    IfcUtil::IfcBaseClass* calendar,
    const std::string& date_type)
{
    if (!duration.days) {
        return start;
    }
    Duration offset = duration;
    offset.days = duration.days + duration.months * 30 + duration.years * 12 * 30 - 1;
    offset.months = 0;
    offset.years = 0;
    if (date_type == "START") {
        offset.days = -offset.days;
    }
    auto result = offset_date(start, offset, duration_type, calendar);
    return date_type == "START" ? with_time(result, 9) : with_time(result, 17);
}

inline int count_working_days(DateTime start, DateTime finish, IfcUtil::IfcBaseClass* calendar) {
    if (date_serial(start) == date_serial(finish)) {
        return 0;
    }
    int result = 0;
    int current = date_serial(start);
    int end = date_serial(finish);
    while (current <= end) {
        auto day = civil_from_days(current);
        if (calendar && !read_ref_aggregate(calendar, "WorkingTimes").empty() && is_working_day(day, calendar)) {
            ++result;
        } else if (!calendar || !is_calendar_applicable(day, calendar)) {
            ++result;
        }
        ++current;
    }
    return result;
}

} // namespace sequence
} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_SEQUENCE_TIME_H */
