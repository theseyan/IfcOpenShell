// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/entity.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"

#include <queue>
#include <unordered_set>
#include <vector>

namespace ifcapi {
namespace bindings {

namespace {

std::vector<express::Base> traverse_list(
    const express::Base& element,
    int max_levels,
    bool breadth_first = false)
{
    return breadth_first
        ? ifcopenshell::file::traverse_breadth_first(element, max_levels)
        : ifcopenshell::file::traverse(element, max_levels);
}

std::vector<express::Base> inverse_list(ifcopenshell::file* file, const express::Base& element) {
    if (!file || !element || element.id() == 0) {
        return {};
    }
    return file->instances_by_reference(static_cast<int>(element.id()));
}

void clear_large_aggregate_attributes(const express::Base& element) {
    ifcapi::detail::entity_view view(element);
    auto* declaration = view.declaration();
    if (!declaration) {
        return;
    }
    for (size_t i = 0; i < declaration->attribute_count(); ++i) {
        auto value = view.attr(i);
        if (value.isNull() || value.type() != ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            continue;
        }
        auto aggregate = static_cast<std::vector<express::Base>>(value);
        if (aggregate.size() > 10) {
            view.set(i, std::vector<express::Base>());
        }
    }
}

bool contains_ref(const std::vector<express::Base>& values, const express::Base& needle) {
    return std::find(values.begin(), values.end(), needle) != values.end();
}

} // namespace

void entity_remove_deep(express::Base* instance) {
    entity_remove_deep_with_options(instance, EntityRemoveDeepOptions{});
}

void entity_remove_deep_with_options(
    express::Base* instance,
    const EntityRemoveDeepOptions& options)
{
    const auto& also_consider = options.also_consider;
    const auto& do_not_delete = options.do_not_delete;
    if (!instance || !*instance) {
        return;
    }
    auto* file = instance->file();
    if (!file) {
        return;
    }

    auto total_inverses = inverse_list(file, *instance).size();
    if (total_inverses > 0) {
        size_t also_considered_inverses = 0;
        bool contained = false;
        for (const auto& considered : also_consider) {
            auto traversed = traverse_list(considered, 1);
            if (contains_ref(traversed, *instance)) {
                ++also_considered_inverses;
                if (total_inverses == also_considered_inverses) {
                    contained = true;
                    break;
                }
            }
        }
        if (!contained) {
            return;
        }
    }

    auto subgraph = traverse_list(*instance, -1, true);
    subgraph.insert(subgraph.end(), also_consider.begin(), also_consider.end());

    std::unordered_set<express::Base> subgraph_set(subgraph.begin(), subgraph.end());
    std::unordered_set<express::Base> do_not_delete_set(do_not_delete.begin(), do_not_delete.end());
    std::unordered_set<express::Base> to_delete;
    std::unordered_set<unsigned> processed_ids;

    std::queue<express::Base> queue;
    queue.push(*instance);
    while (!queue.empty()) {
        auto subelement = queue.front();
        queue.pop();
        if (!subelement) {
            continue;
        }

        unsigned id = subelement.id();
        if (!id || processed_ids.find(id) != processed_ids.end()) {
            continue;
        }
        processed_ids.insert(id);
        if (do_not_delete_set.find(subelement) != do_not_delete_set.end()) {
            continue;
        }

        auto inverses = inverse_list(file, subelement);
        bool all_inverses_in_subgraph = true;
        for (const auto& inverse : inverses) {
            if (subgraph_set.find(inverse) == subgraph_set.end()) {
                all_inverses_in_subgraph = false;
                break;
            }
        }

        if (inverses.size() < 2 || all_inverses_in_subgraph) {
            to_delete.insert(subelement);
            auto children = traverse_list(subelement, 1);
            for (size_t i = 1; i < children.size(); ++i) {
                queue.push(children[i]);
            }
            clear_large_aggregate_attributes(subelement);
        }
    }

    for (auto it = subgraph.rbegin(); it != subgraph.rend(); ++it) {
        if (*it && to_delete.find(*it) != to_delete.end()) {
            file->remove_entity(*it);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
