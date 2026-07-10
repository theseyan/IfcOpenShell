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

/**
 * Format a selector query against an element.
 *
 * Evaluates a format expression (e.g. attribute names, arithmetic,
 * string functions) against an element and returns the formatted string.
 *
 * @param file Optional IFC file context.
 * @param instance The element to format against.
 * @param query The format expression string.
 * @return The formatted result, or empty on error.
 */
IFCAPI_BINDING std::optional<std::string> selector_format(
    std::optional<ifcopenshell::file*> file,
    std::optional<express::Base> instance,
    const std::string& query);

/**
 * Parse a selector query into a list of key entries.
 *
 * Extracts the key path from a get-element query (e.g.
 * "IfcWall/Name" yields keys ["IfcWall", "Name"]).
 *
 * @param query The selector query string.
 * @return Opaque key list handle. Free with selector_keys_free.
 */
IFCAPI_BINDING ifcopenshell_selector_keys_t* selector_parse_keys(const std::string& query);

/**
 * Return the number of keys in a parsed key list.
 *
 * @param keys Opaque key list handle from selector_parse_keys.
 * @return Number of keys, or 0 if keys is null.
 */
IFCAPI_BINDING size_t selector_keys_count(std::optional<ifcopenshell_selector_keys_t*> keys);

/**
 * Return the text of a key at the given index.
 *
 * @param keys Opaque key list handle.
 * @param index Zero-based key index.
 * @return Key text, or empty string if out of range.
 */
IFCAPI_BINDING std::string selector_keys_get(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);

/**
 * Check whether a key at the given index is a regular expression.
 *
 * @param keys Opaque key list handle.
 * @param index Zero-based key index.
 * @return True if the key is a regex pattern.
 */
IFCAPI_BINDING bool selector_keys_is_regex(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);

/**
 * Free a parsed key list.
 *
 * @param keys Opaque key list handle from selector_parse_keys.
 */
IFCAPI_BINDING void selector_keys_free(std::optional<ifcopenshell_selector_keys_t*> keys);

/**
 * Parse a filter query into an AST.
 *
 * Parses a filter expression (e.g. "IfcWall, Pset_WallCommon.FireRating=*2h*")
 * into a tree of selector nodes.
 *
 * @param query The filter query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_filter(const std::string& query);

/**
 * Parse a get-element query into an AST.
 *
 * Parses a key path expression (e.g. "IfcWall/Name") into a tree of
 * selector nodes.
 *
 * @param query The get-element query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_get_element(const std::string& query);

/**
 * Parse a format query into an AST.
 *
 * Parses a format expression (e.g. "Name + ' - ' + GlobalId") into
 * a tree of selector nodes.
 *
 * @param query The format query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_format(const std::string& query);

/**
 * Return the kind of a selector node.
 *
 * Values below IFCSEL_TOKEN_FIRST (100) are grammar rule nodes;
 * values at or above IFCSEL_TOKEN_FIRST are token nodes.
 *
 * @param node The selector node.
 * @return Node kind as an ifcsel_node_kind value, or 0 if node is null.
 */
IFCAPI_BINDING int32_t selector_node_kind(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Return the number of child nodes.
 *
 * @param node The selector node.
 * @return Number of children, or 0 if node is null.
 */
IFCAPI_BINDING size_t selector_node_child_count(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Return a child node at the given index.
 *
 * @param node The parent selector node.
 * @param index Zero-based child index.
 * @return The child node, or null if out of range.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_node_child(
    std::optional<const ifcopenshell_selector_node_t*> node,
    size_t index);

/**
 * Return the text content of a token node.
 *
 * For token nodes (kind >= IFCSEL_TOKEN_FIRST), returns the matched
 * text. For rule nodes, returns empty.
 *
 * @param node The selector node.
 * @return Node text, or empty string if not a token.
 */
IFCAPI_BINDING std::string selector_node_text(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Free a selector AST and all its children.
 *
 * @param root Root node of the AST from selector_parse_filter,
 *             selector_parse_get_element, or selector_parse_format.
 */
IFCAPI_BINDING void selector_node_free(std::optional<ifcopenshell_selector_node_t*> root);

} // namespace bindings
} // namespace ifcapi

#endif
