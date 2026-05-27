// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/boundary.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/feature.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/grid.h"
#include "ifcapi/bindings/material.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/detail/attribute.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"
#include "ifcparse/instance_data.h"
#include "ifcparse/exception.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// Error-reporting helpers route through the autogen layer's named namespace
// (declared in ifcopenshell_api_internal.hpp) so the high-level and low-level
// APIs share a single thread-local error string and the same public
// ifcopenshell_last_error_message() / ifcopenshell_clear_error() entry points.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

inline bool is_instance(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || !entity->id()) return result;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* inverse : *inverses) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

bool exists_in_file(IfcParse::IfcFile* file, int id) {
    if (!file || id <= 0) return false;
    try {
        return file->instance_by_id(static_cast<unsigned>(id)) != nullptr;
    } catch (...) {
        return false;
    }
}

IfcUtil::IfcBaseClass* entity_by_id(IfcParse::IfcFile* file, int id) {
    return exists_in_file(file, id) ? file->instance_by_id(static_cast<unsigned>(id)) : nullptr;
}

void remove_with_history(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* relationship) {
    if (!file || !relationship) return;
    auto* history = ifcapi::detail::read_ref_attr(relationship, "OwnerHistory");
    file->removeEntity(relationship);
    if (history) ifcapi::bindings::entity_remove_deep2(history);
}

std::vector<const IfcUtil::IfcBaseClass*> single_const(IfcUtil::IfcBaseClass* entity) {
    return entity ? std::vector<const IfcUtil::IfcBaseClass*>{entity} : std::vector<const IfcUtil::IfcBaseClass*>{};
}

void root_remove_product_impl(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application);

void root_remove_product_impl(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (!file || !product) return;
    const int product_id = static_cast<int>(product->id());
    std::vector<IfcUtil::IfcBaseClass*> representations;

    if (is_instance(product, "IfcProduct")) {
        auto* product_representation = ifcapi::detail::read_ref_attr(product, "Representation");
        if (product_representation) {
            representations = ifcapi::detail::read_ref_aggregate(product_representation, "Representations");
        }
        auto* object_placement = ifcapi::detail::read_ref_attr(product, "ObjectPlacement");
        if (object_placement && file->getTotalInverses(object_placement->id()) == 1) {
            ifcapi::detail::write_ref_attr(product, "ObjectPlacement", nullptr);
            ifcapi::bindings::entity_remove_deep2(object_placement);
        }
    } else if (is_instance(product, "IfcTypeProduct")) {
        for (auto* representation_map : ifcapi::detail::read_ref_aggregate(product, "RepresentationMaps")) {
            if (auto* mapped = ifcapi::detail::read_ref_attr(representation_map, "MappedRepresentation")) {
                representations.push_back(mapped);
            }
        }
        auto psets = ifcapi::detail::read_ref_aggregate(product, "HasPropertySets");
        for (auto* pset : psets) {
            if (pset && file->getTotalInverses(pset->id()) == 1) {
                ifcapi::bindings::pset_remove_pset(file, product, pset);
            }
        }
    }

    for (auto* representation : representations) {
        ifcapi::bindings::geometry_unassign_representation(file, product, representation);
        ifcapi::bindings::geometry_remove_representation(file, representation, true);
    }

    auto openings = ifcapi::detail::read_inverse_aggregate(product, "HasOpenings");
    for (auto* rel : openings) {
        if (auto* opening = ifcapi::detail::read_ref_attr(rel, "RelatedOpeningElement")) {
            ifcapi::bindings::feature_remove_feature(file, opening, user, application);
        }
    }

    if (is_instance(product, "IfcGrid")) {
        std::vector<IfcUtil::IfcBaseClass*> axes = ifcapi::detail::read_ref_aggregate(product, "UAxes");
        ifcapi::detail::append_unique(axes, ifcapi::detail::read_ref_aggregate(product, "VAxes"));
        ifcapi::detail::append_unique(axes, ifcapi::detail::read_ref_aggregate(product, "WAxes"));
        for (auto* axis : axes) {
            ifcapi::bindings::grid_remove_grid_axis(file, axis);
        }
    }

    std::vector<int> inverse_ids;
    for (auto* inverse : inverse_entities(file, product)) {
        inverse_ids.push_back(static_cast<int>(inverse->id()));
    }

    for (int inverse_id : inverse_ids) {
        auto* inverse = entity_by_id(file, inverse_id);
        if (!inverse) continue;

        if (is_instance(inverse, "IfcRelDefinesByProperties")) {
            ifcapi::bindings::pset_remove_pset(
                file,
                product,
                ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition"));
        } else if (is_instance(inverse, "IfcRelAssociatesMaterial")) {
            ifcapi::bindings::material_unassign_material(file, single_const(product), user, application);
        } else if (is_instance(inverse, "IfcRelDefinesByType")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingType") == product) {
                ifcapi::bindings::type_unassign_type(
                    file,
                    ifcapi::detail::to_const_refs(ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects")),
                    user,
                    application);
            } else {
                ifcapi::bindings::type_unassign_type(file, single_const(product), user, application);
            }
        } else if (is_instance(inverse, "IfcRelSpaceBoundary")) {
            ifcapi::bindings::boundary_remove_boundary(file, inverse);
        } else if (
            is_instance(inverse, "IfcRelFillsElement") ||
            is_instance(inverse, "IfcRelVoidsElement") ||
            is_instance(inverse, "IfcRelServicesBuildings")) {
            remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelNests")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == product) {
                auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                for (auto* subelement : related) {
                    if (is_instance(subelement, "IfcDistributionPort")) {
                        root_remove_product_impl(file, subelement, user, application);
                    }
                }
                inverse = entity_by_id(file, inverse_id);
                if (inverse) remove_with_history(file, inverse);
            } else {
                auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                if (related.size() == 1 && related.front() == product) {
                    remove_with_history(file, inverse);
                }
            }
        } else if (is_instance(inverse, "IfcRelAggregates")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == product || related.size() == 1) {
                remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelContainedInSpatialStructure")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedElements");
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingStructure") == product || related.size() == 1) {
                remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelConnectsElements")) {
            if (is_instance(inverse, "IfcRelConnectsWithRealizingElements")) {
                auto* relating = ifcapi::detail::read_ref_attr(inverse, "RelatingElement");
                auto* related = ifcapi::detail::read_ref_attr(inverse, "RelatedElement");
                auto realizing = ifcapi::detail::read_ref_aggregate(inverse, "RealizingElements");
                bool has_other_realizing = false;
                for (auto* element : realizing) {
                    if (element != product) {
                        has_other_realizing = true;
                        break;
                    }
                }
                if (product != relating && product != related && has_other_realizing) {
                    continue;
                }
            }
            remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelConnectsPortToElement")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatedElement") == product) {
                if (auto* port = ifcapi::detail::read_ref_attr(inverse, "RelatingPort")) {
                    root_remove_product_impl(file, port, user, application);
                }
                inverse = entity_by_id(file, inverse_id);
                if (inverse) remove_with_history(file, inverse);
            } else if (ifcapi::detail::read_ref_attr(inverse, "RelatingPort") == product) {
                remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelConnectsPorts")) {
            if (product != ifcapi::detail::read_ref_attr(inverse, "RelatingPort") &&
                product != ifcapi::detail::read_ref_attr(inverse, "RelatedPort")) {
                continue;
            }
            remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelAssignsToGroup")) {
            if (ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelAssignsToProduct")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingProduct") == product ||
                (related.size() == 1 && related.front() == product)) {
                remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelFlowControlElements")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedControlElements");
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingFlowElement") == product ||
                (related.size() == 1 && related.front() == product)) {
                remove_with_history(file, inverse);
            }
        }
    }

    auto* live_product = entity_by_id(file, product_id);
    if (!live_product) return;
    auto* history = ifcapi::detail::read_ref_attr(live_product, "OwnerHistory");
    file->removeEntity(live_product);
    if (history) ifcapi::bindings::entity_remove_deep2(history);
}
}

// Resolve the enumeration_type for an entity attribute at the given index.
// Walks through named_type → type_declaration chains until an enumeration_type is found.
static const IfcParse::enumeration_type* resolve_enum_type(
    const IfcParse::entity* entity_decl, size_t attr_idx)
{
    auto all_attrs = entity_decl->all_attributes();
    if (attr_idx >= all_attrs.size()) return nullptr;

    const IfcParse::parameter_type* pt = all_attrs[attr_idx]->type_of_attribute();
    while (pt) {
        auto* named = pt->as_named_type();
        if (!named) break;
        auto* decl = named->declared_type();
        if (auto* et = decl->as_enumeration_type()) return et;
        if (auto* td = decl->as_type_declaration()) {
            pt = td->declared_type();
        } else {
            break;
        }
    }
    return nullptr;
}

// Try to set an enumeration attribute by string value.
// Returns true if the value was found and set, false otherwise.
static bool try_set_enum(
    IfcUtil::IfcBaseClass* entity,
    const IfcParse::entity* entity_decl,
    size_t attr_idx,
    const std::string& value)
{
    auto* enum_type = resolve_enum_type(entity_decl, attr_idx);
    if (!enum_type) return false;

    const auto& items = enum_type->enumeration_items();
    auto it = std::find(items.begin(), items.end(), value);
    if (it == items.end()) return false;

    size_t enum_idx = static_cast<size_t>(std::distance(items.begin(), it));
    entity->set_attribute_value(attr_idx, EnumerationReference(enum_type, enum_idx));
    return true;
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* root_create_entity(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    const char* predefined_type,
    const char* name,
    IfcUtil::IfcBaseClass* owner_history)
{
    ifcopenshell_clear_error();

    if (!file) { set_error("file is NULL"); return 0; }
    if (ifc_class.empty()) { set_error("ifc_class is empty"); return 0; }

    try {
        const auto* schema = file->schema();
        const std::string schema_name = schema->name();

        const IfcParse::declaration* decl = nullptr;
        try {
            decl = schema->declaration_by_name(ifc_class);
        } catch (...) {
            set_error(std::string("Unknown IFC class: ") + ifc_class);
            return 0;
        }

        const IfcParse::entity* entity_decl = decl->as_entity();
        if (!entity_decl) {
            set_error(std::string("Not an entity type: ") + ifc_class);
            return 0;
        }

        auto* entity = file->create(decl);
        if (!entity) {
            set_error("Failed to create entity");
            return 0;
        }

        // Helper: find attribute index by name, returns -1 if not found
        auto attr_index = [&](const char* attr_name) -> int {
            try {
                return static_cast<int>(entity_decl->attribute_index(attr_name));
            } catch (...) {
                return -1;
            }
        };

        // Helper: check if entity is_a given type name
        auto is_a = [&](const IfcParse::entity* e, const char* type_name) -> bool {
            try {
                const auto* t = schema->declaration_by_name(type_name);
                return e->is(*t);
            } catch (...) {
                return false;
            }
        };

        auto has_attr = [&](const char* n) -> bool { return attr_index(n) >= 0; };

        auto set_string = [&](const char* attr_name, const std::string& value) {
            int idx = attr_index(attr_name);
            if (idx >= 0) entity->set_attribute_value(static_cast<size_t>(idx), value);
        };

        auto set_enum = [&](const char* attr_name, const std::string& value) -> bool {
            int idx = attr_index(attr_name);
            if (idx < 0) return false;
            return try_set_enum(entity, entity_decl, static_cast<size_t>(idx), value);
        };

        auto set_bool = [&](const char* attr_name, bool value) {
            int idx = attr_index(attr_name);
            if (idx >= 0) entity->set_attribute_value(static_cast<size_t>(idx), value);
        };

        // 1. Set GlobalId
        {
            int gi = attr_index("GlobalId");
            if (gi >= 0) {
                std::string guid = ifcapi::guid_new();
                entity->set_attribute_value(static_cast<size_t>(gi), guid);
            }
        }

        // 2. Set OwnerHistory
        if (owner_history) {
            int oh = attr_index("OwnerHistory");
            if (oh >= 0) {
                entity->set_attribute_value(static_cast<size_t>(oh), owner_history);
            }
        }

        // 3. Set Name
        if (name) {
            set_string("Name", name);
        }

        // 4. Handle PredefinedType
        if (predefined_type && predefined_type[0] != '\0') {
            if (has_attr("PredefinedType")) {
                if (!set_enum("PredefinedType", predefined_type)) {
                    set_enum("PredefinedType", "USERDEFINED");
                    if (has_attr("ObjectType")) {
                        set_string("ObjectType", predefined_type);
                    } else if (has_attr("ElementType")) {
                        set_string("ElementType", predefined_type);
                    } else if (has_attr("ProcessType")) {
                        set_string("ProcessType", predefined_type);
                    }
                }
            } else if (has_attr("ObjectType")) {
                set_string("ObjectType", predefined_type);
            }
        }

        // 5. Schema-specific defaults
        bool is_ifc2x3 = (schema_name.find("2x3") != std::string::npos ||
                          schema_name.find("2X3") != std::string::npos);

        if (is_ifc2x3) {
            if (is_a(entity_decl, "IfcElementType") && has_attr("PredefinedType")) {
                int pt = attr_index("PredefinedType");
                if (pt >= 0 && entity->get_attribute_value(static_cast<size_t>(pt)).isNull()) {
                    set_enum("PredefinedType", "NOTDEFINED");
                }
            }

            if (is_a(entity_decl, "IfcSpatialStructureElement")) {
                set_enum("CompositionType", "ELEMENT");
            } else if (is_a(entity_decl, "IfcRoof")) {
                set_enum("ShapeType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcDoorStyle") || is_a(entity_decl, "IfcWindowStyle")) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            }
        } else {
            if (is_a(entity_decl, "IfcElementType") && has_attr("PredefinedType")) {
                int pt = attr_index("PredefinedType");
                if (pt >= 0 && entity->get_attribute_value(static_cast<size_t>(pt)).isNull()) {
                    set_enum("PredefinedType", "NOTDEFINED");
                }
            }

            bool is_ifc4_only = (schema_name == "IFC4");
            if (is_ifc4_only && (is_a(entity_decl, "IfcDoorStyle") || is_a(entity_decl, "IfcWindowStyle"))) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            } else if (is_a(entity_decl, "IfcDoorType")) {
                set_enum("OperationType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcWindowType")) {
                set_enum("PartitioningType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            }
        }

        return entity;

    } catch (const IfcParse::IfcException& e) {
        set_error(e.what());
        return 0;
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    } catch (...) {
        set_error("Unknown C++ exception in ifcopenshell_root_create_entity");
        return 0;
    }
}

void root_remove_product(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* product,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        root_remove_product_impl(file, product, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    } catch (...) {
        set_error("Unknown C++ exception in ifcopenshell_root_remove_product");
    }
}

} // namespace bindings
} // namespace ifcapi
