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
    std::optional<ifcopenshell::file*> file,
    std::optional<express::Base> instance,
    const std::string& query);

IFCAPI_BINDING ifcopenshell_selector_keys_t* selector_parse_keys(const std::string& query);
IFCAPI_BINDING size_t selector_keys_count(std::optional<ifcopenshell_selector_keys_t*> keys);
IFCAPI_BINDING std::string selector_keys_get(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);
IFCAPI_BINDING bool selector_keys_is_regex(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);
IFCAPI_BINDING void selector_keys_free(std::optional<ifcopenshell_selector_keys_t*> keys);

IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_filter(const std::string& query);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_get_element(const std::string& query);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_format(const std::string& query);
IFCAPI_BINDING int32_t selector_node_kind(std::optional<const ifcopenshell_selector_node_t*> node);
IFCAPI_BINDING size_t selector_node_child_count(std::optional<const ifcopenshell_selector_node_t*> node);
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_node_child(
    std::optional<const ifcopenshell_selector_node_t*> node,
    size_t index);
IFCAPI_BINDING std::string selector_node_text(std::optional<const ifcopenshell_selector_node_t*> node);
IFCAPI_BINDING void selector_node_free(std::optional<ifcopenshell_selector_node_t*> root);

} // namespace bindings
} // namespace ifcapi

#endif
