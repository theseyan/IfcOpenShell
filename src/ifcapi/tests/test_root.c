// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcopenshell_api.h"

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

static void test_guid(void) {
    printf("=== GUID tests ===\n");

    /* Generate a new GUID */
    char* guid = ifcopenshell_guid_new();
    ASSERT(guid != NULL, "guid_new returns non-NULL");
    ASSERT(strlen(guid) == 22, "GUID is 22 chars");
    printf("  Generated GUID: %s\n", guid);

    /* Round-trip: compress → expand → compress */
    char* uuid = ifcopenshell_guid_expand(guid);
    ASSERT(uuid != NULL, "guid_expand returns non-NULL");
    ASSERT(strlen(uuid) == 32, "UUID is 32 hex chars");
    printf("  Expanded UUID:  %s\n", uuid);

    char* guid2 = ifcopenshell_guid_compress(uuid);
    ASSERT(guid2 != NULL, "guid_compress returns non-NULL");
    ASSERT(strcmp(guid, guid2) == 0, "round-trip GUID matches");

    ifcopenshell_free_string(guid);
    ifcopenshell_free_string(uuid);
    ifcopenshell_free_string(guid2);

    /* Known test vector from IFC spec */
    char* known = ifcopenshell_guid_compress("5765f8e4d5554a83a81c05af3132e830");
    ASSERT(known != NULL, "known compress returns non-NULL");
    printf("  Known vector:   %s (len=%zu)\n", known, strlen(known));
    ASSERT(strlen(known) == 22, "known GUID is 22 chars");

    char* known_back = ifcopenshell_guid_expand(known);
    ASSERT(known_back != NULL, "known expand returns non-NULL");
    ASSERT(strcmp(known_back, "5765f8e4d5554a83a81c05af3132e830") == 0,
           "known round-trip matches");

    ifcopenshell_free_string(known);
    ifcopenshell_free_string(known_back);

    printf("  GUID tests done.\n\n");
}

static void test_error_handling(void) {
    printf("=== Error handling tests ===\n");

    ifcopenshell_clear_error();
    ASSERT(strlen(ifcopenshell_last_error_message()) == 0, "error starts empty");

    /* NULL file should fail gracefully */
    ifcopenshell_ifc_instance_t* inst = ifcopenshell_root_create_entity(NULL, "IfcWall", NULL, NULL, 0);
    ASSERT(inst == NULL, "NULL file returns NULL");
    ASSERT(strlen(ifcopenshell_last_error_message()) > 0, "error message set");
    printf("  Error msg: %s\n", ifcopenshell_last_error_message());

    ifcopenshell_clear_error();
    ASSERT(strlen(ifcopenshell_last_error_message()) == 0, "error cleared");

    printf("  Error handling tests done.\n\n");
}

static void test_file_ops(void) {
    printf("=== File operations tests ===\n");

    /* Create IFC4 file */
    ifcopenshell_ifc_file_t* f = ifcopenshell_file_create("IFC4");
    ASSERT(f != NULL, "file_create returns non-NULL");
    printf("  Created IFC4 file: %p\n", (void*)f);

    const char* schema = ifcopenshell_file_schema(f);
    ASSERT(schema != NULL, "file_schema returns non-NULL");
    ASSERT(strcmp(schema, "IFC4") == 0, "schema is IFC4");
    printf("  Schema: %s\n", schema);

    /* Create entity */
    ifcopenshell_ifc_instance_t* wall = ifcopenshell_file_create_entity(f, "IfcWall");
    ASSERT(wall != NULL, "create_entity returns non-NULL");

    /* Entity type */
    const char* type = ifcopenshell_entity_type(wall);
    ASSERT(type != NULL, "entity_type returns non-NULL");
    ASSERT(strcmp(type, "IfcWall") == 0, "entity type is IfcWall");

    /* is_a */
    ASSERT(ifcopenshell_entity_is_a(wall, "IfcWall"), "is_a IfcWall");
    ASSERT(ifcopenshell_entity_is_a(wall, "IfcRoot"), "is_a IfcRoot");
    ASSERT(!ifcopenshell_entity_is_a(wall, "IfcDoor"), "not is_a IfcDoor");

    /* Set/get string attribute */
    ifcopenshell_entity_set_string(wall, "Name", "TestWall");
    char* name = ifcopenshell_entity_get_string(wall, "Name");
    ASSERT(name != NULL, "get_string returns non-NULL");
    ASSERT(strcmp(name, "TestWall") == 0, "Name matches");
    ifcopenshell_free_string(name);

    /* by_type */
    int32_t count = ifcopenshell_file_by_type_count(f, "IfcWall");
    ASSERT(count == 1, "by_type_count is 1");

    ifcopenshell_ifc_instance_destroy(wall);
    ifcopenshell_ifc_file_destroy(f);
    printf("  File operations tests done.\n\n");
}

int main(void) {
    printf("ifcapi smoke tests\n\n");

    test_guid();
    test_error_handling();
    test_file_ops();

    printf("=== Results: %d/%d passed ===\n",
           tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
