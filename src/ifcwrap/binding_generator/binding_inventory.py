# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
import ast
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any


_C_FUNCTION_RE = re.compile(
    r"""
    (?:^|\n)\s*
    (?!\#)
    (?:IFCAPI_EXPORT\s+)?
    (?P<return_type>(?:struct\s+)?[A-Za-z_][A-Za-z0-9_:\s\*]*?)
    \s+
    (?P<name>ifcopenshell_[A-Za-z0-9_]+)
    \s*\(
        (?P<params>[^;{}]*)
    \)\s*;
    """,
    re.VERBOSE | re.MULTILINE,
)


@dataclass(frozen=True)
class CFunction:
    name: str
    source: str
    return_type: str
    params: str


def _repo_relative(path: Path, repo_root: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


def _strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    return re.sub(r"//.*", "", text)


def parse_c_functions(path: Path, repo_root: Path | None = None) -> list[CFunction]:
    root = repo_root or path.parent
    text = _strip_comments(path.read_text(encoding="utf-8"))
    source = _repo_relative(path, root)
    functions: list[CFunction] = []
    for match in _C_FUNCTION_RE.finditer(text):
        return_type = " ".join(match.group("return_type").split())
        params = " ".join(match.group("params").split())
        functions.append(
            CFunction(
                name=match.group("name"),
                source=source,
                return_type=return_type,
                params=params,
            )
        )
    return functions


def parse_ctypes_symbols(path: Path, repo_root: Path | None = None) -> dict[str, list[str]]:
    root = repo_root or path.parent
    source = _repo_relative(path, root)
    try:
        tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
    except SyntaxError:
        return {}

    symbols: dict[str, set[str]] = {}

    def add(name: str) -> None:
        if name.startswith("ifcopenshell_"):
            symbols.setdefault(name, set()).add(source)

    for node in ast.walk(tree):
        if isinstance(node, ast.Dict):
            for key in node.keys:
                if isinstance(key, ast.Constant) and isinstance(key.value, str):
                    add(key.value)
            continue

        if isinstance(node, ast.Assign):
            for target in node.targets:
                if not isinstance(target, ast.Attribute) or target.attr not in {"argtypes", "restype"}:
                    continue
                value = target.value
                if isinstance(value, ast.Attribute):
                    add(value.attr)
            continue

        if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id == "getattr":
            if len(node.args) >= 2 and isinstance(node.args[1], ast.Constant) and isinstance(node.args[1].value, str):
                add(node.args[1].value)

    return {name: sorted(paths) for name, paths in sorted(symbols.items())}


def _generated_slice(name: str) -> str:
    if name.startswith("ifcopenshell_ifcgeom_"):
        return "ifcgeom"
    if name.startswith("ifcopenshell_ifcapi_"):
        return "ifcapi"
    if name.startswith("ifcopenshell_ifcparse_"):
        return "ifcparse"
    if name.startswith("ifcopenshell_ifc_"):
        return "ifcparse_handle"
    if name in {"ifcopenshell_clear_error", "ifcopenshell_last_error_message"}:
        return "common_error"
    if name.startswith(("ifcopenshell_string", "ifcopenshell_bool_list", "ifcopenshell_int", "ifcopenshell_uint", "ifcopenshell_double")):
        return "common_value"
    return "common_or_other"


def _concept_key(name: str) -> str:
    key = name.removeprefix("ifcopenshell_")
    for prefix in ("ifcparse_", "ifcgeom_"):
        key = key.removeprefix(prefix)
    key = key.removeprefix("ifc_")
    return key


def _domain_key(name: str) -> str | None:
    key = _concept_key(name)
    if key.startswith("file_"):
        return "file"
    if key.startswith(("instance_", "entity_")):
        return "entity"
    if key.startswith(("schema_", "declaration_", "attribute_", "inverse_attribute_", "parameter_type_")):
        return "schema"
    if key.startswith(("guid_", "value_", "string_", "bool_list", "int", "uint", "double")):
        return "value"
    return None


def _function_map(functions: list[CFunction]) -> dict[str, dict[str, str]]:
    return {
        function.name: {
            "source": function.source,
            "return_type": function.return_type,
            "params": function.params,
        }
        for function in sorted(functions, key=lambda item: item.name)
    }


def _is_allowed_manual_generated_overlap(source: str, name: str) -> bool:
    """Classify intentional manual ctypes declarations for generated C symbols."""
    if source == "src/ifcapi/python/ifcopenshell/geom/_capi.py":
        return name.startswith("ifcopenshell_ifcgeom_") or name in {
            "ifcopenshell_double_list_destroy",
            "ifcopenshell_ifcparse_instance_list_create_from_handles",
            "ifcopenshell_ifcparse_instance_list_destroy",
            "ifcopenshell_ifcparse_instance_list_get",
            "ifcopenshell_ifcparse_instance_list_size",
            "ifcopenshell_int32_list_destroy",
            "ifcopenshell_int32_list_list_destroy",
            "ifcopenshell_int32_list_list_list_destroy",
            "ifcopenshell_string_destroy",
            "ifcopenshell_string_list_destroy",
        }
    if source == "src/ifcapi/python/ifcopenshell/geom/main.py":
        return name == "ifcopenshell_ifcparse_instance_list_create_from_handles"
    if source == "src/ifcapi/python/ifcopenshell/__init__.py":
        return name in {
            "ifcopenshell_clear_error",
            "ifcopenshell_ifc_file_by_id",
            "ifcopenshell_ifc_file_key_value_store_iter",
            "ifcopenshell_ifc_file_key_value_store_query",
            "ifcopenshell_ifc_file_storage_mode",
            "ifcopenshell_ifc_instance_destroy",
            "ifcopenshell_ifc_instance_id",
            "ifcopenshell_ifc_instance_streamer_destroy",
            "ifcopenshell_ifc_instance_streamer_has_semicolon",
            "ifcopenshell_ifc_instance_streamer_push_page",
            "ifcopenshell_ifc_instance_streamer_read_instance_py",
            "ifcopenshell_last_error_message",
        }
    if source == "src/ifcapi/python/ifcopenshell/entity_instance.py":
        return name in {
            "ifcopenshell_double_list_destroy",
            "ifcopenshell_double_list_list_destroy",
            "ifcopenshell_ifc_instance_get_argument",
            "ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance",
            "ifcopenshell_ifcparse_attribute_value_as_bool",
            "ifcopenshell_ifcparse_attribute_value_as_double",
            "ifcopenshell_ifcparse_attribute_value_as_double_list",
            "ifcopenshell_ifcparse_attribute_value_as_double_list_list",
            "ifcopenshell_ifcparse_attribute_value_as_enumeration_value",
            "ifcopenshell_ifcparse_attribute_value_as_instance",
            "ifcopenshell_ifcparse_attribute_value_as_instance_list",
            "ifcopenshell_ifcparse_attribute_value_as_int32",
            "ifcopenshell_ifcparse_attribute_value_as_int32_list",
            "ifcopenshell_ifcparse_attribute_value_as_int32_list_list",
            "ifcopenshell_ifcparse_attribute_value_as_string",
            "ifcopenshell_ifcparse_attribute_value_as_string_list",
            "ifcopenshell_ifcparse_attribute_value_destroy",
            "ifcopenshell_ifcparse_attribute_value_is_null",
            "ifcopenshell_ifcparse_attribute_value_type",
            "ifcopenshell_ifcparse_instance_list_destroy",
            "ifcopenshell_ifcparse_instance_list_get",
            "ifcopenshell_ifcparse_instance_list_size",
            "ifcopenshell_int32_list_destroy",
            "ifcopenshell_int32_list_list_destroy",
        }
    return False


def _collect_highlevel_headers(repo_root: Path) -> list[Path]:
    include_dir = repo_root / "src" / "ifcapi" / "include" / "ifcapi"
    if not include_dir.exists():
        return []
    return sorted(include_dir.rglob("*.h"))


def _collect_python_sources(repo_root: Path) -> list[Path]:
    python_dir = repo_root / "src" / "ifcapi" / "python" / "ifcopenshell"
    if not python_dir.exists():
        return []
    return sorted(python_dir.rglob("*.py"))


def _is_generated_python_binding(path: Path) -> bool:
    try:
        head = path.read_text(encoding="utf-8").splitlines()[:3]
    except UnicodeDecodeError:
        return False
    return any("generated by IfcOpenShell's binding generator" in line for line in head)


def build_inventory(repo_root: Path) -> dict[str, Any]:
    generated_header = repo_root / "src" / "ifcwrap" / "binding_generator" / "generated" / "ifcopenshell_api.h"
    generated = parse_c_functions(generated_header, repo_root) if generated_header.exists() else []

    highlevel: list[CFunction] = []
    for header in _collect_highlevel_headers(repo_root):
        highlevel.extend(parse_c_functions(header, repo_root))

    manual_ctypes_symbols: dict[str, list[str]] = {}
    generated_ctypes_symbols: dict[str, list[str]] = {}
    generated_python_sources: list[Path] = []
    manual_python_sources: list[Path] = []
    for source in _collect_python_sources(repo_root):
        target = generated_ctypes_symbols if _is_generated_python_binding(source) else manual_ctypes_symbols
        if target is generated_ctypes_symbols:
            generated_python_sources.append(source)
        else:
            manual_python_sources.append(source)
        for name, paths in parse_ctypes_symbols(source, repo_root).items():
            target.setdefault(name, [])
            target[name].extend(paths)
    manual_ctypes_symbols = {name: sorted(set(paths)) for name, paths in sorted(manual_ctypes_symbols.items())}
    generated_ctypes_symbols = {name: sorted(set(paths)) for name, paths in sorted(generated_ctypes_symbols.items())}

    generated_names = {function.name for function in generated}
    generated_highlevel_names = {function.name for function in generated if _generated_slice(function.name) == "ifcapi"}
    highlevel_names = {function.name for function in highlevel}
    exported_names = generated_names | highlevel_names
    manual_ctypes_names = set(manual_ctypes_symbols)
    generated_ctypes_names = set(generated_ctypes_symbols)

    allowed_manual_generated_overlap: list[dict[str, str]] = []
    unexpected_manual_generated_overlap: list[dict[str, str]] = []
    for name in sorted(manual_ctypes_names & generated_names):
        for source in manual_ctypes_symbols[name]:
            item = {"name": name, "source": source}
            if _is_allowed_manual_generated_overlap(source, name):
                allowed_manual_generated_overlap.append(item)
            else:
                unexpected_manual_generated_overlap.append(item)

    generated_by_concept: dict[str, list[str]] = {}
    for name in generated_names:
        generated_by_concept.setdefault(_concept_key(name), []).append(name)

    potential_duplicates: list[dict[str, Any]] = []
    for name in sorted(highlevel_names):
        concept = _concept_key(name)
        exact = sorted(generated_by_concept.get(concept, []))
        domain = _domain_key(name)
        related = sorted(
            generated_name
            for generated_name in generated_names
            if domain is not None and _domain_key(generated_name) == domain
        )
        if exact or related:
            potential_duplicates.append(
                {
                    "highlevel": name,
                    "concept": concept,
                    "exact_core_matches": exact,
                    "related_core_domain": domain,
                    "related_core_symbols": related[:25],
                    "related_core_symbol_count": len(related),
                }
            )

    return {
        "generated_c": {
            "header": _repo_relative(generated_header, repo_root),
            "symbol_count": len(generated_names),
            "symbols_by_slice": {
                slice_name: sorted(name for name in generated_names if _generated_slice(name) == slice_name)
                for slice_name in sorted({_generated_slice(name) for name in generated_names})
            },
            "symbols": _function_map(generated),
        },
        "generated_highlevel_c": {
            "symbol_count": len(generated_highlevel_names),
            "symbols": {
                name: data
                for name, data in _function_map(generated).items()
                if name in generated_highlevel_names
            },
        },
        "handwritten_highlevel_c": {
            "header_count": len(_collect_highlevel_headers(repo_root)),
            "symbol_count": len(highlevel_names),
            "symbols": _function_map(highlevel),
        },
        "generated_python_ctypes": {
            "source_count": len(generated_python_sources),
            "symbol_count": len(generated_ctypes_names),
            "symbols": generated_ctypes_symbols,
            "missing_from_c_headers": sorted(generated_ctypes_names - exported_names),
            "missing_generated_c_symbols": sorted(generated_names - generated_ctypes_names),
            "declared_in_c_headers": sorted(generated_ctypes_names & exported_names),
        },
        "manual_python_ctypes": {
            "source_count": len(manual_python_sources),
            "symbol_count": len(manual_ctypes_names),
            "symbols": manual_ctypes_symbols,
            "missing_from_c_headers": sorted(manual_ctypes_names - exported_names),
            "declared_in_c_headers": sorted(manual_ctypes_names & exported_names),
            "allowed_generated_c_redeclarations": allowed_manual_generated_overlap,
            "unexpected_generated_c_redeclarations": unexpected_manual_generated_overlap,
        },
        "duplication": {
            "potential_core_highlevel": potential_duplicates,
            "potential_core_highlevel_count": len(potential_duplicates),
            "exact_symbol_collisions": sorted(generated_names & highlevel_names),
        },
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Inventory generated and handwritten IfcOpenShell binding surfaces.")
    parser.add_argument("--repo-root", type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument("--output", type=Path)
    args = parser.parse_args(argv)

    inventory = build_inventory(args.repo_root.resolve())
    text = json.dumps(inventory, indent=2, sort_keys=True) + "\n"
    if args.output is None:
        print(text, end="")
    else:
        args.output.write_text(text, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
