// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/constraint.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

std::vector<IfcUtil::IfcBaseClass*> mutable_entities(
    const std::vector<const IfcUtil::IfcBaseClass*>& entities)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(entities.size());
    for (auto* entity : entities) {
        if (entity) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(entity));
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> constraint_rels(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* constraint) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !constraint || constraint->id() <= 0) return result;
    auto inverses = file->getInverse(constraint->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* rel : *inverses) {
        if (rel && rel->declaration().is("IfcRelAssociatesConstraint")) {
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

IfcUtil::IfcBaseClass* create_rel_associates_constraint(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* constraint,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesConstraint");
    auto* rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel->set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingConstraint"), constraint);
    return rel;
}

void remove_null_constraint_rels(IfcParse::IfcFile* file) {
    auto rels = file->instances_by_type("IfcRelAssociatesConstraint");
    if (!rels) return;
    std::vector<IfcUtil::IfcBaseClass*> to_remove;
    for (auto* rel : *rels) {
        if (!ifcapi::detail::read_ref_attr(rel, "RelatingConstraint")) {
            to_remove.push_back(rel);
        }
    }
    for (auto* rel : to_remove) {
        ifcapi::detail::remove_with_history(file, rel);
    }
}

void delete_reference_chain(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* reference) {
    if (!reference) return;
    auto* inner = ifcapi::detail::read_ref_attr(reference, "InnerReference");
    if (inner) delete_reference_chain(file, inner);
    file->removeEntity(reference);
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* constraint_add_objective(IfcParse::IfcFile* file) {
    const auto* declaration = file->schema()->declaration_by_name("IfcObjective");
    auto* objective = file->create(declaration);
    detail::write_string_attr(objective, "Name", "Unnamed");
    detail::write_string_attr(objective, "ConstraintGrade", "NOTDEFINED");
    detail::write_string_attr(objective, "ObjectiveQualifier", "NOTDEFINED");
    return objective;
}

IfcUtil::IfcBaseClass* constraint_add_metric(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* objective)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcMetric");
    auto* metric = file->create(declaration);
    detail::write_string_attr(metric, "Name", "Unnamed");
    detail::write_string_attr(metric, "ConstraintGrade", "NOTDEFINED");
    detail::write_string_attr(metric, "Benchmark", "EQUALTO");
    if (objective) {
        auto values = detail::read_ref_aggregate(objective, "BenchmarkValues");
        values.push_back(metric);
        detail::write_ref_aggregate(objective, "BenchmarkValues", values);
    }
    return metric;
}

std::vector<IfcUtil::IfcBaseClass*> constraint_add_metric_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* metric,
    const std::string& reference_path)
{
    std::vector<IfcUtil::IfcBaseClass*> references;
    if (reference_path.empty()) return references;
    const auto* declaration = file->schema()->declaration_by_name("IfcReference");
    for (const auto& attribute : split_reference_path(reference_path)) {
        auto* reference = file->create(declaration);
        detail::write_string_attr(reference, "AttributeIdentifier", attribute);
        if (references.empty()) {
            detail::write_ref_attr(metric, "ReferencePath", reference);
        } else {
            detail::write_ref_attr(references.back(), "InnerReference", reference);
        }
        references.push_back(reference);
    }
    return references;
}

IfcUtil::IfcBaseClass* constraint_assign_constraint(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* constraint,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    if (product_vec.empty()) return nullptr;
    auto rels = constraint_rels(file, constraint);

    std::unordered_set<IfcUtil::IfcBaseClass*> related_objects;
    for (auto* rel : rels) {
        for (auto* object : detail::read_ref_aggregate(rel, "RelatedObjects")) {
            related_objects.insert(object);
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> products_to_assign;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_new;
    for (auto* product : product_vec) {
        if (related_objects.find(product) == related_objects.end() && seen_new.insert(product).second) {
            products_to_assign.push_back(product);
        }
    }
    if (products_to_assign.empty()) return rels.empty() ? nullptr : rels.front();

    auto* rel = rels.empty() ? nullptr : rels.front();
    if (!rel) {
        return create_rel_associates_constraint(file, products_to_assign, constraint, owner_history, user, application);
    }

    auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<IfcUtil::IfcBaseClass*> seen(related.begin(), related.end());
    for (auto* product : products_to_assign) {
        if (seen.insert(product).second) related.push_back(product);
    }
    detail::write_ref_aggregate(rel, "RelatedObjects", related);
    detail::update_owner_history(file, rel, user, application);
    return rel;
}

void constraint_unassign_constraint(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* constraint,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    if (product_vec.empty()) return;
    std::unordered_set<IfcUtil::IfcBaseClass*> products_set(product_vec.begin(), product_vec.end());

    for (auto* rel : constraint_rels(file, constraint)) {
        auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
        bool intersects = false;
        std::vector<IfcUtil::IfcBaseClass*> remaining;
        for (auto* object : related) {
            if (products_set.find(object) != products_set.end()) {
                intersects = true;
            } else {
                remaining.push_back(object);
            }
        }
        if (!intersects) continue;
        if (remaining.empty()) {
            detail::remove_with_history(file, rel);
        } else {
            detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            detail::update_owner_history(file, rel, user, application);
        }
    }
}

void constraint_remove_constraint(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* constraint)
{
    file->removeEntity(constraint);
    remove_null_constraint_rels(file);
}

void constraint_remove_metric(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* metric)
{
    delete_reference_chain(file, detail::read_ref_attr(metric, "ReferencePath"));
    file->removeEntity(metric);
    remove_null_constraint_rels(file);

    try {
        file->schema()->declaration_by_name("IfcResourceConstraintRelationship");
    } catch (...) {
        return;
    }
    auto resource_rels = file->instances_by_type("IfcResourceConstraintRelationship");
    if (!resource_rels) return;
    std::vector<IfcUtil::IfcBaseClass*> to_remove;
    for (auto* rel : *resource_rels) {
        if (!detail::read_ref_attr(rel, "RelatingConstraint")) {
            to_remove.push_back(rel);
        }
    }
    for (auto* rel : to_remove) {
        file->removeEntity(rel);
    }
}

} // namespace bindings
} // namespace ifcapi
