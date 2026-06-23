# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from pathlib import Path

_EMPTY_ARG = "__IFCOPENSHELL_EMPTY__"

try:
    from ..c_backend import build_binding_ir
    from ..host_metadata import build_host_metadata
    from . import TargetGenerationRequest, get_target
except ImportError:  # pragma: no cover - script execution fallback
    import sys

    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from c_backend import build_binding_ir
    from host_metadata import build_host_metadata
    from targets import TargetGenerationRequest, get_target


def default_project_specs(spec_dir: Path) -> tuple[list[Path], list[Path], list[str], list[str], list[str]]:
    return (
        [spec_dir / "ifcparse.yml", spec_dir / "ifcgeom.yml"],
        [
            spec_dir / "cpp" / "ifcparse.hpp",
            spec_dir / "cpp" / "ifcapi.hpp",
            spec_dir / "cpp" / "ifcgeom.hpp",
        ],
        ["ifcparse::bindings", "ifcapi::bindings", "ifcgeom::bindings"],
        ["ifcopenshell_parse", "ifcopenshell", "ifcopenshell_geom"],
        ["ifcopenshell", "ifcopenshell", "ifcopenshell_geom"],
    )


def _resolve_project_specs(
    spec_dir: Path,
    *,
    spec_paths: list[Path] | None = None,
    cpp_spec_paths: list[Path] | None = None,
    cpp_spec_namespace: list[str] | None = None,
    cpp_spec_c_prefix: list[str] | None = None,
    cpp_spec_handle_c_prefix: list[str] | None = None,
) -> tuple[list[Path], list[Path], list[str], list[str], list[str]]:
    if not any(
        value is not None
        for value in (spec_paths, cpp_spec_paths, cpp_spec_namespace, cpp_spec_c_prefix, cpp_spec_handle_c_prefix)
    ):
        return default_project_specs(spec_dir)

    if not all(
        value is not None
        for value in (spec_paths, cpp_spec_paths, cpp_spec_namespace, cpp_spec_c_prefix, cpp_spec_handle_c_prefix)
    ):
        raise ValueError(
            "When overriding specs, --spec, --cpp-spec, --cpp-spec-namespace, --cpp-spec-c-prefix, "
            "and --cpp-spec-handle-c-prefix must all be provided."
        )

    if not (
        len(cpp_spec_paths) == len(cpp_spec_namespace) == len(cpp_spec_c_prefix) == len(cpp_spec_handle_c_prefix)
    ):
        raise ValueError("Overridden C++ spec argument lists must be the same length.")

    return (
        list(spec_paths),
        list(cpp_spec_paths),
        list(cpp_spec_namespace),
        list(cpp_spec_c_prefix),
        ["" if value == _EMPTY_ARG else value for value in cpp_spec_handle_c_prefix],
    )


def _parse_target_options(values: list[str]) -> dict[str, str]:
    options: dict[str, str] = {}
    for value in values:
        key, separator, option_value = value.partition("=")
        if not separator or not key:
            msg = f"Invalid --target-option '{value}', expected key=value"
            raise ValueError(msg)
        options[key] = option_value
    return options


def _parse_bool_option(value: str | None) -> bool:
    if value is None:
        return False
    return value.strip().lower() in {"1", "true", "yes", "on"}


def generate_target(
    *,
    target_name: str,
    spec_dir: Path,
    api_header: Path,
    output_dir: Path,
    options: dict[str, str] | None = None,
    discovery_include_dirs: tuple[Path, ...] = (),
    discovery_defines: tuple[str, ...] = (),
    discovery_clang_args: tuple[str, ...] = (),
    module: str = "ifcopenshell",
    c_prefix: str = "ifcopenshell",
    spec_paths: list[Path] | None = None,
    cpp_spec_paths: list[Path] | None = None,
    cpp_spec_namespace: list[str] | None = None,
    cpp_spec_c_prefix: list[str] | None = None,
    cpp_spec_handle_c_prefix: list[str] | None = None,
):
    spec_paths, cpp_spec_paths, cpp_spec_namespace, cpp_spec_c_prefix, cpp_spec_handle_c_prefix = _resolve_project_specs(
        spec_dir,
        spec_paths=spec_paths,
        cpp_spec_paths=cpp_spec_paths,
        cpp_spec_namespace=cpp_spec_namespace,
        cpp_spec_c_prefix=cpp_spec_c_prefix,
        cpp_spec_handle_c_prefix=cpp_spec_handle_c_prefix,
    )
    ir = build_binding_ir(
        spec_paths,
        module=module,
        c_prefix=c_prefix,
        discovery_include_dirs=discovery_include_dirs,
        discovery_defines=discovery_defines,
        discovery_clang_args=discovery_clang_args,
        cpp_spec_paths=cpp_spec_paths,
        cpp_spec_namespace=cpp_spec_namespace,
        cpp_spec_c_prefix=cpp_spec_c_prefix,
        cpp_spec_handle_c_prefix=cpp_spec_handle_c_prefix,
    )
    request = TargetGenerationRequest(
        ir=ir,
        metadata=build_host_metadata(ir),
        api_header_path=api_header,
        options=dict(options or {}),
        output_dir=output_dir,
        exports_only=_parse_bool_option((options or {}).get("exports_only")),
    )
    return get_target(target_name).generate(request)


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate target language bindings from an existing C API header.")
    parser.add_argument("--target", required=True, help="Target backend name.")
    parser.add_argument(
        "--spec-dir",
        type=Path,
        required=True,
        help="Directory containing binding_generator spec files.",
    )
    parser.add_argument("--api-header", type=Path, required=True, help="Path to the generated C API header.")
    parser.add_argument("--output-dir", type=Path, required=True, help="Directory for generated target artifacts.")
    parser.add_argument(
        "--target-option",
        action="append",
        default=[],
        help="Target-specific option in key=value form.",
    )
    parser.add_argument("--discovery-include-dir", type=Path, action="append", default=[])
    parser.add_argument("--discovery-define", action="append", default=[])
    parser.add_argument("--discovery-clang-arg", action="append", default=[])
    parser.add_argument("--module", type=str, default="ifcopenshell")
    parser.add_argument("--c-prefix", type=str, default="ifcopenshell")
    parser.add_argument("--spec", type=Path, action="append", default=None)
    parser.add_argument("--cpp-spec", type=Path, action="append", default=None)
    parser.add_argument("--cpp-spec-namespace", action="append", default=None)
    parser.add_argument("--cpp-spec-c-prefix", action="append", default=None)
    parser.add_argument("--cpp-spec-handle-c-prefix", action="append", default=None)
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    artifacts = generate_target(
        target_name=args.target,
        spec_dir=args.spec_dir,
        api_header=args.api_header,
        output_dir=args.output_dir,
        options=_parse_target_options(args.target_option),
        discovery_include_dirs=tuple(args.discovery_include_dir),
        discovery_defines=tuple(args.discovery_define),
        discovery_clang_args=tuple(args.discovery_clang_arg),
        module=args.module,
        c_prefix=args.c_prefix,
        spec_paths=args.spec,
        cpp_spec_paths=args.cpp_spec,
        cpp_spec_namespace=args.cpp_spec_namespace,
        cpp_spec_c_prefix=args.cpp_spec_c_prefix,
        cpp_spec_handle_c_prefix=args.cpp_spec_handle_c_prefix,
    )
    for artifact in artifacts.artifacts:
        print(f"{artifact.kind}:{artifact.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
