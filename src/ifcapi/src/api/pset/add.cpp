// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/exception.h"

#include <cstring>
#include <set>
#include <string>
#include <vector>

namespace {

using ifcapi::detail::set_error;

using ifcapi::detail::find_attr_index;
using ifcapi::detail::get_ref_aggregate;
using ifcapi::detail::read_string_attr;
using ifcapi::detail::set_ref_aggregate;

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
                           IfcUtil::IfcBaseClass* pset,
                           IfcUtil::IfcBaseClass* owner_history,
                           IfcUtil::IfcBaseClass* user,
                           IfcUtil::IfcBaseClass* application) {
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
    int oh = find_attr_index(rel_entity_decl, "OwnerHistory");
    ifcapi::detail::set_ref(
        new_rel, oh, ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
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
                                                IfcUtil::IfcBaseClass* user,
                                                IfcUtil::IfcBaseClass* application,
                                                const char* method_of_measurement = nullptr) {
    const auto* decl = file->schema()->declaration_by_name(ifc_class);
    auto* def = file->create(decl);
    auto* entity_decl = decl->as_entity();
    int gi = find_attr_index(entity_decl, "GlobalId");
    if (gi >= 0) def->set_attribute_value(static_cast<size_t>(gi), ifcapi::guid_new());
    int oh = find_attr_index(entity_decl, "OwnerHistory");
    ifcapi::detail::set_ref(def, oh, ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    int ni = find_attr_index(entity_decl, "Name");
    if (ni >= 0) def->set_attribute_value(static_cast<size_t>(ni), name);
    if (method_of_measurement) {
        int mi = find_attr_index(entity_decl, "MethodOfMeasurement");
        if (mi >= 0) def->set_attribute_value(static_cast<size_t>(mi), std::string(method_of_measurement));
    }
    return def;
}

}  // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* pset_add_pset(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    const std::string& name,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application,
    const char* ifc2x3_subclass)
{
    if (!file || !product) {
        set_error("pset_add_pset: missing required argument");
        return nullptr;
    }
    try {
        std::string nm(name);

        if (entity_is_a(product, "IfcObject") || entity_is_a(product, "IfcContext")) {
            if (auto* existing = find_existing_pset_on_object(product, nm)) {
                return existing;
            }
            auto* pset = create_named_definition(file, "IfcPropertySet", nm, owner_history, user, application);
            assign_pset_to_object(file, product, pset, owner_history, user, application);
            return pset;
        }

        if (entity_is_a(product, "IfcTypeObject")) {
            if (auto* existing = find_existing_pset_on_type(product, nm)) {
                return existing;
            }
            auto* pset = create_named_definition(file, "IfcPropertySet", nm, owner_history, user, application);
            append_to_type_pset_list(product, pset);
            return pset;
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
                    return d;
                }
            }

            const auto* decl = file->schema()->declaration_by_name(ifc_class);
            auto* def = file->create(decl);
            auto* entity_decl = decl->as_entity();
            int mi = find_attr_index(entity_decl, "Material");
            if (mi >= 0) def->set_attribute_value(static_cast<size_t>(mi), product);
            int ni = find_attr_index(entity_decl, "Name");
            if (ni >= 0) def->set_attribute_value(static_cast<size_t>(ni), nm);
            return def;
        }

        if (entity_is_a(product, "IfcProfileDef")) {
            if (!is_ifc2x3) {
                for (auto* d : get_inverse(product, "HasProperties")) {
                    if (read_string_attr(d, "Name") == nm) {
                        return d;
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
            return def;
        }

        set_error(
            ifcapi::detail::ERROR_TYPE,
            std::string("Class '") + product->declaration().name() + "' doesn't support adding a property set.");
        return nullptr;
    } catch (const std::exception& e) {
        set_error(std::string("pset_add_pset: ") + e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* pset_add_qto(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    const std::string& name,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || !product || name.empty()) {
        set_error("pset_add_qto: missing required argument");
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
                return existing;
            }
            auto* qto = create_named_definition(file, "IfcElementQuantity", nm, owner_history, user, application, mom);

            // Always create a fresh IfcRelDefinesByProperties for quantities.
            const auto* rel_decl = file->schema()->declaration_by_name("IfcRelDefinesByProperties");
            auto* rel = file->create(rel_decl);
            auto* rel_entity_decl = rel_decl->as_entity();
            int gi = find_attr_index(rel_entity_decl, "GlobalId");
            if (gi >= 0) rel->set_attribute_value(static_cast<size_t>(gi), ifcapi::guid_new());
            int oh = find_attr_index(rel_entity_decl, "OwnerHistory");
            ifcapi::detail::set_ref(
                rel, oh, ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
            int ro = find_attr_index(rel_entity_decl, "RelatedObjects");
            set_ref_aggregate(rel, ro, {product});
            int rp = find_attr_index(rel_entity_decl, "RelatingPropertyDefinition");
            if (rp >= 0) rel->set_attribute_value(static_cast<size_t>(rp), qto);
            return qto;
        }

        if (entity_is_a(product, "IfcTypeObject")) {
            if (auto* existing = find_existing_pset_on_type(product, nm)) {
                return existing;
            }
            auto* qto = create_named_definition(file, "IfcElementQuantity", nm, owner_history, user, application, mom);
            append_to_type_pset_list(product, qto);
            return qto;
        }

        return nullptr;
    } catch (const std::exception& e) {
        set_error(std::string("pset_add_qto: ") + e.what());
        return nullptr;
    }
}

} // namespace bindings
} // namespace ifcapi
