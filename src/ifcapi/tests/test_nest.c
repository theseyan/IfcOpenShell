// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"
#include <stdio.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, m); tests_failed++; } } while (0)

static ifcopenshell_parse_instance_list_t *make_list(ifcopenshell_instance_t **items, size_t size) {
    ifcopenshell_instance_list_t handles = {items, size};
    ifcopenshell_parse_instance_list_t *result = NULL;
    ASSERT(ifcopenshell_parse_instance_list_create_from_handles(&handles, &result), "instance list creation succeeds");
    return result;
}

static uint32_t related_id(ifcopenshell_instance_t *rel, size_t index) {
    ifcopenshell_parse_attribute_value_t *value = NULL;
    ifcopenshell_parse_instance_list_t *items = NULL;
    ifcopenshell_instance_t *item = NULL;
    uint32_t id = 0;
    ASSERT(ifcopenshell_instance_get_argument_by_name(rel, "RelatedObjects", &value), "read RelatedObjects succeeds");
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(value, &items), "RelatedObjects is a list");
    ASSERT(ifcopenshell_parse_instance_list_get(items, index, &item), "related object lookup succeeds");
    ASSERT(ifcopenshell_instance_id(item, &id), "related object id succeeds");
    ifcopenshell_instance_destroy(item);
    ifcopenshell_parse_instance_list_destroy(items);
    ifcopenshell_parse_attribute_value_destroy(value);
    return id;
}

static uint32_t entity_id(ifcopenshell_instance_t *entity) {
    uint32_t id = 0;
    ASSERT(ifcopenshell_instance_id(entity, &id), "entity id succeeds");
    return id;
}

static void test_reorder_and_change(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *parent = NULL, *target = NULL, *a = NULL, *b = NULL, *c = NULL, *rel = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcTask", &parent), "create parent succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcTask", &target), "create target succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcTask", &a), "create a succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcTask", &b), "create b succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcTask", &c), "create c succeeds");
    ifcopenshell_instance_t *children[] = {a, b, c};
    ifcopenshell_parse_instance_list_t *list = make_list(children, 3);
    ifcopenshell_nest_assign_object_options_t assign = {0};
    assign.products = list;
    assign.relating_object = parent;
    ASSERT(ifcopenshell_nest_assign_object(file, &assign, &rel), "nest assignment succeeds");
    ifcopenshell_nest_reorder_nesting_options_t reorder = {0};
    reorder.item = c;
    ASSERT(ifcopenshell_nest_reorder_nesting(file, &reorder), "omitted old index reorder succeeds");
    ASSERT(related_id(rel, 0) == entity_id(c), "omitted old index locates item and defaults destination to front");
    reorder.old_index = 0;
    reorder.has_old_index = true;
    reorder.new_index = 99;
    reorder.has_new_index = true;
    ASSERT(ifcopenshell_nest_reorder_nesting(file, &reorder), "explicit zero old index succeeds");
    ASSERT(related_id(rel, 2) == entity_id(c), "explicit zero is literal and oversized destination appends");
    reorder.old_index = -1;
    reorder.new_index = -99;
    ASSERT(ifcopenshell_nest_reorder_nesting(file, &reorder), "negative and clamped indexes succeed");
    ASSERT(related_id(rel, 0) == entity_id(c), "very negative destination clamps to front");
    uint32_t before0 = related_id(rel, 0), before1 = related_id(rel, 1), before2 = related_id(rel, 2);
    reorder.old_index = 99;
    ASSERT(!ifcopenshell_nest_reorder_nesting(file, &reorder), "invalid old index reports failure");
    ASSERT(ifcopenshell_last_error_message()[0] != '\0', "invalid index exposes last error");
    ASSERT(related_id(rel, 0) == before0 && related_id(rel, 1) == before1 && related_id(rel, 2) == before2,
           "invalid index leaves order unchanged");
    ifcopenshell_nest_change_nest_options_t change = {0};
    change.item = c;
    change.new_parent = target;
    ASSERT(ifcopenshell_nest_change_nest(file, &change), "change nest succeeds");
    ifcopenshell_parse_instance_list_destroy(list);
    ifcopenshell_instance_destroy(rel);
    ifcopenshell_instance_destroy(c);
    ifcopenshell_instance_destroy(b);
    ifcopenshell_instance_destroy(a);
    ifcopenshell_instance_destroy(target);
    ifcopenshell_instance_destroy(parent);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_nest_smoke_tests(void) {
    printf("=== Nest C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_reorder_and_change();
    printf("Nest C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
