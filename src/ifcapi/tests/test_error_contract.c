// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;
#define ASSERT(c, m) do { tests_run++; if (!(c)) { fprintf(stderr, "FAIL [%s:%d]: %s (%s)\n", __FILE__, __LINE__, m, ifcopenshell_last_error_message()); tests_failed++; } } while (0)

static void test_clear_and_value_error(void) {
    double bearing = 0.0;

    ASSERT(!ifcopenshell_cogo_bearing2dd("N invalid E", &bearing), "invalid bearing fails");
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_VALUE, "invalid bearing is a value error");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_INVALID_QUADRANT_BEARING,
           "invalid bearing has a stable code");
    ASSERT(ifcopenshell_last_error_message()[0] != '\0', "invalid bearing retains a diagnostic");

    ifcopenshell_clear_error();
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_NONE, "clear resets kind");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_NONE, "clear resets code");
    ASSERT(ifcopenshell_last_error_message()[0] == '\0', "clear resets message");

    ASSERT(ifcopenshell_cogo_bearing2dd("N 45 E", &bearing), "valid bearing succeeds");
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_NONE, "success does not expose stale kind");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_NONE, "success does not expose stale code");
}

static void test_recursion_error(void) {
    ifcopenshell_file_t* file = NULL;
    ifcopenshell_instance_t *first = NULL, *second = NULL, *first_time = NULL, *second_time = NULL;
    ifcopenshell_instance_t *forward = NULL, *cycle = NULL;
    ifcopenshell_string_t missing = {0};
    ifcopenshell_sequence_add_task_options_t task_options = {0};
    ifcopenshell_sequence_add_task_time_options_t time_options = {0};
    ifcopenshell_sequence_assign_sequence_options_t sequence_options = {0};

    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "create file for cycle test");
    ASSERT(ifcopenshell_selector_format(NULL, NULL, "{{undefined}}", &missing),
           "undefined selector value is a successful nullable result");
    ASSERT(missing.data == NULL, "nullable selector value returns no result");
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_NONE, "nullable absence is not an error");
    ifcopenshell_string_destroy(&missing);
    ASSERT(ifcopenshell_sequence_add_task(file, &task_options, &first), "create first task");
    ASSERT(ifcopenshell_sequence_add_task(file, &task_options, &second), "create second task");
    ASSERT(ifcopenshell_sequence_add_task_time(file, first, &time_options, &first_time), "create first task time");
    ASSERT(ifcopenshell_sequence_add_task_time(file, second, &time_options, &second_time), "create second task time");
    ASSERT(ifcopenshell_sequence_assign_sequence(file, first, second, &sequence_options, &forward), "create forward edge");
    ASSERT(!ifcopenshell_sequence_assign_sequence(file, second, first, &sequence_options, &cycle), "reject cyclic edge");
    ASSERT(cycle == NULL, "cyclic edge returns no result");
    ASSERT(ifcopenshell_last_error_kind() == IFCOPENSHELL_ERROR_RECURSION, "cycle is a recursion error");
    ASSERT(ifcopenshell_last_error_code() == IFCOPENSHELL_ERROR_CODE_RECURSIVE_SCHEDULE_CASCADE,
           "cycle has the recursive cascade code");
    ASSERT(ifcopenshell_last_error_message()[0] != '\0', "cycle retains a diagnostic");

    ifcopenshell_instance_destroy(forward);
    ifcopenshell_instance_destroy(second_time);
    ifcopenshell_instance_destroy(first_time);
    ifcopenshell_instance_destroy(second);
    ifcopenshell_instance_destroy(first);
    ifcopenshell_file_destroy(file);
}

int ifcapi_run_error_contract_smoke_tests(void) {
    printf("=== Typed error contract C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_clear_and_value_error();
    test_recursion_error();
    printf("Typed error contract C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
