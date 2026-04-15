from __future__ import annotations

from pathlib import Path

from src.ifcwrap.binding_generator import load_authored_spec
from src.ifcwrap.binding_generator.policy_ir import DirectMethodPolicyOp, InlineAdapterPolicyOp
from src.ifcwrap.tests._binding_generator_test_utils import require_repo_compile_commands


def test_load_ifcparse_authored_spec() -> None:
    path = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    spec = load_authored_spec(
        path,
        compile_commands_path=require_repo_compile_commands(
            "compile_commands.json is required for AST-backed spec tests"
        ),
    )

    assert spec.slice == "ifcparse"
    assert spec.c_prefix == "ifcopenshell_ifcparse"
    assert spec.discovery is not None
    assert "file" in spec.handles
    assert spec.handles["file"].cpp_type == "IfcParse::IfcFile"
    assert "entity" in spec.handles
    assert "attribute" in spec.handles
    assert "inverse_attribute" in spec.handles
    assert any(call.expose_as == "open" for call in spec.functions)
    assert any(call.expose_as == "new_file" for call in spec.functions)
    assert any(call.expose_as == "traverse" for call in spec.functions)
    assert any(call.expose_as == "schema_by_name" for call in spec.functions)
    assert any(call.expose_as == "schema_names" for call in spec.functions)
    assert any(call.expose_as == "by_id" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "by_type" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "by_type_excl_subtypes" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "instances_by_reference" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "create" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "add" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "entity_names" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "to_string" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "get_argument" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "get_argument_index" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "get_argument_name" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "get_attribute_category" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "unset_argument" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_bool" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_int32" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_double" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_string" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_instance" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_instance_list" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "set_argument_enumeration" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "size" and call.receiver == "instance_list" for call in spec.methods)
    assert any(call.expose_as == "schema" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "declaration" and call.receiver == "instance" for call in spec.methods)
    assert any(call.expose_as == "declaration_by_name" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "declaration_by_index" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "name" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "declarations" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "entities" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "enumeration_types" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "select_types" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "type_declarations" and call.receiver == "schema" for call in spec.methods)
    assert any(call.expose_as == "as_type_declaration" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "as_select_type" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "as_enumeration_type" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "as_entity" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "name_uc" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "type" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "schema" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "is_a" and call.receiver == "declaration" for call in spec.methods)
    assert any(call.expose_as == "declared_type" and call.receiver == "type_declaration" for call in spec.methods)
    assert any(call.expose_as == "argument_types" and call.receiver == "type_declaration" for call in spec.methods)
    assert any(call.expose_as == "as_aggregation_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_named_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_simple_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_named_type" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "is_a" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "declared_type" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "bound1" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "type_of_aggregation" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "type_of_aggregation_string" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "type_of_element" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "kind" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "kind" and call.receiver == "simple_type" for call in spec.methods)
    assert any(call.expose_as == "select_list_names" and call.receiver == "select_type" for call in spec.methods)
    assert any(call.expose_as == "as_select_type" and call.receiver == "select_type" for call in spec.methods)
    assert any(call.expose_as == "enumeration_items" and call.receiver == "enumeration" for call in spec.methods)
    assert any(call.expose_as == "argument_types" and call.receiver == "enumeration" for call in spec.methods)
    assert any(call.expose_as == "as_enumeration_type" and call.receiver == "enumeration" for call in spec.methods)
    assert any(call.expose_as == "argument_types" and call.receiver == "entity" for call in spec.methods)
    assert any(call.expose_as == "attribute_count" and call.receiver == "entity" for call in spec.methods)
    assert any(call.expose_as == "name" and call.receiver == "attribute" for call in spec.methods)
    assert any(call.expose_as == "type_of_aggregation_string" and call.receiver == "inverse_attribute" for call in spec.methods)
    assert any(call.expose_as == "type" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_instance_list" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_int32_list" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_double_list" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_int32_list_list" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_double_list_list" and call.receiver == "attribute_value" for call in spec.methods)
    assert any(call.expose_as == "as_string_list" and call.receiver == "attribute_value" for call in spec.methods)


def test_ifcparse_authored_spec_captures_ownership_and_adapter_rules() -> None:
    path = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    spec = load_authored_spec(
        path,
        compile_commands_path=require_repo_compile_commands(
            "compile_commands.json is required for AST-backed spec tests"
        ),
    )

    calls = {call.c_name: call for call in (*spec.functions, *spec.methods)}
    assert all(call.policy_operation is not None for call in (*spec.functions, *spec.methods))

    assert calls["ifcopenshell_ifcparse_open"].returns.ownership == "owned"
    assert calls["ifcopenshell_ifcparse_new_file"].returns.ownership == "owned"
    assert calls["ifcopenshell_ifcparse_new_file"].params[0].type.kind == "string"
    assert calls["ifcopenshell_ifcparse_new_file"].params[1].type.kind == "int32"
    assert calls["ifcopenshell_ifcparse_new_file"].params[2].type.kind == "string"
    assert calls["ifcopenshell_ifcparse_version"].returns.ownership == "static"
    assert calls["ifcopenshell_ifcparse_schema_names"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_schema_names"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifcparse_traverse"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifcparse_schema_by_name"].returns.handle == "schema"
    assert calls["ifcopenshell_ifc_file_by_id"].returns.ownership == "borrowed"
    assert calls["ifcopenshell_ifc_file_by_id"].params[0].type.kind == "int32"
    assert isinstance(calls["ifcopenshell_ifc_file_get_total_inverses_by_id"].policy_operation, DirectMethodPolicyOp)
    assert calls["ifcopenshell_ifc_file_instances_by_reference"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_by_type"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_by_type_excl_subtypes"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_create"].returns.handle == "instance"
    assert calls["ifcopenshell_ifc_file_add"].returns.handle == "instance"
    assert calls["ifcopenshell_ifc_file_entity_names"].returns.kind == "uint32"
    assert calls["ifcopenshell_ifc_file_entity_names"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_file_file_pointer"].returns.kind == "size"
    assert calls["ifcopenshell_ifc_file_get_inverse_indices"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_file_get_inverse_indices"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_file_good"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_file_header"].returns.handle == "header"
    assert calls["ifcopenshell_ifc_file_schema"].returns.handle == "schema"
    assert calls["ifcopenshell_ifc_instance_declaration"].returns.handle == "declaration"
    assert calls["ifcopenshell_ifc_instance_identity"].returns.kind == "uint32"
    assert calls["ifcopenshell_ifc_instance_file_pointer"].returns.kind == "size"
    assert calls["ifcopenshell_ifc_instance_get_argument_index"].returns.kind == "uint32"
    assert calls["ifcopenshell_ifc_instance_get_argument_name"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_instance_get_attribute_category"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_instance_get_inverse"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_schema_name"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_schema_declaration_by_name"].returns.handle == "declaration"
    assert calls["ifcopenshell_ifc_schema_declaration_by_name"].params[0].type.kind == "string"
    assert calls["ifcopenshell_ifc_schema_declaration_by_index"].params[0].type.kind == "size"
    assert calls["ifcopenshell_ifc_schema_declarations"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_schema_declarations"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_schema_declarations"].returns.handle == "declaration"
    assert calls["ifcopenshell_ifc_schema_entities"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_schema_entities"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_schema_entities"].returns.handle == "entity"
    assert calls["ifcopenshell_ifc_schema_enumeration_types"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_schema_enumeration_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_schema_enumeration_types"].returns.handle == "enumeration"
    assert calls["ifcopenshell_ifc_schema_select_types"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_schema_select_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_schema_select_types"].returns.handle == "select_type"
    assert calls["ifcopenshell_ifc_schema_type_declarations"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_schema_type_declarations"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_schema_type_declarations"].returns.handle == "type_declaration"
    assert calls["ifcopenshell_ifc_declaration_as_type_declaration"].returns.handle == "type_declaration"
    assert calls["ifcopenshell_ifc_declaration_as_select_type"].returns.handle == "select_type"
    assert calls["ifcopenshell_ifc_declaration_as_enumeration_type"].returns.handle == "enumeration"
    assert calls["ifcopenshell_ifc_declaration_as_entity"].returns.handle == "entity"
    assert calls["ifcopenshell_ifc_declaration_name"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_declaration_name_uc"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_declaration_type"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_declaration_schema"].returns.handle == "schema"
    assert calls["ifcopenshell_ifc_declaration_is_a"].returns.kind == "bool"
    assert calls["ifcopenshell_ifc_type_declaration_declared_type"].returns.handle == "parameter_type"
    assert calls["ifcopenshell_ifc_type_declaration_argument_types"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_type_declaration_argument_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_parameter_type_as_aggregation_type"].returns.handle == "aggregation_type"
    assert calls["ifcopenshell_ifc_parameter_type_as_named_type"].returns.handle == "named_type"
    assert calls["ifcopenshell_ifc_parameter_type_as_simple_type"].returns.handle == "simple_type"
    assert calls["ifcopenshell_ifc_parameter_type_kind"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_named_type_as_named_type"].returns.handle == "named_type"
    assert calls["ifcopenshell_ifc_named_type_is_a"].returns.kind == "bool"
    assert calls["ifcopenshell_ifc_named_type_declared_type"].returns.handle == "declaration"
    assert calls["ifcopenshell_ifc_simple_type_kind"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_aggregation_type_bound1"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_aggregation_type_bound2"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_aggregation_type_type_of_aggregation"].returns.kind == "int32"
    assert calls["ifcopenshell_ifc_aggregation_type_type_of_aggregation_string"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_aggregation_type_type_of_element"].returns.handle == "parameter_type"
    assert calls["ifcopenshell_ifc_aggregation_type_kind"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_select_type_select_list_names"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_select_type_select_list_names"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_select_type_as_select_type"].returns.handle == "select_type"
    assert calls["ifcopenshell_ifc_enumeration_enumeration_items"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_enumeration_enumeration_items"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_enumeration_argument_types"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_enumeration_argument_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_enumeration_as_enumeration_type"].returns.handle == "enumeration"
    assert calls["ifcopenshell_ifc_enumeration_lookup_enum_value"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_enumeration_lookup_enum_offset"].returns.kind == "size"
    assert calls["ifcopenshell_ifc_entity_argument_types"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_entity_argument_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_entity_all_attributes"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_entity_all_attributes"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_entity_all_attributes"].returns.handle == "attribute"
    assert calls["ifcopenshell_ifc_entity_all_inverse_attributes"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_entity_all_inverse_attributes"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifc_entity_all_inverse_attributes"].returns.handle == "inverse_attribute"
    assert calls["ifcopenshell_ifc_attribute_name"].returns.kind == "string"
    assert calls["ifcopenshell_ifc_attribute_type_of_attribute"].returns.handle == "parameter_type"
    assert calls["ifcopenshell_ifc_inverse_attribute_type_of_aggregation_string"].returns.kind == "string"
    assert isinstance(calls["ifcopenshell_ifcparse_instance_list_size"].policy_operation, DirectMethodPolicyOp)
    assert isinstance(calls["ifcopenshell_ifc_instance_id"].policy_operation, DirectMethodPolicyOp)
    assert isinstance(calls["ifcopenshell_ifc_file_schema_name"].policy_operation, InlineAdapterPolicyOp)
    assert calls["ifcopenshell_ifc_instance_get_argument"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_instance_get_argument"].returns.handle == "attribute_value"
    assert calls["ifcopenshell_ifc_instance_unset_argument"].returns.kind == "void"
    assert calls["ifcopenshell_ifc_instance_set_argument_bool"].params[1].type.kind == "bool"
    assert calls["ifcopenshell_ifc_instance_set_argument_int32"].params[1].type.kind == "int32"
    assert calls["ifcopenshell_ifc_instance_set_argument_double"].params[1].type.kind == "double"
    assert calls["ifcopenshell_ifc_instance_set_argument_string"].params[1].type.kind == "string"
    assert calls["ifcopenshell_ifc_instance_set_argument_instance"].params[1].type.handle == "instance"
    assert calls["ifcopenshell_ifc_instance_set_argument_instance_list"].params[1].type.handle == "instance_list"
    assert calls["ifcopenshell_ifc_instance_set_argument_int32_list"].params[1].type.kind == "int32"
    assert calls["ifcopenshell_ifc_instance_set_argument_int32_list"].params[1].type.sequence_depth == 1
    assert calls["ifcopenshell_ifc_instance_set_argument_double_list"].params[1].type.kind == "double"
    assert calls["ifcopenshell_ifc_instance_set_argument_double_list"].params[1].type.sequence_depth == 1
    assert calls["ifcopenshell_ifc_instance_set_argument_string_list"].params[1].type.kind == "string"
    assert calls["ifcopenshell_ifc_instance_set_argument_string_list"].params[1].type.sequence_depth == 1
    assert calls["ifcopenshell_ifc_instance_set_argument_int32_list_list"].params[1].type.kind == "int32"
    assert calls["ifcopenshell_ifc_instance_set_argument_int32_list_list"].params[1].type.sequence_depth == 2
    assert calls["ifcopenshell_ifc_instance_set_argument_double_list_list"].params[1].type.kind == "double"
    assert calls["ifcopenshell_ifc_instance_set_argument_double_list_list"].params[1].type.sequence_depth == 2
    assert calls["ifcopenshell_ifc_instance_set_argument_enumeration"].params[1].type.handle == "enumeration"
    assert calls["ifcopenshell_ifcparse_attribute_value_is_null"].returns.kind == "bool"
    assert calls["ifcopenshell_ifcparse_attribute_value_type"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_attribute_value_size"].returns.kind == "size"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32"].returns.kind == "int32"
    assert calls["ifcopenshell_ifcparse_open"].policy_operation is not None
    assert calls["ifcopenshell_ifc_file_by_id"].policy_operation is not None
    assert calls["ifcopenshell_ifc_file_schema_name"].policy_operation is not None
    assert calls["ifcopenshell_ifcparse_attribute_value_as_bool"].returns.kind == "bool"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_double"].returns.kind == "double"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32_list"].returns.kind == "int32"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32_list"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifcparse_attribute_value_as_double_list"].returns.kind == "double"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_double_list"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32_list_list"].returns.kind == "int32"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32_list_list"].returns.sequence_depth == 2
    assert calls["ifcopenshell_ifcparse_attribute_value_as_double_list_list"].returns.kind == "double"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_double_list_list"].returns.sequence_depth == 2
    assert calls["ifcopenshell_ifcparse_attribute_value_as_string_list"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_string_list"].returns.sequence_depth == 1
    assert calls["ifcopenshell_ifcparse_attribute_value_as_string"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_instance"].returns.handle == "instance"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_instance_list"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_enumeration_value"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_enumeration_index"].returns.kind == "size"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_enumeration_type"].returns.handle == "enumeration"
