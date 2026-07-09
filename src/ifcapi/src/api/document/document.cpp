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

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

std::string exact_class_name(express::Base entity) {
    return entity ? entity.declaration().name() : std::string();
}

express::Base first_project(ifcopenshell::file* file) {
    if (!file) return {};
    const auto* declaration = file->schema()->declaration_by_name("IfcProject");
    auto projects = file->instances_by_type(declaration);
    return projects.empty() ? express::Base() : projects.front();
}

std::vector<express::Base> mutable_entities(
    const std::vector<express::Base>& entities)
{
    std::vector<express::Base> result;
    result.reserve(entities.size());
    for (auto entity : entities) if (entity) result.push_back(entity);
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    std::vector<express::Base> result;
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

std::vector<express::Base> document_association_rels(
    ifcopenshell::file* file,
    express::Base document)
{
    std::vector<express::Base> result;
    if (!file || !document) return result;
    if (is_ifc2x3(file)) {
        auto rels = ifcapi::detail::instances_by_type(file, "IfcRelAssociatesDocument");
        for (auto rel : rels) {
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

std::vector<express::Base> referenced_elements(
    ifcopenshell::file* file,
    express::Base document)
{
    std::vector<express::Base> result;
    std::unordered_set<express::Base> seen;
    for (auto rel : document_association_rels(file, document)) {
        for (auto object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (object && seen.insert(object).second) result.push_back(object);
        }
    }
    return result;
}

std::vector<express::Base> products_not_already_referenced(
    const std::vector<express::Base>& products,
    const std::vector<express::Base>& referenced)
{
    std::unordered_set<express::Base> referenced_set(referenced.begin(), referenced.end());
    std::unordered_set<express::Base> seen;
    std::vector<express::Base> result;
    for (auto product : products) {
        if (product && referenced_set.find(product) == referenced_set.end() && seen.insert(product).second) {
            result.push_back(product);
        }
    }
    return result;
}

express::Base create_rel_associates_document(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base document,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesDocument");
    auto rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel.set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingDocument"), document);
    return rel;
}

express::Base create_document_information(ifcopenshell::file* file) {
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentInformation");
    auto information = file->create(declaration);
    ifcapi::detail::write_string_attr(information, is_ifc2x3(file) ? "DocumentId" : "Identification", "X");
    ifcapi::detail::write_string_attr(information, "Name", "Unnamed");
    return information;
}

express::Base create_document_information_relationship(
    ifcopenshell::file* file,
    express::Base parent,
    const std::vector<express::Base>& documents)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentInformationRelationship");
    auto rel = file->create(declaration);
    ifcapi::detail::write_ref_attr(rel, "RelatingDocument", parent);
    ifcapi::detail::write_ref_aggregate(rel, "RelatedDocuments", documents);
    return rel;
}

void append_unique(express::Base rel, const char* attribute, express::Base value) {
    auto values = ifcapi::detail::read_ref_aggregate(rel, attribute);
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(rel, attribute, values);
    }
}

std::vector<express::Base> reference_association_rels(
    ifcopenshell::file* file,
    express::Base reference)
{
    std::vector<express::Base> result;
    if (!file || !reference) return result;
    if (is_ifc2x3(file)) {
        if (reference.id() <= 0) return result;
        for (auto inverse : file->instances_by_reference(static_cast<int>(reference.id()))) {
            if (inverse && inverse.declaration().is("IfcRelAssociatesDocument")) {
                result.push_back(inverse);
            }
        }
        return result;
    }
    return inverse_entities(reference, "DocumentRefForObjects");
}

std::vector<express::Base> document_references(
    ifcopenshell::file* file,
    express::Base information)
{
    return is_ifc2x3(file)
        ? ifcapi::detail::read_ref_aggregate(information, "DocumentReferences")
        : inverse_entities(information, "HasDocumentReferences");
}

void remove_information_impl(ifcopenshell::file* file, express::Base information);

void remove_reference_impl(ifcopenshell::file* file, express::Base reference) {
    auto rels = reference_association_rels(file, reference);
    for (auto rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    file->remove_entity(reference);
}

void remove_information_impl(ifcopenshell::file* file, express::Base information) {
    for (auto reference : document_references(file, information)) {
        remove_reference_impl(file, reference);
    }

    auto pointer_rels = inverse_entities(information, "IsPointer");
    for (auto rel : pointer_rels) {
        auto related_documents = ifcapi::detail::read_ref_aggregate(rel, "RelatedDocuments");
        for (auto related : related_documents) {
            remove_information_impl(file, related);
        }
    }

    auto pointed_to_rels = inverse_entities(information, "IsPointedTo");
    for (auto rel : pointed_to_rels) {
        auto related_documents = ifcapi::detail::read_ref_aggregate(rel, "RelatedDocuments");
        if (related_documents.size() == 1 && related_documents[0] == information) {
            file->remove_entity(rel);
        }
    }

    auto rels = document_association_rels(file, information);
    for (auto rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
    file->remove_entity(information);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base document_add_information(
    ifcopenshell::file* file,
    const DocumentAddInformationOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    auto information = create_document_information(file);
    auto parent = options.parent.value_or(express::Base());
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

express::Base document_add_reference(
    ifcopenshell::file* file,
    std::optional<express::Base> information)
{
    auto information_value = information.value_or(express::Base());
    const auto* declaration = file->schema()->declaration_by_name("IfcDocumentReference");
    auto reference = file->create(declaration);
    if (is_ifc2x3(file)) {
        ifcapi::detail::write_string_attr(reference, "ItemReference", "X");
        if (information_value) {
            auto references = ifcapi::detail::read_ref_aggregate(information_value, "DocumentReferences");
            references.push_back(reference);
            ifcapi::detail::write_ref_aggregate(information_value, "DocumentReferences", references);
        }
    } else {
        ifcapi::detail::write_string_attr(reference, "Identification", "X");
        ifcapi::detail::write_ref_attr(reference, "ReferencedDocument", information_value);
    }
    return reference;
}

express::Base document_assign_document(
    ifcopenshell::file* file,
    const DocumentAssignDocumentOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    auto document = options.document;

    auto product_vec = mutable_entities(options.products);
    auto products_to_add = products_not_already_referenced(product_vec, referenced_elements(file, document));
    if (products_to_add.empty()) return {};

    auto rels = document_association_rels(file, document);
    auto rel = rels.empty() ? express::Base() : rels.front();
    if (!rel) {
        return create_rel_associates_document(file, products_to_add, document, owner_history, user, application);
    }

    auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<express::Base> seen(related.begin(), related.end());
    for (auto product : products_to_add) {
        if (seen.insert(product).second) related.push_back(product);
    }
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
    ifcapi::detail::update_owner_history(file, rel, user, application);
    return rel;
}

void document_unassign_document(
    ifcopenshell::file* file,
    const DocumentUnassignDocumentOptions& options)
{
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    auto document = options.document;

    auto product_vec = mutable_entities(options.products);
    std::unordered_set<express::Base> products_set(product_vec.begin(), product_vec.end());
    std::vector<express::Base> rels;
    std::unordered_set<express::Base> seen_rels;
    for (auto product : product_vec) {
        for (auto rel : inverse_entities(product, "HasAssociations")) {
            if (rel && rel.declaration().is("IfcRelAssociatesDocument")
                && ifcapi::detail::read_ref_attr(rel, "RelatingDocument") == document
                && seen_rels.insert(rel).second) {
                rels.push_back(rel);
            }
        }
    }

    for (auto rel : rels) {
        std::vector<express::Base> remaining;
        for (auto object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
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
    ifcopenshell::file* file,
    express::Base* reference)
{
    remove_reference_impl(file, detail::deref_or_empty(reference));
}

void document_remove_information(
    ifcopenshell::file* file,
    express::Base* information)
{
    remove_information_impl(file, detail::deref_or_empty(information));
}

} // namespace bindings
} // namespace ifcapi
