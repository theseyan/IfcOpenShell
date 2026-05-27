// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.

#ifndef IFCAPI_BINDINGS_SELECTOR_H
#define IFCAPI_BINDINGS_SELECTOR_H

#include "ifcapi/bindings/contract.h"
#include "ifcapi/selector/ast.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

struct ifcopenshell_selector_keys_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::optional<std::string> selector_format(
    IFCAPI_NULLABLE ifcopenshell::file* file,
    IFCAPI_NULLABLE express::Base* instance,
    const std::string& query);

IFCAPI_BINDING ifcopenshell_selector_keys_t* selector_parse_keys(const std::string& query);
IFCAPI_BINDING size_t selector_keys_count(IFCAPI_NULLABLE ifcopenshell_selector_keys_t* keys);
IFCAPI_BINDING std::string selector_keys_get(IFCAPI_NULLABLE ifcopenshell_selector_keys_t* keys, size_t index);
IFCAPI_BINDING bool selector_keys_is_regex(IFCAPI_NULLABLE ifcopenshell_selector_keys_t* keys, size_t index);
IFCAPI_BINDING void selector_keys_free(IFCAPI_NULLABLE ifcopenshell_selector_keys_t* keys);

IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_filter(const std::string& query);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_get_element(const std::string& query);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_format(const std::string& query);
IFCAPI_BINDING int32_t selector_node_kind(IFCAPI_NULLABLE const ifcopenshell_selector_node_t* node);
IFCAPI_BINDING size_t selector_node_child_count(IFCAPI_NULLABLE const ifcopenshell_selector_node_t* node);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_node_child(
    IFCAPI_NULLABLE const ifcopenshell_selector_node_t* node,
    size_t index);
IFCAPI_BINDING std::string selector_node_text(IFCAPI_NULLABLE const ifcopenshell_selector_node_t* node);
IFCAPI_BINDING void selector_node_free(IFCAPI_NULLABLE ifcopenshell_selector_node_t* root);

} // namespace bindings
} // namespace ifcapi

#endif
