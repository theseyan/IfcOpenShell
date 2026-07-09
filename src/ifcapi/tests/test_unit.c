// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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

static ifcopenshell_instance_list_t make_instance_list(ifcopenshell_instance_t** items, size_t size) {
    ifcopenshell_instance_list_t result;
    result.items = items;
    result.size = size;
    return result;
}

static size_t list_size(ifcopenshell_parse_instance_list_t* list) {
    size_t result = 0;
    ASSERT(ifcopenshell_parse_instance_list_size(list, &result), "instance list size succeeds");
    return result;
}

static ifcopenshell_parse_instance_list_t* by_type(ifcopenshell_file_t* file, const char* ifc_class) {
    ifcopenshell_parse_instance_list_t* result = NULL;
    ASSERT(ifcopenshell_file_by_type(file, ifc_class, &result), "by_type succeeds");
    return result;
}

static ifcopenshell_parse_instance_list_t* instance_list_argument(ifcopenshell_instance_t* inst, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = NULL;
    ASSERT(ifcopenshell_instance_get_argument(inst, index, &value), "get_argument succeeds");
    ASSERT(value != NULL, "get_argument returns non-NULL");
    ifcopenshell_parse_instance_list_t* result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(value, &result), "argument as instance list succeeds");
    ASSERT(result != NULL, "instance list argument is non-NULL");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static void test_unit_creation(void) {
    printf("=== Unit creation C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* length = NULL;
    ASSERT(ifcopenshell_unit_add_si_unit(file, "LENGTHUNIT", "MILLI", &length), "unit_add_si_unit succeeds");
    ASSERT(length != NULL, "SI unit is non-NULL");
    assert_instance_is(length, "IfcSIUnit", "created SI unit is IfcSIUnit");

    ifcopenshell_instance_t* currency = NULL;
    ASSERT(ifcopenshell_unit_add_monetary_unit(file, "USD", &currency), "unit_add_monetary_unit succeeds");
    ASSERT(currency != NULL, "monetary unit is non-NULL");
    assert_instance_is(currency, "IfcMonetaryUnit", "created monetary unit is IfcMonetaryUnit");

    int64_t dimension_values[7] = {1, 0, -1, 0, 0, 0, 0};
    ifcopenshell_int64_list_t dimensions = {dimension_values, 7};
    ifcopenshell_instance_t* context = NULL;
    ASSERT(ifcopenshell_unit_add_context_dependent_unit(
        file, "USERDEFINED", "BOXES", &dimensions, &context), "unit_add_context_dependent_unit succeeds");
    ASSERT(context != NULL, "context dependent unit is non-NULL");
    assert_instance_is(context, "IfcContextDependentUnit", "created context unit is IfcContextDependentUnit");

    ifcopenshell_instance_t* time = NULL;
    ASSERT(ifcopenshell_unit_add_si_unit(file, "TIMEUNIT", NULL, &time), "time unit add succeeds");
    ifcopenshell_instance_t* derived_units[2] = {length, time};
    ifcopenshell_instance_list_t unit_list = make_instance_list(derived_units, 2);
    int64_t exponent_values[2] = {1, -1};
    ifcopenshell_int64_list_t exponents = {exponent_values, 2};
    ifcopenshell_instance_t* derived = NULL;
    ASSERT(ifcopenshell_unit_add_derived_unit(
        file, "LINEARVELOCITYUNIT", NULL, &unit_list, &exponents, &derived), "unit_add_derived_unit succeeds");
    ASSERT(derived != NULL, "derived unit is non-NULL");
    assert_instance_is(derived, "IfcDerivedUnit", "created derived unit is IfcDerivedUnit");

    ifcopenshell_parse_instance_list_t* derived_elements = by_type(file, "IfcDerivedUnitElement");
    ASSERT(list_size(derived_elements) == 2, "derived unit creates one element per component unit");
    ifcopenshell_parse_instance_list_destroy(derived_elements);

    ifcopenshell_instance_destroy(derived);
    ifcopenshell_instance_destroy(time);
    ifcopenshell_instance_destroy(context);
    ifcopenshell_instance_destroy(currency);
    ifcopenshell_instance_destroy(length);
    ifcopenshell_file_destroy(file);

    printf("  Unit creation C ABI tests done.\n\n");
}

static void test_unit_unassign_and_remove(void) {
    printf("=== Unit unassign/remove C ABI tests ===\n");

    ifcopenshell_file_t* file = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(file != NULL, "file is non-NULL");

    ifcopenshell_instance_t* length = NULL;
    ASSERT(ifcopenshell_unit_add_si_unit(file, "LENGTHUNIT", "MILLI", &length), "length unit add succeeds");
    ifcopenshell_instance_t* area = NULL;
    ASSERT(ifcopenshell_unit_add_si_unit(file, "AREAUNIT", NULL, &area), "area unit add succeeds");

    ifcopenshell_instance_t* project = NULL;
    ifcopenshell_root_create_entity_options_t project_options = {0};
    project_options.ifc_class = "IfcProject";
    project_options.name = "C Smoke Project";
    project_options.has_name = true;
    ASSERT(ifcopenshell_root_create_entity(file, &project_options, &project), "create project succeeds");
    ASSERT(project != NULL, "project is non-NULL");

    ifcopenshell_instance_t* assignment = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, "IfcUnitAssignment", &assignment),
           "create unit assignment succeeds");
    ASSERT(ifcopenshell_instance_set_argument_instance(project, 8, assignment),
           "set project UnitsInContext succeeds");
    ifcopenshell_instance_t* assigned_units[2] = {length, area};
    ifcopenshell_instance_list_t assigned_unit_list = make_instance_list(assigned_units, 2);
    ifcopenshell_parse_instance_list_t* assigned_parse_list = NULL;
    ASSERT(ifcopenshell_parse_instance_list_create_from_handles(&assigned_unit_list, &assigned_parse_list),
           "create assignment unit list succeeds");
    ASSERT(ifcopenshell_instance_set_argument_instance_list(assignment, 0, assigned_parse_list),
           "set unit assignment units succeeds");
    ifcopenshell_parse_instance_list_destroy(assigned_parse_list);

    ifcopenshell_instance_t* unassign_units[1] = {length};
    ifcopenshell_instance_list_t unassign_list = make_instance_list(unassign_units, 1);
    ASSERT(ifcopenshell_unit_unassign_unit(file, &unassign_list), "unit_unassign_unit succeeds");
    ifcopenshell_parse_instance_list_t* kept_units = instance_list_argument(assignment, 0);
    ASSERT(list_size(kept_units) == 1, "unit unassignment keeps non-selected unit");
    ifcopenshell_parse_instance_list_destroy(kept_units);

    ASSERT(ifcopenshell_unit_remove_unit(file, area), "unit_remove_unit succeeds");
    ifcopenshell_parse_instance_list_t* assignments = by_type(file, "IfcUnitAssignment");
    ASSERT(list_size(assignments) == 0, "removing last assigned unit removes assignment");
    ifcopenshell_parse_instance_list_destroy(assignments);
    ifcopenshell_parse_instance_list_t* si_units = by_type(file, "IfcSIUnit");
    ASSERT(list_size(si_units) == 1, "unit_remove_unit purges only the selected unit");
    ifcopenshell_parse_instance_list_destroy(si_units);

    ifcopenshell_instance_destroy(assignment);
    ifcopenshell_instance_destroy(project);
    ifcopenshell_instance_destroy(area);
    ifcopenshell_instance_destroy(length);
    ifcopenshell_file_destroy(file);

    printf("  Unit unassign/remove C ABI tests done.\n\n");
}

int ifcapi_run_unit_smoke_tests(void) {
    printf("ifcapi unit C ABI smoke tests\n\n");

    test_unit_creation();
    test_unit_unassign_and_remove();

    printf("=== Results: %d/%d passed ===\n", tests_run - tests_failed, tests_run);

    return tests_failed > 0 ? 1 : 0;
}
