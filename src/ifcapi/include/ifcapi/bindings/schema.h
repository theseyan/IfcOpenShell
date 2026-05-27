// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.

#pragma once

#include "ifcapi/bindings/contract.h"

#include <string>

namespace IfcParse {
class IfcFile;
}

namespace IfcUtil {
class IfcBaseClass;
}

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base schema_reassign_class(
    IFCAPI_NULLABLE ifcopenshell::file* file,
    express::Base* element,
    const std::string& new_class);

} // namespace bindings
} // namespace ifcapi
