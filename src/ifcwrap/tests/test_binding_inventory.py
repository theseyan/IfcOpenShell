# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import ast
from pathlib import Path
import re
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.binding_inventory import build_inventory, parse_c_functions, parse_ctypes_symbols
from src.ifcwrap.binding_generator.c_backend import generate_merged


_C_TYPEDEF_RE = re.compile(r"\btypedef\s+(?:struct|enum)\s+(ifcopenshell_[A-Za-z0-9_]+_t)\b")
_INTERNAL_STRUCT_RE = re.compile(r"^\s*struct\s+(ifcopenshell_[A-Za-z0-9_]+_t)\s*\{", re.MULTILINE)
_HELPER_BODY_RE = re.compile(
    r"\b(?:static\s+)?[\w:<>,\s\*&]+?\s+"
    r"(?P<name>(?:make|to_cpp)_[A-Za-z0-9_]+|ifcopenshell_[A-Za-z0-9_]+_destroy)\s*\(",
    re.MULTILINE,
)


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def _c_function_signatures(path: Path, repo_root: Path) -> dict[str, tuple[str, str]]:
    return {function.name: (function.return_type, function.params) for function in parse_c_functions(path, repo_root)}


def _c_type_names(path: Path) -> set[str]:
    return set(_C_TYPEDEF_RE.findall(path.read_text(encoding="utf-8")))


def _internal_struct_names(path: Path) -> set[str]:
    return set(_INTERNAL_STRUCT_RE.findall(path.read_text(encoding="utf-8")))


def _python_generated_types(path: Path) -> set[str]:
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    names: set[str] = set()
    for node in ast.walk(tree):
        if isinstance(node, ast.ClassDef) and node.name.startswith("ifcopenshell_") and node.name.endswith("_t"):
            names.add(node.name)
        elif isinstance(node, ast.Assign):
            for target in node.targets:
                if isinstance(target, ast.Name) and target.id.startswith("ifcopenshell_") and target.id.endswith("_t"):
                    names.add(target.id)
    return names


def _cpp_helper_bodies(path: Path) -> dict[str, str]:
    text = path.read_text(encoding="utf-8")
    bodies: dict[str, str] = {}
    for match in _HELPER_BODY_RE.finditer(text):
        name = match.group("name")
        body_start = text.find("{", match.end())
        if body_start == -1:
            continue
        depth = 0
        for index in range(body_start, len(text)):
            if text[index] == "{":
                depth += 1
            elif text[index] == "}":
                depth -= 1
                if depth == 0:
                    bodies[name] = text[body_start : index + 1]
                    break
    return bodies


def test_parse_c_functions_handles_exported_multiline_declarations(tmp_path: Path) -> None:
    header = tmp_path / "api.h"
    header.write_text(
        dedent(
            """
            #define IFCAPI_EXPORT
            IFCAPI_EXPORT struct ifcopenshell_ifc_instance_t*
            ifcopenshell_file_create_entity(
                ifcopenshell_ifc_file_t* file,
                const char* type_name);

            bool ifcopenshell_ifc_file_by_type(
                ifcopenshell_ifc_file_t* self,
                const char* type,
                ifcopenshell_ifcparse_instance_list_t** out_result);
            """
        ),
        encoding="utf-8",
    )

    functions = {function.name: function for function in parse_c_functions(header, tmp_path)}

    assert set(functions) == {"ifcopenshell_file_create_entity", "ifcopenshell_ifc_file_by_type"}
    assert functions["ifcopenshell_file_create_entity"].return_type == "struct ifcopenshell_ifc_instance_t*"
    assert "const char* type_name" in functions["ifcopenshell_file_create_entity"].params


def test_parse_ctypes_symbols_finds_signature_dicts_and_direct_assignments(tmp_path: Path) -> None:
    source = tmp_path / "wrapper.py"
    source.write_text(
        dedent(
            """
            def _bind(lib):
                sigs = {
                    "ifcopenshell_ifc_file_by_type": (c_bool, []),
                    "ifcopenshell_string_destroy": (None, []),
                }
                lib.ifcopenshell_file_create_entity.restype = c_void_p
                lib.ifcopenshell_file_create_entity.argtypes = [c_void_p]
                getattr(lib, "ifcopenshell_last_error_message").restype = c_char_p
            """
        ),
        encoding="utf-8",
    )

    symbols = parse_ctypes_symbols(source, tmp_path)

    assert set(symbols) == {
        "ifcopenshell_file_create_entity",
        "ifcopenshell_ifc_file_by_type",
        "ifcopenshell_last_error_message",
        "ifcopenshell_string_destroy",
    }


def test_build_inventory_reports_generated_highlevel_and_duplicate_concepts(tmp_path: Path) -> None:
    generated_dir = tmp_path / "src" / "ifcwrap" / "binding_generator" / "generated"
    generated_dir.mkdir(parents=True)
    (generated_dir / "ifcopenshell_api.h").write_text(
        dedent(
            """
            bool ifcopenshell_ifc_file_by_type(ifcopenshell_ifc_file_t* self, const char* type, void** out_result);
            bool ifcopenshell_ifcparse_schema_names(ifcopenshell_string_list_t* out_result);
            void ifcopenshell_string_destroy(ifcopenshell_string_t* value);
            """
        ),
        encoding="utf-8",
    )

    highlevel_dir = tmp_path / "src" / "ifcapi" / "include" / "ifcapi"
    highlevel_dir.mkdir(parents=True)
    (highlevel_dir / "ifcapi.h").write_text(
        "IFCAPI_EXPORT int32_t ifcopenshell_file_by_type("
        "const ifcopenshell_ifc_file_t* file, const char* type, void** out);\n",
        encoding="utf-8",
    )

    python_dir = tmp_path / "src" / "ifcapi" / "python" / "ifcopenshell"
    python_dir.mkdir(parents=True)
    (python_dir / "ifcopenshell_wrapper.py").write_text(
        'sigs = {"ifcopenshell_ifc_file_by_type": (c_bool, []), "ifcopenshell_file_by_type": (c_int, [])}\n',
        encoding="utf-8",
    )
    (python_dir / "_generated_capi.py").write_text(
        "\n".join(
            [
                "# This file was generated by IfcOpenShell's binding generator. Do not edit manually.",
                "FUNCTION_SIGNATURES = {",
                '    "ifcopenshell_ifcparse_schema_names": (c_bool, []),',
                "}",
                "",
            ]
        ),
        encoding="utf-8",
    )

    inventory = build_inventory(tmp_path)

    assert inventory["generated_c"]["symbol_count"] == 3
    assert inventory["generated_highlevel_c"]["symbol_count"] == 0
    assert inventory["handwritten_highlevel_c"]["symbol_count"] == 1
    assert inventory["generated_python_ctypes"]["symbol_count"] == 1
    assert inventory["generated_python_ctypes"]["missing_generated_c_symbols"] == [
        "ifcopenshell_ifc_file_by_type",
        "ifcopenshell_string_destroy",
    ]
    assert inventory["manual_python_ctypes"]["symbol_count"] == 2
    assert inventory["manual_python_ctypes"]["unexpected_generated_c_redeclarations"] == [
        {"name": "ifcopenshell_ifc_file_by_type", "source": "src/ifcapi/python/ifcopenshell/ifcopenshell_wrapper.py"}
    ]
    assert inventory["duplication"]["potential_core_highlevel_count"] == 1
    duplicate = inventory["duplication"]["potential_core_highlevel"][0]
    assert duplicate["highlevel"] == "ifcopenshell_file_by_type"
    assert duplicate["exact_core_matches"] == ["ifcopenshell_ifc_file_by_type"]


def test_generated_python_ctypes_cover_generated_c_api() -> None:
    repo_root = Path(__file__).resolve().parents[3]
    inventory = build_inventory(repo_root)

    assert inventory["generated_python_ctypes"]["missing_from_c_headers"] == []
    assert inventory["generated_python_ctypes"]["missing_generated_c_symbols"] == []


def test_manual_ctypes_generated_c_redeclarations_are_classified() -> None:
    repo_root = Path(__file__).resolve().parents[3]
    inventory = build_inventory(repo_root)

    assert inventory["manual_python_ctypes"]["unexpected_generated_c_redeclarations"] == []
    assert inventory["manual_python_ctypes"]["allowed_generated_c_redeclarations"] == []


def test_no_unknown_manual_ctypes_or_duplicate_c_exports() -> None:
    repo_root = Path(__file__).resolve().parents[3]
    inventory = build_inventory(repo_root)

    assert inventory["manual_python_ctypes"]["missing_from_c_headers"] == []
    assert inventory["duplication"]["exact_symbol_collisions"] == []
    assert inventory["duplication"]["potential_core_highlevel"] == []


def test_generated_highlevel_c_symbols_are_reported_separately() -> None:
    repo_root = _repo_root()
    inventory = build_inventory(repo_root)

    generated_highlevel = inventory["generated_highlevel_c"]["symbols"]
    assert inventory["generated_highlevel_c"]["symbol_count"] == len(generated_highlevel)
    assert "ifcopenshell_ifcapi_unit_convert" in generated_highlevel
    assert "ifcopenshell_ifcapi_value_kind" in generated_highlevel


def test_fresh_generation_matches_checked_in_api_semantics(tmp_path: Path) -> None:
    repo_root = _repo_root()
    compile_commands = repo_root / "build-capi-stable" / "compile_commands.json"
    if not compile_commands.exists():
        pytest.skip("build-capi-stable/compile_commands.json is required for fresh generated-output coverage")

    spec_dir = repo_root / "src" / "ifcwrap" / "binding_generator" / "specs"
    fresh_header = tmp_path / "ifcopenshell_api.h"
    fresh_cpp = tmp_path / "ifcopenshell_api.cpp"
    fresh_internal = tmp_path / "ifcopenshell_api_internal.hpp"
    fresh_python = tmp_path / "_generated_capi.py"

    generate_merged(
        [spec_dir / "ifcparse.yml", spec_dir / "ifcgeom.yml", spec_dir / "ifcapi.yml"],
        "ifcopenshell",
        "ifcopenshell",
        fresh_header,
        fresh_cpp,
        compile_commands_path=compile_commands,
        internal_header_out=fresh_internal,
        python_out=fresh_python,
    )

    generated_dir = repo_root / "src" / "ifcwrap" / "binding_generator" / "generated"
    checked_header = generated_dir / "ifcopenshell_api.h"
    checked_cpp = generated_dir / "ifcopenshell_api.cpp"
    checked_internal = generated_dir / "ifcopenshell_api_internal.hpp"
    checked_python = repo_root / "src" / "ifcapi" / "python" / "ifcopenshell" / "_generated_capi.py"

    fresh_c_signatures = _c_function_signatures(fresh_header, repo_root)
    checked_c_signatures = _c_function_signatures(checked_header, repo_root)
    fresh_python_symbols = set(parse_ctypes_symbols(fresh_python, repo_root))
    checked_python_symbols = set(parse_ctypes_symbols(checked_python, repo_root))

    assert fresh_c_signatures == checked_c_signatures
    assert _c_type_names(fresh_header) == _c_type_names(checked_header)
    assert _internal_struct_names(fresh_internal) == _internal_struct_names(checked_internal)
    assert fresh_python_symbols == checked_python_symbols
    assert _python_generated_types(fresh_python) == _python_generated_types(checked_python)
    assert set(fresh_c_signatures) <= fresh_python_symbols
    assert _cpp_helper_bodies(fresh_cpp) == _cpp_helper_bodies(checked_cpp)
