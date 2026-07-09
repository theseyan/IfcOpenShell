// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/system.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

std::string exact_class_name(express::Base entity) {
    return entity ? entity.declaration().name() : std::string();
}

express::Base create_entity(ifcopenshell::file* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<express::Base> inverse_entities(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() <= 0) return result;

    for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

bool contains(const std::vector<express::Base>& values, express::Base value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

std::vector<express::Base> append_unique(
    std::vector<express::Base> values,
    express::Base value)
{
    if (value && !contains(values, value)) {
        values.push_back(value);
    }
    return values;
}

express::Base create_owner_relation(
    ifcopenshell::file* file,
    const char* ifc_class,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto rel = create_entity(file, ifc_class);
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    return rel;
}

void update_port_placement(ifcopenshell::file* file, express::Base port) {
    auto placement = ifcapi::detail::read_ref_attr(port, "ObjectPlacement");
    if (is_a(placement, "IfcLocalPlacement")) {
        auto matrix = ifcapi::bindings::placement_get_local_placement(placement);
        ifcapi::bindings::geometry_edit_object_placement(
            file,
            ifcapi::bindings::GeometryEditObjectPlacementOptions{port, matrix, false, false});
    }
}

bool is_assignable(express::Base product, express::Base system) {
    const static std::unordered_map<std::string, std::vector<const char*>> group_types = {
        {"IfcZone", {"IfcZone", "IfcSpace", "IfcSpatialZone"}},
        {"IfcBuiltSystem", {"IfcBuiltElement", "IfcFurnishingElement", "IfcElementAssembly", "IfcTransportElement"}},
        {"IfcBuildingSystem", {"IfcBuildingElement", "IfcFurnishingElement", "IfcElementAssembly", "IfcTransportElement"}},
        {"IfcDistributionSystem", {"IfcDistributionElement"}},
        {"IfcDistributionCircuit", {"IfcDistributionElement"}},
        {"IfcStructuralAnalysisModel", {"IfcStructuralMember", "IfcStructuralConnection"}},
        {"IfcSystem", {"IfcProduct"}},
        {"IfcElectricalCircuit", {"IfcProduct"}},
        {"IfcGroup", {"IfcObjectDefinition"}},
    };
    auto it = group_types.find(exact_class_name(system));
    if (it == group_types.end()) return false;
    for (const char* assignable : it->second) {
        if (is_a(product, assignable)) return true;
    }
    return false;
}

void clear_flow_direction(express::Base port) {
    ifcapi::detail::write_blank_attr(port, "FlowDirection");
}

void write_flow_direction(express::Base port, const std::string& direction) {
    if (port && !ifcapi::detail::write_enum_attr(port, "FlowDirection", direction)) {
        throw std::runtime_error("Unable to find keyword in schema: " + direction);
    }
}

void remove_relation_with_history(ifcopenshell::file* file, express::Base rel) {
    ifcapi::detail::remove_with_history(file, rel);
}

void purge_existing_connections_to_other_ports(
    ifcopenshell::file* file,
    express::Base port1,
    express::Base port2)
{
    for (auto rel : inverse_entities(port1, "ConnectedTo")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatedPort") != port2) remove_relation_with_history(file, rel);
    }
    for (auto rel : inverse_entities(port1, "ConnectedFrom")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") != port2) remove_relation_with_history(file, rel);
    }
    for (auto rel : inverse_entities(port2, "ConnectedTo")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatedPort") != port1) remove_relation_with_history(file, rel);
    }
    for (auto rel : inverse_entities(port2, "ConnectedFrom")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") != port1) remove_relation_with_history(file, rel);
    }
}

void create_connects_ports(
    ifcopenshell::file* file,
    express::Base relating_port,
    express::Base related_port,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    auto rel = create_owner_relation(file, "IfcRelConnectsPorts", owner_history, user, application);
    ifcapi::detail::write_ref_attr(rel, "RelatingPort", relating_port);
    ifcapi::detail::write_ref_attr(rel, "RelatedPort", related_port);
}

void purge_connected_to(ifcopenshell::file* file, express::Base port) {
    for (auto rel : inverse_entities(port, "ConnectedTo")) {
        remove_relation_with_history(file, rel);
    }
}

void purge_connected_from(ifcopenshell::file* file, express::Base port) {
    for (auto rel : inverse_entities(port, "ConnectedFrom")) {
        remove_relation_with_history(file, rel);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base system_add_port(
    ifcopenshell::file* file,
    const SystemAddPortOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    auto port = create_entity(file, "IfcDistributionPort");
    ifcapi::detail::write_string_attr(port, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        port,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    if (options.element) {
        SystemAssignPortOptions assign_opts;
        assign_opts.element = *options.element;
        assign_opts.port = port;
        assign_opts.owner_history = options.owner_history;
        assign_opts.user = options.user;
        assign_opts.application = options.application;
        system_assign_port(file, assign_opts);
    }
    return port;
}

express::Base system_add_system(
    ifcopenshell::file* file,
    const SystemAddSystemOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());

    std::string resolved_class = options.ifc_class;
    if (is_ifc2x3(file) && resolved_class == "IfcDistributionSystem") {
        resolved_class = "IfcSystem";
    }
    RootCreateEntityOptions root_opts;
    root_opts.ifc_class = resolved_class;
    root_opts.name = "Unnamed";
    if (owner_history) {
        root_opts.owner_history = owner_history;
    }
    return root_create_entity(file, root_opts);
}

express::Base system_assign_flow_control(
    ifcopenshell::file* file,
    const SystemAssignFlowControlOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    auto assigned = inverse_entities(options.related_flow_control, "AssignedToFlowElement");
    if (!assigned.empty()) {
        auto assignment = assigned.front();
        if (ifcapi::detail::read_ref_attr(assignment, "RelatingFlowElement") == options.relating_flow_element) {
            return assignment;
        }
        return {};
    }

    auto flow_rels = inverse_entities(options.relating_flow_element, "HasControlElements");
    if (!flow_rels.empty()) {
        auto assignment = flow_rels.front();
        auto controls = ifcapi::detail::read_ref_aggregate(assignment, "RelatedControlElements");
        if (!contains(controls, options.related_flow_control)) {
            controls.push_back(options.related_flow_control);
            ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", controls);
            ifcapi::detail::update_owner_history(file, assignment, user, application);
        }
        return assignment;
    }

    auto assignment = create_owner_relation(file, "IfcRelFlowControlElements", owner_history, user, application);
    ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", {options.related_flow_control});
    ifcapi::detail::write_ref_attr(assignment, "RelatingFlowElement", options.relating_flow_element);
    return assignment;
}

express::Base system_assign_port(
    ifcopenshell::file* file,
    const SystemAssignPortOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    if (is_ifc2x3(file)) {
        for (auto rel : inverse_entities(options.element, "HasPorts")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") == options.port) return rel;
        }
        auto rel = create_owner_relation(file, "IfcRelConnectsPortToElement", owner_history, user, application);
        ifcapi::detail::write_ref_attr(rel, "RelatingPort", options.port);
        ifcapi::detail::write_ref_attr(rel, "RelatedElement", options.element);
        update_port_placement(file, options.port);
        return rel;
    }

    auto rels = inverse_entities(options.element, "IsNestedBy");
    for (auto rel : rels) {
        if (contains(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), options.port)) return rel;
    }

    express::Base rel = {};
    if (!rels.empty()) {
        rel = rels.front();
        auto related = append_unique(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), options.port);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    } else {
        rel = create_owner_relation(file, "IfcRelNests", owner_history, user, application);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {options.port});
        ifcapi::detail::write_ref_attr(rel, "RelatingObject", options.element);
    }
    update_port_placement(file, options.port);
    return rel;
}

express::Base system_assign_system(
    ifcopenshell::file* file,
    const SystemAssignSystemOptions& options)
{
    for (auto product : options.products) {
        if (!is_assignable(product, options.system)) {
            throw std::runtime_error("product is not assignable to system");
        }
    }
    GroupAssignGroupOptions opts;
    opts.products = options.products;
    opts.group = options.system;
    if (options.owner_history) opts.owner_history = *options.owner_history;
    if (options.user) opts.user = *options.user;
    if (options.application) opts.application = *options.application;
    return group_assign_group(file, opts);
}

void system_connect_port(
    ifcopenshell::file* file,
    const SystemConnectPortOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    auto element = options.element.value_or(express::Base());

    if (options.port1 == options.port2) return;

    purge_existing_connections_to_other_ports(file, options.port1, options.port2);

    if (options.direction == "SOURCE") {
        write_flow_direction(options.port1, "SOURCE");
        write_flow_direction(options.port2, "SINK");
    } else if (options.direction == "SINK") {
        write_flow_direction(options.port1, "SINK");
        write_flow_direction(options.port2, "SOURCE");
    } else {
        write_flow_direction(options.port1, options.direction);
        write_flow_direction(options.port2, options.direction);
    }

    if (options.direction == "SOURCE" || options.direction == "SOURCEANDSINK" || options.direction == "NOTDEFINED") {
        if (inverse_entities(options.port1, "ConnectedTo").empty()) {
            create_connects_ports(file, options.port1, options.port2, owner_history, user, application);
        }
    } else {
        purge_connected_to(file, options.port1);
    }

    if (options.direction == "SINK" || options.direction == "SOURCEANDSINK" || options.direction == "NOTDEFINED") {
        if (inverse_entities(options.port1, "ConnectedFrom").empty()) {
            create_connects_ports(file, options.port2, options.port1, owner_history, user, application);
        }
    } else {
        purge_connected_from(file, options.port1);
    }

    for (auto rel : inverse_entities(options.port1, "ConnectedTo")) {
        ifcapi::detail::write_ref_attr(rel, "RealizingElement", element);
    }
    for (auto rel : inverse_entities(options.port1, "ConnectedFrom")) {
        ifcapi::detail::write_ref_attr(rel, "RealizingElement", element);
    }
}

void system_disconnect_port(ifcopenshell::file* file, express::Base* port) {
    auto port_val = ifcapi::detail::deref_or_empty(port);
    auto rels = inverse_entities(port_val, "ConnectedTo");
    auto from = inverse_entities(port_val, "ConnectedFrom");
    rels.insert(rels.end(), from.begin(), from.end());
    for (auto rel : rels) {
        clear_flow_direction(ifcapi::detail::read_ref_attr(rel, "RelatingPort"));
        clear_flow_direction(ifcapi::detail::read_ref_attr(rel, "RelatedPort"));
        remove_relation_with_history(file, rel);
    }
}

void system_remove_system(ifcopenshell::file* file, express::Base* system) {
    auto system_val = ifcapi::detail::deref_or_empty(system);
    std::vector<int> inverse_ids;
    for (auto inverse : inverse_entities(file, system_val)) {
        inverse_ids.push_back(inverse.id());
    }
    for (int id : inverse_ids) {
        express::Base inverse = {};
        try {
            inverse = file->instance_by_id(id);
        } catch (...) {
            continue;
        }
        if (!inverse) continue;
        if (is_a(inverse, "IfcRelDefinesByProperties")) {
            auto pset = ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition");
            pset_remove_pset(file, &system_val, &pset);
        } else if (is_a(inverse, "IfcRelAssignsToGroup")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingGroup") == system_val
                || ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                remove_relation_with_history(file, inverse);
            }
        }
    }
    ifcapi::detail::remove_with_history(file, system_val);
}

void system_unassign_flow_control(
    ifcopenshell::file* file,
    const SystemUnassignFlowControlOptions& options)
{
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    auto assigned = inverse_entities(options.related_flow_control, "AssignedToFlowElement");
    if (assigned.empty()) return;
    auto assignment = assigned.front();
    if (ifcapi::detail::read_ref_attr(assignment, "RelatingFlowElement") != options.relating_flow_element) return;
    auto controls = ifcapi::detail::read_ref_aggregate(assignment, "RelatedControlElements");
    if (controls.size() == 1) {
        remove_relation_with_history(file, assignment);
        return;
    }
    controls.erase(std::remove(controls.begin(), controls.end(), options.related_flow_control), controls.end());
    ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", controls);
    ifcapi::detail::update_owner_history(file, assignment, user, application);
}

void system_unassign_port(
    ifcopenshell::file* file,
    const SystemUnassignPortOptions& options)
{
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());

    if (is_ifc2x3(file)) {
        for (auto rel : inverse_entities(options.element, "HasPorts")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") == options.port) {
                remove_relation_with_history(file, rel);
                return;
            }
        }
        return;
    }

    for (auto rel : inverse_entities(options.element, "IsNestedBy")) {
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        if (!contains(related, options.port)) continue;
        if (related.size() == 1) {
            remove_relation_with_history(file, rel);
            return;
        }
        related.erase(std::remove(related.begin(), related.end(), options.port), related.end());
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    }
}

void system_unassign_system(
    ifcopenshell::file* file,
    const SystemUnassignSystemOptions& options)
{
    GroupUnassignGroupOptions opts;
    opts.products = options.products;
    opts.group = options.system;
    if (options.user) opts.user = *options.user;
    if (options.application) opts.application = *options.application;
    group_unassign_group(file, opts);
}

} // namespace bindings
} // namespace ifcapi
