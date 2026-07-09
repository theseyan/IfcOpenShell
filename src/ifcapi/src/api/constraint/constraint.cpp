// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/constraint.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

std::vector<express::Base> constraint_rels(ifcopenshell::file* file, express::Base constraint) {
    std::vector<express::Base> result;
    if (!file || !constraint || constraint.id() <= 0) {
        return result;
    }
    for (auto rel : file->instances_by_reference(static_cast<int>(constraint.id()))) {
        if (rel && rel.declaration().is("IfcRelAssociatesConstraint")) {
            result.push_back(rel);
        }
    }
    return result;
}

std::vector<std::string> split_reference_path(const std::string& path) {
    std::vector<std::string> result;
    std::stringstream stream(path);
    std::string token;
    while (std::getline(stream, token, '.')) {
        result.push_back(token);
    }
    return result;
}

express::Base create_rel_associates_constraint(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base constraint,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesConstraint");
    auto rel = file->create(declaration);
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", products);
    ifcapi::detail::write_ref_attr(rel, "RelatingConstraint", constraint);
    return rel;
}

void remove_null_constraint_rels(ifcopenshell::file* file) {
    auto* declaration = ifcapi::detail::declaration_by_name(file, "IfcRelAssociatesConstraint");
    if (!declaration) {
        return;
    }
    std::vector<express::Base> to_remove;
    for (auto rel : file->instances_by_type(declaration)) {
        if (!ifcapi::detail::read_ref_attr(rel, "RelatingConstraint")) {
            to_remove.push_back(rel);
        }
    }
    for (auto rel : to_remove) {
        ifcapi::detail::remove_with_history(file, rel);
    }
}

void delete_reference_chain(ifcopenshell::file* file, express::Base reference) {
    if (!reference) {
        return;
    }
    auto inner = ifcapi::detail::read_ref_attr(reference, "InnerReference");
    if (inner) {
        delete_reference_chain(file, inner);
    }
    file->remove_entity(reference);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base constraint_add_objective(ifcopenshell::file* file) {
    const auto* declaration = file->schema()->declaration_by_name("IfcObjective");
    auto objective = file->create(declaration);
    detail::write_string_attr(objective, "Name", "Unnamed");
    detail::write_string_attr(objective, "ConstraintGrade", "NOTDEFINED");
    detail::write_string_attr(objective, "ObjectiveQualifier", "NOTDEFINED");
    return objective;
}

express::Base constraint_add_metric(ifcopenshell::file* file, std::optional<express::Base> objective) {
    const auto* declaration = file->schema()->declaration_by_name("IfcMetric");
    auto metric = file->create(declaration);
    detail::write_string_attr(metric, "Name", "Unnamed");
    detail::write_string_attr(metric, "ConstraintGrade", "NOTDEFINED");
    detail::write_string_attr(metric, "Benchmark", "EQUALTO");
    auto objective_value = objective.value_or(express::Base());
    if (objective_value) {
        auto values = detail::read_ref_aggregate(objective_value, "BenchmarkValues");
        values.push_back(metric);
        detail::write_ref_aggregate(objective_value, "BenchmarkValues", values);
    }
    return metric;
}

std::vector<express::Base> constraint_add_metric_reference(
    ifcopenshell::file* file,
    express::Base* metric,
    const std::string& reference_path)
{
    std::vector<express::Base> references;
    auto metric_value = detail::deref_or_empty(metric);
    if (reference_path.empty() || !metric_value) {
        return references;
    }
    const auto* declaration = file->schema()->declaration_by_name("IfcReference");
    for (const auto& attribute : split_reference_path(reference_path)) {
        auto reference = file->create(declaration);
        detail::write_string_attr(reference, "AttributeIdentifier", attribute);
        if (references.empty()) {
            detail::write_ref_attr(metric_value, "ReferencePath", reference);
        } else {
            detail::write_ref_attr(references.back(), "InnerReference", reference);
        }
        references.push_back(reference);
    }
    return references;
}

express::Base constraint_assign_constraint(
    ifcopenshell::file* file,
    const ConstraintAssignConstraintOptions& options)
{
    std::vector<express::Base> product_vec;
    for (auto product : options.products) {
        if (product) {
            product_vec.push_back(product);
        }
    }
    auto constraint_value = options.constraint;
    if (product_vec.empty() || !constraint_value) {
        return {};
    }
    auto rels = constraint_rels(file, constraint_value);

    std::unordered_set<express::Base> related_objects;
    for (auto rel : rels) {
        for (auto object : detail::read_ref_aggregate(rel, "RelatedObjects")) {
            related_objects.insert(object);
        }
    }

    std::vector<express::Base> products_to_assign;
    std::unordered_set<express::Base> seen_new;
    for (auto product : product_vec) {
        if (related_objects.find(product) == related_objects.end() && seen_new.insert(product).second) {
            products_to_assign.push_back(product);
        }
    }
    if (products_to_assign.empty()) {
        return rels.empty() ? express::Base() : rels.front();
    }

    auto rel = rels.empty() ? express::Base() : rels.front();
    if (!rel) {
        return create_rel_associates_constraint(
            file,
            products_to_assign,
            constraint_value,
            options.owner_history.value_or(express::Base()),
            options.user.value_or(express::Base()),
            options.application.value_or(express::Base()));
    }

    auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<express::Base> seen(related.begin(), related.end());
    for (auto product : products_to_assign) {
        if (seen.insert(product).second) {
            related.push_back(product);
        }
    }
    detail::write_ref_aggregate(rel, "RelatedObjects", related);
    detail::update_owner_history(file, rel, options.user.value_or(express::Base()), options.application.value_or(express::Base()));
    return rel;
}

void constraint_unassign_constraint(
    ifcopenshell::file* file,
    const ConstraintUnassignConstraintOptions& options)
{
    std::unordered_set<express::Base> products_set;
    for (auto product : options.products) {
        if (product) {
            products_set.insert(product);
        }
    }
    auto constraint_value = options.constraint;
    if (products_set.empty() || !constraint_value) {
        return;
    }

    for (auto rel : constraint_rels(file, constraint_value)) {
        auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
        bool intersects = false;
        std::vector<express::Base> remaining;
        for (auto object : related) {
            if (products_set.find(object) != products_set.end()) {
                intersects = true;
            } else {
                remaining.push_back(object);
            }
        }
        if (!intersects) {
            continue;
        }
        if (remaining.empty()) {
            detail::remove_with_history(file, rel);
        } else {
            detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            detail::update_owner_history(file, rel, options.user.value_or(express::Base()), options.application.value_or(express::Base()));
        }
    }
}

void constraint_remove_constraint(ifcopenshell::file* file, express::Base* constraint) {
    auto constraint_value = detail::deref_or_empty(constraint);
    if (file && constraint_value) {
        file->remove_entity(constraint_value);
    }
    remove_null_constraint_rels(file);
}

void constraint_remove_metric(ifcopenshell::file* file, express::Base* metric) {
    auto metric_value = detail::deref_or_empty(metric);
    if (!file || !metric_value) {
        return;
    }
    delete_reference_chain(file, detail::read_ref_attr(metric_value, "ReferencePath"));
    file->remove_entity(metric_value);
    remove_null_constraint_rels(file);

    auto* declaration = detail::declaration_by_name(file, "IfcResourceConstraintRelationship");
    if (!declaration) {
        return;
    }
    std::vector<express::Base> to_remove;
    for (auto rel : file->instances_by_type(declaration)) {
        if (!detail::read_ref_attr(rel, "RelatingConstraint")) {
            to_remove.push_back(rel);
        }
    }
    for (auto rel : to_remove) {
        file->remove_entity(rel);
    }
}

} // namespace bindings
} // namespace ifcapi
