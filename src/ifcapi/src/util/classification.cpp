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

inline bool is_a(express::Base e, const char* name) {
    return e && e.declaration().is(name);
}

std::vector<express::Base> get_inverse(express::Base e, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(e, attr);
}

express::Base read_ref(express::Base e, const char* attr) {
    return ifcapi::detail::read_ref_attr(e, attr);
}

express::Base resolve_type(ifcopenshell::file* file, express::Base e) {
    if (!e) return {};
    if (is_a(e, "IfcTypeObject")) return e;
    bool is_ifc2x3 = file && file->schema() && file->schema()->name() == "IFC2X3";
    if (!is_ifc2x3) {
        auto inv = get_inverse(e, "IsTypedBy");
        if (!inv.empty()) {
            return read_ref(inv[0], "RelatingType");
        }
        return {};
    }
    auto inv = get_inverse(e, "IsDefinedBy");
    for (auto rel : inv) {
        if (is_a(rel, "IfcRelDefinesByType")) {
            return read_ref(rel, "RelatingType");
        }
    }
    return {};
}

express::Base get_classification(express::Base ref) {
    while (ref) {
        if (is_a(ref, "IfcClassification")) return ref;
        express::Base parent = read_ref(ref, "ReferencedSource");
        if (!parent) return {};
        ref = parent;
    }
    return {};
}

void collect_inherited(express::Base element, bool should_inherit,
                       std::vector<express::Base>& out,
                       std::unordered_set<express::Base>& seen);

// Returns true if element is non-IfcRoot and we used the external-reference shortcut.
bool try_external_refs(express::Base element,
                       std::vector<express::Base>& out,
                       std::unordered_set<express::Base>& seen) {
    if (is_a(element, "IfcRoot")) return false;
    auto refs = get_inverse(element, "HasExternalReferences");
    if (!refs.empty()) {
        for (auto rel : refs) {
            auto rr = read_ref(rel, "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    refs = get_inverse(element, "HasExternalReference");
    if (!refs.empty()) {
        for (auto rel : refs) {
            auto rr = read_ref(rel, "RelatingReference");
            if (rr && seen.insert(rr).second) out.push_back(rr);
        }
        return true;
    }
    return false;
}

void collect_for_element(express::Base element, bool should_inherit,
                         std::vector<express::Base>& out) {
    std::unordered_set<express::Base> seen;

    // Non-IfcRoot shortcut: HasExternalReferences / HasExternalReference.
    if (try_external_refs(element, out, seen)) return;

    // Inherited from type.
    std::vector<express::Base> inherited;
    auto* file = element ? element.file() : nullptr;
    if (should_inherit && is_a(element, "IfcObject")) {
        auto type_obj = resolve_type(file, element);
        if (type_obj && type_obj != element) {
            std::unordered_set<express::Base> _;
            collect_inherited(type_obj, /*should_inherit=*/true, inherited, _);
        }
    }

    // Occurrence references via HasAssociations.
    std::vector<express::Base> occurrence;
    {
        std::unordered_set<express::Base> occ_seen;
        auto rels = get_inverse(element, "HasAssociations");
        {
            for (auto rel : rels) {
                if (!is_a(rel, "IfcRelAssociatesClassification")) continue;
                auto rc = read_ref(rel, "RelatingClassification");
                if (rc && occ_seen.insert(rc).second) occurrence.push_back(rc);
            }
        }
    }

    if (!inherited.empty()) {
        // Group both lists by classification system; occurrence wins per system.
        std::unordered_map<express::Base, std::vector<express::Base>> per_system;
        std::vector<express::Base> system_order;
        auto add_to = [&](express::Base ref) {
            auto cls = get_classification(ref);
            auto it = per_system.find(cls);
            if (it == per_system.end()) {
                per_system.emplace(cls, std::vector<express::Base>{ref});
                system_order.push_back(cls);
            } else {
                it->second.push_back(ref);
            }
        };
        for (auto r : inherited) add_to(r);
        // Occurrence overrides per system.
        std::unordered_map<express::Base, std::vector<express::Base>> occ_per_system;
        std::vector<express::Base> occ_order;
        for (auto r : occurrence) {
            auto cls = get_classification(r);
            auto it = occ_per_system.find(cls);
            if (it == occ_per_system.end()) {
                occ_per_system.emplace(cls, std::vector<express::Base>{r});
                occ_order.push_back(cls);
            } else {
                it->second.push_back(r);
            }
        }
        for (auto cls : occ_order) {
            per_system[cls] = occ_per_system[cls];
            if (std::find(system_order.begin(), system_order.end(), cls) == system_order.end()) {
                system_order.push_back(cls);
            }
        }
        std::unordered_set<express::Base> dedup;
        for (auto cls : system_order) {
            for (auto v : per_system[cls]) {
                if (dedup.insert(v).second) out.push_back(v);
            }
        }
        return;
    }

    for (auto r : occurrence) out.push_back(r);
}

void collect_inherited(express::Base element, bool should_inherit,
                       std::vector<express::Base>& out,
                       std::unordered_set<express::Base>& /*seen*/) {
    // Mirrors collect_for_element on the type element.
    collect_for_element(element, should_inherit, out);
}

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

std::set<express::Base> referenced_elements(express::Base reference) {
    std::set<express::Base> result;
    auto refs = ifcapi::bindings::element_get_referenced_elements(&reference);
    for (auto item : refs) {
        if (item) {
            result.insert(item);
        }
    }
    return result;
}

bool products_are_subset_of_referenced(
    express::Base reference,
    const std::vector<express::Base>& products)
{
    auto referenced = referenced_elements(reference);
    for (auto product : products) {
        if (referenced.find(product) == referenced.end()) {
            return false;
        }
    }
    return true;
}

express::Base find_project(ifcopenshell::file* file) {
    auto projects = ifcapi::detail::instances_by_type(file, "IfcProject");
    if (projects.empty()) {
        throw std::runtime_error("No IfcProject found");
    }
    return projects.front();
}

void relate_classification_to_project(ifcopenshell::file* file, express::Base classification) {
    auto rel = file->create(file->schema()->declaration_by_name("IfcRelAssociatesClassification"));
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {find_project(file)});
    ifcapi::detail::write_ref_attr(rel, "RelatingClassification", classification);
}

express::Base create_rel_associates_classification(
    ifcopenshell::file* file,
    const std::vector<express::Base>& related_objects,
    express::Base relating_classification,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto rel = file->create(file->schema()->declaration_by_name("IfcRelAssociatesClassification"));
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related_objects);
    ifcapi::detail::write_ref_attr(rel, "RelatingClassification", relating_classification);
    return rel;
}

express::Base find_existing_reference(
    ifcopenshell::file* file,
    const ifcapi::detail::OptionalString& identification)
{
    const char* attr = is_ifc2x3(file) ? "ItemReference" : "Identification";
    for (auto reference : ifcapi::detail::instances_by_type(file, "IfcClassificationReference")) {
        if (ifcapi::detail::optional_string_attr_equals(reference, attr, identification)) {
            return reference;
        }
    }
    return {};
}

void split_products(
    const std::vector<express::Base>& products,
    std::set<express::Base>& rooted,
    std::set<express::Base>& non_rooted)
{
    for (auto product : products) {
        if (is_a(product, "IfcRoot")) {
            rooted.insert(product);
        } else {
            non_rooted.insert(product);
        }
    }
}

express::Base first_classification_root_rel(ifcopenshell::file* file, express::Base reference) {
    if (is_ifc2x3(file)) {
        for (auto rel : ifcapi::detail::instances_by_type(file, "IfcRelAssociatesClassification")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingClassification") == reference) {
                return rel;
            }
        }
        return {};
    }
    auto rels = ifcapi::detail::read_inverse_aggregate(reference, "ClassificationRefForObjects");
    return rels.empty() ? express::Base() : rels.front();
}

express::Base first_external_reference_rel(express::Base reference) {
    auto rels = ifcapi::detail::read_inverse_aggregate(reference, "ExternalReferenceForResources");
    return rels.empty() ? express::Base() : rels.front();
}

void update_reference_relationships(
    ifcopenshell::file* file,
    express::Base reference,
    const std::set<express::Base>& rooted_products,
    const std::set<express::Base>& non_rooted_products,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    if (!rooted_products.empty()) {
        auto root_rel = first_classification_root_rel(file, reference);
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
        auto non_root_rel = first_external_reference_rel(reference);
        if (non_root_rel) {
            auto related =
                ifcapi::detail::to_ref_set(ifcapi::detail::read_ref_aggregate(non_root_rel, "RelatedResourceObjects"));
            related.insert(non_rooted_products.begin(), non_rooted_products.end());
            ifcapi::detail::write_ref_aggregate(
                non_root_rel,
                "RelatedResourceObjects",
                ifcapi::detail::to_ref_vector(related));
        } else {
            auto rel = file->create(file->schema()->declaration_by_name("IfcExternalReferenceRelationship"));
            ifcapi::detail::write_ref_attr(rel, "RelatingReference", reference);
            ifcapi::detail::write_ref_aggregate(
                rel,
                "RelatedResourceObjects",
                ifcapi::detail::to_ref_vector(non_rooted_products));
        }
    }
}

std::set<express::Base> intersect_products(
    const std::vector<express::Base>& products,
    const std::set<express::Base>& referenced)
{
    std::set<express::Base> result;
    for (auto product : products) {
        if (referenced.find(product) != referenced.end()) {
            result.insert(product);
        }
    }
    return result;
}

std::set<express::Base> external_reference_rels_for_products(
    const std::set<express::Base>& products,
    express::Base reference)
{
    std::set<express::Base> result;
    for (auto product : products) {
        auto rels = ifcapi::detail::read_inverse_aggregate(product, "HasExternalReferences");
        if (rels.empty()) {
            rels = ifcapi::detail::read_inverse_aggregate(product, "HasExternalReference");
        }
        for (auto rel : rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingReference") == reference) {
                result.insert(rel);
            }
        }
    }
    return result;
}

std::vector<express::Base> classification_references(
    ifcopenshell::file* file,
    express::Base classification)
{
    std::vector<express::Base> result;
    if (is_ifc2x3(file)) {
        for (auto reference : ifcapi::detail::instances_by_type(file, "IfcClassificationReference")) {
            if (ifcapi::detail::read_ref_attr(reference, "ReferencedSource") == classification) {
                result.push_back(reference);
            }
        }
        return result;
    }
    for (auto reference : ifcapi::detail::read_inverse_aggregate(classification, "HasReferences")) {
        result.push_back(reference);
        auto children = classification_references(file, reference);
        result.insert(result.end(), children.begin(), children.end());
    }
    return result;
}

}  // namespace

namespace ifcapi {
namespace bindings {

std::vector<express::Base> classification_get_references(express::Base* element, bool should_inherit) {
    auto element_value = ifcapi::detail::deref_or_empty(element);
    if (!element_value) return {};

    std::vector<express::Base> items;
    collect_for_element(element_value, should_inherit, items);
    return items;
}

express::Base classification_add_classification(ifcopenshell::file* file, const std::string& name) {
    ifcopenshell_clear_error();
    try {
        auto classification = file->create(file->schema()->declaration_by_name("IfcClassification"));
        ifcapi::detail::write_string_attr(classification, "Name", name);
        relate_classification_to_project(file, classification);
        return classification;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
        return {};
    }
}

express::Base classification_add_reference(
    ifcopenshell::file* file,
    const ClassificationAddReferenceOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto reference_value = options.reference.value_or(express::Base());
        auto classification_value = options.classification.value_or(express::Base());
        auto owner_history_value = options.owner_history.value_or(express::Base());
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        auto product_vec = ifcapi::detail::to_mutable_refs(options.products);
        if (product_vec.empty()) {
            return {};
        }
        if (reference_value && products_are_subset_of_referenced(reference_value, product_vec)) {
            return reference_value;
        }

        std::set<express::Base> rooted_products;
        std::set<express::Base> non_rooted_products;
        split_products(product_vec, rooted_products, non_rooted_products);
        if (!non_rooted_products.empty() && is_ifc2x3(file)) {
            ifcapi::detail::set_error(
                ifcapi::detail::ERROR_TYPE,
                "Cannot add reference to non-IfcRoot element in IFC2X3.");
            return {};
        }

        if (!reference_value) {
            ifcapi::detail::OptionalString optional_identification{options.identification.has_value(), options.identification.value_or("")};
            reference_value = find_existing_reference(file, optional_identification);
            if (!reference_value) {
                reference_value = file->create(file->schema()->declaration_by_name("IfcClassificationReference"));
                ifcapi::detail::write_optional_string_attr(reference_value, "Name", {options.name.has_value(), options.name.value_or("")});
                ifcapi::detail::write_ref_attr(reference_value, "ReferencedSource", classification_value);
                ifcapi::detail::write_optional_string_attr(
                    reference_value,
                    is_ifc2x3(file) ? "ItemReference" : "Identification",
                    optional_identification);
            }
        }

        update_reference_relationships(
            file,
            reference_value,
            rooted_products,
            non_rooted_products,
            owner_history_value,
            user_value,
            application_value);
        return reference_value;
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
        return {};
    }
}

void classification_remove_reference(
    ifcopenshell::file* file,
    const ClassificationRemoveReferenceOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto reference_value = options.reference;
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        auto product_vec = ifcapi::detail::to_mutable_refs(options.products);
        auto products_to_remove = intersect_products(product_vec, referenced_elements(reference_value));
        if (products_to_remove.empty()) {
            return;
        }

        std::set<express::Base> rooted_products;
        std::set<express::Base> non_rooted_products;
        split_products(product_vec, rooted_products, non_rooted_products);
        if (!non_rooted_products.empty() && is_ifc2x3(file)) {
            ifcapi::detail::set_error(
                ifcapi::detail::ERROR_TYPE,
                "Cannot add reference to non-IfcRoot element in IFC2X3.");
            return;
        }

        if (!rooted_products.empty()) {
            std::set<express::Base> reference_rels;
            for (auto product : rooted_products) {
                for (auto rel : ifcapi::detail::read_inverse_aggregate(product, "HasAssociations")) {
                    if (is_a(rel, "IfcRelAssociatesClassification") &&
                        ifcapi::detail::read_ref_attr(rel, "RelatingClassification") == reference_value) {
                        reference_rels.insert(rel);
                    }
                }
            }
            for (auto rel : reference_rels) {
                auto related_objects = ifcapi::detail::to_ref_set(
                    ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"));
                for (auto product : rooted_products) {
                    related_objects.erase(product);
                }
                if (related_objects.empty()) {
                    ifcapi::detail::remove_with_history(file, rel);
                } else {
                    ifcapi::detail::write_ref_aggregate(
                        rel,
                        "RelatedObjects",
                        ifcapi::detail::to_ref_vector(related_objects));
                    ifcapi::detail::update_owner_history(file, rel, user_value, application_value);
                }
            }
        }

        for (auto rel : external_reference_rels_for_products(non_rooted_products, reference_value)) {
            auto related_objects = ifcapi::detail::to_ref_set(
                ifcapi::detail::read_ref_aggregate(rel, "RelatedResourceObjects"));
            for (auto product : non_rooted_products) {
                related_objects.erase(product);
            }
            if (related_objects.empty()) {
                file->remove_entity(rel);
            } else {
                ifcapi::detail::write_ref_aggregate(
                    rel,
                    "RelatedResourceObjects",
                    ifcapi::detail::to_ref_vector(related_objects));
            }
        }

        if (referenced_elements(reference_value).empty()) {
            file->remove_entity(reference_value);
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void classification_remove_classification(ifcopenshell::file* file, express::Base* classification) {
    ifcopenshell_clear_error();
    try {
        auto classification_value = ifcapi::detail::deref_or_empty(classification);
        for (auto reference : classification_references(file, classification_value)) {
            file->remove_entity(reference);
        }
        file->remove_entity(classification_value);

        for (auto rel : ifcapi::detail::instances_by_type(file, "IfcRelAssociatesClassification")) {
            if (!ifcapi::detail::read_ref_attr(rel, "RelatingClassification")) {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }

        if (!is_ifc2x3(file)) {
            for (auto rel : ifcapi::detail::instances_by_type(file, "IfcExternalReferenceRelationship")) {
                if (!ifcapi::detail::read_ref_attr(rel, "RelatingReference")) {
                    file->remove_entity(rel);
                }
            }
        }
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e.what());
    }
}

void classification_edit_classification(
    ifcopenshell::file* file,
    express::Base classification,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, classification, attributes);
}

void classification_edit_reference(
    ifcopenshell::file* file,
    express::Base reference,
    ifcopenshell_pset_props_t* attributes)
{
    detail::edit_named_attributes(file, reference, attributes);
}

} // namespace bindings
} // namespace ifcapi
