// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.

#pragma once

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base schema_reassign_class(
    std::optional<ifcopenshell::file*> file,
    express::Base* element,
    const std::string& new_class);

} // namespace bindings
} // namespace ifcapi
