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

bool is_ifc2x3(IfcParse::IfcFile* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

bool is_a(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

std::string exact_class_name(IfcUtil::IfcBaseClass* entity) {
    return entity ? entity->declaration().name() : std::string();
}

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const char* ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!file || !entity || entity->id() <= 0) return result;
    auto inverses = file->getInverse(entity->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto* inverse : *inverses) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> inverse_entities(IfcUtil::IfcBaseClass* entity, const char* attribute) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto* base = dynamic_cast<IfcUtil::IfcBaseEntity*>(entity);
    if (!base) return result;
    try {
        auto inverses = base->get_inverse(attribute);
        if (!inverses) return result;
        for (size_t i = 0; i < inverses->size(); ++i) {
            if ((*inverses)[i]) result.push_back((*inverses)[i]);
        }
    } catch (...) {
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> mutable_entities(
    const std::vector<const IfcUtil::IfcBaseClass*>& entities)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(entities.size());
    for (auto* entity : entities) {
        if (entity) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(entity));
    }
    return result;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

std::vector<IfcUtil::IfcBaseClass*> append_unique(
    std::vector<IfcUtil::IfcBaseClass*> values,
    IfcUtil::IfcBaseClass* value)
{
    if (value && !contains(values, value)) {
        values.push_back(value);
    }
    return values;
}

IfcUtil::IfcBaseClass* create_owner_relation(
    IfcParse::IfcFile* file,
    const char* ifc_class,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = create_entity(file, ifc_class);
    ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        rel,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    return rel;
}

void update_port_placement(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* port) {
    auto* placement = ifcapi::detail::read_ref_attr(port, "ObjectPlacement");
    if (is_a(placement, "IfcLocalPlacement")) {
        auto matrix = ifcapi::bindings::placement_get_local_placement(placement);
        ifcapi::bindings::geometry_edit_object_placement(file, port, matrix, false, false);
    }
}

bool is_assignable(IfcUtil::IfcBaseClass* product, IfcUtil::IfcBaseClass* system) {
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

void clear_flow_direction(IfcUtil::IfcBaseClass* port) {
    ifcapi::detail::write_blank_attr(port, "FlowDirection");
}

void write_flow_direction(IfcUtil::IfcBaseClass* port, const std::string& direction) {
    if (port && !ifcapi::detail::write_enum_attr(port, "FlowDirection", direction)) {
        throw std::runtime_error("Unable to find keyword in schema: " + direction);
    }
}

void remove_relation_with_history(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* rel) {
    ifcapi::detail::remove_with_history(file, rel);
}

void purge_existing_connections_to_other_ports(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* port1,
    IfcUtil::IfcBaseClass* port2)
{
    for (auto* rel : inverse_entities(port1, "ConnectedTo")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatedPort") != port2) remove_relation_with_history(file, rel);
    }
    for (auto* rel : inverse_entities(port1, "ConnectedFrom")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") != port2) remove_relation_with_history(file, rel);
    }
    for (auto* rel : inverse_entities(port2, "ConnectedTo")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatedPort") != port1) remove_relation_with_history(file, rel);
    }
    for (auto* rel : inverse_entities(port2, "ConnectedFrom")) {
        if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") != port1) remove_relation_with_history(file, rel);
    }
}

void create_connects_ports(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_port,
    IfcUtil::IfcBaseClass* related_port,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* rel = create_owner_relation(file, "IfcRelConnectsPorts", owner_history, user, application);
    ifcapi::detail::write_ref_attr(rel, "RelatingPort", relating_port);
    ifcapi::detail::write_ref_attr(rel, "RelatedPort", related_port);
}

void purge_connected_to(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* port) {
    for (auto* rel : inverse_entities(port, "ConnectedTo")) {
        remove_relation_with_history(file, rel);
    }
}

void purge_connected_from(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* port) {
    for (auto* rel : inverse_entities(port, "ConnectedFrom")) {
        remove_relation_with_history(file, rel);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* system_add_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto* port = create_entity(file, "IfcDistributionPort");
    ifcapi::detail::write_string_attr(port, "GlobalId", ifcapi::guid_new());
    ifcapi::detail::write_ref_attr(
        port,
        "OwnerHistory",
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    if (element) {
        system_assign_port(file, element, port, owner_history, user, application);
    }
    return port;
}

IfcUtil::IfcBaseClass* system_add_system(
    IfcParse::IfcFile* file,
    const std::string& ifc_class,
    IfcUtil::IfcBaseClass* owner_history)
{
    std::string resolved_class = ifc_class;
    if (is_ifc2x3(file) && resolved_class == "IfcDistributionSystem") {
        resolved_class = "IfcSystem";
    }
    return root_create_entity(file, resolved_class, nullptr, "Unnamed", owner_history);
}

IfcUtil::IfcBaseClass* system_assign_flow_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_flow_element,
    IfcUtil::IfcBaseClass* related_flow_control,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto assigned = inverse_entities(related_flow_control, "AssignedToFlowElement");
    if (!assigned.empty()) {
        auto* assignment = assigned.front();
        if (ifcapi::detail::read_ref_attr(assignment, "RelatingFlowElement") == relating_flow_element) {
            return assignment;
        }
        return nullptr;
    }

    auto flow_rels = inverse_entities(relating_flow_element, "HasControlElements");
    if (!flow_rels.empty()) {
        auto* assignment = flow_rels.front();
        auto controls = ifcapi::detail::read_ref_aggregate(assignment, "RelatedControlElements");
        if (!contains(controls, related_flow_control)) {
            controls.push_back(related_flow_control);
            ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", controls);
            ifcapi::detail::update_owner_history(file, assignment, user, application);
        }
        return assignment;
    }

    auto* assignment = create_owner_relation(file, "IfcRelFlowControlElements", owner_history, user, application);
    ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", {related_flow_control});
    ifcapi::detail::write_ref_attr(assignment, "RelatingFlowElement", relating_flow_element);
    return assignment;
}

IfcUtil::IfcBaseClass* system_assign_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* port,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (is_ifc2x3(file)) {
        for (auto* rel : inverse_entities(element, "HasPorts")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") == port) return rel;
        }
        auto* rel = create_owner_relation(file, "IfcRelConnectsPortToElement", owner_history, user, application);
        ifcapi::detail::write_ref_attr(rel, "RelatingPort", port);
        ifcapi::detail::write_ref_attr(rel, "RelatedElement", element);
        update_port_placement(file, port);
        return rel;
    }

    auto rels = inverse_entities(element, "IsNestedBy");
    for (auto* rel : rels) {
        if (contains(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), port)) return rel;
    }

    IfcUtil::IfcBaseClass* rel = nullptr;
    if (!rels.empty()) {
        rel = rels.front();
        auto related = append_unique(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects"), port);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    } else {
        rel = create_owner_relation(file, "IfcRelNests", owner_history, user, application);
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {port});
        ifcapi::detail::write_ref_attr(rel, "RelatingObject", element);
    }
    update_port_placement(file, port);
    return rel;
}

IfcUtil::IfcBaseClass* system_assign_system(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* system,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    for (auto* product : mutable_entities(products)) {
        if (!is_assignable(product, system)) {
            throw std::runtime_error("product is not assignable to system");
        }
    }
    return group_assign_group(file, products, system, owner_history, user, application);
}

void system_connect_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* port1,
    IfcUtil::IfcBaseClass* port2,
    const std::string& direction,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* owner_history,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (port1 == port2) return;

    purge_existing_connections_to_other_ports(file, port1, port2);

    if (direction == "SOURCE") {
        write_flow_direction(port1, "SOURCE");
        write_flow_direction(port2, "SINK");
    } else if (direction == "SINK") {
        write_flow_direction(port1, "SINK");
        write_flow_direction(port2, "SOURCE");
    } else {
        write_flow_direction(port1, direction);
        write_flow_direction(port2, direction);
    }

    if (direction == "SOURCE" || direction == "SOURCEANDSINK" || direction == "NOTDEFINED") {
        if (inverse_entities(port1, "ConnectedTo").empty()) {
            create_connects_ports(file, port1, port2, owner_history, user, application);
        }
    } else {
        purge_connected_to(file, port1);
    }

    if (direction == "SINK" || direction == "SOURCEANDSINK" || direction == "NOTDEFINED") {
        if (inverse_entities(port1, "ConnectedFrom").empty()) {
            create_connects_ports(file, port2, port1, owner_history, user, application);
        }
    } else {
        purge_connected_from(file, port1);
    }

    for (auto* rel : inverse_entities(port1, "ConnectedTo")) {
        ifcapi::detail::write_ref_attr(rel, "RealizingElement", element);
    }
    for (auto* rel : inverse_entities(port1, "ConnectedFrom")) {
        ifcapi::detail::write_ref_attr(rel, "RealizingElement", element);
    }
}

void system_disconnect_port(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* port) {
    auto rels = inverse_entities(port, "ConnectedTo");
    auto from = inverse_entities(port, "ConnectedFrom");
    rels.insert(rels.end(), from.begin(), from.end());
    for (auto* rel : rels) {
        clear_flow_direction(ifcapi::detail::read_ref_attr(rel, "RelatingPort"));
        clear_flow_direction(ifcapi::detail::read_ref_attr(rel, "RelatedPort"));
        remove_relation_with_history(file, rel);
    }
}

void system_remove_system(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* system) {
    std::vector<int> inverse_ids;
    for (auto* inverse : inverse_entities(file, system)) {
        inverse_ids.push_back(inverse->id());
    }
    for (int id : inverse_ids) {
        IfcUtil::IfcBaseClass* inverse = nullptr;
        try {
            inverse = file->instance_by_id(id);
        } catch (...) {
            continue;
        }
        if (!inverse) continue;
        if (is_a(inverse, "IfcRelDefinesByProperties")) {
            pset_remove_pset(file, system, ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition"));
        } else if (is_a(inverse, "IfcRelAssignsToGroup")) {
            if (ifcapi::detail::read_ref_attr(inverse, "RelatingGroup") == system
                || ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() == 1) {
                remove_relation_with_history(file, inverse);
            }
        }
    }
    ifcapi::detail::remove_with_history(file, system);
}

void system_unassign_flow_control(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* relating_flow_element,
    IfcUtil::IfcBaseClass* related_flow_control,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    auto assigned = inverse_entities(related_flow_control, "AssignedToFlowElement");
    if (assigned.empty()) return;
    auto* assignment = assigned.front();
    if (ifcapi::detail::read_ref_attr(assignment, "RelatingFlowElement") != relating_flow_element) return;
    auto controls = ifcapi::detail::read_ref_aggregate(assignment, "RelatedControlElements");
    if (controls.size() == 1) {
        remove_relation_with_history(file, assignment);
        return;
    }
    controls.erase(std::remove(controls.begin(), controls.end(), related_flow_control), controls.end());
    ifcapi::detail::write_ref_aggregate(assignment, "RelatedControlElements", controls);
    ifcapi::detail::update_owner_history(file, assignment, user, application);
}

void system_unassign_port(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* port,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    if (is_ifc2x3(file)) {
        for (auto* rel : inverse_entities(element, "HasPorts")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingPort") == port) {
                remove_relation_with_history(file, rel);
                return;
            }
        }
        return;
    }

    for (auto* rel : inverse_entities(element, "IsNestedBy")) {
        auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects");
        if (!contains(related, port)) continue;
        if (related.size() == 1) {
            remove_relation_with_history(file, rel);
            return;
        }
        related.erase(std::remove(related.begin(), related.end(), port), related.end());
        ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", related);
        ifcapi::detail::update_owner_history(file, rel, user, application);
    }
}

void system_unassign_system(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& products,
    IfcUtil::IfcBaseClass* system,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    group_unassign_group(file, products, system, user, application);
}

} // namespace bindings
} // namespace ifcapi
