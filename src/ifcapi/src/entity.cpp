// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/entity.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcBaseClass.h"

#include <vector>

namespace ifcapi {
namespace bindings {

void entity_remove_deep2(IfcUtil::IfcBaseClass* instance) {
    if (!instance) return;
    auto* file = instance->file_;
    if (!file) return;
    try {
        // Collect all entities referenced by this entity (traverse 1 level).
        std::vector<IfcUtil::IfcBaseClass*> referenced;
        auto traversed = file->traverse(instance, 1);
        if (traversed) {
            for (auto& ref : *traversed) {
                if (ref != instance) {
                    referenced.push_back(ref);
                }
            }
        }

        file->removeEntity(instance);

        for (auto* ref : referenced) {
            try {
                auto* check = file->instance_by_id(ref->id());
                if (!check) continue;
                auto inverses = file->getInverse(ref->id(), nullptr, -1);
                if (!inverses || inverses->size() == 0) {
                    entity_remove_deep2(ref);
                }
            } catch (...) {
                // Entity was already removed or inaccessible — skip.
            }
        }
    } catch (...) {}
}

} // namespace bindings
} // namespace ifcapi
