// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <math.h>
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

static void assert_instance_is(ifcopenshell_instance_t* inst, const char* ifc_class, const char* msg) {
    bool result = false;
    ASSERT(ifcopenshell_instance_is_a(inst, ifc_class, &result), "is_a succeeds");
    ASSERT(result, msg);
}

static ifcopenshell_parse_attribute_value_t* argument(ifcopenshell_instance_t* inst, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = NULL;
    ASSERT(ifcopenshell_instance_get_argument(inst, index, &value), "get_argument succeeds");
    ASSERT(value != NULL, "get_argument returns non-NULL");
    return value;
}

static void assert_string_argument(ifcopenshell_instance_t* inst, size_t index, const char* expected, const char* msg) {
    ifcopenshell_parse_attribute_value_t* value = argument(inst, index);
    ifcopenshell_string_t s = {0};
    ASSERT(ifcopenshell_parse_attribute_value_as_string(value, &s), "argument as string succeeds");
    ASSERT(s.data != NULL, "string argument data is non-NULL");
    ASSERT(s.size == strlen(expected), "string argument size matches");
    ASSERT(strncmp(s.data, expected, s.size) == 0, msg);
    ifcopenshell_string_destroy(&s);
    ifcopenshell_parse_attribute_value_destroy(value);
}

static ifcopenshell_instance_t* instance_argument(ifcopenshell_instance_t* inst, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = argument(inst, index);
    ifcopenshell_instance_t* result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance(value, &result), "argument as instance succeeds");
    ASSERT(result != NULL, "instance argument is non-NULL");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static ifcopenshell_parse_instance_list_t* instance_list_argument(ifcopenshell_instance_t* inst, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = argument(inst, index);
    ifcopenshell_parse_instance_list_t* result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(value, &result), "argument as instance list succeeds");
    ASSERT(result != NULL, "instance list argument is non-NULL");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static ifcopenshell_instance_t* list_get(ifcopenshell_parse_instance_list_t* list, size_t index) {
    ifcopenshell_instance_t* result = NULL;
    ASSERT(ifcopenshell_parse_instance_list_get(list, index, &result), "instance list get succeeds");
    ASSERT(result != NULL, "instance list item is non-NULL");
    return result;
}

static ifcopenshell_instance_t* owned_list_get(ifcopenshell_instance_list_t* list, size_t index) {
    ASSERT(list != NULL, "owned instance list is non-NULL");
    ASSERT(index < list->size, "owned instance list index in bounds");
    ifcopenshell_instance_t* result = list->items[index];
    ASSERT(result != NULL, "owned instance list item is non-NULL");
    return result;
}

static ifcopenshell_instance_list_t make_instance_list(ifcopenshell_instance_t** items, size_t size) {
    ifcopenshell_instance_list_t result;
    result.items = items;
    result.size = size;
    return result;
}

static bool same_instance(ifcopenshell_instance_t* a, ifcopenshell_instance_t* b) {
    uint32_t a_id = 0;
    uint32_t b_id = 0;
    ASSERT(ifcopenshell_instance_id(a, &a_id), "first instance id succeeds");
    ASSERT(ifcopenshell_instance_id(b, &b_id), "second instance id succeeds");
    return a_id == b_id;
}

static size_t list_size(ifcopenshell_parse_instance_list_t* list) {
    size_t result = 0;
    ASSERT(ifcopenshell_parse_instance_list_size(list, &result), "instance list size succeeds");
    return result;
}

static ifcopenshell_instance_list_t by_type(ifcopenshell_file_t* file, const char* ifc_class) {
    ifcopenshell_instance_list_t result = {0};
    ASSERT(ifcopenshell_file_by_type(file, ifc_class, &result), "by_type succeeds");
    return result;
}

static ifcopenshell_instance_t* find_named_instance(
    ifcopenshell_parse_instance_list_t* list,
    const char* name) {
    size_t count = list_size(list);
    for (size_t i = 0; i < count; ++i) {
        ifcopenshell_instance_t* inst = list_get(list, i);
        ifcopenshell_parse_attribute_value_t* value = argument(inst, 0);
        ifcopenshell_string_t s = {0};
        bool ok = ifcopenshell_parse_attribute_value_as_string(value, &s);
        bool match = ok && s.data != NULL && s.size == strlen(name) && strncmp(s.data, name, s.size) == 0;
        ifcopenshell_string_destroy(&s);
        ifcopenshell_parse_attribute_value_destroy(value);
        if (match) {
            return inst;
        }
        ifcopenshell_instance_destroy(inst);
    }
    return NULL;
}

static void assert_nominal_string(ifcopenshell_instance_t* prop, const char* expected) {
    ifcopenshell_instance_t* nominal = instance_argument(prop, 2);
    assert_string_argument(nominal, 0, expected, "nominal string matches");
    ifcopenshell_instance_destroy(nominal);
}

static void assert_nominal_double(ifcopenshell_instance_t* prop, double expected) {
    ifcopenshell_instance_t* nominal = instance_argument(prop, 2);
    ifcopenshell_parse_attribute_value_t* value = argument(nominal, 0);
    double result = 0.0;
    ASSERT(ifcopenshell_parse_attribute_value_as_double(value, &result), "nominal double succeeds");
    ASSERT(fabs(result - expected) < 1.e-9, "nominal double matches");
    ifcopenshell_parse_attribute_value_destroy(value);
    ifcopenshell_instance_destroy(nominal);
}

static void assert_nominal_bool(ifcopenshell_instance_t* prop, bool expected) {
    ifcopenshell_instance_t* nominal = instance_argument(prop, 2);
    ifcopenshell_parse_attribute_value_t* value = argument(nominal, 0);
    bool result = false;
    ASSERT(ifcopenshell_parse_attribute_value_as_bool(value, &result), "nominal bool succeeds");
    ASSERT(result == expected, "nominal bool matches");
    ifcopenshell_parse_attribute_value_destroy(value);
    ifcopenshell_instance_destroy(nominal);
}

static void test_pset_add_and_edit(void) {
    printf("=== Pset add/edit C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &wall), "create wall succeeds");
    ASSERT(wall != NULL, "wall is non-NULL");

    ifcopenshell_instance_t* pset = NULL;
    ASSERT(ifcopenshell_pset_add_pset(
        file, wall, "Pset_CSmoke", NULL, NULL, NULL, NULL, &pset), "pset_add_pset succeeds");
    ASSERT(pset != NULL, "pset is non-NULL");
    assert_instance_is(pset, "IfcPropertySet", "created instance is IfcPropertySet");

    void* props = NULL;
    ASSERT(ifcopenshell_pset_props_new(&props), "props_new succeeds");
    ASSERT(props != NULL, "props is non-NULL");
    ASSERT(ifcopenshell_pset_props_set_string(props, "Reference", "ABC"), "props string succeeds");
    ASSERT(ifcopenshell_pset_props_set_double(props, "Height", 3.25), "props double succeeds");
    ASSERT(ifcopenshell_pset_props_set_bool(props, "IsExternal", true), "props bool succeeds");

    bool ok = false;
    ASSERT(ifcopenshell_pset_edit_pset(
        file, pset, "Pset_CSmokeEdited", props, NULL, true, &ok), "pset_edit_pset call succeeds");
    ASSERT(ok, "pset_edit_pset returns true");
    ASSERT(ifcopenshell_pset_props_free(props), "props_free succeeds");

    assert_string_argument(pset, 2, "Pset_CSmokeEdited", "pset name was edited");
    ifcopenshell_parse_instance_list_t* properties = instance_list_argument(pset, 4);
    ASSERT(list_size(properties) == 3, "pset has three properties");

    ifcopenshell_instance_t* prop = find_named_instance(properties, "Reference");
    ASSERT(prop != NULL, "Reference property exists");
    if (prop) {
        assert_instance_is(prop, "IfcPropertySingleValue", "Reference is single value");
        assert_nominal_string(prop, "ABC");
        ifcopenshell_instance_destroy(prop);
    }

    prop = find_named_instance(properties, "Height");
    ASSERT(prop != NULL, "Height property exists");
    if (prop) {
        assert_nominal_double(prop, 3.25);
        ifcopenshell_instance_destroy(prop);
    }

    prop = find_named_instance(properties, "IsExternal");
    ASSERT(prop != NULL, "IsExternal property exists");
    if (prop) {
        assert_nominal_bool(prop, true);
        ifcopenshell_instance_destroy(prop);
    }

    ifcopenshell_parse_instance_list_destroy(properties);
    ifcopenshell_instance_destroy(pset);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_file_destroy(file);

    printf("  Pset add/edit C ABI tests done.\n\n");
}

static void test_qto_add_and_edit(void) {
    printf("=== Qto add/edit C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &wall), "create wall succeeds");
    ASSERT(wall != NULL, "wall is non-NULL");

    ifcopenshell_instance_t* qto = NULL;
    ASSERT(ifcopenshell_pset_add_qto(file, wall, "Qto_CSmoke", NULL, NULL, NULL, &qto),
           "pset_add_qto succeeds");
    ASSERT(qto != NULL, "qto is non-NULL");
    assert_instance_is(qto, "IfcElementQuantity", "created instance is IfcElementQuantity");

    void* props = NULL;
    ASSERT(ifcopenshell_pset_props_new(&props), "props_new succeeds");
    ASSERT(props != NULL, "props is non-NULL");
    ASSERT(ifcopenshell_pset_props_set_double(props, "Length", 12.5), "qto length prop succeeds");

    bool ok = false;
    ASSERT(ifcopenshell_pset_edit_qto(file, qto, "Qto_CSmokeEdited", props, NULL, &ok),
           "pset_edit_qto call succeeds");
    ASSERT(ok, "pset_edit_qto returns true");
    ASSERT(ifcopenshell_pset_props_free(props), "props_free succeeds");

    assert_string_argument(qto, 2, "Qto_CSmokeEdited", "qto name was edited");
    ifcopenshell_parse_instance_list_t* quantities = instance_list_argument(qto, 5);
    ASSERT(list_size(quantities) == 1, "qto has one quantity");

    ifcopenshell_instance_t* quantity = find_named_instance(quantities, "Length");
    ASSERT(quantity != NULL, "Length quantity exists");
    if (quantity) {
        assert_instance_is(quantity, "IfcQuantityLength", "Length is IfcQuantityLength");
        ifcopenshell_parse_attribute_value_t* value = argument(quantity, 3);
        double result = 0.0;
        ASSERT(ifcopenshell_parse_attribute_value_as_double(value, &result), "LengthValue read succeeds");
        ASSERT(fabs(result - 12.5) < 1.e-9, "LengthValue matches");
        ifcopenshell_parse_attribute_value_destroy(value);
        ifcopenshell_instance_destroy(quantity);
    }

    ifcopenshell_parse_instance_list_destroy(quantities);
    ifcopenshell_instance_destroy(qto);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_file_destroy(file);

    printf("  Qto add/edit C ABI tests done.\n\n");
}

static void test_pset_assign_unassign_and_unshare(void) {
    printf("=== Pset assign/unassign/unshare C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* walls[3] = {0};
    for (size_t i = 0; i < 3; ++i) {
        ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &walls[i]), "create wall succeeds");
        ASSERT(walls[i] != NULL, "wall is non-NULL");
    }

    ifcopenshell_instance_t* pset = NULL;
    ASSERT(ifcopenshell_pset_add_pset(file, walls[0], "Pset_AssignSmoke", NULL, NULL, NULL, NULL, &pset),
           "pset_add_pset succeeds");
    ASSERT(pset != NULL, "pset is non-NULL");

    ifcopenshell_instance_t* assign_items[2] = {walls[1], walls[2]};
    ifcopenshell_instance_list_t assign_list = make_instance_list(assign_items, 2);
    ifcopenshell_instance_t* rel = NULL;
    ASSERT(ifcopenshell_pset_assign_pset(file, &assign_list, pset, NULL, NULL, NULL, &rel),
           "pset_assign_pset succeeds");
    ASSERT(rel != NULL, "pset_assign_pset returns rel for occurrences");
    ifcopenshell_parse_instance_list_t* related = instance_list_argument(rel, 4);
    ASSERT(list_size(related) == 3, "assignment merged related objects");
    ifcopenshell_parse_instance_list_destroy(related);

    ifcopenshell_instance_t* unassign_items[1] = {walls[2]};
    ifcopenshell_instance_list_t unassign_list = make_instance_list(unassign_items, 1);
    ASSERT(ifcopenshell_pset_unassign_pset(file, &unassign_list, pset), "pset_unassign_pset succeeds");
    related = instance_list_argument(rel, 4);
    ASSERT(list_size(related) == 2, "unassignment retained non-selected occurrences");
    ifcopenshell_parse_instance_list_destroy(related);
    ifcopenshell_instance_destroy(rel);

    ASSERT(ifcopenshell_pset_assign_pset(file, &unassign_list, pset, NULL, NULL, NULL, &rel),
           "pset_assign_pset reassigns wall");
    ASSERT(rel != NULL, "reassignment returns rel");
    ifcopenshell_instance_destroy(rel);

    ifcopenshell_instance_list_t unshare_list = make_instance_list(assign_items, 2);
    ifcopenshell_instance_list_t copied = {0};
    ASSERT(ifcopenshell_pset_unshare_pset(file, &unshare_list, pset, NULL, NULL, NULL, &copied),
           "pset_unshare_pset succeeds");
    ASSERT(copied.size == 2, "unshare returns one copied pset per selected product");
    ASSERT(owned_list_get(&copied, 0) != NULL, "first copied pset exists");
    ASSERT(owned_list_get(&copied, 1) != NULL, "second copied pset exists");

    ifcopenshell_instance_list_t psets = by_type(file, "IfcPropertySet");
    ASSERT(psets.size == 3, "unshare leaves three property sets");
    ifcopenshell_instance_list_destroy(&psets);
    ifcopenshell_instance_list_t rels = by_type(file, "IfcRelDefinesByProperties");
    ASSERT(rels.size == 3, "unshare leaves one rel per pset");
    ifcopenshell_instance_list_destroy(&rels);
    ifcopenshell_instance_list_destroy(&copied);

    for (size_t i = 0; i < 3; ++i) {
        ifcopenshell_instance_destroy(walls[i]);
    }
    ifcopenshell_instance_destroy(pset);
    ifcopenshell_file_destroy(file);

    printf("  Pset assign/unassign/unshare C ABI tests done.\n\n");
}

static void test_pset_type_assignment_and_remove(void) {
    printf("=== Pset type assignment/remove C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* wall_type = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWallType", &wall_type), "create wall type succeeds");
    ASSERT(wall_type != NULL, "wall type is non-NULL");

    ifcopenshell_instance_t* pset = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcPropertySet", &pset), "create pset succeeds");
    ASSERT(pset != NULL, "pset is non-NULL");

    ifcopenshell_instance_t* type_items[1] = {wall_type};
    ifcopenshell_instance_list_t type_list = make_instance_list(type_items, 1);
    ifcopenshell_instance_t* rel = NULL;
    ASSERT(ifcopenshell_pset_assign_pset(file, &type_list, pset, NULL, NULL, NULL, &rel),
           "type-only pset_assign_pset succeeds");
    ASSERT(rel != NULL, "type-only assignment returns a nullable handle wrapper");
    ifcopenshell_instance_destroy(rel);
    ifcopenshell_instance_list_t type_rels = by_type(file, "IfcRelDefinesByProperties");
    ASSERT(type_rels.size == 0, "type-only assignment creates no occurrence relation");
    ifcopenshell_instance_list_destroy(&type_rels);

    ifcopenshell_parse_instance_list_t* type_psets = instance_list_argument(wall_type, 5);
    ASSERT(list_size(type_psets) == 1, "type assignment added one HasPropertySets item");
    ifcopenshell_instance_t* assigned = list_get(type_psets, 0);
    ASSERT(same_instance(assigned, pset), "type assignment points to pset");
    ifcopenshell_instance_destroy(assigned);
    ifcopenshell_parse_instance_list_destroy(type_psets);

    ASSERT(ifcopenshell_pset_unassign_pset(file, &type_list, pset), "type pset_unassign_pset succeeds");
    ifcopenshell_parse_attribute_value_t* attr = argument(wall_type, 5);
    bool is_null = false;
    ASSERT(ifcopenshell_parse_attribute_value_is_null(attr, &is_null), "null attribute check succeeds");
    ASSERT(is_null, "type unassignment clears HasPropertySets");
    ifcopenshell_parse_attribute_value_destroy(attr);

    ifcopenshell_instance_destroy(pset);
    ifcopenshell_instance_destroy(wall_type);
    ifcopenshell_file_destroy(file);

    file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file for remove succeeds");
    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &wall), "create wall for remove succeeds");
    ASSERT(ifcopenshell_pset_add_pset(file, wall, "Pset_RemoveSmoke", NULL, NULL, NULL, NULL, &pset),
           "pset_add_pset for remove succeeds");
    void* props = NULL;
    ASSERT(ifcopenshell_pset_props_new(&props), "remove props_new succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(props, "Reference", "ABC"), "remove props string succeeds");
    bool ok = false;
    ASSERT(ifcopenshell_pset_edit_pset(file, pset, NULL, props, NULL, true, &ok), "edit pset for remove succeeds");
    ASSERT(ok, "edit pset for remove returns true");
    ASSERT(ifcopenshell_pset_props_free(props), "remove props_free succeeds");
    ASSERT(ifcopenshell_pset_remove_pset(file, wall, pset), "pset_remove_pset succeeds");
    ifcopenshell_instance_list_t psets = by_type(file, "IfcPropertySet");
    ASSERT(psets.size == 0, "remove pset purges pset");
    ifcopenshell_instance_list_destroy(&psets);
    ifcopenshell_instance_list_t props_left = by_type(file, "IfcPropertySingleValue");
    ASSERT(props_left.size == 0, "remove pset purges owned properties");
    ifcopenshell_instance_list_destroy(&props_left);

    ifcopenshell_instance_destroy(pset);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_file_destroy(file);

    printf("  Pset type assignment/remove C ABI tests done.\n\n");
}

static void test_group_remove_with_pset(void) {
    printf("=== Group remove C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* wall = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcWall", &wall), "create wall succeeds");
    ASSERT(wall != NULL, "wall is non-NULL");

    ifcopenshell_instance_t* group = NULL;
    ASSERT(ifcopenshell_group_add_group(file, "Group_CSmoke", NULL, NULL, NULL, NULL, &group),
           "group_add_group succeeds");
    ASSERT(group != NULL, "group is non-NULL");

    ifcopenshell_instance_t* group_items[1] = {wall};
    ifcopenshell_instance_list_t group_list = make_instance_list(group_items, 1);
    ifcopenshell_instance_t* group_rel = NULL;
    ASSERT(ifcopenshell_group_assign_group(file, &group_list, group, NULL, NULL, NULL, &group_rel),
           "group_assign_group succeeds");
    ASSERT(group_rel != NULL, "group relation is non-NULL");

    ifcopenshell_instance_t* pset = NULL;
    ASSERT(ifcopenshell_pset_add_pset(file, group, "Pset_GroupSmoke", NULL, NULL, NULL, NULL, &pset),
           "group pset_add_pset succeeds");
    void* props = NULL;
    ASSERT(ifcopenshell_pset_props_new(&props), "group pset props_new succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(props, "Reference", "ABC"), "group pset prop succeeds");
    bool ok = false;
    ASSERT(ifcopenshell_pset_edit_pset(file, pset, NULL, props, NULL, true, &ok), "group pset edit succeeds");
    ASSERT(ok, "group pset edit returns true");
    ASSERT(ifcopenshell_pset_props_free(props), "group pset props_free succeeds");

    ASSERT(ifcopenshell_group_remove_group(file, group), "group_remove_group succeeds");

    ifcopenshell_instance_list_t groups = by_type(file, "IfcGroup");
    ASSERT(groups.size == 0, "group removal purges group");
    ifcopenshell_instance_list_destroy(&groups);
    ifcopenshell_instance_list_t group_rels = by_type(file, "IfcRelAssignsToGroup");
    ASSERT(group_rels.size == 0, "group removal purges group relationships");
    ifcopenshell_instance_list_destroy(&group_rels);
    ifcopenshell_instance_list_t psets = by_type(file, "IfcPropertySet");
    ASSERT(psets.size == 0, "group removal purges group psets");
    ifcopenshell_instance_list_destroy(&psets);
    ifcopenshell_instance_list_t props_left = by_type(file, "IfcPropertySingleValue");
    ASSERT(props_left.size == 0, "group removal purges group pset properties");
    ifcopenshell_instance_list_destroy(&props_left);

    ifcopenshell_instance_destroy(pset);
    ifcopenshell_instance_destroy(group_rel);
    ifcopenshell_instance_destroy(group);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_file_destroy(file);

    printf("  Group remove C ABI tests done.\n\n");
}

int ifcapi_run_pset_smoke_tests(void) {
    printf("ifcapi pset C ABI smoke tests\n\n");

    test_pset_add_and_edit();
    test_qto_add_and_edit();
    test_pset_assign_unassign_and_unshare();
    test_pset_type_assignment_and_remove();
    test_group_remove_with_pset();

    printf("=== Results: %d/%d passed ===\n", tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
