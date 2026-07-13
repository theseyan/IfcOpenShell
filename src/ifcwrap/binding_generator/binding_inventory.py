# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any

_C_FUNCTION_RE = re.compile(
    r"""
    (?:^|\n)\s*
    (?!\#)
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


def _generated_slice(name: str) -> str:
    if name.startswith("ifcopenshell_geom_"):
        return "ifcgeom"
    if name.startswith("ifcopenshell_parse_"):
        return "ifcparse"
    if name.startswith(
        ("ifcopenshell_file_", "ifcopenshell_instance_", "ifcopenshell_schema_")
    ):
        return "ifcparse_handle"
    if name.startswith(
        (
            "ifcopenshell_aggregate_",
            "ifcopenshell_attribute_",
            "ifcopenshell_boundary_",
            "ifcopenshell_classification_",
            "ifcopenshell_context_",
            "ifcopenshell_cost_",
            "ifcopenshell_element_",
            "ifcopenshell_geometry_",
            "ifcopenshell_pset_",
            "ifcopenshell_root_",
            "ifcopenshell_selector_",
            "ifcopenshell_shape_builder_",
            "ifcopenshell_unit_",
            "ifcopenshell_value_",
        )
    ):
        return "ifcapi"
    if name in {
        "ifcopenshell_clear_error",
        "ifcopenshell_last_error_message",
        "ifcopenshell_last_error_kind",
    }:
        return "common_error"
    if name.startswith(
        (
            "ifcopenshell_string",
            "ifcopenshell_bool_list",
            "ifcopenshell_int",
            "ifcopenshell_uint",
            "ifcopenshell_double",
        )
    ):
        return "common_value"
    return "common_or_other"


def _concept_key(name: str) -> str:
    key = name.removeprefix("ifcopenshell_")
    for prefix in ("ifcparse_", "ifcgeom_", "parse_", "geom_"):
        key = key.removeprefix(prefix)
    key = key.removeprefix("ifc_")
    return key


def _domain_key(name: str) -> str | None:
    key = _concept_key(name)
    if key.startswith("file_"):
        return "file"
    if key.startswith(("instance_", "entity_")):
        return "entity"
    if key.startswith(
        (
            "schema_",
            "declaration_",
            "attribute_",
            "inverse_attribute_",
            "parameter_type_",
        )
    ):
        return "schema"
    if key.startswith(
        ("guid_", "value_", "string_", "bool_list", "int", "uint", "double")
    ):
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


def _collect_highlevel_headers(repo_root: Path) -> list[Path]:
    include_dir = repo_root / "src" / "ifcapi" / "include" / "ifcapi"
    if not include_dir.exists():
        return []
    return sorted(include_dir.rglob("*.h"))


def build_inventory(repo_root: Path) -> dict[str, Any]:
    generated_header = (
        repo_root
        / "src"
        / "ifcwrap"
        / "binding_generator"
        / "generated"
        / "ifcopenshell_api.h"
    )
    generated = (
        parse_c_functions(generated_header, repo_root)
        if generated_header.exists()
        else []
    )

    highlevel: list[CFunction] = []
    for header in _collect_highlevel_headers(repo_root):
        highlevel.extend(parse_c_functions(header, repo_root))

    generated_names = {function.name for function in generated}
    generated_highlevel_names = {
        function.name
        for function in generated
        if _generated_slice(function.name) == "ifcapi"
    }
    highlevel_names = {function.name for function in highlevel}
    exported_names = generated_names | highlevel_names

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
                slice_name: sorted(
                    name
                    for name in generated_names
                    if _generated_slice(name) == slice_name
                )
                for slice_name in sorted(
                    {_generated_slice(name) for name in generated_names}
                )
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
        "duplication": {
            "potential_core_highlevel": potential_duplicates,
            "potential_core_highlevel_count": len(potential_duplicates),
            "exact_symbol_collisions": sorted(generated_names & highlevel_names),
        },
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Inventory generated and handwritten IfcOpenShell binding surfaces."
    )
    parser.add_argument(
        "--repo-root", type=Path, default=Path(__file__).resolve().parents[3]
    )
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
