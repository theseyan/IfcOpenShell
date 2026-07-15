// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/schema.h"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/error.h"

#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <exception>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct TypeMapEntry {
    const char* occurrence;
    std::vector<const char*> types;
};

#include "root_type_map.inc"

using TypeMap = std::vector<std::pair<std::string, std::vector<std::string>>>;

template <size_t Size>
TypeMap make_type_map(const TypeMapEntry (&entries)[Size], bool ifc2x3) {
    TypeMap result;
    result.reserve(Size);
    for (const auto& entry : entries) {
        std::vector<std::string> types;
        types.reserve(entry.types.size());
        for (const auto* type : entry.types) types.emplace_back(type);
        if (ifc2x3) {
            std::stable_sort(types.begin(), types.end(), [](const std::string& left, const std::string& right) {
                return left == "IfcBuildingElementProxyType" && right != "IfcBuildingElementProxyType";
            });
        }
        result.emplace_back(entry.occurrence, std::move(types));
    }
    return result;
}

std::string fallback_schema(const std::string& schema) {
    if (schema.rfind("IFC2X3", 0) == 0) return "IFC2X3";
    if (schema.rfind("IFC4X3", 0) == 0) return "IFC4X3";
    if (schema.rfind("IFC4", 0) == 0) return "IFC4";
    return {};
}

TypeMap occurrence_to_type_map(ifcopenshell::file* file) {
    const auto schema = fallback_schema(file && file->schema() ? file->schema()->name() : std::string());
    if (schema == "IFC2X3") return make_type_map(TYPE_MAP_IFC2X3, true);
    if (schema == "IFC4X3") return make_type_map(TYPE_MAP_IFC4X3, false);
    if (schema == "IFC4") return make_type_map(TYPE_MAP_IFC4, false);
    return {};
}

std::vector<std::string> applicable_types(ifcopenshell::file* file, const std::string& occurrence_class) {
    for (const auto& [occurrence, types] : occurrence_to_type_map(file)) {
        if (occurrence == occurrence_class) return types;
    }
    return {};
}

std::vector<std::string> applicable_occurrences(ifcopenshell::file* file, const std::string& type_class) {
    std::vector<std::string> result;
    const bool ifc2x3 = fallback_schema(file->schema()->name()) == "IFC2X3";
    for (const auto& [occurrence, types] : occurrence_to_type_map(file)) {
        if (std::find(types.begin(), types.end(), type_class) != types.end()) result.push_back(occurrence);
    }
    if (ifc2x3) {
        std::string guessed = type_class;
        if (guessed.size() >= 5 && guessed.compare(guessed.size() - 5, 5, "Style") == 0) {
            guessed.resize(guessed.size() - 5);
        } else if (guessed.size() >= 4) {
            guessed.resize(guessed.size() - 4);
        }
        if (std::find(result.begin(), result.end(), guessed) != result.end()) {
            std::vector<std::string> filtered;
            for (const auto& occurrence : result) {
                if (occurrence.find(guessed) != std::string::npos) filtered.push_back(occurrence);
            }
            result = std::move(filtered);
        }
    }
    return result;
}

bool is_a(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

bool declaration_is(ifcopenshell::file* file, const std::string& ifc_class, const char* supertype) {
    try {
        auto* declaration = file->schema()->declaration_by_name(ifc_class);
        auto* entity = declaration ? declaration->as_entity() : nullptr;
        return entity && entity->is(supertype);
    } catch (...) {
        return false;
    }
}

bool entity_declaration_exists(ifcopenshell::file* file, const std::string& ifc_class) {
    try {
        auto* declaration = file->schema()->declaration_by_name(ifc_class);
        return declaration && declaration->as_entity();
    } catch (...) {
        return false;
    }
}

void set_value_error(const std::string& message) {
    ifcapi::detail::set_error(ifcapi::detail::ERROR_VALUE, message);
}

std::vector<express::Base> representations(express::Base element) {
    std::vector<express::Base> result;
    if (is_a(element, "IfcProduct")) {
        auto definition = ifcapi::detail::read_ref_attr(element, "Representation");
        return ifcapi::detail::read_ref_aggregate(definition, "Representations");
    }
    if (is_a(element, "IfcTypeProduct")) {
        for (auto map : ifcapi::detail::read_ref_aggregate(element, "RepresentationMaps")) {
            auto representation = ifcapi::detail::read_ref_attr(map, "MappedRepresentation");
            if (representation) result.push_back(representation);
        }
    }
    return result;
}

std::vector<express::Base> direct_property_sets(express::Base occurrence) {
    std::vector<express::Base> result;
    for (auto relation : ifcapi::detail::read_inverse_aggregate(occurrence, "IsDefinedBy")) {
        if (!is_a(relation, "IfcRelDefinesByProperties")) continue;
        auto property_set = ifcapi::detail::read_ref_attr(relation, "RelatingPropertyDefinition");
        if (property_set && std::find(result.begin(), result.end(), property_set) == result.end()) {
            result.push_back(property_set);
        }
    }
    return result;
}

void apply_predefined_type(express::Base element, const std::optional<std::string>& predefined_type) {
    if (!element || !predefined_type || predefined_type->empty()
        || !ifcapi::detail::entity_has_attr(element, "PredefinedType")) {
        return;
    }
    if (ifcapi::detail::write_enum_attr(element, "PredefinedType", *predefined_type)) return;
    ifcapi::detail::write_enum_attr(element, "PredefinedType", "USERDEFINED");
    ifcapi::detail::write_string_attr(
        element,
        is_a(element, "IfcTypeProduct") ? "ElementType" : "ObjectType",
        *predefined_type);
}

express::Base reassign_one(
    ifcopenshell::file* file,
    express::Base element,
    const std::string& ifc_class,
    const std::optional<std::string>& predefined_type)
{
    auto result = ifcapi::bindings::schema_reassign_class(file, &element, ifc_class);
    if (result) apply_predefined_type(result, predefined_type);
    return result;
}

express::Base simple_reassignment(
    ifcopenshell::file* file,
    express::Base element,
    const std::string& ifc_class,
    const std::optional<std::string>& predefined_type,
    const std::optional<std::string>& occurrence_override)
{
    auto result = reassign_one(file, element, ifc_class, predefined_type);
    if (!result) return {};

    if (is_a(result, "IfcTypeProduct")) {
        std::string occurrence_class;
        if (occurrence_override && !occurrence_override->empty()) {
            occurrence_class = *occurrence_override;
        } else {
            auto candidates = applicable_occurrences(file, ifc_class);
            if (candidates.empty()) {
                set_value_error("No applicable occurrence class for " + ifc_class);
                return {};
            }
            occurrence_class = candidates.front();
        }
        if (!entity_declaration_exists(file, occurrence_class)) {
            set_value_error("Occurrence class does not exist in schema: " + occurrence_class);
            return {};
        }
        if (declaration_is(file, occurrence_class, "IfcTypeProduct")) {
            set_value_error("Unexpected occurrence_class: '" + occurrence_class + "'.");
            return {};
        }
        auto occurrences = ifcapi::bindings::element_get_types(&result);
        for (auto occurrence : occurrences) {
            if (!reassign_one(file, occurrence, occurrence_class, predefined_type)) return {};
        }
    } else {
        auto related_type = ifcapi::bindings::element_get_type(&result);
        if (related_type) {
            auto candidates = applicable_types(file, ifc_class);
            if (candidates.empty()) {
                set_value_error("No applicable type class for " + ifc_class);
                return {};
            }
            auto reassigned_type = reassign_one(file, *related_type, candidates.front(), predefined_type);
            if (!reassigned_type) return {};
            const auto result_id = result.id();
            const auto exact_occurrence_class = result.declaration().name();
            for (auto occurrence : ifcapi::bindings::element_get_types(&reassigned_type)) {
                if (occurrence.id() == result_id) continue;
                if (!reassign_one(file, occurrence, exact_occurrence_class, predefined_type)) return {};
            }
        }
    }
    return result;
}

express::Base switch_class_kind(
    ifcopenshell::file* file,
    express::Base element,
    const std::string& ifc_class,
    const std::optional<std::string>& predefined_type,
    bool occurrence_to_type)
{
    auto saved_representations = representations(element);
    for (auto representation : saved_representations) {
        auto to_unassign = representation;
        if (!occurrence_to_type) {
            to_unassign = ifcapi::bindings::representation_resolve(&to_unassign);
        }
        ifcapi::bindings::geometry_unassign_representation(file, &element, &to_unassign);
    }

    std::vector<express::Base> property_sets;
    if (!occurrence_to_type) {
        auto occurrences = ifcapi::bindings::element_get_types(&element);
        property_sets = ifcapi::detail::read_ref_aggregate(element, "HasPropertySets");
        element = reassign_one(file, element, ifc_class, predefined_type);
        if (!element) return {};
        ifcapi::bindings::type_unassign_type(file, {occurrences, std::nullopt, std::nullopt});
    } else {
        if (ifcapi::bindings::element_get_type(&element)) {
            ifcapi::bindings::type_unassign_type(file, {{element}, std::nullopt, std::nullopt});
        }
        if (ifcapi::bindings::element_get_container(&element, {false, std::nullopt})) {
            ifcapi::bindings::spatial_unassign_container(file, {{element}, std::nullopt, std::nullopt});
        } else if (ifcapi::bindings::element_get_aggregate(&element)) {
            ifcapi::bindings::aggregate_unassign_object(file, {{element}, std::nullopt, std::nullopt});
        }
        property_sets = direct_property_sets(element);
        for (auto property_set : property_sets) {
            ifcapi::bindings::pset_unassign_pset(file, {element}, &property_set);
        }
        element = reassign_one(file, element, ifc_class, predefined_type);
        if (!element) return {};
    }

    for (auto property_set : property_sets) {
        ifcapi::bindings::pset_assign_pset(file, {{element}, property_set, std::nullopt, std::nullopt, std::nullopt});
    }
    for (auto representation : saved_representations) {
        ifcapi::bindings::geometry_assign_representation(file, &element, &representation);
    }
    if (!occurrence_to_type && !saved_representations.empty()) {
        ifcapi::bindings::geometry_edit_object_placement(file, {element, {}, true, false});
    }
    return element;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base root_reassign_class(ifcopenshell::file* file, const RootReassignClassOptions& options) {
    ifcopenshell_clear_error();
    if (!file || !options.product) {
        ifcopenshell::capi::set_last_error("root_reassign_class: missing file or product");
        return {};
    }
    const auto ifc_class = options.ifc_class.value_or("IfcBuildingElementProxy");
    try {
        auto* declaration = file->schema()->declaration_by_name(ifc_class);
        auto* entity_declaration = declaration ? declaration->as_entity() : nullptr;
        if (!entity_declaration) {
            set_value_error("Class does not exist in schema: " + ifc_class);
            return {};
        }
        const bool was_type = is_a(options.product, "IfcTypeProduct");
        const bool will_be_type = entity_declaration->is("IfcTypeProduct");
        if (was_type == will_be_type) {
            return simple_reassignment(
                file,
                options.product,
                ifc_class,
                options.predefined_type,
                options.occurrence_class);
        }
        return switch_class_kind(file, options.product, ifc_class, options.predefined_type, will_be_type);
    } catch (const ifcopenshell::exception&) {
        set_value_error("Class does not exist in schema: " + ifc_class);
        return {};
    } catch (const std::exception& exception) {
        ifcapi::detail::set_error(exception, "root_reassign_class: ");
        return {};
    } catch (...) {
        ifcopenshell::capi::set_last_error("root_reassign_class: unknown error");
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
