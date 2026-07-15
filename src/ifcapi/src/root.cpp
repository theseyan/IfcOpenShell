// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

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
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/exception.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

bool is_instance(const express::Base& entity, const char* ifc_class) {
    auto d = ifcopenshell::lock_data(entity.data_weak());
    return d && d->declaration()->is(ifc_class);
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, const express::Base& entity) {
    if (!file || !entity || entity.id() == 0) {
        return {};
    }
    return file->instances_by_reference(static_cast<int>(entity.id()));
}

express::Base entity_by_id(ifcopenshell::file* file, int id) {
    if (!file || id <= 0) {
        return {};
    }
    try {
        return file->instance_by_id(static_cast<int>(id));
    } catch (...) {
        return {};
    }
}

std::vector<express::Base> single_ref(const express::Base& entity) {
    return entity ? std::vector<express::Base>{entity} : std::vector<express::Base>{};
}

bool is_a(const ifcopenshell::schema_definition* schema, const ifcopenshell::entity* entity_decl, const char* type_name) {
    if (!schema || !entity_decl) {
        return false;
    }
    try {
        const auto* type = schema->declaration_by_name(type_name);
        return entity_decl->is(*type);
    } catch (...) {
        return false;
    }
}

void root_remove_product_impl(
    ifcopenshell::file* file,
    express::Base product,
    express::Base user,
    express::Base application);

void root_remove_product_impl(
    ifcopenshell::file* file,
    express::Base product,
    express::Base user,
    express::Base application)
{
    if (!file || !product) {
        return;
    }
    const int product_id = static_cast<int>(product.id());
    std::vector<express::Base> representations;

    if (is_instance(product, "IfcProduct")) {
        auto product_representation = ifcapi::detail::read_ref_attr(product, "Representation");
        if (product_representation) {
            representations = ifcapi::detail::read_ref_aggregate(product_representation, "Representations");
        }
        auto object_placement = ifcapi::detail::read_ref_attr(product, "ObjectPlacement");
        if (object_placement && ifcapi::detail::total_inverses(file, object_placement) == 1) {
            ifcapi::detail::write_ref_attr(product, "ObjectPlacement", {});
            ifcapi::bindings::entity_remove_deep(&object_placement);
        }
    } else if (is_instance(product, "IfcTypeProduct")) {
        for (auto representation_map : ifcapi::detail::read_ref_aggregate(product, "RepresentationMaps")) {
            if (auto mapped = ifcapi::detail::read_ref_attr(representation_map, "MappedRepresentation")) {
                representations.push_back(mapped);
            }
        }
        auto psets = ifcapi::detail::read_ref_aggregate(product, "HasPropertySets");
        for (auto pset : psets) {
            if (pset && ifcapi::detail::total_inverses(file, pset) == 1) {
                auto product_ref = product;
                ifcapi::bindings::pset_remove_pset(file, &product_ref, &pset);
            }
        }
    }

    for (auto representation : representations) {
        auto product_ref = product;
        ifcapi::bindings::geometry_unassign_representation(file, &product_ref, &representation);
        ifcapi::bindings::geometry_remove_representation(
            file,
            &representation,
            ifcapi::bindings::GeometryRemoveRepresentationOptions{true});
    }

    auto openings = ifcapi::detail::read_inverse_aggregate(product, "HasOpenings");
    for (auto rel : openings) {
        if (auto opening = ifcapi::detail::read_ref_attr(rel, "RelatedOpeningElement")) {
            ifcapi::bindings::feature_remove_feature(
                file,
                ifcapi::bindings::FeatureRemoveFeatureOptions{opening, user, application});
        }
    }

    if (is_instance(product, "IfcGrid")) {
        std::vector<express::Base> axes = ifcapi::detail::read_ref_aggregate(product, "UAxes");
        ifcapi::detail::append_unique(axes, ifcapi::detail::read_ref_aggregate(product, "VAxes"));
        ifcapi::detail::append_unique(axes, ifcapi::detail::read_ref_aggregate(product, "WAxes"));
        for (auto axis : axes) {
            ifcapi::bindings::grid_remove_grid_axis(file, &axis);
        }
    }

    std::set<int> inverse_ids;
    for (const auto& inverse : inverse_entities(file, product)) {
        inverse_ids.insert(static_cast<int>(inverse.id()));
    }

    for (int inverse_id : inverse_ids) {
        auto inverse = entity_by_id(file, inverse_id);
        if (!inverse) {
            continue;
        }

        if (is_instance(inverse, "IfcRelDefinesByProperties")) {
            auto pset = ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition");
            auto product_ref = product;
            ifcapi::bindings::pset_remove_pset(file, &product_ref, &pset);
        } else if (is_instance(inverse, "IfcRelAssociatesMaterial")) {
            ifcapi::bindings::material_unassign_material(
                file,
                single_ref(product),
                {user, application});
        } else if (is_instance(inverse, "IfcRelDefinesByType")) {
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingType"), product)) {
                auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                ifcapi::bindings::TypeUnassignTypeOptions type_opts;
                type_opts.objects = related;
                if (user) type_opts.user = user;
                if (application) type_opts.application = application;
                ifcapi::bindings::type_unassign_type(file, type_opts);
            } else {
                ifcapi::bindings::TypeUnassignTypeOptions type_opts;
                type_opts.objects = single_ref(product);
                if (user) type_opts.user = user;
                if (application) type_opts.application = application;
                ifcapi::bindings::type_unassign_type(file, type_opts);
            }
        } else if (is_instance(inverse, "IfcRelSpaceBoundary")) {
            ifcapi::bindings::boundary_remove_boundary(file, &inverse);
        } else if (
            is_instance(inverse, "IfcRelFillsElement") ||
            is_instance(inverse, "IfcRelVoidsElement") ||
            is_instance(inverse, "IfcRelServicesBuildings")) {
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelNests")) {
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingObject"), product)) {
                auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                for (auto subelement : related) {
                    if (is_instance(subelement, "IfcDistributionPort")) {
                        root_remove_product_impl(file, subelement, user, application);
                    }
                }
                inverse = entity_by_id(file, inverse_id);
                if (inverse) {
                    ifcapi::detail::remove_with_history(file, inverse);
                }
            } else {
                auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                if (related.size() == 1 && ifcapi::detail::same_instance(related.front(), product)) {
                    ifcapi::detail::remove_with_history(file, inverse);
                }
            }
        } else if (is_instance(inverse, "IfcRelAggregates")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingObject"), product) || related.size() == 1) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelContainedInSpatialStructure")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedElements");
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingStructure"), product) || related.size() == 1) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelConnectsElements")) {
            if (is_instance(inverse, "IfcRelConnectsWithRealizingElements")) {
                auto relating = ifcapi::detail::read_ref_attr(inverse, "RelatingElement");
                auto related = ifcapi::detail::read_ref_attr(inverse, "RelatedElement");
                auto realizing = ifcapi::detail::read_ref_aggregate(inverse, "RealizingElements");
                bool has_other_realizing = false;
                for (auto element : realizing) {
                    if (!ifcapi::detail::same_instance(element, product)) {
                        has_other_realizing = true;
                        break;
                    }
                }
                if (!ifcapi::detail::same_instance(product, relating) && !ifcapi::detail::same_instance(product, related) && has_other_realizing) {
                    continue;
                }
            }
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelConnectsPortToElement")) {
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatedElement"), product)) {
                if (auto port = ifcapi::detail::read_ref_attr(inverse, "RelatingPort")) {
                    root_remove_product_impl(file, port, user, application);
                }
                inverse = entity_by_id(file, inverse_id);
                if (inverse) {
                    ifcapi::detail::remove_with_history(file, inverse);
                }
            } else if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingPort"), product)) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelConnectsPorts")) {
            if (!ifcapi::detail::same_instance(product, ifcapi::detail::read_ref_attr(inverse, "RelatingPort")) &&
                !ifcapi::detail::same_instance(product, ifcapi::detail::read_ref_attr(inverse, "RelatedPort"))) {
                continue;
            }
            ifcapi::detail::remove_with_history(file, inverse);
        } else if (is_instance(inverse, "IfcRelAssignsToGroup")) {
            if (ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelAssignsToProduct")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingProduct"), product) ||
                (related.size() == 1 && ifcapi::detail::same_instance(related.front(), product))) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        } else if (is_instance(inverse, "IfcRelFlowControlElements")) {
            auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedControlElements");
            if (ifcapi::detail::same_instance(ifcapi::detail::read_ref_attr(inverse, "RelatingFlowElement"), product) ||
                (related.size() == 1 && ifcapi::detail::same_instance(related.front(), product))) {
                ifcapi::detail::remove_with_history(file, inverse);
            }
        }
    }

    auto live_product = entity_by_id(file, product_id);
    if (!live_product) {
        return;
    }
    ifcapi::detail::remove_with_history(file, live_product);
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base root_create_entity(
    ifcopenshell::file* file,
    const RootCreateEntityOptions& options)
{
    ifcopenshell_clear_error();

    if (!file) {
        set_error("file is NULL");
        return {};
    }
    const std::string& ifc_class = options.ifc_class;
    if (ifc_class.empty()) {
        set_error("ifc_class is empty");
        return {};
    }

    try {
        const auto* schema = file->schema();
        const std::string schema_name = schema->name();

        const ifcopenshell::declaration* decl = nullptr;
        try {
            decl = schema->declaration_by_name(ifc_class);
        } catch (...) {
            set_error(std::string("Unknown IFC class: ") + ifc_class);
            return {};
        }

        const ifcopenshell::entity* entity_decl = decl->as_entity();
        if (!entity_decl) {
            set_error(std::string("Not an entity type: ") + ifc_class);
            return {};
        }

        auto entity = file->create(decl);
        if (!entity) {
            set_error("Failed to create entity");
            return {};
        }
        ifcapi::detail::entity_view view(entity);

        if (view.has_attr("GlobalId")) {
            ifcapi::detail::write_string_attr(view, "GlobalId", ifcapi::guid_new());
        }

        if (options.owner_history && *options.owner_history && view.has_attr("OwnerHistory")) {
            ifcapi::detail::write_ref_attr(view, "OwnerHistory", *options.owner_history);
        }

        if (options.name) {
            ifcapi::detail::write_string_attr(view, "Name", *options.name);
        }

        auto set_enum = [&](const char* attr_name, const std::string& value) -> bool {
            return ifcapi::detail::write_enum_attr(view, attr_name, value);
        };
        auto set_string = [&](const char* attr_name, const std::string& value) {
            ifcapi::detail::write_string_attr(view, attr_name, value);
        };
        auto set_bool = [&](const char* attr_name, bool value) {
            view.set(attr_name, value);
        };

        if (options.predefined_type && !options.predefined_type->empty()) {
            const std::string& predefined_type = *options.predefined_type;
            if (view.has_attr("PredefinedType")) {
                if (!set_enum("PredefinedType", predefined_type)) {
                    set_enum("PredefinedType", "USERDEFINED");
                    if (view.has_attr("ObjectType")) {
                        set_string("ObjectType", predefined_type);
                    } else if (view.has_attr("ElementType")) {
                        set_string("ElementType", predefined_type);
                    } else if (view.has_attr("ProcessType")) {
                        set_string("ProcessType", predefined_type);
                    }
                }
            } else if (view.has_attr("ObjectType")) {
                set_string("ObjectType", predefined_type);
            }
        }

        const bool is_ifc2x3 = schema_name.find("2x3") != std::string::npos ||
            schema_name.find("2X3") != std::string::npos;

        if (is_ifc2x3) {
            if (is_a(schema, entity_decl, "IfcElementType") && view.has_attr("PredefinedType") &&
                view.is_null("PredefinedType")) {
                set_enum("PredefinedType", "NOTDEFINED");
            }

            if (is_a(schema, entity_decl, "IfcSpatialStructureElement")) {
                set_enum("CompositionType", "ELEMENT");
            } else if (is_a(schema, entity_decl, "IfcRoof")) {
                set_enum("ShapeType", "NOTDEFINED");
            } else if (is_a(schema, entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            } else if (is_a(schema, entity_decl, "IfcDoorStyle") || is_a(schema, entity_decl, "IfcWindowStyle")) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            }
        } else {
            if (is_a(schema, entity_decl, "IfcElementType") && view.has_attr("PredefinedType") &&
                view.is_null("PredefinedType")) {
                set_enum("PredefinedType", "NOTDEFINED");
            }

            bool is_ifc4_only = schema_name == "IFC4";
            if (is_ifc4_only && (is_a(schema, entity_decl, "IfcDoorStyle") || is_a(schema, entity_decl, "IfcWindowStyle"))) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            } else if (is_a(schema, entity_decl, "IfcDoorType")) {
                set_enum("OperationType", "NOTDEFINED");
            } else if (is_a(schema, entity_decl, "IfcWindowType")) {
                set_enum("PartitioningType", "NOTDEFINED");
            } else if (is_a(schema, entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            }
        }

        return entity;

    } catch (const ifcopenshell::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
        return {};
    } catch (...) {
        set_error("Unknown C++ exception in ifcopenshell_root_create_entity");
        return {};
    }
}

void root_remove_product(
    ifcopenshell::file* file,
    express::Base* product,
    const RootRemoveProductOptions& options)
{
    ifcopenshell_clear_error();
    try {
        root_remove_product_impl(
            file,
            product ? *product : express::Base(),
            options.user.value_or(express::Base{}),
            options.application.value_or(express::Base{}));
    } catch (const std::exception& e) {
        ifcapi::detail::set_error(e);
    } catch (...) {
        set_error("Unknown C++ exception in ifcopenshell_root_remove_product");
    }
}

} // namespace bindings
} // namespace ifcapi
