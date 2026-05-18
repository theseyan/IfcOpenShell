// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/cost.h"

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"

#include <stdexcept>

namespace {

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const std::string& ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

double numeric_value(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
        case ifcapi_pset::Kind::DOUBLE:
        case ifcapi_pset::Kind::TYPED_DOUBLE:
            return entry.d_val;
        case ifcapi_pset::Kind::INT:
        case ifcapi_pset::Kind::TYPED_INT:
            return static_cast<double>(entry.i_val);
        case ifcapi_pset::Kind::BOOL:
        case ifcapi_pset::Kind::TYPED_BOOL:
            return entry.b_val ? 1.0 : 0.0;
        default:
            throw std::runtime_error("Expected a numeric cost value");
    }
}

IfcUtil::IfcBaseClass* create_measure(IfcParse::IfcFile* file, const std::string& ifc_type, double value) {
    auto* result = ifcapi::detail::create_typed_double(file, ifc_type.c_str(), value);
    if (!result) {
        throw std::runtime_error("Unable to create " + ifc_type);
    }
    return result;
}

} // namespace

namespace ifcapi {
namespace bindings {

void cost_edit_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_value,
    ifcopenshell_pset_props_t* attributes,
    bool has_unit_basis,
    bool unit_basis_is_null,
    double value_component,
    IfcUtil::IfcBaseClass* unit_component)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !cost_value) {
            throw std::runtime_error("cost_edit_cost_value requires a file and cost value");
        }
        if (attributes) {
            for (const auto& entry : attributes->entries) {
                if (entry.key == "AppliedValue" && entry.kind != ifcapi_pset::Kind::NONE) {
                    ifcapi::detail::write_ref_attr(
                        cost_value,
                        "AppliedValue",
                        create_measure(file, "IfcMonetaryMeasure", numeric_value(entry)));
                } else {
                    ifcapi::detail::apply_attribute_prop(cost_value, entry);
                }
            }
        }
        if (has_unit_basis) {
            auto* old_unit_basis = ifcapi::detail::read_ref_attr(cost_value, "UnitBasis");
            IfcUtil::IfcBaseClass* new_unit_basis = nullptr;
            if (!unit_basis_is_null) {
                if (!unit_component) {
                    throw std::runtime_error("UnitBasis requires a UnitComponent");
                }
                auto unit_type = ifcapi::detail::read_string_attr(unit_component, "UnitType");
                auto measure_class = unit_get_measure_class(unit_type);
                auto* measure = create_measure(file, measure_class, value_component);
                new_unit_basis = create_entity(file, "IfcMeasureWithUnit");
                ifcapi::detail::write_ref_attr(new_unit_basis, "ValueComponent", measure);
                ifcapi::detail::write_ref_attr(new_unit_basis, "UnitComponent", unit_component);
            }
            if (old_unit_basis) {
                entity_remove_deep2(old_unit_basis);
            }
            ifcapi::detail::write_ref_attr(cost_value, "UnitBasis", new_unit_basis);
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
