// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;

#define ASSERT(c, m)                                                                                                   \
    do {                                                                                                               \
        tests_run++;                                                                                                   \
        if (!(c)) {                                                                                                    \
            fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, m);                                            \
            tests_failed++;                                                                                            \
        }                                                                                                              \
    } while (0)

static ifcopenshell_parse_attribute_value_t* argument(ifcopenshell_instance_t* instance, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = NULL;
    ASSERT(ifcopenshell_instance_get_argument(instance, index, &value), "get argument succeeds");
    ASSERT(value != NULL, "argument is non-null");
    return value;
}

static ifcopenshell_instance_t* instance_argument(ifcopenshell_instance_t* instance, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = argument(instance, index);
    ifcopenshell_instance_t* result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance(value, &result), "argument is an instance");
    ASSERT(result != NULL, "instance argument is non-null");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static ifcopenshell_parse_instance_list_t* instance_list_argument(ifcopenshell_instance_t* instance, size_t index) {
    ifcopenshell_parse_attribute_value_t* value = argument(instance, index);
    ifcopenshell_parse_instance_list_t* result = NULL;
    ASSERT(ifcopenshell_parse_attribute_value_as_instance_list(value, &result), "argument is an instance list");
    ASSERT(result != NULL, "instance list is non-null");
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static size_t list_size(ifcopenshell_parse_instance_list_t* list) {
    size_t result = 0;
    ASSERT(ifcopenshell_parse_instance_list_size(list, &result), "list size succeeds");
    return result;
}

static ifcopenshell_instance_t* list_get(ifcopenshell_parse_instance_list_t* list, size_t index) {
    ifcopenshell_instance_t* result = NULL;
    ASSERT(ifcopenshell_parse_instance_list_get(list, index, &result), "list get succeeds");
    ASSERT(result != NULL, "list item is non-null");
    return result;
}

static uint32_t instance_id(ifcopenshell_instance_t* instance) {
    uint32_t result = 0;
    ASSERT(ifcopenshell_instance_id(instance, &result), "instance id succeeds");
    return result;
}

static void assert_type(ifcopenshell_instance_t* instance, const char* expected) {
    bool result = false;
    ASSERT(ifcopenshell_instance_is_a(instance, expected, &result), "type query succeeds");
    ASSERT(result, "instance has expected type");
}

static void assert_string_argument(ifcopenshell_instance_t* instance, size_t index, const char* expected) {
    ifcopenshell_parse_attribute_value_t* value = argument(instance, index);
    ifcopenshell_string_t actual = {0};
    ASSERT(ifcopenshell_parse_attribute_value_as_string(value, &actual), "argument is a string");
    ASSERT(actual.size == strlen(expected), "string size matches");
    ASSERT(actual.data != NULL && strncmp(actual.data, expected, actual.size) == 0, "string value matches");
    ifcopenshell_string_destroy(&actual);
    ifcopenshell_parse_attribute_value_destroy(value);
}

static void assert_int_argument(ifcopenshell_instance_t* instance, size_t index, int32_t expected) {
    ifcopenshell_parse_attribute_value_t* value = argument(instance, index);
    int32_t actual = 0;
    ASSERT(ifcopenshell_parse_attribute_value_as_int32(value, &actual), "argument is an integer");
    ASSERT(actual == expected, "integer value matches");
    ifcopenshell_parse_attribute_value_destroy(value);
}

static ifcopenshell_string_t borrowed_string(const char* value) {
    ifcopenshell_string_t result = {(char*)value, strlen(value), false, NULL};
    return result;
}

static void test_bearing(void) {
    double result = -1.0;
    ASSERT(ifcopenshell_cogo_bearing2dd("  n\t45 30 0 e  ", &result), "bearing conversion succeeds");
    ASSERT(fabs(result - 44.5) < 1e-12, "bearing result is decimal degrees");
    ASSERT(ifcopenshell_cogo_bearing2dd("S 90 E", &result), "south 90 east succeeds");
    ASSERT(result == 0.0, "south 90 east normalizes to zero");
    ASSERT(!ifcopenshell_cogo_bearing2dd("N 45x E", &result), "trailing numeric junk is rejected");
    ASSERT(ifcopenshell_last_error_kind() != 0, "invalid bearing reports an error");
    ASSERT(strcmp(ifcopenshell_last_error_message(), "Invalid bearing string") == 0, "bearing error text matches");
}

static void test_property_template(void) {
    ifcopenshell_file_t *file = NULL, *other_file = NULL;
    ifcopenshell_instance_t *pset_template = NULL, *prop_template = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &other_file), "second IFC4 file succeeds");
    ASSERT(ifcopenshell_pset_template_add_pset_template(
               file, "Pset_Test", "PSET_TYPEDRIVENOVERRIDE", "IfcWall", &pset_template),
        "property set template creation succeeds");
    ASSERT(ifcopenshell_pset_template_add_prop_template(
               file, pset_template, "Status", NULL, "P_ENUMERATEDVALUE", "IfcLabel", &prop_template),
        "property template creation succeeds");

    void* attributes = NULL;
    ifcopenshell_string_t label_items[] = {borrowed_string("NEW"), borrowed_string("EXISTING")};
    ifcopenshell_string_list_t labels = {label_items, 2, NULL};
    ASSERT(ifcopenshell_pset_props_new(&attributes), "property map creation succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(attributes, "Name", "Status"), "name property succeeds");
    ASSERT(ifcopenshell_pset_props_set_string_list(attributes, "Enumerators", &labels), "label list property succeeds");
    ifcopenshell_pset_template_edit_prop_template_options_t options = {prop_template, attributes};
    ASSERT(ifcopenshell_pset_template_edit_prop_template(file, &options), "enumeration creation succeeds");
    ASSERT(ifcopenshell_pset_props_free(attributes), "property map free succeeds");

    ifcopenshell_instance_t* enumeration = instance_argument(prop_template, 7);
    assert_type(enumeration, "IfcPropertyEnumeration");
    assert_string_argument(enumeration, 0, "Status");
    ifcopenshell_parse_instance_list_t* values = instance_list_argument(enumeration, 1);
    ASSERT(list_size(values) == 2, "two label enumerators are stored");
    ifcopenshell_instance_t* first = list_get(values, 0);
    assert_type(first, "IfcLabel");
    assert_string_argument(first, 0, "NEW");
    ifcopenshell_instance_destroy(first);
    ifcopenshell_parse_instance_list_destroy(values);

    int64_t integer_items[] = {1, 3, 5};
    ifcopenshell_int64_list_t integers = {integer_items, 3, NULL};
    attributes = NULL;
    ASSERT(ifcopenshell_pset_props_new(&attributes), "second property map creation succeeds");
    ASSERT(ifcopenshell_pset_props_set_string(attributes, "PrimaryMeasureType", "IfcInteger"),
        "incoming measure type succeeds");
    ASSERT(ifcopenshell_pset_props_set_int_list(attributes, "Enumerators", &integers), "integer list property succeeds");
    options.attributes = attributes;
    ASSERT(ifcopenshell_pset_template_edit_prop_template(file, &options), "enumeration replacement succeeds");
    ASSERT(ifcopenshell_pset_props_free(attributes), "second property map free succeeds");
    ifcopenshell_instance_t* reused = instance_argument(prop_template, 7);
    ASSERT(instance_id(reused) == instance_id(enumeration), "existing enumeration is reused");
    values = instance_list_argument(reused, 1);
    ASSERT(list_size(values) == 3, "three integer enumerators are stored");
    first = list_get(values, 0);
    assert_type(first, "IfcInteger");
    assert_int_argument(first, 0, 1);
    ifcopenshell_instance_destroy(first);
    ifcopenshell_parse_instance_list_destroy(values);

    attributes = NULL;
    ASSERT(ifcopenshell_pset_props_new(&attributes), "validation property map creation succeeds");
    options.attributes = attributes;
    ASSERT(!ifcopenshell_pset_template_edit_prop_template(other_file, &options), "wrong-file property is rejected");
    options.prop_template = NULL;
    ASSERT(!ifcopenshell_pset_template_edit_prop_template(file, &options), "null property is rejected");
    ASSERT(ifcopenshell_pset_props_free(attributes), "validation property map free succeeds");

    ifcopenshell_instance_destroy(reused);
    ifcopenshell_instance_destroy(enumeration);
    ifcopenshell_instance_destroy(prop_template);
    ifcopenshell_instance_destroy(pset_template);
    ifcopenshell_file_destroy(other_file);
    ifcopenshell_file_destroy(file);
}

static void test_library_date(void) {
    ifcopenshell_file_t *ifc4 = NULL, *ifc2x3 = NULL, *other = NULL;
    ifcopenshell_instance_t *library4 = NULL, *library2x3 = NULL, *wrong_type = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &ifc4), "new IFC4 library file succeeds");
    ASSERT(ifcopenshell_parse_new_file("IFC2X3", 0, "", &ifc2x3), "new IFC2X3 library file succeeds");
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &other), "new ownership test file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(ifc4, "IfcLibraryInformation", &library4),
        "IFC4 library creation succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(ifc2x3, "IfcLibraryInformation", &library2x3),
        "IFC2X3 library creation succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(ifc4, "IfcWall", &wrong_type), "wrong target creation succeeds");

    const char* date_time = "2026-07-15T12:34:56.123456+05:30";
    ASSERT(ifcopenshell_library_edit_version_date(ifc4, library4, date_time), "IFC4 date normalization succeeds");
    assert_string_argument(library4, 3, date_time);
    ASSERT(ifcopenshell_library_edit_version_date(ifc2x3, library2x3, date_time), "IFC2X3 date normalization succeeds");
    ifcopenshell_instance_t* calendar_date = instance_argument(library2x3, 3);
    assert_type(calendar_date, "IfcCalendarDate");
    assert_int_argument(calendar_date, 0, 15);
    assert_int_argument(calendar_date, 1, 7);
    assert_int_argument(calendar_date, 2, 2026);

    ASSERT(!ifcopenshell_library_edit_version_date(other, library4, date_time), "wrong-file library is rejected");
    ASSERT(!ifcopenshell_library_edit_version_date(ifc4, wrong_type, date_time), "wrong entity type is rejected");
    ASSERT(!ifcopenshell_library_edit_version_date(ifc4, NULL, date_time), "null library is rejected");
    ASSERT(!ifcopenshell_library_edit_version_date(ifc4, library4, "2026-02-30T00:00:00"),
        "invalid calendar date is rejected");

    ifcopenshell_instance_destroy(calendar_date);
    ifcopenshell_instance_destroy(wrong_type);
    ifcopenshell_instance_destroy(library2x3);
    ifcopenshell_instance_destroy(library4);
    ifcopenshell_file_destroy(other);
    ifcopenshell_file_destroy(ifc2x3);
    ifcopenshell_file_destroy(ifc4);
}

int ifcapi_run_pset_template_cogo_library_smoke_tests(void) {
    printf("=== Pset template, Cogo, and Library C ABI tests ===\n");
    tests_run = tests_failed = 0;
    test_bearing();
    test_property_template();
    test_library_date();
    printf("Pset template, Cogo, and Library C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
