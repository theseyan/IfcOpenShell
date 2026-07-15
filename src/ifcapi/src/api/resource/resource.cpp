// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/resource.h"

#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/project.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/sequence.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "ifcapi/detail/sequence_time.h"
#include "guid.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<express::Base> get_inverse(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() == 0) {
        return result;
    }
    auto inverses = file->instances_by_reference(static_cast<int>(entity.id()));
    for (auto inverse : inverses) {
        if (inverse) {
            result.push_back(inverse);
        }
    }
    return result;
}

express::Base get_resource(ifcopenshell::file* file, express::Base resource_time) {
    for (auto inverse : get_inverse(file, resource_time)) {
        if (inverse.declaration().is("IfcResource")) {
            return inverse;
        }
    }
    throw std::runtime_error("IfcResourceTime is not assigned to a resource");
}

std::string metric_reference(express::Base metric, bool deep) {
    std::string path;
    auto ref = ifcapi::detail::read_ref_attr(metric, "ReferencePath");
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

std::vector<express::Base> get_metric_constraints(express::Base resource, const std::string& attribute) {
    std::vector<express::Base> metrics;
    for (auto rel : ifcapi::detail::read_inverse_aggregate(resource, "HasAssociations")) {
        if (!rel || !rel.declaration().is("IfcRelAssociatesConstraint")) {
            continue;
        }
        auto constraint = ifcapi::detail::read_ref_attr(rel, "RelatingConstraint");
        for (auto metric : ifcapi::detail::read_ref_aggregate(constraint, "BenchmarkValues")) {
            if (metric_reference(metric, false) == attribute || metric_reference(metric, true) == attribute) {
                metrics.push_back(metric);
            }
        }
    }
    return metrics;
}

bool is_hard_constraint(express::Base metric) {
    return metric && ifcapi::detail::read_string_attr(metric, "ConstraintGrade") == "HARD" &&
        ifcapi::detail::read_string_attr(metric, "Benchmark") == "EQUALTO";
}

express::Base get_task_assignment(express::Base resource) {
    for (auto rel : ifcapi::detail::read_inverse_aggregate(resource, "HasAssignments")) {
        if (rel && rel.declaration().is("IfcRelAssignsToProcess")) {
            return ifcapi::detail::read_ref_attr(rel, "RelatingProcess");
        }
    }
    return {};
}

void require_owned(ifcopenshell::file* file, express::Base entity, const char* name) {
    if (!entity || entity.file() != file) throw std::runtime_error(std::string(name) + " must belong to the target file");
}

std::optional<double> numeric_value(const attribute_value& value) {
    if (value.isNull()) return std::nullopt;
    if (value.type() == ifcopenshell::Argument_DOUBLE) return static_cast<double>(value);
    if (value.type() == ifcopenshell::Argument_INT) return static_cast<double>(static_cast<int>(value));
    if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
        auto nested = static_cast<express::Base>(value);
        return nested ? numeric_value(nested.get_attribute_value(0)) : std::nullopt;
    }
    return std::nullopt;
}

std::optional<std::string> scalar_string(express::Base value) {
    if (!value) return std::nullopt;
    auto arg = value.get_attribute_value(0);
    if (!arg.isNull() && arg.type() == ifcopenshell::Argument_STRING) return static_cast<std::string>(arg);
    return std::nullopt;
}

express::Base named_pset(express::Base object, const std::string& name) {
    for (auto rel : ifcapi::detail::read_inverse_aggregate(object, "IsDefinedBy")) {
        if (!rel || !rel.declaration().is("IfcRelDefinesByProperties")) continue;
        auto pset = ifcapi::detail::read_ref_attr(rel, "RelatingPropertyDefinition");
        if (pset && ifcapi::detail::read_string_attr(pset, "Name") == name) return pset;
    }
    for (auto rel : ifcapi::detail::read_inverse_aggregate(object, "IsTypedBy")) {
        auto type = ifcapi::detail::read_ref_attr(rel, "RelatingType");
        for (auto pset : ifcapi::detail::read_ref_aggregate(type, "HasPropertySets")) {
            if (ifcapi::detail::read_string_attr(pset, "Name") == name) return pset;
        }
    }
    return {};
}

express::Base productivity_pset(express::Base resource) {
    if (auto pset = named_pset(resource, "EPset_Productivity")) return pset;
    auto nests = ifcapi::detail::read_inverse_aggregate(resource, "Nests");
    if (nests.empty()) nests = ifcapi::detail::read_inverse_aggregate(resource, "Decomposes");
    for (auto rel : nests) {
        if (!rel.declaration().is("IfcRelNests")) continue;
        return named_pset(ifcapi::detail::read_ref_attr(rel, "RelatingObject"), "EPset_Productivity");
    }
    return {};
}

express::Base pset_property(express::Base pset, const std::string& name) {
    for (auto prop : ifcapi::detail::read_ref_aggregate(pset, "HasProperties")) {
        if (ifcapi::detail::read_string_attr(prop, "Name") == name) return prop;
    }
    return {};
}

std::optional<std::string> pset_string(express::Base pset, const std::string& name) {
    auto prop = pset_property(pset, name);
    return scalar_string(ifcapi::detail::read_ref_attr(prop, "NominalValue"));
}

std::optional<double> pset_number(express::Base pset, const std::string& name) {
    auto nominal = ifcapi::detail::read_ref_attr(pset_property(pset, name), "NominalValue");
    return nominal ? numeric_value(nominal.get_attribute_value(0)) : std::nullopt;
}

std::optional<double> definition_quantity(express::Base definition, const std::string& name) {
    if (!definition) return std::nullopt;
    if (definition.declaration().is("IfcElementQuantity")) {
        for (auto quantity : ifcapi::detail::read_ref_aggregate(definition, "Quantities")) {
            if (ifcapi::detail::read_string_attr(quantity, "Name") == name) return numeric_value(quantity.get_attribute_value(3));
        }
    } else {
        auto nominal = ifcapi::detail::read_ref_attr(pset_property(definition, name), "NominalValue");
        if (nominal) return numeric_value(nominal.get_attribute_value(0));
    }
    return std::nullopt;
}

std::optional<double> first_product_quantity(express::Base product, const std::string& name) {
    for (auto rel : ifcapi::detail::read_inverse_aggregate(product, "IsDefinedBy")) {
        if (!rel || !rel.declaration().is("IfcRelDefinesByProperties")) continue;
        if (auto value = definition_quantity(ifcapi::detail::read_ref_attr(rel, "RelatingPropertyDefinition"), name)) return value;
    }
    for (auto rel : ifcapi::detail::read_inverse_aggregate(product, "IsTypedBy")) {
        auto type = ifcapi::detail::read_ref_attr(rel, "RelatingType");
        for (auto definition : ifcapi::detail::read_ref_aggregate(type, "HasPropertySets")) {
            if (auto value = definition_quantity(definition, name)) return value;
        }
    }
    return std::nullopt;
}

bool attribute_locked(express::Base resource, const std::string& attribute) {
    for (auto metric : get_metric_constraints(resource, attribute)) if (is_hard_constraint(metric)) return true;
    return false;
}

std::string format_float(double value) {
    std::ostringstream stream;
    stream.precision(15);
    stream << value;
    auto result = stream.str();
    if (result.find_first_of(".eE") == std::string::npos) result += ".0";
    return result;
}

void remove_or_update_member(ifcopenshell::file* file, express::Base rel, const char* attr, express::Base member,
    express::Base user, express::Base application) {
    auto values = ifcapi::detail::read_ref_aggregate(rel, attr);
    std::vector<express::Base> remaining;
    for (auto value : values) if (!ifcapi::detail::same_instance(value, member)) remaining.push_back(value);
    if (remaining.size() == values.size()) return;
    if (remaining.empty()) ifcapi::detail::remove_with_history(file, rel);
    else {
        ifcapi::detail::write_ref_aggregate(rel, attr, remaining);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    }
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

void apply_resource_time_attr(express::Base resource_time, const ifcapi_pset::Entry& entry) {
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

express::Base resource_add_resource(ifcopenshell::file* file, const ResourceAddResourceOptions& options) {
    ifcopenshell_clear_error();
    try {
        if (!file) throw std::runtime_error("resource_add_resource requires a file");
        if (options.parent_resource) require_owned(file, *options.parent_resource, "Parent resource");
        RootCreateEntityOptions root;
        root.ifc_class = options.ifc_class.value_or("IfcCrewResource");
        root.name = options.name && !options.name->empty() ? *options.name : "Unnamed";
        root.predefined_type = options.predefined_type.value_or("NOTDEFINED");
        root.owner_history = ifcapi::detail::ensure_owner_history(file, options.owner_history.value_or(express::Base()),
            options.user.value_or(express::Base()), options.application.value_or(express::Base()));
        auto resource = root_create_entity(file, root);
        if (!resource) throw std::runtime_error("Failed to create resource");
        if (options.parent_resource && *options.parent_resource) {
            NestAssignObjectOptions nest{{resource}, *options.parent_resource, options.owner_history, options.user, options.application};
            if (!nest_assign_object(file, nest)) throw std::runtime_error("Failed to nest resource");
        } else if (file->schema()->name() != "IFC2X3") {
            auto contexts = ifcapi::detail::instances_by_type(file, "IfcContext");
            if (contexts.empty()) throw std::out_of_range("No IfcContext exists for resource declaration");
            ProjectAssignDeclarationOptions declaration{{resource}, contexts.front(), options.owner_history, options.user, options.application};
            project_assign_declaration(file, declaration);
        }
        return resource;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

express::Base resource_add_resource_quantity(ifcopenshell::file* file, express::Base* resource_ptr, const std::string& ifc_class) {
    ifcopenshell_clear_error();
    try {
        auto resource = ifcapi::detail::deref_or_empty(resource_ptr);
        require_owned(file, resource, "Resource");
        static const std::map<std::string, std::vector<std::string>> supported = {
            {"IfcCrewResource", {"IfcQuantityTime"}}, {"IfcLaborResource", {"IfcQuantityTime"}},
            {"IfcSubContractResource", {"IfcQuantityTime"}}, {"IfcConstructionEquipmentResource", {"IfcQuantityTime"}},
            {"IfcConstructionMaterialResource", {"IfcQuantityVolume", "IfcQuantityArea", "IfcQuantityLength", "IfcQuantityWeight"}},
            {"IfcConstructionProductResource", {"IfcQuantityCount"}},
        };
        auto type = resource.declaration().name();
        auto it = supported.find(type);
        if (it == supported.end() || std::find(it->second.begin(), it->second.end(), ifc_class) == it->second.end()) {
            throw std::invalid_argument("Resource type '" + type + "' does not support quantity type '" + ifc_class + "'");
        }
        const auto* declaration = file->schema()->declaration_by_name(ifc_class);
        if (!declaration || !declaration->is("IfcPhysicalSimpleQuantity")) throw std::invalid_argument("Invalid resource quantity class '" + ifc_class + "'");
        auto quantity = file->create(declaration);
        ifcapi::detail::write_string_attr(quantity, "Name", "Unnamed");
        if (ifc_class == "IfcQuantityCount") quantity.set_attribute_value(3, 0);
        else quantity.set_attribute_value(3, 0.0);
        auto old = ifcapi::detail::read_ref_attr(resource, "BaseQuantity");
        auto idx = ifcapi::detail::attr_index_of(resource, "BaseQuantity");
        if (idx < 0) throw std::runtime_error("Resource has no BaseQuantity attribute");
        ifcapi::detail::entity_view(resource).set(static_cast<size_t>(idx), quantity);
        if (old) entity_remove_deep(&old);
        return quantity;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

express::Base resource_add_resource_time(ifcopenshell::file* file, express::Base* resource_ptr) {
    ifcopenshell_clear_error();
    try {
        auto resource = ifcapi::detail::deref_or_empty(resource_ptr);
        require_owned(file, resource, "Resource");
        auto usage_idx = ifcapi::detail::attr_index_of(resource, "Usage");
        const auto* declaration = file->schema()->declaration_by_name("IfcResourceTime");
        if (usage_idx < 0 || !declaration) throw std::runtime_error("IfcResourceTime is not available in this schema");
        auto time = file->create(declaration);
        ifcapi::detail::entity_view(resource).set(static_cast<size_t>(usage_idx), time);
        return time;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

express::Base resource_assign_resource(ifcopenshell::file* file, const ResourceAssignmentOptions& options) {
    ifcopenshell_clear_error();
    try {
        require_owned(file, options.relating_resource, "Relating resource");
        require_owned(file, options.related_object, "Related object");
        express::Base rel;
        for (auto candidate : ifcapi::detail::read_inverse_aggregate(options.relating_resource, "ResourceOf")) {
            if (candidate.declaration().is("IfcRelAssignsToResource")) { rel = candidate; break; }
        }
        if (rel) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            for (auto object : related) if (ifcapi::detail::same_instance(object, options.related_object)) return rel;
            related.push_back(options.related_object);
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
            ifcapi::detail::update_owner_history(file, rel, options.user.value_or(express::Base()), options.application.value_or(express::Base()));
            return rel;
        }
        const auto* declaration = file->schema()->declaration_by_name("IfcRelAssignsToResource");
        rel = file->create(declaration);
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        auto history = ifcapi::detail::ensure_owner_history(file, options.owner_history.value_or(express::Base()),
            options.user.value_or(express::Base()), options.application.value_or(express::Base()));
        auto* entity = declaration->as_entity();
        ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"), history);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {options.related_object});
        ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingResource"), options.relating_resource);
        return rel;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

void resource_unassign_resource(ifcopenshell::file* file, const ResourceAssignmentOptions& options) {
    ifcopenshell_clear_error();
    try {
        require_owned(file, options.relating_resource, "Relating resource");
        require_owned(file, options.related_object, "Related object");
        for (auto rel : ifcapi::detail::read_inverse_aggregate(options.related_object, "HasAssignments")) {
            if (!rel.declaration().is("IfcRelAssignsToResource") ||
                !ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(rel, "RelatingResource"), options.relating_resource)) continue;
            remove_or_update_member(file, rel, "RelatedObjects", options.related_object,
                options.user.value_or(express::Base()), options.application.value_or(express::Base()));
            return;
        }
    } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); throw; }
}

void resource_remove_resource_quantity(ifcopenshell::file* file, express::Base* resource_ptr) {
    ifcopenshell_clear_error();
    try {
        auto resource = ifcapi::detail::deref_or_empty(resource_ptr);
        require_owned(file, resource, "Resource");
        auto old = ifcapi::detail::read_ref_attr(resource, "BaseQuantity");
        ifcapi::detail::clear_attr(resource, "BaseQuantity");
        if (old) entity_remove_deep(&old);
    } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); throw; }
}

void resource_calculate_resource_work(ifcopenshell::file* file, express::Base* resource_ptr) {
    ifcopenshell_clear_error();
    try {
        auto resource = ifcapi::detail::deref_or_empty(resource_ptr);
        require_owned(file, resource, "Resource");
        if (attribute_locked(resource, "Usage.ScheduleWork")) return;
        auto pset = productivity_pset(resource);
        if (!pset) return;
        auto consumed = pset_string(pset, "BaseQuantityConsumed");
        auto produced_name = pset_string(pset, "BaseQuantityProducedName");
        auto produced_value = pset_number(pset, "BaseQuantityProducedValue");
        if (!consumed || consumed->empty() || !produced_name || produced_name->empty() || !produced_value || *produced_value == 0.0) return;
        ifcapi::detail::sequence::Duration duration;
        if (!ifcapi::detail::sequence::parse_duration(*consumed, duration)) return;

        auto task = get_task_assignment(resource);
        if (!task) return;
        std::vector<express::Base> products;
        for (auto rel : ifcapi::detail::read_inverse_aggregate(task, "HasAssignments")) {
            if (rel && rel.declaration().is("IfcRelAssignsToProduct")) {
                auto product = ifcapi::detail::read_ref_attr(rel, "RelatingProduct");
                if (product) products.push_back(product);
            }
        }
        double total = 0.0;
        if (*produced_name == "Count") total = static_cast<double>(products.size());
        else for (auto product : products) total += first_product_quantity(product, *produced_name).value_or(0.0);
        if (total == 0.0) return;

        double required;
        std::string text;
        if (consumed->find('T') != std::string::npos) {
            double seconds = duration.days * 86400.0 + duration.seconds;
            required = total * seconds / *produced_value / 3600.0;
            text = "PT" + format_float(required) + "H";
        } else {
            double days = duration.days + duration.seconds / 86400.0;
            required = total * days / *produced_value;
            text = "P" + format_float(required) + "D";
        }
        auto usage = ifcapi::detail::read_ref_attr(resource, "Usage");
        if (!usage) usage = resource_add_resource_time(file, resource_ptr);
        if (!usage) throw std::runtime_error("Failed to create resource time");
        ifcapi::detail::write_string_attr(usage, "ScheduleWork", text);
    } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); throw; }
}

void resource_calculate_resource_usage(ifcopenshell::file* file, express::Base* resource_ptr) {
    ifcopenshell_clear_error();
    try {
        auto resource = ifcapi::detail::deref_or_empty(resource_ptr);
        require_owned(file, resource, "Resource");
        if (attribute_locked(resource, "Usage.ScheduleUsage")) return;
        auto usage = ifcapi::detail::read_ref_attr(resource, "Usage");
        auto work = usage ? ifcapi::detail::read_optional_string_attr(usage, "ScheduleWork") : ifcapi::detail::OptionalString{};
        if (!work.has_value || work.value.empty()) return;
        auto task = get_task_assignment(resource);
        auto task_time = task ? ifcapi::detail::read_ref_attr(task, "TaskTime") : express::Base();
        auto schedule = task_time ? ifcapi::detail::read_optional_string_attr(task_time, "ScheduleDuration") : ifcapi::detail::OptionalString{};
        if (!task_time || !schedule.has_value || schedule.value.empty()) return;
        ifcapi::detail::sequence::Duration task_duration;
        ifcapi::detail::sequence::Duration work_duration;
        if (!ifcapi::detail::sequence::parse_duration(schedule.value, task_duration)) throw std::invalid_argument("Malformed task ScheduleDuration");
        if (!ifcapi::detail::sequence::parse_duration(work.value, work_duration)) throw std::invalid_argument("Malformed resource ScheduleWork");
        auto duration_type = ifcapi::detail::read_optional_string_attr(task_time, "DurationType");
        double hours_per_day = !duration_type.has_value || duration_type.value == "WORKTIME" ? 8.0 : 24.0;
        double task_seconds = task_duration.days * hours_per_day * 3600.0 + task_duration.seconds;
        if (task_seconds == 0.0) throw std::domain_error("Task ScheduleDuration must be non-zero");
        double work_seconds = work_duration.days * 86400.0 + work_duration.seconds;
        ifcapi::detail::write_double_attr(usage, "ScheduleUsage", work_seconds / task_seconds);
    } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); throw; }
}

void resource_remove_resource(ifcopenshell::file* file, const ResourceRemoveResourceOptions& options) {
    ifcopenshell_clear_error();
    try {
        auto resource = options.resource;
        require_owned(file, resource, "Resource");
        auto user = options.user.value_or(express::Base());
        auto application = options.application.value_or(express::Base());

        std::vector<express::Base> children;
        for (auto inverse : get_inverse(file, resource)) {
            if (!inverse || !inverse.declaration().is("IfcRelNests") ||
                !ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingObject"), resource)) continue;
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            children.insert(children.end(), related.begin(), related.end());
            ifcapi::detail::remove_with_history(file, inverse);
        }
        for (auto child : children) resource_remove_resource(file, ResourceRemoveResourceOptions{child, options.user, options.application});

        auto inverses = get_inverse(file, resource);
        for (auto inverse : inverses) {
            if (!inverse) continue;
            if (inverse.declaration().is("IfcRelNests") || inverse.declaration().is("IfcRelAssignsToControl") ||
                inverse.declaration().is("IfcRelDeclares")) {
                const char* attr = inverse.declaration().is("IfcRelDeclares") ? "RelatedDefinitions" : "RelatedObjects";
                remove_or_update_member(file, inverse, attr, resource, user, application);
            } else if (inverse.declaration().is("IfcRelAssignsToResource")) {
                auto relating = ifcapi::detail::read_ref_attr(inverse, "RelatingResource");
                if (ifcapi::detail::same_instance(relating, resource)) ifcapi::detail::remove_with_history(file, inverse);
                else remove_or_update_member(file, inverse, "RelatedObjects", resource, user, application);
            }
        }
        auto usage = ifcapi::detail::read_ref_attr(resource, "Usage");
        if (usage) { ifcapi::detail::clear_attr(resource, "Usage"); file->remove_entity(usage); }
        auto quantity = ifcapi::detail::read_ref_attr(resource, "BaseQuantity");
        if (quantity) { ifcapi::detail::clear_attr(resource, "BaseQuantity"); entity_remove_deep(&quantity); }
        ifcapi::detail::remove_with_history(file, resource);
    } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); throw; }
}

void resource_edit_resource_time(ifcopenshell::file* file, express::Base* resource_time_ptr, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try {
        auto resource_time = ifcapi::detail::deref_or_empty(resource_time_ptr);
        if (!file || !resource_time) {
            throw std::runtime_error("resource_edit_resource_time requires a file and resource time");
        }
        auto resource = get_resource(file, resource_time);
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
                if (auto task = get_task_assignment(resource)) {
                    sequence_calculate_task_duration(file, &task);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        throw;
    }
}

void resource_edit_resource(
    ifcopenshell::file* file,
    express::Base resource,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, resource, attributes);
}

void resource_edit_resource_quantity(
    ifcopenshell::file* file,
    express::Base resource_quantity,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, resource_quantity, attributes);
}

} // namespace bindings
} // namespace ifcapi
