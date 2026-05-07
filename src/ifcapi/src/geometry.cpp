// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Native port of the high-level representation-mapping APIs from
// `ifcopenshell.api.geometry` and parts of `ifcopenshell.util.element`.
//
// Functions implemented:
//   - geometry_map_representation
//   - geometry_assign_representation
//   - geometry_unassign_representation
//   - geometry_copy_representation
//   - (file-local) deep_copy_entity, get_representation_for_product
//
// These are used internally by type assignment to propagate IfcRepresentationMaps
// onto related occurrences when a typed object is assigned.

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/bindings/unit.h"
#include "guid.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/ArgumentType.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "ifcopenshell_api_internal.hpp"

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }

inline IfcUtil::IfcBaseEntity* as_entity(IfcUtil::IfcBaseClass* e) {
    return dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
}

inline int attr_index(const IfcParse::entity* d, const char* name) {
    if (!d) return -1;
    return d->attribute_index(name);
}

inline int attr_index_of(IfcUtil::IfcBaseClass* e, const char* name) {
    auto* be = as_entity(e);
    if (!be) return -1;
    return attr_index(be->declaration().as_entity(), name);
}

IfcUtil::IfcBaseClass* read_ref(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return nullptr;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return nullptr;
        return (IfcUtil::IfcBaseClass*)v;
    } catch (...) { return nullptr; }
}

std::string read_string(IfcUtil::IfcBaseClass* e, const char* attr) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return {};
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return {};
        return (std::string)v;
    } catch (...) { return {}; }
}

std::vector<IfcUtil::IfcBaseClass*> read_ref_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> out;
    int idx = attr_index_of(e, attr);
    if (idx < 0) return out;
    try {
        auto v = e->get_attribute_value(static_cast<size_t>(idx));
        if (v.isNull()) return out;
        auto agg = (aggregate_of_instance::ptr)v;
        if (!agg) return out;
        for (auto& it : *agg) out.push_back(it);
    } catch (...) {}
    return out;
}

void write_ref_list(IfcUtil::IfcBaseClass* e, const char* attr,
                    const std::vector<IfcUtil::IfcBaseClass*>& refs) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    if (refs.empty()) {
        e->set_attribute_value(static_cast<size_t>(idx), Blank{});
        return;
    }
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* r : refs) agg->push(r);
    e->set_attribute_value(static_cast<size_t>(idx), agg);
}

void write_double_list(IfcUtil::IfcBaseClass* e, const char* attr,
                       const std::vector<double>& values) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    e->set_attribute_value(static_cast<size_t>(idx), values);
}

void write_int(IfcUtil::IfcBaseClass* e, const char* attr, int v) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    e->set_attribute_value(static_cast<size_t>(idx), v);
}

void write_string(IfcUtil::IfcBaseClass* e, const char* attr, const std::string& v) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    e->set_attribute_value(static_cast<size_t>(idx), v);
}

void write_ref(IfcUtil::IfcBaseClass* e, const char* attr, IfcUtil::IfcBaseClass* ref) {
    int idx = attr_index_of(e, attr);
    if (idx < 0) return;
    if (!ref) {
        e->set_attribute_value(static_cast<size_t>(idx), Blank{});
        return;
    }
    e->set_attribute_value(static_cast<size_t>(idx), ref);
}

inline bool is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    return e && e->declaration().is(name);
}

// Construct standard geometry primitives reusable across representation
// assignment / mapping operations.

IfcUtil::IfcBaseClass* make_cartesian_point(IfcParse::IfcFile* file,
                                            double x, double y, double z) {
    auto* decl = file->schema()->declaration_by_name("IfcCartesianPoint");
    auto* p = file->create(decl);
    write_double_list(p, "Coordinates", {x, y, z});
    return p;
}

IfcUtil::IfcBaseClass* make_direction(IfcParse::IfcFile* file,
                                      double x, double y, double z) {
    auto* decl = file->schema()->declaration_by_name("IfcDirection");
    auto* p = file->create(decl);
    write_double_list(p, "DirectionRatios", {x, y, z});
    return p;
}

IfcUtil::IfcBaseClass* make_axis2_placement_3d(IfcParse::IfcFile* file) {
    auto* decl = file->schema()->declaration_by_name("IfcAxis2Placement3D");
    auto* p = file->create(decl);
    write_ref(p, "Location",     make_cartesian_point(file, 0.0, 0.0, 0.0));
    write_ref(p, "Axis",         make_direction(file, 0.0, 0.0, 1.0));
    write_ref(p, "RefDirection", make_direction(file, 1.0, 0.0, 0.0));
    return p;
}

IfcUtil::IfcBaseClass* make_cartesian_transformation_op_3d(IfcParse::IfcFile* file) {
    auto* decl = file->schema()->declaration_by_name("IfcCartesianTransformationOperator3D");
    auto* p = file->create(decl);
    write_ref(p, "Axis1",       make_direction(file, 1.0, 0.0, 0.0));
    write_ref(p, "Axis2",       make_direction(file, 0.0, 1.0, 0.0));
    write_ref(p, "LocalOrigin", make_cartesian_point(file, 0.0, 0.0, 0.0));
    write_int(p, "Scale",       1);
    write_ref(p, "Axis3",       make_direction(file, 0.0, 0.0, 1.0));
    return p;
}

IfcUtil::IfcBaseClass* make_representation_map(IfcParse::IfcFile* file,
                                               IfcUtil::IfcBaseClass* mapped_rep) {
    auto* decl = file->schema()->declaration_by_name("IfcRepresentationMap");
    auto* p = file->create(decl);
    write_ref(p, "MappingOrigin",       make_axis2_placement_3d(file));
    write_ref(p, "MappedRepresentation", mapped_rep);
    return p;
}

IfcUtil::IfcBaseClass* make_mapped_item(IfcParse::IfcFile* file,
                                        IfcUtil::IfcBaseClass* source,
                                        IfcUtil::IfcBaseClass* target) {
    auto* decl = file->schema()->declaration_by_name("IfcMappedItem");
    auto* p = file->create(decl);
    write_ref(p, "MappingSource", source);
    write_ref(p, "MappingTarget", target);
    return p;
}

IfcUtil::IfcBaseClass* find_existing_representation_map(IfcParse::IfcFile* file,
                                                       IfcUtil::IfcBaseClass* representation) {
    auto inverses = file->getInverse(representation->id(), nullptr, -1);
    if (!inverses) return nullptr;
    for (auto& inv : *inverses) {
        if (is_a(inv, "IfcRepresentationMap")) return inv;
    }
    return nullptr;
}

// ---- copy_deep ----------------------------------------------------------

bool excluded(IfcUtil::IfcBaseClass* e, const std::vector<std::string>& exclude) {
    if (!e) return false;
    for (const auto& name : exclude) {
        if (e->declaration().is(name)) return true;
    }
    return false;
}

IfcUtil::IfcBaseClass* deep_copy_entity(IfcParse::IfcFile* file,
                                       IfcUtil::IfcBaseClass* element,
                                       const std::vector<std::string>& exclude,
                                       std::unordered_map<unsigned, IfcUtil::IfcBaseClass*>& memo) {
    if (!element) return nullptr;
    auto id = element->id();
    if (id) {
        auto it = memo.find(id);
        if (it != memo.end()) return it->second;
    }

    auto* be = as_entity(element);
    if (!be) return element; // simple types: return as-is

    const auto* decl = be->declaration().as_entity();
    if (!decl) return element;

    auto* dst = file->create(decl);
    if (id) memo[id] = dst;

    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto v = element->get_attribute_value(i);
            if (v.isNull()) continue;
            auto t = v.type();

            // Replace GlobalIds with fresh GUIDs.
            if (attrs[i]->name() == "GlobalId") {
                dst->set_attribute_value(i, ifcapi::guid_new());
                continue;
            }

            switch (t) {
                case IfcUtil::Argument_INT:
                    dst->set_attribute_value(i, (int)v); break;
                case IfcUtil::Argument_BOOL:
                    dst->set_attribute_value(i, (bool)v); break;
                case IfcUtil::Argument_LOGICAL:
                    dst->set_attribute_value(i, (boost::logic::tribool)v); break;
                case IfcUtil::Argument_DOUBLE:
                    dst->set_attribute_value(i, (double)v); break;
                case IfcUtil::Argument_STRING:
                    dst->set_attribute_value(i, (std::string)v); break;
                case IfcUtil::Argument_BINARY:
                    dst->set_attribute_value(i, (boost::dynamic_bitset<>)v); break;
                case IfcUtil::Argument_ENUMERATION:
                    dst->set_attribute_value(i, (std::string)v); break;
                case IfcUtil::Argument_ENTITY_INSTANCE: {
                    auto* ref = (IfcUtil::IfcBaseClass*)v;
                    if (excluded(ref, exclude)) {
                        dst->set_attribute_value(i, ref);
                    } else {
                        dst->set_attribute_value(i, deep_copy_entity(file, ref, exclude, memo));
                    }
                    break;
                }
                case IfcUtil::Argument_AGGREGATE_OF_INT:
                    dst->set_attribute_value(i, (std::vector<int>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_DOUBLE:
                    dst->set_attribute_value(i, (std::vector<double>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_STRING:
                    dst->set_attribute_value(i, (std::vector<std::string>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_BINARY:
                    dst->set_attribute_value(i, (std::vector<boost::dynamic_bitset<>>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto agg = (aggregate_of_instance::ptr)v;
                    auto out = aggregate_of_instance::ptr(new aggregate_of_instance());
                    if (agg) {
                        for (auto& item : *agg) {
                            if (excluded(item, exclude)) {
                                out->push(item);
                            } else {
                                out->push(deep_copy_entity(file, item, exclude, memo));
                            }
                        }
                    }
                    dst->set_attribute_value(i, out);
                    break;
                }
                case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
                    dst->set_attribute_value(i, (std::vector<std::vector<int>>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
                    dst->set_attribute_value(i, (std::vector<std::vector<double>>)v); break;
                case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto agg = (aggregate_of_aggregate_of_instance::ptr)v;
                    auto out = aggregate_of_aggregate_of_instance::ptr(new aggregate_of_aggregate_of_instance());
                    if (agg) {
                        for (auto outer_it = agg->begin(); outer_it != agg->end(); ++outer_it) {
                            std::vector<IfcUtil::IfcBaseClass*> inner;
                            for (auto* item : *outer_it) {
                                if (excluded(item, exclude)) inner.push_back(item);
                                else inner.push_back(deep_copy_entity(file, item, exclude, memo));
                            }
                            out->push(inner);
                        }
                    }
                    dst->set_attribute_value(i, out);
                    break;
                }
                default:
                    break;
            }
        } catch (...) {
            // skip unsupported attribute
        }
    }
    return dst;
}

// ---- get_representation_for_product ------------------------------------

IfcUtil::IfcBaseClass* get_representation_for_product(IfcUtil::IfcBaseClass* product,
                                                     const std::string& context_identifier) {
    if (!product) return nullptr;
    if (is_a(product, "IfcProduct")) {
        auto* def = read_ref(product, "Representation");
        if (!def) return nullptr;
        for (auto* rep : read_ref_list(def, "Representations")) {
            if (read_string(rep, "RepresentationIdentifier") == context_identifier) {
                return rep;
            }
        }
    } else if (is_a(product, "IfcTypeProduct")) {
        for (auto* m : read_ref_list(product, "RepresentationMaps")) {
            auto* mapped = read_ref(m, "MappedRepresentation");
            if (mapped && read_string(mapped, "RepresentationIdentifier") == context_identifier) {
                return mapped;
            }
        }
    }
    return nullptr;
}

// ---- representation removal --------------------------------------------

// Simplified port of ifcopenshell.api.geometry.remove_representation.
// Walks the representation graph, and for each visited entity that has no
// remaining inverses (after the rep is removed) deletes it.  Excludes
// IfcGeometricRepresentationContext from deletion.
void remove_representation_simple(IfcParse::IfcFile* file,
                                  IfcUtil::IfcBaseClass* representation) {
    if (!representation) return;
    auto subs = file->traverse(representation, -1);
    if (!subs) {
        file->removeEntity(representation);
        return;
    }

    // Collect every entity to potentially delete, excluding contexts.
    std::vector<IfcUtil::IfcBaseClass*> candidates;
    candidates.reserve(subs->size());
    for (auto& s : *subs) {
        if (!s) continue;
        if (is_a(s, "IfcGeometricRepresentationContext")) continue;
        candidates.push_back(s);
    }

    std::set<unsigned> deletable;
    for (auto* c : candidates) {
        if (c->id() != 0) deletable.insert(c->id());
    }

    // Only delete a node if every inverse is also deleted. If a candidate is
    // retained because of an external inverse, anything it references must be
    // retained too.
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto* e : candidates) {
            unsigned id = e->id();
            if (id == 0 || deletable.find(id) == deletable.end()) continue;
            auto invs = file->getInverse(id, nullptr, -1);
            if (!invs) continue;
            for (auto& inv : *invs) {
                if (!inv) continue;
                if (deletable.find(inv->id()) == deletable.end()) {
                    deletable.erase(id);
                    changed = true;
                    break;
                }
            }
        }
    }

    for (auto* e : candidates) {
        unsigned id = e->id();
        if (id == 0 || deletable.find(id) == deletable.end()) continue;
        if (file->instance_by_id(id)) file->removeEntity(e);
    }
}

void unassign_product_representation(IfcParse::IfcFile* file,
                                     IfcUtil::IfcBaseClass* product,
                                     IfcUtil::IfcBaseClass* representation) {
    auto* def = read_ref(product, "Representation");
    if (!def) return;
    auto reps = read_ref_list(def, "Representations");
    auto found = std::find(reps.begin(), reps.end(), representation);
    if (found == reps.end()) return;
    reps.erase(found);
    if (reps.empty()) {
        if (auto* be = as_entity(def)) {
            try {
                auto aspects = be->get_inverse("HasShapeAspects");
                std::vector<IfcUtil::IfcBaseClass*> shape_aspects;
                if (aspects) {
                    for (auto& aspect : *aspects) {
                        if (aspect) shape_aspects.push_back(aspect);
                    }
                }
                for (auto* aspect : shape_aspects) {
                    auto shape_reps = read_ref_list(aspect, "ShapeRepresentations");
                    file->removeEntity(aspect);
                    for (auto* shape_rep : shape_reps) {
                        remove_representation_simple(file, shape_rep);
                    }
                }
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to process shape aspects: ") + e.what());
            }
        }
        write_ref(product, "Representation", nullptr);
        file->removeEntity(def);
    } else {
        write_ref_list(def, "Representations", reps);
    }
}

// ---- map_representation -------------------------------------------------

IfcUtil::IfcBaseClass* map_representation_impl(IfcParse::IfcFile* file,
                                               IfcUtil::IfcBaseClass* representation) {
    auto* mapping_source = find_existing_representation_map(file, representation);
    if (!mapping_source) {
        mapping_source = make_representation_map(file, representation);
    }
    auto* mapping_target = make_cartesian_transformation_op_3d(file);
    auto* mapped_item = make_mapped_item(file, mapping_source, mapping_target);

    auto* sr_decl = file->schema()->declaration_by_name("IfcShapeRepresentation");
    auto* sr = file->create(sr_decl);
    write_ref(sr, "ContextOfItems", read_ref(representation, "ContextOfItems"));
    auto rid = read_string(representation, "RepresentationIdentifier");
    if (!rid.empty()) write_string(sr, "RepresentationIdentifier", rid);
    write_string(sr, "RepresentationType", "MappedRepresentation");
    write_ref_list(sr, "Items", {mapped_item});
    return sr;
}

// ---- assign_representation ---------------------------------------------

// Get the type of a product (IFC4: IsTypedBy, IFC2X3: IsDefinedBy).
IfcUtil::IfcBaseClass* get_product_type(IfcParse::IfcFile* file,
                                        IfcUtil::IfcBaseClass* product) {
    auto* be = as_entity(product);
    if (!be) return nullptr;
    try {
        auto invs = be->get_inverse("IsTypedBy");
        if (invs && invs->size() > 0) {
            auto* rel = (*invs)[0];
            return read_ref(rel, "RelatingType");
        }
    } catch (...) {}
    try {
        auto invs = be->get_inverse("IsDefinedBy");
        if (invs) {
            const auto* rdt = file->schema()->declaration_by_name("IfcRelDefinesByType");
            for (size_t i = 0; i < invs->size(); ++i) {
                if ((*invs)[i]->declaration().is(*rdt)) {
                    return read_ref((*invs)[i], "RelatingType");
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

// Get the material association of an element (IfcRelAssociatesMaterial.RelatingMaterial).
IfcUtil::IfcBaseClass* get_material_simple(IfcUtil::IfcBaseClass* element) {
    auto* be = as_entity(element);
    if (!be) return nullptr;
    try {
        auto invs = be->get_inverse("HasAssociations");
        if (!invs) return nullptr;
        for (auto& inv : *invs) {
            if (is_a(inv, "IfcRelAssociatesMaterial")) {
                return read_ref(inv, "RelatingMaterial");
            }
        }
    } catch (...) {}
    return nullptr;
}

void assign_product_representation(IfcParse::IfcFile* file,
                                   IfcUtil::IfcBaseClass* product,
                                   IfcUtil::IfcBaseClass* representation) {
    auto* def = read_ref(product, "Representation");
    if (!def) {
        auto* pds_decl = file->schema()->declaration_by_name("IfcProductDefinitionShape");
        def = file->create(pds_decl);
        write_ref(product, "Representation", def);
    }
    auto reps = read_ref_list(def, "Representations");
    reps.push_back(representation);
    write_ref_list(def, "Representations", reps);
}

void assign_representation_impl(IfcParse::IfcFile* file,
                                IfcUtil::IfcBaseClass*& product /*may be re-routed*/,
                                IfcUtil::IfcBaseClass* representation) {
    if (is_a(product, "IfcProduct")) {
        auto* product_type = get_product_type(file, product);
        if (product_type) {
            auto maps = read_ref_list(product_type, "RepresentationMaps");
            std::string rep_type = read_string(representation, "RepresentationType");
            if (!maps.empty() && rep_type != "MappedRepresentation") {
                // Skip re-routing for profile/layer-based types.
                auto* mat = get_material_simple(product_type);
                bool is_profile_or_layer =
                    mat && (is_a(mat, "IfcMaterialProfileSet") ||
                            is_a(mat, "IfcMaterialLayerSet"));
                if (!is_profile_or_layer) {
                    product = product_type;
                }
            }
        }
    }

    if (is_a(product, "IfcProduct")) {
        assign_product_representation(file, product, representation);
    } else if (is_a(product, "IfcTypeProduct")) {
        auto maps = read_ref_list(product, "RepresentationMaps");
        maps.push_back(make_representation_map(file, representation));
        write_ref_list(product, "RepresentationMaps", maps);

        // Resolve related occurrences for the type and assign mapped reps.
        auto* be = as_entity(product);
        if (be) {
            std::vector<IfcUtil::IfcBaseClass*> related_objects;
            // IFC4+: Types ; IFC2X3: ObjectTypeOf
            for (const char* inv_name : {"Types", "ObjectTypeOf"}) {
                try {
                    auto invs = be->get_inverse(inv_name);
                    if (invs && invs->size() > 0) {
                        for (auto* obj : read_ref_list((*invs)[0], "RelatedObjects")) {
                            related_objects.push_back(obj);
                        }
                        break;
                    }
                } catch (...) {}
            }
            for (auto* element : related_objects) {
                auto* mapped = map_representation_impl(file, representation);
                assign_product_representation(file, element, mapped);
            }
        }
    }
    // OwnerHistory updating is handled by the caller / Python wrapper.
}

}  // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* geometry_map_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation)
{
    ifcopenshell_clear_error();
    if (!file || !representation) {
        set_error("Invalid arguments");
        return nullptr;
    }
    try {
        return map_representation_impl(file, representation);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* geometry_assign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation)
{
    ifcopenshell_clear_error();
    if (!file || !product || !representation) {
        set_error("Invalid arguments");
        return nullptr;
    }
    try {
        assign_representation_impl(file, product, representation);
        return product;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

// ---- IfcTypeProduct unassignment ---------------------------------------

} // namespace bindings
} // namespace ifcapi

namespace {

void process_shape_aspects_for_rep_map(IfcParse::IfcFile* file,
                                       IfcUtil::IfcBaseClass* rep_map) {
    std::vector<IfcUtil::IfcBaseClass*> shape_aspects;
    bool is_2x3 = file->schema() && std::string(file->schema()->name()) == "IFC2X3";
    if (is_2x3) {
        // IFC2X3: scan all IfcShapeAspect, filter by PartOfProductDefinitionShape.
        try {
            auto all = file->instances_by_type(std::string("IfcShapeAspect"));
            if (all) {
                for (auto& a : *all) {
                    if (read_ref(a, "PartOfProductDefinitionShape") == rep_map) {
                        shape_aspects.push_back(a);
                    }
                }
            }
        } catch (...) {}
    } else {
        auto* be = as_entity(rep_map);
        if (be) {
            try {
                auto invs = be->get_inverse("HasShapeAspects");
                if (invs) {
                    for (auto& a : *invs) shape_aspects.push_back(a);
                }
            } catch (...) {}
        }
    }
    for (auto* sa : shape_aspects) {
        auto reps = read_ref_list(sa, "ShapeRepresentations");
        file->removeEntity(sa);
        for (auto* r : reps) {
            remove_representation_simple(file, r);
        }
    }
}

void unassign_products_using_mapped_representation(IfcParse::IfcFile* file,
                                                   IfcUtil::IfcBaseClass* rep_map) {
    struct PR { IfcUtil::IfcBaseClass* product; IfcUtil::IfcBaseClass* rep; };
    std::vector<PR> mapped_pairs;
    std::vector<IfcUtil::IfcBaseClass*> just_reps;

    auto* be = as_entity(rep_map);
    if (!be) return;
    aggregate_of_instance::ptr map_usages;
    try { map_usages = be->get_inverse("MapUsage"); } catch (...) {}
    if (!map_usages) return;
    for (auto& mu : *map_usages) {
        if (!mu) continue;
        auto invs = file->getInverse(mu->id(), nullptr, -1);
        if (!invs) continue;
        for (auto& inv : *invs) {
            if (!inv || !is_a(inv, "IfcShapeRepresentation")) continue;
            auto* inv_be = as_entity(inv);
            if (!inv_be) continue;
            aggregate_of_instance::ptr defs;
            try { defs = inv_be->get_inverse("OfProductRepresentation"); } catch (...) {}
            if (!defs) continue;
            for (auto& def : *defs) {
                auto* def_be = as_entity(def);
                if (!def_be) continue;
                aggregate_of_instance::ptr products;
                try { products = def_be->get_inverse("ShapeOfProduct"); } catch (...) {}
                if (!products) continue;
                for (auto& product : *products) {
                    mapped_pairs.push_back({product, inv});
                    just_reps.push_back(inv);
                }
            }
        }
    }
    for (auto& pr : mapped_pairs) {
        unassign_product_representation(file, pr.product, pr.rep);
    }
    for (auto* r : just_reps) {
        remove_representation_simple(file, r);
    }
}

void unassign_type_representation(IfcParse::IfcFile* file,
                                  IfcUtil::IfcBaseClass* type_product,
                                  IfcUtil::IfcBaseClass* representation) {
    auto rep_maps = read_ref_list(type_product, "RepresentationMaps");
    IfcUtil::IfcBaseClass* matching = nullptr;
    for (auto* rm : rep_maps) {
        if (read_ref(rm, "MappedRepresentation") == representation) {
            matching = rm; break;
        }
    }
    if (!matching) return;

    unassign_products_using_mapped_representation(file, matching);

    std::vector<IfcUtil::IfcBaseClass*> remaining;
    for (auto* rm : rep_maps) if (rm != matching) remaining.push_back(rm);
    if (remaining.empty()) {
        int idx = attr_index_of(type_product, "RepresentationMaps");
        if (idx >= 0) type_product->set_attribute_value(static_cast<size_t>(idx), Blank{});
    } else {
        write_ref_list(type_product, "RepresentationMaps", remaining);
    }

    process_shape_aspects_for_rep_map(file, matching);

    // Replace MappedRepresentation with a fresh empty IfcShapeRepresentation
    // so the rep_map subgraph (which still references the *old* representation
    // shared by other things) becomes safely deletable.
    auto* sr_decl = file->schema()->declaration_by_name("IfcShapeRepresentation");
    auto* placeholder = file->create(sr_decl);
    write_ref(matching, "MappedRepresentation", placeholder);

    ifcapi::bindings::entity_remove_deep2(matching);
}

// ---- profile extents (axis-aligned 2D bbox of an IfcProfileDef) ---------

bool profile_extents(IfcParse::IfcFile* file,
                     IfcUtil::IfcBaseClass* profile,
                     double* x_out, double* y_out) {
    if (!profile || !x_out || !y_out) return false;

    auto try_attr = [&](const char* name) -> double {
        int idx = attr_index_of(profile, name);
        if (idx < 0) return 0.0;
        try {
            auto v = profile->get_attribute_value(idx);
            return (double)v;
        } catch (...) { return 0.0; }
    };

    if (is_a(profile, "IfcAsymmetricIShapeProfileDef")) {
        *x_out = try_attr("OverallWidth"); *y_out = try_attr("OverallDepth"); return true;
    }
    if (is_a(profile, "IfcCShapeProfileDef")) {
        *x_out = try_attr("Width"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcCircleProfileDef")) {
        double r = try_attr("Radius");
        *x_out = r * 2; *y_out = r * 2; return true;
    }
    if (is_a(profile, "IfcEllipseProfileDef")) {
        *x_out = try_attr("SemiAxis1") * 2; *y_out = try_attr("SemiAxis2") * 2; return true;
    }
    if (is_a(profile, "IfcIShapeProfileDef")) {
        *x_out = try_attr("OverallWidth"); *y_out = try_attr("OverallDepth"); return true;
    }
    if (is_a(profile, "IfcLShapeProfileDef")) {
        *x_out = try_attr("Width"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcRectangleProfileDef")) {
        *x_out = try_attr("XDim"); *y_out = try_attr("YDim"); return true;
    }
    if (is_a(profile, "IfcTShapeProfileDef")) {
        *x_out = try_attr("FlangeWidth"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcUShapeProfileDef")) {
        *x_out = try_attr("FlangeWidth"); *y_out = try_attr("Depth"); return true;
    }
    if (is_a(profile, "IfcZShapeProfileDef")) {
        *x_out = (try_attr("FlangeWidth") * 2) - try_attr("WebThickness");
        *y_out = try_attr("Depth"); return true;
    }
    auto* entity = profile->as<IfcUtil::IfcBaseEntity>();
    if (!file || !entity) return false;

    ifcopenshell::geometry::Settings settings;
    settings.get<ifcopenshell::geometry::settings::OutputDimensionality>().value =
        ifcopenshell::geometry::settings::CURVES_SURFACES_AND_SOLIDS;
    settings.get<ifcopenshell::geometry::settings::IteratorOutput>().value =
        ifcopenshell::geometry::settings::TRIANGULATED;

    ifcopenshell::geometry::Converter kernel(
        ifcopenshell::geometry::kernels::construct(file, "opencascade", settings),
        file,
        settings);

    IfcGeom::ConversionResults shapes = kernel.convert(profile);
    if (shapes.empty()) return false;

    auto brep = boost::shared_ptr<IfcGeom::Representation::BRep>(
        new IfcGeom::Representation::BRep(
            kernel.settings(),
            entity->declaration().name(),
            std::to_string(entity->id()),
            shapes));
    auto identity = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>();
    IfcGeom::BRepElement brep_element(
        entity->id(),
        -1,
        entity->declaration().name(),
        entity->declaration().name(),
        std::string(),
        std::string(),
        identity,
        brep,
        nullptr);
    IfcGeom::TriangulationElement triangulated(brep_element);
    const auto& verts = triangulated.geometry().verts();
    if (verts.size() < 3) return false;

    double xmin = std::numeric_limits<double>::infinity();
    double xmax = -std::numeric_limits<double>::infinity();
    double ymin = std::numeric_limits<double>::infinity();
    double ymax = -std::numeric_limits<double>::infinity();
    for (size_t i = 0; i + 2 < verts.size(); i += 3) {
        xmin = std::min(xmin, verts[i]);
        xmax = std::max(xmax, verts[i]);
        ymin = std::min(ymin, verts[i + 1]);
        ymax = std::max(ymax, verts[i + 1]);
    }

    double unit_scale = ifcapi::bindings::unit_calculate_unit_scale(file, "LENGTHUNIT");
    if (unit_scale == 0.0) unit_scale = 1.0;
    *x_out = (xmax - xmin) / unit_scale;
    *y_out = (ymax - ymin) / unit_scale;
    return true;
}

}  // namespace

namespace ifcapi {
namespace bindings {

void geometry_unassign_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* representation)
{
    if (!file || !product || !representation) return;
    try {
        if (is_a(product, "IfcProduct")) {
            unassign_product_representation(file, product, representation);
        } else if (is_a(product, "IfcTypeProduct")) {
            unassign_type_representation(file, product, representation);
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    } catch (...) {
        set_error("geometry_unassign_representation: unknown exception");
    }
}

IfcUtil::IfcBaseClass* geometry_copy_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* source,
    IfcUtil::IfcBaseClass* target,
    const char* context_identifier)
{
    ifcopenshell_clear_error();
    if (!file || !source || !target) {
        set_error("Invalid arguments");
        return nullptr;
    }
    try {
        std::string ctx = context_identifier ? context_identifier : "Body";
        auto* source_rep = get_representation_for_product(source, ctx);
        if (!source_rep) return nullptr;

        std::unordered_map<unsigned, IfcUtil::IfcBaseClass*> memo;
        auto* new_rep = deep_copy_entity(file, source_rep,
                                         {"IfcGeometricRepresentationContext"}, memo);

        auto* existing = get_representation_for_product(target, ctx);
        if (existing) {
            if (is_a(target, "IfcProduct")) {
                unassign_product_representation(file, target, existing);
            }
            remove_representation_simple(file, existing);
        }

        assign_representation_impl(file, target, new_rep);
        return new_rep;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

bool type_map_type_representations(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* related_object,
    IfcUtil::IfcBaseClass* relating_type)
{
    ifcopenshell_clear_error();
    if (!file || !related_object || !relating_type) {
        set_error("Invalid arguments");
        return false;
    }
    try {
        auto maps = read_ref_list(relating_type, "RepresentationMaps");
        if (maps.empty()) return true;

        auto* def = read_ref(related_object, "Representation");
        if (def) {
            auto reps = read_ref_list(def, "Representations");
            for (auto* rep : reps) {
                unassign_product_representation(file, related_object, rep);
                remove_representation_simple(file, rep);
            }
        }

        for (auto* rmap : maps) {
            auto* rep = read_ref(rmap, "MappedRepresentation");
            if (!rep) continue;
            auto* mapped = map_representation_impl(file, rep);
            assign_product_representation(file, related_object, mapped);
        }
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

std::vector<double> geometry_profile_extents(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* profile)
{
    ifcopenshell_clear_error();
    if (!file || !profile) {
        set_error("Invalid arguments");
        return {};
    }
    try {
        double x = 0.0;
        double y = 0.0;
        if (!profile_extents(file, profile, &x, &y)) {
            return {};
        }
        return {x, y};
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
