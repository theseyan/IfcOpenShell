// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/project.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/error.h"
#include "ifcapi/detail/matrix.h"
#include "ifcapi/detail/pset.h"
#include "ifcapi/detail/relationship.h"
#include "placement_helpers.hpp"

#include "ifcparse/exception.h"
#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include "ifcopenshell_api.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct ifcapi::bindings::ProjectAppendAssetCache {
    struct Entry {
        express::Base target;
        uint32_t target_id = 0;
        uint32_t source_id = 0;
        std::string source_type;
    };

    std::map<std::int64_t, Entry> values;
    ifcopenshell::file* target_file = nullptr;
};

namespace {

using ifcapi::detail::entity_view;

void set_error(const std::string& message) {
    ifcopenshell::capi::set_last_error(message);
}

bool is_a(express::Base value, const char* ifc_class) {
    return value && value.declaration().is(ifc_class);
}

bool same_instance(express::Base left, express::Base right) {
    return ifcapi::detail::same_instance(left, right);
}

express::Base by_guid(ifcopenshell::file* file, express::Base value) {
    if (!file || !value || !value.declaration().as_entity()) {
        return {};
    }
    auto global_id = ifcapi::detail::read_optional_string_attr(value, "GlobalId");
    if (!global_id.has_value || global_id.value.empty()) {
        return {};
    }
    try {
        return file->instance_by_guid(global_id.value);
    } catch (...) {
        return {};
    }
}

bool is_length_type(const ifcopenshell::parameter_type* type) {
    while (type) {
        if (auto named = type->as_named_type()) {
            auto declaration = named->declared_type();
            if (declaration && declaration->name().find("LengthMeasure") != std::string::npos) return true;
            if (auto declared = declaration ? declaration->as_type_declaration() : nullptr) {
                type = declared->declared_type();
                continue;
            }
        }
        if (auto aggregate = type->as_aggregation_type()) {
            type = aggregate->type_of_element();
            continue;
        }
        break;
    }
    return false;
}

bool is_length_attribute(const ifcopenshell::entity* declaration, size_t index) {
    if (!declaration || index >= declaration->all_attributes().size()) {
        return false;
    }
    return is_length_type(declaration->all_attributes()[index]->type_of_attribute());
}

bool is_length_value(express::Base value) {
    return value && value.declaration().name().find("LengthMeasure") != std::string::npos;
}

std::optional<double> numeric_value(const attribute_value& value) {
    if (value.isNull()) return std::nullopt;
    if (value.type() == ifcopenshell::Argument_DOUBLE) return static_cast<double>(value);
    if (value.type() == ifcopenshell::Argument_INT) return static_cast<double>(static_cast<int>(value));
    if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
        auto nested = static_cast<express::Base>(value);
        return nested ? numeric_value(nested.get_attribute_value(0)) : std::nullopt;
    }
    return std::nullopt;
}

std::optional<double> optional_double(express::Base value, const char* name) {
    entity_view entity(value);
    auto index = entity.attr_index(name);
    return index < 0 ? std::nullopt : numeric_value(entity.attr(static_cast<size_t>(index)));
}

void append_unique(std::vector<express::Base>& values, express::Base value) {
    if (value && std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
    }
}

void append_unique(std::vector<express::Base>& values, const std::vector<express::Base>& additional) {
    for (auto value : additional) {
        append_unique(values, value);
    }
}

void replace_reference(ifcopenshell::file* file, express::Base old_value, express::Base new_value) {
    if (!file || !old_value || !new_value || old_value.id() == 0) {
        return;
    }
    auto inverses = file->instances_by_reference(static_cast<int>(old_value.id()));
    for (auto inverse : inverses) {
        ifcapi::detail::replace_attribute_reference(inverse, old_value, new_value);
    }
}

void prune_cache(ifcapi::bindings::ProjectAppendAssetCache* cache, ifcopenshell::file* target_file) {
    if (!cache || !target_file) {
        return;
    }
    for (auto it = cache->values.begin(); it != cache->values.end();) {
        auto& entry = it->second;
        bool valid = false;
        if (entry.target_id != 0) {
            try {
                auto current = target_file->instance_by_id(static_cast<int>(entry.target_id));
                valid = current && same_instance(current, entry.target);
                if (valid) {
                    entry.target = current;
                }
            } catch (...) {
                valid = false;
            }
        } else {
            valid = entry.target && entry.target.file() == target_file;
        }
        if (!valid) {
            it = cache->values.erase(it);
        } else {
            ++it;
        }
    }
}

void validate_cache(ifcapi::bindings::ProjectAppendAssetCache* cache, ifcopenshell::file* target_file) {
    if (!cache) {
        return;
    }
    if (cache->target_file && cache->target_file != target_file && !cache->values.empty()) {
        throw std::runtime_error("Append-asset cache belongs to a different target file");
    }
    if (cache->values.empty()) {
        cache->target_file = target_file;
    } else if (!cache->target_file) {
        cache->target_file = target_file;
    }
    if (cache->target_file != target_file) {
        throw std::runtime_error("Append-asset cache belongs to a different target file");
    }
    prune_cache(cache, target_file);
}

struct Helmert {
    double eastings = 0.0;
    double northings = 0.0;
    double height = 0.0;
    double xaa = 1.0;
    double xao = 0.0;
    double scale = 1.0;
    double factor_x = 1.0;
    double factor_y = 1.0;
    double factor_z = 1.0;
};

std::optional<double> property_value(express::Base pset, const char* name) {
    for (auto property : ifcapi::detail::read_ref_aggregate(pset, "HasProperties")) {
        if (!is_a(property, "IfcPropertySingleValue")
            || ifcapi::detail::read_string_attr(property, "Name") != name) {
            continue;
        }
        auto nominal = ifcapi::detail::read_ref_attr(property, "NominalValue");
        if (nominal) {
            try {
            return numeric_value(nominal.get_attribute_value(0));
            } catch (...) {
            }
        }
    }
    return std::nullopt;
}

std::optional<Helmert> helmert(ifcopenshell::file* file) {
    if (!file) {
        return std::nullopt;
    }
    Helmert result;
    if (file->schema()->name() == "IFC2X3") {
        auto project = ifcapi::detail::first_instance_by_type(file, "IfcProject");
        auto conversion = ifcapi::detail::named_property_set(project, "ePSet_MapConversion");
        if (!conversion) {
            return std::nullopt;
        }
        result.eastings = property_value(conversion, "Eastings").value_or(0.0);
        result.northings = property_value(conversion, "Northings").value_or(0.0);
        result.height = property_value(conversion, "OrthogonalHeight").value_or(0.0);
        result.xaa = property_value(conversion, "XAxisAbscissa").value_or(1.0);
        result.xao = property_value(conversion, "XAxisOrdinate").value_or(0.0);
        result.scale = property_value(conversion, "Scale").value_or(1.0);
        return result;
    }

    auto operations = ifcapi::detail::instances_by_type(file, "IfcCoordinateOperation");
    if (operations.empty()) {
        return std::nullopt;
    }
    auto conversion = operations.front();
    if (is_a(conversion, "IfcMapConversion")) {
        result.eastings = optional_double(conversion, "Eastings").value_or(0.0);
        result.northings = optional_double(conversion, "Northings").value_or(0.0);
        result.height = optional_double(conversion, "OrthogonalHeight").value_or(0.0);
        result.xaa = optional_double(conversion, "XAxisAbscissa").value_or(1.0);
        result.xao = optional_double(conversion, "XAxisOrdinate").value_or(0.0);
        result.scale = optional_double(conversion, "Scale").value_or(1.0);
        if (is_a(conversion, "IfcMapConversionScaled")) {
            result.factor_x = optional_double(conversion, "FactorX").value_or(1.0);
            result.factor_y = optional_double(conversion, "FactorY").value_or(1.0);
            result.factor_z = optional_double(conversion, "FactorZ").value_or(1.0);
        }
        return result;
    }
    if (is_a(conversion, "IfcRigidOperation")) {
        result.eastings = optional_double(conversion, "FirstCoordinate").value_or(0.0);
        result.northings = optional_double(conversion, "SecondCoordinate").value_or(0.0);
        result.height = optional_double(conversion, "Height").value_or(0.0);
        return result;
    }
    return std::nullopt;
}

std::optional<std::vector<double>> world_coordinate_system(ifcopenshell::file* file) {
    express::Base selected;
    for (auto context : ifcapi::detail::instances_by_type(file, "IfcGeometricRepresentationContext")) {
        if (context.declaration().name() != "IfcGeometricRepresentationContext") {
            continue;
        }
        selected = context;
        if (ifcapi::detail::read_string_attr(context, "ContextType") == "Model") {
            break;
        }
    }
    auto wcs = ifcapi::detail::read_ref_attr(selected, "WorldCoordinateSystem");
    if (!wcs) {
        return std::nullopt;
    }
    std::vector<double> matrix(16);
    if (!ifcapi::compute_axis2placement(wcs, matrix.data())) {
        return std::nullopt;
    }
    return matrix;
}

void normalize_column(std::vector<double>& matrix, size_t column) {
    double length = 0.0;
    for (size_t row = 0; row < 3; ++row) {
        length += matrix[row * 4 + column] * matrix[row * 4 + column];
    }
    if (length == 0.0) {
        return;
    }
    length = std::sqrt(length);
    for (size_t row = 0; row < 3; ++row) {
        matrix[row * 4 + column] /= length;
    }
}

std::vector<double> local_to_global(ifcopenshell::file* file, std::vector<double> matrix) {
    auto parameters = helmert(file);
    if (!parameters) {
        return matrix;
    }
    if (auto wcs = world_coordinate_system(file)) {
        matrix = ifcapi::detail::matmul4(ifcapi::detail::invert_rigid4(*wcs), matrix);
    }
    const auto& p = *parameters;
    double theta = std::atan2(p.xao, p.xaa);
    std::vector<double> transform{
        p.scale * p.factor_x * std::cos(theta), -p.scale * p.factor_y * std::sin(theta), 0.0, p.eastings,
        p.scale * p.factor_x * std::sin(theta), p.scale * p.factor_y * std::cos(theta), 0.0, p.northings,
        0.0, 0.0, p.scale * p.factor_z, p.height,
        0.0, 0.0, 0.0, 1.0};
    auto result = ifcapi::detail::matmul4(transform, matrix);
    normalize_column(result, 0);
    normalize_column(result, 1);
    normalize_column(result, 2);
    return result;
}

std::vector<double> global_to_local(ifcopenshell::file* file, std::vector<double> matrix) {
    auto parameters = helmert(file);
    if (!parameters) {
        return matrix;
    }
    const auto& p = *parameters;
    matrix[3] -= p.eastings;
    matrix[7] -= p.northings;
    matrix[11] -= p.height;
    double theta = std::atan2(p.xao, p.xaa);
    std::vector<double> inverse_scale{
        1.0 / (p.scale * p.factor_x), 0.0, 0.0, 0.0,
        0.0, 1.0 / (p.scale * p.factor_y), 0.0, 0.0,
        0.0, 0.0, 1.0 / (p.scale * p.factor_z), 0.0,
        0.0, 0.0, 0.0, 1.0};
    std::vector<double> inverse_rotation{
        std::cos(theta), std::sin(theta), 0.0, 0.0,
        -std::sin(theta), std::cos(theta), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0};
    auto result = ifcapi::detail::matmul4(inverse_scale, ifcapi::detail::matmul4(inverse_rotation, matrix));
    normalize_column(result, 0);
    normalize_column(result, 1);
    normalize_column(result, 2);
    if (auto wcs = world_coordinate_system(file)) {
        result = ifcapi::detail::matmul4(*wcs, result);
    }
    return result;
}

class AppendAsset {
    ifcopenshell::file* file_;
    ifcopenshell::file* library_;
    express::Base source_asset_;
    ifcapi::bindings::ProjectAppendAssetCache* cache_;
    bool unique_names_;
    std::string target_class_;
    std::map<std::int64_t, express::Base> added_;
    std::map<std::int64_t, express::Base> copied_;
    std::map<std::string, std::vector<std::string>> inverse_attributes_;
    std::set<express::Base> existing_contexts_;
    double conversion_factor_ = 0.0;
    bool has_conversion_factor_ = false;

    std::int64_t identity(express::Base value) const {
        return static_cast<std::int64_t>(value.identity());
    }

    bool unique_enabled() const { return unique_names_; }

    express::Base cache_get(express::Base source) {
        if (!cache_ || !unique_enabled()) {
            return {};
        }
        auto it = cache_->values.find(identity(source));
        return it == cache_->values.end() ? express::Base() : it->second.target;
    }

    void cache_put(express::Base source, express::Base target) {
        if (!cache_ || !unique_enabled() || !target) {
            return;
        }
        cache_->target_file = file_;
        cache_->values[identity(source)] = {
            target,
            target.id(),
            source.id(),
            source.declaration().name(),
        };
    }

    express::Base find_by_name(express::Base source, const char* attr, bool require_name) {
        auto name = ifcapi::detail::read_optional_string_attr(source, attr);
        if (require_name && !name.has_value) {
            return {};
        }
        for (auto candidate : file_->instances_by_type(source.declaration().name())) {
            if (!is_a(candidate, source.declaration().name().c_str())) {
                continue;
            }
            auto candidate_name = ifcapi::detail::read_optional_string_attr(candidate, attr);
            if (candidate_name.has_value == name.has_value
                && (!name.has_value || candidate_name.value == name.value)) {
                return candidate;
            }
        }
        return {};
    }

    express::Base find_equivalent_material_set(express::Base source, const char* name_attr) {
        auto name = ifcapi::detail::read_optional_string_attr(source, name_attr);
        if (!name.has_value) return {};
        for (auto candidate : file_->instances_by_type(source.declaration().name())) {
            auto candidate_name = ifcapi::detail::read_optional_string_attr(candidate, name_attr);
            if (candidate_name.has_value && candidate_name.value == name.value
                && material_sets_equal(source, candidate)) {
                return candidate;
            }
        }
        return {};
    }

    express::Base find_existing_nested(express::Base source) {
        if (!unique_enabled() || !source) {
            return {};
        }
        auto mapped = cache_get(source);
        if (mapped) {
            return mapped;
        }
        const auto class_name = source.declaration().name();
        if (class_name == "IfcOrganization" || class_name == "IfcPerson") {
            const char* attr = file_->schema()->name() == "IFC2X3" ? "Id" : "Identification";
            auto result = find_by_name(source, attr, true);
            if (result) cache_put(source, result);
            return result;
        }
        if (is_a(source, "IfcProfileDef")) {
            auto result = find_by_name(source, "ProfileName", true);
            if (result) cache_put(source, result);
            return result;
        }
        if (is_a(source, "IfcMaterial")) {
            auto result = find_by_name(source, "Name", false);
            if (result) cache_put(source, result);
            return result;
        }
        if (class_name == "IfcPersonAndOrganization") {
            auto person = find_existing_nested(ifcapi::detail::read_ref_attr(source, "ThePerson"));
            auto organization = find_existing_nested(ifcapi::detail::read_ref_attr(source, "TheOrganization"));
            if (person && organization) {
                for (auto candidate : ifcapi::detail::instances_by_type(file_, "IfcPersonAndOrganization")) {
                    if (same_instance(ifcapi::detail::read_ref_attr(candidate, "ThePerson"), person)
                        && same_instance(ifcapi::detail::read_ref_attr(candidate, "TheOrganization"), organization)) {
                        cache_put(source, candidate);
                        return candidate;
                    }
                }
            }
            return {};
        }
        if (is_a(source, "IfcMaterialLayerSet")) {
            auto result = find_equivalent_material_set(source, "LayerSetName");
            if (result) cache_put(source, result);
            return result;
        }
        if (is_a(source, "IfcMaterialConstituentSet") || is_a(source, "IfcMaterialProfileSet")) {
            auto result = find_equivalent_material_set(source, "Name");
            if (result) cache_put(source, result);
            return result;
        }
        if (is_a(source, "IfcPresentationStyle")) {
            auto result = find_by_name(source, "Name", true);
            if (result) cache_put(source, result);
            return result;
        }
        if (class_name == "IfcApplication") {
            auto result = find_by_name(source, "ApplicationIdentifier", true);
            if (result) cache_put(source, result);
            return result;
        }
        return {};
    }

    bool material_sets_equal(express::Base source, express::Base target) {
        if (!source || !target || source.declaration().name() != target.declaration().name()) return false;
        if (is_a(source, "IfcMaterialLayerSet")) {
            auto a = ifcapi::detail::read_ref_aggregate(source, "MaterialLayers");
            auto b = ifcapi::detail::read_ref_aggregate(target, "MaterialLayers");
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) {
                auto material_a = ifcapi::detail::read_ref_attr(a[i], "Material");
                auto material_b = ifcapi::detail::read_ref_attr(b[i], "Material");
                if (static_cast<bool>(material_a) != static_cast<bool>(material_b)) return false;
                auto name_a = ifcapi::detail::read_optional_string_attr(material_a, "Name");
                auto name_b = ifcapi::detail::read_optional_string_attr(material_b, "Name");
                if ((material_a && name_a.has_value != name_b.has_value)
                    || (material_a && name_a.has_value && name_a.value != name_b.value)
                    || optional_double(a[i], "LayerThickness") != optional_double(b[i], "LayerThickness")) return false;
            }
            return true;
        }
        const char* members = is_a(source, "IfcMaterialConstituentSet") ? "MaterialConstituents" : "MaterialProfiles";
        auto a = ifcapi::detail::read_ref_aggregate(source, members);
        auto b = ifcapi::detail::read_ref_aggregate(target, members);
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            const char* name = is_a(source, "IfcMaterialConstituentSet") ? "Name" : "Profile";
            auto material_a = ifcapi::detail::read_ref_attr(a[i], "Material");
            auto material_b = ifcapi::detail::read_ref_attr(b[i], "Material");
            auto material_name_a = ifcapi::detail::read_optional_string_attr(material_a, "Name");
            auto material_name_b = ifcapi::detail::read_optional_string_attr(material_b, "Name");
            if (material_name_a.has_value != material_name_b.has_value
                || (material_name_a.has_value && material_name_a.value != material_name_b.value)) return false;
            auto name_a = is_a(source, "IfcMaterialConstituentSet")
                ? ifcapi::detail::read_optional_string_attr(a[i], name)
                : ifcapi::detail::read_optional_string_attr(ifcapi::detail::read_ref_attr(a[i], name), "ProfileName");
            auto name_b = is_a(source, "IfcMaterialConstituentSet")
                ? ifcapi::detail::read_optional_string_attr(b[i], name)
                : ifcapi::detail::read_optional_string_attr(ifcapi::detail::read_ref_attr(b[i], name), "ProfileName");
            if (name_a.has_value != name_b.has_value || (name_a.has_value && name_a.value != name_b.value)) return false;
        }
        return true;
    }

    express::Base find_existing_element(express::Base source) {
        if (!source) return {};
        auto it = added_.find(identity(source));
        if (it != added_.end()) return it->second;
        if (is_a(source, "IfcRoot")) {
            if (auto result = by_guid(file_, source)) return result;
        }
        if (!unique_enabled()) return {};
        if (is_a(source, "IfcMaterial")) return find_by_name(source, "Name", false);
        if (is_a(source, "IfcMaterialLayerSet")) {
            return find_equivalent_material_set(source, "LayerSetName");
        }
        if (is_a(source, "IfcMaterialConstituentSet") || is_a(source, "IfcMaterialProfileSet")) {
            return find_equivalent_material_set(source, "Name");
        }
        if (is_a(source, "IfcProfileDef")) return find_by_name(source, "ProfileName", true);
        if (is_a(source, "IfcPresentationStyle")) return find_by_name(source, "Name", true);
        if (source.declaration().name() == "IfcOrganization" || source.declaration().name() == "IfcPerson") {
            return find_existing_nested(source);
        }
        return {};
    }

    double conversion_factor() {
        if (!has_conversion_factor_) {
            conversion_factor_ = ifcapi::bindings::unit_calculate_unit_scale(library_, "LENGTHUNIT")
                / ifcapi::bindings::unit_calculate_unit_scale(file_, "LENGTHUNIT");
            has_conversion_factor_ = true;
        }
        return conversion_factor_;
    }

    express::Base file_add(express::Base source, std::optional<double> factor = std::nullopt) {
        if (!source) return {};
        if (source.file() == file_) return source;
        if (!unique_enabled() || source.id() == 0) {
            if (is_length_value(source)) {
                auto value = static_cast<double>(source.get_attribute_value(0));
                return ifcapi::detail::create_typed_double(
                    file_, source.declaration().name().c_str(), value * factor.value_or(conversion_factor()));
            }
            return file_->add_entity(source);
        }
        auto source_identity = identity(source);
        if (auto mapped = copied_.find(source_identity); mapped != copied_.end()) return mapped->second;
        if (auto mapped = cache_get(source)) return mapped;
        if (auto existing = find_existing_nested(source)) {
            copied_[source_identity] = existing;
            return existing;
        }

        auto* declaration = source.declaration().as_entity();
        if (!declaration) return file_->add_entity(source);
        auto result = file_->create(declaration);
        copied_[source_identity] = result;
        cache_put(source, result);
        auto attributes = declaration->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {
            auto value = source.get_attribute_value(i);
            if (value.isNull()) continue;
            bool length = is_length_attribute(declaration, i);
            switch (value.type()) {
            case ifcopenshell::Argument_ENTITY_INSTANCE:
                result.set_attribute_value(i, file_add(static_cast<express::Base>(value), factor));
                break;
            case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
                std::vector<express::Base> refs;
                for (auto ref : static_cast<std::vector<express::Base>>(value)) refs.push_back(file_add(ref, factor));
                result.set_attribute_value(i, refs);
                break;
            }
            case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
                std::vector<std::vector<express::Base>> refs;
                for (auto row : static_cast<std::vector<std::vector<express::Base>>>(value)) {
                    std::vector<express::Base> copied;
                    for (auto ref : row) copied.push_back(file_add(ref, factor));
                    refs.push_back(copied);
                }
                result.set_attribute_value(i, refs);
                break;
            }
            case ifcopenshell::Argument_DOUBLE: {
                auto number = static_cast<double>(value);
                result.set_attribute_value(i, length ? number * factor.value_or(conversion_factor()) : number);
                break;
            }
            case ifcopenshell::Argument_AGGREGATE_OF_DOUBLE: {
                auto numbers = static_cast<std::vector<double>>(value);
                if (length) for (auto& number : numbers) number *= factor.value_or(conversion_factor());
                result.set_attribute_value(i, numbers);
                break;
            }
            case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE: {
                auto numbers = static_cast<std::vector<std::vector<double>>>(value);
                if (length) for (auto& row : numbers) for (auto& number : row) number *= factor.value_or(conversion_factor());
                result.set_attribute_value(i, numbers);
                break;
            }
            default:
                ifcapi::detail::copy_attribute_value(result, i, value);
                break;
            }
        }
        return result;
    }

    bool another_asset(express::Base value) {
        if (!value || same_instance(value, source_asset_)) return false;
        if (is_a(value, "IfcRoot") && by_guid(file_, value)) return false;
        if (is_a(value, "IfcDistributionPort")) return false;
        if (value.declaration().is(target_class_)) return true;
        if (target_class_ == "IfcProduct" && is_a(value, "IfcTypeProduct")) return true;
        if (target_class_ == "IfcTypeProduct" && is_a(value, "IfcProduct")) return true;
        return false;
    }

    bool whitelisted(express::Base value, const std::string& encoded) {
        auto split = encoded.find('.');
        auto attr = split == std::string::npos ? encoded : encoded.substr(0, split);
        auto inverse_class = split == std::string::npos ? std::string() : encoded.substr(split + 1);
        auto inverses = ifcapi::detail::read_inverse_aggregate(value, attr.c_str());
        for (auto inverse : inverses) {
            if (inverse_class.empty() || inverse.declaration().is(inverse_class)) return true;
        }
        return false;
    }

    bool has_whitelisted_inverses(express::Base value) {
        for (const auto& [class_name, attributes] : inverse_attributes_) {
            if (!value.declaration().is(class_name)) continue;
            for (const auto& attribute : attributes) {
                if (whitelisted(value, attribute)) return true;
            }
        }
        return false;
    }

    void add_inverse(express::Base source_inverse) {
        auto source_identity = identity(source_inverse);
        if (auto it = added_.find(source_identity); it != added_.end()) return;
        auto mapped = cache_get(source_inverse);
        if (mapped && !is_a(source_inverse, "IfcRelationship")) {
            added_[source_identity] = mapped;
            return;
        }
        auto existing = mapped ? mapped : by_guid(file_, source_inverse);
        auto result = existing;
        if (!result) result = file_->create(source_inverse.declaration().as_entity());
        added_[source_identity] = result;
        cache_put(source_inverse, result);
        auto attributes = source_inverse.declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {
            auto value = source_inverse.get_attribute_value(i);
            if (value.isNull()) continue;
            if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE) {
                auto ref = static_cast<express::Base>(value);
                if (another_asset(ref) && !is_a(source_inverse, "IfcRelVoidsElement") && !is_a(source_inverse, "IfcRelProjectsElement")) continue;
                result.set_attribute_value(i, add_element(ref));
            } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                auto refs = static_cast<std::vector<express::Base>>(value);
                std::vector<express::Base> copied;
                bool layer_items = is_a(source_inverse, "IfcPresentationLayerAssignment") && attributes[i]->name() == "AssignedItems";
                for (auto ref : refs) {
                    if (another_asset(ref)) continue;
                    express::Base copied_ref;
                    if (layer_items) {
                        auto it = added_.find(identity(ref));
                        copied_ref = it == added_.end() ? cache_get(ref) : it->second;
                    } else {
                        copied_ref = add_element(ref);
                    }
                    if (copied_ref) append_unique(copied, copied_ref);
                }
                if (existing) {
                    auto previous = ifcapi::detail::read_ref_aggregate(result, attributes[i]->name().c_str());
                    append_unique(previous, copied);
                    result.set_attribute_value(i, previous);
                } else {
                    result.set_attribute_value(i, copied);
                }
            } else {
                ifcapi::detail::copy_attribute_value(result, i, value);
            }
        }
    }

    void check_inverses(express::Base source) {
        for (const auto& [class_name, attributes] : inverse_attributes_) {
            if (!source.declaration().is(class_name)) continue;
            for (const auto& attribute : attributes) {
                for (auto inverse : ifcapi::detail::read_inverse_aggregate(source, attribute.substr(0, attribute.find('.')).c_str())) {
                    auto dot = attribute.find('.');
                    if (dot != std::string::npos && !inverse.declaration().is(attribute.substr(dot + 1))) continue;
                    add_inverse(inverse);
                }
            }
        }
    }

    express::Base add_element(express::Base source) {
        if (!source || source.id() == 0) return {};
        if (auto existing = find_existing_element(source)) return existing;
        auto result = file_add(source);
        added_[identity(source)] = result;
        check_inverses(source);
        std::deque<express::Base> queue;
        auto traversed = library_->traverse(source, 1);
        for (size_t i = 1; i < traversed.size(); ++i) queue.push_back(traversed[i]);
        while (!queue.empty()) {
            auto subelement = queue.front();
            queue.pop_front();
            if (auto existing = find_existing_element(subelement)) {
                added_[identity(subelement)] = existing;
                if (!has_whitelisted_inverses(existing)) check_inverses(subelement);
                continue;
            }
            auto copied = file_add(subelement);
            added_[identity(subelement)] = copied;
            check_inverses(subelement);
            auto children = library_->traverse(subelement, 1);
            for (size_t i = 1; i < children.size(); ++i) queue.push_back(children[i]);
        }
        return result;
    }

    bool equivalent_context(express::Base left, express::Base right) {
        if (!left || !right || left.declaration().name() != right.declaration().name()) return false;
        auto same_optional = [](const auto& left_value, const auto& right_value) {
            return left_value.has_value == right_value.has_value
                && (!left_value.has_value || left_value.value == right_value.value);
        };
        if (!same_optional(
                ifcapi::detail::read_optional_string_attr(left, "ContextType"),
                ifcapi::detail::read_optional_string_attr(right, "ContextType"))
            || !same_optional(
                ifcapi::detail::read_optional_string_attr(left, "ContextIdentifier"),
                ifcapi::detail::read_optional_string_attr(right, "ContextIdentifier"))) return false;
        return !is_a(left, "IfcGeometricRepresentationSubContext")
            || same_optional(
                ifcapi::detail::read_optional_string_attr(left, "TargetView"),
                ifcapi::detail::read_optional_string_attr(right, "TargetView"));
    }

    express::Base existing_context(express::Base source) {
        for (auto context : existing_contexts_) if (equivalent_context(context, source)) return context;
        return {};
    }

    void capture_existing_contexts() {
        existing_contexts_.clear();
        auto contexts = ifcapi::detail::instances_by_type(file_, "IfcGeometricRepresentationContext");
        existing_contexts_.insert(contexts.begin(), contexts.end());
    }

    express::Base create_context(express::Base source) {
        auto parent = ifcapi::detail::read_ref_attr(source, "ParentContext");
        express::Base target_parent;
        if (parent) {
            target_parent = existing_context(parent);
            if (!target_parent) {
                target_parent = create_context(parent);
                existing_contexts_.insert(target_parent);
            }
        }
        auto result = ifcapi::bindings::context_add_context(file_, {
            ifcapi::detail::read_string_attr(source, "ContextType"),
            ifcapi::detail::read_string_attr(source, "ContextIdentifier"),
            ifcapi::detail::read_string_attr(source, "TargetView"),
            std::nullopt,
            target_parent ? std::optional<express::Base>(target_parent) : std::nullopt});
        existing_contexts_.insert(result);
        return result;
    }

    void reuse_contexts() {
        std::set<express::Base> contexts;
        for (const auto& [source_identity, target] : added_) {
            (void)source_identity;
            if (is_a(target, "IfcGeometricRepresentationContext") && existing_contexts_.find(target) == existing_contexts_.end()) {
                contexts.insert(target);
            }
        }
        std::vector<express::Base> ordered;
        for (auto context : contexts) if (context.declaration().name() == "IfcGeometricRepresentationContext") ordered.push_back(context);
        for (auto context : contexts) if (context.declaration().name() == "IfcGeometricRepresentationSubContext") ordered.push_back(context);
        for (auto context : ordered) {
            auto replacement = existing_context(context);
            if (!replacement) replacement = create_context(context);
            replace_reference(file_, context, replacement);
        }
        for (auto context : contexts) {
            auto copy = context;
            if (copy && copy.file() == file_) ifcapi::bindings::entity_remove_deep(&copy);
        }
        prune_cache(cache_, file_);
    }

    express::Base append_material() {
        inverse_attributes_ = {{"IfcMaterial", {"HasExternalReferences", "HasProperties", "HasRepresentation"}}};
        capture_existing_contexts();
        auto result = add_element(source_asset_);
        if (is_a(result, "IfcMaterial") && !ifcapi::detail::read_inverse_aggregate(result, "HasRepresentation").empty()) reuse_contexts();
        return result;
    }

    express::Base append_cost_schedule() {
        inverse_attributes_ = {{"IfcCostSchedule", {"Controls"}}, {"IfcCostItem", {"IsNestedBy"}}};
        return add_element(source_asset_);
    }

    express::Base append_profile() {
        inverse_attributes_ = {{"IfcProfileDef", {"HasProperties"}}};
        return add_element(source_asset_);
    }

    express::Base append_style() {
        inverse_attributes_.clear();
        return add_element(source_asset_);
    }

    express::Base append_type() {
        inverse_attributes_ = {
            {"IfcObjectDefinition", {"HasAssociations"}},
            {"IfcDistributionElementType", {"IsNestedBy"}},
            {file_->schema()->name() == "IFC2X3" ? "IfcMaterial" : "IfcMaterialDefinition", {"HasExternalReferences", "HasProperties", "HasRepresentation"}},
            {"IfcRepresentationItem", {"StyledByItem", "LayerAssignment"}},
            {"IfcRepresentation", {"LayerAssignments"}},
            {"IfcProductDefinitionShape", {"HasShapeAspects"}},
            {"IfcRepresentationMap", {"HasShapeAspects"}}};
        capture_existing_contexts();
        auto result = add_element(source_asset_);
        reuse_contexts();
        return result;
    }

    express::Base append_product() {
        inverse_attributes_ = {
            {"IfcObjectDefinition", {"HasAssociations"}},
            {"IfcObject", {"IsDefinedBy.IfcRelDefinesByProperties"}},
            {"IfcElement", {"HasOpenings"}},
            {"IfcDistributionElement", {"IsNestedBy"}},
            {file_->schema()->name() == "IFC2X3" ? "IfcMaterial" : "IfcMaterialDefinition", {"HasExternalReferences", "HasProperties", "HasRepresentation"}},
            {"IfcRepresentationItem", {"StyledByItem", file_->schema()->name() == "IFC2X3" ? "LayerAssignments" : "LayerAssignment"}},
            {"IfcRepresentation", {"LayerAssignments"}},
            {"IfcProductDefinitionShape", {"HasShapeAspects"}},
            {"IfcRepresentationMap", {"HasShapeAspects"}}};
        capture_existing_contexts();
        auto result = add_element(source_asset_);
        reuse_contexts();
        auto source_placement = ifcapi::detail::read_ref_attr(source_asset_, "ObjectPlacement");
        if (source_placement && result) {
            auto matrix = ifcapi::bindings::placement_get_local_placement(source_placement);
            matrix = local_to_global(library_, matrix);
            matrix = global_to_local(file_, matrix);
            ifcapi::bindings::geometry_edit_object_placement(file_, {result, matrix, false, false});
            prune_cache(cache_, file_);
        }
        auto source_type = ifcapi::bindings::element_get_type(&source_asset_);
        if (source_type) {
            auto old_source = source_asset_;
            source_asset_ = *source_type;
            auto copied_type = append_type();
            source_asset_ = old_source;
            auto user = ifcapi::detail::first_instance_by_type(file_, "IfcPersonAndOrganization");
            auto application = ifcapi::detail::first_instance_by_type(file_, "IfcApplication");
            if (copied_type) {
                ifcapi::bindings::type_assign_type(file_, {std::vector<express::Base>{result}, copied_type, false, std::nullopt, user, application});
            }
        }
        return result;
    }

  public:
    AppendAsset(ifcopenshell::file* file, const ifcapi::bindings::ProjectAppendAssetOptions& options)
        : file_(file),
          library_(options.library),
          source_asset_(options.element),
          cache_(options.cache.value_or(nullptr)),
          unique_names_(options.assume_asset_uniqueness_by_name.value_or(true)) {
        if (!file_ || !library_ || !source_asset_) throw std::runtime_error("project_append_asset requires target, library, and element");
        validate_cache(cache_, file_);
        if (is_a(source_asset_, "IfcTypeProduct")) target_class_ = "IfcTypeProduct";
        else if (is_a(source_asset_, "IfcProduct")) target_class_ = "IfcProduct";
        else if (is_a(source_asset_, "IfcMaterial")) target_class_ = "IfcMaterial";
        else if (is_a(source_asset_, "IfcCostSchedule")) target_class_ = "IfcCostSchedule";
        else if (is_a(source_asset_, "IfcProfileDef")) target_class_ = "IfcProfileDef";
        else if (is_a(source_asset_, "IfcPresentationStyle")) target_class_ = "IfcPresentationStyle";
    }

    express::Base execute() {
        if (target_class_.empty()) return {};
        if (target_class_ == "IfcTypeProduct") return append_type();
        if (target_class_ == "IfcProduct") return append_product();
        if (target_class_ == "IfcMaterial") return append_material();
        if (target_class_ == "IfcCostSchedule") return append_cost_schedule();
        if (target_class_ == "IfcProfileDef") return append_profile();
        return append_style();
    }
};

} // namespace

namespace ifcapi {
namespace bindings {

ProjectAppendAssetCache* project_append_asset_cache_new() {
    return new ProjectAppendAssetCache();
}

void project_append_asset_cache_free(ProjectAppendAssetCache* cache) {
    delete cache;
}

void project_append_asset_cache_set(
    ProjectAppendAssetCache* cache,
    express::Base source,
    express::Base target)
{
    if (!cache || !source || !target) return;
    auto* file = target.file();
    if (cache->target_file && cache->target_file != file && !cache->values.empty()) {
        throw std::runtime_error("Append-asset cache belongs to a different target file");
    }
    cache->target_file = file;
    cache->values[static_cast<std::int64_t>(source.identity())] = {
        target,
        target.id(),
        source.id(),
        source.declaration().name(),
    };
}

ProjectAppendAssetCacheEntries project_append_asset_cache_entries(
    ProjectAppendAssetCache* cache)
{
    if (!cache) return {};
    prune_cache(cache, cache->target_file);
    ProjectAppendAssetCacheEntries result;
    result.source_identities.reserve(cache->values.size());
    result.source_ids.reserve(cache->values.size());
    result.source_types.reserve(cache->values.size());
    result.targets.reserve(cache->values.size());
    for (const auto& [source_identity, entry] : cache->values) {
        result.source_identities.push_back(source_identity);
        result.source_ids.push_back(entry.source_id);
        result.source_types.push_back(entry.source_type);
        result.targets.push_back(entry.target);
    }
    return result;
}

std::optional<express::Base> project_append_asset(
    ifcopenshell::file* file,
    const ifcapi::bindings::ProjectAppendAssetOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto result = AppendAsset(file, options).execute();
        return result ? std::optional<express::Base>(result) : std::nullopt;
    } catch (const std::exception& error) {
        ifcapi::detail::set_error(error);
        return std::nullopt;
    } catch (...) {
        set_error("Unknown exception in project_append_asset");
        return std::nullopt;
    }
}

} // namespace bindings
} // namespace ifcapi
