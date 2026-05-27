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

void write_logical_attr(express::Base entity, const char* attr, boost::logic::tribool value) {
    int idx = ifcapi::detail::attr_index_of(entity, attr);
    if (idx >= 0) {
        if (boost::logic::indeterminate(value)) {
            entity.set_attribute_value(static_cast<size_t>(idx), std::string("UNKNOWN"));
        } else if (value) {
            entity.set_attribute_value(static_cast<size_t>(idx), true);
        } else {
            entity.set_attribute_value(static_cast<size_t>(idx), false);
        }
    }
}

std::vector<express::Base> mutable_items(const std::vector<express::Base>& items) {
    std::vector<express::Base> result;
    result.reserve(items.size());
    for (auto item : items) if (item) result.push_back(item);
    return result;
}

bool contains(const std::vector<express::Base>& items, express::Base item) {
    return std::find(items.begin(), items.end(), item) != items.end();
}

bool contains_all(
    const std::vector<express::Base>& assigned,
    const std::vector<express::Base>& selected)
{
    return std::all_of(selected.begin(), selected.end(), [&](auto item) { return contains(assigned, item); });
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base layer_add_layer(
    ifcopenshell::file* file,
    const std::string& name)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcPresentationLayerAssignment");
    auto result = file->create(declaration);
    detail::write_string_attr(result, "Name", name);
    return result;
}

express::Base layer_add_layer_with_style(
    ifcopenshell::file* file,
    const std::string& name,
    boost::logic::tribool on,
    boost::logic::tribool frozen,
    boost::logic::tribool blocked,
    const std::vector<express::Base>& styles)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcPresentationLayerWithStyle");
    auto result = file->create(declaration);
    detail::write_string_attr(result, "Name", name);
    write_logical_attr(result, "LayerOn", on);
    write_logical_attr(result, "LayerFrozen", frozen);
    write_logical_attr(result, "LayerBlocked", blocked);
    detail::write_ref_aggregate(result, "LayerStyles", mutable_items(styles));
    return result;
}

void layer_assign_layer(
    ifcopenshell::file*,
    const std::vector<express::Base>& items,
    express::Base* layer)
{
    auto layer_value = detail::deref_or_empty(layer);
    auto assigned = detail::read_ref_aggregate(layer_value, "AssignedItems");
    auto selected = mutable_items(items);
    if (contains_all(assigned, selected)) return;
    for (auto item : selected) {
        if (!contains(assigned, item)) assigned.push_back(item);
    }
    detail::write_ref_aggregate(layer_value, "AssignedItems", assigned);
}

void layer_unassign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer)
{
    auto layer_value = detail::deref_or_empty(layer);
    if (detail::is_attr_null(layer_value, "AssignedItems")) {
        throw std::runtime_error("Layer has no assigned items");
    }
    auto assigned = detail::read_ref_aggregate(layer_value, "AssignedItems");
    auto selected = mutable_items(items);
    if (!contains_all(assigned, selected)) return;
    std::vector<express::Base> kept;
    for (auto item : assigned) {
        if (!contains(selected, item)) kept.push_back(item);
    }
    if (kept.empty()) {
        file->remove_entity(layer_value);
    } else {
        detail::write_ref_aggregate(layer_value, "AssignedItems", kept);
    }
}

void layer_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer)
{
    file->remove_entity(detail::deref_or_empty(layer));
}

} // namespace bindings
} // namespace ifcapi
