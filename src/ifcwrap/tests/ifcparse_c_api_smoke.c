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
    "ENDSEC;\n"
    "END-ISO-10303-21;\n";

int main(void) {
    ifcopenshell_string_t version = {0};
    ifcopenshell_string_t schema_lookup_name = {0};
    ifcopenshell_string_t declaration_name = {0};
    ifcopenshell_string_t declaration_schema_name = {0};
    ifcopenshell_string_t enum_value = {0};
    ifcopenshell_string_t aggregation_kind = {0};
    ifcopenshell_string_t parameter_kind = {0};
    ifcopenshell_string_t simple_kind = {0};
    ifcopenshell_string_t named_declaration_name = {0};
    ifcopenshell_ifc_file_t* file = NULL;
    ifcopenshell_ifc_instance_t* point = NULL;
    ifcopenshell_ifc_instance_t* point_two = NULL;
    ifcopenshell_ifc_instance_t* polyline = NULL;
    ifcopenshell_ifc_instance_t* person = NULL;
    ifcopenshell_ifc_instance_t* colour = NULL;
    ifcopenshell_ifc_instance_t* indexed_polygonal_face = NULL;
    ifcopenshell_ifc_instance_t* texture_vertex_list = NULL;
    ifcopenshell_ifc_instance_t* indexed_polygonal_face_with_voids = NULL;
    ifcopenshell_ifc_instance_t* traversed = NULL;
    ifcopenshell_ifc_instance_t* aggregate_item = NULL;
    ifcopenshell_ifc_header_t* header = NULL;
    ifcopenshell_ifc_file_description_t* file_description = NULL;
    ifcopenshell_ifc_file_name_t* file_name = NULL;
    ifcopenshell_ifc_file_schema_t* file_schema = NULL;
    ifcopenshell_ifc_instance_streamer_t* streamer = NULL;
    ifcopenshell_ifc_schema_t* schema = NULL;
    ifcopenshell_ifc_schema_t* schema_lookup = NULL;
    ifcopenshell_ifc_schema_t* declaration_schema = NULL;
    ifcopenshell_ifc_declaration_t* declaration = NULL;
    ifcopenshell_ifc_declaration_t* declaration_by_name = NULL;
    ifcopenshell_ifc_declaration_t* declaration_by_index = NULL;
    ifcopenshell_ifc_declaration_t* enum_declaration = NULL;
    ifcopenshell_ifc_declaration_t* line_index_declaration = NULL;
    ifcopenshell_ifc_declaration_t* segment_index_select_declaration = NULL;
    ifcopenshell_ifc_declaration_t* positive_integer_declaration = NULL;
    ifcopenshell_ifc_declaration_t* identifier_declaration = NULL;
    ifcopenshell_ifc_declaration_t* role_enum_declaration = NULL;
    ifcopenshell_ifc_declaration_t* named_declaration = NULL;
    ifcopenshell_ifc_enumeration_t* enumeration = NULL;
    ifcopenshell_ifc_enumeration_t* role_enumeration = NULL;
    ifcopenshell_ifc_type_declaration_t* type_declaration = NULL;
    ifcopenshell_ifc_type_declaration_t* positive_integer_type_declaration = NULL;
    ifcopenshell_ifc_type_declaration_t* identifier_type_declaration = NULL;
    ifcopenshell_ifc_select_type_t* select_type = NULL;
    ifcopenshell_ifc_parameter_type_t* parameter_type = NULL;
    ifcopenshell_ifc_parameter_type_t* element_parameter_type = NULL;
    ifcopenshell_ifc_parameter_type_t* named_parameter_type = NULL;
    ifcopenshell_ifc_parameter_type_t* simple_parameter_type = NULL;
    ifcopenshell_ifc_named_type_t* named_type = NULL;
    ifcopenshell_ifc_simple_type_t* simple_type = NULL;
    ifcopenshell_ifc_aggregation_type_t* aggregation_type = NULL;
    ifcopenshell_ifc_entity_t* entity = NULL;
    ifcopenshell_ifc_attribute_t* attribute = NULL;
    ifcopenshell_ifcparse_attribute_value_t* argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* point_coordinates = NULL;
    ifcopenshell_ifcparse_attribute_value_t* indexed_face_indices = NULL;
    ifcopenshell_ifcparse_attribute_value_t* polyline_points = NULL;
    ifcopenshell_ifcparse_attribute_value_t* family_name_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* middle_names_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* texture_coords_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* inner_coord_indices_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* mutated_argument = NULL;
    ifcopenshell_ifcparse_attribute_value_t* reopened_point_coordinates = NULL;
    ifcopenshell_ifcparse_attribute_value_t* reopened_family_name_argument = NULL;
    ifcopenshell_ifcparse_instance_list_t* traversed_instances = NULL;
    ifcopenshell_ifcparse_instance_list_t* traversed_instances_bfs = NULL;
    ifcopenshell_ifcparse_instance_list_t* polyline_point_list = NULL;
    ifcopenshell_ifcparse_instance_list_t* unique_polyline_point_list = NULL;
    ifcopenshell_ifcparse_instance_list_t* instances_by_type = NULL;
    ifcopenshell_ifcparse_instance_list_t* instances_by_type_excl_subtypes = NULL;
    ifcopenshell_ifcparse_instance_list_t* instances_by_reference = NULL;
    ifcopenshell_ifcparse_instance_list_t* inverse_instances = NULL;
    ifcopenshell_ifc_instance_t* created_integer = NULL;
    ifcopenshell_ifc_instance_t* created_boolean = NULL;
    ifcopenshell_ifc_instance_t* created_real = NULL;
    ifcopenshell_ifc_instance_t* created_actor_role = NULL;
    ifcopenshell_ifc_instance_t* created_axis2_placement = NULL;
    ifcopenshell_ifc_instance_t* created_polyline = NULL;
    ifcopenshell_ifc_instance_t* axis_location = NULL;
    ifcopenshell_ifc_instance_t* inverse_instance = NULL;
    ifcopenshell_ifc_file_t* reopened_file = NULL;
    ifcopenshell_ifc_instance_t* reopened_point_two = NULL;
    ifcopenshell_ifc_instance_t* reopened_person = NULL;
    ifcopenshell_int32_list_t indexed_face_index_values = {0};
    ifcopenshell_int32_list_t inverse_index_values_by_id = {0};
    ifcopenshell_double_list_t point_coordinate_values = {0};
    ifcopenshell_double_list_t reopened_point_coordinate_values = {0};
    ifcopenshell_int32_list_list_t inner_coord_index_values = {0};
    ifcopenshell_double_list_list_t texture_coord_values = {0};
    ifcopenshell_string_list_t schema_names = {0};
    ifcopenshell_string_list_t header_description = {0};
    ifcopenshell_string_list_t header_authors = {0};
    ifcopenshell_string_list_t header_schema_identifiers = {0};
    ifcopenshell_string_list_t enumeration_items = {0};
    ifcopenshell_string_list_t select_list_names = {0};
    ifcopenshell_string_list_t middle_name_values = {0};
    ifcopenshell_string_list_t file_types = {0};
    ifcopenshell_string_list_t entity_argument_types = {0};
    ifcopenshell_string_list_t type_declaration_argument_types = {0};
    ifcopenshell_string_list_t enumeration_argument_types = {0};
    ifcopenshell_string_list_t attribute_names = {0};
    ifcopenshell_string_list_t inverse_attribute_names = {0};
    ifcopenshell_uint32_list_t entity_ids = {0};
    ifcopenshell_uint32_list_t bypassed_instances = {0};
    ifcopenshell_bool_list_t entity_derived = {0};
    ifcopenshell_string_t class_name = {0};
    ifcopenshell_string_t schema_name = {0};
    ifcopenshell_string_t attribute_type = {0};
    ifcopenshell_string_t file_spf = {0};
    ifcopenshell_string_t declaration_name_uc = {0};
    ifcopenshell_string_t argument_name = {0};
    ifcopenshell_string_t aggregation_type_name = {0};
    ifcopenshell_string_t attribute_name = {0};
    ifcopenshell_string_t family_name = {0};
    ifcopenshell_string_t mutated_family_name = {0};
    ifcopenshell_string_t actor_role_name = {0};
    ifcopenshell_string_t actor_role_description = {0};
    ifcopenshell_string_t argument_type_name = {0};
    ifcopenshell_string_t instance_spf = {0};
    ifcopenshell_ifc_declaration_list_t schema_declarations = {0};
    ifcopenshell_ifc_enumeration_list_t schema_enumeration_types = {0};
    ifcopenshell_ifc_select_type_list_t schema_select_types = {0};
    ifcopenshell_ifc_type_declaration_list_t schema_type_declarations = {0};
    ifcopenshell_ifc_entity_list_t schema_entities = {0};
    ifcopenshell_ifc_attribute_list_t entity_attributes = {0};
    uint32_t instance_id = 0;
    uint32_t instance_identity = 0;
    uint32_t argument_index = 0;
    uint32_t max_id = 0;
    bool declaration_is_a = false;
    bool list_contains_point = false;
    bool streamer_has_semicolon = false;
    bool attribute_optional = true;
    bool entity_is_abstract = true;
    int32_t declaration_index = -1;
    int32_t declaration_type = -1;
    int32_t file_status = -1;
    int32_t file_storage_mode = -1;
    int32_t guessed_file_type = -1;
    int32_t streamer_status = -1;
    int32_t aggregation_type_value = -1;
    int32_t aggregation_bound1 = -1;
    int32_t aggregation_bound2 = -1;
    size_t enum_offset = 0;
    size_t attribute_size = 0;
    size_t entity_attribute_count = 0;
    size_t file_pointer = 0;
    size_t instance_file_pointer = 0;
    size_t architect_role_index = 0;
    size_t traversed_count = 0;
    size_t traversed_count_bfs = 0;
    size_t total_inverses = 1;
    int32_t total_inverses_for_instance = -1;
    size_t i = 0;
    bool found_line_index = false;
    bool found_arc_index = false;
    bool found_cartesian_point_type = false;
    bool found_polyline_type = false;
    bool found_family_name_attr = false;
    bool found_middle_names_attr = false;
    bool attribute_is_null = true;
    bool reopened_attribute_is_null = false;
    bool wrapped_boolean_value = false;
    int32_t wrapped_integer_value = 0;
    double wrapped_real_value = -1.0;
    double coordinate_value = -1.0;
    char output_path[512] = {0};

    snprintf(output_path, sizeof(output_path), "/tmp/ifcopenshell_ifcparse_capi_smoke_%ld.ifc", (long)getpid());

    expect_true(ifcopenshell_ifcparse_version(&version), ifcopenshell_ifcparse_last_error_message());
    expect_true(version.data != NULL && version.size > 0, "Version string is empty");

    expect_true(ifcopenshell_ifcparse_read_memory(IFC_FIXTURE, &file), ifcopenshell_ifcparse_last_error_message());
    expect_true(file != NULL, "File handle is null");

    expect_true(ifcopenshell_ifcparse_schema_by_name("IFC4", &schema_lookup), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_lookup != NULL, "Schema lookup handle is null");
    expect_true(ifcopenshell_ifcparse_schema_names(&schema_names), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_names.size > 0u, "Schema names list is empty");
    expect_true(schema_names.items[0].data != NULL, "Schema names list item is null");
    expect_true(ifcopenshell_ifc_schema_name(schema_lookup, &schema_lookup_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(schema_lookup_name.data, "IFC4") == 0, "Unexpected looked up schema name");
    expect_true(ifcopenshell_ifc_schema_declarations(schema_lookup, &schema_declarations), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_declarations.size > 0u, "Schema declarations list is empty");
    expect_true(schema_declarations.items[0] != NULL, "Schema declarations first item is null");
    expect_true(ifcopenshell_ifc_declaration_name(schema_declarations.items[0], &declaration_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_name.data != NULL && declaration_name.size > 0u, "Schema declaration name is empty");
    ifcopenshell_string_destroy(&declaration_name);
    expect_true(ifcopenshell_ifc_schema_enumeration_types(schema_lookup, &schema_enumeration_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_enumeration_types.size > 0u, "Schema enumeration types list is empty");
    expect_true(schema_enumeration_types.items[0] != NULL, "Schema enumeration types first item is null");
    expect_true(ifcopenshell_ifc_schema_select_types(schema_lookup, &schema_select_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_select_types.size > 0u, "Schema select types list is empty");
    expect_true(schema_select_types.items[0] != NULL, "Schema select types first item is null");
    expect_true(ifcopenshell_ifc_schema_type_declarations(schema_lookup, &schema_type_declarations), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_type_declarations.size > 0u, "Schema type declarations list is empty");
    expect_true(schema_type_declarations.items[0] != NULL, "Schema type declarations first item is null");
    expect_true(ifcopenshell_ifc_schema_entities(schema_lookup, &schema_entities), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema_entities.size > 0u, "Schema entities list is empty");
    expect_true(schema_entities.items[0] != NULL, "Schema entities first item is null");

    expect_true(ifcopenshell_ifc_file_by_id(file, 1u, &point), ifcopenshell_ifcparse_last_error_message());
    expect_true(point != NULL, "Point instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 2u, &point_two), ifcopenshell_ifcparse_last_error_message());
    expect_true(point_two != NULL, "Second point instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_type(file, "IfcCartesianPoint", &instances_by_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(instances_by_type != NULL, "Instances-by-type list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(instances_by_type, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected instances-by-type size");
    expect_true(ifcopenshell_ifc_file_by_type_excl_subtypes(file, "IfcCartesianPoint", &instances_by_type_excl_subtypes), ifcopenshell_ifcparse_last_error_message());
    expect_true(instances_by_type_excl_subtypes != NULL, "Instances-by-type-excl-subtypes list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(instances_by_type_excl_subtypes, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected instances-by-type-excl-subtypes size");
    expect_true(ifcopenshell_ifc_file_instances_by_reference(file, 1u, &instances_by_reference), ifcopenshell_ifcparse_last_error_message());
    expect_true(instances_by_reference != NULL, "Instances-by-reference list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(instances_by_reference, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 1u, "Unexpected instances-by-reference size");

    expect_true(ifcopenshell_ifc_file_by_id(file, 3u, &polyline), ifcopenshell_ifcparse_last_error_message());
    expect_true(polyline != NULL, "Polyline instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 4u, &person), ifcopenshell_ifcparse_last_error_message());
    expect_true(person != NULL, "Person instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 5u, &colour), ifcopenshell_ifcparse_last_error_message());
    expect_true(colour != NULL, "Colour instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 6u, &indexed_polygonal_face), ifcopenshell_ifcparse_last_error_message());
    expect_true(indexed_polygonal_face != NULL, "Indexed polygonal face instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 7u, &texture_vertex_list), ifcopenshell_ifcparse_last_error_message());
    expect_true(texture_vertex_list != NULL, "Texture vertex list instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(file, 8u, &indexed_polygonal_face_with_voids), ifcopenshell_ifcparse_last_error_message());
    expect_true(indexed_polygonal_face_with_voids != NULL, "Indexed polygonal face with voids instance handle is null");

    expect_true(ifcopenshell_ifc_file_good(file, &file_status), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_status == 0, "Unexpected file open status");
    expect_true(ifcopenshell_ifc_file_to_string(file, &file_spf), ifcopenshell_ifcparse_last_error_message());
    expect_true(strstr(file_spf.data, "ISO-10303-21") != NULL, "File SPF output missing header");
    expect_true(ifcopenshell_ifc_file_entity_names(file, &entity_ids), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity_ids.size == 8u, "Unexpected entity id list size");
    expect_true(entity_ids.items[0] == 1u, "Unexpected first entity id");
    expect_true(entity_ids.items[7] == 8u, "Unexpected last entity id");
    expect_true(ifcopenshell_ifc_file_file_pointer(file, &file_pointer), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_pointer != 0u, "Unexpected file pointer value");
    expect_true(ifcopenshell_ifc_file_storage_mode(file, &file_storage_mode), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_storage_mode == 0, "Unexpected file storage mode");
    expect_true(ifcopenshell_ifcparse_guess_file_type("smoke.ifc", &guessed_file_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(guessed_file_type == 0, "Unexpected guessed file type");

    expect_true(ifcopenshell_ifc_file_header(file, &header), ifcopenshell_ifcparse_last_error_message());
    expect_true(header != NULL, "Header handle is null");
    expect_true(ifcopenshell_ifc_header_file_description(header, &file_description), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_description != NULL, "File description handle is null");
    expect_true(ifcopenshell_ifc_header_file_name(header, &file_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_name != NULL, "File name handle is null");
    expect_true(ifcopenshell_ifc_header_file_schema(header, &file_schema), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_schema != NULL, "File schema handle is null");
    expect_true(ifcopenshell_ifc_file_description_description(file_description, &header_description), ifcopenshell_ifcparse_last_error_message());
    expect_true(header_description.size == 1u, "Unexpected header description size");
    expect_true(strcmp(header_description.items[0].data, "ViewDefinition [CoordinationView]") == 0, "Unexpected header description");
    expect_true(ifcopenshell_ifc_file_name_author(file_name, &header_authors), ifcopenshell_ifcparse_last_error_message());
    expect_true(header_authors.size == 1u, "Unexpected header author count");
    expect_true(strcmp(header_authors.items[0].data, "Codex") == 0, "Unexpected header author");
    expect_true(ifcopenshell_ifc_file_schema_schema_identifiers(file_schema, &header_schema_identifiers), ifcopenshell_ifcparse_last_error_message());
    expect_true(header_schema_identifiers.size == 1u, "Unexpected header schema identifier count");
    expect_true(strcmp(header_schema_identifiers.items[0].data, "IFC4") == 0, "Unexpected header schema identifier");

    expect_true(ifcopenshell_ifc_file_schema(file, &schema), ifcopenshell_ifcparse_last_error_message());
    expect_true(schema != NULL, "Schema handle is null");

    expect_true(ifcopenshell_ifc_file_get_max_id(file, &max_id), ifcopenshell_ifcparse_last_error_message());
    expect_true(max_id == 8u, "Unexpected max instance id");

    expect_true(ifcopenshell_ifc_instance_id(point, &instance_id), ifcopenshell_ifcparse_last_error_message());
    expect_true(instance_id == 1u, "Unexpected instance id");

    expect_true(ifcopenshell_ifc_instance_identity(point, &instance_identity), ifcopenshell_ifcparse_last_error_message());
    expect_true(instance_identity != 0u, "Unexpected instance identity");
    expect_true(ifcopenshell_ifc_instance_file_pointer(point, &instance_file_pointer), ifcopenshell_ifcparse_last_error_message());
    expect_true(instance_file_pointer == file_pointer, "Unexpected instance file pointer");
    expect_true(ifcopenshell_ifc_instance_get_argument_index(point, "Coordinates", &argument_index), ifcopenshell_ifcparse_last_error_message());
    expect_true(argument_index == 0u, "Unexpected argument index");
    expect_true(ifcopenshell_ifc_instance_get_argument_name(point, 0u, &argument_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(argument_name.data, "Coordinates") == 0, "Unexpected argument name");

    expect_true(ifcopenshell_ifc_instance_declaration(point, &declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration != NULL, "Declaration handle is null");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcCartesianPoint", &declaration_by_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_by_name != NULL, "Schema declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcActionRequestTypeEnum", &enum_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(enum_declaration != NULL, "Enum declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_declaration_name(declaration, &declaration_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(declaration_name.data, "IfcCartesianPoint") == 0, "Unexpected declaration name");
    expect_true(ifcopenshell_ifc_declaration_name_uc(declaration, &declaration_name_uc), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(declaration_name_uc.data, "IFCCARTESIANPOINT") == 0, "Unexpected upper-case declaration name");
    expect_true(ifcopenshell_ifc_declaration_schema(declaration, &declaration_schema), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_schema != NULL, "Declaration schema handle is null");
    expect_true(ifcopenshell_ifc_schema_name(declaration_schema, &declaration_schema_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(declaration_schema_name.data, "IFC4") == 0, "Unexpected declaration schema name");
    expect_true(ifcopenshell_ifc_declaration_index_in_schema(declaration, &declaration_index), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_index >= 0, "Unexpected declaration index");
    expect_true(ifcopenshell_ifc_declaration_type(declaration, &declaration_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_type == declaration_index, "Unexpected declaration type value");
    expect_true(ifcopenshell_ifc_schema_declaration_by_index(schema_lookup, (size_t)declaration_index, &declaration_by_index), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_by_index != NULL, "Schema declaration lookup by index returned null");
    expect_true(ifcopenshell_ifc_declaration_is_a(declaration, "IfcCartesianPoint", &declaration_is_a), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_is_a, "Declaration type check failed");
    expect_true(ifcopenshell_ifc_declaration_as_entity(declaration, &entity), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity != NULL, "Entity cast returned null");
    expect_true(ifcopenshell_ifc_entity_attribute_count(entity, &entity_attribute_count), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity_attribute_count == 1u, "Unexpected entity attribute count");
    expect_true(ifcopenshell_ifc_entity_is_abstract(entity, &entity_is_abstract), ifcopenshell_ifcparse_last_error_message());
    expect_true(!entity_is_abstract, "IfcCartesianPoint should not be abstract");
    expect_true(ifcopenshell_ifc_entity_derived(entity, &entity_derived), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity_derived.size == 1u, "Unexpected derived flag count");
    expect_true(!entity_derived.items[0], "IfcCartesianPoint.Coordinates should not be derived");
    expect_true(ifcopenshell_ifc_entity_argument_types(entity, &entity_argument_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity_argument_types.size == 1u, "Unexpected entity argument type count");
    expect_true(strcmp(entity_argument_types.items[0].data, "AGGREGATE OF DOUBLE") == 0, "Unexpected entity argument type");
    expect_true(ifcopenshell_ifc_entity_all_attributes(entity, &entity_attributes), ifcopenshell_ifcparse_last_error_message());
    expect_true(entity_attributes.size == 1u, "Unexpected entity attribute list size");
    expect_true(entity_attributes.items[0] != NULL, "Entity attribute item is null");
    attribute = entity_attributes.items[0];
    expect_true(ifcopenshell_ifc_attribute_name(attribute, &attribute_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_name.data, "Coordinates") == 0, "Unexpected entity attribute name");
    expect_true(ifcopenshell_ifc_attribute_optional(attribute, &attribute_optional), ifcopenshell_ifcparse_last_error_message());
    expect_true(!attribute_optional, "IfcCartesianPoint.Coordinates should not be optional");
    expect_true(ifcopenshell_ifc_attribute_type_of_attribute(attribute, &element_parameter_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(element_parameter_type != NULL, "Attribute parameter type is null");
    expect_true(ifcopenshell_ifc_declaration_as_enumeration_type(enum_declaration, &enumeration), ifcopenshell_ifcparse_last_error_message());
    expect_true(enumeration != NULL, "Enumeration cast returned null");
    expect_true(ifcopenshell_ifc_enumeration_argument_types(enumeration, &enumeration_argument_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(enumeration_argument_types.size == 1u, "Unexpected enumeration argument type count");
    expect_true(strcmp(enumeration_argument_types.items[0].data, "STRING") == 0, "Unexpected enumeration argument type");
    expect_true(ifcopenshell_ifc_enumeration_enumeration_items(enumeration, &enumeration_items), ifcopenshell_ifcparse_last_error_message());
    expect_true(enumeration_items.size > 0u, "Enumeration items list is empty");
    expect_true(strcmp(enumeration_items.items[0].data, "EMAIL") == 0, "Unexpected first enumeration item");
    expect_true(ifcopenshell_ifc_enumeration_lookup_enum_value(enumeration, 0u, &enum_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(enum_value.data, "EMAIL") == 0, "Unexpected enumeration value");
    expect_true(ifcopenshell_ifc_enumeration_lookup_enum_offset(enumeration, "EMAIL", &enum_offset), ifcopenshell_ifcparse_last_error_message());
    expect_true(enum_offset == 0u, "Unexpected enumeration offset");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcLineIndex", &line_index_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(line_index_declaration != NULL, "Type declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_declaration_as_type_declaration(line_index_declaration, &type_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(type_declaration != NULL, "Type declaration cast returned null");
    expect_true(ifcopenshell_ifc_type_declaration_argument_types(type_declaration, &type_declaration_argument_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(type_declaration_argument_types.size == 1u, "Unexpected type declaration argument type count");
    expect_true(strcmp(type_declaration_argument_types.items[0].data, "AGGREGATE OF INT") == 0, "Unexpected type declaration argument type");
    expect_true(ifcopenshell_ifc_type_declaration_declared_type(type_declaration, &parameter_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(parameter_type != NULL, "Declared parameter type returned null");
    expect_true(ifcopenshell_ifc_parameter_type_as_aggregation_type(parameter_type, &aggregation_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(aggregation_type != NULL, "Aggregation type cast returned null");
    expect_true(ifcopenshell_ifc_aggregation_type_bound1(aggregation_type, &aggregation_bound1), ifcopenshell_ifcparse_last_error_message());
    expect_true(aggregation_bound1 == 2, "Unexpected aggregation lower bound");
    expect_true(ifcopenshell_ifc_aggregation_type_bound2(aggregation_type, &aggregation_bound2), ifcopenshell_ifcparse_last_error_message());
    expect_true(aggregation_bound2 == -1, "Unexpected aggregation upper bound");
    expect_true(ifcopenshell_ifc_aggregation_type_type_of_aggregation(aggregation_type, &aggregation_type_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(aggregation_type_value == 2, "Unexpected aggregation enum value");
    expect_true(ifcopenshell_ifc_aggregation_type_type_of_aggregation_string(aggregation_type, &aggregation_type_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(aggregation_type_name.data, "list") == 0, "Unexpected aggregation type string");
    expect_true(ifcopenshell_ifc_aggregation_type_kind(aggregation_type, &aggregation_kind), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(aggregation_kind.data, "LIST") == 0, "Unexpected aggregation kind");
    expect_true(ifcopenshell_ifc_aggregation_type_type_of_element(aggregation_type, &element_parameter_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(element_parameter_type != NULL, "Aggregation element type returned null");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcSegmentIndexSelect", &segment_index_select_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(segment_index_select_declaration != NULL, "Select declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_declaration_as_select_type(segment_index_select_declaration, &select_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(select_type != NULL, "Select type cast returned null");
    expect_true(ifcopenshell_ifc_select_type_select_list_names(select_type, &select_list_names), ifcopenshell_ifcparse_last_error_message());
    expect_true(select_list_names.size >= 2u, "Select type members list is unexpectedly small");
    for (i = 0; i < select_list_names.size; ++i) {
        if (strcmp(select_list_names.items[i].data, "IfcLineIndex") == 0) {
            found_line_index = true;
        }
        if (strcmp(select_list_names.items[i].data, "IfcArcIndex") == 0) {
            found_arc_index = true;
        }
    }
    expect_true(found_line_index, "Select type members missing IfcLineIndex");
    expect_true(found_arc_index, "Select type members missing IfcArcIndex");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcPositiveInteger", &positive_integer_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(positive_integer_declaration != NULL, "Named declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_declaration_as_type_declaration(positive_integer_declaration, &positive_integer_type_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(positive_integer_type_declaration != NULL, "Named type declaration cast returned null");
    expect_true(ifcopenshell_ifc_type_declaration_declared_type(positive_integer_type_declaration, &named_parameter_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(named_parameter_type != NULL, "Named parameter type returned null");
    expect_true(ifcopenshell_ifc_parameter_type_kind(named_parameter_type, &parameter_kind), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(parameter_kind.data, "NAMED") == 0, "Unexpected parameter type kind for IfcPositiveInteger");
    expect_true(ifcopenshell_ifc_parameter_type_as_named_type(named_parameter_type, &named_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(named_type != NULL, "Named type cast returned null");
    expect_true(ifcopenshell_ifc_named_type_declared_type(named_type, &named_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(named_declaration != NULL, "Named type declaration target returned null");
    expect_true(ifcopenshell_ifc_declaration_name(named_declaration, &named_declaration_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(named_declaration_name.data, "IfcInteger") == 0, "Unexpected named type target");
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcIdentifier", &identifier_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(identifier_declaration != NULL, "Simple declaration lookup by name returned null");
    expect_true(ifcopenshell_ifc_declaration_as_type_declaration(identifier_declaration, &identifier_type_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(identifier_type_declaration != NULL, "Simple type declaration cast returned null");
    expect_true(ifcopenshell_ifc_type_declaration_declared_type(identifier_type_declaration, &simple_parameter_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(simple_parameter_type != NULL, "Simple parameter type returned null");
    expect_true(ifcopenshell_ifc_parameter_type_kind(simple_parameter_type, &parameter_kind), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(parameter_kind.data, "SIMPLE") == 0, "Unexpected parameter type kind for IfcIdentifier");
    expect_true(ifcopenshell_ifc_parameter_type_as_simple_type(simple_parameter_type, &simple_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(simple_type != NULL, "Simple type cast returned null");
    expect_true(ifcopenshell_ifc_simple_type_kind(simple_type, &simple_kind), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(simple_kind.data, "STRING") == 0, "Unexpected simple type kind");

    expect_true(ifcopenshell_ifc_file_get_total_inverses_by_id(file, 3, &total_inverses), ifcopenshell_ifcparse_last_error_message());
    expect_true(total_inverses == 0u, "Unexpected inverse count by id");
    expect_true(ifcopenshell_ifc_file_get_total_inverses(file, point, &total_inverses_for_instance), ifcopenshell_ifcparse_last_error_message());
    expect_true(total_inverses_for_instance == 1, "Unexpected inverse count for point instance");
    expect_true(ifcopenshell_ifc_file_get_inverse(file, point, &inverse_instances), ifcopenshell_ifcparse_last_error_message());
    expect_true(inverse_instances != NULL, "Inverse instance list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(inverse_instances, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 1u, "Unexpected inverse instance list size");
    expect_true(ifcopenshell_ifcparse_instance_list_get(inverse_instances, 0u, &inverse_instance), ifcopenshell_ifcparse_last_error_message());
    expect_true(inverse_instance != NULL, "Inverse instance is null");
    expect_true(ifcopenshell_ifc_instance_class_name(inverse_instance, false, &class_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(class_name.data, "IfcPolyline") == 0, "Unexpected inverse instance class");
    ifcopenshell_string_destroy(&class_name);

    expect_true(ifcopenshell_ifc_file_types(file, &file_types), ifcopenshell_ifcparse_last_error_message());
    expect_true(file_types.size >= 6u, "Unexpected file types list size");
    for (i = 0; i < file_types.size; ++i) {
        if (strcmp(file_types.items[i].data, "IfcCartesianPoint") == 0) {
            found_cartesian_point_type = true;
        }
        if (strcmp(file_types.items[i].data, "IfcPolyline") == 0) {
            found_polyline_type = true;
        }
    }
    expect_true(found_cartesian_point_type, "IfcCartesianPoint missing from file types");
    expect_true(found_polyline_type, "IfcPolyline missing from file types");

    expect_true(ifcopenshell_ifc_instance_get_argument_type(point, 0u, &argument_type_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(argument_type_name.data, "AGGREGATE OF DOUBLE") == 0, "Unexpected point argument type");
    expect_true(ifcopenshell_ifc_instance_to_string(point, true, &instance_spf), ifcopenshell_ifcparse_last_error_message());
    expect_true(strstr(instance_spf.data, "IFCCARTESIANPOINT") != NULL, "Point SPF output missing class name");
    expect_true(strstr(instance_spf.data, "(0.,0.,0.)") != NULL, "Point SPF output missing coordinates");
    ifcopenshell_string_destroy(&instance_spf);
    expect_true(ifcopenshell_ifc_instance_get_attribute_names(person, &attribute_names), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_names.size >= 4u, "Unexpected person attribute name list size");
    for (i = 0; i < attribute_names.size; ++i) {
        if (strcmp(attribute_names.items[i].data, "FamilyName") == 0) {
            found_family_name_attr = true;
        }
        if (strcmp(attribute_names.items[i].data, "MiddleNames") == 0) {
            found_middle_names_attr = true;
        }
    }
    expect_true(found_family_name_attr, "FamilyName missing from person attribute names");
    expect_true(found_middle_names_attr, "MiddleNames missing from person attribute names");
    expect_true(ifcopenshell_ifc_instance_get_attribute_category(person, "FamilyName", &declaration_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_type == 1, "Unexpected attribute category for FamilyName");
    expect_true(ifcopenshell_ifc_instance_get_inverse_attribute_names(point, &inverse_attribute_names), ifcopenshell_ifcparse_last_error_message());
    for (i = 0; i < inverse_attribute_names.size; ++i) {
        expect_true(inverse_attribute_names.items[i].data != NULL, "Inverse attribute name is null");
        expect_true(inverse_attribute_names.items[i].size > 0u, "Inverse attribute name is empty");
    }
    expect_true(inverse_attribute_names.size > 0u, "Expected inverse attribute names for point");
    expect_true(ifcopenshell_ifc_instance_get_attribute_category(point, inverse_attribute_names.items[0].data, &declaration_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(declaration_type == 2, "Unexpected attribute category for inverse attribute");
    expect_true(ifcopenshell_ifc_instance_get_inverse(point, inverse_attribute_names.items[0].data, &traversed_instances_bfs), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed_instances_bfs != NULL, "Inverse lookup result is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(traversed_instances_bfs, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    ifcopenshell_ifcparse_instance_list_destroy(traversed_instances_bfs);
    traversed_instances_bfs = NULL;
    expect_true(ifcopenshell_ifc_file_get_inverse_indices(file, point, &indexed_face_index_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(indexed_face_index_values.size == 1u, "Unexpected inverse index list size");
    expect_true(ifcopenshell_ifc_file_get_inverse_indices_by_id(file, 1, &inverse_index_values_by_id), ifcopenshell_ifcparse_last_error_message());
    expect_true(inverse_index_values_by_id.size == 1u, "Unexpected inverse index-by-id list size");

    expect_true(ifcopenshell_ifc_instance_get_argument(point_two, 0u, &point_coordinates), ifcopenshell_ifcparse_last_error_message());
    expect_true(point_coordinates != NULL, "Point coordinate attribute value handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_is_null(point_coordinates, &attribute_is_null), ifcopenshell_ifcparse_last_error_message());
    expect_true(!attribute_is_null, "Point coordinate attribute unexpectedly null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(point_coordinates, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF DOUBLE") == 0, "Unexpected point coordinate attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_size(point_coordinates, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 3u, "Unexpected point coordinate attribute size");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_double_list(point_coordinates, &point_coordinate_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(point_coordinate_values.size == 3u, "Unexpected point coordinate value list size");
    expect_true(point_coordinate_values.items != NULL, "Point coordinate value list is null");
    expect_true(point_coordinate_values.items[0] > 0.999 && point_coordinate_values.items[0] < 1.001, "Unexpected first point coordinate");
    expect_true(point_coordinate_values.items[1] == 0.0, "Unexpected second point coordinate");
    expect_true(point_coordinate_values.items[2] == 0.0, "Unexpected third point coordinate");
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(polyline, 0u, &polyline_points), ifcopenshell_ifcparse_last_error_message());
    expect_true(polyline_points != NULL, "Polyline points attribute value handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(polyline_points, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF ENTITY INSTANCE") == 0, "Unexpected polyline points attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_size(polyline_points, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected polyline points attribute size");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_instance_list(polyline_points, &polyline_point_list), ifcopenshell_ifcparse_last_error_message());
    expect_true(polyline_point_list != NULL, "Polyline point list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_reserve(polyline_point_list, 4u), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_instance_list_size(polyline_point_list, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected polyline point list size");
    expect_true(ifcopenshell_ifcparse_instance_list_contains(polyline_point_list, point, &list_contains_point), ifcopenshell_ifcparse_last_error_message());
    expect_true(list_contains_point, "Polyline point list should contain the first point");
    expect_true(ifcopenshell_ifcparse_instance_list_unique(polyline_point_list, &unique_polyline_point_list), ifcopenshell_ifcparse_last_error_message());
    expect_true(unique_polyline_point_list != NULL, "Unique polyline point list is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(unique_polyline_point_list, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected unique polyline point list size");
    expect_true(ifcopenshell_ifcparse_instance_list_get(polyline_point_list, 0u, &aggregate_item), ifcopenshell_ifcparse_last_error_message());
    expect_true(aggregate_item != NULL, "Polyline point list item is null");
    expect_true(ifcopenshell_ifc_instance_class_name(aggregate_item, false, &class_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(class_name.data, "IfcCartesianPoint") == 0, "Unexpected polyline point item class");
    expect_true(ifcopenshell_ifcparse_instance_list_remove(unique_polyline_point_list, point_two), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_instance_list_size(unique_polyline_point_list, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 1u, "Unexpected polyline point list size after remove");
    ifcopenshell_string_destroy(&class_name);
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(person, 1u, &family_name_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(family_name_argument != NULL, "Person family name attribute value handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(family_name_argument, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "STRING") == 0, "Unexpected family name attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_string(family_name_argument, &family_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(family_name.data, "Doe") == 0, "Unexpected family name value");
    ifcopenshell_string_destroy(&family_name);
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(person, 3u, &middle_names_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(middle_names_argument != NULL, "Person middle names attribute value handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(middle_names_argument, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF STRING") == 0, "Unexpected middle names attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_string_list(middle_names_argument, &middle_name_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(middle_name_values.size == 2u, "Unexpected middle names list size");
    expect_true(strcmp(middle_name_values.items[0].data, "Quincy") == 0, "Unexpected first middle name");
    expect_true(strcmp(middle_name_values.items[1].data, "Ray") == 0, "Unexpected second middle name");
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(indexed_polygonal_face, 0u, &indexed_face_indices), ifcopenshell_ifcparse_last_error_message());
    expect_true(indexed_face_indices != NULL, "Indexed polygonal face coordinate index handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(indexed_face_indices, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF INT") == 0, "Unexpected indexed polygonal face attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_int32_list(indexed_face_indices, &indexed_face_index_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(indexed_face_index_values.size == 3u, "Unexpected indexed polygonal face list size");
    expect_true(indexed_face_index_values.items != NULL, "Indexed polygonal face list items are null");
    expect_true(indexed_face_index_values.items[0] == 1, "Unexpected indexed polygonal face first index");
    expect_true(indexed_face_index_values.items[1] == 2, "Unexpected indexed polygonal face second index");
    expect_true(indexed_face_index_values.items[2] == 3, "Unexpected indexed polygonal face third index");
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(texture_vertex_list, 0u, &texture_coords_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(texture_coords_argument != NULL, "Texture vertex list coordinate handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(texture_coords_argument, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF AGGREGATE OF DOUBLE") == 0, "Unexpected texture vertex list attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_double_list_list(texture_coords_argument, &texture_coord_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(texture_coord_values.size == 2u, "Unexpected texture coordinate outer list size");
    expect_true(texture_coord_values.items[0].size == 2u, "Unexpected first texture coordinate list size");
    expect_true(texture_coord_values.items[1].size == 2u, "Unexpected second texture coordinate list size");
    expect_true(texture_coord_values.items[0].items[0] == 0.0, "Unexpected first texture coordinate x");
    expect_true(texture_coord_values.items[0].items[1] == 0.0, "Unexpected first texture coordinate y");
    expect_true(texture_coord_values.items[1].items[0] == 1.0, "Unexpected second texture coordinate x");
    expect_true(texture_coord_values.items[1].items[1] == 1.0, "Unexpected second texture coordinate y");
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifc_instance_get_argument(indexed_polygonal_face_with_voids, 1u, &inner_coord_indices_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(inner_coord_indices_argument != NULL, "Indexed polygonal face with voids inner index handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_type(inner_coord_indices_argument, &attribute_type), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(attribute_type.data, "AGGREGATE OF AGGREGATE OF INT") == 0, "Unexpected inner coordinate index attribute type");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_int32_list_list(inner_coord_indices_argument, &inner_coord_index_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(inner_coord_index_values.size == 1u, "Unexpected inner coordinate outer list size");
    expect_true(inner_coord_index_values.items[0].size == 3u, "Unexpected inner coordinate list size");
    expect_true(inner_coord_index_values.items[0].items[0] == 4, "Unexpected first inner coordinate index");
    expect_true(inner_coord_index_values.items[0].items[1] == 5, "Unexpected second inner coordinate index");
    expect_true(inner_coord_index_values.items[0].items[2] == 6, "Unexpected third inner coordinate index");
    ifcopenshell_string_destroy(&attribute_type);

    expect_true(ifcopenshell_ifcparse_traverse(polyline, 1, &traversed_instances), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed_instances != NULL, "Traverse result is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(traversed_instances, &traversed_count), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed_count == 3u, "Unexpected traverse result size");
    expect_true(ifcopenshell_ifcparse_instance_list_get(traversed_instances, 1u, &traversed), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed != NULL, "Traversed instance handle is null");

    expect_true(ifcopenshell_ifcparse_traverse_breadth_first(polyline, 1, &traversed_instances_bfs), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed_instances_bfs != NULL, "Breadth-first traverse result is null");
    expect_true(ifcopenshell_ifcparse_instance_list_size(traversed_instances_bfs, &traversed_count_bfs), ifcopenshell_ifcparse_last_error_message());
    expect_true(traversed_count_bfs == 3u, "Unexpected breadth-first traverse result size");

    expect_true(ifcopenshell_ifc_instance_get_argument(colour, 1u, &argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(argument != NULL, "Attribute value handle is null");
    expect_true(ifcopenshell_ifcparse_attribute_value_as_double(argument, &coordinate_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(coordinate_value > 0.099 && coordinate_value < 0.101, "Unexpected colour scalar value");

    expect_true(ifcopenshell_ifc_instance_class_name(traversed, false, &class_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(class_name.data, "IfcCartesianPoint") == 0, "Unexpected class name");

    expect_true(ifcopenshell_ifc_file_schema_name(file, &schema_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(schema_name.data, "IFC4") == 0, "Unexpected schema name");
    expect_true(ifcopenshell_ifcparse_stream_from_string(IFC_FIXTURE, &streamer), ifcopenshell_ifcparse_last_error_message());
    expect_true(streamer != NULL, "Instance streamer handle is null");
    expect_true(ifcopenshell_ifc_instance_streamer_status(streamer, &streamer_status), ifcopenshell_ifcparse_last_error_message());
    expect_true(streamer_status == 0, "Unexpected instance streamer status");
    expect_true(ifcopenshell_ifc_instance_streamer_has_semicolon(streamer, &streamer_has_semicolon), ifcopenshell_ifcparse_last_error_message());
    expect_true(streamer_has_semicolon, "Expected streamer to detect semicolons");
    expect_true(ifcopenshell_ifc_instance_streamer_semicolon_count(streamer, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size > 0u, "Expected streamer semicolon count");
    expect_true(ifcopenshell_ifc_instance_streamer_bypassed_instances(streamer, &bypassed_instances), ifcopenshell_ifcparse_last_error_message());
    expect_true(bypassed_instances.size == 0u, "Unexpected bypassed instance count");

    {
        double mutated_point_values_raw[] = {2.5, 3.5, 4.5};
        ifcopenshell_double_list_t mutated_point_values = {mutated_point_values_raw, 3u};
        expect_true(ifcopenshell_ifc_instance_set_argument_double_list(point_two, 0u, &mutated_point_values), ifcopenshell_ifcparse_last_error_message());
        ifcopenshell_ifcparse_attribute_value_destroy(point_coordinates);
        point_coordinates = NULL;
        ifcopenshell_double_list_destroy(&point_coordinate_values);
        point_coordinate_values = (ifcopenshell_double_list_t){0};
        expect_true(ifcopenshell_ifc_instance_get_argument(point_two, 0u, &point_coordinates), ifcopenshell_ifcparse_last_error_message());
        expect_true(ifcopenshell_ifcparse_attribute_value_as_double_list(point_coordinates, &point_coordinate_values), ifcopenshell_ifcparse_last_error_message());
        expect_true(point_coordinate_values.items[0] > 2.499 && point_coordinate_values.items[0] < 2.501, "Unexpected mutated first point coordinate");
        expect_true(point_coordinate_values.items[1] > 3.499 && point_coordinate_values.items[1] < 3.501, "Unexpected mutated second point coordinate");
        expect_true(point_coordinate_values.items[2] > 4.499 && point_coordinate_values.items[2] < 4.501, "Unexpected mutated third point coordinate");
    }

    expect_true(ifcopenshell_ifc_instance_set_argument_string(person, 1u, "Smith"), ifcopenshell_ifcparse_last_error_message());
    ifcopenshell_ifcparse_attribute_value_destroy(family_name_argument);
    family_name_argument = NULL;
    expect_true(ifcopenshell_ifc_instance_get_argument(person, 1u, &family_name_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_string(family_name_argument, &mutated_family_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(mutated_family_name.data, "Smith") == 0, "Unexpected mutated family name");
    ifcopenshell_string_destroy(&mutated_family_name);

    {
        ifcopenshell_string_t middle_name_items[] = {
            {(char*)"Alex", 4u, false},
            {(char*)"Jordan", 6u, false},
        };
        ifcopenshell_string_list_t middle_name_input = {middle_name_items, 2u};
        expect_true(ifcopenshell_ifc_instance_set_argument_string_list(person, 3u, &middle_name_input), ifcopenshell_ifcparse_last_error_message());
        ifcopenshell_ifcparse_attribute_value_destroy(middle_names_argument);
        middle_names_argument = NULL;
        ifcopenshell_string_list_destroy(&middle_name_values);
        middle_name_values = (ifcopenshell_string_list_t){0};
        expect_true(ifcopenshell_ifc_instance_get_argument(person, 3u, &middle_names_argument), ifcopenshell_ifcparse_last_error_message());
        expect_true(ifcopenshell_ifcparse_attribute_value_as_string_list(middle_names_argument, &middle_name_values), ifcopenshell_ifcparse_last_error_message());
        expect_true(strcmp(middle_name_values.items[0].data, "Alex") == 0, "Unexpected mutated first middle name");
        expect_true(strcmp(middle_name_values.items[1].data, "Jordan") == 0, "Unexpected mutated second middle name");
    }

    expect_true(ifcopenshell_ifc_instance_unset_argument(person, 1u), ifcopenshell_ifcparse_last_error_message());
    ifcopenshell_ifcparse_attribute_value_destroy(family_name_argument);
    family_name_argument = NULL;
    expect_true(ifcopenshell_ifc_instance_get_argument(person, 1u, &family_name_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_is_null(family_name_argument, &attribute_is_null), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_is_null, "Family name should be null after unset");

    {
        int32_t reversed_index_values_raw[] = {3, 2, 1};
        ifcopenshell_int32_list_t reversed_index_values = {reversed_index_values_raw, 3u};
        expect_true(ifcopenshell_ifc_instance_set_argument_int32_list(indexed_polygonal_face, 0u, &reversed_index_values), ifcopenshell_ifcparse_last_error_message());
        ifcopenshell_ifcparse_attribute_value_destroy(indexed_face_indices);
        indexed_face_indices = NULL;
        ifcopenshell_int32_list_destroy(&indexed_face_index_values);
        indexed_face_index_values = (ifcopenshell_int32_list_t){0};
        expect_true(ifcopenshell_ifc_instance_get_argument(indexed_polygonal_face, 0u, &indexed_face_indices), ifcopenshell_ifcparse_last_error_message());
        expect_true(ifcopenshell_ifcparse_attribute_value_as_int32_list(indexed_face_indices, &indexed_face_index_values), ifcopenshell_ifcparse_last_error_message());
        expect_true(indexed_face_index_values.items[0] == 3, "Unexpected mutated indexed polygonal face first index");
        expect_true(indexed_face_index_values.items[1] == 2, "Unexpected mutated indexed polygonal face second index");
        expect_true(indexed_face_index_values.items[2] == 1, "Unexpected mutated indexed polygonal face third index");
    }

    {
        double tex0_raw[] = {0.25, 0.5};
        double tex1_raw[] = {0.75, 1.0};
        ifcopenshell_double_list_t texture_lists[] = {
            {tex0_raw, 2u},
            {tex1_raw, 2u},
        };
        ifcopenshell_double_list_list_t texture_input = {texture_lists, 2u};
        expect_true(ifcopenshell_ifc_instance_set_argument_double_list_list(texture_vertex_list, 0u, &texture_input), ifcopenshell_ifcparse_last_error_message());
        ifcopenshell_ifcparse_attribute_value_destroy(texture_coords_argument);
        texture_coords_argument = NULL;
        ifcopenshell_double_list_list_destroy(&texture_coord_values);
        texture_coord_values = (ifcopenshell_double_list_list_t){0};
        expect_true(ifcopenshell_ifc_instance_get_argument(texture_vertex_list, 0u, &texture_coords_argument), ifcopenshell_ifcparse_last_error_message());
        expect_true(ifcopenshell_ifcparse_attribute_value_as_double_list_list(texture_coords_argument, &texture_coord_values), ifcopenshell_ifcparse_last_error_message());
        expect_true(texture_coord_values.items[0].items[0] > 0.249 && texture_coord_values.items[0].items[0] < 0.251, "Unexpected mutated first texture coordinate x");
        expect_true(texture_coord_values.items[0].items[1] > 0.499 && texture_coord_values.items[0].items[1] < 0.501, "Unexpected mutated first texture coordinate y");
        expect_true(texture_coord_values.items[1].items[0] > 0.749 && texture_coord_values.items[1].items[0] < 0.751, "Unexpected mutated second texture coordinate x");
        expect_true(texture_coord_values.items[1].items[1] > 0.999 && texture_coord_values.items[1].items[1] < 1.001, "Unexpected mutated second texture coordinate y");
    }

    {
        int32_t inner0_raw[] = {7, 8, 9};
        int32_t inner1_raw[] = {10, 11, 12};
        ifcopenshell_int32_list_t inner_lists[] = {
            {inner0_raw, 3u},
            {inner1_raw, 3u},
        };
        ifcopenshell_int32_list_list_t inner_index_input = {inner_lists, 2u};
        expect_true(ifcopenshell_ifc_instance_set_argument_int32_list_list(indexed_polygonal_face_with_voids, 1u, &inner_index_input), ifcopenshell_ifcparse_last_error_message());
        ifcopenshell_ifcparse_attribute_value_destroy(inner_coord_indices_argument);
        inner_coord_indices_argument = NULL;
        ifcopenshell_int32_list_list_destroy(&inner_coord_index_values);
        inner_coord_index_values = (ifcopenshell_int32_list_list_t){0};
        expect_true(ifcopenshell_ifc_instance_get_argument(indexed_polygonal_face_with_voids, 1u, &inner_coord_indices_argument), ifcopenshell_ifcparse_last_error_message());
        expect_true(ifcopenshell_ifcparse_attribute_value_as_int32_list_list(inner_coord_indices_argument, &inner_coord_index_values), ifcopenshell_ifcparse_last_error_message());
        expect_true(inner_coord_index_values.size == 2u, "Unexpected mutated inner coordinate outer list size");
        expect_true(inner_coord_index_values.items[1].items[2] == 12, "Unexpected mutated inner coordinate last index");
    }

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcInteger", &created_integer), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_int32(created_integer, 0u, 42), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_integer, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_int32(mutated_argument, &wrapped_integer_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(wrapped_integer_value == 42, "Unexpected mutated wrapped integer value");
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcBoolean", &created_boolean), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_bool(created_boolean, 0u, true), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_boolean, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_bool(mutated_argument, &wrapped_boolean_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(wrapped_boolean_value, "Unexpected mutated wrapped boolean value");
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcReal", &created_real), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_double(created_real, 0u, 6.25), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_real, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_double(mutated_argument, &wrapped_real_value), ifcopenshell_ifcparse_last_error_message());
    expect_true(wrapped_real_value > 6.249 && wrapped_real_value < 6.251, "Unexpected mutated wrapped real value");
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcActorRole", &created_actor_role), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_schema_declaration_by_name(schema_lookup, "IfcRoleEnum", &role_enum_declaration), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_declaration_as_enumeration_type(role_enum_declaration, &role_enumeration), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_enumeration_lookup_enum_offset(role_enumeration, "ARCHITECT", &architect_role_index), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_enumeration(created_actor_role, 0u, role_enumeration, architect_role_index), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_string(created_actor_role, 2u, "Lead architect"), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_actor_role, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_enumeration_value(mutated_argument, &actor_role_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(actor_role_name.data, "ARCHITECT") == 0, "Unexpected mutated actor role enumeration");
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;
    expect_true(ifcopenshell_ifc_instance_get_argument(created_actor_role, 2u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_string(mutated_argument, &actor_role_description), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(actor_role_description.data, "Lead architect") == 0, "Unexpected mutated actor role description");
    ifcopenshell_string_destroy(&actor_role_description);
    ifcopenshell_string_destroy(&actor_role_name);
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcAxis2Placement2D", &created_axis2_placement), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_instance(created_axis2_placement, 0u, point), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_axis2_placement, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_instance(mutated_argument, &axis_location), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_class_name(axis_location, false, &class_name), ifcopenshell_ifcparse_last_error_message());
    expect_true(strcmp(class_name.data, "IfcCartesianPoint") == 0, "Unexpected mutated axis placement location class");
    ifcopenshell_string_destroy(&class_name);
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;

    expect_true(ifcopenshell_ifcparse_new_instance("IFC4", "IfcPolyline", &created_polyline), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_set_argument_instance_list(created_polyline, 0u, polyline_point_list), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifc_instance_get_argument(created_polyline, 0u, &mutated_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_instance_list(mutated_argument, &traversed_instances_bfs), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_instance_list_size(traversed_instances_bfs, &attribute_size), ifcopenshell_ifcparse_last_error_message());
    expect_true(attribute_size == 2u, "Unexpected mutated created polyline point list size");
    ifcopenshell_ifcparse_attribute_value_destroy(mutated_argument);
    mutated_argument = NULL;
    ifcopenshell_ifcparse_instance_list_destroy(traversed_instances_bfs);
    traversed_instances_bfs = NULL;

    expect_true(ifcopenshell_ifc_instance_to_string(point_two, true, &instance_spf), ifcopenshell_ifcparse_last_error_message());
    expect_true(strstr(instance_spf.data, "2.5") != NULL, "Mutated point SPF output missing first coordinate");
    ifcopenshell_string_destroy(&instance_spf);

    expect_true(ifcopenshell_ifc_file_write(file, output_path), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_open(output_path, false, &reopened_file), ifcopenshell_ifcparse_last_error_message());
    expect_true(reopened_file != NULL, "Reopened file handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(reopened_file, 2u, &reopened_point_two), ifcopenshell_ifcparse_last_error_message());
    expect_true(reopened_point_two != NULL, "Reopened point instance handle is null");
    expect_true(ifcopenshell_ifc_file_by_id(reopened_file, 4u, &reopened_person), ifcopenshell_ifcparse_last_error_message());
    expect_true(reopened_person != NULL, "Reopened person instance handle is null");
    expect_true(ifcopenshell_ifc_instance_get_argument(reopened_point_two, 0u, &reopened_point_coordinates), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_as_double_list(reopened_point_coordinates, &reopened_point_coordinate_values), ifcopenshell_ifcparse_last_error_message());
    expect_true(reopened_point_coordinate_values.size == 3u, "Unexpected reopened point coordinate list size");
    expect_true(reopened_point_coordinate_values.items[0] > 2.499 && reopened_point_coordinate_values.items[0] < 2.501, "Unexpected reopened first point coordinate");
    expect_true(reopened_point_coordinate_values.items[1] > 3.499 && reopened_point_coordinate_values.items[1] < 3.501, "Unexpected reopened second point coordinate");
    expect_true(reopened_point_coordinate_values.items[2] > 4.499 && reopened_point_coordinate_values.items[2] < 4.501, "Unexpected reopened third point coordinate");
    expect_true(ifcopenshell_ifc_instance_get_argument(reopened_person, 1u, &reopened_family_name_argument), ifcopenshell_ifcparse_last_error_message());
    expect_true(ifcopenshell_ifcparse_attribute_value_is_null(reopened_family_name_argument, &reopened_attribute_is_null), ifcopenshell_ifcparse_last_error_message());
    expect_true(reopened_attribute_is_null, "Reopened family name should be null after unset");

    ifcopenshell_ifc_declaration_destroy(role_enum_declaration);
    role_enum_declaration = NULL;
    ifcopenshell_ifc_enumeration_destroy(role_enumeration);
    role_enumeration = NULL;

    ifcopenshell_int32_list_list_destroy(&inner_coord_index_values);
    ifcopenshell_int32_list_destroy(&inverse_index_values_by_id);
    ifcopenshell_int32_list_destroy(&indexed_face_index_values);
    ifcopenshell_uint32_list_destroy(&entity_ids);
    ifcopenshell_uint32_list_destroy(&bypassed_instances);
    ifcopenshell_bool_list_destroy(&entity_derived);
    ifcopenshell_double_list_list_destroy(&texture_coord_values);
    ifcopenshell_double_list_destroy(&point_coordinate_values);
    ifcopenshell_double_list_destroy(&reopened_point_coordinate_values);
    ifcopenshell_string_list_destroy(&middle_name_values);
    ifcopenshell_string_list_destroy(&header_description);
    ifcopenshell_string_list_destroy(&header_authors);
    ifcopenshell_string_list_destroy(&header_schema_identifiers);
    ifcopenshell_string_list_destroy(&enumeration_argument_types);
    ifcopenshell_string_list_destroy(&type_declaration_argument_types);
    ifcopenshell_string_list_destroy(&entity_argument_types);
    ifcopenshell_string_list_destroy(&inverse_attribute_names);
    ifcopenshell_string_list_destroy(&attribute_names);
    ifcopenshell_string_list_destroy(&file_types);
    ifcopenshell_ifc_type_declaration_list_destroy(&schema_type_declarations);
    ifcopenshell_ifc_select_type_list_destroy(&schema_select_types);
    ifcopenshell_ifc_enumeration_list_destroy(&schema_enumeration_types);
    ifcopenshell_ifc_attribute_list_destroy(&entity_attributes);
    ifcopenshell_ifc_entity_list_destroy(&schema_entities);
    ifcopenshell_ifc_declaration_list_destroy(&schema_declarations);
    ifcopenshell_ifcparse_attribute_value_destroy(inner_coord_indices_argument);
    ifcopenshell_ifcparse_attribute_value_destroy(texture_coords_argument);
    ifcopenshell_ifcparse_attribute_value_destroy(middle_names_argument);
    ifcopenshell_ifcparse_attribute_value_destroy(indexed_face_indices);
    ifcopenshell_ifcparse_attribute_value_destroy(argument);
    ifcopenshell_ifcparse_attribute_value_destroy(family_name_argument);
    ifcopenshell_ifcparse_attribute_value_destroy(reopened_family_name_argument);
    ifcopenshell_ifcparse_attribute_value_destroy(reopened_point_coordinates);
    ifcopenshell_ifcparse_instance_list_destroy(polyline_point_list);
    ifcopenshell_ifcparse_instance_list_destroy(unique_polyline_point_list);
    ifcopenshell_ifcparse_instance_list_destroy(instances_by_reference);
    ifcopenshell_ifcparse_instance_list_destroy(instances_by_type_excl_subtypes);
    ifcopenshell_ifcparse_instance_list_destroy(instances_by_type);
    ifcopenshell_ifcparse_instance_list_destroy(inverse_instances);
    ifcopenshell_ifc_instance_destroy(aggregate_item);
    ifcopenshell_ifc_instance_destroy(inverse_instance);
    ifcopenshell_ifcparse_attribute_value_destroy(polyline_points);
    ifcopenshell_ifcparse_attribute_value_destroy(point_coordinates);
    ifcopenshell_ifc_aggregation_type_destroy(aggregation_type);
    ifcopenshell_ifc_entity_destroy(entity);
    ifcopenshell_ifc_simple_type_destroy(simple_type);
    ifcopenshell_ifc_named_type_destroy(named_type);
    ifcopenshell_ifc_parameter_type_destroy(simple_parameter_type);
    ifcopenshell_ifc_parameter_type_destroy(named_parameter_type);
    ifcopenshell_ifc_parameter_type_destroy(element_parameter_type);
    ifcopenshell_ifc_parameter_type_destroy(parameter_type);
    ifcopenshell_ifc_select_type_destroy(select_type);
    ifcopenshell_ifc_type_declaration_destroy(identifier_type_declaration);
    ifcopenshell_ifc_type_declaration_destroy(positive_integer_type_declaration);
    ifcopenshell_ifc_type_declaration_destroy(type_declaration);
    ifcopenshell_ifc_instance_destroy(traversed);
    ifcopenshell_ifcparse_instance_list_destroy(traversed_instances_bfs);
    ifcopenshell_ifcparse_instance_list_destroy(traversed_instances);
    ifcopenshell_ifc_enumeration_destroy(enumeration);
    ifcopenshell_ifc_declaration_destroy(named_declaration);
    ifcopenshell_ifc_declaration_destroy(identifier_declaration);
    ifcopenshell_ifc_declaration_destroy(positive_integer_declaration);
    ifcopenshell_ifc_declaration_destroy(segment_index_select_declaration);
    ifcopenshell_ifc_declaration_destroy(line_index_declaration);
    ifcopenshell_ifc_declaration_destroy(enum_declaration);
    ifcopenshell_ifc_declaration_destroy(declaration_by_index);
    ifcopenshell_ifc_declaration_destroy(declaration_by_name);
    ifcopenshell_ifc_declaration_destroy(declaration);
    ifcopenshell_ifc_schema_destroy(declaration_schema);
    ifcopenshell_ifc_schema_destroy(schema_lookup);
    ifcopenshell_ifc_schema_destroy(schema);
    ifcopenshell_ifc_instance_streamer_destroy(streamer);
    ifcopenshell_ifc_file_schema_destroy(file_schema);
    ifcopenshell_ifc_file_name_destroy(file_name);
    ifcopenshell_ifc_file_description_destroy(file_description);
    ifcopenshell_string_destroy(&declaration_schema_name);
    ifcopenshell_string_destroy(&declaration_name);
    ifcopenshell_string_destroy(&declaration_name_uc);
    ifcopenshell_string_destroy(&aggregation_kind);
    ifcopenshell_string_destroy(&aggregation_type_name);
    ifcopenshell_string_destroy(&simple_kind);
    ifcopenshell_string_destroy(&parameter_kind);
    ifcopenshell_string_destroy(&named_declaration_name);
    ifcopenshell_string_destroy(&attribute_name);
    ifcopenshell_string_destroy(&argument_name);
    ifcopenshell_string_destroy(&enum_value);
    ifcopenshell_string_destroy(&file_spf);
    ifcopenshell_string_destroy(&schema_lookup_name);
    ifcopenshell_string_list_destroy(&select_list_names);
    ifcopenshell_string_list_destroy(&enumeration_items);
    ifcopenshell_string_list_destroy(&schema_names);
    ifcopenshell_ifc_header_destroy(header);
    ifcopenshell_string_destroy(&schema_name);
    ifcopenshell_string_destroy(&class_name);
    ifcopenshell_string_destroy(&instance_spf);
    ifcopenshell_string_destroy(&argument_type_name);
    ifcopenshell_ifc_instance_destroy(axis_location);
    ifcopenshell_ifc_instance_destroy(created_polyline);
    ifcopenshell_ifc_instance_destroy(created_axis2_placement);
    ifcopenshell_ifc_instance_destroy(created_actor_role);
    ifcopenshell_ifc_instance_destroy(created_real);
    ifcopenshell_ifc_instance_destroy(created_boolean);
    ifcopenshell_ifc_instance_destroy(created_integer);
    ifcopenshell_ifc_instance_destroy(person);
    ifcopenshell_ifc_instance_destroy(polyline);
    ifcopenshell_ifc_instance_destroy(indexed_polygonal_face_with_voids);
    ifcopenshell_ifc_instance_destroy(point_two);
    ifcopenshell_ifc_instance_destroy(point);
    ifcopenshell_ifc_instance_destroy(texture_vertex_list);
    ifcopenshell_ifc_instance_destroy(indexed_polygonal_face);
    ifcopenshell_ifc_instance_destroy(colour);
    ifcopenshell_ifc_instance_destroy(reopened_person);
    ifcopenshell_ifc_instance_destroy(reopened_point_two);
    ifcopenshell_ifc_file_destroy(reopened_file);
    ifcopenshell_ifc_file_destroy(file);
    remove(output_path);
    ifcopenshell_string_destroy(&version);
    return 0;
}
