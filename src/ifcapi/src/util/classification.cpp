// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/classification.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

#include <algorithm>
#include <cstdlib>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

inline bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

aggregate_of_instance::ptr get_inverse(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    try {
        return be->get_inverse(attr);
    } catch (...) {
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    auto* d = be->declaration().as_entity();
    if (!d) return nullptr;
    int idx = d->attribute_index(attr);
    if (idx < 0) return nullptr;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)val;
    } catch (...) {
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* resolve_type(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* e) {
    if (!e) return nullptr;
    if (is_a(e, "IfcTypeObject")) return e;
    bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    if (!is_ifc2x3) {
        auto inv = get_inverse(e, "IsTypedBy");
        if (inv && inv->size() > 0) {
            return read_ref((*inv)[0], "RelatingType");
        }
        return nullptr;
    }
    auto inv = get_inverse(e, "IsDefinedBy");
    if (!inv) return nullptr;
    for (size_t i = 0; i < inv->size(); ++i) {
        if (is_a((*inv)[i], "IfcRelDefinesByType")) {
            return read_ref((*inv)[i], "RelatingType");
        }
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* get_classification(IfcUtil::IfcBaseClass* ref) {
    while (ref) {
        if (is_a(ref, "IfcClassification")) return ref;
        IfcUtil::IfcBaseClass* parent = read_ref(ref, "ReferencedSource");
        if (!parent) return nullptr;
        ref = parent;
    }
    return nullptr;
}

void collect_inherited(IfcUtil::IfcBaseClass* element, bool should_inherit,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& seen);

// Returns true if element is non-IfcRoot and we used the external-reference shortcut.
bool try_external_refs(IfcUtil::IfcBaseClass* element,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& seen) {
    if (is_a(element, "IfcRoot")) return false;
    if (auto refs = get_inverse(element, "HasExternalReferences")) {
        for (size_t i = 0; i < refs->size(); ++i) {
            auto* rr = read_ref((*refs)[i], "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    if (auto refs = get_inverse(element, "HasExternalReference")) {
        for (size_t i = 0; i < refs->size(); ++i) {
            auto* rr = read_ref((*refs)[i], "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    return false;
}

void collect_for_element(IfcUtil::IfcBaseClass* element, bool should_inherit,
                         std::vector<IfcUtil::IfcBaseClass*>& out) {
    std::unordered_set<IfcUtil::IfcBaseClass*> seen;

    // Non-IfcRoot shortcut: HasExternalReferences / HasExternalReference.
    if (try_external_refs(element, out, seen)) return;

    // Inherited from type.
    std::vector<IfcUtil::IfcBaseClass*> inherited;
    auto* file = element ? element->file_ : nullptr;
    if (should_inherit && is_a(element, "IfcObject")) {
        auto* type_obj = resolve_type(file, element);
        if (type_obj && type_obj != element) {
            std::unordered_set<IfcUtil::IfcBaseClass*> _;
            collect_inherited(type_obj, /*should_inherit=*/true, inherited, _);
        }
    }

    // Occurrence references via HasAssociations.
    std::vector<IfcUtil::IfcBaseClass*> occurrence;
    {
        std::unordered_set<IfcUtil::IfcBaseClass*> occ_seen;
        auto rels = get_inverse(element, "HasAssociations");
        if (rels) {
            for (size_t i = 0; i < rels->size(); ++i) {
                auto* rel = (*rels)[i];
                if (!is_a(rel, "IfcRelAssociatesClassification")) continue;
                auto* rc = read_ref(rel, "RelatingClassification");
                if (rc && occ_seen.insert(rc).second) occurrence.push_back(rc);
            }
        }
    }

    if (!inherited.empty()) {
        // Group both lists by classification system; occurrence wins per system.
        std::unordered_map<IfcUtil::IfcBaseClass*, std::vector<IfcUtil::IfcBaseClass*>> per_system;
        std::vector<IfcUtil::IfcBaseClass*> system_order;
        auto add_to = [&](IfcUtil::IfcBaseClass* ref) {
            auto* cls = get_classification(ref);
            auto it = per_system.find(cls);
            if (it == per_system.end()) {
                per_system.emplace(cls, std::vector<IfcUtil::IfcBaseClass*>{ref});
                system_order.push_back(cls);
            } else {
                it->second.push_back(ref);
            }
        };
        for (auto* r : inherited) add_to(r);
        // Occurrence overrides per system.
        std::unordered_map<IfcUtil::IfcBaseClass*, std::vector<IfcUtil::IfcBaseClass*>> occ_per_system;
        std::vector<IfcUtil::IfcBaseClass*> occ_order;
        for (auto* r : occurrence) {
            auto* cls = get_classification(r);
            auto it = occ_per_system.find(cls);
            if (it == occ_per_system.end()) {
                occ_per_system.emplace(cls, std::vector<IfcUtil::IfcBaseClass*>{r});
                occ_order.push_back(cls);
            } else {
                it->second.push_back(r);
            }
        }
        for (auto* cls : occ_order) {
            per_system[cls] = occ_per_system[cls];
            if (std::find(system_order.begin(), system_order.end(), cls) == system_order.end()) {
                system_order.push_back(cls);
            }
        }
        std::unordered_set<IfcUtil::IfcBaseClass*> dedup;
        for (auto* cls : system_order) {
            for (auto* v : per_system[cls]) {
                if (dedup.insert(v).second) out.push_back(v);
            }
        }
        return;
    }

    for (auto* r : occurrence) out.push_back(r);
}

void collect_inherited(IfcUtil::IfcBaseClass* element, bool should_inherit,
                       std::vector<IfcUtil::IfcBaseClass*>& out,
                       std::unordered_set<IfcUtil::IfcBaseClass*>& /*seen*/) {
    // Mirrors collect_for_element on the type element.
    collect_for_element(element, should_inherit, out);
}

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

std::set<IfcUtil::IfcBaseClass*> referenced_elements(IfcUtil::IfcBaseClass* reference) {
    std::set<IfcUtil::IfcBaseClass*> result;
    auto refs = ifcapi::bindings::element_get_referenced_elements(reference);
    if (!refs) {
        return result;
    }
    for (auto* item : *refs) {
        if (item) {
            result.insert(item);
        }
    }
    return result;
}

bool products_are_subset_of_referenced(
    IfcUtil::IfcBaseClass* reference,
    const std::vector<IfcUtil::IfcBaseClass*>& products)
{
    auto referenced = referenced_elements(reference);
    for (auto* product : products) {
        if (referenced.find(product) == referenced.end()) {
            return false;
        }
    }
    return true;
}

IfcUtil::IfcBaseClass* find_project(IfcParse::IfcFile* file) {
    auto projects = ifcapi::detail::instances_by_type(file, "IfcProject");
    if (projects.empty()) {
        throw std::runtime_error("No IfcProject found");
    }
    return projects.front();
}

void relate_classification_to_project(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* classification) {
    auto* rel = file->create(file->schema()->declaration_by_name("IfcRelAssociatesClassification"));
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {find_project(file)});
    ifcapi::detail::write_ref_attr(rel, "RelatingClassification", classification);
}

IfcUtil::IfcBaseClass* create_rel_associates_classification(
    IfcParse::IfcFile* file,
    const std::vector<IfcUtil::IfcBaseClass*>& related_objects,
    IfcUtil::IfcBaseClass* relating_classification,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = file->create(file->schema()->declaration_by_name("IfcRelAssociatesClassification"));
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
    ifcapi::detail::write_ref_attr(rel, "RelatingClassification", relating_classification);
    return rel;
}

IfcUtil::IfcBaseClass* find_existing_reference(
    IfcParse::IfcFile* file,
    const ifcapi::detail::OptionalString& identification)
{
    const char* attr = is_ifc2x3(file) ? "ItemReference" : "Identification";
    for (auto* reference : ifcapi::detail::instances_by_type(file, "IfcClassificationReference")) {
        if (ifcapi::detail::optional_string_attr_equals(reference, attr, identification)) {
            return reference;
        }
    }
    return nullptr;
}

void split_products(
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    std::set<IfcUtil::IfcBaseClass*>& rooted,
    std::set<IfcUtil::IfcBaseClass*>& non_rooted)
{
    for (auto* product : products) {
        if (is_a(product, "IfcRoot")) {
            rooted.insert(product);
        } else {
            non_rooted.insert(product);
        }
    }
}

IfcUtil::IfcBaseClass* first_classification_root_rel(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* reference) {
    if (is_ifc2x3(file)) {
        for (auto* rel : ifcapi::detail::instances_by_type(file, "IfcRelAssociatesClassification")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingClassification") == reference) {
                return rel;
            }
        }
        return nullptr;
    }
    auto rels = ifcapi::detail::read_inverse_aggregate(reference, "ClassificationRefForObjects");
    return rels.empty() ? nullptr : rels.front();
}

IfcUtil::IfcBaseClass* first_external_reference_rel(IfcUtil::IfcBaseClass* reference) {
    auto rels = ifcapi::detail::read_inverse_aggregate(reference, "ExternalReferenceForResources");
    return rels.empty() ? nullptr : rels.front();
}

void update_reference_relationships(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference,
    const std::set<IfcUtil::IfcBaseClass*>& rooted_products,
    const std::set<IfcUtil::IfcBaseClass*>& non_rooted_products,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!rooted_products.empty()) {
        auto* root_rel = first_classification_root_rel(file, reference);
        if (root_rel) {
            auto related = ifcapi::detail::to_ref_set(ifcapi::detail::read_ref_aggregate(root_rel, "RelatedObjects"));
            related.insert(rooted_products.begin(), rooted_products.end());
            ifcapi::detail::write_ref_aggregate(root_rel, "RelatedObjects", ifcapi::detail::to_ref_vector(related));
            ifcapi::detail::update_owner_history(file, root_rel, user, application);
        } else {
            create_rel_associates_classification(
                file,
                ifcapi::detail::to_ref_vector(rooted_products),
                reference,
                owner_history,
                user,
                application);
        }
    }

    if (!non_rooted_products.empty()) {
        auto* non_root_rel = first_external_reference_rel(reference);
        if (non_root_rel) {
            auto related =
                ifcapi::detail::to_ref_set(ifcapi::detail::read_ref_aggregate(non_root_rel, "RelatedResourceObjects"));
            related.insert(non_rooted_products.begin(), non_rooted_products.end());
            ifcapi::detail::write_ref_aggregate(
                non_root_rel,
                "RelatedResourceObjects",
                ifcapi::detail::to_ref_vector(related));
        } else {
            auto* rel = file->create(file->schema()->declaration_by_name("IfcExternalReferenceRelationship"));
            ifcapi::detail::write_ref_attr(rel, "RelatingReference", reference);
            ifcapi::detail::write_ref_aggregate(
                rel,
                "RelatedResourceObjects",
                ifcapi::detail::to_ref_vector(non_rooted_products));
        }
    }
}

std::set<IfcUtil::IfcBaseClass*> intersect_products(
    const std::vector<IfcUtil::IfcBaseClass*>& products,
    const std::set<IfcUtil::IfcBaseClass*>& referenced)
{
    std::set<IfcUtil::IfcBaseClass*> result;
    for (auto* product : products) {
        if (referenced.find(product) != referenced.end()) {
            result.insert(product);
        }
    }
    return result;
}

std::set<IfcUtil::IfcBaseClass*> external_reference_rels_for_products(
    const std::set<IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference)
{
    std::set<IfcUtil::IfcBaseClass*> result;
    for (auto* product : products) {
        auto rels = ifcapi::detail::read_inverse_aggregate(product, "HasExternalReferences");
        if (rels.empty()) {
            rels = ifcapi::detail::read_inverse_aggregate(product, "HasExternalReference");
        }
        for (auto* rel : rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingReference") == reference) {
                result.insert(rel);
            }
        }
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> classification_references(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* classification)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (is_ifc2x3(file)) {
        for (auto* reference : ifcapi::detail::instances_by_type(file, "IfcClassificationReference")) {
            if (ifcapi::detail::read_ref_attr(reference, "ReferencedSource") == classification) {
                result.push_back(reference);
            }
        }
        return result;
    }
    for (auto* reference : ifcapi::detail::read_inverse_aggregate(classification, "HasReferences")) {
        result.push_back(reference);
        auto children = classification_references(file, reference);
        result.insert(result.end(), children.begin(), children.end());
    }
    return result;
}

}  // namespace

namespace ifcapi {
namespace bindings {

aggregate_of_instance::ptr classification_get_references(IfcUtil::IfcBaseClass* element, bool should_inherit) {
    aggregate_of_instance::ptr result(new aggregate_of_instance);
    if (!element) return result;

    std::vector<IfcUtil::IfcBaseClass*> items;
    collect_for_element(element, should_inherit, items);
    for (auto* item : items) {
        if (item) result->push(item);
    }
    return result;
}

IfcUtil::IfcBaseClass* classification_add_classification(IfcParse::IfcFile* file, const std::string& name) {
    ifcopenshell_clear_error();
    try {
        auto* classification = file->create(file->schema()->declaration_by_name("IfcClassification"));
        ifcapi::detail::write_string_attr(classification, "Name", name);
        relate_classification_to_project(file, classification);
        return classification;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* classification_add_reference(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* reference,
    const std::string& identification,
    bool has_identification,
    const std::string& name,
    bool has_name,
    IfcUtil::IfcBaseClass* classification,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto product_vec = ifcapi::detail::to_mutable_refs(products);
        if (product_vec.empty()) {
            return nullptr;
        }
        if (reference && products_are_subset_of_referenced(reference, product_vec)) {
            return reference;
        }

        std::set<IfcUtil::IfcBaseClass*> rooted_products;
        std::set<IfcUtil::IfcBaseClass*> non_rooted_products;
        split_products(product_vec, rooted_products, non_rooted_products);
        if (!non_rooted_products.empty() && is_ifc2x3(file)) {
            ifcapi::detail::set_error(
                ifcapi::detail::ERROR_TYPE,
                "Cannot add reference to non-IfcRoot element in IFC2X3.");
            return nullptr;
        }

        if (!reference) {
            ifcapi::detail::OptionalString optional_identification{has_identification, identification};
            reference = find_existing_reference(file, optional_identification);
            if (!reference) {
                reference = file->create(file->schema()->declaration_by_name("IfcClassificationReference"));
                ifcapi::detail::write_optional_string_attr(reference, "Name", {has_name, name});
                ifcapi::detail::write_ref_attr(reference, "ReferencedSource", classification);
                ifcapi::detail::write_optional_string_attr(
                    reference,
                    is_ifc2x3(file) ? "ItemReference" : "Identification",
                    optional_identification);
            }
        }

        update_reference_relationships(
            file,
            reference,
            rooted_products,
            non_rooted_products,
            owner_history,
            user,
            application);
        return reference;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
        return nullptr;
    }
}

void classification_remove_reference(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* reference,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        auto product_vec = ifcapi::detail::to_mutable_refs(products);
        auto products_to_remove = intersect_products(product_vec, referenced_elements(reference));
        if (products_to_remove.empty()) {
            return;
        }

        std::set<IfcUtil::IfcBaseClass*> rooted_products;
        std::set<IfcUtil::IfcBaseClass*> non_rooted_products;
        split_products(product_vec, rooted_products, non_rooted_products);
        if (!non_rooted_products.empty() && is_ifc2x3(file)) {
            ifcapi::detail::set_error(
                ifcapi::detail::ERROR_TYPE,
                "Cannot add reference to non-IfcRoot element in IFC2X3.");
            return;
        }

        if (!rooted_products.empty()) {
            std::set<IfcUtil::IfcBaseClass*> reference_rels;
            for (auto* product : rooted_products) {
                for (auto* rel : ifcapi::detail::read_inverse_aggregate(product, "HasAssociations")) {
                    if (is_a(rel, "IfcRelAssociatesClassification") &&
                        ifcapi::detail::read_ref_attr(rel, "RelatingClassification") == reference) {
                        reference_rels.insert(rel);
                    }
                }
            }
            for (auto* rel : reference_rels) {
                auto related_objects = ifcapi::detail::to_ref_set(
                    ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"));
                for (auto* product : rooted_products) {
                    related_objects.erase(product);
                }
                if (related_objects.empty()) {
                    ifcapi::detail::remove_with_history(file, rel);
                } else {
                    ifcapi::detail::write_ref_aggregate(
                        rel,
                        "RelatedObjects",
                        ifcapi::detail::to_ref_vector(related_objects));
                    ifcapi::detail::update_owner_history(file, rel, user, application);
                }
            }
        }

        for (auto* rel : external_reference_rels_for_products(non_rooted_products, reference)) {
            auto related_objects = ifcapi::detail::to_ref_set(
                ifcapi::detail::read_ref_aggregate(rel, "RelatedResourceObjects"));
            for (auto* product : non_rooted_products) {
                related_objects.erase(product);
            }
            if (related_objects.empty()) {
                file->removeEntity(rel);
            } else {
                ifcapi::detail::write_ref_aggregate(
                    rel,
                    "RelatedResourceObjects",
                    ifcapi::detail::to_ref_vector(related_objects));
            }
        }

        if (referenced_elements(reference).empty()) {
            file->removeEntity(reference);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void classification_remove_classification(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* classification) {
    ifcopenshell_clear_error();
    try {
        for (auto* reference : classification_references(file, classification)) {
            file->removeEntity(reference);
        }
        file->removeEntity(classification);

        for (auto* rel : ifcapi::detail::instances_by_type(file, "IfcRelAssociatesClassification")) {
            if (!ifcapi::detail::read_ref_attr(rel, "RelatingClassification")) {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }

        if (!is_ifc2x3(file)) {
            for (auto* rel : ifcapi::detail::instances_by_type(file, "IfcExternalReferenceRelationship")) {
                if (!ifcapi::detail::read_ref_attr(rel, "RelatingReference")) {
                    file->removeEntity(rel);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
