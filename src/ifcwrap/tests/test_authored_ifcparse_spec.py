from __future__ import annotations

import json
from pathlib import Path
import shlex

from src.ifcwrap.binding_generator import load_authored_spec, load_merged_specs
from src.ifcwrap.binding_generator.c_backend import CppSpecConfig, _merge_cpp_specs
from src.ifcwrap.binding_generator.cpp_spec_frontend import discover_cpp_spec_handles, lower_cpp_spec_handles_to_specs
from src.ifcwrap.binding_generator.policy_ir import DirectMethodPolicyOp
from src.ifcwrap.tests._binding_generator_test_utils import require_repo_compile_commands


def _ifcparse_paths() -> tuple[Path, Path]:
    path = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    return path, path.with_name("cpp") / "ifcparse.hpp"


def _ifcparse_include_dirs() -> tuple[Path, ...]:
    repo_src = Path(__file__).resolve().parents[2]
    include_dirs = [repo_src, repo_src / "ifcparse", repo_src / "ifcparse" / "schemas"]
    compile_commands = require_repo_compile_commands("compile_commands.json is required for AST-backed spec tests")
    entries = json.loads(compile_commands.read_text(encoding="utf-8"))
    entry = next((item for item in entries if item["file"].endswith("/src/ifcparse/parse.cpp")), None)
    if entry is not None:
        args = entry.get("arguments") or shlex.split(entry["command"])
        index = 0
        while index < len(args):
            arg = args[index]
            include_dir = None
            if arg in {"-I", "-isystem"} and index + 1 < len(args):
                include_dir = args[index + 1]
                index += 2
            else:
                if arg.startswith("-I") and len(arg) > 2:
                    include_dir = arg[2:]
                elif arg.startswith("-isystem") and len(arg) > len("-isystem"):
                    include_dir = arg[len("-isystem") :]
                index += 1
            if include_dir:
                path = Path(include_dir)
                if path not in include_dirs:
                    include_dirs.append(path)
    return tuple(include_dirs)


def _load_ifcparse_authored():
    path, cpp_path = _ifcparse_paths()
    return load_authored_spec(
        path,
        compile_commands_path=require_repo_compile_commands(
            "compile_commands.json is required for AST-backed spec tests"
        ),
        existing_handles=lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(cpp_path, c_prefix="ifcopenshell_ifc")),
    )


def _load_ifcparse_merged():
    path, cpp_path = _ifcparse_paths()
    base = load_merged_specs(
        [path],
        module="ifcopenshell",
        c_prefix="ifcopenshell",
        compile_commands_path=require_repo_compile_commands(
            "compile_commands.json is required for AST-backed spec tests"
        ),
        existing_handles=lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(cpp_path, c_prefix="ifcopenshell_ifc")),
    )
    return _merge_cpp_specs(
        base,
        (CppSpecConfig(path=cpp_path, namespace="ifcparse::bindings", c_prefix="ifcopenshell_ifcparse", handle_c_prefix="ifcopenshell_ifc"),),
        None,
        discovery_include_dirs=_ifcparse_include_dirs(),
    )


def test_load_ifcparse_authored_spec() -> None:
    spec = _load_ifcparse_authored()

    assert spec.slice == "ifcparse"
    assert spec.c_prefix == "ifcopenshell_ifcparse"
    assert spec.discovery is not None
    assert "file" in spec.handles
    assert spec.handles["file"].cpp_type == "ifcopenshell::file"
    assert "entity" in spec.handles
    assert "attribute" in spec.handles
    assert "inverse_attribute" in spec.handles
    assert not spec.functions
    assert any(call.expose_as == "by_id" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "by_type" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "by_type_excl_subtypes" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "instances_by_reference" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "create" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "add" and call.receiver == "file" for call in spec.methods)
    assert any(call.expose_as == "get_argument" and call.receiver == "instance" for call in spec.methods)
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
    assert any(call.expose_as == "as_aggregation_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_named_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_simple_type" and call.receiver == "parameter_type" for call in spec.methods)
    assert any(call.expose_as == "as_named_type" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "is_a" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "declared_type" and call.receiver == "named_type" for call in spec.methods)
    assert any(call.expose_as == "bound1" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "type_of_element" and call.receiver == "aggregation_type" for call in spec.methods)
    assert any(call.expose_as == "as_select_type" and call.receiver == "select_type" for call in spec.methods)
    assert any(call.expose_as == "enumeration_items" and call.receiver == "enumeration" for call in spec.methods)
    assert any(call.expose_as == "as_enumeration_type" and call.receiver == "enumeration" for call in spec.methods)
    assert any(call.expose_as == "attribute_count" and call.receiver == "entity" for call in spec.methods)
    assert any(call.expose_as == "name" and call.receiver == "attribute" for call in spec.methods)


def test_ifcparse_authored_spec_captures_ownership_and_adapter_rules() -> None:
    spec = _load_ifcparse_merged()

    calls = {call.c_name: call for call in (*spec.functions, *spec.methods)}
    assert all(call.policy_operation is not None for call in (*spec.functions, *spec.methods))

    assert calls["ifcopenshell_ifcparse_instance_list_create_from_handles"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifcparse_instance_list_create_from_handles"].params[0].type.handle == "instance"
    assert calls["ifcopenshell_ifc_file_by_id"].returns.ownership == "borrowed"
    assert calls["ifcopenshell_ifc_file_by_id"].params[0].type.kind == "int32"
    assert isinstance(calls["ifcopenshell_ifc_file_get_total_inverses_by_id"].policy_operation, DirectMethodPolicyOp)
    assert calls["ifcopenshell_ifc_file_instances_by_reference"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_instances_by_reference"].returns.sequence_depth == 0
    assert calls["ifcopenshell_ifc_file_by_type"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_by_type"].returns.sequence_depth == 0
    assert calls["ifcopenshell_ifc_file_by_type_excl_subtypes"].returns.handle == "instance_list"
    assert calls["ifcopenshell_ifc_file_by_type_excl_subtypes"].returns.sequence_depth == 0
    assert calls["ifcopenshell_ifc_file_create"].returns.handle == "instance"
    assert calls["ifcopenshell_ifc_file_add"].returns.handle == "instance"
    assert calls["ifcopenshell_ifc_file_header"].returns.handle == "header"
    assert calls["ifcopenshell_ifc_file_schema"].returns.handle == "schema"
    assert calls["ifcopenshell_ifc_instance_declaration"].returns.handle == "declaration"
    assert calls["ifcopenshell_ifc_instance_identity"].returns.kind == "uint32"
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
    assert calls["ifcopenshell_ifcparse_instance_list_size"].policy_operation is not None
    assert isinstance(calls["ifcopenshell_ifc_instance_id"].policy_operation, DirectMethodPolicyOp)
    assert calls["ifcopenshell_ifc_instance_get_argument"].returns.kind == "handle"
    assert calls["ifcopenshell_ifc_instance_get_argument"].returns.handle == "attribute_value"
    assert calls["ifcopenshell_ifcparse_attribute_value_is_null"].returns.kind == "bool"
    assert calls["ifcopenshell_ifcparse_attribute_value_type"].returns.kind == "string"
    assert calls["ifcopenshell_ifcparse_attribute_value_size"].returns.kind == "size"
    assert calls["ifcopenshell_ifcparse_attribute_value_as_int32"].returns.kind == "int32"
    assert calls["ifcopenshell_ifcparse_instance_list_create_from_handles"].policy_operation is not None
    assert calls["ifcopenshell_ifc_file_by_id"].policy_operation is not None
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
