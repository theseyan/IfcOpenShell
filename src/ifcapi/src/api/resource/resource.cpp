// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/resource.h"

#include "ifcapi/bindings/sequence.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/sequence_time.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<IfcUtil::IfcBaseClass*> get_inverse(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || entity->id() == 0) {
        return result;
    }
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    for (auto* inverse : *inverses) {
        if (inverse) {
            result.push_back(inverse);
        }
    }
    return result;
}

IfcUtil::IfcBaseClass* get_resource(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* resource_time) {
    for (auto* inverse : get_inverse(file, resource_time)) {
        if (inverse->declaration().is("IfcResource")) {
            return inverse;
        }
    }
    throw std::runtime_error("IfcResourceTime is not assigned to a resource");
}

std::string metric_reference(IfcUtil::IfcBaseClass* metric, bool deep) {
    std::string path;
    auto* ref = ifcapi::detail::read_ref_attr(metric, "ReferencePath");
    while (ref) {
        auto part = ifcapi::detail::read_string_attr(ref, "AttributeIdentifier");
        if (!deep) {
            return part;
        }
        if (path.empty()) {
            path = part;
        } else if (!part.empty()) {
            path += "." + part;
        }
        ref = ifcapi::detail::read_ref_attr(ref, "InnerReference");
    }
    return path;
}

std::vector<IfcUtil::IfcBaseClass*> get_metric_constraints(IfcUtil::IfcBaseClass* resource, const std::string& attribute) {
    std::vector<IfcUtil::IfcBaseClass*> metrics;
    for (auto* rel : ifcapi::detail::read_ref_aggregate(resource, "HasAssociations")) {
        if (!rel || !rel->declaration().is("IfcRelAssociatesConstraint")) {
            continue;
        }
        auto* constraint = ifcapi::detail::read_ref_attr(rel, "RelatingConstraint");
        for (auto* metric : ifcapi::detail::read_ref_aggregate(constraint, "BenchmarkValues")) {
            if (metric_reference(metric, false) == attribute || metric_reference(metric, true) == attribute) {
                metrics.push_back(metric);
            }
        }
    }
    return metrics;
}

bool is_hard_constraint(IfcUtil::IfcBaseClass* metric) {
    return metric && ifcapi::detail::read_string_attr(metric, "ConstraintGrade") == "HARD" &&
        ifcapi::detail::read_string_attr(metric, "Benchmark") == "EQUALTO";
}

IfcUtil::IfcBaseClass* get_task_assignment(IfcUtil::IfcBaseClass* resource) {
    for (auto* rel : ifcapi::detail::read_ref_aggregate(resource, "HasAssignments")) {
        if (rel && rel->declaration().is("IfcRelAssignsToProcess")) {
            return ifcapi::detail::read_ref_attr(rel, "RelatingProcess");
        }
    }
    return nullptr;
}

std::string format_fractional_seconds(int seconds, int microseconds, bool pad_seconds) {
    char buffer[32];
    if (microseconds == 0) {
        std::snprintf(buffer, sizeof(buffer), pad_seconds ? "%02d" : "%d", seconds);
        return std::string(buffer);
    }
    std::snprintf(buffer, sizeof(buffer), pad_seconds ? "%02d.%06d" : "%d.%06d", seconds, microseconds);
    std::string value = buffer;
    if (!pad_seconds) {
        while (!value.empty() && value.back() == '0') value.pop_back();
    }
    return value;
}

std::string format_date_time_value(const ifcapi_pset::Entry& entry) {
    char prefix[24];
    std::snprintf(prefix, sizeof(prefix), "%04d-%02d-%02dT%02d:%02d:", entry.year, entry.month, entry.day, entry.hour, entry.minute);
    std::string value = std::string(prefix) + format_fractional_seconds(entry.second, entry.microsecond, true);
    if (entry.has_timezone) {
        int offset = entry.timezone_offset_minutes;
        char tz[8];
        std::snprintf(tz, sizeof(tz), "%c%02d:%02d", offset < 0 ? '-' : '+', std::abs(offset) / 60, std::abs(offset) % 60);
        value += tz;
    }
    return value;
}

std::string format_duration_value(const ifcapi_pset::Entry& entry) {
    std::string value;
    if (entry.duration_negative) {
        value += "-";
    }
    value += "P";
    if (entry.duration_years) value += std::to_string(entry.duration_years) + "Y";
    if (entry.duration_months) value += std::to_string(entry.duration_months) + "M";
    if (entry.duration_days) value += std::to_string(entry.duration_days) + "D";
    if (entry.duration_hours || entry.duration_minutes || entry.duration_seconds || entry.duration_microseconds) {
        value += "T";
        if (entry.duration_hours) value += std::to_string(entry.duration_hours) + "H";
        if (entry.duration_minutes) value += std::to_string(entry.duration_minutes) + "M";
        if (entry.duration_seconds || entry.duration_microseconds) {
            value += format_fractional_seconds(entry.duration_seconds, entry.duration_microseconds, false) + "S";
        }
    }
    if (value == "P" || value == "-P") {
        value += "0D";
    }
    return value;
}

void apply_resource_time_attr(IfcUtil::IfcBaseClass* resource_time, const ifcapi_pset::Entry& entry) {
    if (entry.kind == ifcapi_pset::Kind::DATE || entry.kind == ifcapi_pset::Kind::DATETIME) {
        ifcapi::detail::write_string_attr(resource_time, entry.key.c_str(), format_date_time_value(entry));
    } else if (entry.kind == ifcapi_pset::Kind::DURATION) {
        ifcapi::detail::write_string_attr(resource_time, entry.key.c_str(), format_duration_value(entry));
    } else {
        ifcapi::detail::apply_attribute_prop(resource_time, entry);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

void resource_edit_resource_time(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* resource_time, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        if (!file || !resource_time) {
            throw std::runtime_error("resource_edit_resource_time requires a file and resource time");
        }
        auto* resource = get_resource(file, resource_time);
        if (!attributes) {
            return;
        }
        for (const auto& entry : attributes->entries) {
            auto metrics = get_metric_constraints(resource, "Usage." + entry.key);
            if (!metrics.empty() && is_hard_constraint(metrics.front())) {
                continue;
            }
            apply_resource_time_attr(resource_time, entry);
            if (entry.key == "ScheduleUsage" && !get_metric_constraints(resource, "Usage.ScheduleWork").empty()) {
                if (auto* task = get_task_assignment(resource)) {
                    sequence_calculate_task_duration(file, task);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
