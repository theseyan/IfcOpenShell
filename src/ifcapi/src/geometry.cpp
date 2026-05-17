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
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
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
#include <unordered_set>
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

bool is_terminal_connection(const std::string& connection_type) {
    return connection_type == "ATSTART" || connection_type == "ATEND";
}

std::vector<IfcUtil::IfcBaseClass*> inverse_refs(IfcUtil::IfcBaseClass* entity, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(entity, attr);
}

std::vector<IfcUtil::IfcBaseClass*> all_inverse_refs(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || !entity->id()) {
        return result;
    }
    try {
        auto inverses = file->getInverse(entity->id(), nullptr, -1);
        if (!inverses) {
            return result;
        }
        for (auto* inverse : *inverses) {
            if (inverse) {
                result.push_back(inverse);
            }
        }
    } catch (...) {
    }
    return result;
}

void append_unique_connection(
    std::vector<IfcUtil::IfcBaseClass*>& connections,
    std::unordered_set<IfcUtil::IfcBaseClass*>& seen,
    IfcUtil::IfcBaseClass* connection)
{
    if (connection && seen.insert(connection).second) {
        connections.push_back(connection);
    }
}

void remove_connections_with_history(IfcParse::IfcFile* file, const std::vector<IfcUtil::IfcBaseClass*>& connections) {
    for (auto* connection : connections) {
        ifcapi::detail::remove_with_history(file, connection);
    }
}

void write_optional_string(IfcUtil::IfcBaseClass* entity, const char* attr, const char* value, bool has_value) {
    int idx = attr_index_of(entity, attr);
    if (idx < 0) {
        return;
    }
    if (has_value) {
        entity->set_attribute_value(static_cast<size_t>(idx), std::string(value ? value : ""));
    } else {
        entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
    }
}

void write_bool(IfcUtil::IfcBaseClass* entity, const char* attr, bool value) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

void write_empty_int_aggregate(IfcUtil::IfcBaseClass* entity, const char* attr) {
    int idx = attr_index_of(entity, attr);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), std::vector<int>());
    }
}

IfcUtil::IfcBaseClass* create_shape_representation_like(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation,
    const std::vector<IfcUtil::IfcBaseClass*>& items)
{
    auto* aspect_rep = file->create(file->schema()->declaration_by_name("IfcShapeRepresentation"));
    write_ref(aspect_rep, "ContextOfItems", read_ref(representation, "ContextOfItems"));
    write_string(aspect_rep, "RepresentationIdentifier", read_string(representation, "RepresentationIdentifier"));
    write_string(aspect_rep, "RepresentationType", read_string(representation, "RepresentationType"));
    write_ref_list(aspect_rep, "Items", items);
    return aspect_rep;
}

bool is_boolean_operand(IfcUtil::IfcBaseClass* item) {
    return is_a(item, "IfcBooleanResult") || is_a(item, "IfcCsgPrimitive3D") || is_a(item, "IfcHalfSpaceSolid") ||
        is_a(item, "IfcSolidModel") || is_a(item, "IfcTessellatedFaceSet");
}

IfcUtil::IfcBaseClass* create_boolean_result(
    IfcParse::IfcFile* file,
    const std::string& operator_type,
    IfcUtil::IfcBaseClass* first,
    IfcUtil::IfcBaseClass* second)
{
    const bool should_clip = operator_type == "DIFFERENCE" && is_a(second, "IfcHalfSpaceSolid") &&
        (is_a(first, "IfcSweptAreaSolid") || is_a(first, "IfcSweptDiskSolid") || is_a(first, "IfcBooleanClippingResult"));
    auto* boolean = file->create(file->schema()->declaration_by_name(should_clip ? "IfcBooleanClippingResult" : "IfcBooleanResult"));
    ifcapi::detail::write_enum_attr(boolean, "Operator", operator_type);
    write_ref(boolean, "FirstOperand", first);
    write_ref(boolean, "SecondOperand", second);
    return boolean;
}

void geometry_remove_boolean_impl(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* item) {
    if (!is_a(item, "IfcBooleanResult")) {
        for (auto* inverse : all_inverse_refs(file, item)) {
            if (is_a(inverse, "IfcBooleanResult")) {
                geometry_remove_boolean_impl(file, inverse);
            }
        }
        return;
    }

    std::vector<IfcUtil::IfcBaseClass*> representations;
    std::vector<IfcUtil::IfcBaseClass*> queue = all_inverse_refs(file, item);
    while (!queue.empty()) {
        auto* inverse = queue.back();
        queue.pop_back();
        if (is_a(inverse, "IfcShapeRepresentation")) {
            if (!ifcapi::detail::contains_ref(representations, inverse)) {
                representations.push_back(inverse);
            }
        } else if (is_a(inverse, "IfcBooleanResult") || is_a(inverse, "IfcCsgSolid")) {
            auto more = all_inverse_refs(file, inverse);
            queue.insert(queue.end(), more.begin(), more.end());
        }
    }

    auto* first = read_ref(item, "FirstOperand");
    auto* second = read_ref(item, "SecondOperand");
    for (auto* inverse : all_inverse_refs(file, item)) {
        ifcapi::detail::replace_attribute_reference(inverse, item, first);
    }

    for (auto* representation : representations) {
        auto items = read_ref_list(representation, "Items");
        items.push_back(second);
        write_ref_list(representation, "Items", items);
    }

    file->removeEntity(item);
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

std::vector<IfcUtil::IfcBaseClass*> geometry_add_boolean(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* first_item,
    const std::vector<const IfcUtil::IfcBaseClass*>& second_items,
    const std::string& operator_type)
{
    ifcopenshell_clear_error();
    std::vector<IfcUtil::IfcBaseClass*> booleans;
    if (!file || !is_boolean_operand(first_item)) {
        return booleans;
    }

    try {
        auto* original_first_item = first_item;
        auto filtered_second_items = ifcapi::detail::to_mutable_refs(second_items);
        filtered_second_items.erase(
            std::remove_if(
                filtered_second_items.begin(),
                filtered_second_items.end(),
                [&](IfcUtil::IfcBaseClass* item) { return item == first_item || !is_boolean_operand(item); }),
            filtered_second_items.end());

        while (true) {
            bool is_part_of_boolean = false;
            for (auto* inverse : all_inverse_refs(file, first_item)) {
                if (!is_a(inverse, "IfcBooleanResult")) {
                    continue;
                }
                is_part_of_boolean = true;
                first_item = inverse;
                auto* first_operand = read_ref(inverse, "FirstOperand");
                auto* second_operand = read_ref(inverse, "SecondOperand");
                if (first_operand == original_first_item) {
                    filtered_second_items.erase(
                        std::remove(filtered_second_items.begin(), filtered_second_items.end(), second_operand),
                        filtered_second_items.end());
                } else if (second_operand == original_first_item) {
                    filtered_second_items.erase(
                        std::remove(filtered_second_items.begin(), filtered_second_items.end(), first_operand),
                        filtered_second_items.end());
                }
                break;
            }
            if (!is_part_of_boolean) {
                break;
            }
        }

        if (filtered_second_items.empty()) {
            return booleans;
        }

        std::vector<IfcUtil::IfcBaseClass*> to_replace;
        for (auto* inverse : all_inverse_refs(file, first_item)) {
            if (is_a(inverse, "IfcShapeRepresentation") || is_a(inverse, "IfcBooleanResult")) {
                if (!ifcapi::detail::contains_ref(to_replace, inverse)) {
                    to_replace.push_back(inverse);
                }
            }
        }

        auto* first = first_item;
        for (auto* second_item : filtered_second_items) {
            first = create_boolean_result(file, operator_type, first, second_item);
            booleans.push_back(first);
        }

        for (auto* inverse : to_replace) {
            ifcapi::detail::replace_attribute_reference(inverse, first_item, first);
        }
        return booleans;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

IfcUtil::IfcBaseClass* geometry_add_shape_aspect(
    IfcParse::IfcFile* file,
    const std::string& name,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* representation,
    IfcUtil::IfcBaseClass* part_of_product,
    const char* description,
    bool has_description)
{
    ifcopenshell_clear_error();
    if (!file || !representation || !part_of_product) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        auto shape_items = ifcapi::detail::to_mutable_refs(items);
        auto* context = read_ref(representation, "ContextOfItems");
        IfcUtil::IfcBaseClass* shape_aspect = nullptr;

        for (auto* aspect : ifcapi::detail::read_inverse_aggregate(part_of_product, "HasShapeAspects")) {
            if (read_string(aspect, "Name") == name) {
                shape_aspect = aspect;
                break;
            }
        }

        if (shape_aspect) {
            write_optional_string(shape_aspect, "Description", description, has_description);
            bool has_context_representation = false;
            for (auto* aspect_rep : read_ref_list(shape_aspect, "ShapeRepresentations")) {
                if (read_ref(aspect_rep, "ContextOfItems") == context) {
                    auto existing_items = read_ref_list(aspect_rep, "Items");
                    ifcapi::detail::append_unique(existing_items, shape_items);
                    write_ref_list(aspect_rep, "Items", existing_items);
                    has_context_representation = true;
                    break;
                }
            }
            if (!has_context_representation) {
                auto aspect_reps = read_ref_list(shape_aspect, "ShapeRepresentations");
                aspect_reps.push_back(create_shape_representation_like(file, representation, shape_items));
                write_ref_list(shape_aspect, "ShapeRepresentations", aspect_reps);
            }
        } else {
            auto* aspect_rep = create_shape_representation_like(file, representation, shape_items);
            shape_aspect = file->create(file->schema()->declaration_by_name("IfcShapeAspect"));
            write_ref_list(shape_aspect, "ShapeRepresentations", {aspect_rep});
            write_string(shape_aspect, "Name", name);
            write_optional_string(shape_aspect, "Description", description, has_description);
            write_bool(shape_aspect, "ProductDefinitional", true);
            write_ref(shape_aspect, "PartOfProductDefinitionShape", part_of_product);
        }

        for (auto* aspect : ifcapi::detail::read_inverse_aggregate(part_of_product, "HasShapeAspects")) {
            if (aspect == shape_aspect) {
                continue;
            }
            auto aspect_reps = read_ref_list(aspect, "ShapeRepresentations");
            for (auto it = aspect_reps.begin(); it != aspect_reps.end();) {
                auto* aspect_rep = *it;
                if (read_ref(aspect_rep, "ContextOfItems") != context) {
                    ++it;
                    continue;
                }
                auto aspect_items = read_ref_list(aspect_rep, "Items");
                aspect_items.erase(
                    std::remove_if(
                        aspect_items.begin(),
                        aspect_items.end(),
                        [&](IfcUtil::IfcBaseClass* item) {
                            return ifcapi::detail::contains_ref(shape_items, item);
                        }),
                    aspect_items.end());
                if (aspect_items.empty()) {
                    file->removeEntity(aspect_rep);
                    it = aspect_reps.erase(it);
                } else {
                    write_ref_list(aspect_rep, "Items", aspect_items);
                    ++it;
                }
            }
            write_ref_list(aspect, "ShapeRepresentations", aspect_reps);
            if (aspect_reps.empty()) {
                file->removeEntity(aspect);
            }
        }

        return shape_aspect;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void geometry_remove_boolean(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* item) {
    ifcopenshell_clear_error();
    if (!file || !item) {
        return;
    }

    try {
        geometry_remove_boolean_impl(file, item);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

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

IfcUtil::IfcBaseClass* geometry_connect_element(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element,
    const char* description,
    bool has_description,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        std::vector<IfcUtil::IfcBaseClass*> incompatible_connections;
        std::unordered_set<IfcUtil::IfcBaseClass*> seen;
        for (auto* rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatingElement") == related_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(related_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatedElement") == relating_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);

        for (auto* rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatedElement") == related_element) {
                write_optional_string(rel, "Description", description, has_description);
                return rel;
            }
        }

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelConnectsElements");
        auto* rel = file->create(rel_decl);
        if (!rel) {
            set_error("Failed to create IfcRelConnectsElements");
            return nullptr;
        }
        write_string(rel, "GlobalId", ifcapi::guid_new());
        write_ref(rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        write_optional_string(rel, "Description", description, has_description);
        write_ref(rel, "RelatingElement", relating_element);
        write_ref(rel, "RelatedElement", related_element);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void geometry_disconnect_element(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        return;
    }

    try {
        std::vector<IfcUtil::IfcBaseClass*> incompatible_connections;
        std::unordered_set<IfcUtil::IfcBaseClass*> seen;
        for (auto* rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatedElement") == related_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatingElement") == related_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(related_element, "ConnectedTo")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatedElement") == relating_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(related_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsElements") && read_ref(rel, "RelatingElement") == relating_element) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

IfcUtil::IfcBaseClass* geometry_connect_path(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element,
    const std::string& relating_connection,
    const std::string& related_connection,
    const char* description,
    bool has_description,
    IfcUtil::IfcBaseClass* connection_geometry,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file || !relating_element || !related_element) {
        set_error("Invalid arguments");
        return nullptr;
    }

    try {
        const std::string& relating_type = relating_connection;
        const std::string& related_type = related_connection;
        std::vector<IfcUtil::IfcBaseClass*> incompatible_connections;
        std::unordered_set<IfcUtil::IfcBaseClass*> seen;

        for (auto* rel : inverse_refs(relating_element, "ConnectedTo")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) {
                continue;
            }
            if (read_ref(rel, "RelatedElement") == related_element ||
                (is_terminal_connection(read_string(rel, "RelatingConnectionType")) &&
                    read_string(rel, "RelatingConnectionType") == relating_type)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(relating_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsPathElements") &&
                is_terminal_connection(read_string(rel, "RelatedConnectionType")) &&
                read_string(rel, "RelatedConnectionType") == relating_type) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(related_element, "ConnectedFrom")) {
            if (is_a(rel, "IfcRelConnectsPathElements") &&
                is_terminal_connection(read_string(rel, "RelatedConnectionType")) &&
                read_string(rel, "RelatedConnectionType") == related_type) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        for (auto* rel : inverse_refs(related_element, "ConnectedTo")) {
            if (!is_a(rel, "IfcRelConnectsPathElements")) {
                continue;
            }
            if (read_ref(rel, "RelatedElement") == relating_element ||
                (is_terminal_connection(read_string(rel, "RelatingConnectionType")) &&
                    read_string(rel, "RelatingConnectionType") == related_type)) {
                append_unique_connection(incompatible_connections, seen, rel);
            }
        }
        remove_connections_with_history(file, incompatible_connections);

        const auto* rel_decl = file->schema()->declaration_by_name("IfcRelConnectsPathElements");
        auto* rel = file->create(rel_decl);
        if (!rel) {
            set_error("Failed to create IfcRelConnectsPathElements");
            return nullptr;
        }
        write_string(rel, "GlobalId", ifcapi::guid_new());
        write_ref(rel, "OwnerHistory", ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
        write_optional_string(rel, "Description", description, has_description);
        write_ref(rel, "ConnectionGeometry", connection_geometry);
        write_ref(rel, "RelatingElement", relating_element);
        write_ref(rel, "RelatedElement", related_element);
        ifcapi::detail::write_enum_attr(rel, "RelatingConnectionType", relating_type);
        ifcapi::detail::write_enum_attr(rel, "RelatedConnectionType", related_type);
        write_empty_int_aggregate(rel, "RelatingPriorities");
        write_empty_int_aggregate(rel, "RelatedPriorities");
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void geometry_disconnect_path(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    const char* connection_type,
    bool has_connection_type,
    IfcUtil::IfcBaseClass* relating_element,
    IfcUtil::IfcBaseClass* related_element)
{
    ifcopenshell_clear_error();
    if (!file) {
        return;
    }

    try {
        std::vector<IfcUtil::IfcBaseClass*> connections;
        std::unordered_set<IfcUtil::IfcBaseClass*> seen;
        if (has_connection_type && element) {
            const std::string type = connection_type ? connection_type : "";
            for (auto* rel : inverse_refs(element, "ConnectedTo")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && read_string(rel, "RelatingConnectionType") == type) {
                    append_unique_connection(connections, seen, rel);
                }
            }
            for (auto* rel : inverse_refs(element, "ConnectedFrom")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && read_string(rel, "RelatedConnectionType") == type) {
                    append_unique_connection(connections, seen, rel);
                }
            }
        } else if (related_element) {
            for (auto* rel : inverse_refs(relating_element, "ConnectedTo")) {
                if (is_a(rel, "IfcRelConnectsPathElements") && read_ref(rel, "RelatedElement") == related_element) {
                    append_unique_connection(connections, seen, rel);
                }
            }
        } else {
            set_error("cannot access local variable 'connections' where it is not associated with a value");
            return;
        }
        remove_connections_with_history(file, connections);
    } catch (const std::exception& e) {
        set_error(e.what());
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

void geometry_remove_representation(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* representation,
    bool should_keep_named_profiles)
{
    if (!file || !representation) return;
    bool is_ifc2x3 = file->schema() && file->schema()->name() == "IFC2X3";
    std::vector<IfcUtil::IfcBaseClass*> styled_items;
    std::vector<IfcUtil::IfcBaseClass*> presentation_layer_assignments_items;
    std::vector<IfcUtil::IfcBaseClass*> presentation_layer_assignments_reps;
    std::vector<IfcUtil::IfcBaseClass*> textures;
    std::vector<IfcUtil::IfcBaseClass*> colours;
    std::vector<IfcUtil::IfcBaseClass*> named_profiles;

    auto traversed = file->traverse(representation, -1);
    if (traversed) {
        for (auto* subelement : *traversed) {
            if (is_a(subelement, "IfcRepresentationItem")) {
                ifcapi::detail::append_unique(
                    styled_items,
                    ifcapi::detail::read_inverse_aggregate(subelement, "StyledByItem"));
                ifcapi::detail::append_unique(
                    presentation_layer_assignments_items,
                    ifcapi::detail::read_inverse_aggregate(
                        subelement,
                        is_ifc2x3 ? "LayerAssignments" : "LayerAssignment"));
                if (is_a(subelement, "IfcTessellatedFaceSet")) {
                    ifcapi::detail::append_unique(
                        textures,
                        ifcapi::detail::read_inverse_aggregate(subelement, "HasTextures"));
                    ifcapi::detail::append_unique(
                        colours,
                        ifcapi::detail::read_inverse_aggregate(subelement, "HasColours"));
                }
            } else if (is_a(subelement, "IfcRepresentation")) {
                ifcapi::detail::append_unique(
                    presentation_layer_assignments_reps,
                    ifcapi::detail::read_inverse_aggregate(subelement, "LayerAssignments"));
            } else if (is_a(subelement, "IfcProfileDef") && !read_string(subelement, "ProfileName").empty()) {
                ifcapi::detail::append_unique(named_profiles, subelement);
            }
        }
    }

    auto do_not_delete = ifcapi::detail::instances_by_type(file, "IfcGeometricRepresentationContext");
    if (should_keep_named_profiles) {
        ifcapi::detail::append_unique(do_not_delete, named_profiles);
    }

    std::vector<IfcUtil::IfcBaseClass*> also_consider = presentation_layer_assignments_reps;
    for (auto* layer : presentation_layer_assignments_items) {
        if (!ifcapi::detail::contains_ref(presentation_layer_assignments_reps, layer)) {
            also_consider.push_back(layer);
        }
    }
    ifcapi::detail::append_unique(also_consider, styled_items);
    ifcapi::detail::append_unique(also_consider, textures);

    entity_remove_deep2_ex(
        representation,
        ifcapi::detail::to_const_refs(also_consider),
        ifcapi::detail::to_const_refs(do_not_delete));

    for (auto* texture : textures) {
        entity_remove_deep2(texture);
    }
    for (auto* colour : colours) {
        entity_remove_deep2(colour);
    }

    for (auto* styled_item : styled_items) {
        if (!ifcapi::detail::exists_in_file(file, styled_item)) continue;
        auto* item = read_ref(styled_item, "Item");
        if (!item || !ifcapi::detail::exists_in_file(file, item)) {
            file->removeEntity(styled_item);
        }
    }

    std::vector<IfcUtil::IfcBaseClass*> presentation_layer_assignments = presentation_layer_assignments_reps;
    ifcapi::detail::append_unique(presentation_layer_assignments, presentation_layer_assignments_items);
    for (auto* layer : presentation_layer_assignments) {
        if (!ifcapi::detail::exists_in_file(file, layer)) continue;
        auto assigned_items = read_ref_list(layer, "AssignedItems");
        bool all_deleted = true;
        for (auto* item : assigned_items) {
            if (ifcapi::detail::exists_in_file(file, item)) {
                all_deleted = false;
                break;
            }
        }
        if (all_deleted) {
            file->removeEntity(layer);
        }
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
