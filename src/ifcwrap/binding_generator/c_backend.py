from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from types import SimpleNamespace
from typing import Sequence, Union

try:
    from .authored_spec import AuthoredBindingSpec, MergedBindingSpec, load_authored_spec, load_merged_specs
    from .binding_ir import (
        BindingIR,
        lower_binding_spec,
    )
    from .binding_model import HandleSpec
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
    from .clang_discovery import CompilationConfig, DiscoveryEnvironment
    from .cpp_spec_frontend import (
        discover_cpp_spec_functions,
        discover_cpp_spec_contract_headers,
        discover_cpp_spec_handles,
        discover_cpp_spec_result_structs,
        lower_cpp_spec_functions_to_calls,
        lower_cpp_spec_handles_to_specs,
        lower_cpp_spec_result_structs_to_specs,
    )
    from .debug import debug_log, debug_path
    from .python_ctypes_backend import generate_python_ctypes
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, MergedBindingSpec, load_authored_spec, load_merged_specs
    from binding_ir import (
        BindingIR,
        lower_binding_spec,
    )
    from binding_model import HandleSpec
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
    from clang_discovery import CompilationConfig, DiscoveryEnvironment
    from cpp_spec_frontend import (
        discover_cpp_spec_functions,
        discover_cpp_spec_contract_headers,
        discover_cpp_spec_handles,
        discover_cpp_spec_result_structs,
        lower_cpp_spec_functions_to_calls,
        lower_cpp_spec_handles_to_specs,
        lower_cpp_spec_result_structs_to_specs,
    )
    from debug import debug_log, debug_path
    from python_ctypes_backend import generate_python_ctypes

# Type alias for spec types
SourceBindingSpec = Union[AuthoredBindingSpec, MergedBindingSpec]


@dataclass(frozen=True)
class CppSpecConfig:
    path: Path
    namespace: str
    c_prefix: str | None = None


def _normalize_repeated_cpp_option(
    values: str | Sequence[str] | None,
    count: int,
    *,
    option_name: str,
    required: bool,
) -> tuple[str | None, ...]:
    if isinstance(values, str):
        normalized: tuple[str, ...] = (values,)
    else:
        normalized = tuple(values or ())
    if not normalized:
        if required:
            msg = f"{option_name} is required when cpp_spec_paths are provided"
            raise ValueError(msg)
        return (None,) * count
    if len(normalized) == 1:
        return normalized * count
    if len(normalized) != count:
        msg = f"{option_name} must be provided once or exactly once per --cpp-spec"
        raise ValueError(msg)
    return normalized


def _cpp_spec_configs(
    spec_paths: Sequence[Path],
    namespaces: str | Sequence[str] | None,
    c_prefixes: str | Sequence[str] | None,
) -> tuple[CppSpecConfig, ...]:
    namespace_values = _normalize_repeated_cpp_option(
        namespaces,
        len(spec_paths),
        option_name="cpp_spec_namespace",
        required=True,
    )
    c_prefix_values = _normalize_repeated_cpp_option(
        c_prefixes,
        len(spec_paths),
        option_name="cpp_spec_c_prefix",
        required=False,
    )
    return tuple(
        CppSpecConfig(path=path, namespace=namespace, c_prefix=c_prefix)
        for path, namespace, c_prefix in zip(spec_paths, namespace_values, c_prefix_values)
        if namespace is not None
    )


def _cpp_spec_public_header(spec_path: Path, include_dirs: tuple[Path, ...]) -> str:
    resolved = spec_path.resolve()
    for include_dir in include_dirs:
        try:
            return resolved.relative_to(include_dir.resolve()).as_posix()
        except ValueError:
            continue
    return spec_path.name


def _merge_cpp_specs(
    base: MergedBindingSpec,
    configs: Sequence[CppSpecConfig],
    compile_commands_path: Path | None,
    *,
    discovery_include_dirs: tuple[Path, ...] = (),
    discovery_defines: tuple[str, ...] = (),
    discovery_clang_args: tuple[str, ...] = (),
) -> MergedBindingSpec:
    environment = _cpp_spec_environment(
        None,
        discovery_include_dirs=discovery_include_dirs,
        discovery_defines=discovery_defines,
        discovery_clang_args=discovery_clang_args,
    )
    handles = dict(base.handles)
    result_structs = dict(base.result_structs)
    calls = list(base.functions)
    methods = list(base.methods)
    existing_c_names = {call.c_name for call in (*base.functions, *base.methods)}
    public_headers = list(base.public_headers)
    for config in configs:
        public_header = _cpp_spec_public_header(config.path, discovery_include_dirs)
        if public_header not in public_headers:
            public_headers.append(public_header)

        for handle_name, handle in lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(config.path)).items():
            if handle_name in handles and handles[handle_name] != handle:
                msg = f"C++ spec handle '{handle_name}' is declared with conflicting metadata"
                raise ValueError(msg)
            handles[handle_name] = handle
        for struct_name, struct in lower_cpp_spec_result_structs_to_specs(
            discover_cpp_spec_result_structs(config.path, config.namespace),
            handles,
        ).items():
            if struct_name in result_structs and result_structs[struct_name] != struct:
                msg = f"C++ spec result struct '{struct_name}' is declared with conflicting metadata"
                raise ValueError(msg)
            result_structs[struct_name] = struct
        for call in lower_cpp_spec_functions_to_calls(
            discover_cpp_spec_functions(
                environment,
                config.path,
                config.namespace,
                contract_headers=discover_cpp_spec_contract_headers(config.path, discovery_include_dirs),
            ),
            handles,
            result_structs,
            c_prefix=config.c_prefix,
        ):
            if call.c_name in existing_c_names:
                calls = [existing for existing in calls if existing.c_name != call.c_name]
                methods = [existing for existing in methods if existing.c_name != call.c_name]
            else:
                existing_c_names.add(call.c_name)
            if call.receiver is None:
                calls.append(call)
            else:
                methods.append(call)

    return MergedBindingSpec(
        module=base.module,
        c_prefix=base.c_prefix,
        public_headers=tuple(public_headers),
        handles=handles,
        result_structs=result_structs,
        functions=tuple(calls),
        methods=tuple(methods),
        discovery_diagnostics=base.discovery_diagnostics,
    )


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
#include <limits>
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
    cpp_spec_paths: list[Path] | None = None,
    cpp_spec_namespace: str | Sequence[str] | None = None,
    cpp_spec_c_prefix: str | Sequence[str] | None = None,
) -> None:
    """Generate bindings from multiple specs merged together."""
    debug_log(
        "c_backend.generate_merged.start",
        f"specs={len(spec_paths)} header_out={debug_path(header_out)} cpp_out={debug_path(cpp_out)} internal_header_out={debug_path(internal_header_out)} compile_commands={debug_path(compile_commands_path)}",
    )
    cpp_spec_configs = _cpp_spec_configs(cpp_spec_paths, cpp_spec_namespace, cpp_spec_c_prefix) if cpp_spec_paths else ()
    cpp_spec_handles: dict[str, HandleSpec] = {}
    for config in cpp_spec_configs:
        for handle_name, handle in lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(config.path)).items():
            if handle_name in cpp_spec_handles and cpp_spec_handles[handle_name] != handle:
                msg = f"C++ spec handle '{handle_name}' is declared with conflicting metadata"
                raise ValueError(msg)
            cpp_spec_handles[handle_name] = handle
    merged_spec = load_merged_specs(
        spec_paths,
        module,
        c_prefix,
        compile_commands_path=compile_commands_path,
        discovery_include_dirs=discovery_include_dirs,
        discovery_defines=discovery_defines,
        discovery_clang_args=discovery_clang_args,
        existing_handles=cpp_spec_handles,
    )
    if cpp_spec_configs:
        merged_spec = _merge_cpp_specs(
            merged_spec,
            cpp_spec_configs,
            compile_commands_path,
            discovery_include_dirs=discovery_include_dirs,
            discovery_defines=discovery_defines,
            discovery_clang_args=discovery_clang_args,
        )
    spec = lower_binding_spec(merged_spec)
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


def _cpp_spec_environment(
    compile_commands_path: Path | None,
    *,
    discovery_include_dirs: tuple[Path, ...] = (),
    discovery_defines: tuple[str, ...] = (),
    discovery_clang_args: tuple[str, ...] = (),
) -> DiscoveryEnvironment:
    return DiscoveryEnvironment(
        compile_commands_path=compile_commands_path,
        compilation=CompilationConfig(
            include_dirs=discovery_include_dirs,
            defines=discovery_defines,
            clang_args=("-x", "c++", "-std=c++17", *discovery_clang_args),
        ),
    )


def generate_cpp_specs(
    spec_paths: list[Path],
    namespace: str | Sequence[str],
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
    function_c_prefix: str | Sequence[str] | None = None,
) -> None:
    """Generate bindings from explicit C++ spec translation units."""
    configs = _cpp_spec_configs(spec_paths, namespace, function_c_prefix)
    environment = _cpp_spec_environment(
        None,
        discovery_include_dirs=discovery_include_dirs,
        discovery_defines=discovery_defines,
        discovery_clang_args=discovery_clang_args,
    )
    handles = {}
    result_structs = {}
    calls = []
    existing_c_names: set[str] = set()
    for config in configs:
        for handle_name, handle in lower_cpp_spec_handles_to_specs(discover_cpp_spec_handles(config.path)).items():
            if handle_name in handles and handles[handle_name] != handle:
                msg = f"C++ spec handle '{handle_name}' is declared with conflicting metadata"
                raise ValueError(msg)
            handles[handle_name] = handle
        for struct_name, struct in lower_cpp_spec_result_structs_to_specs(
            discover_cpp_spec_result_structs(config.path, config.namespace),
            handles,
        ).items():
            if struct_name in result_structs and result_structs[struct_name] != struct:
                msg = f"C++ spec result struct '{struct_name}' is declared with conflicting metadata"
                raise ValueError(msg)
            result_structs[struct_name] = struct
        for call in lower_cpp_spec_functions_to_calls(
            discover_cpp_spec_functions(
                environment,
                config.path,
                config.namespace,
                contract_headers=discover_cpp_spec_contract_headers(config.path, discovery_include_dirs),
            ),
            handles,
            result_structs,
            c_prefix=config.c_prefix,
        ):
            if call.c_name in existing_c_names:
                msg = f"C++ spec export '{call.c_name}' duplicates an existing generated C symbol"
                raise ValueError(msg)
            existing_c_names.add(call.c_name)
            calls.append(call)

    spec = lower_binding_spec(
        SimpleNamespace(
            module=module,
            c_prefix=c_prefix,
            public_headers=tuple(_cpp_spec_public_header(config.path, discovery_include_dirs) for config in configs),
            public_header_plugins={},
            handles=handles,
            result_structs=result_structs,
            functions=tuple(calls),
            methods=(),
            depends_on_common=None,
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
    debug_log("c_backend.generate_cpp_specs.done", f"specs={len(spec_paths)} module={module}")


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate the first C backend skeleton from a handwritten binding spec.")
    parser.add_argument("--spec", type=Path, action="append", default=[],
                        help="Path to a binding spec YAML. Can be specified multiple times for merged output.")
    parser.add_argument(
        "--cpp-spec",
        type=Path,
        action="append",
        default=[],
        help="Path to a C++ binding spec translation unit. Can be specified multiple times.",
    )
    parser.add_argument(
        "--cpp-spec-namespace",
        type=str,
        action="append",
        default=[],
        help=(
            "Namespace containing exported C++ spec functions when using --cpp-spec. "
            "Provide once for all C++ specs or once per --cpp-spec."
        ),
    )
    parser.add_argument(
        "--cpp-spec-c-prefix",
        type=str,
        action="append",
        default=[],
        help=(
            "Optional C symbol prefix to apply to C++ spec function names. "
            "If omitted, C++ spec function names are used as exact C symbols. "
            "Provide once for all C++ specs or once per --cpp-spec."
        ),
    )
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
    if args.cpp_spec and not args.spec:
        if not args.cpp_spec_namespace:
            msg = "--cpp-spec-namespace is required with --cpp-spec"
            raise ValueError(msg)
        generate_cpp_specs(
            args.cpp_spec,
            args.cpp_spec_namespace,
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
            function_c_prefix=args.cpp_spec_c_prefix,
        )
        return 0
    if not args.spec:
        msg = "At least one --spec or --cpp-spec is required"
        raise ValueError(msg)
    if len(args.spec) == 1 and not args.cpp_spec:
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
        if args.cpp_spec and not args.cpp_spec_namespace:
            msg = "--cpp-spec-namespace is required with --cpp-spec"
            raise ValueError(msg)
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
            cpp_spec_paths=args.cpp_spec,
            cpp_spec_namespace=args.cpp_spec_namespace,
            cpp_spec_c_prefix=args.cpp_spec_c_prefix,
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
