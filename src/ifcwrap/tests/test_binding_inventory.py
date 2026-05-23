# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import ast
from pathlib import Path
import re
from textwrap import dedent

import pytest
import yaml

from src.ifcwrap.binding_generator.binding_inventory import build_inventory, parse_c_functions, parse_ctypes_symbols
from src.ifcwrap.binding_generator.c_backend import generate_merged
from src.ifcwrap.binding_generator.contract_discovery import discover_marked_functions_in_headers


_C_TYPEDEF_RE = re.compile(r"\btypedef\s+(?:struct|enum)\s+(ifcopenshell_[A-Za-z0-9_]+_t)\b")
_C_STRUCT_BLOCK_RE = re.compile(
    r"\btypedef\s+struct\s+(?P<name>ifcopenshell_[A-Za-z0-9_]+_t)\s*\{(?P<body>.*?)\}\s*(?P=name)\s*;",
    re.DOTALL,
)
_C_ENUM_BLOCK_RE = re.compile(
    r"\btypedef\s+enum\s+(?P<name>ifcopenshell_[A-Za-z0-9_]+_t)\s*\{(?P<body>.*?)\}\s*(?P=name)\s*;",
    re.DOTALL,
)
_INTERNAL_STRUCT_RE = re.compile(r"^\s*struct\s+(ifcopenshell_[A-Za-z0-9_]+_t)\s*\{", re.MULTILINE)
_INTERNAL_STRUCT_BLOCK_RE = re.compile(
    r"^\s*struct\s+(?P<name>ifcopenshell_[A-Za-z0-9_]+_t)\s*\{(?P<body>.*?)^\s*\};",
    re.DOTALL | re.MULTILINE,
)
_HELPER_BODY_RE = re.compile(
    r"\b(?:static\s+)?[\w:<>,\s\*&]+?\s+"
    r"(?P<name>(?:make|to_cpp)_[A-Za-z0-9_]+|ifcopenshell_[A-Za-z0-9_]+_destroy)\s*\(",
    re.MULTILINE,
)
_SCALAR_PARAM_KINDS = {"bool", "double", "int32", "logical", "size", "string", "uint32"}


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def _resolve_ifcapi_public_header(repo_root: Path, header: str) -> Path:
    for include_root in (repo_root / "src/ifcapi/include", repo_root / "src/ifcapi/src"):
        candidate = include_root / header
        if candidate.exists():
            return candidate
    raise FileNotFoundError(header)


def _c_function_signatures(path: Path, repo_root: Path) -> dict[str, tuple[str, str]]:
    return {function.name: (function.return_type, function.params) for function in parse_c_functions(path, repo_root)}


def _c_type_names(path: Path) -> set[str]:
    return set(_C_TYPEDEF_RE.findall(path.read_text(encoding="utf-8")))


def _field_declarations(body: str) -> tuple[str, ...]:
    fields: list[str] = []
    for raw_line in body.splitlines():
        line = raw_line.strip()
        if not line or line.startswith("//"):
            continue
        if line.endswith(";"):
            fields.append(" ".join(line[:-1].split()))
    return tuple(fields)


def _c_struct_layouts(path: Path) -> dict[str, tuple[str, ...]]:
    text = path.read_text(encoding="utf-8")
    return {
        match.group("name"): _field_declarations(match.group("body"))
        for match in _C_STRUCT_BLOCK_RE.finditer(text)
    }


def _c_enum_ordinals(path: Path) -> dict[str, tuple[tuple[str, str], ...]]:
    text = path.read_text(encoding="utf-8")
    enums: dict[str, tuple[tuple[str, str], ...]] = {}
    for match in _C_ENUM_BLOCK_RE.finditer(text):
        values: list[tuple[str, str]] = []
        for raw_entry in match.group("body").split(","):
            entry = " ".join(raw_entry.split())
            if not entry:
                continue
            name, _, value = entry.partition("=")
            values.append((name.strip(), value.strip()))
        enums[match.group("name")] = tuple(values)
    return enums


def _internal_struct_names(path: Path) -> set[str]:
    return set(_INTERNAL_STRUCT_RE.findall(path.read_text(encoding="utf-8")))


def _internal_struct_layouts(path: Path) -> dict[str, tuple[str, ...]]:
    text = path.read_text(encoding="utf-8")
    return {
        match.group("name"): _field_declarations(match.group("body"))
        for match in _INTERNAL_STRUCT_BLOCK_RE.finditer(text)
    }


def _node_source(node: ast.AST) -> str:
    return " ".join(ast.unparse(node).split())


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


def _python_structure_fields(path: Path) -> dict[str, tuple[tuple[str, str], ...]]:
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    fields: dict[str, tuple[tuple[str, str], ...]] = {}
    for node in tree.body:
        if not isinstance(node, ast.ClassDef):
            continue
        for statement in node.body:
            if not isinstance(statement, ast.Assign):
                continue
            if not any(isinstance(target, ast.Name) and target.id == "_fields_" for target in statement.targets):
                continue
            if not isinstance(statement.value, ast.List):
                continue
            class_fields: list[tuple[str, str]] = []
            for element in statement.value.elts:
                if not isinstance(element, ast.Tuple) or len(element.elts) < 2:
                    continue
                field_name = ast.literal_eval(element.elts[0])
                class_fields.append((field_name, _node_source(element.elts[1])))
            fields[node.name] = tuple(class_fields)
    return fields


def _python_signature_map(path: Path) -> dict[str, str]:
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    for node in tree.body:
        if not isinstance(node, ast.Assign):
            continue
        if not any(isinstance(target, ast.Name) and target.id == "FUNCTION_SIGNATURES" for target in node.targets):
            continue
        if not isinstance(node.value, ast.Dict):
            msg = "FUNCTION_SIGNATURES must be a dict literal"
            raise AssertionError(msg)
        signatures: dict[str, str] = {}
        for key_node, value_node in zip(node.value.keys, node.value.values, strict=True):
            key = ast.literal_eval(key_node)
            signatures[key] = _node_source(value_node)
        return signatures
    msg = f"{path} does not define FUNCTION_SIGNATURES"
    raise AssertionError(msg)


def _python_generated_constants(path: Path) -> dict[str, int]:
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    constants: dict[str, int] = {}
    for node in tree.body:
        if not isinstance(node, ast.Assign) or len(node.targets) != 1:
            continue
        target = node.targets[0]
        if not isinstance(target, ast.Name) or not target.id.startswith("IFCOPENSHELL_"):
            continue
        constants[target.id] = ast.literal_eval(node.value)
    return constants


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


def test_ifcapi_contract_discovery_uses_marked_public_headers() -> None:
    repo_root = _repo_root()
    spec_path = repo_root / "src/ifcwrap/binding_generator/specs/ifcapi.yml"
    spec = yaml.safe_load(spec_path.read_text(encoding="utf-8"))
    headers = [
        _resolve_ifcapi_public_header(repo_root, header)
        for header in spec["public_headers"]
        if header.startswith("ifcapi/bindings/")
    ]

    marked_names = {function.name for function in discover_marked_functions_in_headers(headers)}
    function_blocks = spec["discover"]["functions"]
    assert len(function_blocks) == 1
    assert function_blocks[0]["namespace"] == "ifcapi::bindings"
    assert "translation_unit" not in function_blocks[0]
    assert "include" not in function_blocks[0]
    assert set(function_blocks[0].get("type_overrides", {})) <= marked_names


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


def test_scalar_param_specs_do_not_carry_ownership_policy() -> None:
    spec_dir = _repo_root() / "src/ifcwrap/binding_generator/specs"
    offenders: list[str] = []

    for spec_path in sorted(spec_dir.glob("*.yml")):
        spec = yaml.safe_load(spec_path.read_text(encoding="utf-8"))

        def visit(node: object, path: tuple[str, ...] = ()) -> None:
            if isinstance(node, dict):
                params = node.get("params")
                if isinstance(params, list):
                    for index, param in enumerate(params):
                        param_type = param.get("type") if isinstance(param, dict) else None
                        if (
                            isinstance(param_type, dict)
                            and param_type.get("kind") in _SCALAR_PARAM_KINDS
                            and "ownership" in param_type
                        ):
                            offenders.append(f"{spec_path.name}:{'.'.join(path + ('params', str(index), 'type'))}")
                for key, value in node.items():
                    visit(value, path + (str(key),))
            elif isinstance(node, list):
                for index, value in enumerate(node):
                    visit(value, path + (str(index),))

        visit(spec)

    assert offenders == []


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
    assert _c_struct_layouts(fresh_header) == _c_struct_layouts(checked_header)
    assert _c_enum_ordinals(fresh_header) == _c_enum_ordinals(checked_header)
    assert _internal_struct_names(fresh_internal) == _internal_struct_names(checked_internal)
    assert _internal_struct_layouts(fresh_internal) == _internal_struct_layouts(checked_internal)
    assert fresh_python_symbols == checked_python_symbols
    assert _python_generated_types(fresh_python) == _python_generated_types(checked_python)
    assert _python_structure_fields(fresh_python) == _python_structure_fields(checked_python)
    assert _python_signature_map(fresh_python) == _python_signature_map(checked_python)
    assert _python_generated_constants(fresh_python) == _python_generated_constants(checked_python)
    assert set(fresh_c_signatures) <= fresh_python_symbols
    assert _cpp_helper_bodies(fresh_cpp) == _cpp_helper_bodies(checked_cpp)
