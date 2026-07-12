# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
from textwrap import dedent

from src.ifcwrap.binding_generator.authored_spec import load_authored_spec
from src.ifcwrap.binding_generator.binding_ir import (
    BindingIR,
    CallIR,
    DirectCallOp,
    lower_binding_spec,
)
from src.ifcwrap.binding_generator.binding_model import (
    HandleSpec,
    OptionStructFieldSpec,
    OptionStructSpec,
    ParamSpec,
    ResultStructFieldSpec,
    ResultStructSpec,
    TypeSpec,
)
from src.ifcwrap.binding_generator.host_metadata import build_host_metadata


def test_host_metadata_derives_layouts_and_signatures(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handles:
              - name: file
                cpp_type: Demo::File
                c_type: ifcopenshell_demo_file_t
                destructor: delete
              - name: item
                cpp_type: Demo::Item
                c_type: ifcopenshell_demo_item_t
                destructor: delete
            functions:
              - expose_as: create_file
                handle: file
                params:
                  - name: schema
                    type:
                      kind: string
              - expose_as: schema_names
                cpp_name: schema_names
                returns:
                  kind: string_list
                params: []
            methods:
              - receiver: file
                expose_as: by_type
                cpp_name: by_type
                returns:
                  kind: handle_list
                  handle: item
                  ownership: owned
                params:
                  - name: type_name
                    type:
                      kind: string
              - receiver: item
                expose_as: set_values
                cpp_name: set_values
                returns:
                  kind: void
                params:
                  - name: values
                    type:
                      kind: double_list
              - receiver: item
                expose_as: set_counts
                cpp_name: set_counts
                returns:
                  kind: void
                params:
                  - name: values
                    type:
                      kind: int64_list
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    metadata = build_host_metadata(lower_binding_spec(load_authored_spec(spec_path)))

    assert metadata.error_functions == {
        "clear_error": "ifcopenshell_demo_clear_error",
        "last_error_message": "ifcopenshell_demo_last_error_message",
        "last_error_kind": "ifcopenshell_demo_last_error_kind",
    }
    assert metadata.handles["file"].layout == "ptr_owned"
    assert metadata.handles["file"].fields[0].c_type == "void*"
    assert metadata.handles["file"].destroy_function == "ifcopenshell_demo_file_destroy"

    assert metadata.value_types["string"].fields[0].name == "data"
    assert metadata.value_types["string_list"].destroy_function == "ifcopenshell_string_list_destroy"
    assert metadata.value_types["double_list"].fields[0].c_type == "double*"
    assert metadata.value_types["int64_list"].fields[0].c_type == "int64_t*"
    assert metadata.value_types["demo_item_list"].fields[0].c_type == "ifcopenshell_demo_item_t**"
    assert metadata.value_types["demo_item_list_list"].fields[0].c_type == "ifcopenshell_demo_item_list_t*"

    create = metadata.functions["ifcopenshell_demo_create_file"]
    assert create.restype == "bool"
    assert [param.c_type for param in create.params] == [
        "const char*",
        "ifcopenshell_demo_file_t**",
    ]
    assert create.params[-1].role == "out_result"
    assert create.error_policy == "bool_return_last_error"

    by_type = metadata.functions["ifcopenshell_demo_file_by_type"]
    assert [param.role for param in by_type.params] == [
        "receiver",
        "param",
        "out_result",
    ]
    assert [param.c_type for param in by_type.params] == [
        "ifcopenshell_demo_file_t*",
        "const char*",
        "ifcopenshell_demo_item_list_t*",
    ]

    set_values = metadata.functions["ifcopenshell_demo_item_set_values"]
    assert [param.c_type for param in set_values.params] == [
        "ifcopenshell_demo_item_t*",
        "const ifcopenshell_double_list_t*",
    ]

    set_counts = metadata.functions["ifcopenshell_demo_item_set_counts"]
    assert [param.c_type for param in set_counts.params] == [
        "ifcopenshell_demo_item_t*",
        "const ifcopenshell_int64_list_t*",
    ]


def test_host_metadata_includes_sequences_used_only_by_option_structs() -> None:
    metadata = build_host_metadata(
        BindingIR(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(),
            handles={
                "item": HandleSpec(
                    name="item",
                    cpp_type="Demo::Item",
                    c_type="ifcopenshell_demo_item_t",
                    destructor="delete",
                ),
            },
            result_structs={},
            functions=(
                CallIR(
                    expose_as="add_mesh",
                    c_name="ifcopenshell_demo_add_mesh",
                    receiver=None,
                    returns=TypeSpec(kind="void"),
                    params=(ParamSpec("options", TypeSpec(kind="option", struct="AddMeshOptions")),),
                    operation=DirectCallOp(cpp_name="Demo::add_mesh"),
                ),
            ),
            methods=(),
            option_structs={
                "AddMeshOptions": OptionStructSpec(
                    name="AddMeshOptions",
                    cpp_type="Demo::AddMeshOptions",
                    c_type="ifcopenshell_demo_add_mesh_options_t",
                    fields=(
                        OptionStructFieldSpec("faces", TypeSpec(kind="int32", sequence_depth=4)),
                        OptionStructFieldSpec(
                            "grouped_items",
                            TypeSpec(kind="handle", handle="item", sequence_depth=2),
                        ),
                    ),
                ),
            },
        )
    )

    assert "int32_list" in metadata.value_types
    assert "int32_list_list_list_list" in metadata.value_types
    assert metadata.value_types["int32_list_list_list_list"].sequence_depth == 4
    assert "demo_item_list" in metadata.value_types
    assert "demo_item_list_list" in metadata.value_types


def test_host_metadata_preserves_option_and_result_field_docs() -> None:
    metadata = build_host_metadata(
        BindingIR(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(),
            handles={},
            result_structs={
                "DemoResult": ResultStructSpec(
                    name="DemoResult",
                    cpp_type="Demo::Result",
                    c_type="ifcopenshell_demo_result_t",
                    fields=(
                        ResultStructFieldSpec(
                            "value",
                            TypeSpec(kind="double"),
                            doc="Result value in model units.",
                        ),
                        ResultStructFieldSpec("undocumented", TypeSpec(kind="bool")),
                    ),
                )
            },
            functions=(),
            methods=(),
            option_structs={
                "DemoOptions": OptionStructSpec(
                    name="DemoOptions",
                    cpp_type="Demo::Options",
                    c_type="ifcopenshell_demo_options_t",
                    fields=(
                        OptionStructFieldSpec(
                            "enabled",
                            TypeSpec(kind="bool", nullable=True),
                            doc="Whether the feature is enabled.\n\nOptional in the input.",
                        ),
                        OptionStructFieldSpec("undocumented", TypeSpec(kind="string")),
                    ),
                )
            },
        )
    )

    option_fields = {field.name: field for field in metadata.option_structs["DemoOptions"].fields}
    assert option_fields["enabled"].doc == "Whether the feature is enabled.\n\nOptional in the input."
    assert option_fields["undocumented"].doc is None

    result_fields = {field.name: field for field in metadata.value_types["DemoResult"].fields}
    assert result_fields["value"].doc == "Result value in model units."
    assert result_fields["undocumented"].doc is None
