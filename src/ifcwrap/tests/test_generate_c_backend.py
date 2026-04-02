# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

import pytest

from src.ifcwrap.binding_generator.generate_c_backend import generate


def _compile_commands() -> Path:
    path = Path("/tmp/ifcopenshell-capi-build/compile_commands.json")
    if not path.exists():
        pytest.skip("compile_commands.json is required for AST-backed generation tests")
    return path


def test_generate_ifcparse_c_backend(tmp_path: Path) -> None:
    spec = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    header_out = tmp_path / "ifcparse_api.h"
    cpp_out = tmp_path / "ifcparse_api.cpp"

    generate(spec, header_out, cpp_out, compile_commands_path=_compile_commands())

    header = header_out.read_text(encoding="utf-8")
    cpp = cpp_out.read_text(encoding="utf-8")

    assert "typedef struct ifcopenshell_ifc_file_t ifcopenshell_ifc_file_t;" in header
    assert "typedef struct ifcopenshell_ifc_declaration_list_t {" in header
    assert "typedef struct ifcopenshell_ifc_enumeration_list_t {" in header
    assert "typedef struct ifcopenshell_ifc_select_type_list_t {" in header
    assert "typedef struct ifcopenshell_ifc_type_declaration_list_t {" in header
    assert "typedef struct ifcopenshell_int32_list_t {" in header
    assert "typedef struct ifcopenshell_uint32_list_t {" in header
    assert "typedef struct ifcopenshell_int32_list_list_t {" in header
    assert "typedef struct ifcopenshell_double_list_t {" in header
    assert "typedef struct ifcopenshell_double_list_list_t {" in header
    assert "bool ifcopenshell_ifcparse_open(const char* path, bool readonly, ifcopenshell_ifc_file_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_new_file(const char* schema_identifier, int32_t file_type, const char* path, ifcopenshell_ifc_file_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_schema_by_name(const char* arg_0, ifcopenshell_ifc_schema_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_schema_names(ifcopenshell_string_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_by_type(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_by_type_excl_subtypes(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_instances_by_reference(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_add(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity, int32_t id, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_create(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t* decl, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_entity_names(ifcopenshell_ifc_file_t* self, ifcopenshell_uint32_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_file_to_string(ifcopenshell_ifc_file_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_file_get_total_inverses_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, size_t* out_result);" in header
    assert "bool ifcopenshell_ifc_file_header(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_header_t** out_result);" in header
    assert "bool ifcopenshell_ifc_file_schema(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_schema_t** out_result);" in header
    assert "bool ifcopenshell_ifc_schema_name(ifcopenshell_ifc_schema_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_schema_declaration_by_name(ifcopenshell_ifc_schema_t* self, const char* name, ifcopenshell_ifc_declaration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_schema_declaration_by_index(ifcopenshell_ifc_schema_t* self, size_t name, ifcopenshell_ifc_declaration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_schema_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_schema_entities(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_entity_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_schema_enumeration_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_enumeration_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_schema_select_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_select_type_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_schema_type_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_type_declaration_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_instance_declaration(ifcopenshell_ifc_instance_t* self, ifcopenshell_ifc_declaration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_as_type_declaration(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_as_select_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_select_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_as_enumeration_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_enumeration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_as_entity(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_entity_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_name(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_name_uc(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_schema(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_schema_t** out_result);" in header
    assert "bool ifcopenshell_ifc_declaration_is_a(ifcopenshell_ifc_declaration_t* self, const char* name, bool* out_result);" in header
    assert "bool ifcopenshell_ifc_type_declaration_declared_type(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_parameter_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_parameter_type_as_aggregation_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_parameter_type_as_named_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_named_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_parameter_type_as_simple_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_simple_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_parameter_type_kind(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_named_type_declared_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_declaration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_simple_type_kind(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_bound1(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_bound2(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_type_of_aggregation(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_type_of_aggregation_string(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_type_of_element(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_ifc_parameter_type_t** out_result);" in header
    assert "bool ifcopenshell_ifc_aggregation_type_kind(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_entity_attribute_count(ifcopenshell_ifc_entity_t* self, size_t* out_result);" in header
    assert "bool ifcopenshell_ifc_attribute_name(ifcopenshell_ifc_attribute_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_select_type_select_list_names(ifcopenshell_ifc_select_type_t* self, ifcopenshell_string_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_enumeration_enumeration_items(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_enumeration_argument_types(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result);" in header
    assert "bool ifcopenshell_ifc_enumeration_lookup_enum_value(ifcopenshell_ifc_enumeration_t* self, size_t i, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifc_enumeration_lookup_enum_offset(ifcopenshell_ifc_enumeration_t* self, const char* string, size_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_is_null(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_size(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32(ifcopenshell_ifcparse_attribute_value_t* self, int32_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_bool(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double(ifcopenshell_ifcparse_attribute_value_t* self, double* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_string_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_list_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_string(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_instance(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_instance_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_value(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_index(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_enumeration_t** out_result);" in header
    assert "bool ifcopenshell_ifc_instance_identity(ifcopenshell_ifc_instance_t* self, uint32_t* out_result);" in header
    assert "bool ifcopenshell_ifc_instance_get_argument(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_attribute_value_t** out_result);" in header
    assert "bool ifcopenshell_ifc_instance_unset_argument(ifcopenshell_ifc_instance_t* self, size_t index);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_bool(ifcopenshell_ifc_instance_t* self, size_t index, bool value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_int32(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_double(ifcopenshell_ifc_instance_t* self, size_t index, double value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_string(ifcopenshell_ifc_instance_t* self, size_t index, const char* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_instance(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_instance_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_instance_list(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifcparse_instance_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_int32_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_double_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_string_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_string_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_int32_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_double_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_list_t* value);" in header
    assert "bool ifcopenshell_ifc_instance_set_argument_enumeration(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_enumeration_t* enumeration, size_t enumeration_index);" in header
    assert "bool ifcopenshell_ifcparse_instance_list_size(ifcopenshell_ifcparse_instance_list_t* self, size_t* out_result);" in header
    assert "bool ifcopenshell_ifcparse_instance_list_get(ifcopenshell_ifcparse_instance_list_t* self, size_t index, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "const char* ifcopenshell_ifcparse_last_error_message(void);" in header
    assert "void ifcopenshell_ifc_file_destroy(ifcopenshell_ifc_file_t* handle);" in header
    assert "void ifcopenshell_ifc_declaration_list_destroy(ifcopenshell_ifc_declaration_list_t* value);" in header
    assert "void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value);" in header
    assert "void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value);" in header
    assert "void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value);" in header
    assert "void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value);" in header
    assert "void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value);" in header

    assert 'thread_local std::string g_last_error;' in cpp
    assert "bool ifcopenshell_ifcparse_open(const char* path, bool readonly, ifcopenshell_ifc_file_t** out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_new_file(const char* schema_identifier, int32_t file_type, const char* path, ifcopenshell_ifc_file_t** out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_schema_by_name(const char* arg_0, ifcopenshell_ifc_schema_t** out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_schema_names(ifcopenshell_string_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifc_instance_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_by_type(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_by_type_excl_subtypes(ifcopenshell_ifc_file_t* self, const char* type, ifcopenshell_ifcparse_instance_list_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_instances_by_reference(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifcparse_instance_list_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_add(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_instance_t* entity, int32_t id, ifcopenshell_ifc_instance_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_create(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_declaration_t* decl, ifcopenshell_ifc_instance_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_get_total_inverses_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, size_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_file_header(ifcopenshell_ifc_file_t* self, ifcopenshell_ifc_header_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_name(ifcopenshell_ifc_schema_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_declaration_by_name(ifcopenshell_ifc_schema_t* self, const char* name, ifcopenshell_ifc_declaration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_declaration_by_index(ifcopenshell_ifc_schema_t* self, size_t name, ifcopenshell_ifc_declaration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_declaration_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_entities(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_entity_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_enumeration_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_enumeration_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_select_types(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_select_type_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_schema_type_declarations(ifcopenshell_ifc_schema_t* self, ifcopenshell_ifc_type_declaration_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_instance_declaration(ifcopenshell_ifc_instance_t* self, ifcopenshell_ifc_declaration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_as_type_declaration(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_type_declaration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_as_select_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_select_type_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_as_enumeration_type(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_enumeration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_as_entity(ifcopenshell_ifc_declaration_t* self, ifcopenshell_ifc_entity_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_name(ifcopenshell_ifc_declaration_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_declaration_is_a(ifcopenshell_ifc_declaration_t* self, const char* name, bool* out_result)" in cpp
    assert "bool ifcopenshell_ifc_type_declaration_declared_type(ifcopenshell_ifc_type_declaration_t* self, ifcopenshell_ifc_parameter_type_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_parameter_type_as_aggregation_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_aggregation_type_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_parameter_type_as_named_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_named_type_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_parameter_type_as_simple_type(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_ifc_simple_type_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_parameter_type_kind(ifcopenshell_ifc_parameter_type_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_named_type_declared_type(ifcopenshell_ifc_named_type_t* self, ifcopenshell_ifc_declaration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_simple_type_kind(ifcopenshell_ifc_simple_type_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_aggregation_type_bound1(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_aggregation_type_type_of_aggregation(ifcopenshell_ifc_aggregation_type_t* self, int32_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_aggregation_type_type_of_aggregation_string(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_aggregation_type_kind(ifcopenshell_ifc_aggregation_type_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_entity_attribute_count(ifcopenshell_ifc_entity_t* self, size_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_attribute_name(ifcopenshell_ifc_attribute_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_select_type_select_list_names(ifcopenshell_ifc_select_type_t* self, ifcopenshell_string_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_enumeration_enumeration_items(ifcopenshell_ifc_enumeration_t* self, ifcopenshell_string_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifc_enumeration_lookup_enum_value(ifcopenshell_ifc_enumeration_t* self, size_t i, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_is_null(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_size(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32(ifcopenshell_ifcparse_attribute_value_t* self, int32_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_bool(ifcopenshell_ifcparse_attribute_value_t* self, bool* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double(ifcopenshell_ifcparse_attribute_value_t* self, double* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_int32_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_double_list_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_string_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_list_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_string(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_instance(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_instance_t** out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_instance_list(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifcparse_instance_list_t** out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_value(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_string_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_index(ifcopenshell_ifcparse_attribute_value_t* self, size_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_attribute_value_as_enumeration_type(ifcopenshell_ifcparse_attribute_value_t* self, ifcopenshell_ifc_enumeration_t** out_result)" in cpp
    assert "bool ifcopenshell_ifc_instance_get_argument(" in cpp
    assert "bool ifcopenshell_ifc_instance_unset_argument(ifcopenshell_ifc_instance_t* self, size_t index)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_bool(ifcopenshell_ifc_instance_t* self, size_t index, bool value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_int32(ifcopenshell_ifc_instance_t* self, size_t index, int32_t value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_double(ifcopenshell_ifc_instance_t* self, size_t index, double value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_string(ifcopenshell_ifc_instance_t* self, size_t index, const char* value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_string_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_string_list_t* value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_int32_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_double_list_list(ifcopenshell_ifc_instance_t* self, size_t index, const ifcopenshell_double_list_list_t* value)" in cpp
    assert "bool ifcopenshell_ifc_instance_set_argument_enumeration(ifcopenshell_ifc_instance_t* self, size_t index, ifcopenshell_ifc_enumeration_t* enumeration, size_t enumeration_index)" in cpp
    assert "bool ifcopenshell_ifcparse_instance_list_size(ifcopenshell_ifcparse_instance_list_t* self, size_t* out_result)" in cpp
    assert "bool ifcopenshell_ifcparse_instance_list_get(ifcopenshell_ifcparse_instance_list_t* self, size_t index, ifcopenshell_ifc_instance_t** out_result)" in cpp
    assert "*out_result = new ifcopenshell_ifc_file_t{generated_result, true};" in cpp
    assert "*out_result = make_string_list(IfcParse::schema_names());" in cpp
    assert "*out_result = make_int32_list(generated_result);" in cpp
    assert "*out_result = make_uint32_list(generated_result);" in cpp
    assert "*out_result = make_int32_list_list(generated_result);" in cpp
    assert "*out_result = make_double_list(generated_result);" in cpp
    assert "*out_result = make_double_list_list(generated_result);" in cpp
    assert "make_ifc_declaration_list(" in cpp
    assert "make_ifc_enumeration_list(" in cpp
    assert "make_ifc_select_type_list(" in cpp
    assert "make_ifc_type_declaration_list(" in cpp
    assert "*out_result = new ifcopenshell_ifcparse_attribute_value_t{self_cpp->get_attribute_value(index)};" in cpp
    assert "*out_result = new ifcopenshell_ifcparse_instance_list_t{IfcParse::traverse(instance_cpp, max_level)};" in cpp
    assert "*out_result = new ifcopenshell_ifc_header_t{generated_result, false};" in cpp
    assert "new ifcopenshell_ifc_declaration_t{" in cpp
    assert "auto* self_cpp = self->ptr;" in cpp
    assert "auto self_cpp = self->value;" in cpp
    assert "data_cpp.size()" in cpp
    assert "set_instance_argument(self_cpp, index, value_cpp);" in cpp
    assert "unset_instance_argument(self_cpp, index);" in cpp


def test_generate_ifcparse_c_backend_with_compile_commands_when_available(tmp_path: Path) -> None:
    spec = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    compile_commands = Path("/tmp/ifcopenshell-capi-build/compile_commands.json")
    if not compile_commands.exists():
        return

    header_out = tmp_path / "ifcparse_api.h"
    cpp_out = tmp_path / "ifcparse_api.cpp"

    generate(spec, header_out, cpp_out, compile_commands_path=compile_commands)

    header = header_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t id, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_level, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
