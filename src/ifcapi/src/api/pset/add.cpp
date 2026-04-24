// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <cstring>
#include <set>
#include <string>
#include <vector>

namespace {

inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

int find_attr_index(const IfcParse::entity* decl, const char* name) {
    if (!decl) return -1;
    auto attrs = decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (attrs[i]->name() == name) return static_cast<int>(i);
    }
    return -1;
}

bool entity_is_a(IfcUtil::IfcBaseClass* e, const char* name) {
    if (!e) return false;
    return e->declaration().is(name);
}

std::vector<IfcUtil::IfcBaseClass*> get_inverse(IfcUtil::IfcBaseClass* e, const char* attr) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return result;
    try {
        auto agg = be->get_inverse(attr);
        if (agg) {
            for (size_t i = 0; i < agg->size(); ++i) result.push_back((*agg)[i]);
        }
    } catch (...) {}
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> get_ref_aggregate(IfcUtil::IfcBaseClass* e, int attr_idx) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!e || attr_idx < 0) return result;
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(attr_idx));
        if (val.isNull()) return result;
        auto agg = (aggregate_of_instance::ptr)val;
        if (agg) {
            for (auto& it : *agg) result.push_back(it);
        }
    } catch (...) {}
    return result;
}

void set_ref_aggregate(IfcUtil::IfcBaseClass* e, int attr_idx,
                       const std::vector<IfcUtil::IfcBaseClass*>& items) {
    if (!e || attr_idx < 0) return;
    auto agg = aggregate_of_instance::ptr(new aggregate_of_instance());
    for (auto* p : items) agg->push(p);
    e->set_attribute_value(static_cast<size_t>(attr_idx), agg);
}

std::string read_string_attr(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return std::string();
    auto* decl = be->declaration().as_entity();
    int idx = find_attr_index(decl, attr);
    if (idx < 0) return std::string();
    try {
        auto val = e->get_attribute_value(static_cast<size_t>(idx));
        if (val.isNull()) return std::string();
        return (std::string)val;
    } catch (...) {
        return std::string();
    }
}

// Find an existing pset by name on `product`.
IfcUtil::IfcBaseClass* find_existing_pset_on_object(IfcUtil::IfcBaseClass* product, const std::string& name) {
    for (auto* rel : get_inverse(product, "IsDefinedBy")) {
        if (!rel || !entity_is_a(rel, "IfcRelDefinesByProperties")) continue;
        IfcUtil::IfcBaseClass* def = nullptr;
        try {
            auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(rel);
            if (!be) continue;
            auto* decl = be->declaration().as_entity();
            int idx = find_attr_index(decl, "RelatingPropertyDefinition");
            if (idx < 0) continue;
            auto val = rel->get_attribute_value(static_cast<size_t>(idx));
            if (val.isNull()) continue;
            def = (IfcUtil::IfcBaseClass*)val;
        } catch (...) { continue; }
        if (def && read_string_attr(def, "Name") == name) return def;
    }
    return nullptr;
}

IfcUtil::IfcBaseClass* find_existing_pset_on_type(IfcUtil::IfcBaseClass* product, const std::string& name) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(product);
    if (!be) return nullptr;
    auto* decl = be->declaration().as_entity();
    int idx = find_attr_index(decl, "HasPropertySets");
    if (idx < 0) return nullptr;
    auto items = get_ref_aggregate(product, idx);
    for (auto* d : items) {
        if (d && read_string_attr(d, "Name") == name) return d;
    }
    return nullptr;
}

// Wire `pset` to `product` (occurrence) — find or create IfcRelDefinesByProperties.
void assign_pset_to_object(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* product,
                           IfcUtil::IfcBaseClass* pset, IfcUtil::IfcBaseClass* owner_history) {
    bool is_ifc2x3 = (std::string(file->schema()->name()) == "IFC2X3");
    const char* inv_name = is_ifc2x3 ? "PropertyDefinitionOf" : "DefinesOccurrence";
    auto rels = get_inverse(pset, inv_name);
    IfcUtil::IfcBaseClass* rel = rels.empty() ? nullptr : rels.front();

    if (rel) {
        auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(rel);
        auto* rel_decl = be->declaration().as_entity();
        int ro_idx = find_attr_index(rel_decl, "RelatedObjects");
        auto current = get_ref_aggregate(rel, ro_idx);
        std::set<IfcUtil::IfcBaseClass*> seen(current.begin(), current.end());
        if (seen.insert(product).second) {
            current.push_back(product);
            set_ref_aggregate(rel, ro_idx, current);
        }
        return;
    }

    const auto* rel_decl_p = file->schema()->declaration_by_name("IfcRelDefinesByProperties");
    auto* new_rel = file->create(rel_decl_p);
    auto* rel_entity_decl = rel_decl_p->as_entity();
    int gi = find_attr_index(rel_entity_decl, "GlobalId");
    if (gi >= 0) new_rel->set_attribute_value(static_cast<size_t>(gi), ifcapi::guid_new());
    if (owner_history) {
        int oh = find_attr_index(rel_entity_decl, "OwnerHistory");
        if (oh >= 0) new_rel->set_attribute_value(static_cast<size_t>(oh), owner_history);
    }
    int ro = find_attr_index(rel_entity_decl, "RelatedObjects");
    set_ref_aggregate(new_rel, ro, {product});
    int rp = find_attr_index(rel_entity_decl, "RelatingPropertyDefinition");
    if (rp >= 0) new_rel->set_attribute_value(static_cast<size_t>(rp), pset);
}

void append_to_type_pset_list(IfcUtil::IfcBaseClass* product, IfcUtil::IfcBaseClass* pset) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(product);
    if (!be) return;
    auto* decl = be->declaration().as_entity();
    int idx = find_attr_index(decl, "HasPropertySets");
    if (idx < 0) return;
    auto current = get_ref_aggregate(product, idx);
    current.push_back(pset);
    set_ref_aggregate(product, idx, current);
}

// Generic "create a definition entity with name/owner/guid attributes".
IfcUtil::IfcBaseClass* create_named_definition(IfcParse::IfcFile* file, const char* ifc_class,
                                               const std::string& name,
                                               IfcUtil::IfcBaseClass* owner_history,
                                               const char* method_of_measurement = nullptr) {
    const auto* decl = file->schema()->declaration_by_name(ifc_class);
    auto* def = file->create(decl);
    auto* entity_decl = decl->as_entity();
    int gi = find_attr_index(entity_decl, "GlobalId");
    if (gi >= 0) def->set_attribute_value(static_cast<size_t>(gi), ifcapi::guid_new());
    if (owner_history) {
        int oh = find_attr_index(entity_decl, "OwnerHistory");
        if (oh >= 0) def->set_attribute_value(static_cast<size_t>(oh), owner_history);
    }
    int ni = find_attr_index(entity_decl, "Name");
    if (ni >= 0) def->set_attribute_value(static_cast<size_t>(ni), name);
    if (method_of_measurement) {
        int mi = find_attr_index(entity_decl, "MethodOfMeasurement");
        if (mi >= 0) def->set_attribute_value(static_cast<size_t>(mi), std::string(method_of_measurement));
    }
    return def;
}

}  // namespace

extern "C" {

IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_pset_add_pset(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* product_h,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history_h,
    const char* ifc2x3_subclass)
{
    if (!file_h || !product_h || !name) {
        set_error("ifcopenshell_api_pset_add_pset: missing required argument");
        return nullptr;
    }
    auto* file = file_h->ptr;
    auto* product = product_h->ptr;
    auto* owner_history = owner_history_h ? owner_history_h->ptr : nullptr;
    if (!file || !product) {
        set_error("ifcopenshell_api_pset_add_pset: NULL underlying pointer");
        return nullptr;
    }
    try {
        std::string nm(name);

        if (entity_is_a(product, "IfcObject") || entity_is_a(product, "IfcContext")) {
            if (auto* existing = find_existing_pset_on_object(product, nm)) {
                return ifcopenshell::capi::wrap_instance(existing);
            }
            auto* pset = create_named_definition(file, "IfcPropertySet", nm, owner_history);
            assign_pset_to_object(file, product, pset, owner_history);
            return ifcopenshell::capi::wrap_instance(pset);
        }

        if (entity_is_a(product, "IfcTypeObject")) {
            if (auto* existing = find_existing_pset_on_type(product, nm)) {
                return ifcopenshell::capi::wrap_instance(existing);
            }
            auto* pset = create_named_definition(file, "IfcPropertySet", nm, owner_history);
            append_to_type_pset_list(product, pset);
            return ifcopenshell::capi::wrap_instance(pset);
        }

        // Material / Profile paths.
        bool is_ifc2x3 = (std::string(file->schema()->name()) == "IFC2X3");

        if (entity_is_a(product, "IfcMaterial") || entity_is_a(product, "IfcMaterialDefinition")) {
            std::string ifc_class;
            if (is_ifc2x3) {
                ifc_class = (ifc2x3_subclass && *ifc2x3_subclass)
                    ? std::string(ifc2x3_subclass)
                    : std::string("IfcExtendedMaterialProperties");
            } else {
                ifc_class = "IfcMaterialProperties";
            }
            std::vector<IfcUtil::IfcBaseClass*> defs;
            if (is_ifc2x3) {
                try {
                    auto all = file->instances_by_type(std::string("IfcMaterialProperties"));
                    if (all) {
                        for (auto& it : *all) {
                            auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(it);
                            if (!be) continue;
                            int mi = find_attr_index(be->declaration().as_entity(), "Material");
                            if (mi < 0) continue;
                            try {
                                auto v = it->get_attribute_value(static_cast<size_t>(mi));
                                if (!v.isNull() && (IfcUtil::IfcBaseClass*)v == product) defs.push_back(it);
                            } catch (...) {}
                        }
                    }
                } catch (...) {}
            } else {
                defs = get_inverse(product, "HasProperties");
            }
            for (auto* d : defs) {
                if (read_string_attr(d, "Name") == nm) {
                    return ifcopenshell::capi::wrap_instance(d);
                }
            }

            const auto* decl = file->schema()->declaration_by_name(ifc_class);
            auto* def = file->create(decl);
            auto* entity_decl = decl->as_entity();
            int mi = find_attr_index(entity_decl, "Material");
            if (mi >= 0) def->set_attribute_value(static_cast<size_t>(mi), product);
            int ni = find_attr_index(entity_decl, "Name");
            if (ni >= 0) def->set_attribute_value(static_cast<size_t>(ni), nm);
            return ifcopenshell::capi::wrap_instance(def);
        }

        if (entity_is_a(product, "IfcProfileDef")) {
            if (!is_ifc2x3) {
                for (auto* d : get_inverse(product, "HasProperties")) {
                    if (read_string_attr(d, "Name") == nm) {
                        return ifcopenshell::capi::wrap_instance(d);
                    }
                }
            }
            std::string ifc_class;
            if (is_ifc2x3) {
                ifc_class = (ifc2x3_subclass && *ifc2x3_subclass)
                    ? std::string(ifc2x3_subclass)
                    : std::string("IfcGeneralProfileProperties");
            } else {
                ifc_class = "IfcProfileProperties";
            }
            const auto* decl = file->schema()->declaration_by_name(ifc_class);
            auto* def = file->create(decl);
            auto* entity_decl = decl->as_entity();
            int pi = find_attr_index(entity_decl, "ProfileDefinition");
            if (pi >= 0) def->set_attribute_value(static_cast<size_t>(pi), product);
            if (!is_ifc2x3) {
                int ni = find_attr_index(entity_decl, "Name");
                if (ni >= 0) def->set_attribute_value(static_cast<size_t>(ni), nm);
            }
            return ifcopenshell::capi::wrap_instance(def);
        }

        set_error(std::string("Class '") + product->declaration().name() + "' doesn't support adding a property set.");
        return nullptr;
    } catch (const std::exception& e) {
        set_error(std::string("ifcopenshell_api_pset_add_pset: ") + e.what());
        return nullptr;
    }
}

IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_api_pset_add_qto(
    ifcopenshell_ifc_file_t* file_h,
    ifcopenshell_ifc_instance_t* product_h,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history_h)
{
    if (!file_h || !product_h || !name) {
        set_error("ifcopenshell_api_pset_add_qto: missing required argument");
        return nullptr;
    }
    auto* file = file_h->ptr;
    auto* product = product_h->ptr;
    auto* owner_history = owner_history_h ? owner_history_h->ptr : nullptr;
    if (!file || !product) {
        set_error("ifcopenshell_api_pset_add_qto: NULL underlying pointer");
        return nullptr;
    }
    try {
        std::string nm(name);
        const char* mom = nullptr;
        std::string base = "BaseQuantities";
        if (nm.size() >= base.size() && nm.compare(nm.size() - base.size(), base.size(), base) == 0) {
            mom = "BaseQuantities";
        }

        if (entity_is_a(product, "IfcObject") || entity_is_a(product, "IfcContext")) {
            if (auto* existing = find_existing_pset_on_object(product, nm)) {
                return ifcopenshell::capi::wrap_instance(existing);
            }
            auto* qto = create_named_definition(file, "IfcElementQuantity", nm, owner_history, mom);

            // Always create a fresh IfcRelDefinesByProperties for quantities.
            const auto* rel_decl = file->schema()->declaration_by_name("IfcRelDefinesByProperties");
            auto* rel = file->create(rel_decl);
            auto* rel_entity_decl = rel_decl->as_entity();
            int gi = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi >= 0) rel->set_attribute_value(static_cast<size_t>(gi), ifcapi::guid_new());
            if (owner_history) {
                int oh = find_attr_index(rel_entity_decl, "OwnerHistory");
                if (oh >= 0) rel->set_attribute_value(static_cast<size_t>(oh), owner_history);
            }
            int ro = find_attr_index(rel_entity_decl, "RelatedObjects");
            set_ref_aggregate(rel, ro, {product});
            int rp = find_attr_index(rel_entity_decl, "RelatingPropertyDefinition");
            if (rp >= 0) rel->set_attribute_value(static_cast<size_t>(rp), qto);
            return ifcopenshell::capi::wrap_instance(qto);
        }

        if (entity_is_a(product, "IfcTypeObject")) {
            if (auto* existing = find_existing_pset_on_type(product, nm)) {
                return ifcopenshell::capi::wrap_instance(existing);
            }
            auto* qto = create_named_definition(file, "IfcElementQuantity", nm, owner_history, mom);
            append_to_type_pset_list(product, qto);
            return ifcopenshell::capi::wrap_instance(qto);
        }

        set_error(std::string("Class '") + product->declaration().name() + "' doesn't support adding a quantity set.");
        return nullptr;
    } catch (const std::exception& e) {
        set_error(std::string("ifcopenshell_api_pset_add_qto: ") + e.what());
        return nullptr;
    }
}

}  // extern "C"
