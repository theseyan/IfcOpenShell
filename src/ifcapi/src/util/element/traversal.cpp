// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/element.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "ifcopenshell_api_internal.hpp"
#include "entity_introspection.hpp"

namespace {

inline IfcUtil::IfcBaseClass* get_entity(const ifcopenshell_ifc_instance_t* instance) {
    return instance ? instance->ptr : nullptr;
}

inline bool is_a(IfcUtil::IfcBaseClass* e, const char* type) {
    return e && e->declaration().is(type);
}

aggregate_of_instance::ptr inverse_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return nullptr;
    try { return be->get_inverse(attr); } catch (...) { return nullptr; }
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    return ifcapi::get_entity_ref(e, attr);
}

std::vector<IfcUtil::IfcBaseClass*> read_ref_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    return ifcapi::get_entity_list(e, attr);
}

// Read a list of entity references via either a forward aggregate attribute or
// an inverse attribute (whichever is defined on the entity for `attr`).
std::vector<IfcUtil::IfcBaseClass*> read_any_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto out = read_ref_list(e, attr);
    if (!out.empty()) return out;
    if (auto inv = inverse_attr(e, attr)) {
        for (size_t i = 0; i < inv->size(); ++i) out.push_back((*inv)[i]);
    }
    return out;
}

bool is_ifc2x3(IfcParse::IfcFile* f) {
    return f && f->schema() && f->schema()->name() == "IFC2X3";
}

ifcopenshell_ifc_instance_t** alloc_handles(const std::vector<IfcUtil::IfcBaseClass*>& items, uint32_t* out_count) {
    if (out_count) *out_count = static_cast<uint32_t>(items.size());
    if (items.empty()) return nullptr;
    auto** buf = static_cast<ifcopenshell_ifc_instance_t**>(
        std::malloc(items.size() * sizeof(ifcopenshell_ifc_instance_t*)));
    if (!buf) {
        if (out_count) *out_count = 0;
        return nullptr;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        buf[i] = ifcopenshell::capi::wrap_instance(items[i]);
    }
    return buf;
}

ifcopenshell_ifc_instance_t** alloc_handles_unique(
    const std::vector<IfcUtil::IfcBaseClass*>& items, uint32_t* out_count)
{
    std::vector<IfcUtil::IfcBaseClass*> unique;
    std::unordered_set<int32_t> seen;
    for (auto* e : items) {
        if (!e) continue;
        int32_t id = static_cast<int32_t>(e->id());
        if (id == 0) {
            unique.push_back(e);
            continue;
        }
        if (seen.insert(id).second) unique.push_back(e);
    }
    return alloc_handles(unique, out_count);
}

aggregate_of_instance::ptr make_instance_list(const std::vector<IfcUtil::IfcBaseClass*>& items) {
    aggregate_of_instance::ptr result(new aggregate_of_instance);
    for (auto* item : items) {
        if (item) result->push(item);
    }
    return result;
}

aggregate_of_instance::ptr make_unique_instance_list(const std::vector<IfcUtil::IfcBaseClass*>& items) {
    std::vector<IfcUtil::IfcBaseClass*> unique;
    std::unordered_set<int32_t> seen;
    for (auto* item : items) {
        if (!item) continue;
        int32_t id = static_cast<int32_t>(item->id());
        if (id == 0 || seen.insert(id).second) unique.push_back(item);
    }
    return make_instance_list(unique);
}

// Forward declaration for mutual recursion.
void collect_elements_by_material(IfcParse::IfcFile* f, IfcUtil::IfcBaseClass* mat,
                                  std::unordered_set<int32_t>& seen,
                                  std::vector<IfcUtil::IfcBaseClass*>& out);

void collect_elements_by_representation(IfcParse::IfcFile* f, IfcUtil::IfcBaseClass* rep,
                                        std::unordered_set<int32_t>& seen,
                                        std::vector<IfcUtil::IfcBaseClass*>& out);

void add_unique(IfcUtil::IfcBaseClass* e,
                std::unordered_set<int32_t>& seen,
                std::vector<IfcUtil::IfcBaseClass*>& out)
{
    if (!e) return;
    int32_t id = static_cast<int32_t>(e->id());
    if (id && seen.insert(id).second) out.push_back(e);
}

void collect_elements_by_representation(IfcParse::IfcFile* f, IfcUtil::IfcBaseClass* rep,
                                        std::unordered_set<int32_t>& seen,
                                        std::vector<IfcUtil::IfcBaseClass*>& out)
{
    if (!rep) return;
    for (auto* pr : read_any_list(rep, "OfProductRepresentation")) {
        for (auto* prod : read_any_list(pr, "ShapeOfProduct")) {
            add_unique(prod, seen, out);
        }
    }
    for (auto* rmap : read_any_list(rep, "RepresentationMap")) {
        auto inv = ifcapi::get_all_inverses(f, rmap);
        for (auto* it : inv) {
            if (is_a(it, "IfcTypeProduct")) {
                add_unique(it, seen, out);
            } else if (is_a(it, "IfcMappedItem")) {
                auto inv2 = ifcapi::get_all_inverses(f, it);
                for (auto* sr : inv2) {
                    if (is_a(sr, "IfcShapeRepresentation")) {
                        collect_elements_by_representation(f, sr, seen, out);
                    }
                }
            }
        }
    }
}

void collect_elements_by_material(IfcParse::IfcFile* f, IfcUtil::IfcBaseClass* mat,
                                  std::unordered_set<int32_t>& seen,
                                  std::vector<IfcUtil::IfcBaseClass*>& out)
{
    if (!mat) return;
    for (auto* inv : ifcapi::get_all_inverses(f, mat)) {
        if (is_a(inv, "IfcRelAssociatesMaterial")) {
            for (auto* obj : read_ref_list(inv, "RelatedObjects")) add_unique(obj, seen, out);
        } else if (is_a(inv, "IfcMaterialLayer")) {
            for (auto* set_ : read_any_list(inv, "ToMaterialLayerSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialProfile")) {
            for (auto* set_ : read_any_list(inv, "ToMaterialProfileSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialConstituent")) {
            for (auto* set_ : read_any_list(inv, "ToMaterialConstituentSet"))
                collect_elements_by_material(f, set_, seen, out);
        } else if (is_a(inv, "IfcMaterialLayerSetUsage") ||
                   is_a(inv, "IfcMaterialProfileSetUsage") ||
                   is_a(inv, "IfcMaterialList")) {
            collect_elements_by_material(f, inv, seen, out);
        }
    }
}

void collect_openings(IfcUtil::IfcBaseClass* e,
                      std::unordered_set<int32_t>& seen,
                      std::vector<IfcUtil::IfcBaseClass*>& out,
                      IfcParse::IfcFile* file)
{
    if (!e) return;
    auto inv = inverse_attr(e, "HasOpenings");
    if (inv) {
        for (size_t i = 0; i < inv->size(); ++i) {
            auto* rel = (*inv)[i];
            int32_t id = static_cast<int32_t>(rel->id());
            if (id && seen.insert(id).second) out.push_back(rel);
        }
    }
    // Recurse into aggregate parent.
    auto decomposes = inverse_attr(e, "Decomposes");
    if (decomposes && decomposes->size() > 0) {
        auto* rel = (*decomposes)[0];
        bool ok = true;
        if (is_ifc2x3(file) && !is_a(rel, "IfcRelAggregates")) ok = false;
        if (ok) {
            auto* parent = read_ref(rel, "RelatingObject");
            if (parent) collect_openings(parent, seen, out, file);
        }
    }
}

bool is_userdefined_predefined_type(IfcUtil::IfcBaseClass* e) {
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

aggregate_of_instance::ptr element_get_types(IfcUtil::IfcBaseClass* type_element) {
    if (!type_element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto* f = type_element->file_;
    const char* attr = is_ifc2x3(f) ? "ObjectTypeOf" : "Types";
    auto inv = inverse_attr(type_element, attr);
    if (!inv || inv->size() == 0) return aggregate_of_instance::ptr(new aggregate_of_instance);
    return make_instance_list(read_ref_list((*inv)[0], "RelatedObjects"));
}

aggregate_of_instance::ptr element_get_shape_aspects(
    IfcUtil::IfcBaseClass* element,
    bool should_inherit)
{
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto* f = element->file_;

    std::vector<IfcUtil::IfcBaseClass*> result;

    if (ifcapi::has_attr(element, "Representation")) {
        if (should_inherit) {
            auto* type_e = element_get_type(element);
            if (type_e) {
                auto inherited = element_get_shape_aspects(type_e, false);
                if (inherited) {
                    for (size_t i = 0; i < inherited->size(); ++i) result.push_back((*inherited)[i]);
                }
            }
        }
        auto* rep = read_ref(element, "Representation");
        if (rep) {
            for (auto* sa : read_any_list(rep, "HasShapeAspects")) result.push_back(sa);
        }
        return make_instance_list(result);
    }

    if (is_ifc2x3(f)) return aggregate_of_instance::ptr(new aggregate_of_instance);

    for (auto* rmap : read_ref_list(element, "RepresentationMaps")) {
        for (auto* sa : read_any_list(rmap, "HasShapeAspects")) result.push_back(sa);
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_groups(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto inv = inverse_attr(element, "HasAssignments");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (size_t i = 0; i < inv->size(); ++i) {
        auto* rel = (*inv)[i];
        if (is_a(rel, "IfcRelAssignsToGroup")) {
            if (auto* g = read_ref(rel, "RelatingGroup")) result.push_back(g);
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_controls(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto inv = inverse_attr(element, "HasAssignments");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (size_t i = 0; i < inv->size(); ++i) {
        auto* rel = (*inv)[i];
        if (is_a(rel, "IfcRelAssignsToControl")) {
            if (auto* c = read_ref(rel, "RelatingControl")) result.push_back(c);
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_parts(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    bool not2x3 = !is_ifc2x3(element->file_);
    auto inv = inverse_attr(element, "IsDecomposedBy");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (size_t i = 0; i < inv->size(); ++i) {
        auto* rel = (*inv)[i];
        if (not2x3 || is_a(rel, "IfcRelAggregates")) {
            for (auto* o : read_ref_list(rel, "RelatedObjects")) result.push_back(o);
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_contained(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto inv = inverse_attr(element, "ContainsElements");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (size_t i = 0; i < inv->size(); ++i) {
        auto* rel = (*inv)[i];
        for (auto* o : read_ref_list(rel, "RelatedElements")) result.push_back(o);
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_referenced_structures(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto inv = inverse_attr(element, "ReferencedInStructures");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    for (size_t i = 0; i < inv->size(); ++i) {
        if (auto* s = read_ref((*inv)[i], "RelatingStructure")) result.push_back(s);
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_structure_referenced_elements(IfcUtil::IfcBaseClass* structure) {
    if (!structure) return aggregate_of_instance::ptr(new aggregate_of_instance);
    auto inv = inverse_attr(structure, "ReferencesElements");
    if (!inv) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    for (size_t i = 0; i < inv->size(); ++i) {
        for (auto* el : read_ref_list((*inv)[i], "RelatedElements")) {
            add_unique(el, seen, result);
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_openings(IfcUtil::IfcBaseClass* element) {
    if (!element) return aggregate_of_instance::ptr(new aggregate_of_instance);
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    collect_openings(element, seen, result, element->file_);
    return make_instance_list(result);
}

IfcUtil::IfcBaseClass* element_get_filled_void(IfcUtil::IfcBaseClass* element) {
    if (!element) return nullptr;
    auto inv = inverse_attr(element, "FillsVoids");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingOpeningElement");
}

IfcUtil::IfcBaseClass* element_get_voided_element(IfcUtil::IfcBaseClass* element) {
    if (!element) return nullptr;
    auto inv = inverse_attr(element, "VoidsElements");
    if (!inv || inv->size() == 0) return nullptr;
    return read_ref((*inv)[0], "RelatingBuildingElement");
}

bool element_is_userdefined_type(IfcUtil::IfcBaseClass* element) {
    if (!element) return false;

    auto* type_e = element_get_type(element);
    bool result = false;
    bool decided = false;

    if (type_e) {
        std::string pt = ifcapi::get_string_attr(type_e, "PredefinedType");
        if (pt == "USERDEFINED") { result = true; decided = true; }
        else if (pt.empty()) {
            std::string et;
            bool has_secondary = false;
            if (ifcapi::has_attr(type_e, "ElementType")) {
                et = ifcapi::get_string_attr(type_e, "ElementType");
                has_secondary = true;
            } else if (ifcapi::has_attr(type_e, "ProcessType")) {
                et = ifcapi::get_string_attr(type_e, "ProcessType");
                has_secondary = true;
            }
            if (has_secondary && !et.empty()) { result = true; decided = true; }
        } else if (pt != "NOTDEFINED") {
            result = false; decided = true;
        }
    }
    if (decided) return result;

    std::string pt = ifcapi::get_string_attr(element, "PredefinedType");
    if (pt == "USERDEFINED") return true;
    if (pt.empty()) {
        return !ifcapi::get_string_attr(element, "ObjectType").empty();
    }
    return false;
}

aggregate_of_instance::ptr element_get_referenced_elements(IfcUtil::IfcBaseClass* reference) {
    if (!reference) return make_instance_list({});
    auto* f = reference->file_;
    if (!f) return make_instance_list({});

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

    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    auto rd = lookup(reference->declaration().name());

    if (is_ifc2x3(f)) {
        if (rd.rel_class) {
            try {
                auto insts = f->instances_by_type(std::string(rd.rel_class));
                if (insts) {
                    for (auto& it : *insts) {
                        if (read_ref(it, rd.relating_attr) == reference) {
                            for (auto* obj : read_ref_list(it, "RelatedObjects")) {
                                add_unique(obj, seen, result);
                            }
                        }
                    }
                }
            } catch (...) {}
        }
    } else {
        if (is_a(reference, "IfcExternalReference")) {
            for (auto* erel : read_any_list(reference, "ExternalReferenceForResources")) {
                for (auto* obj : read_any_list(erel, "RelatedResourceObjects")) {
                    add_unique(obj, seen, result);
                }
            }
        }
        if (rd.inverse_attr) {
            for (auto* rel : read_any_list(reference, rd.inverse_attr)) {
                for (auto* obj : read_ref_list(rel, "RelatedObjects")) {
                    add_unique(obj, seen, result);
                }
            }
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_elements_by_material(IfcUtil::IfcBaseClass* material) {
    if (!material) return make_instance_list({});
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    collect_elements_by_material(material->file_, material, seen, result);
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_elements_by_representation(IfcUtil::IfcBaseClass* representation) {
    if (!representation) return make_instance_list({});
    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    collect_elements_by_representation(representation->file_, representation, seen, result);
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_elements_by_style(IfcUtil::IfcBaseClass* style) {
    if (!style) return make_instance_list({});
    auto* f = style->file_;

    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;

    std::vector<IfcUtil::IfcBaseClass*> queue = ifcapi::get_all_inverses(f, style);
    while (!queue.empty()) {
        auto* inv = queue.back();
        queue.pop_back();
        std::string cls = inv->declaration().name();
        if (cls == "IfcPresentationStyleAssignment" ||
            cls == "IfcFillAreaStyleHatching" ||
            cls == "IfcFillAreaStyle" ||
            cls == "IfcFillAreaStyleTiles") {
            auto more = ifcapi::get_all_inverses(f, inv);
            for (auto* m : more) queue.push_back(m);
            continue;
        }
        if (!is_a(inv, "IfcStyledItem")) continue;
        auto* item = read_ref(inv, "Item");
        if (item) {
            for (auto* inv2 : ifcapi::get_all_inverses(f, item)) {
                if (is_a(inv2, "IfcShapeRepresentation")) {
                    collect_elements_by_representation(f, inv2, seen, result);
                }
            }
            for (auto* m : ifcapi::get_all_inverses(f, inv)) queue.push_back(m);
        } else {
            for (auto* sr : ifcapi::get_all_inverses(f, inv)) {
                if (is_a(sr, "IfcStyledRepresentation")) {
                    for (auto* mdr : read_any_list(sr, "OfProductRepresentation")) {
                        if (auto* mat = read_ref(mdr, "RepresentedMaterial")) {
                            collect_elements_by_material(f, mat, seen, result);
                        }
                    }
                }
            }
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_elements_by_profile(IfcUtil::IfcBaseClass* profile) {
    if (!profile) return make_instance_list({});
    auto* f = profile->file_;

    std::vector<IfcUtil::IfcBaseClass*> queue = ifcapi::get_all_inverses(f, profile);
    std::unordered_set<int32_t> processed;
    std::vector<IfcUtil::IfcBaseClass*> reps;
    std::unordered_set<int32_t> rep_seen;
    while (!queue.empty()) {
        auto* item = queue.back();
        queue.pop_back();
        int32_t id = static_cast<int32_t>(item->id());
        if (id) processed.insert(id);
        if (is_a(item, "IfcRepresentationItem")) {
            for (auto* inv : ifcapi::get_all_inverses(f, item)) {
                int32_t iid = static_cast<int32_t>(inv->id());
                if (iid && processed.find(iid) == processed.end()) {
                    queue.push_back(inv);
                }
            }
        } else if (is_a(item, "IfcShapeRepresentation")) {
            if (id && rep_seen.insert(id).second) reps.push_back(item);
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    for (auto* rep : reps) collect_elements_by_representation(f, rep, seen, result);
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_elements_by_layer(IfcUtil::IfcBaseClass* layer) {
    if (!layer) return make_instance_list({});
    auto* f = layer->file_;

    std::vector<IfcUtil::IfcBaseClass*> result;
    std::unordered_set<int32_t> seen;
    for (auto* item : read_ref_list(layer, "AssignedItems")) {
        if (is_a(item, "IfcShapeRepresentation")) {
            collect_elements_by_representation(f, item, seen, result);
        } else if (is_a(item, "IfcRepresentationItem")) {
            for (auto* inv : ifcapi::get_all_inverses(f, item)) {
                if (is_a(inv, "IfcShapeRepresentation")) {
                    collect_elements_by_representation(f, inv, seen, result);
                }
            }
        }
    }
    return make_instance_list(result);
}

aggregate_of_instance::ptr element_get_layers(IfcUtil::IfcBaseClass* element) {
    if (!element) return make_instance_list({});
    auto* f = element->file_;
    bool ifc2x3 = is_ifc2x3(f);

    std::vector<IfcUtil::IfcBaseClass*> representations;
    if (auto* rep = read_ref(element, "Representation")) {
        representations.push_back(rep);
    } else {
        for (auto* rm : read_ref_list(element, "RepresentationMaps")) representations.push_back(rm);
    }

    std::vector<IfcUtil::IfcBaseClass*> result;
    for (auto* root : representations) {
        aggregate_of_instance::ptr traversed;
        try { traversed = f->traverse(root, -1); } catch (...) { continue; }
        if (!traversed) continue;
        for (auto& sub : *traversed) {
            if (is_a(sub, "IfcShapeRepresentation")) {
                for (auto* la : read_any_list(sub, "LayerAssignments")) result.push_back(la);
            } else if (is_a(sub, "IfcGeometricRepresentationItem")) {
                const char* attr = ifc2x3 ? "LayerAssignments" : "LayerAssignment";
                for (auto* la : read_any_list(sub, attr)) result.push_back(la);
            }
        }
    }
    return make_unique_instance_list(result);
}

void element_replace_element(IfcUtil::IfcBaseClass* old_element, IfcUtil::IfcBaseClass* new_element) {
    if (!old_element || !new_element) return;
    auto* f = old_element->file_;
    if (!f) return;
    aggregate_of_instance::ptr inverses;
    try { inverses = f->getInverse(old_element->id(), nullptr, -1); } catch (...) { return; }
    if (!inverses) return;
    // Materialize the list first so structural mutation doesn't invalidate it.
    std::vector<IfcUtil::IfcBaseClass*> list;
    for (auto& it : *inverses) list.push_back(it);
    for (auto* inv : list) {
        auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(inv);
        if (!be) continue;
        auto* d = be->declaration().as_entity();
        if (!d) continue;
        size_t n = d->attribute_count();
        for (size_t i = 0; i < n; ++i) {
            try {
                auto v = inv->get_attribute_value(i);
                if (v.isNull()) continue;
                if (v.type() == IfcUtil::Argument_ENTITY_INSTANCE) {
                    if ((IfcUtil::IfcBaseClass*)v == old_element) {
                        inv->set_attribute_value(i, new_element);
                    }
                } else if (v.type() == IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                    auto agg = (aggregate_of_instance::ptr)v;
                    if (!agg) continue;
                    bool changed = false;
                    auto repl = aggregate_of_instance::ptr(new aggregate_of_instance);
                    for (auto& it : *agg) {
                        if (it == old_element) { repl->push(new_element); changed = true; }
                        else { repl->push(it); }
                    }
                    if (changed) {
                        inv->set_attribute_value(i, repl);
                    }
                }
            } catch (...) {}
        }
    }
}

void element_remove_deep(IfcUtil::IfcBaseClass* element) {
    if (!element) return;
    auto* f = element->file_;
    if (!f) return;

    auto traversed = f->traverse_breadth_first(element, -1);
    if (!traversed) return;
    std::vector<IfcUtil::IfcBaseClass*> subgraph;
    std::unordered_set<int32_t> subgraph_set;
    for (auto& it : *traversed) {
        if (!it) continue;
        subgraph.push_back(it);
        subgraph_set.insert(static_cast<int32_t>(it->id()));
    }
    for (auto rit = subgraph.rbegin(); rit != subgraph.rend(); ++rit) {
        auto* ref = *rit;
        if (!ref) continue;
        if (!ref->id()) continue;
        bool can_remove = true;
        auto invs = f->getInverse(ref->id(), nullptr, -1);
        if (invs) {
            for (auto& inv : *invs) {
                if (!inv || subgraph_set.find(static_cast<int32_t>(inv->id())) == subgraph_set.end()) {
                    can_remove = false;
                    break;
                }
            }
        }
        if (can_remove) {
            f->removeEntity(ref);
        }
    }
}

}  // namespace bindings
}  // namespace ifcapi
