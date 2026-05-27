// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/pset.h"
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

bool is_a(IfcUtil::IfcBaseClass* entity, const std::string& type_name) {
    if (!entity) return false;
    try {
        auto* schema = entity->declaration().schema();
        const auto* decl = schema ? schema->declaration_by_name(type_name) : nullptr;
        return decl && entity->declaration().is(*decl);
    } catch (...) {
        return false;
    }
}

std::string exact_class_name(IfcUtil::IfcBaseClass* entity) {
    return entity ? entity->declaration().name() : std::string();
}

std::vector<IfcUtil::IfcBaseClass*> mut_products(const std::vector<const IfcUtil::IfcBaseClass*>& products) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(products.size());
    for (auto* product : products) {
        if (product) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(product));
    }
    return result;
}

bool contains_ptr(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* needle) {
    return std::find(values.begin(), values.end(), needle) != values.end();
}

void split_products(
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    std::vector<IfcUtil::IfcBaseClass*>& occurrences,
    std::vector<IfcUtil::IfcBaseClass*>& types)
{
    for (auto* product : products) {
        if (is_a(product, "IfcTypeProduct")) {
            types.push_back(product);
        } else {
            occurrences.push_back(product);
        }
    }
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const std::string& attr) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return result;
    try {
        auto inverses = base->get_inverse(attr);
        if (inverses) {
            for (size_t i = 0; i < inverses->size(); ++i) {
                if ((*inverses)[i]) result.push_back((*inverses)[i]);
            }
        }
    } catch (...) {
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> defining_rels(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset) {
    const bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    return inverse_entities(pset, is_ifc2x3 ? "PropertyDefinitionOf" : "DefinesOccurrence");
}

std::vector<IfcUtil::IfcBaseClass*> pset_elements(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::set<IfcUtil::IfcBaseClass*> seen;
    for (auto* rel : defining_rels(file, pset)) {
        for (auto* obj : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (seen.insert(obj).second) result.push_back(obj);
        }
    }
    for (auto* type : inverse_entities(pset, "DefinesType")) {
        if (seen.insert(type).second) result.push_back(type);
    }
    return result;
}

bool same_entity_set(
    const std::vector<IfcUtil::IfcBaseClass*>& lhs,
    const std::vector<IfcUtil::IfcBaseClass*>& rhs)
{
    return std::set<IfcUtil::IfcBaseClass*>(lhs.begin(), lhs.end()) == std::set<IfcUtil::IfcBaseClass*>(rhs.begin(), rhs.end());
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity || entity->id() <= 0) return 0;
    try {
        return file->getTotalInverses(entity->id());
    } catch (...) {
        try {
            auto inverses = file->getInverse(entity->id(), nullptr, -1);
            return inverses ? inverses->size() : 0;
        } catch (...) {
            return 0;
        }
    }
}

IfcUtil::IfcBaseClass* create_rel_defines_by_properties(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* pset,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    const auto* rel_decl = file->schema()->declaration_by_name("IfcRelDefinesByProperties");
    auto* rel = file->create(rel_decl);
    auto* rel_entity_decl = rel_decl->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(rel_entity_decl, "GlobalId");
    if (guid_idx >= 0) rel->set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(rel_entity_decl, "RelatingPropertyDefinition"), pset);
    return rel;
}

void append_type_pset(IfcUtil::IfcBaseClass* type, IfcUtil::IfcBaseClass* pset) {
    auto psets = ifcapi::detail::read_ref_aggregate(type, "HasPropertySets");
    psets.push_back(pset);
    ifcapi::detail::write_ref_aggregate(type, "HasPropertySets", psets);
}

void remove_type_pset(IfcUtil::IfcBaseClass* type, IfcUtil::IfcBaseClass* pset) {
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

void remove_owner_history_entity(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    auto* history = ifcapi::detail::read_ref_attr(entity, "OwnerHistory");
    file->removeEntity(entity);
    if (history) ifcapi::bindings::entity_remove_deep2(history);
}

std::vector<IfcUtil::IfcBaseClass*> removable_properties(IfcUtil::IfcBaseClass* pset) {
    const std::string class_name = exact_class_name(pset);
    if (is_a(pset, "IfcPropertySet")) return ifcapi::detail::read_ref_aggregate(pset, "HasProperties");
    if (is_a(pset, "IfcQuantitySet")) return ifcapi::detail::read_ref_aggregate(pset, "Quantities");
    if (class_name == "IfcMaterialProperties" || class_name == "IfcProfileProperties") {
        return ifcapi::detail::read_ref_aggregate(pset, "Properties");
    }
    return {};
}

void purge_pset_payload(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* pset) {
    for (auto* prop : removable_properties(pset)) {
        if (total_inverses(file, prop) != 1) continue;
        if (is_a(prop, "IfcPropertyEnumeratedValue")) {
            auto* enumeration = ifcapi::detail::read_ref_attr(prop, "EnumerationReference");
            if (enumeration && total_inverses(file, enumeration) == 1) {
                file->removeEntity(enumeration);
            }
        }
        file->removeEntity(prop);
    }
    remove_owner_history_entity(file, pset);
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* pset_assign_pset(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* pset,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || !pset) throw std::runtime_error("Invalid pset assignment arguments");
    auto product_vec = mut_products(products);
    std::vector<IfcUtil::IfcBaseClass*> occurrences;
    std::vector<IfcUtil::IfcBaseClass*> types;
    split_products(product_vec, occurrences, types);

    IfcUtil::IfcBaseClass* rel = nullptr;
    if (!occurrences.empty()) {
        auto rels = defining_rels(file, pset);
        rel = rels.empty() ? nullptr : rels.front();
        if (rel) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            for (auto* occurrence : occurrences) {
                if (!contains_ptr(related, occurrence)) related.push_back(occurrence);
            }
            ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        } else {
            rel = create_rel_defines_by_properties(file, occurrences, pset, owner_history, user, application);
        }
    }

    for (auto* type : types) {
        append_type_pset(type, pset);
    }
    return rel;
}

void pset_unassign_pset(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* pset)
{
    if (!file || !pset) throw std::runtime_error("Invalid pset unassignment arguments");
    auto product_vec = mut_products(products);
    std::vector<IfcUtil::IfcBaseClass*> occurrences;
    std::vector<IfcUtil::IfcBaseClass*> types;
    split_products(product_vec, occurrences, types);

    if (!occurrences.empty()) {
        for (auto* rel : defining_rels(file, pset)) {
            auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
            bool touches = false;
            for (auto* occurrence : occurrences) {
                if (contains_ptr(related, occurrence)) {
                    touches = true;
                    break;
                }
            }
            if (!touches) continue;
            std::vector<IfcUtil::IfcBaseClass*> kept;
            for (auto* item : related) {
                if (!contains_ptr(occurrences, item)) kept.push_back(item);
            }
            if (kept.empty()) {
                remove_owner_history_entity(file, rel);
            } else {
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", kept);
            }
        }
    }

    for (auto* type : types) {
        remove_type_pset(type, pset);
    }
}

void pset_remove_pset(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* pset)
{
    if (!file || !product || !pset) throw std::runtime_error("Invalid pset removal arguments");

    std::vector<IfcUtil::IfcBaseClass*> to_purge;
    bool should_remove_pset = true;
    auto inverses = file->getInverse(pset->id(), nullptr, -1);
    if (inverses) {
        for (auto* inverse : *inverses) {
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
    for (auto* rel : to_purge) {
        remove_owner_history_entity(file, rel);
    }
}

std::vector<IfcUtil::IfcBaseClass*> pset_unshare_pset(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* pset,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || !pset) throw std::runtime_error("Invalid pset unshare arguments");
    auto selected = mut_products(products);
    if (selected.empty()) throw std::runtime_error("No products provided.");
    auto original_products = selected;
    if (same_entity_set(selected, pset_elements(file, pset))) {
        selected.erase(selected.begin());
    }
    if (selected.empty()) {
        throw std::runtime_error("Provided product is the only element to which pset is assigned.");
    }

    std::vector<const IfcUtil::IfcBaseClass*> selected_const(selected.begin(), selected.end());
    pset_unassign_pset(file, selected_const, pset);

    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(selected.size());
    for (auto* product : selected) {
        auto* copy = ifcapi::detail::shallow_copy(file, pset);
        if (!copy) throw std::runtime_error("Failed to copy property set");
        result.push_back(copy);
        std::vector<const IfcUtil::IfcBaseClass*> one{product};
        pset_assign_pset(file, one, copy, owner_history, user, application);
    }
    (void)original_products;
    return result;
}

} // namespace bindings
} // namespace ifcapi
