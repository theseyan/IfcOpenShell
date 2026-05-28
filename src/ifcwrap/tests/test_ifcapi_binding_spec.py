# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shlex

import pytest

from src.ifcwrap.binding_generator.authored_spec import (
    HandleSpec,
    MergedBindingSpec,
    load_merged_specs,
)
from src.ifcwrap.binding_generator.binding_ir import DirectCallOp, lower_binding_spec
from src.ifcwrap.binding_generator.c_backend import _merge_cpp_specs
from src.ifcwrap.binding_generator.host_metadata import build_host_metadata
from src.ifcwrap.binding_generator.python_ctypes_backend import render_python_ctypes


def _spec_dir() -> Path:
    return Path(__file__).resolve().parents[1] / "binding_generator" / "specs"


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def _cpp_specs() -> list[Path]:
    return [_spec_dir() / "cpp" / "ifcapi.hpp"]


def _cpp_discovery_include_dirs() -> tuple[Path, ...]:
    root = _repo_root()
    include_dirs = [
        root / "src",
        root / "src" / "ifcwrap",
        root / "src" / "ifcapi" / "include",
        root / "src" / "ifcapi" / "src",
    ]
    commands = json.loads(_compile_commands().read_text(encoding="utf-8"))
    for command in commands:
        if "ifcopenshell_api.cpp" not in str(command.get("file", "")):
            continue
        args = command.get("arguments") or shlex.split(command["command"])
        for index, arg in enumerate(args):
            include_dir = None
            if arg in {"-I", "-isystem"} and index + 1 < len(args):
                include_dir = args[index + 1]
            elif arg.startswith("-I"):
                include_dir = arg[2:]
            elif arg.startswith("-isystem") and len(arg) > len("-isystem"):
                include_dir = arg[len("-isystem") :]
            if include_dir:
                path = Path(include_dir)
                if path not in include_dirs:
                    include_dirs.append(path)
        break
    return tuple(include_dirs)


def _merge_ifcapi_guid_cpp_spec(merged):
    return _merge_cpp_specs(
        merged,
        _cpp_specs(),
        "ifcapi::bindings",
        "ifcopenshell_ifcapi",
        None,
        discovery_include_dirs=_cpp_discovery_include_dirs(),
    )


def _ifcapi_merged_with_core_handles() -> MergedBindingSpec:
    return MergedBindingSpec(
        module="ifcopenshell",
        c_prefix="ifcopenshell",
        public_headers=(),
        handles=_core_handles(),
        result_structs={},
        functions=(),
        methods=(),
    )


def _compile_commands() -> Path:
    path = Path("build-capi-stable/compile_commands.json")
    if not path.exists():
        pytest.skip("build-capi-stable/compile_commands.json is required for ifcapi discovery tests")
    return path


def _core_handles() -> dict[str, HandleSpec]:
    return {
        "file": HandleSpec(
            name="file",
            cpp_type="ifcopenshell::file",
            c_type="ifcopenshell_ifc_file_t",
            destructor="delete",
            ptr_type="raw",
        ),
        "instance": HandleSpec(
            name="instance",
            cpp_type="express::Base",
            c_type="ifcopenshell_ifc_instance_t",
            destructor="none",
            ptr_type="value",
            empty_check="!static_cast<bool>({value})",
        ),
        "instance_list": HandleSpec(
            name="instance_list",
            cpp_type="std::vector<express::Base>",
            c_type="ifcopenshell_ifcparse_instance_list_t",
            destructor="none",
            ptr_type="value",
        ),
    }


def test_ifcapi_spec_imports_core_handles_without_redefining_them() -> None:
    spec = _merge_ifcapi_guid_cpp_spec(_ifcapi_merged_with_core_handles())
    assert "ifcwrap/binding_generator/specs/cpp/ifcapi.hpp" in spec.public_headers
    assert spec.handles["value"].c_type == "ifcopenshell_ifcapi_value_t"
    assert spec.handles["value"].destructor == "function:ifcapi::bindings::value_free"
    assert spec.result_structs["ifcopenshell_sequence_duplicate_task_result_t"].c_type == (
        "ifcopenshell_sequence_duplicate_task_result_t"
    )

    assert {
        "ifcopenshell_ifcapi_element_get_type",
        "ifcopenshell_ifcapi_element_get_aggregate",
        "ifcopenshell_ifcapi_element_get_nest",
        "ifcopenshell_ifcapi_element_get_container",
        "ifcopenshell_ifcapi_element_get_parent",
        "ifcopenshell_ifcapi_element_get_material",
        "ifcopenshell_ifcapi_shape_is_x",
        "ifcopenshell_ifcapi_unit_get_prefix",
        "ifcopenshell_ifcapi_unit_get_measure_class",
        "ifcopenshell_ifcapi_unit_get_measure_unit_type",
        "ifcopenshell_ifcapi_unit_get_symbol_measure_class",
        "ifcopenshell_ifcapi_unit_get_si_dimensions",
        "ifcopenshell_ifcapi_unit_convert",
        "ifcopenshell_ifcapi_unit_format_length",
        "ifcopenshell_ifcapi_unit_get_unit_assignment",
        "ifcopenshell_ifcapi_unit_get_unit_symbol",
        "ifcopenshell_ifcapi_unit_convert_unit",
        "ifcopenshell_ifcapi_unit_resolve_property_unit",
        "ifcopenshell_ifcapi_unit_resolve_property_table_defined_measure_class",
        "ifcopenshell_ifcapi_unit_calculate_unit_scale",
        "ifcopenshell_ifcapi_selector_get_element_value",
        "ifcopenshell_ifcapi_selector_set_element_value",
        "ifcopenshell_ifcapi_compute_derived",
        "ifcopenshell_ifcapi_value_kind",
        "ifcopenshell_ifcapi_value_new_string",
        "ifcopenshell_ifcapi_value_list_append",
    }.issubset({call.c_name for call in spec.functions})
    calls = {call.c_name: call for call in spec.functions}
    assert calls["ifcopenshell_ifcapi_value_new_string"].returns.ownership == "owned"
    guid_calls = {call.c_name: call for call in spec.functions if call.c_name.startswith("ifcopenshell_ifcapi_guid_")}
    assert set(guid_calls) == {
        "ifcopenshell_ifcapi_guid_new",
        "ifcopenshell_ifcapi_guid_compress",
        "ifcopenshell_ifcapi_guid_expand",
    }
    assert guid_calls["ifcopenshell_ifcapi_guid_new"].returns.ownership == "owned"


def test_ifcapi_spec_lowers_to_host_binding_metadata() -> None:
    merged = _merge_ifcapi_guid_cpp_spec(_ifcapi_merged_with_core_handles())
    ir = lower_binding_spec(merged)
    calls = {call.c_name: call for call in ir.functions}

    get_container = calls["ifcopenshell_ifcapi_element_get_container"]
    assert isinstance(get_container.operation, DirectCallOp)
    assert get_container.operation.cpp_name == "ifcapi::bindings::element_get_container"
    assert get_container.returns.handle == "instance"
    assert [param.type.kind for param in get_container.params] == ["handle", "bool", "string"]

    python = render_python_ctypes(build_host_metadata(ir), generic_handles=True)
    assert '"ifcopenshell_ifcapi_guid_new": (' in python
    assert '"ifcopenshell_ifcapi_guid_compress": (' in python
    assert '"ifcopenshell_ifcapi_guid_expand": (' in python
    assert '"ifcopenshell_ifcapi_element_get_container": (' in python
    assert '"ifcopenshell_ifcapi_element_get_material": (' in python
    assert '"ifcopenshell_ifcapi_shape_is_x": (' in python
    assert '"ifcopenshell_ifcapi_unit_get_prefix": (' in python
    assert '"ifcopenshell_ifcapi_unit_get_measure_class": (' in python
    assert '"ifcopenshell_ifcapi_unit_get_si_dimensions": (' in python
    assert '"ifcopenshell_ifcapi_unit_format_length": (' in python
    assert '"ifcopenshell_ifcapi_unit_get_unit_assignment": (' in python
    assert '"ifcopenshell_ifcapi_unit_resolve_property_unit": (' in python
    assert '"ifcopenshell_ifcapi_compute_derived": (' in python
    assert '"ifcopenshell_ifcapi_value_kind": (' in python
    assert '"ifcopenshell_ifcapi_selector_set_element_value": (' in python
    assert '"ifcopenshell_ifcapi_value_new_string": (' in python
    assert "def bind(lib, *, strict=True, names=None, prefixes=None):" in python


def test_ifcapi_spec_merges_as_third_unified_slice(tmp_path: Path) -> None:
    core_spec = tmp_path / "ifcparse.yml"
    core_spec.write_text(
        """
schema_version: 1
module: ifcopenshell
slice: ifcparse
c_prefix: ifcopenshell_ifcparse
public_headers: []
handles:
  - name: file
    cpp_type: ifcopenshell::file
    c_type: ifcopenshell_ifc_file_t
    destructor: delete
  - name: instance
    cpp_type: express::Base
    c_type: ifcopenshell_ifc_instance_t
    destructor: none
    ptr_type: value
    empty_check: "!static_cast<bool>({value})"
  - name: instance_list
    cpp_type: std::vector<express::Base>
    c_type: ifcopenshell_ifcparse_instance_list_t
    destructor: none
    ptr_type: value
functions:
  - expose_as: instance_identity
    implementation:
      kind: inline_cpp
      body: |
        return instance;
    returns:
      kind: handle
      handle: instance
      ownership: borrowed
    params:
      - name: instance
        type:
          kind: handle
          handle: instance
          ownership: borrowed
""",
        encoding="utf-8",
    )
    geom_spec = tmp_path / "ifcgeom.yml"
    geom_spec.write_text(
        """
schema_version: 1
module: ifcopenshell
slice: ifcgeom
c_prefix: ifcopenshell_ifcgeom
public_headers: []
imports:
  - slice: ifcparse
    handles:
      - instance
functions:
  - expose_as: instance_visible
    implementation:
      kind: inline_cpp
      body: |
        return instance != nullptr;
    returns:
      kind: bool
    params:
      - name: instance
        type:
          kind: handle
          handle: instance
          ownership: borrowed
""",
        encoding="utf-8",
    )

    merged = load_merged_specs(
        [core_spec, geom_spec],
        module="ifcopenshell",
        c_prefix="ifcopenshell",
        compile_commands_path=_compile_commands(),
    )
    merged = _merge_ifcapi_guid_cpp_spec(merged)
    ir = lower_binding_spec(merged)
    calls = {call.c_name: call for call in (*ir.functions, *ir.methods)}

    assert "ifcopenshell_ifcparse_instance_identity" in calls
    assert "ifcopenshell_ifcgeom_instance_visible" in calls
    assert "ifcopenshell_ifcapi_guid_new" in calls
    assert "ifcopenshell_ifcapi_element_get_container" in calls
    assert calls["ifcopenshell_ifcapi_element_get_container"].params[0].type.handle == "instance"
