// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/system.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/material.h"

#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <exception>
#include <string>
#include <vector>

namespace {

using ifcapi::detail::entity_view;

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

express::Base current_entity(ifcopenshell::file* file, unsigned id) {
    try {
        return file->instance_by_id(static_cast<int>(id));
    } catch (...) {
        return {};
    }
}

std::vector<unsigned> inverse_ids(ifcopenshell::file* file, express::Base entity) {
    std::vector<unsigned> result;
    for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
        if (inverse && inverse.id()) result.push_back(inverse.id());
    }
    return result;
}

express::Base copy_class_impl(ifcopenshell::file* file, express::Base source);

void copy_direct_attributes(ifcopenshell::file* file, express::Base result) {
    if (is_a(result, "IfcProduct")) {
        ifcapi::detail::clear_attr(result, "Representation");
        auto placement = ifcapi::detail::read_ref_attr(result, "ObjectPlacement");
        if (placement) {
            auto copied_placement = ifcapi::detail::shallow_copy(file, placement);
            auto relative = ifcapi::detail::read_ref_attr(placement, "RelativePlacement");
            if (relative) {
                ifcapi::detail::write_ref_attr(copied_placement, "RelativePlacement", ifcapi::detail::deep_copy(file, relative));
            }
            ifcapi::detail::write_ref_attr(result, "ObjectPlacement", copied_placement);
        }
    } else if (is_a(result, "IfcTypeProduct")) {
        ifcapi::detail::clear_attr(result, "RepresentationMaps");
    }

    if (is_a(result, "IfcTypeObject")) {
        auto property_sets = ifcapi::detail::read_ref_aggregate(result, "HasPropertySets");
        std::vector<express::Base> copied_property_sets;
        copied_property_sets.reserve(property_sets.size());
        for (auto property_set : property_sets) {
            copied_property_sets.push_back(ifcapi::detail::deep_copy(file, property_set));
        }
        if (!copied_property_sets.empty()) {
            ifcapi::detail::write_ref_aggregate(result, "HasPropertySets", copied_property_sets);
        }
    }
}

void copy_default_inverse(express::Base inverse, express::Base source, express::Base result) {
    auto* declaration = inverse.declaration().as_entity();
    if (!declaration) return;
    const auto attributes = declaration->all_attributes();
    for (size_t index = 0; index < attributes.size(); ++index) {
        try {
            auto value = inverse.get_attribute_value(index);
            if (value.isNull()) continue;
            if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE
                && static_cast<express::Base>(value) == source) {
                auto copied_inverse = ifcapi::detail::shallow_copy(source.file(), inverse);
                copied_inverse.set_attribute_value(index, result);
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto aggregate = static_cast<std::vector<express::Base>>(value);
                if (std::find(aggregate.begin(), aggregate.end(), source) != aggregate.end()
                    && std::find(aggregate.begin(), aggregate.end(), result) == aggregate.end()) {
                    aggregate.push_back(result);
                    inverse.set_attribute_value(index, aggregate);
                }
            }
        } catch (...) {
        }
    }
}

void copy_nested_ports(
    ifcopenshell::file* file,
    express::Base inverse,
    express::Base source,
    express::Base result)
{
    std::vector<express::Base> ports;
    if (is_a(inverse, "IfcRelNests")) {
        for (auto nested : ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects")) {
            if (is_a(nested, "IfcDistributionPort")) ports.push_back(nested);
        }
    } else {
        auto port = ifcapi::detail::read_ref_attr(inverse, "RelatingPort");
        if (port) ports.push_back(port);
    }
    if (ports.empty()) return;

    std::vector<express::Base> copied_ports;
    copied_ports.reserve(ports.size());
    for (auto port : ports) copied_ports.push_back(copy_class_impl(file, port));

    auto copied_inverse = ifcapi::detail::shallow_copy(file, inverse);
    if (is_a(inverse, "IfcRelNests")) {
        ifcapi::detail::write_ref_attr(copied_inverse, "RelatingObject", result);
        ifcapi::detail::write_ref_aggregate(copied_inverse, "RelatedObjects", copied_ports);
    } else {
        ifcapi::detail::write_ref_attr(copied_inverse, "RelatedElement", result);
        ifcapi::detail::write_ref_attr(copied_inverse, "RelatingPort", copied_ports.front());
    }

    for (auto port : copied_ports) {
        ifcapi::bindings::system_unassign_port(file, {source, port, std::nullopt, std::nullopt});
        ifcapi::bindings::system_disconnect_port(file, &port);
        auto placement = ifcapi::detail::read_ref_attr(port, "ObjectPlacement");
        if (is_a(placement, "IfcLocalPlacement")) {
            auto matrix = ifcapi::bindings::placement_get_local_placement(placement);
            ifcapi::bindings::geometry_edit_object_placement(file, {port, matrix, false, false});
        }
    }
}

void copy_opening(
    ifcopenshell::file* file,
    express::Base inverse,
    express::Base result)
{
    auto opening = ifcapi::detail::read_ref_attr(inverse, "RelatedOpeningElement");
    if (!opening) return;
    if (is_a(opening, "IfcOpeningElement")
        && !ifcapi::detail::read_inverse_aggregate(opening, "HasFillings").empty()) {
        return;
    }
    auto copied_opening = copy_class_impl(file, opening);
    auto voids = ifcapi::detail::read_inverse_aggregate(copied_opening, "VoidsElements");
    if (!voids.empty()) ifcapi::detail::write_ref_attr(voids.front(), "RelatingBuildingElement", result);

    auto placement = ifcapi::detail::read_ref_attr(copied_opening, "ObjectPlacement");
    auto result_placement = ifcapi::detail::read_ref_attr(result, "ObjectPlacement");
    if (is_a(placement, "IfcLocalPlacement") && result_placement) {
        ifcapi::detail::write_ref_attr(placement, "PlacementRelTo", result_placement);
    }
    auto representation = ifcapi::detail::read_ref_attr(opening, "Representation");
    if (representation) {
        ifcapi::detail::write_ref_attr(
            copied_opening,
            "Representation",
            ifcapi::detail::deep_copy(file, representation, {"IfcGeometricRepresentationContext"}));
    }
}

void copy_indirect_attributes(ifcopenshell::file* file, express::Base source, express::Base result) {
    for (const auto id : inverse_ids(file, source)) {
        auto inverse = current_entity(file, id);
        if (!inverse) continue;

        if (is_a(inverse, "IfcRelDefinesByProperties")) {
            auto copied_inverse = ifcapi::detail::shallow_copy(file, inverse);
            ifcapi::detail::write_ref_aggregate(copied_inverse, "RelatedObjects", {result});
            auto definition = ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition");
            ifcapi::detail::write_ref_attr(copied_inverse, "RelatingPropertyDefinition", ifcapi::detail::deep_copy(file, definition));
        } else if ((is_a(inverse, "IfcRelNests")
                    && ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == source)
                   || (is_a(inverse, "IfcRelConnectsPortToElement")
                       && ifcapi::detail::read_ref_attr(inverse, "RelatedElement") == source)) {
            copy_nested_ports(file, inverse, source, result);
        } else if (is_a(inverse, "IfcRelAggregates")
                   && ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == source) {
            continue;
        } else if (is_a(inverse, "IfcRelContainedInSpatialStructure")
                   && ifcapi::detail::read_ref_attr(inverse, "RelatingStructure") == source) {
            continue;
        } else if (is_a(inverse, "IfcRelDefinesByType")
                   && ifcapi::detail::read_ref_attr(inverse, "RelatingType") == source) {
            continue;
        } else if (is_a(inverse, "IfcRelVoidsElement")
                   && ifcapi::detail::read_ref_attr(inverse, "RelatingBuildingElement") == source) {
            copy_opening(file, inverse, result);
        } else if (is_a(inverse, "IfcRelFillsElement") || is_a(inverse, "IfcRelConnectsPathElements")) {
            continue;
        } else if (is_a(inverse, "IfcRelAssociatesMaterial")) {
            auto material = ifcapi::detail::read_ref_attr(inverse, "RelatingMaterial");
            const auto class_name = material ? material.declaration().name() : std::string();
            if (class_name.find("Usage") != std::string::npos) {
                auto copied_inverse = ifcapi::detail::shallow_copy(file, inverse);
                ifcapi::detail::write_ref_attr(copied_inverse, "RelatingMaterial", ifcapi::detail::shallow_copy(file, material));
                ifcapi::detail::write_ref_aggregate(copied_inverse, "RelatedObjects", {result});
            } else if (class_name.find("Set") != std::string::npos) {
                auto copied_inverse = ifcapi::detail::shallow_copy(file, inverse);
                ifcapi::detail::write_ref_attr(
                    copied_inverse,
                    "RelatingMaterial",
                    ifcapi::detail::copy_material_definition(file, material));
                ifcapi::detail::write_ref_aggregate(copied_inverse, "RelatedObjects", {result});
            } else {
                copy_default_inverse(inverse, source, result);
            }
        } else {
            copy_default_inverse(inverse, source, result);
        }
    }
}

express::Base copy_class_impl(ifcopenshell::file* file, express::Base source) {
    auto result = ifcapi::detail::shallow_copy(file, source);
    if (!result) return {};
    copy_direct_attributes(file, result);
    copy_indirect_attributes(file, source, result);
    return result;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base root_copy_class(ifcopenshell::file* file, express::Base* product) {
    ifcopenshell_clear_error();
    auto source = ifcapi::detail::deref_or_empty(product);
    if (!file || !source) {
        set_error("root_copy_class: missing file or product");
        return {};
    }
    try {
        return copy_class_impl(file, source);
    } catch (const std::exception& exception) {
        set_error(std::string("root_copy_class: ") + exception.what());
        return {};
    } catch (...) {
        set_error("root_copy_class: unknown error");
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
