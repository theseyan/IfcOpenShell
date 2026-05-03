// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/entity.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/ArgumentType.h"

#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace ifcapi {
namespace bindings {

namespace {

std::vector<IfcUtil::IfcBaseClass*> to_mutable(const std::vector<const IfcUtil::IfcBaseClass*>& values) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    result.reserve(values.size());
    for (auto* value : values) {
        if (value) result.push_back(const_cast<IfcUtil::IfcBaseClass*>(value));
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> traverse_list(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    int max_levels,
    bool breadth_first = false)
{
    std::vector<IfcUtil::IfcBaseClass*> result;
    auto traversed = breadth_first
        ? file->traverse_breadth_first(element, max_levels)
        : file->traverse(element, max_levels);
    if (!traversed) return result;
    for (auto& item : *traversed) {
        if (item) result.push_back(item);
    }
    return result;
}

std::vector<IfcUtil::IfcBaseClass*> inverse_list(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* element) {
    std::vector<IfcUtil::IfcBaseClass*> result;
    if (!element || !element->id()) return result;
    auto inverses = file->getInverse(element->id(), nullptr, -1);
    if (!inverses) return result;
    for (auto& inverse : *inverses) {
        if (inverse) result.push_back(inverse);
    }
    return result;
}

void clear_large_aggregate_attributes(IfcUtil::IfcBaseClass* element) {
    auto* entity = dynamic_cast<IfcUtil::IfcBaseEntity*>(element);
    if (!entity) return;
    auto* declaration = entity->declaration().as_entity();
    if (!declaration) return;

    for (size_t i = 0; i < declaration->attribute_count(); ++i) {
        auto value = element->get_attribute_value(i);
        if (value.isNull() || value.type() != IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) continue;
        auto aggregate = (aggregate_of_instance::ptr)value;
        if (aggregate && aggregate->size() > 10) {
            element->set_attribute_value(i, aggregate_of_instance::ptr(new aggregate_of_instance));
        }
    }
}

bool contains_ptr(const std::vector<IfcUtil::IfcBaseClass*>& values, IfcUtil::IfcBaseClass* needle) {
    for (auto* value : values) {
        if (value == needle) return true;
    }
    return false;
}

} // namespace

void entity_remove_deep2(IfcUtil::IfcBaseClass* instance) {
    entity_remove_deep2_ex(instance, {}, {});
}

void entity_remove_deep2_ex(
    IfcUtil::IfcBaseClass* instance,
    const std::vector<const IfcUtil::IfcBaseClass*>& also_consider_const,
    const std::vector<const IfcUtil::IfcBaseClass*>& do_not_delete_const)
{
    if (!instance) return;
    auto* file = instance->file_;
    if (!file) return;

    auto also_consider = to_mutable(also_consider_const);
    auto do_not_delete = to_mutable(do_not_delete_const);

    auto total_inverses = inverse_list(file, instance).size();
    if (total_inverses > 0) {
        size_t also_considered_inverses = 0;
        bool contained = false;
        for (auto* considered : also_consider) {
            auto traversed = traverse_list(file, considered, 1);
            if (contains_ptr(traversed, instance)) {
                ++also_considered_inverses;
                if (total_inverses == also_considered_inverses) {
                    contained = true;
                    break;
                }
            }
        }
        if (!contained) return;
    }

    auto subgraph = traverse_list(file, instance, -1, true);
    subgraph.insert(subgraph.end(), also_consider.begin(), also_consider.end());

    std::unordered_set<IfcUtil::IfcBaseClass*> subgraph_set(subgraph.begin(), subgraph.end());
    std::unordered_set<IfcUtil::IfcBaseClass*> do_not_delete_set(do_not_delete.begin(), do_not_delete.end());
    std::unordered_set<IfcUtil::IfcBaseClass*> to_delete;
    std::unordered_set<unsigned> processed_ids;

    std::queue<IfcUtil::IfcBaseClass*> queue;
    queue.push(instance);
    while (!queue.empty()) {
        auto* subelement = queue.front();
        queue.pop();
        if (!subelement) continue;

        unsigned id = subelement->id();
        if (!id || processed_ids.find(id) != processed_ids.end()) continue;
        processed_ids.insert(id);
        if (do_not_delete_set.find(subelement) != do_not_delete_set.end()) continue;

        auto inverses = inverse_list(file, subelement);
        bool all_inverses_in_subgraph = true;
        for (auto* inverse : inverses) {
            if (subgraph_set.find(inverse) == subgraph_set.end()) {
                all_inverses_in_subgraph = false;
                break;
            }
        }

        if (inverses.size() < 2 || all_inverses_in_subgraph) {
            to_delete.insert(subelement);
            auto children = traverse_list(file, subelement, 1);
            for (size_t i = 1; i < children.size(); ++i) queue.push(children[i]);
            clear_large_aggregate_attributes(subelement);
        }
    }

    for (auto it = subgraph.rbegin(); it != subgraph.rend(); ++it) {
        auto* subelement = *it;
        if (!subelement || to_delete.find(subelement) == to_delete.end()) continue;
        file->removeEntity(subelement);
    }
}

} // namespace bindings
} // namespace ifcapi
