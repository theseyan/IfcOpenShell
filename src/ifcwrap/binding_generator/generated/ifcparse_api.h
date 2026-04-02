// This file was generated with the assistance of an AI coding tool.

#ifndef IFCOPENSHELL_IFCPARSE_API_H
#define IFCOPENSHELL_IFCPARSE_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_string_t {
    char* data;
    size_t size;
    bool owned;
} ifcopenshell_string_t;

typedef struct ifcopenshell_string_list_t {
    ifcopenshell_string_t* items;
    size_t size;
} ifcopenshell_string_list_t;

typedef struct ifcopenshell_bool_list_t {
    bool* items;
    size_t size;
} ifcopenshell_bool_list_t;

typedef struct ifcopenshell_int32_list_t {
    int32_t* items;
    size_t size;
} ifcopenshell_int32_list_t;

typedef struct ifcopenshell_uint32_list_t {
    uint32_t* items;
    size_t size;
} ifcopenshell_uint32_list_t;

typedef struct ifcopenshell_int32_list_list_t {
    ifcopenshell_int32_list_t* items;
    size_t size;
} ifcopenshell_int32_list_list_t;

typedef struct ifcopenshell_double_list_t {
    double* items;
    size_t size;
} ifcopenshell_double_list_t;

typedef struct ifcopenshell_double_list_list_t {
    ifcopenshell_double_list_t* items;
    size_t size;
} ifcopenshell_double_list_list_t;

typedef struct ifcopenshell_ifc_file_t ifcopenshell_ifc_file_t;
typedef struct ifcopenshell_ifc_instance_streamer_t ifcopenshell_ifc_instance_streamer_t;
typedef struct ifcopenshell_ifc_instance_t ifcopenshell_ifc_instance_t;
typedef struct ifcopenshell_ifc_header_t ifcopenshell_ifc_header_t;
typedef struct ifcopenshell_ifc_file_description_t ifcopenshell_ifc_file_description_t;
typedef struct ifcopenshell_ifc_file_name_t ifcopenshell_ifc_file_name_t;
typedef struct ifcopenshell_ifc_file_schema_t ifcopenshell_ifc_file_schema_t;
typedef struct ifcopenshell_ifc_declaration_t ifcopenshell_ifc_declaration_t;
typedef struct ifcopenshell_ifc_type_declaration_t ifcopenshell_ifc_type_declaration_t;
typedef struct ifcopenshell_ifc_select_type_t ifcopenshell_ifc_select_type_t;
typedef struct ifcopenshell_ifc_schema_t ifcopenshell_ifc_schema_t;
typedef struct ifcopenshell_ifc_enumeration_t ifcopenshell_ifc_enumeration_t;
typedef struct ifcopenshell_ifc_parameter_type_t ifcopenshell_ifc_parameter_type_t;
typedef struct ifcopenshell_ifc_named_type_t ifcopenshell_ifc_named_type_t;
typedef struct ifcopenshell_ifc_simple_type_t ifcopenshell_ifc_simple_type_t;
typedef struct ifcopenshell_ifc_aggregation_type_t ifcopenshell_ifc_aggregation_type_t;
typedef struct ifcopenshell_ifc_entity_t ifcopenshell_ifc_entity_t;
typedef struct ifcopenshell_ifc_attribute_t ifcopenshell_ifc_attribute_t;
typedef struct ifcopenshell_ifc_inverse_attribute_t ifcopenshell_ifc_inverse_attribute_t;
typedef struct ifcopenshell_ifcparse_attribute_value_t ifcopenshell_ifcparse_attribute_value_t;
typedef struct ifcopenshell_ifcparse_instance_list_t ifcopenshell_ifcparse_instance_list_t;

typedef struct ifcopenshell_ifc_declaration_list_t {
    ifcopenshell_ifc_declaration_t** items;
    size_t size;
} ifcopenshell_ifc_declaration_list_t;
typedef struct ifcopenshell_ifc_entity_list_t {
    ifcopenshell_ifc_entity_t** items;
    size_t size;
} ifcopenshell_ifc_entity_list_t;
typedef struct ifcopenshell_ifc_enumeration_list_t {
    ifcopenshell_ifc_enumeration_t** items;
    size_t size;
} ifcopenshell_ifc_enumeration_list_t;
typedef struct ifcopenshell_ifc_select_type_list_t {
    ifcopenshell_ifc_select_type_t** items;
    size_t size;
} ifcopenshell_ifc_select_type_list_t;
typedef struct ifcopenshell_ifc_type_declaration_list_t {
    ifcopenshell_ifc_type_declaration_t** items;
    size_t size;
} ifcopenshell_ifc_type_declaration_list_t;
typedef struct ifcopenshell_ifc_attribute_list_t {
    ifcopenshell_ifc_attribute_t** items;
    size_t size;
} ifcopenshell_ifc_attribute_list_t;
typedef struct ifcopenshell_ifc_inverse_attribute_list_t {
    ifcopenshell_ifc_inverse_attribute_t** items;
    size_t size;
} ifcopenshell_ifc_inverse_attribute_list_t;

void ifcopenshell_string_destroy(ifcopenshell_string_t* value);
void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value);
void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value);
void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value);
void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value);
void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value);
void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value);
void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value);
void ifcopenshell_ifcparse_clear_error(void);
const char* ifcopenshell_ifcparse_last_error_message(void);

void ifcopenshell_ifc_file_destroy(ifcopenshell_ifc_file_t* handle);
void ifcopenshell_ifc_instance_streamer_destroy(ifcopenshell_ifc_instance_streamer_t* handle);
void ifcopenshell_ifc_instance_destroy(ifcopenshell_ifc_instance_t* handle);
void ifcopenshell_ifc_header_destroy(ifcopenshell_ifc_header_t* handle);
void ifcopenshell_ifc_file_description_destroy(ifcopenshell_ifc_file_description_t* handle);
void ifcopenshell_ifc_file_name_destroy(ifcopenshell_ifc_file_name_t* handle);
void ifcopenshell_ifc_file_schema_destroy(ifcopenshell_ifc_file_schema_t* handle);
void ifcopenshell_ifc_declaration_destroy(ifcopenshell_ifc_declaration_t* handle);
void ifcopenshell_ifc_type_declaration_destroy(ifcopenshell_ifc_type_declaration_t* handle);
void ifcopenshell_ifc_select_type_destroy(ifcopenshell_ifc_select_type_t* handle);
void ifcopenshell_ifc_schema_destroy(ifcopenshell_ifc_schema_t* handle);
void ifcopenshell_ifc_enumeration_destroy(ifcopenshell_ifc_enumeration_t* handle);
void ifcopenshell_ifc_parameter_type_destroy(ifcopenshell_ifc_parameter_type_t* handle);
void ifcopenshell_ifc_named_type_destroy(ifcopenshell_ifc_named_type_t* handle);
void ifcopenshell_ifc_simple_type_destroy(ifcopenshell_ifc_simple_type_t* handle);
void ifcopenshell_ifc_aggregation_type_destroy(ifcopenshell_ifc_aggregation_type_t* handle);
void ifcopenshell_ifc_entity_destroy(ifcopenshell_ifc_entity_t* handle);
void ifcopenshell_ifc_attribute_destroy(ifcopenshell_ifc_attribute_t* handle);
void ifcopenshell_ifc_inverse_attribute_destroy(ifcopenshell_ifc_inverse_attribute_t* handle);
void ifcopenshell_ifcparse_attribute_value_destroy(ifcopenshell_ifcparse_attribute_value_t* handle);
void ifcopenshell_ifcparse_instance_list_destroy(ifcopenshell_ifcparse_instance_list_t* handle);
void ifcopenshell_ifc_declaration_list_destroy(ifcopenshell_ifc_declaration_list_t* value);
void ifcopenshell_ifc_entity_list_destroy(ifcopenshell_ifc_entity_list_t* value);
void ifcopenshell_ifc_enumeration_list_destroy(ifcopenshell_ifc_enumeration_list_t* value);
void ifcopenshell_ifc_select_type_list_destroy(ifcopenshell_ifc_select_type_list_t* value);
void ifcopenshell_ifc_type_declaration_list_destroy(ifcopenshell_ifc_type_declaration_list_t* value);
void ifcopenshell_ifc_attribute_list_destroy(ifcopenshell_ifc_attribute_list_t* value);
void ifcopenshell_ifc_inverse_attribute_list_destroy(ifcopenshell_ifc_inverse_attribute_list_t* value);

bool ifcopenshell_ifcparse_clear_schemas(void);
bool ifcopenshell_ifcparse_register_schema(ifcopenshell_ifc_schema_t* arg_0);
bool ifcopenshell_ifcparse_schema_by_name(const char* arg_0, ifcopenshell_ifc_schema_t** out_result);
bool ifcopenshell_ifcparse_schema_names(ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifcparse_traverse_breadth_first(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifcparse_open(const char* path, bool readonly, ifcopenshell_ifc_file_t** out_result);
bool ifcopenshell_ifcparse_new_file(const char* schema_identifier, int32_t file_type, const char* path, ifcopenshell_ifc_file_t** out_result);
bool ifcopenshell_ifcparse_read_memory(const char* data, ifcopenshell_ifc_file_t** out_result);
bool ifcopenshell_ifcparse_stream_from_string(const char* data, ifcopenshell_ifc_instance_streamer_t** out_result);
bool ifcopenshell_ifcparse_version(ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_guess_file_type(const char* path, int32_t* out_result);
bool ifcopenshell_ifcparse_set_feature(const char* name, bool value);
bool ifcopenshell_ifcparse_get_feature(const char* name, bool* out_result);
bool ifcopenshell_ifcparse_get_log(ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_turn_on_detailed_logging(void);
bool ifcopenshell_ifcparse_turn_off_detailed_logging(void);
bool ifcopenshell_ifcparse_set_log_format_json(void);
bool ifcopenshell_ifcparse_set_log_format_text(void);
bool ifcopenshell_ifcparse_ifc_si_prefix_to_value(const char* prefix, double* out_result);
bool ifcopenshell_ifcparse_get_si_equivalent(ifcopenshell_ifc_instance_t* named_unit, double* out_result);
bool ifcopenshell_ifcparse_get_info_cpp(ifcopenshell_ifc_instance_t* instance, bool include_identifier, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_operator_token_ptr(size_t start, const char* data, int32_t* out_result);
bool ifcopenshell_ifcparse_general_token_ptr(size_t start, const char* token, int32_t* out_result);
bool ifcopenshell_ifcparse_new_instance(const char* schema_identifier, const char* declaration_name, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_file_create(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t* decl, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_file_get_inverses_by_declaration(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_ifc_declaration_t* type, int32_t attribute_index, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_by_type(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_by_type_excl_subtypes(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_fresh_id(ifcopenshell_ifc_file_t* self, uint32_t* out_result);
bool ifcopenshell_ifc_file_add_entities(ifcopenshell_ifc_file_t* self, ifcopenshell_ifcparse_instance_list_t* entities);
bool ifcopenshell_ifc_file_add(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity, int32_t id, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_file_add_type_ref(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* new_entity);
bool ifcopenshell_ifc_file_batch(ifcopenshell_ifc_file_t* self);
bool ifcopenshell_ifc_file_build_inverses(ifcopenshell_ifc_file_t* self);
bool ifcopenshell_ifc_file_build_inverses_(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* arg_0);
bool ifcopenshell_ifc_file_bypass_type(ifcopenshell_ifc_file_t* self, const char* type_name);
bool ifcopenshell_ifc_file_create_timestamp(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_get_max_id(ifcopenshell_ifc_file_t* self, uint32_t* out_result);
bool ifcopenshell_ifc_file_get_total_inverses_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, size_t* out_result);
bool ifcopenshell_ifc_file_get_inverse_indices_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_ifc_file_ifcroot_type(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t** out_result);
bool ifcopenshell_ifc_file_by_guid(ifcopenshell_ifc_file_t* self, const char* guid, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_file_instances_by_reference(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_process_deletion_inverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* inst);
bool ifcopenshell_ifc_file_recalculate_id_counter(ifcopenshell_ifc_file_t* self);
bool ifcopenshell_ifc_file_remove(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity);
bool ifcopenshell_ifc_file_remove_type_ref(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* new_entity);
bool ifcopenshell_ifc_file_reset_identity_cache(ifcopenshell_ifc_file_t* self);
bool ifcopenshell_ifc_file_schema(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_schema_t** out_result);
bool ifcopenshell_ifc_file_traverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_traverse_breadth_first(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_unbatch(ifcopenshell_ifc_file_t* self);
bool ifcopenshell_ifc_instance_declaration(ifcopenshell_ifc_instance_t* self, ifcopenshell_ifc_declaration_t** out_result);
bool ifcopenshell_ifc_instance_get_argument(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_attribute_value_t** out_result);
bool ifcopenshell_ifc_instance_id(ifcopenshell_ifc_instance_t* self, uint32_t* out_result);
bool ifcopenshell_ifc_instance_identity(ifcopenshell_ifc_instance_t* self, uint32_t* out_result);
bool ifcopenshell_ifc_schema_declaration_by_name(ifcopenshell_ifc_schema_t* self, const char* name, ifcopenshell_ifc_declaration_t** out_result);
bool ifcopenshell_ifc_schema_declaration_by_index(ifcopenshell_ifc_schema_t* self, size_t name, ifcopenshell_ifc_declaration_t** out_result);
bool ifcopenshell_ifc_schema_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_list_t* out_result);
bool ifcopenshell_ifc_schema_entities(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_entity_list_t* out_result);
bool ifcopenshell_ifc_schema_enumeration_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_enumeration_list_t* out_result);
bool ifcopenshell_ifc_schema_name(ifcopenshell_ifc_schema_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_schema_select_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_select_type_list_t* out_result);
bool ifcopenshell_ifc_schema_type_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_type_declaration_list_t* out_result);
bool ifcopenshell_ifc_declaration_is_a(ifcopenshell_ifc_declaration_t* self, const char* name, bool* out_result);
bool ifcopenshell_ifc_declaration_as_entity(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_declaration_as_enumeration_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_enumeration_t** out_result);
bool ifcopenshell_ifc_declaration_as_select_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_select_type_t** out_result);
bool ifcopenshell_ifc_declaration_as_type_declaration(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result);
bool ifcopenshell_ifc_declaration_index_in_schema(ifcopenshell_ifc_declaration_t* self, int32_t* out_result);
bool ifcopenshell_ifc_declaration_name(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_declaration_name_uc(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_declaration_schema(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_schema_t** out_result);
bool ifcopenshell_ifc_declaration_type(ifcopenshell_ifc_declaration_t* self, int32_t* out_result);
bool ifcopenshell_ifc_type_declaration_as_type_declaration(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result);
bool ifcopenshell_ifc_type_declaration_declared_type(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_parameter_type_t** out_result);
bool ifcopenshell_ifc_select_type_as_select_type(ifcopenshell_ifc_select_type_t* self, ifcopenshell_ifc_select_type_t** out_result);
bool ifcopenshell_ifc_select_type_select_list(ifcopenshell_ifc_select_type_t* self, ifcopenshell_ifc_declaration_list_t* out_result);
bool ifcopenshell_ifc_enumeration_as_enumeration_type(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_ifc_enumeration_t** out_result);
bool ifcopenshell_ifc_enumeration_enumeration_items(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_enumeration_lookup_enum_offset(ifcopenshell_ifc_enumeration_t* self, const char* string, size_t* out_result);
bool ifcopenshell_ifc_enumeration_lookup_enum_value(ifcopenshell_ifc_enumeration_t* self, size_t i, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_parameter_type_as_aggregation_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result);
bool ifcopenshell_ifc_parameter_type_as_named_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_named_type_t** out_result);
bool ifcopenshell_ifc_parameter_type_as_simple_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_simple_type_t** out_result);
bool ifcopenshell_ifc_named_type_is_a(ifcopenshell_ifc_named_type_t* self, const char* name, bool* out_result);
bool ifcopenshell_ifc_named_type_as_named_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_named_type_t** out_result);
bool ifcopenshell_ifc_named_type_declared_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_declaration_t** out_result);
bool ifcopenshell_ifc_simple_type_as_simple_type(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_ifc_simple_type_t** out_result);
bool ifcopenshell_ifc_aggregation_type_as_aggregation_type(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result);
bool ifcopenshell_ifc_aggregation_type_bound1(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_ifc_aggregation_type_bound2(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_ifc_aggregation_type_type_of_element(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_parameter_type_t** out_result);
bool ifcopenshell_ifc_header_file(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_t** out_result);
bool ifcopenshell_ifc_header_file_description(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_description_t** out_result);
bool ifcopenshell_ifc_header_file_name(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_name_t** out_result);
bool ifcopenshell_ifc_header_file_schema(ifcopenshell_ifc_header_t* self, ifcopenshell_ifc_file_schema_t** out_result);
bool ifcopenshell_ifc_header_read(ifcopenshell_ifc_header_t* self);
bool ifcopenshell_ifc_header_try_read(ifcopenshell_ifc_header_t* self, bool* out_result);
bool ifcopenshell_ifc_file_description_class(ifcopenshell_ifc_file_description_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_description_declaration(ifcopenshell_ifc_file_description_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_description_description(ifcopenshell_ifc_file_description_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_file_description_implementation_level(ifcopenshell_ifc_file_description_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_description_setdescription(ifcopenshell_ifc_file_description_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_ifc_file_description_setimplementation_level(ifcopenshell_ifc_file_description_t* self, const char* v);
bool ifcopenshell_ifc_file_name_class(ifcopenshell_ifc_file_name_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_name_author(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_file_name_authorization(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_name_declaration(ifcopenshell_ifc_file_name_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_name_name(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_name_organization(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_file_name_originating_system(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_name_preprocessor_version(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_name_setauthor(ifcopenshell_ifc_file_name_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_ifc_file_name_setauthorization(ifcopenshell_ifc_file_name_t* self, const char* v);
bool ifcopenshell_ifc_file_name_setname(ifcopenshell_ifc_file_name_t* self, const char* v);
bool ifcopenshell_ifc_file_name_setorganization(ifcopenshell_ifc_file_name_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_ifc_file_name_setoriginating_system(ifcopenshell_ifc_file_name_t* self, const char* v);
bool ifcopenshell_ifc_file_name_setpreprocessor_version(ifcopenshell_ifc_file_name_t* self, const char* v);
bool ifcopenshell_ifc_file_name_settime_stamp(ifcopenshell_ifc_file_name_t* self, const char* v);
bool ifcopenshell_ifc_file_name_time_stamp(ifcopenshell_ifc_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_schema_class(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_schema_declaration(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_file_schema_schema_identifiers(ifcopenshell_ifc_file_schema_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_file_schema_setschema_identifiers(ifcopenshell_ifc_file_schema_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_ifc_entity_attribute_index(ifcopenshell_ifc_entity_t* self, const char* attr_name, int32_t* out_result);
bool ifcopenshell_ifc_entity_all_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_attribute_list_t* out_result);
bool ifcopenshell_ifc_entity_all_inverse_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_inverse_attribute_list_t* out_result);
bool ifcopenshell_ifc_entity_as_entity(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_entity_attribute_by_index(ifcopenshell_ifc_entity_t* self, size_t index, ifcopenshell_ifc_attribute_t** out_result);
bool ifcopenshell_ifc_entity_attribute_count(ifcopenshell_ifc_entity_t* self, size_t* out_result);
bool ifcopenshell_ifc_entity_attributes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_attribute_list_t* out_result);
bool ifcopenshell_ifc_entity_derived(ifcopenshell_ifc_entity_t* self, ifcopenshell_bool_list_t* out_result);
bool ifcopenshell_ifc_entity_is_abstract(ifcopenshell_ifc_entity_t* self, bool* out_result);
bool ifcopenshell_ifc_entity_set_attributes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_attribute_list_t* attributes, const ifcopenshell_bool_list_t* derived);
bool ifcopenshell_ifc_entity_set_inverse_attributes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_inverse_attribute_list_t* inverse_attributes);
bool ifcopenshell_ifc_entity_set_subtypes(ifcopenshell_ifc_entity_t* self, const ifcopenshell_ifc_entity_list_t* subtypes);
bool ifcopenshell_ifc_entity_subtypes(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_list_t* out_result);
bool ifcopenshell_ifc_entity_supertype(ifcopenshell_ifc_entity_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_attribute_name(ifcopenshell_ifc_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_attribute_optional(ifcopenshell_ifc_attribute_t* self, bool* out_result);
bool ifcopenshell_ifc_attribute_type_of_attribute(ifcopenshell_ifc_attribute_t* self, ifcopenshell_ifc_parameter_type_t** out_result);
bool ifcopenshell_ifc_inverse_attribute_attribute_reference(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_ifc_attribute_t** out_result);
bool ifcopenshell_ifc_inverse_attribute_bound1(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_ifc_inverse_attribute_bound2(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_ifc_inverse_attribute_entity_reference(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_ifc_entity_t** out_result);
bool ifcopenshell_ifc_inverse_attribute_name(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_streamer_bypassed_instances(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_uint32_list_t* out_result);
bool ifcopenshell_ifc_instance_streamer_has_semicolon(ifcopenshell_ifc_instance_streamer_t* self, bool* out_result);
bool ifcopenshell_ifc_instance_streamer_push_page(ifcopenshell_ifc_instance_streamer_t* self, const char* page);
bool ifcopenshell_ifc_instance_streamer_semicolon_count(ifcopenshell_ifc_instance_streamer_t* self, size_t* out_result);
bool ifcopenshell_ifcparse_instance_list_contains(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifc_instance_t* arg_0, bool* out_result);
bool ifcopenshell_ifcparse_instance_list_remove(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifc_instance_t* arg_0);
bool ifcopenshell_ifcparse_instance_list_reserve(ifcopenshell_ifcparse_instance_list_t* self, size_t capacity);
bool ifcopenshell_ifcparse_instance_list_size(ifcopenshell_ifcparse_instance_list_t* self, size_t* out_result);
bool ifcopenshell_ifcparse_instance_list_unique(ifcopenshell_ifcparse_instance_list_t* self, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_initialize(ifcopenshell_ifc_file_t* self, const char* path, int32_t file_type, bool readonly, bool* out_result);
bool ifcopenshell_ifc_file_write(ifcopenshell_ifc_file_t* self, const char* path);
bool ifcopenshell_ifc_file_storage_mode(ifcopenshell_ifc_file_t* self, int32_t* out_result);
bool ifcopenshell_ifc_file_schema_name(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_get_inverse(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_file_get_total_inverses(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, int32_t* out_result);
bool ifcopenshell_ifc_file_types(ifcopenshell_ifc_file_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_file_to_string(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_entity_names(ifcopenshell_ifc_file_t* self, ifcopenshell_uint32_list_t* out_result);
bool ifcopenshell_ifc_file_file_pointer(ifcopenshell_ifc_file_t* self, size_t* out_result);
bool ifcopenshell_ifc_file_get_inverse_indices(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* instance, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_ifc_file_good(ifcopenshell_ifc_file_t* self, int32_t* out_result);
bool ifcopenshell_ifc_file_get_unit(ifcopenshell_ifc_file_t* self, const char* unit_type, double* out_result);
bool ifcopenshell_ifc_file_key_value_store_query(ifcopenshell_ifc_file_t* self, const char* key, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_file_key_value_store_iter(ifcopenshell_ifc_file_t* self, const char* prefix, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_instance_file_pointer(ifcopenshell_ifc_instance_t* self, size_t* out_result);
bool ifcopenshell_ifc_instance_data(ifcopenshell_ifc_instance_t* self, size_t* out_result);
bool ifcopenshell_ifc_instance_get_argument_index(ifcopenshell_ifc_instance_t* self, const char* name, uint32_t* out_result);
bool ifcopenshell_ifc_instance_get_argument_name(ifcopenshell_ifc_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_get_attribute_category(ifcopenshell_ifc_instance_t* self, const char* name, int32_t* out_result);
bool ifcopenshell_ifc_instance_set_attribute_value(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_attribute_value_t* value);
bool ifcopenshell_ifc_instance_unset_attribute_value(ifcopenshell_ifc_instance_t* self, const char* name);
bool ifcopenshell_ifc_instance_get_inverse(ifcopenshell_ifc_instance_t* self, const char* name, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifc_instance_get_attribute_value(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_attribute_value_t** out_result);
bool ifcopenshell_ifc_instance_get_argument_type(ifcopenshell_ifc_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_to_string(ifcopenshell_ifc_instance_t* self, bool valid_spf, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_class_name(ifcopenshell_ifc_instance_t* self, bool with_schema, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_is_a(ifcopenshell_ifc_instance_t* self, const char* declaration_name, bool* out_result);
bool ifcopenshell_ifc_instance_get_attribute_names(ifcopenshell_ifc_instance_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_instance_get_inverse_attribute_names(ifcopenshell_ifc_instance_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_instance_unset_argument(ifcopenshell_ifc_instance_t* self, size_t index);
bool ifcopenshell_ifc_instance_set_argument_bool(ifcopenshell_ifc_instance_t* self, size_t index, bool value);
bool ifcopenshell_ifc_instance_set_argument_int32(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value);
bool ifcopenshell_ifc_instance_set_argument_double(ifcopenshell_ifc_instance_t* self, size_t index, double value);
bool ifcopenshell_ifc_instance_set_argument_string(ifcopenshell_ifc_instance_t* self, size_t index, const char* value);
bool ifcopenshell_ifc_instance_set_argument_instance(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_instance_t* value);
bool ifcopenshell_ifc_instance_set_argument_instance_list(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_instance_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_int32_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_double_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_string_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_string_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_int32_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_double_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_logical(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value);
bool ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value);
bool ifcopenshell_ifc_instance_set_argument_enumeration(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_enumeration_t* enumeration, size_t enumeration_index);
bool ifcopenshell_ifcparse_attribute_value_is_null(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result);
bool ifcopenshell_ifcparse_attribute_value_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_size(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_int32(ifcopenshell_ifcparse_attribute_value_t* self, int32_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_bool(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_double(ifcopenshell_ifcparse_attribute_value_t* self, double* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_int32_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_double_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_int32_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_double_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_string_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_string(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_instance(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifcparse_attribute_value_as_instance_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifcparse_instance_list_t** out_result);
bool ifcopenshell_ifcparse_attribute_value_as_enumeration_value(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_enumeration_index(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result);
bool ifcopenshell_ifcparse_attribute_value_as_enumeration_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_enumeration_t** out_result);
bool ifcopenshell_ifc_file_header(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_header_t** out_result);
bool ifcopenshell_ifc_header_write(ifcopenshell_ifc_header_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_schema_instantiate(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_t* declaration, ifcopenshell_ifc_instance_t** out_result);
bool ifcopenshell_ifc_select_type_select_list_names(ifcopenshell_ifc_select_type_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_type_declaration_argument_types(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_enumeration_argument_types(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_parameter_type_kind(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_simple_type_declared_type(ifcopenshell_ifc_simple_type_t* self, int32_t* out_result);
bool ifcopenshell_ifc_simple_type_kind(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_aggregation_type_type_of_aggregation(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_ifc_aggregation_type_type_of_aggregation_string(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_inverse_attribute_type_of_aggregation(ifcopenshell_ifc_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_ifc_inverse_attribute_type_of_aggregation_string(ifcopenshell_ifc_inverse_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_entity_argument_types(ifcopenshell_ifc_entity_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_ifc_instance_streamer_bypass_types(ifcopenshell_ifc_instance_streamer_t* self, const ifcopenshell_string_list_t* type_names);
bool ifcopenshell_ifc_instance_streamer_status(ifcopenshell_ifc_instance_streamer_t* self, int32_t* out_result);
bool ifcopenshell_ifc_instance_streamer_references(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_streamer_inverses(ifcopenshell_ifc_instance_streamer_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_instance_streamer_read_instance_py(ifcopenshell_ifc_instance_streamer_t* self, bool type_as_declaration_instance, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifc_aggregation_type_kind(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_ifcparse_instance_list_get(ifcopenshell_ifcparse_instance_list_t* self, size_t index, ifcopenshell_ifc_instance_t** out_result);

#ifdef __cplusplus
}
#endif

#endif
