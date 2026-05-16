// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/boundary.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/relationship.h"
#include "ifcopenshell_api_internal.hpp"

#include <string>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* boundary_copy_boundary(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* boundary) {
    ifcopenshell_clear_error();
    try {
        auto* result = ifcapi::detail::shallow_copy(file, boundary);
        if (!result) return nullptr;
        if (auto* geometry = ifcapi::detail::read_ref_attr(result, "ConnectionGeometry")) {
            ifcapi::detail::write_ref_attr(result, "ConnectionGeometry", shape_builder_deep_copy(file, geometry));
        }
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void boundary_remove_boundary(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* boundary) {
    ifcopenshell_clear_error();
    try {
        auto* geometry = ifcapi::detail::read_ref_attr(boundary, "ConnectionGeometry");
        if (geometry) {
            ifcapi::detail::write_ref_attr(boundary, "ConnectionGeometry", nullptr);
            entity_remove_deep2(geometry);
        }
        ifcapi::detail::remove_with_history(file, boundary);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
