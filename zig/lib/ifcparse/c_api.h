#ifndef IFCOPENSHELL_ZIG_IFCPARSE_C_API_H
#define IFCOPENSHELL_ZIG_IFCPARSE_C_API_H

#include <stddef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_ifcparse_file ifcopenshell_ifcparse_file_t;
typedef struct ifcopenshell_ifcparse_entity_ref ifcopenshell_ifcparse_entity_ref_t;
typedef struct ifcopenshell_ifcparse_entity_list ifcopenshell_ifcparse_entity_list_t;
typedef struct ifcopenshell_ifcparse_int_list ifcopenshell_ifcparse_int_list_t;
typedef struct ifcopenshell_ifcparse_double_list ifcopenshell_ifcparse_double_list_t;
typedef struct ifcopenshell_ifcparse_int_matrix ifcopenshell_ifcparse_int_matrix_t;
typedef struct ifcopenshell_ifcparse_double_matrix ifcopenshell_ifcparse_double_matrix_t;
typedef struct ifcopenshell_ifcparse_entity_matrix ifcopenshell_ifcparse_entity_matrix_t;
typedef struct ifcopenshell_ifcparse_type_ref ifcopenshell_ifcparse_type_ref_t;
typedef struct ifcopenshell_ifcparse_type_list ifcopenshell_ifcparse_type_list_t;
typedef struct ifcopenshell_ifcparse_string_list ifcopenshell_ifcparse_string_list_t;
typedef struct ifcopenshell_ifcparse_attribute_ref ifcopenshell_ifcparse_attribute_ref_t;
typedef struct ifcopenshell_ifcparse_attribute_list ifcopenshell_ifcparse_attribute_list_t;
typedef struct ifcopenshell_ifcparse_inverse_attribute_ref ifcopenshell_ifcparse_inverse_attribute_ref_t;
typedef struct ifcopenshell_ifcparse_inverse_attribute_list ifcopenshell_ifcparse_inverse_attribute_list_t;
typedef struct ifcopenshell_ifcparse_parameter_type_ref ifcopenshell_ifcparse_parameter_type_ref_t;

typedef enum ifcopenshell_ifcparse_status {
    IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS = 0,
    IFCOPENSHELL_IFCPARSE_STATUS_READ_ERROR = 1,
    IFCOPENSHELL_IFCPARSE_STATUS_NO_HEADER = 2,
    IFCOPENSHELL_IFCPARSE_STATUS_UNSUPPORTED_SCHEMA = 3,
    IFCOPENSHELL_IFCPARSE_STATUS_INVALID_SYNTAX = 4,
    IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN = 5,
} ifcopenshell_ifcparse_status_t;

typedef enum ifcopenshell_ifcparse_filetype {
    IFCOPENSHELL_IFCPARSE_FT_IFCSPF = 0,
    IFCOPENSHELL_IFCPARSE_FT_IFCXML = 1,
    IFCOPENSHELL_IFCPARSE_FT_IFCZIP = 2,
    IFCOPENSHELL_IFCPARSE_FT_ROCKSDB = 3,
    IFCOPENSHELL_IFCPARSE_FT_UNKNOWN = 4,
    IFCOPENSHELL_IFCPARSE_FT_AUTODETECT = 5,
} ifcopenshell_ifcparse_filetype_t;

typedef enum ifcopenshell_ifcparse_argument_type {
    IFCOPENSHELL_IFCPARSE_ARG_NULL = 0,
    IFCOPENSHELL_IFCPARSE_ARG_DERIVED = 1,
    IFCOPENSHELL_IFCPARSE_ARG_INT = 2,
    IFCOPENSHELL_IFCPARSE_ARG_BOOL = 3,
    IFCOPENSHELL_IFCPARSE_ARG_LOGICAL = 4,
    IFCOPENSHELL_IFCPARSE_ARG_DOUBLE = 5,
    IFCOPENSHELL_IFCPARSE_ARG_STRING = 6,
    IFCOPENSHELL_IFCPARSE_ARG_BINARY = 7,
    IFCOPENSHELL_IFCPARSE_ARG_ENUMERATION = 8,
    IFCOPENSHELL_IFCPARSE_ARG_ENTITY_INSTANCE = 9,
    IFCOPENSHELL_IFCPARSE_ARG_EMPTY_AGGREGATE = 10,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_INT = 11,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_DOUBLE = 12,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_STRING = 13,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_BINARY = 14,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_ENTITY_INSTANCE = 15,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_EMPTY_AGGREGATE = 16,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_INT = 17,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_DOUBLE = 18,
    IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE = 19,
    IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN = 20
} ifcopenshell_ifcparse_argument_type_t;

typedef enum ifcopenshell_ifcparse_declaration_kind {
    IFCOPENSHELL_IFCPARSE_DECL_UNKNOWN = 0,
    IFCOPENSHELL_IFCPARSE_DECL_TYPE_DECLARATION = 1,
    IFCOPENSHELL_IFCPARSE_DECL_SELECT_TYPE = 2,
    IFCOPENSHELL_IFCPARSE_DECL_ENUMERATION_TYPE = 3,
    IFCOPENSHELL_IFCPARSE_DECL_ENTITY = 4,
} ifcopenshell_ifcparse_declaration_kind_t;

typedef enum ifcopenshell_ifcparse_parameter_type_kind {
    IFCOPENSHELL_IFCPARSE_PARAM_UNKNOWN = 0,
    IFCOPENSHELL_IFCPARSE_PARAM_NAMED = 1,
    IFCOPENSHELL_IFCPARSE_PARAM_SIMPLE = 2,
    IFCOPENSHELL_IFCPARSE_PARAM_AGGREGATION = 3,
} ifcopenshell_ifcparse_parameter_type_kind_t;

typedef enum ifcopenshell_ifcparse_simple_type {
    IFCOPENSHELL_IFCPARSE_SIMPLE_BINARY = 0,
    IFCOPENSHELL_IFCPARSE_SIMPLE_BOOLEAN = 1,
    IFCOPENSHELL_IFCPARSE_SIMPLE_INTEGER = 2,
    IFCOPENSHELL_IFCPARSE_SIMPLE_LOGICAL = 3,
    IFCOPENSHELL_IFCPARSE_SIMPLE_NUMBER = 4,
    IFCOPENSHELL_IFCPARSE_SIMPLE_REAL = 5,
    IFCOPENSHELL_IFCPARSE_SIMPLE_STRING = 6,
    IFCOPENSHELL_IFCPARSE_SIMPLE_INVALID = 7,
} ifcopenshell_ifcparse_simple_type_t;

typedef enum ifcopenshell_ifcparse_aggregation_type {
    IFCOPENSHELL_IFCPARSE_AGGR_ARRAY = 0,
    IFCOPENSHELL_IFCPARSE_AGGR_BAG = 1,
    IFCOPENSHELL_IFCPARSE_AGGR_LIST = 2,
    IFCOPENSHELL_IFCPARSE_AGGR_SET = 3,
    IFCOPENSHELL_IFCPARSE_AGGR_INVALID = 4,
} ifcopenshell_ifcparse_aggregation_type_t;

typedef enum ifcopenshell_ifcparse_inverse_aggregation_type {
    IFCOPENSHELL_IFCPARSE_INV_AGGR_BAG = 0,
    IFCOPENSHELL_IFCPARSE_INV_AGGR_SET = 1,
    IFCOPENSHELL_IFCPARSE_INV_AGGR_UNSPECIFIED = 2,
    IFCOPENSHELL_IFCPARSE_INV_AGGR_INVALID = 3,
} ifcopenshell_ifcparse_inverse_aggregation_type_t;

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_version(void);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_filetype_t ifcopenshell_ifcparse_guess_file_type(const char* path);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_create_timestamp(void);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_create_empty(
    const char* schema_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open_from_memory(
    const char* data,
    size_t length
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open(
    const char* path,
    ifcopenshell_ifcparse_filetype_t filetype,
    int readonly
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_file_close(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_status_t ifcopenshell_ifcparse_file_status(const ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_file_status_message(ifcopenshell_ifcparse_status_t status);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_file_schema_name(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_description(
    ifcopenshell_ifcparse_file_t* file
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_name(
    ifcopenshell_ifcparse_file_t* file
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_schema(
    ifcopenshell_ifcparse_file_t* file
);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_file_entity_count(const ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_entities(
    ifcopenshell_ifcparse_file_t* file
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_file_entity_ids(
    ifcopenshell_ifcparse_file_t* file
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_guid(
    ifcopenshell_ifcparse_file_t* file,
    const char* guid
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int include_subtypes
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_reference(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_get_inverse(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    const char* type_name,
    int attribute_index
);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_file_get_total_inverses(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_file_get_inverse_indices(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_traverse_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    int max_level,
    int breadth_first
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_traverse(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int max_level,
    int breadth_first
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type_with_id(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int id
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_add_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int id
);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_file_add_entities(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* const* entities,
    size_t entity_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_file_remove_entity_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_file_remove_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_file_batch_begin(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_file_batch_end(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE unsigned int ifcopenshell_ifcparse_file_max_id(const ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_file_recalculate_id_counter(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_file_write_spf(
    ifcopenshell_ifcparse_file_t* file,
    const char* path
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_file_to_spf(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE double ifcopenshell_ifcparse_file_unit_scale(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_unit_entity(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_entity_list_close(ifcopenshell_ifcparse_entity_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_entity_list_count(const ifcopenshell_ifcparse_entity_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_entity_list_reset(ifcopenshell_ifcparse_entity_list_t* list);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_get(
    const ifcopenshell_ifcparse_entity_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_next(
    ifcopenshell_ifcparse_entity_list_t* list
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_id(const ifcopenshell_ifcparse_entity_ref_t* entity);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_entity_type_name(const ifcopenshell_ifcparse_entity_ref_t* entity);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_entity_type_name_with_schema(
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_is_a(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* type_name
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_attribute_category(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* attribute_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_attribute_names(
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_inverse_attribute_names(
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_entity_argument_count(
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_argument_index(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* argument_name
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_entity_argument_name(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_declared_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_value_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_argument_is_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_entity_to_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int valid_spf
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_get_argument_as_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_get_argument_as_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_get_argument_as_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_get_argument_as_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double* out_value
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_entity_get_argument_as_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_get_argument_as_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_entity_get_argument_as_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_double_list_t* ifcopenshell_ifcparse_entity_get_argument_as_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_get_argument_as_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_entity_get_argument_as_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_entity_get_inverse(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* inverse_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_int_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_int_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_double_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_double_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_entity_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_entity_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_unset_argument(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* value
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const int* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const double* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* const* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* const* values,
    size_t value_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_int_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const int* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_double_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const double* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_entity_set_argument_entity_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* const* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_int_list_close(ifcopenshell_ifcparse_int_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_int_list_count(const ifcopenshell_ifcparse_int_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_int_list_reset(ifcopenshell_ifcparse_int_list_t* list);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_int_list_get(
    const ifcopenshell_ifcparse_int_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_int_list_next(ifcopenshell_ifcparse_int_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_double_list_close(ifcopenshell_ifcparse_double_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_double_list_count(const ifcopenshell_ifcparse_double_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_double_list_reset(ifcopenshell_ifcparse_double_list_t* list);

EMSCRIPTEN_KEEPALIVE double ifcopenshell_ifcparse_double_list_get(
    const ifcopenshell_ifcparse_double_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE double ifcopenshell_ifcparse_double_list_next(ifcopenshell_ifcparse_double_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_int_matrix_close(ifcopenshell_ifcparse_int_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_int_matrix_row_count(const ifcopenshell_ifcparse_int_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_int_matrix_col_count(
    const ifcopenshell_ifcparse_int_matrix_t* matrix,
    size_t row
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_int_matrix_get(
    const ifcopenshell_ifcparse_int_matrix_t* matrix,
    size_t row,
    size_t col
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_double_matrix_close(ifcopenshell_ifcparse_double_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_double_matrix_row_count(const ifcopenshell_ifcparse_double_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_double_matrix_col_count(
    const ifcopenshell_ifcparse_double_matrix_t* matrix,
    size_t row
);

EMSCRIPTEN_KEEPALIVE double ifcopenshell_ifcparse_double_matrix_get(
    const ifcopenshell_ifcparse_double_matrix_t* matrix,
    size_t row,
    size_t col
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_entity_matrix_close(ifcopenshell_ifcparse_entity_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_entity_matrix_row_count(const ifcopenshell_ifcparse_entity_matrix_t* matrix);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_entity_matrix_col_count(
    const ifcopenshell_ifcparse_entity_matrix_t* matrix,
    size_t row
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_matrix_get(
    const ifcopenshell_ifcparse_entity_matrix_t* matrix,
    size_t row,
    size_t col
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_file_types(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_type_list_close(ifcopenshell_ifcparse_type_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_type_list_count(const ifcopenshell_ifcparse_type_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_type_list_reset(ifcopenshell_ifcparse_type_list_t* list);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_get(
    const ifcopenshell_ifcparse_type_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_next(
    ifcopenshell_ifcparse_type_list_t* list
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_type_name(const ifcopenshell_ifcparse_type_ref_t* type_ref);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_type_index(const ifcopenshell_ifcparse_type_ref_t* type_ref);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_schema_declaration_by_name(
    const char* schema_name,
    const char* declaration_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_schema_entities(
    const char* schema_name
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_type_schema_name(const ifcopenshell_ifcparse_type_ref_t* type_ref);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_type_is_a(
    const ifcopenshell_ifcparse_type_ref_t* type_ref,
    const char* type_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_declaration_kind_t ifcopenshell_ifcparse_type_kind(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_type_is_abstract(const ifcopenshell_ifcparse_type_ref_t* type_ref);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_supertype(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_type_subtypes(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_declared_type(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_type_select_list(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_type_enumeration_items(
    const ifcopenshell_ifcparse_type_ref_t* type_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_type_attribute_index(
    const ifcopenshell_ifcparse_type_ref_t* type_ref,
    const char* attribute_name
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_attribute_list_t* ifcopenshell_ifcparse_type_attributes(
    const ifcopenshell_ifcparse_type_ref_t* type_ref,
    int include_inherited
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_inverse_attribute_list_t* ifcopenshell_ifcparse_type_inverse_attributes(
    const ifcopenshell_ifcparse_type_ref_t* type_ref,
    int include_inherited
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_attribute_list_close(ifcopenshell_ifcparse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_attribute_list_count(const ifcopenshell_ifcparse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_attribute_list_reset(ifcopenshell_ifcparse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_attribute_ref_t* ifcopenshell_ifcparse_attribute_list_get(
    const ifcopenshell_ifcparse_attribute_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_attribute_ref_t* ifcopenshell_ifcparse_attribute_list_next(
    ifcopenshell_ifcparse_attribute_list_t* list
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_attribute_name(const ifcopenshell_ifcparse_attribute_ref_t* attribute_ref);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_attribute_optional(const ifcopenshell_ifcparse_attribute_ref_t* attribute_ref);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_parameter_type_ref_t* ifcopenshell_ifcparse_attribute_parameter_type(
    const ifcopenshell_ifcparse_attribute_ref_t* attribute_ref
);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_inverse_attribute_list_close(ifcopenshell_ifcparse_inverse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_inverse_attribute_list_count(const ifcopenshell_ifcparse_inverse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_inverse_attribute_list_reset(ifcopenshell_ifcparse_inverse_attribute_list_t* list);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_inverse_attribute_ref_t* ifcopenshell_ifcparse_inverse_attribute_list_get(
    const ifcopenshell_ifcparse_inverse_attribute_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_inverse_attribute_ref_t* ifcopenshell_ifcparse_inverse_attribute_list_next(
    ifcopenshell_ifcparse_inverse_attribute_list_t* list
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_inverse_attribute_name(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_inverse_aggregation_type_t ifcopenshell_ifcparse_inverse_attribute_aggregation_type(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_inverse_attribute_bound1(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_inverse_attribute_bound2(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_inverse_attribute_entity_reference(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_attribute_ref_t* ifcopenshell_ifcparse_inverse_attribute_attribute_reference(
    const ifcopenshell_ifcparse_inverse_attribute_ref_t* inverse_attribute_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_parameter_type_kind_t ifcopenshell_ifcparse_parameter_type_kind(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_parameter_type_named_declared_type(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_simple_type_t ifcopenshell_ifcparse_parameter_type_simple_declared_type(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_aggregation_type_t ifcopenshell_ifcparse_parameter_type_aggregation_type(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_parameter_type_aggregation_bound1(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE int ifcopenshell_ifcparse_parameter_type_aggregation_bound2(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE const ifcopenshell_ifcparse_parameter_type_ref_t* ifcopenshell_ifcparse_parameter_type_aggregation_element_type(
    const ifcopenshell_ifcparse_parameter_type_ref_t* parameter_type_ref
);

EMSCRIPTEN_KEEPALIVE ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_schema_names(void);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_string_list_close(ifcopenshell_ifcparse_string_list_t* list);

EMSCRIPTEN_KEEPALIVE size_t ifcopenshell_ifcparse_string_list_count(const ifcopenshell_ifcparse_string_list_t* list);

EMSCRIPTEN_KEEPALIVE void ifcopenshell_ifcparse_string_list_reset(ifcopenshell_ifcparse_string_list_t* list);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_string_list_get(
    const ifcopenshell_ifcparse_string_list_t* list,
    size_t index
);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_string_list_next(ifcopenshell_ifcparse_string_list_t* list);

EMSCRIPTEN_KEEPALIVE void* ifcopenshell_ifcparse_file_native(ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE const void* ifcopenshell_ifcparse_file_native_const(const ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE void* ifcopenshell_ifcparse_entity_native(const ifcopenshell_ifcparse_entity_ref_t* entity);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_file_last_error(const ifcopenshell_ifcparse_file_t* file);

EMSCRIPTEN_KEEPALIVE const char* ifcopenshell_ifcparse_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
