from __future__ import annotations

import json
from pathlib import Path
import shlex
from types import SimpleNamespace

import pytest

from src.ifcwrap.binding_generator.binding_model import ParamSpec, TypeSpec
from src.ifcwrap.binding_generator.binding_ir import BindingIR
from src.ifcwrap.binding_generator.c_backend import _render_result_assignment
from src.ifcwrap.binding_generator.c_call_rendering import _render_param_prelude
from src.ifcwrap.binding_generator.c_backend import generate_merged
from src.ifcwrap.binding_generator.c_runtime_support import _render_cpp_support_runtime
from src.ifcwrap.tests._binding_generator_test_utils import find_repo_compile_commands


def _compile_command_include_dirs(compile_commands: Path, file_suffix: str) -> tuple[Path, ...]:
    entries = json.loads(compile_commands.read_text(encoding="utf-8"))
    entry = next((item for item in entries if item["file"].endswith(file_suffix)), None)
    if entry is None:
        return ()
    args = entry.get("arguments") or shlex.split(entry["command"])
    include_dirs: list[Path] = []
    index = 0
    while index < len(args):
        arg = args[index]
        if arg in {"-I", "-isystem"} and index + 1 < len(args):
            include_dirs.append(Path(args[index + 1]))
            index += 2
            continue
        if arg.startswith("-I") and len(arg) > 2:
            include_dirs.append(Path(arg[2:]))
        elif arg.startswith("-isystem") and len(arg) > len("-isystem"):
            include_dirs.append(Path(arg[len("-isystem") :]))
        index += 1
    return tuple(include_dirs)


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


def test_size_param_prelude_preserves_size_t_abi_width() -> None:
    generated = _render_param_prelude(
        ParamSpec(name="start", type=TypeSpec(kind="size", cpp_type="unsigned long")),
        BindingIR(
            module="fixture",
            c_prefix="ifcopenshell_fixture",
            public_headers=(),
            handles={},
            result_structs={},
            functions=(),
            methods=(),
        ),
    )

    assert generated == "    auto start_cpp = static_cast<size_t>(start);"


def test_generate_ifcparse_c_backend_with_compile_commands_when_available(tmp_path: Path) -> None:
    spec_dir = Path(__file__).resolve().parents[1] / "binding_generator" / "specs"
    spec = spec_dir / "ifcparse.yml"
    cpp_spec = spec_dir / "cpp" / "ifcparse.hpp"
    compile_commands = find_repo_compile_commands()
    if compile_commands is None:
        return

    header_out = tmp_path / "ifcopenshell_api.h"
    cpp_out = tmp_path / "ifcopenshell_api.cpp"
    repo_src = Path(__file__).resolve().parents[2]
    include_dirs = [repo_src, repo_src / "ifcparse", repo_src / "ifcparse" / "schemas"]
    include_dirs.extend(_compile_command_include_dirs(compile_commands, "/src/ifcparse/parse.cpp"))

    generate_merged(
        [spec],
        "ifcopenshell",
        "ifcopenshell",
        header_out,
        cpp_out,
        compile_commands_path=compile_commands,
        discovery_include_dirs=tuple(include_dirs),
        cpp_spec_paths=[cpp_spec],
        cpp_spec_namespace="ifcparse::bindings",
        cpp_spec_c_prefix="ifcopenshell_ifcparse",
        cpp_spec_handle_c_prefix="ifcopenshell_ifc",
    )

    header = header_out.read_text(encoding="utf-8")
    cpp = cpp_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_ifc_file_by_id(ifcopenshell_ifc_file_t* self, int32_t instance_id, ifcopenshell_ifc_instance_t** out_result);" in header
    assert "bool ifcopenshell_ifcparse_instance_list_create_from_handles(const ifcopenshell_ifc_instance_list_t* instances, ifcopenshell_ifcparse_instance_list_t** out_result);" in header
    assert cpp.count("ifcparse::bindings::as_instance_list(self_cpp)") == 1
    assert "ifcparse::bindings::as_instance_list(self_cpp).begin()" not in cpp
