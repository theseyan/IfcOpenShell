// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/project.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <algorithm>
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

bool has_inverse_attr(IfcUtil::IfcBaseClass* entity, const char* attribute) {
    auto* declaration = entity ? entity->declaration().as_entity() : nullptr;
    if (!declaration) return false;
    for (auto* inverse : declaration->all_inverse_attributes()) {
        if (inverse && inverse->name() == attribute) return true;
    }
    return false;
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const char* attribute) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return result;
    try {
        auto inverses = base->get_inverse(attribute);
        if (!inverses) return result;
        for (size_t i = 0; i < inverses->size(); ++i) {
            if ((*inverses)[i]) result.push_back((*inverses)[i]);
        }
    } catch (...) {
    }
    return result;
}

IfcUtil::IfcBaseClass* first_has_context(IfcUtil::IfcBaseClass* definition) {
    auto rels = inverse_entities(definition, "HasContext");
    return rels.empty() ? nullptr : rels.front();
}

bool contains_ptr(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* needle) {
    return std::find(values.begin(), values.end(), needle) != values.end();
}

std::vector<IfcUtil::IfcBaseClass*> declared_rels(IfcUtil::IfcBaseClass* context) {
    return inverse_entities(context, "Declares");
}

IfcUtil::IfcBaseClass* create_rel_declares(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_context,
    const std::vector<IfcUtil::IfcBaseClass*>& definitions,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelDeclares");
    auto* rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel->set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedDefinitions"), definitions);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingContext"), relating_context);
    return rel;
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* project_assign_declaration(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& definitions,
    IfcUtil::IfcBaseClass* relating_context,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto definition_vec = mutable_entities(definitions);
    auto all_declares = declared_rels(relating_context);

    std::vector<IfcUtil::IfcBaseClass*> previous_rels;
    std::vector<IfcUtil::IfcBaseClass*> objects_without_contexts;
    std::vector<IfcUtil::IfcBaseClass*> objects_with_contexts;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_definitions;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_previous_rels;

    for (auto* definition : definition_vec) {
        if (!seen_definitions.insert(definition).second) continue;
        if (!has_inverse_attr(definition, "HasContext")) continue;
        auto* object_rel = first_has_context(definition);
        if (!object_rel) {
            objects_without_contexts.push_back(definition);
            continue;
        }
        if (!contains_ptr(all_declares, object_rel)) {
            if (seen_previous_rels.insert(object_rel).second) previous_rels.push_back(object_rel);
            objects_with_contexts.push_back(definition);
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> objects_to_change = objects_without_contexts;
    objects_to_change.insert(objects_to_change.end(), objects_with_contexts.begin(), objects_with_contexts.end());
    if (objects_to_change.empty()) return nullptr;

    std::unordered_set<IfcUtil::IfcBaseClass*> objects_with_contexts_set(
        objects_with_contexts.begin(), objects_with_contexts.end());
    for (auto* previous_rel : previous_rels) {
        std::vector<IfcUtil::IfcBaseClass*> related_definitions;
        for (auto* definition : ifcapi::detail::read_ref_aggregate(previous_rel, "RelatedDefinitions")) {
            if (objects_with_contexts_set.find(definition) == objects_with_contexts_set.end()) {
                related_definitions.push_back(definition);
            }
        }
        if (related_definitions.empty()) {
            ifcapi::detail::remove_with_history(file, previous_rel);
        } else {
            ifcapi::detail::write_ref_aggregate(previous_rel, "RelatedDefinitions", related_definitions);
            ifcapi::detail::update_owner_history(file, previous_rel, user, application);
        }
    }

    auto* declares = all_declares.empty() ? nullptr : all_declares.front();
    if (!declares) {
        return create_rel_declares(file, relating_context, objects_to_change, owner_history, user, application);
    }

    auto related = ifcapi::detail::read_ref_aggregate(declares, "RelatedDefinitions");
    std::unordered_set<IfcUtil::IfcBaseClass*> seen(related.begin(), related.end());
    for (auto* definition : objects_to_change) {
        if (seen.insert(definition).second) related.push_back(definition);
    }
    ifcapi::detail::write_ref_aggregate(declares, "RelatedDefinitions", related);
    ifcapi::detail::update_owner_history(file, declares, user, application);
    return declares;
}

void project_unassign_declaration(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& definitions,
    IfcUtil::IfcBaseClass* /*relating_context*/,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto definition_vec = mutable_entities(definitions);
    std::unordered_set<IfcUtil::IfcBaseClass*> definition_set(definition_vec.begin(), definition_vec.end());
    std::vector<IfcUtil::IfcBaseClass*> rels;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_rels;
    for (auto* definition : definition_set) {
        auto* rel = first_has_context(definition);
        if (rel && seen_rels.insert(rel).second) rels.push_back(rel);
    }

    for (auto* rel : rels) {
        std::vector<IfcUtil::IfcBaseClass*> related_definitions;
        for (auto* definition : ifcapi::detail::read_ref_aggregate(rel, "RelatedDefinitions")) {
            if (definition_set.find(definition) == definition_set.end()) {
                related_definitions.push_back(definition);
            }
        }
        if (related_definitions.empty()) {
            ifcapi::detail::remove_with_history(file, rel);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, "RelatedDefinitions", related_definitions);
            ifcapi::detail::update_owner_history(file, rel, user, application);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
