// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/detail/attribute.h"

#include <deque>
#include <vector>

namespace {

bool is_a(const express::Base& entity, const char* name) {
    return entity && entity.declaration().is(name);
}

std::vector<express::Base> get_inverse(const express::Base& entity, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(entity, attr);
}

express::Base read_ref(const express::Base& entity, const char* attr) {
    return ifcapi::detail::read_ref_attr(entity, attr);
}

std::vector<express::Base> read_ref_list(const express::Base& entity, const char* attr) {
    return ifcapi::detail::read_ref_aggregate(entity, attr);
}

void collect_materials(const express::Base& material, std::vector<express::Base>& out) {
    if (!material) {
        return;
    }
    if (is_a(material, "IfcMaterial")) {
        out.push_back(material);
        return;
    }
    if (is_a(material, "IfcMaterialLayerSet")) {
        for (const auto& layer : read_ref_list(material, "MaterialLayers")) {
            if (auto layer_material = read_ref(layer, "Material")) {
                out.push_back(layer_material);
            }
        }
        return;
    }
    if (is_a(material, "IfcMaterialProfileSet")) {
        for (const auto& profile : read_ref_list(material, "MaterialProfiles")) {
            if (auto profile_material = read_ref(profile, "Material")) {
                out.push_back(profile_material);
            }
        }
        return;
    }
    if (is_a(material, "IfcMaterialConstituentSet")) {
        for (const auto& constituent : read_ref_list(material, "MaterialConstituents")) {
            if (auto constituent_material = read_ref(constituent, "Material")) {
                out.push_back(constituent_material);
            }
        }
        return;
    }
    if (is_a(material, "IfcMaterialList")) {
        for (const auto& item : read_ref_list(material, "Materials")) {
            if (item) {
                out.push_back(item);
            }
        }
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

std::vector<express::Base> element_get_styles(express::Base* element) {
    if (!element || !*element) {
        return {};
    }

    std::vector<express::Base> styles;
    auto material = ifcapi::bindings::element_get_material(element, true, true);
    std::vector<express::Base> materials;
    collect_materials(material, materials);
    for (const auto& material_item : materials) {
        auto material_representations = read_ref_list(material_item, "HasRepresentation");
        if (material_representations.empty()) {
            material_representations = get_inverse(material_item, "HasRepresentation");
        }
        for (const auto& material_representation : material_representations) {
            for (const auto& representation : read_ref_list(material_representation, "Representations")) {
                for (const auto& item : read_ref_list(representation, "Items")) {
                    for (const auto& style : read_ref_list(item, "Styles")) {
                        if (is_a(style, "IfcSurfaceStyle")) {
                            styles.push_back(style);
                        }
                    }
                }
            }
        }
    }

    auto body = ifcapi::bindings::representation_get_product_representation(
        element, nullptr, "Model", "Body", "MODEL_VIEW");
    if (body) {
        std::deque<express::Base> queue;
        for (const auto& item : read_ref_list(body, "Items")) {
            queue.push_back(item);
        }
        while (!queue.empty()) {
            auto item = queue.back();
            queue.pop_back();
            if (is_a(item, "IfcMappedItem")) {
                auto source = read_ref(item, "MappingSource");
                auto mapped = read_ref(source, "MappedRepresentation");
                for (const auto& subitem : read_ref_list(mapped, "Items")) {
                    queue.push_back(subitem);
                }
            }
            if (is_a(item, "IfcBooleanResult")) {
                if (auto first = read_ref(item, "FirstOperand")) {
                    queue.push_back(first);
                }
                if (auto second = read_ref(item, "SecondOperand")) {
                    queue.push_back(second);
                }
            }
            auto styled_by = get_inverse(item, "StyledByItem");
            if (!styled_by.empty()) {
                for (const auto& style : read_ref_list(styled_by.front(), "Styles")) {
                    if (is_a(style, "IfcSurfaceStyle")) {
                        styles.push_back(style);
                    }
                }
            }
        }
    }

    return styles;
}

} // namespace bindings
} // namespace ifcapi
