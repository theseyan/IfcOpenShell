// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/document.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

std::string exact_class_name(IfcUtil::IfcBaseClass* entity) {
    return entity ? entity->declaration().name() : std::string();
}

IfcUtil::IfcBaseClass* first_project(IfcParse::IfcFile* file) {
    if (!file) return nullptr;
    const auto* declaration = file->schema()->declaration_by_name("IfcProject");
    auto projects = file->instances_by_type(declaration);
    return projects && projects->size() ? (*projects)[0] : nullptr;
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

std::vector<IfcUtil::IfcBaseClass*> document_association_rels(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* document)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !document) return result;
    if (is_ifc2x3(file)) {
        auto rels = file->instances_by_type("IfcRelAssociatesDocument");
        if (!rels) return result;
        for (auto* rel : *rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingDocument") == document) {
                result.push_back(rel);
            }
        }
        return result;
    }

    const std::string ifc_class = exact_class_name(document);
    if (ifc_class == "IfcDocumentReference") {
        return inverse_entities(document, "DocumentRefForObjects");
    }
    if (ifc_class == "IfcDocumentInformation") {
        return inverse_entities(document, "DocumentInfoForObjects");
    }
    throw std::runtime_error("Unexpected document type: " + ifc_class);
}

std::vector<IfcUtil::IfcBaseClass*> referenced_elements(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* document)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;
    for (auto* rel : document_association_rels(file, document)) {
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

IfcUtil::IfcBaseClass* create_rel_associates_document(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* document,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesDocument");
    auto* rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel->set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingDocument"), document);
    return rel;
}

IfcUtil::IfcBaseClass* create_document_information(IfcParse::IfcFile* file) {
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentInformation");
    auto* information = file->create(declaration);
    ifcapi::detail::write_string_attr(information, is_ifc2x3(file) ? "DocumentId" : "Identification", "X");
    ifcapi::detail::write_string_attr(information, "Name", "Unnamed");
    return information;
}

IfcUtil::IfcBaseClass* create_document_information_relationship(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    const std::vector<IfcUtil::IfcBaseClass*>& documents)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentInformationRelationship");
    auto* rel = file->create(declaration);
    ifcapi::detail::write_ref_attr(rel, "RelatingDocument", parent);
    ifcapi::detail::write_ref_aggregate(rel, "RelatedDocuments", documents);
    return rel;
}

void append_unique(IfcUtil::IfcBaseClass* rel, const char* attribute, IfcUtil::IfcBaseClass* value) {
    auto values = ifcapi::detail::read_ref_aggregate(rel, attribute);
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(rel, attribute, values);
    }
}

std::vector<IfcUtil::IfcBaseClass*> reference_association_rels(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !reference) return result;
    if (is_ifc2x3(file)) {
        if (reference->id() <= 0) return result;
        auto inverses = file->getInverse(reference->id(), nullptr, -1);
        if (!inverses) return result;
        for (auto* inverse : *inverses) {
            if (inverse && inverse->declaration().is("IfcRelAssociatesDocument")) {
                result.push_back(inverse);
            }
        }
        return result;
    }
    return inverse_entities(reference, "DocumentRefForObjects");
}

std::vector<IfcUtil::IfcBaseClass*> document_references(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* information)
{
    return is_ifc2x3(file)
        ? ifcapi::detail::read_ref_aggregate(information, "DocumentReferences")
        : inverse_entities(information, "HasDocumentReferences");
}

void remove_information_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* information);

void remove_reference_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* reference) {
    auto rels = reference_association_rels(file, reference);
    for (auto* rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    file->removeEntity(reference);
}

void remove_information_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* information) {
    for (auto* reference : document_references(file, information)) {
        remove_reference_impl(file, reference);
    }

    auto pointer_rels = inverse_entities(information, "IsPointer");
    for (auto* rel : pointer_rels) {
        auto related_documents = ifcapi::detail::read_ref_aggregate(rel, "RelatedDocuments");
        for (auto* related : related_documents) {
            remove_information_impl(file, related);
        }
    }

    auto pointed_to_rels = inverse_entities(information, "IsPointedTo");
    for (auto* rel : pointed_to_rels) {
        auto related_documents = ifcapi::detail::read_ref_aggregate(rel, "RelatedDocuments");
        if (related_documents.size() == 1 && related_documents[0] == information) {
            file->removeEntity(rel);
        }
    }

    auto rels = document_association_rels(file, information);
    for (auto* rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    file->removeEntity(information);
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* document_add_information(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* parent,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* information = create_document_information(file);
    if (!parent) parent = first_project(file);
    if (!parent) throw std::runtime_error("IfcProject is not found.");

    if (is_a(parent, "IfcProject") || is_a(parent, "IfcContext")) {
        create_rel_associates_document(file, {parent}, information, owner_history, user, application);
    } else if (is_a(parent, "IfcDocumentInformation")) {
        auto rels = inverse_entities(parent, "IsPointer");
        if (!rels.empty()) {
            append_unique(rels.front(), "RelatedDocuments", information);
        } else {
            create_document_information_relationship(file, parent, {information});
        }
    }
    return information;
}

IfcUtil::IfcBaseClass* document_add_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* information)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentReference");
    auto* reference = file->create(declaration);
    if (is_ifc2x3(file)) {
        ifcapi::detail::write_string_attr(reference, "ItemReference", "X");
        if (information) {
            auto references = ifcapi::detail::read_ref_aggregate(information, "DocumentReferences");
            references.push_back(reference);
            ifcapi::detail::write_ref_aggregate(information, "DocumentReferences", references);
        }
    } else {
        ifcapi::detail::write_string_attr(reference, "Identification", "X");
        ifcapi::detail::write_ref_attr(reference, "ReferencedDocument", information);
    }
    return reference;
}

IfcUtil::IfcBaseClass* document_assign_document(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* document,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    auto products_to_add = products_not_already_referenced(product_vec, referenced_elements(file, document));
    if (products_to_add.empty()) return nullptr;

    auto rels = document_association_rels(file, document);
    auto* rel = rels.empty() ? nullptr : rels.front();
    if (!rel) {
        return create_rel_associates_document(file, products_to_add, document, owner_history, user, application);
    }

    auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<IfcUtil::IfcBaseClass*> seen(related.begin(), related.end());
    for (auto* product : products_to_add) {
        if (seen.insert(product).second) related.push_back(product);
    }
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
    ifcapi::detail::update_owner_history(file, rel, user, application);
    return rel;
}

void document_unassign_document(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* document,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto product_vec = mutable_entities(products);
    std::unordered_set<IfcUtil::IfcBaseClass*> products_set(product_vec.begin(), product_vec.end());
    std::vector<IfcUtil::IfcBaseClass*> rels;
    std::unordered_set<IfcUtil::IfcBaseClass*> seen_rels;
    for (auto* product : product_vec) {
        for (auto* rel : inverse_entities(product, "HasAssociations")) {
            if (rel && rel->declaration().is("IfcRelAssociatesDocument")
                && ifcapi::detail::read_ref_attr(rel, "RelatingDocument") == document
                && seen_rels.insert(rel).second) {
                rels.push_back(rel);
            }
        }
    }

    for (auto* rel : rels) {
        std::vector<IfcUtil::IfcBaseClass*> remaining;
        for (auto* object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (products_set.find(object) == products_set.end()) remaining.push_back(object);
        }
        if (remaining.empty()) {
            ifcapi::detail::remove_with_history(file, rel);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            ifcapi::detail::update_owner_history(file, rel, user, application);
        }
    }
}

void document_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference)
{
    remove_reference_impl(file, reference);
}

void document_remove_information(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* information)
{
    remove_information_impl(file, information);
}

} // namespace bindings
} // namespace ifcapi
