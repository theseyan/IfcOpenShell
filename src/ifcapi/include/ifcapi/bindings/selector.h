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
 * @return The formatted result, or no result if evaluation fails.
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
 * @return Parsed key list, or no result if the query is invalid. Release it
 * with selector_keys_free.
 */
IFCAPI_BINDING ifcopenshell_selector_keys_t* selector_parse_keys(const std::string& query);

/**
 * Return the number of keys in a parsed key list.
 *
 * @param keys Parsed key list from selector_parse_keys.
 * @return Number of keys, or 0 when no key list is provided.
 */
IFCAPI_BINDING size_t selector_keys_count(std::optional<ifcopenshell_selector_keys_t*> keys);

/**
 * Return the text of a key at the given index.
 *
 * @param keys Parsed key list.
 * @param index Zero-based key index.
 * @return Key text, or an empty string if no key list is provided or the index is out of range.
 */
IFCAPI_BINDING std::string selector_keys_get(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);

/**
 * Check whether a key at the given index is a regular expression.
 *
 * @param keys Parsed key list.
 * @param index Zero-based key index.
 * @return True if the key is a regular expression pattern; otherwise false.
 */
IFCAPI_BINDING bool selector_keys_is_regex(std::optional<ifcopenshell_selector_keys_t*> keys, size_t index);

/**
 * Release a parsed key list.
 *
 * @param keys Parsed key list from selector_parse_keys.
 */
IFCAPI_BINDING void selector_keys_free(std::optional<ifcopenshell_selector_keys_t*> keys);

/**
 * Parse a filter query into a selector syntax tree.
 *
 * Parses a filter expression (e.g. "IfcWall, Pset_WallCommon.FireRating=*2h*")
 * into a tree of selector nodes.
 *
 * @param query The filter query string.
 * @return Root node of the syntax tree, or no result if the query is invalid.
 * Release the tree with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_filter(const std::string& query);

/**
 * Parse a get-element query into a selector syntax tree.
 *
 * Parses a key path expression (e.g. "IfcWall/Name") into a tree of
 * selector nodes.
 *
 * @param query The get-element query string.
 * @return Root node of the syntax tree, or no result if the query is invalid.
 * Release the tree with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_get_element(const std::string& query);

/**
 * Parse a format query into a selector syntax tree.
 *
 * Parses a format expression (e.g. "Name + ' - ' + GlobalId") into
 * a tree of selector nodes.
 *
 * @param query The format query string.
 * @return Root node of the syntax tree, or no result if the query is invalid.
 * Release the tree with selector_node_free.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_parse_format(const std::string& query);

/**
 * Return the kind of a selector node.
 *
 * Values below 100 are grammar-rule nodes; values at or above 100 are token
 * nodes. When no node is provided, the anonymous-token kind is returned.
 *
 * @param node The selector node.
 * @return Node-kind value, or the anonymous-token kind when no node is provided.
 */
IFCAPI_BINDING int32_t selector_node_kind(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Return the number of child nodes.
 *
 * @param node The selector node.
 * @return Number of children, or 0 when no node is provided.
 */
IFCAPI_BINDING size_t selector_node_child_count(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Return a child node at the given index.
 *
 * @param node The parent selector node.
 * @param index Zero-based child index.
 * @return The child node, or no result if no parent is provided or the index is out of range.
 */
IFCAPI_BINDING ifcopenshell_selector_node_t* selector_node_child(
    std::optional<const ifcopenshell_selector_node_t*> node,
    size_t index);

/**
 * Return the text content of a token node.
 *
 * For token nodes (kind at or above 100), returns the matched text. For
 * grammar-rule nodes, returns an empty string.
 *
 * @param node The selector node.
 * @return Node text, or an empty string if no node is provided or the node is not a token.
 */
IFCAPI_BINDING std::string selector_node_text(std::optional<const ifcopenshell_selector_node_t*> node);

/**
 * Release a selector syntax tree and all its descendants.
 *
 * @param root Root node from selector_parse_filter, selector_parse_get_element,
 *             or selector_parse_format.
 */
IFCAPI_BINDING void selector_node_free(std::optional<ifcopenshell_selector_node_t*> root);

} // namespace bindings
} // namespace ifcapi

#endif
