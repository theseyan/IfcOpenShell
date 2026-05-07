// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/selector/ast.h"
#include "ifcopenshell_api.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    tests_run++; \
    if (!(cond)) { \
        fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); \
        tests_failed++; \
    } \
} while(0)

static void assert_string(ifcopenshell_string_t* value, const char* expected, const char* msg) {
    ASSERT(value->data != NULL, "string data is non-NULL");
    ASSERT(value->size == strlen(expected), "string size matches");
    ASSERT(strncmp(value->data, expected, value->size) == 0, msg);
}

static void test_selector_format_nullable_string(void) {
    printf("=== Selector format C ABI tests ===\n");

    ifcopenshell_string_t empty = {0};
    ASSERT(ifcopenshell_ifcapi_selector_format(NULL, NULL, "concat(\"\", \"\")", &empty),
           "empty string format succeeds");
    ASSERT(empty.data != NULL, "empty string is distinct from NULL");
    ASSERT(empty.size == 0, "empty string size is zero");
    ifcopenshell_string_destroy(&empty);

    ifcopenshell_string_t missing = {0};
    ASSERT(ifcopenshell_ifcapi_selector_format(NULL, NULL, "{{undefined}}", &missing),
           "undefined variable format succeeds");
    ASSERT(missing.data == NULL, "undefined variable returns nullable string");
    ASSERT(missing.size == 0, "undefined variable size is zero");
    ifcopenshell_string_destroy(&missing);

    ifcopenshell_clear_error();
    ifcopenshell_string_t invalid = {0};
    ASSERT(!ifcopenshell_ifcapi_selector_format(NULL, NULL, "", &invalid), "invalid format returns false");
    ASSERT(strlen(ifcopenshell_last_error_message()) > 0, "invalid format sets an error");
    ifcopenshell_string_destroy(&invalid);

    printf("  Selector format C ABI tests done.\n\n");
}

static void test_selector_keys(void) {
    printf("=== Selector key parser C ABI tests ===\n");

    void* keys = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_parse_keys("Foobar./F.*/.\"Quoted Name\"", &keys), "parse_keys succeeds");
    ASSERT(keys != NULL, "parse_keys returns a handle");

    size_t count = 0;
    ASSERT(ifcopenshell_ifcapi_selector_keys_count(keys, &count), "keys_count succeeds");
    ASSERT(count == 3, "key count is three");

    ifcopenshell_string_t key = {0};
    bool is_regex = true;
    ASSERT(ifcopenshell_ifcapi_selector_keys_get(keys, 0, &key), "keys_get 0 succeeds");
    assert_string(&key, "Foobar", "key 0 text matches");
    ifcopenshell_string_destroy(&key);
    ASSERT(ifcopenshell_ifcapi_selector_keys_is_regex(keys, 0, &is_regex), "keys_is_regex 0 succeeds");
    ASSERT(!is_regex, "key 0 is not regex");

    ASSERT(ifcopenshell_ifcapi_selector_keys_get(keys, 1, &key), "keys_get 1 succeeds");
    assert_string(&key, "F.*", "key 1 text matches");
    ifcopenshell_string_destroy(&key);
    ASSERT(ifcopenshell_ifcapi_selector_keys_is_regex(keys, 1, &is_regex), "keys_is_regex 1 succeeds");
    ASSERT(is_regex, "key 1 is regex");

    ASSERT(ifcopenshell_ifcapi_selector_keys_get(keys, 2, &key), "keys_get 2 succeeds");
    assert_string(&key, "Quoted Name", "key 2 text matches");
    ifcopenshell_string_destroy(&key);
    ASSERT(ifcopenshell_ifcapi_selector_keys_is_regex(keys, 2, &is_regex), "keys_is_regex 2 succeeds");
    ASSERT(!is_regex, "key 2 is not regex");

    ASSERT(ifcopenshell_ifcapi_selector_keys_free(keys), "keys_free succeeds");
    printf("  Selector key parser C ABI tests done.\n\n");
}

static void test_selector_nodes(void) {
    printf("=== Selector node parser C ABI tests ===\n");

    void* root = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_parse_get_element("material.item.Name.0", &root),
           "parse_get_element succeeds");
    ASSERT(root != NULL, "parse_get_element returns a handle");

    int32_t kind = -1;
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(root, &kind), "root kind succeeds");
    ASSERT(kind == IFCSEL_NODE_START, "root is start node");

    size_t count = 0;
    ASSERT(ifcopenshell_ifcapi_selector_node_child_count(root, &count), "root child count succeeds");
    ASSERT(count == 1, "root has one child");

    void* keys = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_node_child(root, 0, &keys), "root child succeeds");
    ASSERT(keys != NULL, "root child is non-NULL");
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(keys, &kind), "keys kind succeeds");
    ASSERT(kind == IFCSEL_NODE_KEYS_LIST, "child is keys list");
    ASSERT(ifcopenshell_ifcapi_selector_node_child_count(keys, &count), "keys child count succeeds");
    ASSERT(count == 4, "keys list has four keys");

    void* first_key = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_node_child(keys, 0, &first_key), "first key child succeeds");
    ASSERT(first_key != NULL, "first key child is non-NULL");
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(first_key, &kind), "first key kind succeeds");
    ASSERT(kind == IFCSEL_NODE_KEY, "first child is key node");

    ASSERT(ifcopenshell_ifcapi_selector_node_free(root), "node_free succeeds");

    root = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_parse_filter("IfcWall, Name=Foo", &root), "parse_filter succeeds");
    ASSERT(root != NULL, "parse_filter returns a handle");
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(root, &kind), "filter root kind succeeds");
    ASSERT(kind == IFCSEL_NODE_START, "filter root is start node");
    ASSERT(ifcopenshell_ifcapi_selector_node_free(root), "filter node_free succeeds");

    root = NULL;
    ASSERT(ifcopenshell_ifcapi_selector_parse_format("upper({{ class }})", &root), "parse_format succeeds");
    ASSERT(root != NULL, "parse_format returns a handle");
    ASSERT(ifcopenshell_ifcapi_selector_node_child(root, 0, &keys), "format expression child succeeds");
    ASSERT(keys != NULL, "format expression child is non-NULL");
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(keys, &kind), "format expression kind succeeds");
    ASSERT(kind == IFCSEL_NODE_FUNCTION, "format expression is function node");
    ASSERT(ifcopenshell_ifcapi_selector_node_child(keys, 0, &first_key), "function child succeeds");
    ASSERT(first_key != NULL, "function child is non-NULL");
    ASSERT(ifcopenshell_ifcapi_selector_node_kind(first_key, &kind), "function child kind succeeds");
    ASSERT(kind == IFCSEL_NODE_UPPER, "function child is upper node");
    ASSERT(ifcopenshell_ifcapi_selector_node_free(root), "format node_free succeeds");

    printf("  Selector node parser C ABI tests done.\n\n");
}

int main(void) {
    printf("ifcapi selector C ABI smoke tests\n\n");

    test_selector_format_nullable_string();
    test_selector_keys();
    test_selector_nodes();

    printf("=== Results: %d/%d passed ===\n", tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
