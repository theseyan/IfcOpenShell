// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/library.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

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

std::vector<IfcUtil::IfcBaseClass*> library_association_rels(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !reference) return result;
    if (is_ifc2x3(file)) {
        auto rels = file->instances_by_type("IfcRelAssociatesLibrary");
        if (!rels) return result;
        for (auto* rel : *rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingLibrary") == reference) {
                result.push_back(rel);
            }
        }
        return result;
    }
    return inverse_entities(reference, "LibraryRefForObjects");
}

std::vector<IfcUtil::IfcBaseClass*> referenced_elements(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;
    for (auto* rel : library_association_rels(file, reference)) {
        for (auto* object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (object && seen.insert(object).second) result.push_back(object);
        }
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> products_not_already_referenced(
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    const std::vector<IfcUtil::IfcBaseClass*>& referenced)
{
    std::unordered_set<IfcUtil::IfcBaseClass*> referenced_set(referenced.begin(), referenced.end());
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (auto* product : products) {
        if (product && referenced_set.find(product) == referenced_set.end() && seen.insert(product).second) {
            result.push_back(product);
        }
    }
    return result;
}

IfcUtil::IfcBaseClass* create_rel_associates_library(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesLibrary");
    auto* rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel->set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingLibrary"), reference);
    return rel;
}

void append_reference(IfcUtil::IfcBaseClass* library, IfcUtil::IfcBaseClass* reference) {
    auto references = ifcapi::detail::read_ref_aggregate(library, "LibraryReference");
    references.push_back(reference);
    ifcapi::detail::write_ref_aggregate(library, "LibraryReference", references);
}

void remove_library_reference_rels(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* reference) {
    auto rels = library_association_rels(file, reference);
    for (auto* rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* library_add_library(
    IfcParse::IfcFile* file,
    const std::string& name)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcLibraryInformation");
    auto* library = file->create(declaration);
    detail::write_string_attr(library, "Name", name);
    return library;
}

IfcUtil::IfcBaseClass* library_add_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* library)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcLibraryReference");
    auto* reference = file->create(declaration);
    if (is_ifc2x3(file)) {
        append_reference(library, reference);
    } else {
        detail::write_ref_attr(reference, "ReferencedLibrary", library);
    }
    return reference;
}

IfcUtil::IfcBaseClass* library_assign_reference(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    auto products_to_add = products_not_already_referenced(product_vec, referenced_elements(file, reference));
    if (products_to_add.empty()) return nullptr;

    auto rels = library_association_rels(file, reference);
    auto* rel = rels.empty() ? nullptr : rels.front();
    if (!rel) {
        return create_rel_associates_library(file, products_to_add, reference, owner_history, user, application);
    }

    auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<IfcUtil::IfcBaseClass*> seen(related.begin(), related.end());
    for (auto* product : products_to_add) {
        if (seen.insert(product).second) related.push_back(product);
    }
    detail::write_ref_aggregate(rel, "RelatedObjects", related);
    detail::update_owner_history(file, rel, user, application);
    return rel;
}

void library_unassign_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    std::unordered_set<IfcUtil::IfcBaseClass*> products_set(product_vec.begin(), product_vec.end());
    std::vector<IfcUtil::IfcBaseClass*> rels;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_rels;
    for (auto* product : product_vec) {
        for (auto* rel : inverse_entities(product, "HasAssociations")) {
            if (rel && rel->declaration().is("IfcRelAssociatesLibrary")
                && detail::read_ref_attr(rel, "RelatingLibrary") == reference
                && seen_rels.insert(rel).second) {
                rels.push_back(rel);
            }
        }
    }

    for (auto* rel : rels) {
        std::vector<IfcUtil::IfcBaseClass*> remaining;
        for (auto* object : detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (products_set.find(object) == products_set.end()) remaining.push_back(object);
        }
        if (remaining.empty()) {
            detail::remove_with_history(file, rel);
        } else {
            detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            detail::update_owner_history(file, rel, user, application);
        }
    }
}

void library_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference)
{
    remove_library_reference_rels(file, reference);
    file->removeEntity(reference);
}

void library_remove_library(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* library)
{
    std::vector<IfcUtil::IfcBaseClass*> references;
    std::vector<IfcUtil::IfcBaseClass*> rels;
    if (is_ifc2x3(file)) {
        references = detail::read_ref_aggregate(library, "LibraryReference");
        rels = library_association_rels(file, library);
        for (auto* reference : references) {
            auto reference_rels = library_association_rels(file, reference);
            rels.insert(rels.end(), reference_rels.begin(), reference_rels.end());
        }
    } else {
        references = inverse_entities(library, "HasLibraryReferences");
        for (auto* reference : references) {
            auto reference_rels = inverse_entities(reference, "LibraryRefForObjects");
            rels.insert(rels.end(), reference_rels.begin(), reference_rels.end());
        }
        auto library_rels = inverse_entities(library, "LibraryInfoForObjects");
        rels.insert(rels.end(), library_rels.begin(), library_rels.end());
    }

    for (auto* reference : references) {
        file->removeEntity(reference);
    }
    file->removeEntity(library);

    std::unordered_set<IfcUtil::IfcBaseClass*> seen;
    for (auto* rel : rels) {
        if (rel && seen.insert(rel).second) {
            detail::remove_with_history(file, rel);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
