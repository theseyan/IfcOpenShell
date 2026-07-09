// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/project.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <set>
#include <vector>

namespace {

bool has_inverse_attr(express::Base entity, const char* attribute) {
    auto* declaration = entity ? entity.declaration().as_entity() : nullptr;
    if (!declaration) return false;
    for (auto* inverse : declaration->all_inverse_attributes()) {
        if (inverse && inverse->name() == attribute) return true;
    }
    return false;
}

express::Base first_has_context(express::Base definition) {
    auto rels = ifcapi::detail::read_inverse_aggregate(definition, "HasContext");
    return rels.empty() ? express::Base() : rels.front();
}

bool contains_ref(const std::vector<express::Base>& values, express::Base needle) {
    return std::find(values.begin(), values.end(), needle) != values.end();
}

std::vector<express::Base> declared_rels(express::Base context) {
    return ifcapi::detail::read_inverse_aggregate(context, "Declares");
}

express::Base create_rel_declares(
    ifcopenshell::file* file,
    express::Base relating_context,
    const std::vector<express::Base>& definitions,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelDeclares");
    auto rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel.set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedDefinitions"), definitions);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingContext"), relating_context);
    return rel;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base project_assign_declaration(
    ifcopenshell::file* file,
    const ProjectAssignDeclarationOptions& options)
{
    auto relating = options.relating_context;
    auto owner_history_value = options.owner_history.value_or(express::Base());
    auto user_value = options.user.value_or(express::Base());
    auto application_value = options.application.value_or(express::Base());
    auto all_declares = declared_rels(relating);

    std::vector<express::Base> previous_rels;
    std::vector<express::Base> objects_without_contexts;
    std::vector<express::Base> objects_with_contexts;
    std::set<express::Base> seen_definitions;
    std::set<express::Base> seen_previous_rels;

    for (auto definition : options.definitions) {
        if (!definition) continue;
        if (!seen_definitions.insert(definition).second) continue;
        if (!has_inverse_attr(definition, "HasContext")) continue;
        auto object_rel = first_has_context(definition);
        if (!object_rel) {
            objects_without_contexts.push_back(definition);
            continue;
        }
        if (!contains_ref(all_declares, object_rel)) {
            if (seen_previous_rels.insert(object_rel).second) previous_rels.push_back(object_rel);
            objects_with_contexts.push_back(definition);
        }
    }

    std::vector<express::Base> objects_to_change = objects_without_contexts;
    objects_to_change.insert(objects_to_change.end(), objects_with_contexts.begin(), objects_with_contexts.end());
    if (objects_to_change.empty()) return {};

    std::set<express::Base> objects_with_contexts_set(
        objects_with_contexts.begin(), objects_with_contexts.end());
    for (auto previous_rel : previous_rels) {
        std::vector<express::Base> related_definitions;
        for (auto definition : ifcapi::detail::read_ref_aggregate(previous_rel, "RelatedDefinitions")) {
            if (objects_with_contexts_set.find(definition) == objects_with_contexts_set.end()) {
                related_definitions.push_back(definition);
            }
        }
        if (related_definitions.empty()) {
            ifcapi::detail::remove_with_history(file, previous_rel);
        } else {
            ifcapi::detail::write_ref_aggregate(previous_rel, "RelatedDefinitions", related_definitions);
            ifcapi::detail::update_owner_history(file, previous_rel, user_value, application_value);
        }
    }

    auto declares = all_declares.empty() ? express::Base() : all_declares.front();
    if (!declares) {
        return create_rel_declares(
            file,
            relating,
            objects_to_change,
            owner_history_value,
            user_value,
            application_value);
    }

    auto related = ifcapi::detail::read_ref_aggregate(declares, "RelatedDefinitions");
    std::set<express::Base> seen(related.begin(), related.end());
    for (auto definition : objects_to_change) {
        if (seen.insert(definition).second) related.push_back(definition);
    }
    ifcapi::detail::write_ref_aggregate(declares, "RelatedDefinitions", related);
    ifcapi::detail::update_owner_history(file, declares, user_value, application_value);
    return declares;
}

void project_unassign_declaration(
    ifcopenshell::file* file,
    const ProjectUnassignDeclarationOptions& options)
{
    std::set<express::Base> definition_set;
    for (auto definition : options.definitions) {
        if (definition) definition_set.insert(definition);
    }
    auto user_value = options.user.value_or(express::Base());
    auto application_value = options.application.value_or(express::Base());
    std::vector<express::Base> rels;
    std::set<express::Base> seen_rels;
    for (auto definition : definition_set) {
        auto rel = first_has_context(definition);
        if (rel && seen_rels.insert(rel).second) rels.push_back(rel);
    }

    for (auto rel : rels) {
        std::vector<express::Base> related_definitions;
        for (auto definition : ifcapi::detail::read_ref_aggregate(rel, "RelatedDefinitions")) {
            if (definition_set.find(definition) == definition_set.end()) {
                related_definitions.push_back(definition);
            }
        }
        if (related_definitions.empty()) {
            ifcapi::detail::remove_with_history(file, rel);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, "RelatedDefinitions", related_definitions);
            ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
