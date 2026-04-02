/* This file was generated with the assistance of an AI coding tool. */

#include "ifcparse_api.h"

#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
    IFC_FILETYPE_ROCKSDB = 3,
    IFC_STORAGE_MODE_ROCKSDB = 1,
};

static void fail(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

static void expect_true(int condition, const char* message) {
    if (!condition) {
        fail(message);
    }
}

static void expect_ok(int ok) {
    if (!ok) {
        fail(ifcopenshell_ifcparse_last_error_message());
    }
}

static int remove_tree_entry(const char* path, const struct stat* stat_info, int type_flag, struct FTW* ftw_info) {
    (void)stat_info;
    (void)type_flag;
    (void)ftw_info;
    return remove(path);
}

static void cleanup_tree(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return;
    }
    nftw(path, remove_tree_entry, 32, FTW_DEPTH | FTW_PHYS);
}

static int string_list_contains_exact(const ifcopenshell_string_list_t* values, const char* expected) {
    size_t i;
    for (i = 0; i < values->size; ++i) {
        if (strcmp(values->items[i].data, expected) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(void) {
    char root_template[] = "/tmp/ifcopenshell_ifcparse_rocksdb_XXXXXX";
    char db_path[1024] = {0};
    char* root_dir = NULL;

    ifcopenshell_ifc_file_t* file = NULL;
    ifcopenshell_ifc_file_t* reopened = NULL;
    ifcopenshell_ifc_schema_t* schema = NULL;
    ifcopenshell_ifc_declaration_t* point_decl = NULL;
    ifcopenshell_ifc_declaration_t* polyline_decl = NULL;
    ifcopenshell_ifc_declaration_t* person_decl = NULL;
    ifcopenshell_ifc_instance_t* point_one = NULL;
    ifcopenshell_ifc_instance_t* point_two = NULL;
    ifcopenshell_ifc_instance_t* polyline = NULL;
    ifcopenshell_ifc_instance_t* person = NULL;
    ifcopenshell_ifc_instance_t* detached_point = NULL;
    ifcopenshell_ifc_instance_t* added_point = NULL;
    ifcopenshell_ifc_instance_t* reopened_point_one = NULL;
    ifcopenshell_ifc_instance_t* reopened_point_two = NULL;
    ifcopenshell_ifc_instance_t* reopened_polyline = NULL;
    ifcopenshell_ifc_instance_t* reopened_person = NULL;
    ifcopenshell_ifc_instance_t* reopened_added_point = NULL;
    ifcopenshell_ifcparse_instance_list_t* point_instances = NULL;
    ifcopenshell_ifcparse_instance_list_t* inverse_instances = NULL;
    ifcopenshell_ifcparse_attribute_value_t* point_coordinates = NULL;
    ifcopenshell_ifcparse_attribute_value_t* polyline_points = NULL;
    ifcopenshell_ifcparse_attribute_value_t* family_name_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* middle_names_argument = NULL;
    ifcopenshell_string_t schema_key_value = {0};
    ifcopenshell_string_t schema_name = {0};
    ifcopenshell_string_t family_name = {0};
    ifcopenshell_string_list_t entity_keys = {0};
    ifcopenshell_string_list_t middle_names = {0};
    ifcopenshell_double_list_t point_coordinate_values = {0};
    int32_t file_type = -1;
    int32_t storage_mode = -1;
    uint32_t instance_id = 0;
    size_t size_value = 0;

    root_dir = mkdtemp(root_template);
    expect_true(root_dir != NULL, "mkdtemp() failed");
    expect_true(snprintf(db_path, sizeof(db_path), "%s", root_dir) < (int)sizeof(db_path), "Temporary RocksDB path was truncated");

    expect_ok(ifcopenshell_ifcparse_schema_by_name("IFC4", &schema));
    expect_ok(ifcopenshell_ifcparse_new_file("IFC4", IFC_FILETYPE_ROCKSDB, db_path, &file));

    expect_ok(ifcopenshell_ifc_file_storage_mode(file, &storage_mode));
    expect_true(storage_mode == IFC_STORAGE_MODE_ROCKSDB, "New file should use RocksDB storage");
    expect_ok(ifcopenshell_ifc_file_schema_name(file, &schema_name));
    expect_true(strcmp(schema_name.data, "IFC4") == 0, "Unexpected schema name for new RocksDB file");
    ifcopenshell_string_destroy(&schema_name);

    expect_ok(ifcopenshell_ifc_file_key_value_store_query(file, "h|file_schema|0", &schema_key_value));
    expect_true(schema_key_value.size > 0u, "RocksDB file should persist schema metadata");
    ifcopenshell_string_destroy(&schema_key_value);

    expect_ok(ifcopenshell_ifc_file_key_value_store_iter(file, "h|", &entity_keys));
    expect_true(entity_keys.size > 0u, "RocksDB file should contain header keys");
    expect_true(string_list_contains_exact(&entity_keys, "h|file_schema|0"), "Expected file_schema header key");
    ifcopenshell_string_list_destroy(&entity_keys);

    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcCartesianPoint", &point_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcPolyline", &polyline_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcPerson", &person_decl));

    expect_ok(ifcopenshell_ifc_file_create(file, point_decl, &point_one));
    expect_ok(ifcopenshell_ifc_file_create(file, point_decl, &point_two));
    expect_ok(ifcopenshell_ifc_file_create(file, polyline_decl, &polyline));
    expect_ok(ifcopenshell_ifc_file_create(file, person_decl, &person));

    {
        double values_raw[] = {0.0, 0.0, 0.0};
        ifcopenshell_double_list_t values = {values_raw, 3u};
        expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(point_one, 0u, &values));
    }
    {
        double values_raw[] = {1.0, 0.0, 0.0};
        ifcopenshell_double_list_t values = {values_raw, 3u};
        expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(point_two, 0u, &values));
    }
    expect_ok(ifcopenshell_ifc_file_by_type(file, "IfcCartesianPoint", &point_instances));
    expect_ok(ifcopenshell_ifc_instance_set_argument_instance_list(polyline, 0u, point_instances));
    expect_ok(ifcopenshell_ifc_instance_set_argument_string(person, 1u, "Smith"));
    expect_ok(ifcopenshell_ifc_instance_set_argument_string_list(
        person,
        3u,
        &((ifcopenshell_string_list_t){
            (ifcopenshell_string_t[]){
                {"Alex", 4u, false},
                {"Jordan", 6u, false},
            },
            2u,
        })
    ));

    expect_ok(ifcopenshell_ifc_schema_instantiate(schema, point_decl, &detached_point));
    {
        double values_raw[] = {2.0, 2.0, 2.0};
        ifcopenshell_double_list_t values = {values_raw, 3u};
        expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(detached_point, 0u, &values));
    }
    expect_ok(ifcopenshell_ifc_file_add(file, detached_point, -1, &added_point));
    {
        double values_raw[] = {4.0, 5.0, 6.0};
        ifcopenshell_double_list_t values = {values_raw, 3u};
        expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(added_point, 0u, &values));
    }
    expect_ok(ifcopenshell_ifc_instance_id(added_point, &instance_id));
    expect_true(instance_id == 5u, "Unexpected id assigned to detached point in RocksDB file");

    ifcopenshell_ifc_file_destroy(file);
    file = NULL;

    expect_ok(ifcopenshell_ifcparse_guess_file_type(db_path, &file_type));
    expect_true(file_type == IFC_FILETYPE_ROCKSDB, "guess_file_type() should detect RocksDB");

    expect_ok(ifcopenshell_ifcparse_open(db_path, true, &reopened));
    expect_ok(ifcopenshell_ifc_file_storage_mode(reopened, &storage_mode));
    expect_true(storage_mode == IFC_STORAGE_MODE_ROCKSDB, "Reopened file should use RocksDB storage");
    expect_ok(ifcopenshell_ifc_file_schema_name(reopened, &schema_name));
    expect_true(strcmp(schema_name.data, "IFC4") == 0, "Unexpected schema name after RocksDB reopen");
    ifcopenshell_string_destroy(&schema_name);

    expect_ok(ifcopenshell_ifc_file_key_value_store_query(reopened, "h|file_schema|0", &schema_key_value));
    expect_true(schema_key_value.size > 0u, "Reopened RocksDB file should expose schema metadata");
    ifcopenshell_string_destroy(&schema_key_value);

    expect_ok(ifcopenshell_ifc_file_key_value_store_iter(reopened, "h|", &entity_keys));
    expect_true(entity_keys.size > 0u, "Reopened RocksDB file should expose header keys");
    expect_true(string_list_contains_exact(&entity_keys, "h|file_schema|0"), "Expected file_schema header key after RocksDB reopen");
    ifcopenshell_string_list_destroy(&entity_keys);

    expect_ok(ifcopenshell_ifc_file_by_id(reopened, 1, &reopened_point_one));
    expect_ok(ifcopenshell_ifc_file_by_id(reopened, 2, &reopened_point_two));
    expect_ok(ifcopenshell_ifc_file_by_id(reopened, 3, &reopened_polyline));
    expect_ok(ifcopenshell_ifc_file_by_id(reopened, 4, &reopened_person));
    expect_ok(ifcopenshell_ifc_file_by_id(reopened, 5, &reopened_added_point));

    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_point_one, 0u, &point_coordinates));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_double_list(point_coordinates, &point_coordinate_values));
    expect_true(point_coordinate_values.size == 3u, "Unexpected first reopened point coordinate size");
    expect_true(point_coordinate_values.items[0] == 0.0 && point_coordinate_values.items[1] == 0.0 && point_coordinate_values.items[2] == 0.0, "Unexpected first reopened point coordinates");
    ifcopenshell_ifcparse_attribute_value_destroy(point_coordinates);
    point_coordinates = NULL;
    ifcopenshell_double_list_destroy(&point_coordinate_values);

    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_point_two, 0u, &point_coordinates));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_double_list(point_coordinates, &point_coordinate_values));
    expect_true(point_coordinate_values.size == 3u, "Unexpected second reopened point coordinate size");
    expect_true(point_coordinate_values.items[0] == 1.0 && point_coordinate_values.items[1] == 0.0 && point_coordinate_values.items[2] == 0.0, "Unexpected second reopened point coordinates");
    ifcopenshell_ifcparse_attribute_value_destroy(point_coordinates);
    point_coordinates = NULL;
    ifcopenshell_double_list_destroy(&point_coordinate_values);

    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_added_point, 0u, &point_coordinates));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_double_list(point_coordinates, &point_coordinate_values));
    expect_true(point_coordinate_values.size == 3u, "Unexpected added reopened point coordinate size");
    expect_true(point_coordinate_values.items[0] == 4.0 && point_coordinate_values.items[1] == 5.0 && point_coordinate_values.items[2] == 6.0, "Detached addEntity mutation did not persist across RocksDB reopen");
    ifcopenshell_ifcparse_attribute_value_destroy(point_coordinates);
    point_coordinates = NULL;
    ifcopenshell_double_list_destroy(&point_coordinate_values);

    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_polyline, 0u, &polyline_points));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_instance_list(polyline_points, &point_instances));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(point_instances, &size_value));
    expect_true(size_value == 2u, "Unexpected reopened polyline point count");
    expect_ok(ifcopenshell_ifcparse_instance_list_get(point_instances, 0u, &point_one));
    expect_ok(ifcopenshell_ifcparse_instance_list_get(point_instances, 1u, &point_two));
    expect_ok(ifcopenshell_ifc_instance_id(point_one, &instance_id));
    expect_true(instance_id == 1u, "Unexpected first reopened polyline reference");
    expect_ok(ifcopenshell_ifc_instance_id(point_two, &instance_id));
    expect_true(instance_id == 2u, "Unexpected second reopened polyline reference");
    ifcopenshell_ifcparse_instance_list_destroy(point_instances);
    point_instances = NULL;
    ifcopenshell_ifcparse_attribute_value_destroy(polyline_points);
    polyline_points = NULL;

    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_person, 1u, &family_name_argument));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_string(family_name_argument, &family_name));
    expect_true(strcmp(family_name.data, "Smith") == 0, "Unexpected reopened person family name");
    ifcopenshell_string_destroy(&family_name);
    ifcopenshell_ifcparse_attribute_value_destroy(family_name_argument);
    family_name_argument = NULL;
    expect_ok(ifcopenshell_ifc_instance_get_argument(reopened_person, 3u, &middle_names_argument));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_string_list(middle_names_argument, &middle_names));
    expect_true(middle_names.size == 2u, "Unexpected reopened middle name count");
    expect_true(strcmp(middle_names.items[0].data, "Alex") == 0, "Unexpected first reopened middle name");
    expect_true(strcmp(middle_names.items[1].data, "Jordan") == 0, "Unexpected second reopened middle name");
    ifcopenshell_string_list_destroy(&middle_names);
    ifcopenshell_ifcparse_attribute_value_destroy(middle_names_argument);
    middle_names_argument = NULL;

    expect_ok(ifcopenshell_ifc_file_instances_by_reference(reopened, 1, &inverse_instances));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(inverse_instances, &size_value));
    expect_true(size_value == 1u, "Unexpected reopened inverse count for first point");
    expect_ok(ifcopenshell_ifcparse_instance_list_get(inverse_instances, 0u, &polyline));
    expect_ok(ifcopenshell_ifc_instance_id(polyline, &instance_id));
    expect_true(instance_id == 3u, "Unexpected reopened inverse target for first point");
    ifcopenshell_ifcparse_instance_list_destroy(inverse_instances);
    inverse_instances = NULL;

    ifcopenshell_ifc_file_destroy(reopened);

    cleanup_tree(root_dir);
    return 0;
}
