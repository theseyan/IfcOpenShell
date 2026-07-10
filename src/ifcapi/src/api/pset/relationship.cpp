// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool is_a(express::Base entity, const std::string& type_name) {
    if (!entity) return false;
    try {
        auto* schema = entity.declaration().schema();
        const auto* decl = schema ? schema->declaration_by_name(type_name) : nullptr;
        return decl && entity.declaration().is(*decl);
    } catch (...) {
        return false;
    }
}

std::string exact_class_name(express::Base entity) {
    return entity ? entity.declaration().name() : std::string();
}

bool contains_ptr(const std::vector<express::Base>& values, express::Base needle) {
    return std::find(values.begin(), values.end(), needle) != values.end();
}

void split_products(
    const std::vector<express::Base>& products,
    std::vector<express::Base>& occurrences,
    std::vector<express::Base>& types)
{
    for (auto product : products) {
        if (is_a(product, "IfcTypeProduct")) {
            types.push_back(product);
        } else {
            occurrences.push_back(product);
        }
    }
}

std::vector<express::Base> inverse_entities(express::Base entity, const std::string& attr) {
    return ifcapi::detail::read_inverse_aggregate(entity, attr.c_str());
}

std::vector<express::Base> defining_rels(ifcopenshell::file* file, express::Base pset) {
    const bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    return inverse_entities(pset, is_ifc2x3 ? "PropertyDefinitionOf" : "DefinesOccurrence");
}

std::vector<express::Base> pset_elements(ifcopenshell::file* file, express::Base pset) {
    std::vector<express::Base> result;
    std::set<express::Base> seen;
    for (auto rel : defining_rels(file, pset)) {
        for (auto obj : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (seen.insert(obj).second) result.push_back(obj);
        }
    }
    for (auto type : inverse_entities(pset, "DefinesType")) {
        if (seen.insert(type).second) result.push_back(type);
    }
    return result;
}

bool same_entity_set(
    const std::vector<express::Base>& lhs,
    const std::vector<express::Base>& rhs)
{
    return std::set<express::Base>(lhs.begin(), lhs.end()) == std::set<express::Base>(rhs.begin(), rhs.end());
}

size_t total_inverses(ifcopenshell::file* file, express::Base entity) {
    if (!file || !entity || entity.id() <= 0) return 0;
    try {
        return file->get_total_inverses(static_cast<int>(entity.id()));
    } catch (...) {
        return 0;
    }
}

express::Base create_rel_defines_by_properties(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base pset,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* rel_decl = file->schema()->declaration_by_name("IfcRelDefinesByProperties");
    auto rel = file->create(rel_decl);
    auto* rel_entity_decl = rel_decl->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "GlobalId");
    if (guid_idx >= 0) rel.set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "RelatingPropertyDefinition"), pset);
    return rel;
}

void append_type_pset(express::Base type, express::Base pset) {
    auto psets = ifcapi::detail::read_ref_aggregate(type, "HasPropertySets");
    psets.push_back(pset);
    ifcapi::detail::write_ref_aggregate(type, "HasPropertySets", psets);
}

void remove_type_pset(express::Base type, express::Base pset) {
    auto psets = ifcapi::detail::read_ref_aggregate(type, "HasPropertySets");
    auto it = std::find(psets.begin(), psets.end(), pset);
    if (it == psets.end()) {
        throw std::runtime_error("list.remove(x): x not in list");
    }
    psets.erase(it);
    if (psets.empty()) {
        ifcapi::detail::write_blank_attr(type, "HasPropertySets");
    } else {
        ifcapi::detail::write_ref_aggregate(type, "HasPropertySets", psets);
    }
}

void remove_owner_history_entity(ifcopenshell::file* file, express::Base entity) {
    auto history = ifcapi::detail::read_ref_attr(entity, "OwnerHistory");
    file->remove_entity(entity);
    if (history) ifcapi::bindings::entity_remove_deep(&history);
}

std::vector<express::Base> removable_properties(express::Base pset) {
    const std::string class_name = exact_class_name(pset);
    if (is_a(pset, "IfcPropertySet")) return ifcapi::detail::read_ref_aggregate(pset, "HasProperties");
    if (is_a(pset, "IfcQuantitySet")) return ifcapi::detail::read_ref_aggregate(pset, "Quantities");
    if (class_name == "IfcMaterialProperties" || class_name == "IfcProfileProperties") {
        return ifcapi::detail::read_ref_aggregate(pset, "Properties");
    }
    return {};
}

void purge_pset_payload(ifcopenshell::file* file, express::Base pset) {
    for (auto prop : removable_properties(pset)) {
        if (total_inverses(file, prop) != 1) continue;
        if (is_a(prop, "IfcPropertyEnumeratedValue")) {
            auto enumeration = ifcapi::detail::read_ref_attr(prop, "EnumerationReference");
            if (enumeration && total_inverses(file, enumeration) == 1) {
                file->remove_entity(enumeration);
            }
        }
        file->remove_entity(prop);
    }
    remove_owner_history_entity(file, pset);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base pset_assign_pset(
    ifcopenshell::file* file,
    const PsetAssignPsetOptions& options)
{
    auto pset = options.pset;
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    if (!file || !pset) throw std::runtime_error("Invalid pset assignment arguments");
    std::vector<express::Base> occurrences;
    std::vector<express::Base> types;
    split_products(options.products, occurrences, types);

    express::Base rel;
    if (!occurrences.empty()) {
        auto rels = defining_rels(file, pset);
        rel = rels.empty() ? express::Base() : rels.front();
        if (rel) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            for (auto occurrence : occurrences) {
                if (!contains_ptr(related, occurrence)) related.push_back(occurrence);
            }
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        } else {
            rel = create_rel_defines_by_properties(file, occurrences, pset, owner_history, user, application);
        }
    }

    for (auto type : types) {
        append_type_pset(type, pset);
    }
    return rel;
}

void pset_unassign_pset(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* pset_ptr)
{
    auto pset = ifcapi::detail::deref_or_empty(pset_ptr);
    if (!file || !pset) throw std::runtime_error("Invalid pset unassignment arguments");
    std::vector<express::Base> occurrences;
    std::vector<express::Base> types;
    split_products(products, occurrences, types);

    if (!occurrences.empty()) {
        for (auto rel : defining_rels(file, pset)) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            bool touches = false;
            for (auto occurrence : occurrences) {
                if (contains_ptr(related, occurrence)) {
                    touches = true;
                    break;
                }
            }
            if (!touches) continue;
            std::vector<express::Base> kept;
            for (auto item : related) {
                if (!contains_ptr(occurrences, item)) kept.push_back(item);
            }
            if (kept.empty()) {
                remove_owner_history_entity(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", kept);
            }
        }
    }

    for (auto type : types) {
        remove_type_pset(type, pset);
    }
}

void pset_remove_pset(
    ifcopenshell::file* file,
    express::Base* product_ptr,
    express::Base* pset_ptr)
{
    auto product = ifcapi::detail::deref_or_empty(product_ptr);
    auto pset = ifcapi::detail::deref_or_empty(pset_ptr);
    if (!file || !product || !pset) throw std::runtime_error("Invalid pset removal arguments");

    std::vector<express::Base> to_purge;
    bool should_remove_pset = true;
    auto inverses = file->instances_by_reference(static_cast<int>(pset.id()));
    {
        for (auto inverse : inverses) {
            if (!is_a(inverse, "IfcRelDefinesByProperties")) continue;
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            if (related.size() <= 1) {
                to_purge.push_back(inverse);
            } else {
                auto it = std::find(related.begin(), related.end(), product);
                if (it == related.end()) {
                    throw std::runtime_error("list.remove(x): x not in list");
                }
                related.erase(it);
                ifcapi::detail::write_ref_aggregate(inverse, "RelatedObjects", related);
                should_remove_pset = false;
            }
        }
    }

    if (should_remove_pset) {
        purge_pset_payload(file, pset);
    }
    for (auto rel : to_purge) {
        remove_owner_history_entity(file, rel);
    }
}

std::vector<express::Base> pset_unshare_pset(
    ifcopenshell::file* file,
    const PsetUnsharePsetOptions& options)
{
    auto pset = options.pset;
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    if (!file || !pset) throw std::runtime_error("Invalid pset unshare arguments");
    auto selected = options.products;
    if (selected.empty()) throw std::runtime_error("No products provided.");
    auto original_products = selected;
    if (same_entity_set(selected, pset_elements(file, pset))) {
        selected.erase(selected.begin());
    }
    if (selected.empty()) {
        throw std::runtime_error("Provided product is the only element to which pset is assigned.");
    }

    pset_unassign_pset(file, selected, &pset);

    std::vector<express::Base> result;
    result.reserve(selected.size());
    for (auto product : selected) {
        auto copy = ifcapi::detail::shallow_copy(file, pset);
        if (!copy) throw std::runtime_error("Failed to copy property set");
        result.push_back(copy);
        pset_assign_pset(
            file,
            PsetAssignPsetOptions{
                {product},
                copy,
                owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt,
            });
    }
    (void)original_products;
    return result;
}

} // namespace bindings
} // namespace ifcapi
