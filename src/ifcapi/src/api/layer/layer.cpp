// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/layer.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <boost/logic/tribool.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void write_logical_attr(IfcUtil::IfcBaseClass* entity, const char* attr, boost::logic::tribool value) {
    int idx = ifcapi::detail::attr_index_of(entity, attr);
    if (idx >= 0) {
        if (boost::logic::indeterminate(value)) {
            entity->set_attribute_value(static_cast<size_t>(idx), std::string("UNKNOWN"));
        } else if (value) {
            entity->set_attribute_value(static_cast<size_t>(idx), true);
        } else {
            entity->set_attribute_value(static_cast<size_t>(idx), false);
        }
    }
}

std::vector<IfcUtil::IfcBaseClass*> mutable_items(const std::vector<const IfcUtil::IfcBaseClass*>& items) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(items.size());
    for (auto* item : items) {
        if (item) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(item));
    }
    return result;
}

bool contains(const std::vector<IfcUtil::IfcBaseClass*>& items, IfcUtil::IfcBaseClass* item) {
    return std::find(items.begin(), items.end(), item) != items.end();
}

bool contains_all(
    const std::vector<IfcUtil::IfcBaseClass*>& assigned,
    const std::vector<IfcUtil::IfcBaseClass*>& selected)
{
    return std::all_of(selected.begin(), selected.end(), [&](auto* item) { return contains(assigned, item); });
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* layer_add_layer(
    IfcParse::IfcFile* file,
    const std::string& name)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcPresentationLayerAssignment");
    auto* result = file->create(declaration);
    detail::write_string_attr(result, "Name", name);
    return result;
}

IfcUtil::IfcBaseClass* layer_add_layer_with_style(
    IfcParse::IfcFile* file,
    const std::string& name,
    boost::logic::tribool on,
    boost::logic::tribool frozen,
    boost::logic::tribool blocked,
    const std::vector<const IfcUtil::IfcBaseClass*>& styles)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcPresentationLayerWithStyle");
    auto* result = file->create(declaration);
    detail::write_string_attr(result, "Name", name);
    write_logical_attr(result, "LayerOn", on);
    write_logical_attr(result, "LayerFrozen", frozen);
    write_logical_attr(result, "LayerBlocked", blocked);
    detail::write_ref_aggregate(result, "LayerStyles", mutable_items(styles));
    return result;
}

void layer_assign_layer(
    IfcParse::IfcFile*,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* layer)
{
    auto assigned = detail::read_ref_aggregate(layer, "AssignedItems");
    auto selected = mutable_items(items);
    if (contains_all(assigned, selected)) return;
    for (auto* item : selected) {
        if (!contains(assigned, item)) assigned.push_back(item);
    }
    detail::write_ref_aggregate(layer, "AssignedItems", assigned);
}

void layer_unassign_layer(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* layer)
{
    if (detail::is_attr_null(layer, "AssignedItems")) {
        throw std::runtime_error("Layer has no assigned items");
    }
    auto assigned = detail::read_ref_aggregate(layer, "AssignedItems");
    auto selected = mutable_items(items);
    if (!contains_all(assigned, selected)) return;
    std::vector<IfcUtil::IfcBaseClass*> kept;
    for (auto* item : assigned) {
        if (!contains(selected, item)) kept.push_back(item);
    }
    if (kept.empty()) {
        file->removeEntity(layer);
    } else {
        detail::write_ref_aggregate(layer, "AssignedItems", kept);
    }
}

void layer_remove_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer)
{
    file->removeEntity(layer);
}

} // namespace bindings
} // namespace ifcapi
