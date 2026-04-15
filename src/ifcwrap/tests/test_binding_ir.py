# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

from src.ifcwrap.binding_generator.authored_spec import load_merged_specs
from src.ifcwrap.binding_generator.binding_ir import (
    BindingIR,
    ChildrenAddOp,
    ConstructorOp,
    DirectCallOp,
    FieldGetOp,
    InlineImplementationOp,
    OptionalGetOp,
    VariantGetOp,
    VariantSetOp,
    lower_binding_spec,
)
from src.ifcwrap.binding_generator.policy_ir import InlineAdapterPolicyOp
from src.ifcwrap.tests._binding_generator_test_utils import require_repo_compile_commands


def test_lower_binding_spec_produces_typed_operations() -> None:
    compile_commands = require_repo_compile_commands(
        "compile_commands.json is required for binding IR tests"
    )
    spec_dir = Path(__file__).resolve().parents[1] / "binding_generator" / "specs"
    source_spec = load_merged_specs(
        [spec_dir / "ifcparse.yml", spec_dir / "ifcgeom.yml"],
        module="ifcopenshell",
        c_prefix="ifcopenshell",
        compile_commands_path=compile_commands,
    )

    ir = lower_binding_spec(source_spec)
    assert isinstance(ir, BindingIR)

    assert all(call.policy_operation is not None for call in (*source_spec.functions, *source_spec.methods))

    calls = {call.c_name: call for call in (*ir.functions, *ir.methods)}
    source_methods = {(call.receiver, call.expose_as): call for call in source_spec.methods}

    assert isinstance(calls["ifcopenshell_ifcgeom_create_settings"].operation, ConstructorOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_settings_get_bool"].operation, VariantGetOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_settings_set_bool"].operation, VariantSetOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_settings_set_int"].operation, VariantSetOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_tree_enable_face_styles"].operation, DirectCallOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_taxonomy_revolve_angle"].operation, OptionalGetOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_taxonomy_loft_axis"].operation, FieldGetOp)
    assert isinstance(source_methods[("tree", "select_box_element")].policy_operation, InlineAdapterPolicyOp)
    assert isinstance(source_methods[("tree", "select_box_bounds")].policy_operation, InlineAdapterPolicyOp)
    assert isinstance(source_methods[("tree", "select_brep_element")].policy_operation, InlineAdapterPolicyOp)
    assert isinstance(source_methods[("tree", "select_shape_serialization")].policy_operation, InlineAdapterPolicyOp)
    assert isinstance(source_methods[("taxonomy_style", "instance_id")].policy_operation, InlineAdapterPolicyOp)

    add_item_op = calls["ifcopenshell_ifcgeom_taxonomy_loft_add_item"].operation
    assert isinstance(add_item_op, ChildrenAddOp)
    assert add_item_op.field_name == "children"
    assert add_item_op.cast_cpp_type == "ifcopenshell::geometry::taxonomy::geom_item"

    assert isinstance(calls["ifcopenshell_ifcgeom_tree_select_box_element"].operation, InlineImplementationOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_tree_select_box_bounds"].operation, InlineImplementationOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_tree_select_brep_element"].operation, InlineImplementationOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_tree_select_shape_serialization"].operation, InlineImplementationOp)
    assert isinstance(calls["ifcopenshell_ifcgeom_taxonomy_style_instance_id"].operation, InlineImplementationOp)
