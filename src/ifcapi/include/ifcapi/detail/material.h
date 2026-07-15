// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_MATERIAL_H
#define IFCAPI_DETAIL_MATERIAL_H

#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace ifcapi {
namespace detail {

inline express::Base copy_material_definition(ifcopenshell::file* file, express::Base material) {
    if (!file || !material) return {};
    const auto is = [&](const char* ifc_class) { return material.declaration().is(ifc_class); };
    if (!(is("IfcMaterial") || is("IfcMaterialConstituentSet") || is("IfcMaterialConstituent")
          || is("IfcMaterialLayerSet") || is("IfcMaterialLayer") || is("IfcMaterialProfileSet")
          || is("IfcMaterialProfile") || is("IfcMaterialList"))) {
        throw std::runtime_error("Unexpected material type: '" + material.declaration().name() + "'.");
    }

    auto result = shallow_copy(file, material);
    const auto inverses = file->instances_by_reference(static_cast<int>(material.id()));
    for (auto inverse : inverses) {
        if (inverse.declaration().is("IfcMaterialProperties")) {
            auto copied_inverse = shallow_copy(file, inverse);
            write_ref_attr(copied_inverse, "Material", result);
            const char* properties_attribute = "Properties";
            if (file->schema()->name() == "IFC2X3") {
                if (!inverse.declaration().is("IfcExtendedMaterialProperties")) continue;
                properties_attribute = "ExtendedProperties";
            }
            std::vector<express::Base> copied_properties;
            for (auto property : read_ref_aggregate(inverse, properties_attribute)) {
                copied_properties.push_back(deep_copy(file, property));
            }
            if (!copied_properties.empty()) {
                write_ref_aggregate(copied_inverse, properties_attribute, copied_properties);
            }
        } else if (inverse.declaration().is("IfcMaterialDefinitionRepresentation")) {
            auto copied_inverse = deep_copy(
                file,
                inverse,
                {"IfcRepresentationContext", "IfcMaterial", "IfcPresentationStyle"});
            write_ref_attr(copied_inverse, "RepresentedMaterial", result);
        }
    }

    const char* members = nullptr;
    if (is("IfcMaterialConstituentSet")) members = "MaterialConstituents";
    else if (is("IfcMaterialLayerSet")) members = "MaterialLayers";
    else if (is("IfcMaterialProfileSet")) members = "MaterialProfiles";
    if (members) {
        std::vector<express::Base> copied_members;
        for (auto member : read_ref_aggregate(material, members)) {
            copied_members.push_back(copy_material_definition(file, member));
        }
        write_ref_aggregate(result, members, copied_members);
    }
    return result;
}

} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_MATERIAL_H */
