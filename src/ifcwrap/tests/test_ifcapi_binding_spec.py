# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

import yaml

from src.ifcwrap.binding_generator.authored_spec import HandleSpec, load_authored_spec, load_merged_specs
from src.ifcwrap.binding_generator.binding_ir import DirectCallOp, lower_binding_spec
from src.ifcwrap.binding_generator.host_metadata import build_host_metadata
from src.ifcwrap.binding_generator.python_ctypes_backend import render_python_ctypes


def _spec_dir() -> Path:
    return Path(__file__).resolve().parents[1] / "binding_generator" / "specs"


def _core_handles() -> dict[str, HandleSpec]:
    return {
        "file": HandleSpec(
            name="file",
            cpp_type="IfcParse::IfcFile",
            c_type="ifcopenshell_ifc_file_t",
            destructor="delete",
            ptr_type="raw",
        ),
        "instance": HandleSpec(
            name="instance",
            cpp_type="IfcUtil::IfcBaseClass",
            c_type="ifcopenshell_ifc_instance_t",
            destructor="delete",
            ptr_type="raw",
        ),
    }


def test_ifcapi_spec_imports_core_handles_without_redefining_them() -> None:
    spec_path = _spec_dir() / "ifcapi.yml"
    raw_spec = yaml.safe_load(spec_path.read_text(encoding="utf-8"))

    raw_handle_names = {handle["name"] for handle in raw_spec.get("handles", [])}
    assert raw_handle_names == {"value"}
    assert "instance" not in raw_handle_names
    assert raw_spec["imports"] == [{"slice": "ifcparse", "handles": ["file", "instance"]}]

    spec = load_authored_spec(spec_path, existing_handles=_core_handles())
    assert spec.slice == "ifcapi"
    assert "ifcapi/bindings/element.h" in spec.public_headers
    assert "ifcapi/bindings/entity.h" in spec.public_headers
    assert "ifcapi/bindings/shape.h" in spec.public_headers
    assert "ifcapi/bindings/unit.h" in spec.public_headers
    assert "ifcapi/bindings/value.h" in spec.public_headers
    assert spec.handles["value"].c_type == "ifcopenshell_ifcapi_value_t"
    assert spec.handles["value"].destructor == "function:ifcapi::bindings::value_free"
    assert {
        "ifcopenshell_ifcapi_guid_new",
        "ifcopenshell_ifcapi_guid_compress",
        "ifcopenshell_ifcapi_guid_expand",
        "ifcopenshell_ifcapi_element_get_type",
        "ifcopenshell_ifcapi_element_get_aggregate",
        "ifcopenshell_ifcapi_element_get_nest",
        "ifcopenshell_ifcapi_element_get_container",
        "ifcopenshell_ifcapi_element_get_parent",
        "ifcopenshell_ifcapi_element_get_material",
        "ifcopenshell_ifcapi_entity_set_typed_value",
        "ifcopenshell_ifcapi_entity_get_typed_value",
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
        "ifcopenshell_ifcapi_selector_get_element_value",
        "ifcopenshell_ifcapi_selector_set_element_value",
        "ifcopenshell_ifcapi_compute_derived",
        "ifcopenshell_ifcapi_value_kind",
        "ifcopenshell_ifcapi_value_new_string",
        "ifcopenshell_ifcapi_value_list_append",
    }.issubset({call.c_name for call in spec.functions})


def test_ifcapi_spec_lowers_to_host_binding_metadata() -> None:
    spec_path = _spec_dir() / "ifcapi.yml"
    ir = lower_binding_spec(load_authored_spec(spec_path, existing_handles=_core_handles()))
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
  - name: instance
    cpp_type: IfcUtil::IfcBaseClass
    c_type: ifcopenshell_ifc_instance_t
    destructor: delete
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
        [core_spec, geom_spec, _spec_dir() / "ifcapi.yml"],
        module="ifcopenshell",
        c_prefix="ifcopenshell",
    )
    ir = lower_binding_spec(merged)
    calls = {call.c_name: call for call in (*ir.functions, *ir.methods)}

    assert "ifcopenshell_ifcparse_instance_identity" in calls
    assert "ifcopenshell_ifcgeom_instance_visible" in calls
    assert "ifcopenshell_ifcapi_element_get_container" in calls
    assert calls["ifcopenshell_ifcapi_element_get_container"].params[0].type.handle == "instance"
