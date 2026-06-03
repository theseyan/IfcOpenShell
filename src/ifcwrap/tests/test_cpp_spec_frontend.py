# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
import shutil
import subprocess
import sys
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.c_backend import generate_cpp_specs
from src.ifcwrap.binding_generator.clang_discovery import CompilationConfig, DiscoveryEnvironment
from src.ifcwrap.binding_generator.cpp_spec_frontend import (
    discover_cpp_spec_functions,
    discover_cpp_spec_handles,
    lower_cpp_spec_functions_to_calls,
)


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


def test_cpp_spec_generation_supports_per_spec_namespaces_and_prefixes(tmp_path: Path) -> None:
    spec_a = tmp_path / "spec_a.cpp"
    spec_a.write_text(
        dedent(
            """
            namespace example::a {
            inline int value() {
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
            namespace example::b {
            inline int value() {
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
                namespace example {
                inline int value() {
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
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_set_box(void* data) {
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


def test_cpp_spec_frontend_cli_generates_fixture_header(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>

            #define IFCAPI_HANDLE(cpp_type, destructor)
            #define IFCAPI_OWNED

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            inline IFCAPI_OWNED std::string ifcopenshell_demo_label(const DemoFile* file) {
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
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_value(int value) {
                return value;
            }
            inline int ifcopenshell_demo_value(double value) {
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


def test_cpp_spec_frontend_excludes_private_helpers(tmp_path: Path) -> None:
    from unittest.mock import patch

    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_exported() {
                return 42;
            }
            inline int ifcopenshell_demo_internal_helper() {
                return 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    with patch(
        "src.ifcwrap.binding_generator.cpp_spec_frontend._PRIVATE_NAMES",
        frozenset({"to_base_vector", "ifcopenshell_demo_internal_helper"}),
    ):
        functions = discover_cpp_spec_functions(
            _environment(tmp_path),
            spec_path,
            "ifcopenshell::capi_spec",
        )

    exported_names = {f.name for f in functions}
    assert "ifcopenshell_demo_exported" in exported_names
    assert "ifcopenshell_demo_internal_helper" not in exported_names


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
