# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
import re
from typing import Any

import yaml


SHARED_PTR_RE = re.compile(r"^\s*%shared_ptr\((?P<type>[^)]+)\);", re.MULTILINE)
INCLUDE_RE = re.compile(r"^\s*%include\s+\"(?P<path>[^\"]+)\"", re.MULTILINE)
EXTEND_RE = re.compile(r"^\s*%extend\s+(?P<target>[^\s{]+)\s*\{", re.MULTILINE)
TYPEMAP_RE = re.compile(r"^\s*%typemap\((?P<direction>[^)]+)\)\s+(?P<type>[^\{]+)\{", re.MULTILINE)
VECTOR_TYPEMAP_RE = re.compile(r"CREATE_VECTOR_TYPEMAP_OUT\((?P<type>[^)]+)\)")
VECTOR_OF_ITEM_RE = re.compile(r"vector_of_item\((?P<type>[^)]+)\)")


@dataclass(frozen=True)
class Residual:
    spec: str
    path: str
    category: str
    reason: str
    inference_source: str
    next_step: str

    def as_dict(self) -> dict[str, str]:
        return {
            "spec": self.spec,
            "path": self.path,
            "category": self.category,
            "reason": self.reason,
            "inference_source": self.inference_source,
            "next_step": self.next_step,
        }


def _load_yaml(path: Path) -> dict[str, Any]:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        msg = f"{path} must contain a YAML mapping"
        raise ValueError(msg)
    return data


def _count_manual_entries(entries: list[Any], *, spec_name: str, section: str) -> list[Residual]:
    residuals: list[Residual] = []
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            continue
        implementation = entry.get("implementation")
        kind = entry.get("kind", "function")
        cpp_name = str(entry.get("cpp_name") or entry.get("name") or entry.get("expose_as") or f"{section}[{index}]")
        if implementation == "inline":
            reason = "true_adapter_or_abi_shape"
            next_step = "Keep unless a source-derived adapter pattern can prove identical generated behavior."
        elif kind == "constructor":
            reason = "stable_c_abi_factory"
            next_step = "Revisit after parameter-level pointer/reference passing inference is available."
        else:
            reason = "manual_public_entry"
            next_step = "Audit against C++ discovery and migrate only if the generated signature remains stable."
        residuals.append(
            Residual(
                spec=spec_name,
                path=f"{section}[{index}]:{cpp_name}",
                category=f"manual_{section.rstrip('s')}",
                reason=reason,
                inference_source="yaml_only",
                next_step=next_step,
            )
        )
    return residuals


def _is_param_kind_policy_override(override: Any) -> bool:
    if not isinstance(override, dict):
        return False
    if override.get("returns") is not None:
        return False
    params = override.get("params")
    if not isinstance(params, dict) or not params:
        return False
    return all(isinstance(param, dict) and isinstance(param.get("kind"), str) for param in params.values())


def _class_residuals(spec_name: str, classes: list[Any]) -> list[Residual]:
    residuals: list[Residual] = []
    for index, item in enumerate(classes):
        if not isinstance(item, dict):
            continue
        handle = str(item.get("handle", f"classes[{index}]"))
        base_path = f"discover.classes[{index}]({handle})"
        if item.get("type_overrides"):
            for member_name, override in sorted(item["type_overrides"].items()):
                category = "type_override"
                reason = "cxx_inference_gap"
                inference_source = "cxx_ast_candidate"
                next_step = "Infer enum/result/opaque/list return kinds or make overrides overload-scoped where needed."
                if _is_param_kind_policy_override(override):
                    category = "abi_type_policy"
                    reason = "stable_c_abi_policy"
                    inference_source = "yaml_only"
                    next_step = "Keep as explicit ABI policy unless the public C type is deliberately changed."
                residuals.append(
                    Residual(
                        spec=spec_name,
                        path=f"{base_path}.type_overrides[{member_name}]",
                        category=category,
                        reason=reason,
                        inference_source=inference_source,
                        next_step=next_step,
                    )
                )
        for key, category, reason, source, next_step in (
            (
                "discover_children",
                "taxonomy_children_accessor",
                "stable_c_abi_policy",
                "yaml_only",
                "Keep as explicit child-accessor selection policy; child element handles and accessor names are inferred where source-visible.",
            ),
            (
                "ccomponents_accessor",
                "taxonomy_components_accessor",
                "stable_c_abi_policy",
                "yaml_only",
                "Keep as explicit component-accessor selection policy; Eigen dimensions are inferred where source-visible.",
            ),
            (
                "array_pair_fields",
                "array_pair_accessor",
                "stable_c_abi_policy",
                "yaml_only",
                "Keep as explicit split-accessor selection policy; std::array<T,2> element return kinds are inferred.",
            ),
            (
                "variant_accessors",
                "variant_accessor",
                "stable_c_abi_policy",
                "yaml_only",
                "Keep variant accessor suffixes and enum grouping explicit ABI policy; variant alternative type lowering is inferred centrally.",
            ),
            (
                "method_sizes",
                "size_accessor",
                "stable_c_abi_policy",
                "cxx_ast_candidate",
                "Keep as policy unless the generator can infer C count accessors for returned containers safely.",
            ),
            (
                "enum_types_as_int32",
                "enum_return_policy",
                "cxx_inference_gap",
                "cxx_ast_candidate",
                "Infer enum-like return/field types as stable int32 ABI where safe.",
            ),
            (
                "overloads",
                "overload_selection",
                "stable_c_abi_policy",
                "cxx_ast_candidate",
                "Keep selection/rename policy, but make type overrides canonical-overload scoped.",
            ),
        ):
            value = item.get(key)
            if value:
                residuals.append(
                    Residual(
                        spec=spec_name,
                        path=f"{base_path}.{key}",
                        category=category,
                        reason=reason,
                        inference_source=source,
                        next_step=next_step,
                    )
                )
    return residuals


def _function_discovery_residuals(spec_name: str, functions: list[Any]) -> list[Residual]:
    residuals: list[Residual] = []
    for index, item in enumerate(functions):
        if not isinstance(item, dict):
            continue
        namespace = str(item.get("namespace", f"functions[{index}]"))
        base_path = f"discover.functions[{index}]({namespace})"
        if item.get("type_overrides"):
            for function_name in sorted(item["type_overrides"]):
                residuals.append(
                    Residual(
                        spec=spec_name,
                        path=f"{base_path}.type_overrides[{function_name}]",
                        category="type_override",
                        reason="cxx_inference_gap",
                        inference_source="cxx_ast_candidate",
                        next_step="Infer the return/parameter type from the discovered C++ signature, or make the override canonical-overload scoped.",
                    )
                )
        if item.get("overloads"):
            residuals.append(
                Residual(
                    spec=spec_name,
                    path=f"{base_path}.overloads",
                    category="overload_selection",
                    reason="stable_c_abi_policy",
                    inference_source="cxx_ast_candidate",
                    next_step="Preserve explicit exported overload selection while making per-overload policy keys canonical.",
                )
            )
    return residuals


def _handle_family_residuals(spec_name: str, families: list[Any]) -> list[Residual]:
    residuals: list[Residual] = []
    for index, family in enumerate(families):
        if not isinstance(family, dict):
            continue
        namespace = str(family.get("namespace", f"handle_families[{index}]"))
        types = family.get("types", [])
        type_count = len(types) if isinstance(types, list) else 0
        residuals.append(
            Residual(
                spec=spec_name,
                path=f"handle_families[{index}]({namespace}, {type_count} types)",
                category="shared_ptr_handle_family",
                reason="stable_c_abi_policy",
                inference_source="yaml_only",
                next_step="Keep exported handle selection and shared_ptr lifetime policy explicit; source validation only proves the selected types exist.",
            )
        )
    return residuals


def _spec_residuals(spec_path: Path) -> list[Residual]:
    spec = _load_yaml(spec_path)
    spec_name = spec_path.name
    residuals: list[Residual] = []
    residuals.extend(_handle_family_residuals(spec_name, spec.get("handle_families", [])))

    discover = spec.get("discover", {})
    if isinstance(discover, dict):
        residuals.extend(_class_residuals(spec_name, discover.get("classes", [])))
        residuals.extend(_function_discovery_residuals(spec_name, discover.get("functions", [])))

    residuals.extend(_count_manual_entries(spec.get("functions", []), spec_name=spec_name, section="functions"))
    residuals.extend(_count_manual_entries(spec.get("methods", []), spec_name=spec_name, section="methods"))
    return residuals


def _swig_patterns(wrapper_paths: list[Path]) -> dict[str, Any]:
    shared_ptr_types: set[str] = set()
    include_paths: set[str] = set()
    extend_targets: set[str] = set()
    typemaps: set[str] = set()
    vector_typemap_types: set[str] = set()
    vector_item_types: set[str] = set()

    for path in wrapper_paths:
        text = path.read_text(encoding="utf-8")
        shared_ptr_types.update(match.group("type").strip() for match in SHARED_PTR_RE.finditer(text))
        include_paths.update(match.group("path") for match in INCLUDE_RE.finditer(text))
        extend_targets.update(match.group("target") for match in EXTEND_RE.finditer(text))
        typemaps.update(" ".join(match.group("type").split()) for match in TYPEMAP_RE.finditer(text))
        vector_typemap_types.update(match.group("type").strip() for match in VECTOR_TYPEMAP_RE.finditer(text))
        vector_item_types.update(match.group("type").strip() for match in VECTOR_OF_ITEM_RE.finditer(text))

    return {
        "shared_ptr_types": sorted(shared_ptr_types),
        "includes": sorted(include_paths),
        "extend_targets": sorted(extend_targets),
        "typemap_types": sorted(typemaps),
        "vector_typemap_types": sorted(vector_typemap_types),
        "vector_item_types": sorted(vector_item_types),
        "counts": {
            "shared_ptr_types": len(shared_ptr_types),
            "includes": len(include_paths),
            "extend_targets": len(extend_targets),
            "typemap_types": len(typemaps),
            "vector_typemap_types": len(vector_typemap_types),
            "vector_item_types": len(vector_item_types),
        },
    }


def audit_binding_residuals(*, spec_paths: list[Path], swig_paths: list[Path]) -> dict[str, Any]:
    residuals: list[Residual] = []
    for spec_path in spec_paths:
        residuals.extend(_spec_residuals(spec_path))

    reason_counts = Counter(residual.reason for residual in residuals)
    category_counts = Counter(residual.category for residual in residuals)
    source_counts = Counter(residual.inference_source for residual in residuals)
    redundant_candidates = [
        residual.as_dict()
        for residual in residuals
        if residual.reason in {"cxx_inference_gap", "swig_typemap_pattern", "swig_extend_pattern", "swig_policy_derived", "typemap_shape_gap"}
    ]

    return {
        "summary": {
            "residual_count": len(residuals),
            "redundant_candidate_count": len(redundant_candidates),
            "category_counts": dict(sorted(category_counts.items())),
            "reason_counts": dict(sorted(reason_counts.items())),
            "inference_source_counts": dict(sorted(source_counts.items())),
        },
        "swig_patterns": _swig_patterns(swig_paths),
        "residuals": [residual.as_dict() for residual in sorted(residuals, key=lambda item: (item.spec, item.category, item.path))],
        "redundant_candidates": sorted(redundant_candidates, key=lambda item: (item["spec"], item["category"], item["path"])),
    }


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Audit YAML residual binding policy against SWIG and C++ discovery categories.")
    parser.add_argument("--spec", type=Path, action="append", required=True, help="Binding YAML spec to audit")
    parser.add_argument("--swig", type=Path, action="append", required=True, help="SWIG .i wrapper/typemap file to compare")
    return parser


def main() -> int:
    import json

    args = _build_parser().parse_args()
    report = audit_binding_residuals(spec_paths=args.spec, swig_paths=args.swig)
    print(json.dumps(report, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
