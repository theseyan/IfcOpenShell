/* This file was generated with the assistance of an AI coding tool. */

#include "ifcparse_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

static void expect_fail(int ok) {
    if (ok) {
        fail("Expected API call to fail");
    }
    expect_true(ifcopenshell_ifcparse_last_error_message()[0] != '\0', "Expected API call to set an error");
    ifcopenshell_ifcparse_clear_error();
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

static const char* IFC_FIXTURE =
    "ISO-10303-21;\n"
    "HEADER;\n"
    "FILE_DESCRIPTION(('ViewDefinition [CoordinationView]'),'2;1');\n"
    "FILE_NAME('test.ifc','2024-01-01T00:00:00',('Codex'),('OpenAI'),'IfcOpenShell','IfcOpenShell','');\n"
    "FILE_SCHEMA(('IFC4'));\n"
    "ENDSEC;\n"
    "DATA;\n"
    "#1=IFCCARTESIANPOINT((0.,0.,0.));\n"
    "#2=IFCCARTESIANPOINT((1.,0.,0.));\n"
    "#3=IFCPOLYLINE((#1,#2));\n"
    "#4=IFCPERSON($,'Doe','John',('Quincy','Ray'),$,$,$,$);\n"
    "#5=IFCCOLOURRGB($,0.1,0.2,0.3);\n"
    "#6=IFCINDEXEDPOLYGONALFACE((1,2,3));\n"
    "#7=IFCTEXTUREVERTEXLIST(((0.,0.),(1.,1.)));\n"
    "#8=IFCINDEXEDPOLYGONALFACEWITHVOIDS((1,2,3),((4,5,6)));\n"
    "#9=IFCACTORROLE(.ARCHITECT.,$,$);\n"
    "ENDSEC;\n"
    "END-ISO-10303-21;\n";

int main(void) {
    ifcopenshell_ifc_file_t* file = NULL;
    ifcopenshell_ifc_file_t* opened = NULL;
    ifcopenshell_ifc_file_t* file_from_header = NULL;
    ifcopenshell_ifc_schema_t* schema = NULL;
    ifcopenshell_ifc_schema_t* reloaded_schema = NULL;
    ifcopenshell_ifc_declaration_t* point_decl = NULL;
    ifcopenshell_ifc_declaration_t* polyline_decl = NULL;
    ifcopenshell_ifc_declaration_t* role_decl = NULL;
    ifcopenshell_ifc_declaration_t* root_decl = NULL;
    ifcopenshell_ifc_declaration_t* object_def_decl = NULL;
    ifcopenshell_ifc_declaration_t* integer_decl = NULL;
    ifcopenshell_ifc_declaration_t* positive_integer_decl = NULL;
    ifcopenshell_ifc_instance_t* point = NULL;
    ifcopenshell_ifc_instance_t* point_two = NULL;
    ifcopenshell_ifc_instance_t* polyline = NULL;
    ifcopenshell_ifc_instance_t* person = NULL;
    ifcopenshell_ifc_instance_t* actor_role = NULL;
    ifcopenshell_ifc_instance_t* detached_point = NULL;
    ifcopenshell_ifc_instance_t* added_point = NULL;
    ifcopenshell_ifc_instance_t* created_point = NULL;
    ifcopenshell_ifc_instance_t* file_created_point = NULL;
    ifcopenshell_ifc_instance_t* null_instance = NULL;
    ifcopenshell_ifc_header_t* header = NULL;
    ifcopenshell_ifc_file_description_t* file_description = NULL;
    ifcopenshell_ifc_file_name_t* file_name = NULL;
    ifcopenshell_ifc_file_schema_t* file_schema = NULL;
    ifcopenshell_ifc_entity_t* point_entity = NULL;
    ifcopenshell_ifc_entity_t* object_def_entity = NULL;
    ifcopenshell_ifc_entity_t* entity_from_helper = NULL;
    ifcopenshell_ifc_entity_t* supertype = NULL;
    ifcopenshell_ifc_attribute_t* point_attribute = NULL;
    ifcopenshell_ifc_attribute_t* referenced_attribute = NULL;
    ifcopenshell_ifc_inverse_attribute_t* inverse_attribute = NULL;
    ifcopenshell_ifc_parameter_type_t* point_parameter_type = NULL;
    ifcopenshell_ifc_parameter_type_t* simple_parameter_type = NULL;
    ifcopenshell_ifc_parameter_type_t* named_parameter_type = NULL;
    ifcopenshell_ifc_named_type_t* named_type = NULL;
    ifcopenshell_ifc_simple_type_t* simple_type = NULL;
    ifcopenshell_ifc_type_declaration_t* integer_type_decl = NULL;
    ifcopenshell_ifc_type_declaration_t* integer_type_decl_self = NULL;
    ifcopenshell_ifc_type_declaration_t* positive_integer_type_decl = NULL;
    ifcopenshell_ifc_select_type_t* select_type = NULL;
    ifcopenshell_ifc_select_type_t* select_type_self = NULL;
    ifcopenshell_ifc_enumeration_t* role_enum = NULL;
    ifcopenshell_ifc_enumeration_t* role_enum_self = NULL;
    ifcopenshell_ifc_enumeration_t* role_enum_from_value = NULL;
    ifcopenshell_ifc_aggregation_type_t* aggregation_type = NULL;
    ifcopenshell_ifc_aggregation_type_t* aggregation_type_self = NULL;
    ifcopenshell_ifcparse_attribute_value_t* family_name_value = NULL;
    ifcopenshell_ifcparse_attribute_value_t* role_value = NULL;
    ifcopenshell_ifcparse_attribute_value_t* reopened_coordinates_value = NULL;
    ifcopenshell_ifcparse_instance_list_t* inverse_by_decl = NULL;
    ifcopenshell_ifcparse_instance_list_t* detached_point_list = NULL;
    ifcopenshell_ifcparse_instance_list_t* traverse_list = NULL;
    ifcopenshell_ifcparse_instance_list_t* traverse_bfs_list = NULL;
    ifcopenshell_ifcparse_instance_list_t* point_instances = NULL;
    ifcopenshell_ifcparse_instance_list_t* reopened_point_instances = NULL;
    ifcopenshell_ifc_instance_streamer_t* streamer = NULL;
    ifcopenshell_ifc_declaration_list_t select_members = {0};
    ifcopenshell_ifc_attribute_list_t point_attributes = {0};
    ifcopenshell_ifc_inverse_attribute_list_t inverse_attributes = {0};
    ifcopenshell_ifc_entity_list_t object_subtypes = {0};
    ifcopenshell_bool_list_t point_derived = {0};
    ifcopenshell_string_list_t names = {0};
    ifcopenshell_string_list_t header_schema_identifiers = {0};
    ifcopenshell_string_list_t kv_iter = {0};
    ifcopenshell_string_list_t bypass_types = {0};
    ifcopenshell_string_t string_value = {0};
    ifcopenshell_string_t json_value = {0};
    ifcopenshell_string_t key_value = {0};
    ifcopenshell_string_t inverse_json = {0};
    ifcopenshell_string_t refs_json = {0};
    ifcopenshell_string_t instance_json = {0};
    ifcopenshell_string_t timestamp = {0};
    ifcopenshell_string_t header_text = {0};
    bool bool_value = false;
    bool contains_point = false;
    bool contains_point_two = false;
    uint32_t uint_value = 0;
    uint32_t added_point_id = 0;
    uint32_t created_point_id = 0;
    size_t size_value = 0;
    size_t enum_index = 0;
    int32_t int_value = 0;
    double double_value = 0.0;
    ifcopenshell_double_list_t reopened_coordinates = {0};
    char output_path[512] = {0};

    snprintf(output_path, sizeof(output_path), "/tmp/ifcopenshell_ifcparse_capi_surface_%ld.ifc", (long)getpid());

    expect_ok(ifcopenshell_ifcparse_read_memory(IFC_FIXTURE, &file));
    expect_ok(ifcopenshell_ifc_file_by_id(file, 1, &point));
    expect_ok(ifcopenshell_ifc_file_by_id(file, 2, &point_two));
    expect_ok(ifcopenshell_ifc_file_by_id(file, 3, &polyline));
    expect_ok(ifcopenshell_ifc_file_by_id(file, 4, &person));
    expect_ok(ifcopenshell_ifc_file_by_id(file, 9, &actor_role));
    expect_ok(ifcopenshell_ifc_file_schema(file, &schema));
    expect_ok(ifcopenshell_ifc_file_header(file, &header));
    expect_ok(ifcopenshell_ifc_header_file_description(header, &file_description));
    expect_ok(ifcopenshell_ifc_header_file_name(header, &file_name));
    expect_ok(ifcopenshell_ifc_header_file_schema(header, &file_schema));
    expect_ok(ifcopenshell_ifc_instance_declaration(point, &point_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcPolyline", &polyline_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcRoleEnum", &role_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcRoot", &root_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcObjectDefinition", &object_def_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcInteger", &integer_decl));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcPositiveInteger", &positive_integer_decl));

    expect_ok(ifcopenshell_ifcparse_set_feature("use_attribute_value_derived", true));
    expect_ok(ifcopenshell_ifcparse_get_feature("use_attribute_value_derived", &bool_value));
    expect_true(bool_value, "Feature toggle should be enabled");
    expect_ok(ifcopenshell_ifcparse_set_feature("use_attribute_value_derived", false));
    expect_ok(ifcopenshell_ifcparse_set_log_format_json());
    expect_ok(ifcopenshell_ifcparse_turn_on_detailed_logging());
    expect_ok(ifcopenshell_ifcparse_turn_off_detailed_logging());
    expect_ok(ifcopenshell_ifcparse_set_log_format_text());
    expect_ok(ifcopenshell_ifcparse_get_log(&string_value));
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifcparse_ifc_si_prefix_to_value("KILO", &double_value));
    expect_true(double_value == 1000.0, "Unexpected SI prefix conversion");
    expect_fail(ifcopenshell_ifcparse_get_si_equivalent(point, &double_value));
    expect_ok(ifcopenshell_ifcparse_get_info_cpp(point, true, &json_value));
    expect_true(strstr(json_value.data, "\"type\":\"IfcCartesianPoint\"") != NULL, "Missing instance type in JSON");
    ifcopenshell_string_destroy(&json_value);
    expect_ok(ifcopenshell_ifcparse_operator_token_ptr(0u, ";", &int_value));
    expect_ok(ifcopenshell_ifcparse_general_token_ptr(0u, "IFCCARTESIANPOINT", &int_value));

    expect_ok(ifcopenshell_ifc_file_get_inverses_by_declaration(file, 1, polyline_decl, -1, &inverse_by_decl));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(inverse_by_decl, &size_value));
    expect_true(size_value == 1u, "Unexpected inverse-by-declaration count");
    expect_ok(ifcopenshell_ifcparse_instance_list_get(inverse_by_decl, 0u, &null_instance));
    expect_ok(ifcopenshell_ifc_instance_id(null_instance, &uint_value));
    expect_true(uint_value == 3u, "Unexpected inverse-by-declaration target");
    expect_ok(ifcopenshell_ifc_file_fresh_id(file, &uint_value));
    expect_true(uint_value >= 10u, "Unexpected fresh id");
    expect_ok(ifcopenshell_ifc_file_ifcroot_type(file, &root_decl));
    expect_true(root_decl != NULL, "IfcRoot declaration should be available");
    expect_fail(ifcopenshell_ifc_file_by_guid(file, "does-not-exist", &null_instance));
    expect_ok(ifcopenshell_ifc_file_traverse(file, polyline, 1, &traverse_list));
    expect_ok(ifcopenshell_ifc_file_traverse_breadth_first(file, polyline, 1, &traverse_bfs_list));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(traverse_list, &size_value));
    expect_true(size_value == 3u, "Depth-first traverse should return polyline and both points");
    expect_ok(ifcopenshell_ifcparse_instance_list_contains(traverse_list, point, &contains_point));
    expect_true(contains_point, "Depth-first traverse should contain the first point");
    expect_ok(ifcopenshell_ifcparse_instance_list_contains(traverse_list, point_two, &contains_point_two));
    expect_true(contains_point_two, "Depth-first traverse should contain the second point");
    expect_ok(ifcopenshell_ifcparse_instance_list_size(traverse_bfs_list, &size_value));
    expect_true(size_value == 3u, "Breadth-first traverse should return polyline and both points");
    expect_ok(ifcopenshell_ifc_file_create_timestamp(file, &timestamp));
    expect_true(timestamp.size > 0u, "Timestamp should not be empty");
    ifcopenshell_string_destroy(&timestamp);
    expect_ok(ifcopenshell_ifc_file_batch(file));
    expect_ok(ifcopenshell_ifc_file_build_inverses(file));
    expect_ok(ifcopenshell_ifc_file_build_inverses_(file, point));
    expect_ok(ifcopenshell_ifc_file_bypass_type(file, "IfcProxy"));
    expect_ok(ifcopenshell_ifc_file_recalculate_id_counter(file));
    expect_ok(ifcopenshell_ifc_file_reset_identity_cache(file));
    expect_ok(ifcopenshell_ifc_file_get_unit(file, "LENGTHUNIT", &double_value));
    expect_true(double_value == 1.0, "Unexpected default unit scale");
    expect_ok(ifcopenshell_ifc_file_key_value_store_query(file, "missing", &key_value));
    expect_true(key_value.size == 0u, "Expected empty key-value result");
    ifcopenshell_string_destroy(&key_value);
    expect_ok(ifcopenshell_ifc_file_key_value_store_iter(file, "prefix", &kv_iter));
    expect_true(kv_iter.size == 0u, "Expected empty key-value iteration");

    expect_ok(ifcopenshell_ifc_schema_instantiate(schema, point_decl, &detached_point));
    expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(
        detached_point, 0u, &((ifcopenshell_double_list_t){(double[]){2.0, 2.0, 2.0}, 3u})
    ));
    expect_ok(ifcopenshell_ifcparse_traverse(detached_point, 0, &detached_point_list));
    expect_ok(ifcopenshell_ifc_file_add_entities(file, detached_point_list));
    expect_ok(ifcopenshell_ifc_file_by_type(file, "IfcCartesianPoint", &point_instances));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(point_instances, &size_value));
    expect_true(size_value == 3u, "add_entities() should add the detached point to the file");
    expect_ok(ifcopenshell_ifc_schema_instantiate(schema, point_decl, &added_point));
    expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(
        added_point, 0u, &((ifcopenshell_double_list_t){(double[]){4.0, 5.0, 6.0}, 3u})
    ));
    expect_ok(ifcopenshell_ifc_file_add(file, added_point, -1, &file_created_point));
    expect_ok(ifcopenshell_ifc_instance_id(file_created_point, &added_point_id));
    expect_true(added_point_id >= 10u, "add() should assign a fresh id");
    expect_ok(ifcopenshell_ifc_file_by_id(file, added_point_id, &null_instance));
    expect_true(null_instance != NULL, "Added point should be retrievable by id");
    expect_ok(ifcopenshell_ifc_file_create(file, point_decl, &created_point));
    expect_ok(ifcopenshell_ifc_instance_set_argument_double_list(
        created_point, 0u, &((ifcopenshell_double_list_t){(double[]){7.0, 8.0, 9.0}, 3u})
    ));
    expect_ok(ifcopenshell_ifc_instance_id(created_point, &created_point_id));
    expect_ok(ifcopenshell_ifc_file_add_type_ref(file, created_point));
    expect_ok(ifcopenshell_ifc_file_remove_type_ref(file, created_point));
    expect_ok(ifcopenshell_ifc_file_process_deletion_inverse(file, created_point));
    expect_ok(ifcopenshell_ifc_file_remove(file, created_point));
    expect_ok(ifcopenshell_ifc_file_unbatch(file));
    expect_ok(ifcopenshell_ifc_file_by_type(file, "IfcCartesianPoint", &point_instances));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(point_instances, &size_value));
    expect_true(size_value == 4u, "File should contain four points after add/add_entities/remove");

    expect_ok(ifcopenshell_ifc_declaration_as_entity(point_decl, &point_entity));
    expect_ok(ifcopenshell_ifc_declaration_as_entity(object_def_decl, &object_def_entity));
    expect_ok(ifcopenshell_ifc_entity_attribute_index(point_entity, "Coordinates", &int_value));
    expect_true(int_value == 0, "Unexpected attribute index");
    expect_ok(ifcopenshell_ifc_entity_as_entity(point_entity, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_entity_attribute_by_index(point_entity, 0u, &point_attribute));
    expect_ok(ifcopenshell_ifc_entity_attributes(point_entity, &point_attributes));
    expect_ok(ifcopenshell_ifc_entity_derived(point_entity, &point_derived));
    expect_ok(ifcopenshell_ifc_entity_set_attributes(point_entity, &point_attributes, &point_derived));
    expect_ok(ifcopenshell_ifc_entity_all_inverse_attributes(object_def_entity, &inverse_attributes));
    expect_true(inverse_attributes.size > 0u, "IfcObjectDefinition should expose inverse attributes");
    inverse_attribute = inverse_attributes.items[0];
    expect_ok(ifcopenshell_ifc_entity_set_inverse_attributes(object_def_entity, &inverse_attributes));
    expect_ok(ifcopenshell_ifc_entity_subtypes(object_def_entity, &object_subtypes));
    expect_true(object_subtypes.size > 0u, "IfcObjectDefinition should have subtypes");
    expect_ok(ifcopenshell_ifc_entity_set_subtypes(object_def_entity, &object_subtypes));
    expect_ok(ifcopenshell_ifc_entity_supertype(object_def_entity, &supertype));
    expect_true(supertype != NULL, "IfcObjectDefinition should have a supertype");

    expect_ok(ifcopenshell_ifc_inverse_attribute_attribute_reference(inverse_attribute, &referenced_attribute));
    expect_ok(ifcopenshell_ifc_inverse_attribute_bound1(inverse_attribute, &int_value));
    expect_ok(ifcopenshell_ifc_inverse_attribute_bound2(inverse_attribute, &int_value));
    expect_ok(ifcopenshell_ifc_inverse_attribute_entity_reference(inverse_attribute, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_inverse_attribute_name(inverse_attribute, &string_value));
    expect_true(string_value.size > 0u, "Inverse attribute name should not be empty");
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_inverse_attribute_type_of_aggregation(inverse_attribute, &int_value));
    expect_ok(ifcopenshell_ifc_inverse_attribute_type_of_aggregation_string(inverse_attribute, &string_value));
    ifcopenshell_string_destroy(&string_value);

    expect_ok(ifcopenshell_ifc_declaration_as_type_declaration(integer_decl, &integer_type_decl));
    expect_ok(ifcopenshell_ifc_type_declaration_as_type_declaration(integer_type_decl, &integer_type_decl_self));
    expect_ok(ifcopenshell_ifc_type_declaration_declared_type(integer_type_decl, &simple_parameter_type));
    expect_ok(ifcopenshell_ifc_parameter_type_as_simple_type(simple_parameter_type, &simple_type));
    expect_ok(ifcopenshell_ifc_simple_type_as_simple_type(simple_type, &simple_type));
    expect_ok(ifcopenshell_ifc_simple_type_declared_type(simple_type, &int_value));
    expect_ok(ifcopenshell_ifc_declaration_as_type_declaration(positive_integer_decl, &positive_integer_type_decl));
    expect_ok(ifcopenshell_ifc_type_declaration_declared_type(positive_integer_type_decl, &named_parameter_type));
    expect_ok(ifcopenshell_ifc_parameter_type_as_named_type(named_parameter_type, &named_type));
    expect_ok(ifcopenshell_ifc_named_type_as_named_type(named_type, &named_type));
    expect_ok(ifcopenshell_ifc_named_type_is_a(named_type, "IfcInteger", &bool_value));
    expect_true(bool_value, "IfcPositiveInteger should resolve to named type IfcInteger");

    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcSegmentIndexSelect", &point_decl));
    expect_ok(ifcopenshell_ifc_declaration_as_select_type(point_decl, &select_type));
    expect_ok(ifcopenshell_ifc_select_type_as_select_type(select_type, &select_type_self));
    expect_ok(ifcopenshell_ifc_select_type_select_list(select_type, &select_members));
    expect_true(select_members.size >= 2u, "Select type should expose members");

    expect_ok(ifcopenshell_ifc_declaration_as_enumeration_type(role_decl, &role_enum));
    expect_ok(ifcopenshell_ifc_enumeration_as_enumeration_type(role_enum, &role_enum_self));
    expect_ok(ifcopenshell_ifc_instance_get_argument(actor_role, 0u, &role_value));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_enumeration_index(role_value, &enum_index));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_enumeration_type(role_value, &role_enum_from_value));
    expect_true(role_enum_from_value != NULL, "Enumeration type should round-trip from attribute value");

    expect_ok(ifcopenshell_ifc_type_declaration_declared_type(integer_type_decl, &point_parameter_type));
    expect_ok(ifcopenshell_ifc_parameter_type_as_aggregation_type(point_parameter_type, &aggregation_type));
    expect_ok(ifcopenshell_ifc_schema_declaration_by_name(schema, "IfcLineIndex", &point_decl));
    expect_ok(ifcopenshell_ifc_declaration_as_type_declaration(point_decl, &integer_type_decl));
    expect_ok(ifcopenshell_ifc_type_declaration_declared_type(integer_type_decl, &point_parameter_type));
    expect_ok(ifcopenshell_ifc_parameter_type_as_aggregation_type(point_parameter_type, &aggregation_type));
    expect_ok(ifcopenshell_ifc_aggregation_type_as_aggregation_type(aggregation_type, &aggregation_type_self));

    expect_ok(ifcopenshell_ifc_header_file(header, &file_from_header));
    expect_ok(ifcopenshell_ifc_header_write(header, &header_text));
    expect_true(strstr(header_text.data, "FILE_SCHEMA") != NULL, "Header serialization should contain FILE_SCHEMA");
    ifcopenshell_string_destroy(&header_text);
    expect_fail(ifcopenshell_ifc_header_try_read((ifcopenshell_ifc_header_t*)NULL, &bool_value));
    expect_fail(ifcopenshell_ifc_header_read((ifcopenshell_ifc_header_t*)NULL));

    expect_ok(ifcopenshell_ifc_file_description_class(file_description, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_description_declaration(file_description, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_description_implementation_level(file_description, &string_value));
    expect_true(strcmp(string_value.data, "2;1") == 0, "Unexpected implementation level");
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_description_description(file_description, &names));
    expect_ok(ifcopenshell_ifc_file_description_setdescription(file_description, &names));
    expect_ok(ifcopenshell_ifc_file_description_setimplementation_level(file_description, "4;0"));

    expect_ok(ifcopenshell_ifc_file_name_class(file_name, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_name_declaration(file_name, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_name_name(file_name, &string_value));
    expect_true(strcmp(string_value.data, "test.ifc") == 0, "Unexpected FILE_NAME name");
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_name_author(file_name, &names));
    expect_ok(ifcopenshell_ifc_file_name_authorization(file_name, &string_value));
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_name_organization(file_name, &names));
    expect_ok(ifcopenshell_ifc_file_name_originating_system(file_name, &string_value));
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_name_preprocessor_version(file_name, &string_value));
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_name_time_stamp(file_name, &string_value));
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_name_setauthor(file_name, &names));
    expect_ok(ifcopenshell_ifc_file_name_setauthorization(file_name, "approved"));
    expect_ok(ifcopenshell_ifc_file_name_setname(file_name, "surface.ifc"));
    expect_ok(ifcopenshell_ifc_file_name_setorganization(file_name, &names));
    expect_ok(ifcopenshell_ifc_file_name_setoriginating_system(file_name, "IfcOpenShell"));
    expect_ok(ifcopenshell_ifc_file_name_setpreprocessor_version(file_name, "IfcOpenShell Test"));
    expect_ok(ifcopenshell_ifc_file_name_settime_stamp(file_name, "2024-02-02T00:00:00"));

    expect_ok(ifcopenshell_ifc_file_schema_class(file_schema, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_schema_declaration(file_schema, &entity_from_helper));
    expect_ok(ifcopenshell_ifc_file_schema_schema_identifiers(file_schema, &header_schema_identifiers));
    expect_true(
        string_list_contains_exact(&header_schema_identifiers, "IFC4"),
        "FILE_SCHEMA should expose IFC4 before mutation"
    );
    expect_ok(ifcopenshell_ifc_file_schema_setschema_identifiers(file_schema, &header_schema_identifiers));
    expect_ok(ifcopenshell_ifc_file_to_string(file, &string_value));
    expect_true(strstr(string_value.data, "surface.ifc") != NULL, "Serialized file should contain updated FILE_NAME");
    expect_true(strstr(string_value.data, "approved") != NULL, "Serialized file should contain updated authorization");
    ifcopenshell_string_destroy(&string_value);
    expect_ok(ifcopenshell_ifc_file_write(file, output_path));
    expect_ok(ifcopenshell_ifcparse_open(output_path, true, &opened));
    expect_ok(ifcopenshell_ifc_file_by_type(opened, "IfcCartesianPoint", &reopened_point_instances));
    expect_ok(ifcopenshell_ifcparse_instance_list_size(reopened_point_instances, &size_value));
    expect_true(size_value == 4u, "Reopened file should contain persisted added points");
    expect_ok(ifcopenshell_ifc_file_by_id(opened, added_point_id, &null_instance));
    expect_ok(ifcopenshell_ifc_instance_get_argument(null_instance, 0u, &reopened_coordinates_value));
    expect_ok(ifcopenshell_ifcparse_attribute_value_as_double_list(reopened_coordinates_value, &reopened_coordinates));
    expect_true(reopened_coordinates.size == 3u, "Added point coordinates should persist after reopen");
    expect_true(
        reopened_coordinates.items[0] == 4.0 &&
        reopened_coordinates.items[1] == 5.0 &&
        reopened_coordinates.items[2] == 6.0,
        "Added point coordinates changed after reopen"
    );
    ifcopenshell_double_list_destroy(&reopened_coordinates);
    ifcopenshell_ifcparse_attribute_value_destroy(reopened_coordinates_value);
    reopened_coordinates_value = NULL;
    expect_ok(ifcopenshell_ifc_file_initialize(opened, output_path, 0, true, &bool_value));
    expect_true(bool_value, "initialize() should succeed on a written SPF file");

    expect_ok(ifcopenshell_ifc_instance_data(point, &size_value));
    expect_true(size_value != 0u, "Instance data pointer should not be null");
    expect_ok(ifcopenshell_ifc_instance_get_attribute_value(person, 1u, &family_name_value));
    expect_ok(ifcopenshell_ifc_instance_set_attribute_value(person, "FamilyName", family_name_value));
    expect_ok(ifcopenshell_ifc_instance_unset_attribute_value(person, "FamilyName"));
    expect_ok(ifcopenshell_ifc_instance_set_attribute_value(person, "FamilyName", family_name_value));
    expect_ok(ifcopenshell_ifc_instance_is_a(point, "IfcCartesianPoint", &bool_value));
    expect_true(bool_value, "Instance type check failed");
    expect_fail(ifcopenshell_ifc_instance_set_argument_logical(point, 0u, 1));
    expect_fail(ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(point, 0u, &((ifcopenshell_int32_list_list_t){0})));

    expect_ok(ifcopenshell_ifcparse_stream_from_string(IFC_FIXTURE, &streamer));
    bypass_types.items = &((ifcopenshell_string_t){"IfcCartesianPoint", strlen("IfcCartesianPoint"), false});
    bypass_types.size = 1u;
    expect_ok(ifcopenshell_ifc_instance_streamer_bypass_types(streamer, &bypass_types));
    expect_ok(ifcopenshell_ifc_instance_streamer_push_page(streamer, ""));
    expect_ok(ifcopenshell_ifc_instance_streamer_references(streamer, &refs_json));
    expect_ok(ifcopenshell_ifc_instance_streamer_inverses(streamer, &inverse_json));
    expect_ok(ifcopenshell_ifc_instance_streamer_read_instance_py(streamer, true, &instance_json));
    expect_true(refs_json.data != NULL, "References JSON should be present");
    expect_true(inverse_json.data != NULL, "Inverses JSON should be present");
    expect_true(instance_json.data != NULL, "Streamer instance JSON should be present");

    expect_ok(ifcopenshell_ifcparse_register_schema(schema));
    expect_ok(ifcopenshell_ifcparse_clear_schemas());
    expect_ok(ifcopenshell_ifcparse_schema_by_name("IFC4", &reloaded_schema));
    expect_true(reloaded_schema != NULL, "Schema reload after clear_schemas() failed");

    unlink(output_path);
    return 0;
}
