// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/detail/alignment.h"
#include "ifcapi/detail/attribute.h"

#include "ifcparse/schema.h"

#include <cmath>
#include <stdexcept>

namespace ifcapi::detail::alignment {

void require_ifc4x3(ifcopenshell::file* file) {
    if (!file || !file->schema() || file->schema()->name().rfind("IFC4X3", 0) != 0) {
        throw std::runtime_error("Alignment operations require an IFC4X3 schema");
    }
}

void require_owned(ifcopenshell::file* file, express::Base entity, const char* name) {
    if (!file || !entity || entity.file() != file || !ifcapi::detail::exists_in_file(file, entity)) {
        throw std::runtime_error(std::string(name) + " must be a live entity in the target file");
    }
}

void require_type(express::Base entity, const char* type, const char* name) {
    if (!entity || !entity.declaration().is(type)) {
        throw std::runtime_error(std::string(name) + " must be " + type);
    }
}

express::Base create(ifcopenshell::file* file, const char* type) {
    auto* declaration = ifcapi::detail::declaration_by_name(file, type);
    if (!declaration) throw std::runtime_error(std::string(type) + " is unavailable in this schema");
    auto result = file->create(declaration);
    if (!result) throw std::runtime_error(std::string("Failed to create ") + type);
    return result;
}

express::Base point(ifcopenshell::file* file, const std::vector<double>& coordinates) {
    auto result = create(file, "IfcCartesianPoint");
    ifcapi::detail::write_double_aggregate(result, "Coordinates", coordinates);
    return result;
}

express::Base direction(ifcopenshell::file* file, const std::vector<double>& ratios) {
    auto result = create(file, "IfcDirection");
    ifcapi::detail::write_double_aggregate(result, "DirectionRatios", ratios);
    return result;
}

express::Base axis2d(ifcopenshell::file* file, express::Base location, double angle) {
    auto result = create(file, "IfcAxis2Placement2D");
    ifcapi::detail::write_ref_attr(result, "Location", location);
    ifcapi::detail::write_ref_attr(result, "RefDirection", direction(file, {std::cos(angle), std::sin(angle)}));
    return result;
}

express::Base axis3d(
    ifcopenshell::file* file,
    express::Base location,
    const std::vector<double>& axis,
    const std::vector<double>& ref_direction)
{
    auto result = create(file, "IfcAxis2Placement3D");
    ifcapi::detail::write_ref_attr(result, "Location", location);
    ifcapi::detail::write_ref_attr(result, "Axis", direction(file, axis));
    ifcapi::detail::write_ref_attr(result, "RefDirection", direction(file, ref_direction));
    return result;
}

express::Base length_measure(ifcopenshell::file* file, double value) {
    auto result = ifcapi::detail::create_typed_double(file, "IfcLengthMeasure", value);
    if (!result) throw std::runtime_error("Failed to create IfcLengthMeasure");
    return result;
}

double wrapped_double(express::Base value) {
    if (!value) return 0.0;
    auto attribute = value.get_attribute_value(0);
    return attribute.isNull() ? 0.0 : static_cast<double>(attribute);
}

std::string enum_value(express::Base entity, const char* attr) {
    return ifcapi::detail::read_string_attr(entity, attr);
}

std::vector<express::Base> inverses(express::Base entity, const char* attr) {
    return ifcapi::detail::read_inverse_aggregate(entity, attr);
}

std::vector<express::Base> refs(express::Base entity, const char* attr) {
    return ifcapi::detail::read_ref_aggregate(entity, attr);
}

express::Base first_inverse_relating(express::Base entity, const char* inverse, const char* relating_attr) {
    for (auto rel : inverses(entity, inverse)) {
        auto relating = ifcapi::detail::read_ref_attr(rel, relating_attr);
        if (relating) return relating;
    }
    return {};
}

express::Base find_nest_with_related_type(express::Base parent, const char* type) {
    for (auto rel : inverses(parent, "IsNestedBy")) {
        for (auto related : refs(rel, "RelatedObjects")) {
            if (related.declaration().is(type)) return rel;
        }
    }
    return {};
}

std::vector<express::Base> representations(express::Base product) {
    std::vector<express::Base> result;
    auto product_representation = ifcapi::detail::read_ref_attr(product, "Representation");
    if (!product_representation) return result;
    return refs(product_representation, "Representations");
}

} // namespace ifcapi::detail::alignment
