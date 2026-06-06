# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from pathlib import Path

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
        ["ifcopenshell_ifcparse", "ifcopenshell_ifcapi", "ifcopenshell_ifcgeom"],
        ["ifcopenshell_ifc", "", ""],
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
):
    spec_paths, cpp_spec_paths, cpp_spec_namespace, cpp_spec_c_prefix, cpp_spec_handle_c_prefix = (
        default_project_specs(spec_dir)
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
    )
    for artifact in artifacts.artifacts:
        print(f"{artifact.kind}:{artifact.path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
