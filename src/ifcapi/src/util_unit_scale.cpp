// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "entity_introspection.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <string>
#include <unordered_map>

namespace {

const std::unordered_map<std::string, double>& prefix_table() {
    static const std::unordered_map<std::string, double> t = {
        {"EXA", 1e18}, {"PETA", 1e15}, {"TERA", 1e12}, {"GIGA", 1e9},
        {"MEGA", 1e6}, {"KILO", 1e3}, {"HECTO", 1e2}, {"DECA", 1e1},
        {"DECI", 1e-1}, {"CENTI", 1e-2}, {"MILLI", 1e-3}, {"MICRO", 1e-6},
        {"NANO", 1e-9}, {"PICO", 1e-12}, {"FEMTO", 1e-15}, {"ATTO", 1e-18},
    };
    return t;
}

}  // namespace

extern "C" {

IFCAPI_EXPORT double ifcopenshell_util_unit_calculate_unit_scale(
    ifcopenshell_ifc_file_t* file_h, const char* unit_type)
{
    if (!file_h || !file_h->ptr) return 1.0;
    auto* file = file_h->ptr;
    std::string want = (unit_type && *unit_type) ? unit_type : "LENGTHUNIT";
    try {
        const auto* proj_decl = file->schema()->declaration_by_name("IfcProject");
        auto projects = file->instances_by_type(proj_decl);
        if (!projects || projects->size() == 0) return 1.0;
        auto* uia = ifcapi::get_entity_ref((*projects)[0], "UnitsInContext");
        if (!uia) return 1.0;

        double unit_scale = 1.0;
        for (auto* unit : ifcapi::get_entity_list(uia, "Units")) {
            if (ifcapi::get_string_attr(unit, "UnitType") != want) continue;

            // Unwrap IfcConversionBasedUnit chain into its underlying SI unit.
            while (unit && unit->declaration().is("IfcConversionBasedUnit")) {
                auto* cf = ifcapi::get_entity_ref(unit, "ConversionFactor");
                if (!cf) break;
                if (auto* vc = ifcapi::get_entity_ref(cf, "ValueComponent")) {
                    try {
                        auto vv = vc->get_attribute_value(0);
                        if (!vv.isNull()) unit_scale *= (double)vv;
                    } catch (...) {}
                }
                unit = ifcapi::get_entity_ref(cf, "UnitComponent");
            }
            if (unit && unit->declaration().is("IfcSIUnit")) {
                std::string prefix = ifcapi::get_string_attr(unit, "Prefix");
                if (!prefix.empty()) {
                    const auto& tbl = prefix_table();
                    auto pit = tbl.find(prefix);
                    if (pit != tbl.end()) unit_scale *= pit->second;
                }
            }
            return unit_scale;
        }
        return 1.0;
    } catch (...) {
        return 1.0;
    }
}

}  // extern "C"
