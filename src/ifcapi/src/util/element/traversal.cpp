// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/element.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "ifcapi/detail/attribute.h"

#include "entity_introspection.hpp"

namespace {

inline bool is_a(const express::Base& e, const char* type) {
    return ifcapi::entity_is_a(e, type);
}

std::vector<express::Base> inverse_attr(const express::Base& e, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(e, attr);
}

express::Base read_ref(const express::Base& e, const char* attr) {
    return ifcapi::get_entity_ref(e, attr);
}

std::vector<express::Base> read_ref_list(const express::Base& e, const char* attr) {
    return ifcapi::get_entity_list(e, attr);
}

// Read a list of entity references via either a forward aggregate attribute or
// an inverse attribute (whichever is defined on the entity for `attr`).
std::vector<express::Base> read_any_list(const express::Base& e, const char* attr) {
    auto out = read_ref_list(e, attr);
    if (!out.empty()) return out;
    return inverse_attr(e, attr);
}

bool is_ifc2x3(ifcopenshell::file* f) {
    return f && f->schema() && f->schema()->name() == "IFC2X3";
}

int32_t entity_id(const express::Base& e) {
    return e ? static_cast<int32_t>(e.id()) : 0;
}

std::vector<express::Base> make_unique_instance_list(const std::vector<express::Base>& items) {
    std::vector<express::Base> unique;
    std::unordered_set<int32_t> seen;
    for (const auto& item : items) {
        if (!item) continue;
        int32_t id = entity_id(item);
        if (id == 0 || seen.insert(id).second) unique.push_back(item);
    }
    return unique;
}

// Forward declaration for mutual recursion.
void collect_elements_by_material(ifcopenshell::file* f, const express::Base& mat,
                                  std::unordered_set<int32_t>& seen,
                                  std::vector<express::Base>& out);

void collect_elements_by_representation(ifcopenshell::file* f, const express::Base& rep,
                                        std::unordered_set<int32_t>& seen,
                                        std::vector<express::Base>& out);

void add_unique(const express::Base& e,
                std::unordered_set<int32_t>& seen,
                std::vector<express::Base>& out)
{
    if (!e) return;
    int32_t id = entity_id(e);
    if (id && seen.insert(id).second) out.push_back(e);
}

void collect_elements_by_representation(ifcopenshell::file* f, const express::Base& rep,
                                        std::unordered_set<int32_t>& seen,
                                        std::vector<express::Base>& out)
{
    if (!rep) return;
    for (auto pr : read_any_list(rep, "OfProductRepresentation")) {
        for (auto prod : read_any_list(pr, "ShapeOfProduct")) {
            add_unique(prod, seen, out);
        }
    }
    for (auto rmap : read_any_list(rep, "RepresentationMap")) {
        auto inv = ifcapi::get_all_inverses(f, rmap);
        for (auto it : inv) {
            if (is_a(it, "IfcTypeProduct")) {
                add_unique(it, seen, out);
            } else if (is_a(it, "IfcMappedItem")) {
                auto inv2 = ifcapi::get_all_inverses(f, it);
                for (auto sr : inv2) {
                    if (is_a(sr, "IfcShapeRepresentation")) {
                        collect_elements_by_representation(f, sr, seen, out);
                    }
                }
            }
        }
    }
}

void collect_elements_by_material(ifcopenshell::file* f, const express::Base& mat,
                                  std::unordered_set<int32_t>& seen,
                                  std::vector<express::Base>& out)
{
    if (!mat) return;
    for (auto inv : ifcapi::get_all_inverses(f, mat)) {
        if (is_a(inv, "IfcRelAssociatesMaterial")) {
            for (auto obj : read_ref_list(inv, "RelatedObjects")) add_unique(obj, seen, out);
        } else if (is_a(inv, "IfcMaterialLayer")) {
            for (auto set_ : read_any_list(inv, "ToMaterialLayerSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialProfile")) {
            for (auto set_ : read_any_list(inv, "ToMaterialProfileSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialConstituent")) {
            for (auto set_ : read_any_list(inv, "ToMaterialConstituentSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialLayerSetUsage") ||
                   is_a(inv, "IfcMaterialProfileSetUsage") ||
                   is_a(inv, "IfcMaterialList")) {
            collect_elements_by_material(f, inv, seen, out);
        }
    }
}

void collect_openings(const express::Base& e,
                      std::unordered_set<int32_t>& seen,
                      std::vector<express::Base>& out,
                      ifcopenshell::file* file)
{
    if (!e) return;
    auto inv = inverse_attr(e, "HasOpenings");
    for (auto rel : inv) {
        int32_t id = entity_id(rel);
        if (id && seen.insert(id).second) out.push_back(rel);
    }
    // Recurse into aggregate parent.
    auto decomposes = inverse_attr(e, "Decomposes");
    if (!decomposes.empty()) {
        auto rel = decomposes.front();
        bool ok = true;
        if (is_ifc2x3(file) && !is_a(rel, "IfcRelAggregates")) ok = false;
        if (ok) {
            auto parent = read_ref(rel, "RelatingObject");
            if (parent) collect_openings(parent, seen, out, file);
        }
    }
}

bool is_userdefined_predefined_type(const express::Base& e) {
    if (!e) return false;
    std::string pt = ifcapi::get_string_attr(e, "PredefinedType");
    if (pt == "USERDEFINED") return true;
    if (pt.empty()) {
        // Check ElementType / ProcessType depending on entity.
        if (ifcapi::has_attr(e, "ElementType")) {
            auto et = ifcapi::get_string_attr(e, "ElementType");
            if (!et.empty()) return true;
        }
        if (ifcapi::has_attr(e, "ProcessType")) {
            auto pt2 = ifcapi::get_string_attr(e, "ProcessType");
            if (!pt2.empty()) return true;
        }
    }
    return false;
}

}  // namespace

namespace ifcapi {
namespace bindings {

std::vector<express::Base> element_get_types(express::Base* type_element) {
    auto type_value = detail::deref_or_empty(type_element);
    if (!type_value) return {};
    auto* f = type_value.file();
    const char* attr = is_ifc2x3(f) ? "ObjectTypeOf" : "Types";
    auto inv = inverse_attr(type_value, attr);
    if (inv.empty()) return {};
    return read_ref_list(inv.front(), "RelatedObjects");
}

std::vector<express::Base> element_get_shape_aspects(
    express::Base* element,
    const ElementGetShapeAspectsOptions& options)
{
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto* f = element_value.file();
    bool should_inherit = options.should_inherit.value_or(true);

    std::vector<express::Base> result;

    if (ifcapi::has_attr(element_value, "Representation")) {
        if (should_inherit) {
            auto type_e = element_get_type(&element_value);
            if (type_e) {
                auto type_value = *type_e;
                ElementGetShapeAspectsOptions inherited_options;
                inherited_options.should_inherit = false;
                auto inherited = element_get_shape_aspects(&type_value, inherited_options);
                result.insert(result.end(), inherited.begin(), inherited.end());
            }
        }
        auto rep = read_ref(element_value, "Representation");
        if (rep) {
            for (auto sa : read_any_list(rep, "HasShapeAspects")) result.push_back(sa);
        }
        return result;
    }

    if (is_ifc2x3(f)) return {};

    for (auto rmap : read_ref_list(element_value, "RepresentationMaps")) {
        for (auto sa : read_any_list(rmap, "HasShapeAspects")) result.push_back(sa);
    }
    return result;
}

std::vector<express::Base> element_get_groups(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto inv = inverse_attr(element_value, "HasAssignments");
    std::vector<express::Base> result;
    for (auto rel : inv) {
        if (is_a(rel, "IfcRelAssignsToGroup")) {
            if (auto g = read_ref(rel, "RelatingGroup")) result.push_back(g);
        }
    }
    return result;
}

std::vector<express::Base> element_get_controls(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto inv = inverse_attr(element_value, "HasAssignments");
    std::vector<express::Base> result;
    for (auto rel : inv) {
        if (is_a(rel, "IfcRelAssignsToControl")) {
            if (auto c = read_ref(rel, "RelatingControl")) result.push_back(c);
        }
    }
    return result;
}

std::vector<express::Base> element_get_parts(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    bool not2x3 = !is_ifc2x3(element_value.file());
    auto inv = inverse_attr(element_value, "IsDecomposedBy");
    std::vector<express::Base> result;
    for (auto rel : inv) {
        if (not2x3 || is_a(rel, "IfcRelAggregates")) {
            for (auto o : read_ref_list(rel, "RelatedObjects")) result.push_back(o);
        }
    }
    return result;
}

std::vector<express::Base> element_get_contained(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto inv = inverse_attr(element_value, "ContainsElements");
    std::vector<express::Base> result;
    for (auto rel : inv) {
        for (auto o : read_ref_list(rel, "RelatedElements")) result.push_back(o);
    }
    return result;
}

std::vector<express::Base> element_get_referenced_structures(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto inv = inverse_attr(element_value, "ReferencedInStructures");
    std::vector<express::Base> result;
    for (auto rel : inv) {
        if (auto s = read_ref(rel, "RelatingStructure")) result.push_back(s);
    }
    return result;
}

std::vector<express::Base> element_get_structure_referenced_elements(express::Base* structure) {
    auto structure_value = detail::deref_or_empty(structure);
    if (!structure_value) return {};
    auto inv = inverse_attr(structure_value, "ReferencesElements");
    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    for (auto rel : inv) {
        for (auto el : read_ref_list(rel, "RelatedElements")) {
            add_unique(el, seen, result);
        }
    }
    return result;
}

std::vector<express::Base> element_get_openings(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    collect_openings(element_value, seen, result, element_value.file());
    return result;
}

std::optional<express::Base> element_get_filled_void(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return std::nullopt;
    auto inv = inverse_attr(element_value, "FillsVoids");
    if (inv.empty()) return std::nullopt;
    return read_ref(inv.front(), "RelatingOpeningElement");
}

std::optional<express::Base> element_get_voided_element(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return std::nullopt;
    auto inv = inverse_attr(element_value, "VoidsElements");
    if (inv.empty()) return std::nullopt;
    return read_ref(inv.front(), "RelatingBuildingElement");
}

bool element_is_userdefined_type(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return false;

    auto type_e = element_get_type(&element_value);
    bool result = false;
    bool decided = false;

    if (type_e) {
        std::string pt = ifcapi::get_string_attr(*type_e, "PredefinedType");
        if (pt == "USERDEFINED") { result = true; decided = true; }
        else if (pt.empty()) {
            std::string et;
            bool has_secondary = false;
            if (ifcapi::has_attr(*type_e, "ElementType")) {
                et = ifcapi::get_string_attr(*type_e, "ElementType");
                has_secondary = true;
            } else if (ifcapi::has_attr(*type_e, "ProcessType")) {
                et = ifcapi::get_string_attr(*type_e, "ProcessType");
                has_secondary = true;
            }
            if (has_secondary && !et.empty()) { result = true; decided = true; }
        } else if (pt != "NOTDEFINED") {
            result = false; decided = true;
        }
    }
    if (decided) return result;

    std::string pt = ifcapi::get_string_attr(element_value, "PredefinedType");
    if (pt == "USERDEFINED") return true;
    if (pt.empty()) {
        return !ifcapi::get_string_attr(element_value, "ObjectType").empty();
    }
    return false;
}

std::vector<express::Base> element_get_referenced_elements(express::Base* reference) {
    auto reference_value = detail::deref_or_empty(reference);
    if (!reference_value) return {};
    auto* f = reference_value.file();
    if (!f) return {};

    struct RefData { const char* inverse_attr; const char* rel_class; const char* relating_attr; };
    auto lookup = [](const std::string& cls) -> RefData {
        if (cls == "IfcClassificationReference")
            return {"ClassificationRefForObjects", "IfcRelAssociatesClassification", "RelatingClassification"};
        if (cls == "IfcDocumentReference")
            return {"DocumentRefForObjects", "IfcRelAssociatesDocument", "RelatingDocument"};
        if (cls == "IfcLibraryReference")
            return {"LibraryRefForObjects", "IfcRelAssociatesLibrary", "RelatingLibrary"};
        if (cls == "IfcClassification")
            return {"ClassificationForObjects", "IfcRelAssociatesClassification", "RelatingClassification"};
        if (cls == "IfcDocumentInformation")
            return {"DocumentInfoForObjects", "IfcRelAssociatesDocument", "RelatingDocument"};
        if (cls == "IfcLibraryInformation")
            return {"LibraryInfoForObjects", "IfcRelAssociatesLibrary", "RelatingLibrary"};
        return {nullptr, nullptr, nullptr};
    };

    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    auto rd = lookup(reference_value.declaration().name());

    if (is_ifc2x3(f)) {
        if (rd.rel_class) {
            try {
                auto insts = detail::instances_by_type(f, rd.rel_class);
                for (auto it : insts) {
                    if (read_ref(it, rd.relating_attr) == reference_value) {
                        for (auto obj : read_ref_list(it, "RelatedObjects")) {
                            add_unique(obj, seen, result);
                        }
                    }
                }
            } catch (...) {}
        }
    } else {
        if (is_a(reference_value, "IfcExternalReference")) {
            for (auto erel : read_any_list(reference_value, "ExternalReferenceForResources")) {
                for (auto obj : read_any_list(erel, "RelatedResourceObjects")) {
                    add_unique(obj, seen, result);
                }
            }
        }
        if (rd.inverse_attr) {
            for (auto rel : read_any_list(reference_value, rd.inverse_attr)) {
                for (auto obj : read_ref_list(rel, "RelatedObjects")) {
                    add_unique(obj, seen, result);
                }
            }
        }
    }
    return result;
}

std::vector<express::Base> element_get_elements_by_material(express::Base* material) {
    auto material_value = detail::deref_or_empty(material);
    if (!material_value) return {};
    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    collect_elements_by_material(material_value.file(), material_value, seen, result);
    return result;
}

std::vector<express::Base> element_get_elements_by_representation(express::Base* representation) {
    auto representation_value = detail::deref_or_empty(representation);
    if (!representation_value) return {};
    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    collect_elements_by_representation(representation_value.file(), representation_value, seen, result);
    return result;
}

std::vector<express::Base> element_get_elements_by_style(express::Base* style) {
    auto style_value = detail::deref_or_empty(style);
    if (!style_value) return {};
    auto* f = style_value.file();

    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;

    std::vector<express::Base> queue = ifcapi::get_all_inverses(f, style_value);
    while (!queue.empty()) {
        auto inv = queue.back();
        queue.pop_back();
        std::string cls = inv.declaration().name();
        if (cls == "IfcPresentationStyleAssignment" ||
            cls == "IfcFillAreaStyleHatching" ||
            cls == "IfcFillAreaStyle" ||
            cls == "IfcFillAreaStyleTiles") {
            auto more = ifcapi::get_all_inverses(f, inv);
            for (auto m : more) queue.push_back(m);
            continue;
        }
        if (!is_a(inv, "IfcStyledItem")) continue;
        auto item = read_ref(inv, "Item");
        if (item) {
            for (auto inv2 : ifcapi::get_all_inverses(f, item)) {
                if (is_a(inv2, "IfcShapeRepresentation")) {
                    collect_elements_by_representation(f, inv2, seen, result);
                }
            }
            for (auto m : ifcapi::get_all_inverses(f, inv)) queue.push_back(m);
        } else {
            for (auto sr : ifcapi::get_all_inverses(f, inv)) {
                if (is_a(sr, "IfcStyledRepresentation")) {
                    for (auto mdr : read_any_list(sr, "OfProductRepresentation")) {
                        if (auto mat = read_ref(mdr, "RepresentedMaterial")) {
                            collect_elements_by_material(f, mat, seen, result);
                        }
                    }
                }
            }
        }
    }
    return result;
}

std::vector<express::Base> element_get_elements_by_profile(express::Base* profile) {
    auto profile_value = detail::deref_or_empty(profile);
    if (!profile_value) return {};
    auto* f = profile_value.file();

    std::vector<express::Base> queue = ifcapi::get_all_inverses(f, profile_value);
    std::unordered_set<int32_t> processed;
    std::vector<express::Base> reps;
    std::unordered_set<int32_t> rep_seen;
    while (!queue.empty()) {
        auto item = queue.back();
        queue.pop_back();
        int32_t id = static_cast<int32_t>(item.id());
        if (id) processed.insert(id);
        if (is_a(item, "IfcRepresentationItem")) {
            for (auto inv : ifcapi::get_all_inverses(f, item)) {
                int32_t iid = static_cast<int32_t>(inv.id());
                if (iid && processed.find(iid) == processed.end()) {
                    queue.push_back(inv);
                }
            }
        } else if (is_a(item, "IfcShapeRepresentation")) {
            if (id && rep_seen.insert(id).second) reps.push_back(item);
        }
    }

    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    for (auto rep : reps) collect_elements_by_representation(f, rep, seen, result);
    return result;
}

std::vector<express::Base> element_get_elements_by_layer(express::Base* layer) {
    auto layer_value = detail::deref_or_empty(layer);
    if (!layer_value) return {};
    auto* f = layer_value.file();

    std::vector<express::Base> result;
    std::unordered_set<int32_t> seen;
    for (auto item : read_ref_list(layer_value, "AssignedItems")) {
        if (is_a(item, "IfcShapeRepresentation")) {
            collect_elements_by_representation(f, item, seen, result);
        } else if (is_a(item, "IfcRepresentationItem")) {
            for (auto inv : ifcapi::get_all_inverses(f, item)) {
                if (is_a(inv, "IfcShapeRepresentation")) {
                    collect_elements_by_representation(f, inv, seen, result);
                }
            }
        }
    }
    return result;
}

std::vector<express::Base> element_get_layers(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return {};
    auto* f = element_value.file();
    bool ifc2x3 = is_ifc2x3(f);

    std::vector<express::Base> representations;
    if (auto rep = read_ref(element_value, "Representation")) {
        representations.push_back(rep);
    } else {
        for (auto rm : read_ref_list(element_value, "RepresentationMaps")) representations.push_back(rm);
    }

    std::vector<express::Base> result;
    for (auto root : representations) {
        std::vector<express::Base> traversed;
        try { traversed = f->traverse(root, -1); } catch (...) { continue; }
        for (auto sub : traversed) {
            if (is_a(sub, "IfcShapeRepresentation")) {
                for (auto la : read_any_list(sub, "LayerAssignments")) result.push_back(la);
            } else if (is_a(sub, "IfcGeometricRepresentationItem")) {
                const char* attr = ifc2x3 ? "LayerAssignments" : "LayerAssignment";
                for (auto la : read_any_list(sub, attr)) result.push_back(la);
            }
        }
    }
    return make_unique_instance_list(result);
}

void element_replace_element(express::Base* old_element, express::Base* new_element) {
    auto old_value = detail::deref_or_empty(old_element);
    auto new_value = detail::deref_or_empty(new_element);
    if (!old_value || !new_value) return;
    auto* f = old_value.file();
    if (!f) return;
    std::vector<express::Base> inverses;
    try { inverses = f->instances_by_reference(static_cast<int>(old_value.id())); } catch (...) { return; }
    // Materialize the list first so structural mutation doesn't invalidate it.
    for (auto inv : inverses) {
        auto* d = inv ? inv.declaration().as_entity() : nullptr;
        if (!d) continue;
        auto attrs = d->all_attributes();
        for (size_t i = 0; i < attrs.size(); ++i) {
            try {
                auto v = inv.get_attribute_value(i);
                if (v.isNull()) continue;
                if (v.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                    if (static_cast<express::Base>(v) == old_value) {
                        inv.set_attribute_value(i, new_value);
                    }
                } else if (v.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                    auto agg = static_cast<std::vector<express::Base>>(v);
                    bool changed = false;
                    for (auto& it : agg) {
                        if (it == old_value) { it = new_value; changed = true; }
                    }
                    if (changed) {
                        inv.set_attribute_value(i, agg);
                    }
                }
            } catch (...) {}
        }
    }
}

void element_remove_deep(express::Base* element) {
    auto element_value = detail::deref_or_empty(element);
    if (!element_value) return;
    auto* f = element_value.file();
    if (!f) return;

    auto traversed = f->traverse_breadth_first(element_value, -1);
    if (traversed.empty()) return;
    std::vector<express::Base> subgraph;
    std::unordered_set<int32_t> subgraph_set;
    for (auto it : traversed) {
        if (!it) continue;
        subgraph.push_back(it);
        subgraph_set.insert(static_cast<int32_t>(it.id()));
    }
    for (auto rit = subgraph.rbegin(); rit != subgraph.rend(); ++rit) {
        auto ref = *rit;
        if (!ref) continue;
        if (!ref.id()) continue;
        bool can_remove = true;
        for (auto inv : f->instances_by_reference(static_cast<int>(ref.id()))) {
            if (!inv || subgraph_set.find(static_cast<int32_t>(inv.id())) == subgraph_set.end()) {
                can_remove = false;
                break;
            }
        }
        if (can_remove) {
            f->remove_entity(ref);
        }
    }
}

}  // namespace bindings
}  // namespace ifcapi
