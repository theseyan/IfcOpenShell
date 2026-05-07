// SPDX-License-Identifier: LGPL-3.0-or-later
// This file was generated with the assistance of an AI coding tool.

#ifndef IFCAPI_BINDINGS_SELECTOR_H
#define IFCAPI_BINDINGS_SELECTOR_H

#include "ifcapi/selector/ast.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

struct ifcopenshell_selector_keys_t;

namespace ifcapi {
namespace bindings {

std::optional<std::string> selector_format(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* instance,
    const std::string& query);

ifcopenshell_selector_keys_t* selector_parse_keys(const std::string& query);
size_t selector_keys_count(ifcopenshell_selector_keys_t* keys);
std::string selector_keys_get(ifcopenshell_selector_keys_t* keys, size_t index);
bool selector_keys_is_regex(ifcopenshell_selector_keys_t* keys, size_t index);
void selector_keys_free(ifcopenshell_selector_keys_t* keys);

ifcopenshell_selector_node_t* selector_parse_filter(const std::string& query);
ifcopenshell_selector_node_t* selector_parse_get_element(const std::string& query);
ifcopenshell_selector_node_t* selector_parse_format(const std::string& query);
int32_t selector_node_kind(const ifcopenshell_selector_node_t* node);
size_t selector_node_child_count(const ifcopenshell_selector_node_t* node);
ifcopenshell_selector_node_t* selector_node_child(const ifcopenshell_selector_node_t* node, size_t index);
std::string selector_node_text(const ifcopenshell_selector_node_t* node);
void selector_node_free(ifcopenshell_selector_node_t* root);

} // namespace bindings
} // namespace ifcapi

#endif
