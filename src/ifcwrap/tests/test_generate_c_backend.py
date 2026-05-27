from __future__ import annotations

from pathlib import Path
from types import SimpleNamespace

import pytest

from src.ifcwrap.binding_generator.binding_model import TypeSpec
from src.ifcwrap.binding_generator.binding_ir import BindingIR
from src.ifcwrap.binding_generator.c_backend import _render_result_assignment
from src.ifcwrap.binding_generator.c_backend import generate
from src.ifcwrap.binding_generator.c_runtime_support import _render_cpp_support_runtime
from src.ifcwrap.tests._binding_generator_test_utils import find_repo_compile_commands


def test_runtime_support_renders_cpp_braces_and_json_literals() -> None:
    runtime = _render_cpp_support_runtime()

    assert "namespace ifcopenshell {" in runtime
    assert "namespace ifcopenshell {{" not in runtime
    assert 'return std::string(R"({"ref":)") + std::to_string(reference.v) + "}";' in runtime
    assert 'out << "{";' in runtime
    assert 'out << "}";' in runtime
    assert "{{" not in runtime
    assert "}}" not in runtime


def test_nullable_string_result_returns_successful_null_string_without_allocating() -> None:
    generated = _render_result_assignment(
        SimpleNamespace(returns=TypeSpec(kind="string", ownership="copy", nullable=True)),
        BindingIR(
            module="fixture",
            c_prefix="ifcopenshell_fixture",
            public_headers=(),
            handles={},
            result_structs={},
            functions=(),
            methods=(),
        ),
        "example::maybe_name()",
    )

    assert "auto result_value = example::maybe_name();" in generated
    assert "if (!result_value)" in generated
    assert "if (!g_last_error.empty()) { return false; }" in generated
    assert "*out_result = ifcopenshell_string_t{nullptr, 0, false};" in generated
    assert "*out_result = make_string(*result_value);" in generated


def test_generate_ifcparse_c_backend_with_compile_commands_when_available(tmp_path: Path) -> None:
    spec = Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml"
    compile_commands = find_repo_compile_commands()
    if compile_commands is None:
        return

    header_out = tmp_path / "ifcopenshell_api.h"
    cpp_out = tmp_path / "ifcopenshell_api.cpp"

    generate(spec, header_out, cpp_out, compile_commands_path=compile_commands)

    header = header_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_traverse(ifcopenshell_ifc_instance_t* instance, int32_t max_depth, ifcopenshell_ifc_instance_list_t* out_result);" in header
