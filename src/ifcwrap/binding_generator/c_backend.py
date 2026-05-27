from __future__ import annotations

import argparse
from pathlib import Path
from typing import Union

try:
    from .authored_spec import AuthoredBindingSpec, MergedBindingSpec, load_authored_spec, load_merged_specs
    from .binding_ir import (
        BindingIR,
        lower_binding_spec,
    )
    from .c_call_rendering import _render_call_impl, _render_result_assignment
    from .c_sequence_helpers import (
        _render_common_type_impls,
        _render_handle_list_destroy_impl,
        _render_handle_list_helpers,
        _render_handle_list_list_destroy_impl,
        _render_handle_list_list_helpers,
        _render_sequence_helpers,
        _snake_name,
        _used_handle_list_handles,
        _used_scalar_sequence_kinds,
    )
    from .c_handle_rendering import _destroy_body, _handle_storage_type
    from .c_header_rendering import _render_header
    from .c_internal_header import _render_internal_header
    from .c_runtime_support import _render_cpp_support_runtime
    from .debug import debug_log, debug_path
    from .python_ctypes_backend import generate_python_ctypes
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, MergedBindingSpec, load_authored_spec, load_merged_specs
    from binding_ir import (
        BindingIR,
        lower_binding_spec,
    )
    from c_call_rendering import _render_call_impl, _render_result_assignment
    from c_sequence_helpers import (
        _render_common_type_impls,
        _render_handle_list_destroy_impl,
        _render_handle_list_helpers,
        _render_handle_list_list_destroy_impl,
        _render_handle_list_list_helpers,
        _render_sequence_helpers,
        _snake_name,
        _used_handle_list_handles,
        _used_scalar_sequence_kinds,
    )
    from c_handle_rendering import _destroy_body, _handle_storage_type
    from c_header_rendering import _render_header
    from c_internal_header import _render_internal_header
    from c_runtime_support import _render_cpp_support_runtime
    from debug import debug_log, debug_path
    from python_ctypes_backend import generate_python_ctypes

# Type alias for spec types
SourceBindingSpec = Union[AuthoredBindingSpec, MergedBindingSpec]

def _render_cpp(spec: BindingIR, header_name: str) -> str:
    debug_log(
        "c_backend.render_cpp.start",
        f"module={spec.module} handles={len(spec.handles)} functions={len(spec.functions)} methods={len(spec.methods)}",
    )
    handle_structs = []
    for handle in spec.handles.values():
        storage_type = _handle_storage_type(handle)
        if handle.name in {"attribute_value", "instance_list"}:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} value;\n}};")
        elif handle.ptr_type == "shared_ptr":
            # shared_ptr handles don't need owned field - ref counting handles ownership
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} ptr;\n}};")
        elif handle.ptr_type == "value":
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} value;\n}};")
        else:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} ptr;\n    bool owned;\n}};")

    destroy_impls = []
    for handle in spec.handles.values():
        destroy_impls.append(
            f"""void ifcopenshell_{_snake_name(handle.c_type)}_destroy({handle.c_type}* handle) {{
    if (handle == nullptr) {{
        return;
    }}
    {_destroy_body(handle)}
}}"""
        )

    rendered_calls: list[str] = []
    total_calls = len(spec.functions) + len(spec.methods)
    for index, call in enumerate((*spec.functions, *spec.methods), start=1):
        if index == 1 or index % 100 == 0 or index == total_calls:
            debug_log("c_backend.render_cpp.calls", f"{index}/{total_calls} current={call.c_name}")
        rendered_calls.append(_render_call_impl(call, spec))
    call_impls = "\n\n".join(rendered_calls)
    includes = "\n".join(
        f'#include {header}' if header.startswith('<') else f'#include "{header}"'
        for header in spec.public_headers
    )
    handle_structs_block = "\n\n".join(handle_structs)
    destroy_impls_block = "\n\n".join(destroy_impls)
    handle_list_types = _used_handle_list_handles(spec)
    sequence_kinds = _used_scalar_sequence_kinds(spec)
    handle_list_helpers = "\n\n".join(
        _render_handle_list_helpers(handle) for handle in handle_list_types
    )
    handle_list_list_helpers = "\n\n".join(
        _render_handle_list_list_helpers(handle) for handle in handle_list_types
    )
    handle_list_destroy_impls = "\n\n".join(_render_handle_list_destroy_impl(handle) for handle in handle_list_types)
    handle_list_list_destroy_impls = "\n\n".join(
        _render_handle_list_list_destroy_impl(handle) for handle in handle_list_types
    )

    # Only emit common type implementations if this is not a dependent module.
    depends_on_common = spec.depends_on_common
    common_type_impls = "" if depends_on_common else _render_common_type_impls(sequence_kinds)
    sequence_helpers = _render_sequence_helpers(sequence_kinds)
    runtime_support = _render_cpp_support_runtime()

    internal_header_name = header_name.removesuffix(".h") + "_internal.hpp"
    rendered = f"""#include "{header_name}"
#include "{internal_header_name}"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Note: project-specific headers (geometry, serializers, schema, etc.) are
// pulled in transitively via {internal_header_name}; do not re-include them
// here to avoid header-guard-less redefinitions in third-party headers.
#include "utils.h"

{runtime_support}

{handle_list_helpers}
{handle_list_list_helpers}

{common_type_impls}

{sequence_helpers}

void {spec.c_prefix}_clear_error(void) {{
    ifcopenshell::capi::g_last_error.clear();
    ifcopenshell::capi::g_last_error_kind = 0;
}}

const char* {spec.c_prefix}_last_error_message(void) {{
    return ifcopenshell::capi::g_last_error.c_str();
}}

int {spec.c_prefix}_last_error_kind(void) {{
    return ifcopenshell::capi::g_last_error_kind;
}}

{destroy_impls_block}
{handle_list_destroy_impls}
{handle_list_list_destroy_impls}

{call_impls}
"""
    debug_log("c_backend.render_cpp.done", f"module={spec.module} bytes={len(rendered)}")
    return rendered


def generate(
    spec_path: Path,
    header_out: Path,
    cpp_out: Path,
    compile_commands_path: Path | None = None,
    internal_header_out: Path | None = None,
    python_out: Path | None = None,
    discovery_include_dirs: tuple[Path, ...] = (),
    discovery_defines: tuple[str, ...] = (),
    discovery_clang_args: tuple[str, ...] = (),
) -> None:
    debug_log(
        "c_backend.generate.start",
        f"spec={debug_path(spec_path)} header_out={debug_path(header_out)} cpp_out={debug_path(cpp_out)} internal_header_out={debug_path(internal_header_out)} compile_commands={debug_path(compile_commands_path)}",
    )
    spec = lower_binding_spec(
        load_authored_spec(
            spec_path,
            compile_commands_path=compile_commands_path,
            discovery_include_dirs=discovery_include_dirs,
            discovery_defines=discovery_defines,
            discovery_clang_args=discovery_clang_args,
        )
    )
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")
    if internal_header_out is None:
        internal_header_out = cpp_out.with_name(header_out.stem + "_internal.hpp")
    internal_header_out.parent.mkdir(parents=True, exist_ok=True)
    internal_header_out.write_text(_render_internal_header(spec, header_out.name), encoding="utf-8")
    if python_out is not None:
        python_out.parent.mkdir(parents=True, exist_ok=True)
        generate_python_ctypes(spec, python_out, generic_handles=True)
    debug_log("c_backend.generate.done", f"spec={debug_path(spec_path)}")


def generate_merged(
    spec_paths: list[Path],
    module: str,
    c_prefix: str,
    header_out: Path,
    cpp_out: Path,
    compile_commands_path: Path | None = None,
    internal_header_out: Path | None = None,
    python_out: Path | None = None,
    discovery_include_dirs: tuple[Path, ...] = (),
    discovery_defines: tuple[str, ...] = (),
    discovery_clang_args: tuple[str, ...] = (),
) -> None:
    """Generate bindings from multiple specs merged together."""
    debug_log(
        "c_backend.generate_merged.start",
        f"specs={len(spec_paths)} header_out={debug_path(header_out)} cpp_out={debug_path(cpp_out)} internal_header_out={debug_path(internal_header_out)} compile_commands={debug_path(compile_commands_path)}",
    )
    spec = lower_binding_spec(
        load_merged_specs(
            spec_paths,
            module,
            c_prefix,
            compile_commands_path=compile_commands_path,
            discovery_include_dirs=discovery_include_dirs,
            discovery_defines=discovery_defines,
            discovery_clang_args=discovery_clang_args,
        )
    )
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")
    if internal_header_out is None:
        internal_header_out = cpp_out.with_name(header_out.stem + "_internal.hpp")
    internal_header_out.parent.mkdir(parents=True, exist_ok=True)
    internal_header_out.write_text(_render_internal_header(spec, header_out.name), encoding="utf-8")
    if python_out is not None:
        python_out.parent.mkdir(parents=True, exist_ok=True)
        generate_python_ctypes(spec, python_out, generic_handles=True)
    debug_log("c_backend.generate_merged.done", f"module={module}")


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate the first C backend skeleton from a handwritten binding spec.")
    parser.add_argument("--spec", type=Path, action="append", required=True, 
                        help="Path to a binding spec YAML. Can be specified multiple times for merged output.")
    parser.add_argument("--header-out", type=Path, required=True, help="Output path for the generated C header.")
    parser.add_argument("--cpp-out", type=Path, required=True, help="Output path for the generated C++ glue source.")
    parser.add_argument(
        "--internal-header-out",
        type=Path,
        default=None,
        help="Optional output path for the internal C++ header that exposes handle struct definitions and error helpers. Defaults to <cpp-out-dir>/<header-stem>_internal.hpp.",
    )
    parser.add_argument(
        "--python-out",
        type=Path,
        default=None,
        help="Optional output path for generated Python ctypes glue.",
    )
    parser.add_argument(
        "--compile-commands",
        type=Path,
        default=None,
        help="Optional compile_commands.json override for AST-backed source discovery.",
    )
    parser.add_argument(
        "--discovery-include-dir",
        type=Path,
        action="append",
        default=[],
        help="Additional deterministic include directory for no-compile-commands AST discovery.",
    )
    parser.add_argument(
        "--discovery-define",
        action="append",
        default=[],
        help="Additional preprocessor definition for no-compile-commands AST discovery, without the -D prefix.",
    )
    parser.add_argument(
        "--discovery-clang-arg",
        action="append",
        default=[],
        help="Additional raw Clang argument for no-compile-commands AST discovery.",
    )
    parser.add_argument(
        "--module",
        type=str,
        default="ifcopenshell",
        help="Module name for merged output (default: ifcopenshell).",
    )
    parser.add_argument(
        "--c-prefix",
        type=str,
        default="ifcopenshell",
        help="C function prefix for merged output (default: ifcopenshell).",
    )
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    if len(args.spec) == 1:
        # Single spec - use original behavior
        generate(
            args.spec[0],
            args.header_out,
            args.cpp_out,
            compile_commands_path=args.compile_commands,
            internal_header_out=args.internal_header_out,
            python_out=args.python_out,
            discovery_include_dirs=tuple(args.discovery_include_dir),
            discovery_defines=tuple(args.discovery_define),
            discovery_clang_args=tuple(args.discovery_clang_arg),
        )
    else:
        # Multiple specs - merge them
        generate_merged(
            args.spec,
            args.module,
            args.c_prefix,
            args.header_out,
            args.cpp_out,
            compile_commands_path=args.compile_commands,
            internal_header_out=args.internal_header_out,
            python_out=args.python_out,
            discovery_include_dirs=tuple(args.discovery_include_dir),
            discovery_defines=tuple(args.discovery_define),
            discovery_clang_args=tuple(args.discovery_clang_arg),
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
