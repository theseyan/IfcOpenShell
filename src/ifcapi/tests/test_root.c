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
    ifcopenshell_string_t guid = {0};
    ASSERT(ifcopenshell_ifcapi_guid_new(&guid), "guid_new succeeds");
    ASSERT(guid.data != NULL, "guid_new returns non-NULL");
    ASSERT(guid.size == 22, "GUID is 22 chars");
    printf("  Generated GUID: %s\n", guid.data);

    /* Round-trip: compress → expand → compress */
    ifcopenshell_string_t uuid = {0};
    ASSERT(ifcopenshell_ifcapi_guid_expand(guid.data, &uuid), "guid_expand succeeds");
    ASSERT(uuid.data != NULL, "guid_expand returns non-NULL");
    ASSERT(uuid.size == 32, "UUID is 32 hex chars");
    printf("  Expanded UUID:  %s\n", uuid.data);

    ifcopenshell_string_t guid2 = {0};
    ASSERT(ifcopenshell_ifcapi_guid_compress(uuid.data, &guid2), "guid_compress succeeds");
    ASSERT(guid2.data != NULL, "guid_compress returns non-NULL");
    ASSERT(strcmp(guid.data, guid2.data) == 0, "round-trip GUID matches");

    ifcopenshell_string_destroy(&guid);
    ifcopenshell_string_destroy(&uuid);
    ifcopenshell_string_destroy(&guid2);

    /* Known test vector from IFC spec */
    ifcopenshell_string_t known = {0};
    ASSERT(ifcopenshell_ifcapi_guid_compress("5765f8e4d5554a83a81c05af3132e830", &known),
           "known compress succeeds");
    ASSERT(known.data != NULL, "known compress returns non-NULL");
    printf("  Known vector:   %s (len=%zu)\n", known.data, known.size);
    ASSERT(known.size == 22, "known GUID is 22 chars");

    ifcopenshell_string_t known_back = {0};
    ASSERT(ifcopenshell_ifcapi_guid_expand(known.data, &known_back), "known expand succeeds");
    ASSERT(known_back.data != NULL, "known expand returns non-NULL");
    ASSERT(strcmp(known_back.data, "5765f8e4d5554a83a81c05af3132e830") == 0,
           "known round-trip matches");

    ifcopenshell_string_destroy(&known);
    ifcopenshell_string_destroy(&known_back);

    printf("  GUID tests done.\n\n");
}

static void test_error_handling(void) {
    printf("=== Error handling tests ===\n");

    ifcopenshell_clear_error();
    ASSERT(strlen(ifcopenshell_last_error_message()) == 0, "error starts empty");

    /* NULL file should fail gracefully */
    ifcopenshell_ifc_instance_t* inst = NULL;
    ASSERT(!ifcopenshell_ifcapi_root_create_entity(NULL, "IfcWall", NULL, NULL, NULL, &inst),
           "NULL file returns false");
    ASSERT(inst == NULL, "NULL file leaves output NULL");
    ASSERT(strlen(ifcopenshell_last_error_message()) > 0, "error message set");
    printf("  Error msg: %s\n", ifcopenshell_last_error_message());

    ifcopenshell_clear_error();
    ASSERT(strlen(ifcopenshell_last_error_message()) == 0, "error cleared");

    printf("  Error handling tests done.\n\n");
}

static void test_file_ops(void) {
    printf("=== File operations tests ===\n");

    /* Create IFC4 file */
    ifcopenshell_ifc_file_t* f = NULL;
    ASSERT(ifcopenshell_ifcparse_new_file("IFC4", 0, "", &f), "new_file succeeds");
    ASSERT(f != NULL, "new_file returns non-NULL");
    printf("  Created IFC4 file: %p\n", (void*)f);

    ifcopenshell_string_t schema = {0};
    ASSERT(ifcopenshell_ifc_file_schema_name(f, &schema), "schema_name succeeds");
    ASSERT(schema.data != NULL, "schema_name returns non-NULL");
    ASSERT(strcmp(schema.data, "IFC4") == 0, "schema is IFC4");
    printf("  Schema: %s\n", schema.data);
    ifcopenshell_string_destroy(&schema);

    /* Create entity */
    ifcopenshell_ifc_instance_t* wall = NULL;
    ASSERT(ifcopenshell_ifc_file_create_entity_by_name(f, "IfcWall", &wall), "create_entity succeeds");
    ASSERT(wall != NULL, "create_entity returns non-NULL");

    /* Entity declaration/type */
    ifcopenshell_ifc_declaration_t* decl = NULL;
    ASSERT(ifcopenshell_ifc_instance_declaration(wall, &decl), "declaration succeeds");
    ASSERT(decl != NULL, "declaration returns non-NULL");
    ifcopenshell_string_t type = {0};
    ASSERT(ifcopenshell_ifc_declaration_name(decl, &type), "declaration_name succeeds");
    ASSERT(type.data != NULL, "declaration_name returns non-NULL");
    ASSERT(strcmp(type.data, "IfcWall") == 0, "entity type is IfcWall");
    ifcopenshell_string_destroy(&type);
    ifcopenshell_ifc_declaration_destroy(decl);

    /* is_a */
    bool is_type = false;
    ASSERT(ifcopenshell_ifc_instance_is_a(wall, "IfcWall", &is_type), "is_a IfcWall succeeds");
    ASSERT(is_type, "is_a IfcWall");
    ASSERT(ifcopenshell_ifc_instance_is_a(wall, "IfcRoot", &is_type), "is_a IfcRoot succeeds");
    ASSERT(is_type, "is_a IfcRoot");
    ASSERT(ifcopenshell_ifc_instance_is_a(wall, "IfcDoor", &is_type), "is_a IfcDoor succeeds");
    ASSERT(!is_type, "not is_a IfcDoor");

    /* Set/get string attribute */
    ASSERT(ifcopenshell_ifc_instance_set_argument_string(wall, 2, "TestWall"), "set Name succeeds");
    ifcopenshell_ifcparse_attribute_value_t* name_value = NULL;
    ASSERT(ifcopenshell_ifc_instance_get_argument(wall, 2, &name_value), "get Name succeeds");
    ifcopenshell_string_t name = {0};
    ASSERT(ifcopenshell_ifcparse_attribute_value_as_string(name_value, &name), "Name is string");
    ASSERT(name.data != NULL, "Name returns non-NULL");
    ASSERT(strcmp(name.data, "TestWall") == 0, "Name matches");
    ifcopenshell_string_destroy(&name);
    ifcopenshell_ifcparse_attribute_value_destroy(name_value);

    /* by_type */
    ifcopenshell_ifcparse_instance_list_t* walls = NULL;
    ASSERT(ifcopenshell_ifc_file_by_type(f, "IfcWall", &walls), "by_type succeeds");
    size_t count = 0;
    ASSERT(ifcopenshell_ifcparse_instance_list_size(walls, &count), "by_type size succeeds");
    ASSERT(count == 1, "by_type count is 1");
    ifcopenshell_ifcparse_instance_list_destroy(walls);

    ifcopenshell_ifc_instance_destroy(wall);
    ifcopenshell_ifc_file_destroy(f);
    printf("  File operations tests done.\n\n");
}

int ifcapi_run_root_smoke_tests(void) {
    printf("ifcapi root smoke tests\n\n");

    test_guid();
    test_error_handling();
    test_file_ops();

    printf("=== Results: %d/%d passed ===\n",
           tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
