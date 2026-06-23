# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from pathlib import Path

try:
    from .c_backend import generate_merged
    from .targets.generate import generate_target
except ImportError:  # pragma: no cover - script execution fallback
    from c_backend import generate_merged
    from targets.generate import generate_target


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate C ABI and target bindings.")
    parser.add_argument(
        "--spec-dir",
        type=Path,
        required=True,
        help="Directory containing binding_generator spec files.",
    )
    parser.add_argument("--output-dir", type=Path, required=True, help="Directory for generated C ABI files.")
    parser.add_argument(
        "--python-output-dir",
        type=Path,
        default=None,
        help="Directory for generated Python target files.",
    )
    parser.add_argument("--python-utils-out", type=Path, default=None, help="Optional explicit path for _capi_utils.py.")
    parser.add_argument("--discovery-include-dir", type=Path, action="append", default=[])
    parser.add_argument("--discovery-define", action="append", default=[])
    parser.add_argument("--discovery-clang-arg", action="append", default=[])
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    output_dir = args.output_dir.resolve()
    python_output_dir = (args.python_output_dir or output_dir).resolve()
    header_out, _, _ = generate_merged(
        [args.spec_dir / "ifcparse.yml", args.spec_dir / "ifcgeom.yml"],
        "ifcopenshell",
        "ifcopenshell",
        output_dir / "ifcopenshell_api.h",
        output_dir / "ifcopenshell_api.cpp",
        internal_header_out=output_dir / "ifcopenshell_api_internal.hpp",
        discovery_include_dirs=tuple(args.discovery_include_dir),
        discovery_defines=tuple(args.discovery_define),
        discovery_clang_args=tuple(args.discovery_clang_arg),
        cpp_spec_paths=[
            args.spec_dir / "cpp" / "ifcparse.hpp",
            args.spec_dir / "cpp" / "ifcapi.hpp",
            args.spec_dir / "cpp" / "ifcgeom.hpp",
        ],
        cpp_spec_namespace=["ifcparse::bindings", "ifcapi::bindings", "ifcgeom::bindings"],
        cpp_spec_c_prefix=["ifcopenshell_parse", "ifcopenshell", "ifcopenshell_geom"],
        cpp_spec_handle_c_prefix=["ifcopenshell", "ifcopenshell", "ifcopenshell_geom"],
    )
    options = {}
    if args.python_utils_out is not None:
        options["utils-out"] = str(args.python_utils_out.resolve())
    generate_target(
        target_name="python",
        spec_dir=args.spec_dir,
        api_header=header_out,
        output_dir=python_output_dir,
        options=options,
        discovery_include_dirs=tuple(args.discovery_include_dir),
        discovery_defines=tuple(args.discovery_define),
        discovery_clang_args=tuple(args.discovery_clang_arg),
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
