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
    ASSERT(ifcopenshell_guid_generate(&guid), "guid_generate succeeds");
    ASSERT(guid.data != NULL, "guid_generate returns non-NULL");
    ASSERT(guid.size == 22, "GUID is 22 chars");
    printf("  Generated GUID: %s\n", guid.data);

    /* Round-trip: compress → expand → compress */
    ifcopenshell_string_t uuid = {0};
    ASSERT(ifcopenshell_guid_expand(guid.data, &uuid), "guid_expand succeeds");
    ASSERT(uuid.data != NULL, "guid_expand returns non-NULL");
    ASSERT(uuid.size == 32, "UUID is 32 hex chars");
    printf("  Expanded UUID:  %s\n", uuid.data);

    ifcopenshell_string_t guid2 = {0};
    ASSERT(ifcopenshell_guid_compress(uuid.data, &guid2), "guid_compress succeeds");
    ASSERT(guid2.data != NULL, "guid_compress returns non-NULL");
    ASSERT(strcmp(guid.data, guid2.data) == 0, "round-trip GUID matches");

    ifcopenshell_string_destroy(&guid);
    ifcopenshell_string_destroy(&uuid);
    ifcopenshell_string_destroy(&guid2);

    /* Known test vector from IFC spec */
    ifcopenshell_string_t known = {0};
    ASSERT(ifcopenshell_guid_compress("5765f8e4d5554a83a81c05af3132e830", &known),
           "known compress succeeds");
    ASSERT(known.data != NULL, "known compress returns non-NULL");
    printf("  Known vector:   %s (len=%zu)\n", known.data, known.size);
    ASSERT(known.size == 22, "known GUID is 22 chars");

    ifcopenshell_string_t known_back = {0};
    ASSERT(ifcopenshell_guid_expand(known.data, &known_back), "known expand succeeds");
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
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_NONE, "error kind starts empty");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_NONE, "error code starts empty");

    /* NULL file should fail gracefully */
    ifcopenshell_instance_t* inst = NULL;
    ifcopenshell_root_create_entity_options_t options = {0};
    options.ifc_class = "IfcWall";
    ASSERT(!ifcopenshell_root_create_entity(NULL, &options, &inst), "NULL file returns false");
    ASSERT(inst == NULL, "NULL file leaves output NULL");
    ASSERT(strlen(ifcopenshell_last_error_message()) > 0, "error message set");
    printf("  Error msg: %s\n", ifcopenshell_last_error_message());

    ifcopenshell_clear_error();
    ASSERT(strlen(ifcopenshell_last_error_message()) == 0, "error cleared");
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_NONE, "error kind cleared");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_NONE, "error code cleared");

    printf("  Error handling tests done.\n\n");
}

static void test_file_ops(void) {
    printf("=== File operations tests ===\n");

    /* Create IFC4 file */
    ifcopenshell_file_t* f = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &f), "new_file succeeds");
    ASSERT(f != NULL, "new_file returns non-NULL");
    printf("  Created IFC4 file: %p\n", (void*)f);

    ifcopenshell_string_t schema = {0};
    ASSERT(ifcopenshell_file_schema_name(f, &schema), "schema_name succeeds");
    ASSERT(schema.data != NULL, "schema_name returns non-NULL");
    ASSERT(strcmp(schema.data, "IFC4") == 0, "schema is IFC4");
    printf("  Schema: %s\n", schema.data);
    ifcopenshell_string_destroy(&schema);

    /* Create entity */
    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(f, "IfcWall", &wall), "create_entity succeeds");
    ASSERT(wall != NULL, "create_entity returns non-NULL");

    /* Entity declaration/type */
    ifcopenshell_declaration_t* decl = NULL;
    ASSERT(ifcopenshell_instance_declaration(wall, &decl), "declaration succeeds");
    ASSERT(decl != NULL, "declaration returns non-NULL");
    ifcopenshell_string_t type = {0};
    ASSERT(ifcopenshell_declaration_name(decl, &type), "declaration_name succeeds");
    ASSERT(type.data != NULL, "declaration_name returns non-NULL");
    ASSERT(strcmp(type.data, "IfcWall") == 0, "entity type is IfcWall");
    ifcopenshell_string_destroy(&type);
    ifcopenshell_declaration_destroy(decl);

    /* is_a */
    bool is_type = false;
    ASSERT(ifcopenshell_instance_is_a(wall, "IfcWall", &is_type), "is_a IfcWall succeeds");
    ASSERT(is_type, "is_a IfcWall");
    ASSERT(ifcopenshell_instance_is_a(wall, "IfcRoot", &is_type), "is_a IfcRoot succeeds");
    ASSERT(is_type, "is_a IfcRoot");
    ASSERT(ifcopenshell_instance_is_a(wall, "IfcDoor", &is_type), "is_a IfcDoor succeeds");
    ASSERT(!is_type, "not is_a IfcDoor");

    /* Set/get string attribute */
    ASSERT(ifcopenshell_instance_set_argument_string(wall, 2, "TestWall"), "set Name succeeds");
    ifcopenshell_parse_attribute_value_t* name_value = NULL;
    ASSERT(ifcopenshell_instance_get_argument(wall, 2, &name_value), "get Name succeeds");
    ifcopenshell_string_t name = {0};
    ASSERT(ifcopenshell_parse_attribute_value_as_string(name_value, &name), "Name is string");
    ASSERT(name.data != NULL, "Name returns non-NULL");
    ASSERT(strcmp(name.data, "TestWall") == 0, "Name matches");
    ifcopenshell_string_destroy(&name);
    ifcopenshell_parse_attribute_value_destroy(name_value);

    /* by_type */
    ifcopenshell_parse_instance_list_t* walls = NULL;
    ASSERT(ifcopenshell_file_by_type(f, "IfcWall", &walls), "by_type succeeds");
    size_t wall_count = 0;
    ASSERT(ifcopenshell_parse_instance_list_size(walls, &wall_count), "by_type count succeeds");
    ASSERT(wall_count == 1, "by_type count is 1");
    ifcopenshell_parse_instance_list_destroy(walls);

    ifcopenshell_instance_destroy(wall);
    ifcopenshell_file_destroy(f);
    printf("  File operations tests done.\n\n");
}

static void test_root_copy_and_reassign(void) {
    printf("=== Root copy/reassign tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");

    ifcopenshell_root_create_entity_options_t create_options = {0};
    create_options.ifc_class = "IfcWall";
    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_root_create_entity(file, &create_options, &wall), "root create wall succeeds");

    ifcopenshell_instance_t* copy = NULL;
    ASSERT(ifcopenshell_root_copy_class(file, wall, &copy), "root copy class succeeds");
    ASSERT(copy != NULL, "root copy returns an entity");

    ifcopenshell_parse_attribute_value_t* wall_guid_value = NULL;
    ifcopenshell_parse_attribute_value_t* copy_guid_value = NULL;
    ifcopenshell_string_t wall_guid = {0};
    ifcopenshell_string_t copy_guid = {0};
    ASSERT(ifcopenshell_instance_get_argument(wall, 0, &wall_guid_value), "read wall GlobalId succeeds");
    ASSERT(ifcopenshell_instance_get_argument(copy, 0, &copy_guid_value), "read copied GlobalId succeeds");
    ASSERT(ifcopenshell_parse_attribute_value_as_string(wall_guid_value, &wall_guid), "wall GlobalId is a string");
    ASSERT(ifcopenshell_parse_attribute_value_as_string(copy_guid_value, &copy_guid), "copied GlobalId is a string");
    ASSERT(strcmp(wall_guid.data, copy_guid.data) != 0, "copy has a fresh GlobalId");

    ifcopenshell_root_reassign_class_options_t reassign_options = {0};
    reassign_options.product = copy;
    reassign_options.ifc_class = "IfcSlab";
    reassign_options.has_ifc_class = true;
    reassign_options.predefined_type = "FLOOR";
    reassign_options.has_predefined_type = true;
    ifcopenshell_instance_t* slab = NULL;
    ASSERT(ifcopenshell_root_reassign_class(file, &reassign_options, &slab), "root reassign class succeeds");
    bool is_slab = false;
    ASSERT(ifcopenshell_instance_is_a(slab, "IfcSlab", &is_slab), "reassigned entity type query succeeds");
    ASSERT(is_slab, "reassigned entity is an IfcSlab");

    ifcopenshell_instance_t* default_wall = NULL;
    ASSERT(ifcopenshell_root_create_entity(file, &create_options, &default_wall), "create default-test wall succeeds");
    ifcopenshell_root_reassign_class_options_t default_options = {0};
    default_options.product = default_wall;
    ifcopenshell_instance_t* proxy = NULL;
    ASSERT(ifcopenshell_root_reassign_class(file, &default_options, &proxy), "default root reassign succeeds");
    bool is_proxy = false;
    ASSERT(ifcopenshell_instance_is_a(proxy, "IfcBuildingElementProxy", &is_proxy), "default entity type query succeeds");
    ASSERT(is_proxy, "omitted target class defaults to IfcBuildingElementProxy");

    ifcopenshell_string_destroy(&wall_guid);
    ifcopenshell_string_destroy(&copy_guid);
    ifcopenshell_parse_attribute_value_destroy(wall_guid_value);
    ifcopenshell_parse_attribute_value_destroy(copy_guid_value);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_instance_destroy(copy);
    ifcopenshell_instance_destroy(slab);
    ifcopenshell_instance_destroy(default_wall);
    ifcopenshell_instance_destroy(proxy);
    ifcopenshell_file_destroy(file);
    printf("  Root copy/reassign tests done.\n\n");
}

int ifcapi_run_root_smoke_tests(void) {
    printf("ifcapi root smoke tests\n\n");

    test_guid();
    test_error_handling();
    test_file_ops();
    test_root_copy_and_reassign();

    printf("=== Results: %d/%d passed ===\n",
           tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
