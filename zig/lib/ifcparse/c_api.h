#ifndef IFCOPENSHELL_ZIG_IFCPARSE_C_API_H
#define IFCOPENSHELL_ZIG_IFCPARSE_C_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ifcopenshell_ifcparse_file ifcopenshell_ifcparse_file_t;
typedef struct ifcopenshell_ifcparse_entity_ref ifcopenshell_ifcparse_entity_ref_t;
typedef struct ifcopenshell_ifcparse_entity_list ifcopenshell_ifcparse_entity_list_t;
typedef struct ifcopenshell_ifcparse_int_list ifcopenshell_ifcparse_int_list_t;
typedef struct ifcopenshell_ifcparse_double_list ifcopenshell_ifcparse_double_list_t;
typedef struct ifcopenshell_ifcparse_type_ref ifcopenshell_ifcparse_type_ref_t;
typedef struct ifcopenshell_ifcparse_type_list ifcopenshell_ifcparse_type_list_t;
typedef struct ifcopenshell_ifcparse_string_list ifcopenshell_ifcparse_string_list_t;

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

const char* ifcopenshell_ifcparse_version(void);

ifcopenshell_ifcparse_filetype_t ifcopenshell_ifcparse_guess_file_type(const char* path);

const char* ifcopenshell_ifcparse_create_timestamp(void);

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_create_empty(
    const char* schema_name
);

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open_from_memory(
    const char* data,
    size_t length
);

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open(
    const char* path,
    ifcopenshell_ifcparse_filetype_t filetype,
    int readonly
);

void ifcopenshell_ifcparse_file_close(ifcopenshell_ifcparse_file_t* file);

ifcopenshell_ifcparse_status_t ifcopenshell_ifcparse_file_status(const ifcopenshell_ifcparse_file_t* file);

const char* ifcopenshell_ifcparse_file_status_message(ifcopenshell_ifcparse_status_t status);

const char* ifcopenshell_ifcparse_file_schema_name(ifcopenshell_ifcparse_file_t* file);

size_t ifcopenshell_ifcparse_file_entity_count(const ifcopenshell_ifcparse_file_t* file);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_entities(
    ifcopenshell_ifcparse_file_t* file
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_guid(
    ifcopenshell_ifcparse_file_t* file,
    const char* guid
);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int include_subtypes
);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_reference(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_get_inverse(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    const char* type_name,
    int attribute_index
);

size_t ifcopenshell_ifcparse_file_get_total_inverses(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
);

ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_file_get_inverse_indices(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_traverse_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    int max_level,
    int breadth_first
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type_with_id(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int id
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_add_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int id
);

int ifcopenshell_ifcparse_file_remove_entity_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
);

int ifcopenshell_ifcparse_file_remove_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

void ifcopenshell_ifcparse_file_batch_begin(ifcopenshell_ifcparse_file_t* file);

void ifcopenshell_ifcparse_file_batch_end(ifcopenshell_ifcparse_file_t* file);

unsigned int ifcopenshell_ifcparse_file_max_id(const ifcopenshell_ifcparse_file_t* file);

void ifcopenshell_ifcparse_file_recalculate_id_counter(ifcopenshell_ifcparse_file_t* file);

int ifcopenshell_ifcparse_file_write_spf(
    ifcopenshell_ifcparse_file_t* file,
    const char* path
);

const char* ifcopenshell_ifcparse_file_to_spf(ifcopenshell_ifcparse_file_t* file);

double ifcopenshell_ifcparse_file_unit_scale(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_unit_entity(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
);

void ifcopenshell_ifcparse_entity_list_close(ifcopenshell_ifcparse_entity_list_t* list);

size_t ifcopenshell_ifcparse_entity_list_count(const ifcopenshell_ifcparse_entity_list_t* list);

void ifcopenshell_ifcparse_entity_list_reset(ifcopenshell_ifcparse_entity_list_t* list);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_get(
    const ifcopenshell_ifcparse_entity_list_t* list,
    size_t index
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_next(
    ifcopenshell_ifcparse_entity_list_t* list
);

int ifcopenshell_ifcparse_entity_id(const ifcopenshell_ifcparse_entity_ref_t* entity);

const char* ifcopenshell_ifcparse_entity_type_name(const ifcopenshell_ifcparse_entity_ref_t* entity);

int ifcopenshell_ifcparse_entity_is_a(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* type_name
);

size_t ifcopenshell_ifcparse_entity_argument_count(
    const ifcopenshell_ifcparse_entity_ref_t* entity
);

int ifcopenshell_ifcparse_entity_argument_index(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* argument_name
);

const char* ifcopenshell_ifcparse_entity_argument_name(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_declared_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_value_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

int ifcopenshell_ifcparse_entity_argument_is_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

const char* ifcopenshell_ifcparse_entity_to_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int valid_spf
);

int ifcopenshell_ifcparse_entity_get_argument_as_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

int ifcopenshell_ifcparse_entity_get_argument_as_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

int ifcopenshell_ifcparse_entity_get_argument_as_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
);

int ifcopenshell_ifcparse_entity_get_argument_as_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double* out_value
);

const char* ifcopenshell_ifcparse_entity_get_argument_as_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_get_argument_as_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_entity_get_argument_as_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_double_list_t* ifcopenshell_ifcparse_entity_get_argument_as_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_get_argument_as_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_entity_get_argument_as_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

int ifcopenshell_ifcparse_entity_set_argument_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

int ifcopenshell_ifcparse_entity_unset_argument(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
);

int ifcopenshell_ifcparse_entity_set_argument_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

int ifcopenshell_ifcparse_entity_set_argument_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

int ifcopenshell_ifcparse_entity_set_argument_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
);

int ifcopenshell_ifcparse_entity_set_argument_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double value
);

int ifcopenshell_ifcparse_entity_set_argument_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* value
);

int ifcopenshell_ifcparse_entity_set_argument_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* value
);

int ifcopenshell_ifcparse_entity_set_argument_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const int* values,
    size_t value_count
);

int ifcopenshell_ifcparse_entity_set_argument_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const double* values,
    size_t value_count
);

int ifcopenshell_ifcparse_entity_set_argument_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* const* values,
    size_t value_count
);

int ifcopenshell_ifcparse_entity_set_argument_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* const* values,
    size_t value_count
);

void ifcopenshell_ifcparse_int_list_close(ifcopenshell_ifcparse_int_list_t* list);

size_t ifcopenshell_ifcparse_int_list_count(const ifcopenshell_ifcparse_int_list_t* list);

void ifcopenshell_ifcparse_int_list_reset(ifcopenshell_ifcparse_int_list_t* list);

int ifcopenshell_ifcparse_int_list_get(
    const ifcopenshell_ifcparse_int_list_t* list,
    size_t index
);

int ifcopenshell_ifcparse_int_list_next(ifcopenshell_ifcparse_int_list_t* list);

void ifcopenshell_ifcparse_double_list_close(ifcopenshell_ifcparse_double_list_t* list);

size_t ifcopenshell_ifcparse_double_list_count(const ifcopenshell_ifcparse_double_list_t* list);

void ifcopenshell_ifcparse_double_list_reset(ifcopenshell_ifcparse_double_list_t* list);

double ifcopenshell_ifcparse_double_list_get(
    const ifcopenshell_ifcparse_double_list_t* list,
    size_t index
);

double ifcopenshell_ifcparse_double_list_next(ifcopenshell_ifcparse_double_list_t* list);

ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_file_types(ifcopenshell_ifcparse_file_t* file);

void ifcopenshell_ifcparse_type_list_close(ifcopenshell_ifcparse_type_list_t* list);

size_t ifcopenshell_ifcparse_type_list_count(const ifcopenshell_ifcparse_type_list_t* list);

void ifcopenshell_ifcparse_type_list_reset(ifcopenshell_ifcparse_type_list_t* list);

const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_get(
    const ifcopenshell_ifcparse_type_list_t* list,
    size_t index
);

const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_next(
    ifcopenshell_ifcparse_type_list_t* list
);

const char* ifcopenshell_ifcparse_type_name(const ifcopenshell_ifcparse_type_ref_t* type_ref);

int ifcopenshell_ifcparse_type_index(const ifcopenshell_ifcparse_type_ref_t* type_ref);

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_schema_names(void);

void ifcopenshell_ifcparse_string_list_close(ifcopenshell_ifcparse_string_list_t* list);

size_t ifcopenshell_ifcparse_string_list_count(const ifcopenshell_ifcparse_string_list_t* list);

void ifcopenshell_ifcparse_string_list_reset(ifcopenshell_ifcparse_string_list_t* list);

const char* ifcopenshell_ifcparse_string_list_get(
    const ifcopenshell_ifcparse_string_list_t* list,
    size_t index
);

const char* ifcopenshell_ifcparse_string_list_next(ifcopenshell_ifcparse_string_list_t* list);

const char* ifcopenshell_ifcparse_file_last_error(const ifcopenshell_ifcparse_file_t* file);

const char* ifcopenshell_ifcparse_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
