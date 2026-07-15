// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, m); tests_failed++; } } while (0)

static size_t type_count(ifcopenshell_file_t *file, const char *type) {
    ifcopenshell_parse_instance_list_t *values = NULL;
    size_t size = 0;
    ASSERT(ifcopenshell_file_by_type(file, type, &values), "by_type succeeds");
    ASSERT(ifcopenshell_parse_instance_list_size(values, &size), "list size succeeds");
    ifcopenshell_parse_instance_list_destroy(values);
    return size;
}

static size_t related_count(ifcopenshell_instance_t *rel) {
    ifcopenshell_parse_attribute_value_t *value = NULL;
    ifcopenshell_parse_instance_list_t *items = NULL;
    size_t size = 0;
    ASSERT(ifcopenshell_instance_get_argument_by_name(rel, "RelatedObjects", &value), "read RelatedObjects succeeds");
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(value, &items), "RelatedObjects is a list");
    ASSERT(ifcopenshell_parse_instance_list_size(items, &size), "RelatedObjects size succeeds");
    ifcopenshell_parse_instance_list_destroy(items);
    ifcopenshell_parse_attribute_value_destroy(value);
    return size;
}

static ifcopenshell_parse_attribute_value_t *named_argument(ifcopenshell_instance_t *entity, const char *name) {
    ifcopenshell_parse_attribute_value_t *value = NULL;
    ASSERT(ifcopenshell_instance_get_argument_by_name(entity, name, &value), "named argument read succeeds");
    return value;
}

static void assert_string(ifcopenshell_instance_t *entity, const char *name, const char *expected) {
    ifcopenshell_parse_attribute_value_t *value = named_argument(entity, name);
    ifcopenshell_string_t text = {0};
    ASSERT(ifcopenshell_parse_attribute_value_as_string(value, &text), "argument is a string");
    ASSERT(text.data && strcmp(text.data, expected) == 0, "string value matches");
    ifcopenshell_string_destroy(&text);
    ifcopenshell_parse_attribute_value_destroy(value);
}

static ifcopenshell_instance_t *instance_argument(ifcopenshell_instance_t *entity, const char *name) {
    ifcopenshell_parse_attribute_value_t *value = named_argument(entity, name);
    ifcopenshell_instance_t *result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance(value, &result), "argument is an entity");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static void set_string(ifcopenshell_instance_t *entity, const char *name, const char *text) {
    uint32_t index = 0;
    ASSERT(ifcopenshell_instance_get_argument_index(entity, name, &index), "argument index lookup succeeds");
    ASSERT(ifcopenshell_instance_set_argument_string(entity, index, text), "string argument write succeeds");
}

static void test_resource_workflows(void) {
    ifcopenshell_file_t *file = NULL;
    ifcopenshell_instance_t *project = NULL, *history = NULL, *resource = NULL, *quantity = NULL, *time = NULL;
    ifcopenshell_instance_t *product = NULL, *product2 = NULL, *first = NULL, *second = NULL;
    ifcopenshell_instance_t *pset = NULL, *task = NULL, *task_time = NULL, *product_rel = NULL, *product_rel2 = NULL;
    ifcopenshell_instance_t *process_rel = NULL, *usage = NULL, *relation_history = NULL, *child = NULL, *grandchild = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcProject", &project), "create project succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcOwnerHistory", &history), "create owner history succeeds");
    ifcopenshell_resource_add_resource_options_t add = {0};
    add.owner_history = history;
    add.has_owner_history = true;
    ASSERT(ifcopenshell_resource_add_resource(file, &add, &resource), "default resource creation succeeds");
    ASSERT(resource != NULL, "resource handle returned");
    assert_string(resource, "Name", "Unnamed");
    assert_string(resource, "PredefinedType", "NOTDEFINED");
    ASSERT(!ifcopenshell_resource_add_resource_quantity(file, resource, "IfcQuantityCount", &quantity),
           "invalid default crew quantity is rejected");
    ASSERT(quantity == NULL, "invalid quantity returns no handle");
    ASSERT(ifcopenshell_last_error_message()[0] != '\0', "invalid quantity exposes last error");
    ASSERT(ifcopenshell_resource_add_resource_quantity(file, resource, "IfcQuantityTime", &quantity),
           "valid crew quantity succeeds");
    ASSERT(ifcopenshell_resource_add_resource_time(file, resource, &time), "resource time succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcBuildingElementProxy", &product), "create product succeeds");
    ifcopenshell_resource_assignment_options_t assignment = {0};
    assignment.relating_resource = resource;
    assignment.related_object = product;
    assignment.owner_history = history;
    assignment.has_owner_history = true;
    ASSERT(ifcopenshell_resource_assign_resource(file, &assignment, &first), "resource assignment succeeds");
    ASSERT(ifcopenshell_resource_assign_resource(file, &assignment, &second), "duplicate assignment succeeds");
    ASSERT(related_count(first) == 1, "duplicate pair is suppressed");
    relation_history = instance_argument(first, "OwnerHistory");
    uint32_t history_id = 0, relation_history_id = 0;
    ASSERT(ifcopenshell_instance_id(history, &history_id), "history id succeeds");
    ASSERT(ifcopenshell_instance_id(relation_history, &relation_history_id), "relation history id succeeds");
    ASSERT(history_id == relation_history_id, "explicit owner history is retained");

    ifcopenshell_pset_add_pset_options_t add_pset = {0};
    add_pset.product = resource;
    add_pset.name = "EPset_Productivity";
    ASSERT(ifcopenshell_pset_add_pset(file, &add_pset, &pset), "productivity pset creation succeeds");
    void *properties = NULL;
    ASSERT(ifcopenshell_pset_props_new(&properties), "property bag creation succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(properties, "BaseQuantityConsumed", "PT1H"), "consumed duration set succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(properties, "BaseQuantityProducedName", "Count"), "produced name set succeeds");
    ASSERT(ifcopenshell_pset_props_set_double(properties, "BaseQuantityProducedValue", 2.0), "produced value set succeeds");
    ifcopenshell_pset_edit_pset_options_t edit_pset = {0};
    edit_pset.pset = pset;
    edit_pset.properties = properties;
    edit_pset.should_purge = true;
    bool edited = false;
    ASSERT(ifcopenshell_pset_edit_pset(file, &edit_pset, &edited) && edited, "productivity pset edit succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcBuildingElementProxy", &product2), "create second product succeeds");
    ifcopenshell_sequence_add_task_options_t add_task = {0};
    ASSERT(ifcopenshell_sequence_add_task(file, &add_task, &task), "task creation succeeds");
    ifcopenshell_sequence_assign_product_options_t assign_product = {0};
    ASSERT(ifcopenshell_sequence_assign_product(file, product, task, &assign_product, &product_rel), "first product assignment succeeds");
    ASSERT(ifcopenshell_sequence_assign_product(file, product2, task, &assign_product, &product_rel2), "second product assignment succeeds");
    ifcopenshell_sequence_assign_process_options_t assign_process = {0};
    ASSERT(ifcopenshell_sequence_assign_process(file, task, resource, &assign_process, &process_rel), "process assignment succeeds");
    ASSERT(ifcopenshell_resource_calculate_resource_work(file, resource), "resource work calculation succeeds");
    usage = instance_argument(resource, "Usage");
    assert_string(usage, "ScheduleWork", "PT1.0H");
    ifcopenshell_sequence_add_task_time_options_t add_task_time = {0};
    ASSERT(ifcopenshell_sequence_add_task_time(file, task, &add_task_time, &task_time), "task time creation succeeds");
    set_string(task_time, "ScheduleDuration", "P1D");
    ASSERT(ifcopenshell_resource_calculate_resource_usage(file, resource), "resource usage calculation succeeds");
    ifcopenshell_parse_attribute_value_t *schedule_usage = named_argument(usage, "ScheduleUsage");
    double usage_value = 0.0;
    ASSERT(ifcopenshell_parse_attribute_value_as_double(schedule_usage, &usage_value), "ScheduleUsage is numeric");
    ASSERT(fabs(usage_value - 0.125) < 1e-9, "ScheduleUsage calculation matches");
    ifcopenshell_parse_attribute_value_destroy(schedule_usage);

    ifcopenshell_resource_add_resource_options_t nested = {0};
    nested.parent_resource = resource;
    nested.has_parent_resource = true;
    nested.ifc_class = "IfcLaborResource";
    nested.has_ifc_class = true;
    ASSERT(ifcopenshell_resource_add_resource(file, &nested, &child), "nested resource creation succeeds");
    nested.parent_resource = child;
    ASSERT(ifcopenshell_resource_add_resource(file, &nested, &grandchild), "grandchild resource creation succeeds");
    ifcopenshell_resource_remove_resource_options_t remove = {0};
    remove.resource = resource;
    ASSERT(ifcopenshell_resource_remove_resource(file, &remove), "resource removal succeeds");
    ASSERT(type_count(file, "IfcCrewResource") == 0, "resource is removed");
    ASSERT(type_count(file, "IfcLaborResource") == 0, "nested resources are removed recursively");
    ASSERT(type_count(file, "IfcRelDeclares") == 0, "empty declaration is removed");
    ifcopenshell_instance_destroy(grandchild);
    ifcopenshell_instance_destroy(child);
    ifcopenshell_instance_destroy(task_time);
    ifcopenshell_instance_destroy(usage);
    ifcopenshell_instance_destroy(process_rel);
    ifcopenshell_instance_destroy(product_rel2);
    ifcopenshell_instance_destroy(product_rel);
    ifcopenshell_instance_destroy(task);
    ifcopenshell_pset_props_free(properties);
    ifcopenshell_instance_destroy(pset);
    ifcopenshell_instance_destroy(relation_history);
    ifcopenshell_instance_destroy(second);
    ifcopenshell_instance_destroy(first);
    ifcopenshell_instance_destroy(product2);
    ifcopenshell_instance_destroy(product);
    ifcopenshell_instance_destroy(time);
    ifcopenshell_instance_destroy(quantity);
    ifcopenshell_instance_destroy(resource);
    ifcopenshell_instance_destroy(history);
    ifcopenshell_instance_destroy(project);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_resource_smoke_tests(void) {
    printf("=== Resource C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_resource_workflows();
    printf("Resource C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
