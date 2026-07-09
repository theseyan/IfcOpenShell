// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/element.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/file.h"

#include <deque>
#include <set>
#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

namespace {

bool is_a(const express::Base& entity, const char* name) {
    return entity && entity.declaration().is(name);
}

int32_t id_of(const express::Base& entity) {
    return entity ? static_cast<int32_t>(entity.id()) : 0;
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

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

express::Base resolve_aggregate(ifcopenshell::file* file, const express::Base& element);
express::Base resolve_nest(ifcopenshell::file* file, const express::Base& element);

express::Base resolve_type(ifcopenshell::file* file, const express::Base& element) {
    if (!element) {
        return {};
    }
    if (is_a(element, "IfcTypeObject")) {
        return element;
    }

    if (!is_ifc2x3(file) && is_a(element, "IfcObject")) {
        auto inv = get_inverse(element, "IsTypedBy");
        return inv.empty() ? express::Base() : read_ref(inv.front(), "RelatingType");
    }

    for (const auto& rel : get_inverse(element, "IsDefinedBy")) {
        if (is_a(rel, "IfcRelDefinesByType")) {
            return read_ref(rel, "RelatingType");
        }
    }
    return {};
}

express::Base resolve_aggregate(ifcopenshell::file* file, const express::Base& element) {
    if (!element) {
        return {};
    }
    auto inv = get_inverse(element, "Decomposes");
    if (inv.empty()) {
        return {};
    }
    auto rel = inv.front();
    if (is_ifc2x3(file) && !is_a(rel, "IfcRelAggregates")) {
        return {};
    }
    return read_ref(rel, "RelatingObject");
}

express::Base resolve_nest(ifcopenshell::file* file, const express::Base& element) {
    if (!element) {
        return {};
    }
    if (is_ifc2x3(file)) {
        auto inv = get_inverse(element, "Decomposes");
        if (inv.empty() || !is_a(inv.front(), "IfcRelNests")) {
            return {};
        }
        return read_ref(inv.front(), "RelatingObject");
    }
    auto inv = get_inverse(element, "Nests");
    return inv.empty() ? express::Base() : read_ref(inv.front(), "RelatingObject");
}

express::Base resolve_filled_void(const express::Base& element) {
    auto inv = get_inverse(element, "FillsVoids");
    return inv.empty() ? express::Base() : read_ref(inv.front(), "RelatingOpeningElement");
}

express::Base resolve_voided_element(const express::Base& element) {
    auto inv = get_inverse(element, "VoidsElements");
    return inv.empty() ? express::Base() : read_ref(inv.front(), "RelatingBuildingElement");
}

express::Base resolve_container_direct(const express::Base& element) {
    auto inv = get_inverse(element, "ContainedInStructure");
    return inv.empty() ? express::Base() : read_ref(inv.front(), "RelatingStructure");
}

express::Base resolve_parent(ifcopenshell::file* file, const express::Base& element) {
    if (!element) {
        return {};
    }
    if (auto container = resolve_container_direct(element)) {
        return container;
    }
    if (auto aggregate = resolve_aggregate(file, element)) {
        return aggregate;
    }
    if (auto nest = resolve_nest(file, element)) {
        return nest;
    }
    if (auto filled_void = resolve_filled_void(element)) {
        return filled_void;
    }
    return resolve_voided_element(element);
}

express::Base resolve_container(
    ifcopenshell::file* file,
    const express::Base& element,
    bool direct_only,
    const char* ifc_class)
{
    if (!element) {
        return {};
    }
    if (direct_only) {
        auto container = resolve_container_direct(element);
        if (!container || (ifc_class && *ifc_class != '\0' && !is_a(container, ifc_class))) {
            return {};
        }
        return container;
    }
    if (auto container = resolve_container_direct(element)) {
        if (!ifc_class || *ifc_class == '\0') {
            return container;
        }
        while (container) {
            if (is_a(container, ifc_class)) {
                return container;
            }
            container = resolve_aggregate(file, container);
        }
        return {};
    }
    if (auto parent = resolve_parent(file, element)) {
        return resolve_container(file, parent, direct_only, ifc_class);
    }
    return {};
}

} // namespace

std::optional<express::Base> element_get_type(express::Base* instance) {
    if (!instance) return std::nullopt;
    auto result = resolve_type(instance->file(), *instance);
    return result ? std::optional(result) : std::nullopt;
}

std::optional<express::Base> element_get_aggregate(express::Base* instance) {
    if (!instance) return std::nullopt;
    auto result = resolve_aggregate(instance->file(), *instance);
    return result ? std::optional(result) : std::nullopt;
}

std::optional<express::Base> element_get_nest(express::Base* instance) {
    if (!instance) return std::nullopt;
    auto result = resolve_nest(instance->file(), *instance);
    return result ? std::optional(result) : std::nullopt;
}

std::optional<express::Base> element_get_container(
    express::Base* instance,
    const ElementGetContainerOptions& options)
{
    bool direct_only = options.direct_only.value_or(false);
    const char* ifc_class = options.ifc_class ? options.ifc_class->c_str() : nullptr;
    if (!instance) return std::nullopt;
    auto result = resolve_container(instance->file(), *instance, direct_only, ifc_class);
    return result ? std::optional(result) : std::nullopt;
}

std::optional<express::Base> element_get_parent(express::Base* instance) {
    if (!instance) return std::nullopt;
    auto result = resolve_parent(instance->file(), *instance);
    return result ? std::optional(result) : std::nullopt;
}

std::optional<express::Base> element_get_material(
    express::Base* instance,
    const ElementGetMaterialOptions& options)
{
    if (!instance || !*instance) {
        return std::nullopt;
    }
    bool should_skip_usage = options.should_skip_usage.value_or(false);
    bool should_inherit = options.should_inherit.value_or(true);

    for (const auto& rel : get_inverse(*instance, "HasAssociations")) {
        if (!is_a(rel, "IfcRelAssociatesMaterial")) {
            continue;
        }
        auto material = read_ref(rel, "RelatingMaterial");
        if (!material) {
            continue;
        }
        if (should_skip_usage) {
            if (is_a(material, "IfcMaterialLayerSetUsage")) {
                return read_ref(material, "ForLayerSet");
            }
            if (is_a(material, "IfcMaterialProfileSetUsage")) {
                return read_ref(material, "ForProfileSet");
            }
        }
        return material;
    }

    if (should_inherit) {
        auto type_obj = resolve_type(instance->file(), *instance);
        if (type_obj && type_obj != *instance && !get_inverse(type_obj, "HasAssociations").empty()) {
            ElementGetMaterialOptions inherited_options;
            inherited_options.should_skip_usage = should_skip_usage;
            inherited_options.should_inherit = false;
            return element_get_material(&type_obj, inherited_options);
        }
    }
    return std::nullopt;
}

std::vector<express::Base> element_get_decomposition(
    express::Base* instance,
    const ElementGetDecompositionOptions& options)
{
    if (!instance || !*instance) {
        return {};
    }
    bool is_recursive = options.is_recursive.value_or(true);

    std::set<int32_t> seen;
    std::vector<express::Base> result;
    std::deque<express::Base> queue;
    queue.push_back(*instance);

    auto push_all = [&](const std::vector<express::Base>& refs) {
        for (const auto& ref : refs) {
            int32_t id = id_of(ref);
            if (id && seen.insert(id).second) {
                result.push_back(ref);
                queue.push_back(ref);
            }
        }
    };

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop_front();

        for (const auto& rel : get_inverse(current, "ContainsElements")) {
            push_all(read_ref_list(rel, "RelatedElements"));
        }
        for (const auto& rel : get_inverse(current, "IsDecomposedBy")) {
            push_all(read_ref_list(rel, "RelatedObjects"));
        }
        for (const auto& rel : get_inverse(current, "HasOpenings")) {
            auto opening = read_ref(rel, "RelatedOpeningElement");
            int32_t id = id_of(opening);
            if (id && seen.insert(id).second) {
                result.push_back(opening);
                queue.push_back(opening);
            }
        }
        for (const auto& rel : get_inverse(current, "HasFillings")) {
            auto filler = read_ref(rel, "RelatedBuildingElement");
            int32_t id = id_of(filler);
            if (id && seen.insert(id).second) {
                result.push_back(filler);
                queue.push_back(filler);
            }
        }
        for (const auto& rel : get_inverse(current, "IsNestedBy")) {
            push_all(read_ref_list(rel, "RelatedObjects"));
        }
        if (!is_recursive) {
            break;
        }
    }

    return result;
}

std::vector<express::Base> element_get_pset_ids(
    express::Base* element,
    const ElementGetPsetIdsOptions& options)
{
    if (!element || !*element) {
        return {};
    }
    bool psets_only = options.psets_only.value_or(false);
    bool qtos_only = options.qtos_only.value_or(false);
    bool should_inherit = options.should_inherit.value_or(true);
    auto* file = element->file();
    bool file_is_ifc2x3 = is_ifc2x3(file);
    std::vector<express::Base> result;
    std::set<int32_t> seen;

    auto push_def = [&](const express::Base& definition) {
        if (!definition) {
            return;
        }
        if (psets_only &&
            !is_a(definition, "IfcPropertySet") &&
            !is_a(definition, "IfcPreDefinedPropertySet") &&
            !(file_is_ifc2x3 && is_a(definition, "IfcExtendedMaterialProperties"))) {
            return;
        }
        if (qtos_only && !is_a(definition, "IfcElementQuantity")) {
            return;
        }
        int32_t id = id_of(definition);
        if (id && seen.insert(id).second) {
            result.push_back(definition);
        }
    };

    if (is_a(*element, "IfcTypeObject")) {
        for (const auto& definition : read_ref_list(*element, "HasPropertySets")) {
            push_def(definition);
        }
        return result;
    }

    if ((file_is_ifc2x3 && is_a(*element, "IfcMaterial")) ||
        is_a(*element, "IfcMaterialDefinition") ||
        is_a(*element, "IfcProfileDef")) {
        if (qtos_only) {
            return result;
        }
        if (file_is_ifc2x3 && is_a(*element, "IfcMaterial")) {
            for (const auto& instance : ifcapi::detail::instances_by_type(file, "IfcExtendedMaterialProperties")) {
                if (read_ref(instance, "Material") == *element) {
                    push_def(instance);
                }
            }
        } else if (!file_is_ifc2x3) {
            for (const auto& inverse : get_inverse(*element, "HasProperties")) {
                push_def(inverse);
            }
        }
        return result;
    }

    auto is_defined_by = get_inverse(*element, "IsDefinedBy");
    if (!is_defined_by.empty()) {
        if (should_inherit) {
            auto type_obj = resolve_type(file, *element);
            if (type_obj) {
                ElementGetPsetIdsOptions inherited_options;
                inherited_options.psets_only = psets_only;
                inherited_options.qtos_only = qtos_only;
                inherited_options.should_inherit = false;
                for (const auto& inherited : element_get_pset_ids(&type_obj, inherited_options)) {
                    push_def(inherited);
                }
            }
        }
        for (const auto& rel : is_defined_by) {
            if (is_a(rel, "IfcRelDefinesByProperties")) {
                push_def(read_ref(rel, "RelatingPropertyDefinition"));
            }
        }
    }

    return result;
}

} // namespace bindings
} // namespace ifcapi
