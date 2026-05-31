# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
from types import SimpleNamespace
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.binding_ir import DirectCallOp, SpecMethodFunctionCallOp, lower_binding_spec
from src.ifcwrap.binding_generator.c_backend import _render_cpp, generate_cpp_specs
from src.ifcwrap.binding_generator.c_header_rendering import _render_header
from src.ifcwrap.binding_generator.c_internal_header import _render_internal_header
from src.ifcwrap.binding_generator.clang_discovery import CompilationConfig, DiscoveryEnvironment
from src.ifcwrap.binding_generator.cpp_spec_frontend import (
    discover_cpp_spec_functions,
    discover_cpp_spec_handles,
    discover_cpp_spec_result_structs,
    lower_cpp_spec_functions_to_calls,
    lower_cpp_spec_handles_to_specs,
    lower_cpp_spec_result_structs_to_specs,
)
from src.ifcwrap.tests._binding_generator_test_utils import find_repo_compile_commands


def _environment(tmp_path: Path) -> DiscoveryEnvironment:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")
    return DiscoveryEnvironment(
        compilation=CompilationConfig(
            compiler=compiler,
            include_dirs=(tmp_path,),
            working_directory=tmp_path,
        )
    )


def _compile_command_for_source(compile_commands_path: Path, source: Path, include_dir: Path) -> list[str]:
    commands = json.loads(compile_commands_path.read_text(encoding="utf-8"))
    command = next(
        (
            item
            for item in commands
            if str(item.get("file", "")).endswith(".cpp") and "ifcopenshell_api" in str(item.get("file", ""))
        ),
        next(item for item in commands if str(item.get("file", "")).endswith(".cpp")),
    )
    args = list(command.get("arguments") or shlex.split(command["command"]))
    reference_file = str(command["file"])
    filtered = [args[0], "-fsyntax-only", "-I", str(include_dir)]
    skip_next = False
    for arg in args[1:]:
        if skip_next:
            skip_next = False
            continue
        if arg in {"-c", "-o"}:
            skip_next = arg == "-o"
            continue
        if arg.startswith("-o"):
            continue
        if arg == reference_file:
            continue
        filtered.append(arg)
    filtered.append(str(source))
    return filtered


def test_cpp_spec_frontend_discovers_only_marked_exports(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>
            #include <vector>

            #define IFCAPI_EXPORT
            #define IFCAPI_HANDLE(cpp_type, destructor, ...)
            #define IFCAPI_OWNED
            #define IFCAPI_STATIC
            #define IFCAPI_NULLABLE

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            int helper_not_exported(const DemoFile&) {
                return 42;
            }

            IFCAPI_EXPORT IFCAPI_OWNED std::string ifcopenshell_demo_label(
                IFCAPI_NULLABLE const DemoFile* file,
                const std::vector<int>& values
            ) {
                return file && !values.empty() ? "ok" : "empty";
            }

            IFCAPI_EXPORT bool ifcopenshell_demo_is_empty(const DemoFile& file) {
                return helper_not_exported(file) == 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )

    by_name = {function.name: function for function in functions}
    assert set(by_name) == {"ifcopenshell_demo_is_empty", "ifcopenshell_demo_label"}
    assert by_name["ifcopenshell_demo_label"].return_annotations == frozenset({"IFCAPI_OWNED"})
    assert by_name["ifcopenshell_demo_label"].param_annotations == {
        "file": frozenset({"IFCAPI_NULLABLE"})
    }
    assert by_name["ifcopenshell_demo_label"].discovered.return_cpp_type == "std::string"
    assert [param.cpp_type for param in by_name["ifcopenshell_demo_label"].discovered.params] == [
        "const DemoFile *",
        "const std::vector<int> &",
    ]


def test_cpp_spec_frontend_lowers_marked_exports_to_binding_ir_calls(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>
            #include <vector>

            #define IFCAPI_EXPORT
            #define IFCAPI_HANDLE(cpp_type, destructor, ...)
            #define IFCAPI_OWNED
            #define IFCAPI_STATIC
            #define IFCAPI_NULLABLE
            #define IFCAPI_HANDLE_PARAM(handle)
            #define IFCAPI_RESULT_STRUCT(c_type)

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;
            IFCAPI_HANDLE(demo_file_list, std::vector<ifcopenshell::capi_spec::DemoFile>, none, value)
            struct ifcopenshell_demo_file_list_t;

            IFCAPI_RESULT_STRUCT(ifcopenshell_demo_summary_t)
            struct DemoSummary {
                std::string label;
                std::vector<int> values;
            };

            IFCAPI_EXPORT IFCAPI_OWNED std::string ifcopenshell_demo_label(
                IFCAPI_NULLABLE const DemoFile* file,
                const std::vector<int>& values
            ) {
                return file && !values.empty() ? "ok" : "empty";
            }

            IFCAPI_EXPORT DemoSummary ifcopenshell_demo_summary(const DemoFile* file) {
                return {file ? "ok" : "empty", {1, 2, 3}};
            }

            IFCAPI_EXPORT void ifcopenshell_demo_use_file_list(
                IFCAPI_HANDLE_PARAM(demo_file_list) std::vector<DemoFile>* files
            ) {}

            IFCAPI_EXPORT IFCAPI_STATIC const char* ifcopenshell_demo_version() {
                return "fixture";
            }

            IFCAPI_EXPORT int ifcopenshell_demo_buffer_size(const void* data) {
                return data ? 1 : 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    handles = lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(spec_path))
    assert set(handles) == {"demo_file", "demo_file_list"}
    assert handles["demo_file"].cpp_type == "ifcopenshell::capi_spec::DemoFile"
    assert handles["demo_file"].c_type == "ifcopenshell_demo_file_t"
    assert handles["demo_file"].destructor == "delete"
    result_structs = lower_cpp_spec_result_structs_to_specs(
        discover_cpp_spec_result_structs(spec_path, "ifcopenshell::capi_spec"),
        handles,
    )
    assert set(result_structs) == {"DemoSummary"}
    assert result_structs["DemoSummary"].c_type == "ifcopenshell_demo_summary_t"
    assert [field.name for field in result_structs["DemoSummary"].fields] == ["label", "values"]
    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )
    calls = lower_cpp_spec_functions_to_calls(functions, handles, result_structs)
    ir = lower_binding_spec(
        SimpleNamespace(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(),
            public_header_plugins={},
            handles=handles,
            result_structs=result_structs,
            functions=calls,
            methods=(),
            depends_on_common=None,
        )
    )

    assert len(ir.functions) == 5
    calls_by_name = {call.c_name: call for call in ir.functions}
    call = calls_by_name["ifcopenshell_demo_label"]
    assert call.c_name == "ifcopenshell_demo_label"
    assert call.returns.kind == "string"
    assert call.returns.ownership == "owned"
    assert call.params[0].type.kind == "handle"
    assert call.params[0].type.handle == "demo_file"
    assert call.params[0].type.nullable is True
    assert call.params[1].type.kind == "int32"
    assert call.params[1].type.sequence_depth == 1
    assert isinstance(call.operation, DirectCallOp)
    assert call.operation.cpp_name == "ifcopenshell::capi_spec::ifcopenshell_demo_label"
    assert calls_by_name["ifcopenshell_demo_version"].returns.kind == "string"
    assert calls_by_name["ifcopenshell_demo_version"].returns.ownership == "static"
    assert calls_by_name["ifcopenshell_demo_buffer_size"].params[0].type.kind == "opaque_ptr"
    list_param = calls_by_name["ifcopenshell_demo_use_file_list"].params[0].type
    assert list_param.kind == "handle"
    assert list_param.handle == "demo_file_list"
    assert list_param.sequence_depth == 0

    header = _render_header(ir)
    assert "typedef struct ifcopenshell_demo_file_t ifcopenshell_demo_file_t;" in header
    assert "typedef struct ifcopenshell_int32_list_t" in header
    assert (
        "bool ifcopenshell_demo_label(ifcopenshell_demo_file_t* file, "
        "const ifcopenshell_int32_list_t* values, ifcopenshell_string_t* out_result);"
    ) in header
    assert "bool ifcopenshell_demo_use_file_list(ifcopenshell_demo_file_list_t* files);" in header
    assert "typedef struct ifcopenshell_demo_summary_t" in header
    assert "ifcopenshell_string_t label;" in header
    assert "ifcopenshell_int32_list_t values;" in header
    assert (
        "bool ifcopenshell_demo_summary(ifcopenshell_demo_file_t* file, "
        "ifcopenshell_demo_summary_t* out_result);"
    ) in header


def test_cpp_spec_frontend_lowers_method_exports_to_receiver_calls(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>

            #define IFCAPI_EXPORT
            #define IFCAPI_METHOD(receiver)
            #define IFCAPI_HANDLE(cpp_type, destructor)
            #define IFCAPI_OWNED

            namespace ifcopenshell::capi_spec {
            struct DemoFile {
                std::string label(int index) const { return index ? "other" : "zero"; }
            };
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            IFCAPI_EXPORT IFCAPI_METHOD(demo_file) IFCAPI_OWNED std::string label(const DemoFile& self, int index) {
                return self.label(index);
            }
            }
            """
        ),
        encoding="utf-8",
    )

    handles = lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(spec_path))
    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )
    assert functions[0].receiver == "demo_file"
    calls = lower_cpp_spec_functions_to_calls(functions, handles)
    ir = lower_binding_spec(
        SimpleNamespace(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(),
            public_header_plugins={},
            handles=handles,
            result_structs={},
            functions=calls,
            methods=(),
            depends_on_common=None,
        )
    )

    call = ir.functions[0]
    assert call.c_name == "ifcopenshell_demo_file_label"
    assert call.receiver == "demo_file"
    assert [param.name for param in call.params] == ["index"]
    assert isinstance(call.operation, SpecMethodFunctionCallOp)

    header = _render_header(ir)
    assert "bool ifcopenshell_demo_file_label(ifcopenshell_demo_file_t* self, int32_t index, ifcopenshell_string_t* out_result);" in header
    cpp = _render_cpp(ir, "demo.h")
    assert "ifcopenshell::capi_spec::label(*self_cpp, index_cpp)" in cpp


def test_cpp_spec_frontend_rejects_mismatched_method_receiver_type(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT
            #define IFCAPI_METHOD(receiver)
            #define IFCAPI_HANDLE(cpp_type, destructor)

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            struct Other {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            IFCAPI_EXPORT IFCAPI_METHOD(demo_file) int invalid(const Other& self) {
                return 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    handles = lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(spec_path))
    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )
    with pytest.raises(ValueError, match="receiver parameter type"):
        lower_cpp_spec_functions_to_calls(functions, handles)


def test_cpp_spec_generation_supports_per_spec_namespaces_and_prefixes(tmp_path: Path) -> None:
    spec_a = tmp_path / "spec_a.cpp"
    spec_a.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT

            namespace example::a {
            IFCAPI_EXPORT int value() {
                return 1;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    spec_b = tmp_path / "spec_b.cpp"
    spec_b.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT

            namespace example::b {
            IFCAPI_EXPORT int value() {
                return 2;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    header_out = tmp_path / "generated.h"
    cpp_out = tmp_path / "generated.cpp"
    generate_cpp_specs(
        [spec_a, spec_b],
        ["example::a", "example::b"],
        "example",
        "example",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
        function_c_prefix=["example_a", "example_b"],
    )

    header = header_out.read_text(encoding="utf-8")
    assert "bool example_a_value(int32_t* out_result);" in header
    assert "bool example_b_value(int32_t* out_result);" in header
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "example::a::value()" in generated_cpp
    assert "example::b::value()" in generated_cpp


def test_cpp_spec_generation_rejects_mismatched_per_spec_namespaces(tmp_path: Path) -> None:
    spec_a = tmp_path / "spec_a.cpp"
    spec_b = tmp_path / "spec_b.cpp"
    for spec_path in (spec_a, spec_b):
        spec_path.write_text(
            dedent(
                """
                #define IFCAPI_EXPORT

                namespace example {
                IFCAPI_EXPORT int value() {
                    return 1;
                }
                }
                """
            ),
            encoding="utf-8",
        )

    with pytest.raises(ValueError, match="cpp_spec_namespace must be provided once or exactly once per --cpp-spec"):
        generate_cpp_specs(
            [spec_a, spec_b],
            ["example", "example", "extra"],
            "example",
            "example",
            tmp_path / "generated.h",
            tmp_path / "generated.cpp",
            discovery_include_dirs=(tmp_path,),
        )


def test_cpp_spec_frontend_rejects_mutable_void_pointer_params(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT

            namespace ifcopenshell::capi_spec {
            IFCAPI_EXPORT int ifcopenshell_demo_set_box(void* data) {
                return data ? 1 : 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )

    with pytest.raises(ValueError, match="Unsupported discovered parameter type 'void \\*'"):
        lower_cpp_spec_functions_to_calls(functions, {})


def test_cpp_spec_frontend_generated_fixture_cpp_compiles(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")
    compile_commands = find_repo_compile_commands()
    if compile_commands is None:
        pytest.skip("compile_commands.json is required for generated C++ fixture compilation")

    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>
            #include <vector>

            #define IFCAPI_EXPORT
            #define IFCAPI_HANDLE(cpp_type, destructor)
            #define IFCAPI_OWNED
            #define IFCAPI_NULLABLE

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            IFCAPI_EXPORT IFCAPI_OWNED std::string ifcopenshell_demo_label(
                IFCAPI_NULLABLE const DemoFile* file,
                const std::vector<int>& values
            ) {
                return file && !values.empty() ? "ok" : "empty";
            }
            }
            """
        ),
        encoding="utf-8",
    )
    handles = lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(spec_path))
    calls = lower_cpp_spec_functions_to_calls(
        discover_cpp_spec_functions(_environment(tmp_path), spec_path, "ifcopenshell::capi_spec"),
        handles,
    )
    ir = lower_binding_spec(
        SimpleNamespace(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(spec_path.name,),
            public_header_plugins={},
            handles=handles,
            result_structs={},
            functions=calls,
            methods=(),
            depends_on_common=None,
        )
    )

    header_path = tmp_path / "demo_api.h"
    internal_header_path = tmp_path / "demo_api_internal.hpp"
    cpp_path = tmp_path / "demo_api.cpp"
    header_path.write_text(_render_header(ir), encoding="utf-8")
    internal_header_path.write_text(_render_internal_header(ir, header_path.name), encoding="utf-8")
    cpp_path.write_text(
        dedent(
            """
            #include "argument_type.h"
            #include "express.h"
            #include "file.h"
            #include "instance_data.h"
            #include "logger.h"
            #include "storage.h"
            #include <boost/logic/tribool.hpp>
            """
        )
        + _render_cpp(ir, header_path.name),
        encoding="utf-8",
    )

    result = subprocess.run(
        _compile_command_for_source(compile_commands, cpp_path, tmp_path),
        check=False,
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, result.stderr


def test_cpp_spec_frontend_cli_generates_fixture_header(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>

            #define IFCAPI_EXPORT
            #define IFCAPI_HANDLE(cpp_type, destructor)
            #define IFCAPI_OWNED

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            IFCAPI_EXPORT IFCAPI_OWNED std::string ifcopenshell_demo_label(const DemoFile* file) {
                return file ? "ok" : "empty";
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    backend = Path(__file__).resolve().parents[1] / "binding_generator" / "c_backend.py"

    result = subprocess.run(
        [
            sys.executable,
            str(backend),
            "--cpp-spec",
            str(spec_path),
            "--cpp-spec-namespace",
            "ifcopenshell::capi_spec",
            "--header-out",
            str(header_out),
            "--cpp-out",
            str(cpp_out),
            "--discovery-include-dir",
            str(tmp_path),
            "--module",
            "demo",
            "--c-prefix",
            "ifcopenshell_demo",
        ],
        check=False,
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, result.stderr
    header = header_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_demo_file_t ifcopenshell_demo_file_t;" in header
    assert (
        "bool ifcopenshell_demo_label(ifcopenshell_demo_file_t* file, "
        "ifcopenshell_string_t* out_result);"
    ) in header


def test_cpp_spec_frontend_rejects_overloaded_exports(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT

            namespace ifcopenshell::capi_spec {
            IFCAPI_EXPORT int ifcopenshell_demo_value(int value) {
                return value;
            }
            IFCAPI_EXPORT int ifcopenshell_demo_value(double value) {
                return static_cast<int>(value);
            }
            }
            """
        ),
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="declared more than once"):
        discover_cpp_spec_functions(
            _environment(tmp_path),
            spec_path,
            "ifcopenshell::capi_spec",
        )


def test_cpp_spec_frontend_rejects_duplicate_handles(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_HANDLE(cpp_type, destructor)

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;
            }
            """
        ),
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="declared more than once"):
        discover_cpp_spec_handles(spec_path)
