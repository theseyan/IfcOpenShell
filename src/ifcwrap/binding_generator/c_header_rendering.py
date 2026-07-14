# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from .abi_ir import _handle_destroy_name
from .authored_spec import HandleSpec
from .binding_ir import BindingIR
from .c_sequence_helpers import (
    _handle_list_c_type,
    _handle_list_list_c_type,
    _render_common_type_decls,
    _render_handle_list_destroy_decl,
    _render_handle_list_list_destroy_decl,
    _used_handle_list_handles,
    _used_scalar_sequence_kinds,
)
from .c_type_rendering import (
    _ordered_result_structs,
    _render_call_decl,
    _render_option_struct_decl,
    _render_optional_result_struct_decl,
    _render_result_struct_decl,
    _render_variant_decl,
)
from .c_value_rendering import _render_result_struct_destroy_decls
from .c_variant_helpers import _render_variant_destroy_decls
from .debug import debug_log


def _render_handle_destroy_decl(handle: HandleSpec) -> str:
    return f"void {_handle_destroy_name(handle)}({handle.c_type}* handle);"


def _render_header(spec: BindingIR) -> str:
    debug_log(
        "c_backend.render_header.start",
        f"module={spec.module} handles={len(spec.handles)} functions={len(spec.functions)} methods={len(spec.methods)}",
    )
    guard = f"{spec.c_prefix.upper()}_API_H"
    handle_forwards = "\n".join(
        f"typedef struct {handle.c_type} {handle.c_type};"
        for handle in spec.handles.values()
    )
    handle_list_types = _used_handle_list_handles(spec)
    handle_list_forwards = "\n".join(
        f"typedef struct {_handle_list_c_type(handle)} {{\n"
        f"    {handle.c_type}** items;\n"
        f"    size_t size;\n"
        f"}} {_handle_list_c_type(handle)};"
        for handle in handle_list_types
    )
    handle_list_list_forwards = "\n".join(
        f"typedef struct {_handle_list_list_c_type(handle)} {{\n"
        f"    {_handle_list_c_type(handle)}* items;\n"
        f"    size_t size;\n"
        f"}} {_handle_list_list_c_type(handle)};"
        for handle in handle_list_types
    )
    result_struct_decls = "\n\n".join(
        _render_result_struct_decl(struct, spec)
        for struct in _ordered_result_structs(spec)
    )
    optional_result_struct_decls = "\n\n".join(
        _render_optional_result_struct_decl(call.returns, spec)
        for call in spec.calls
        if call.returns.kind == "struct" and call.returns.nullable
    )
    variant_decls = "\n\n".join(
        _render_variant_decl(call.returns, spec)
        for call in spec.calls
        if call.returns.kind == "variant"
    )
    option_struct_decls = "\n\n".join(
        _render_option_struct_decl(struct, spec)
        for struct in spec.option_structs.values()
    )
    destroy_decls = "\n".join(
        _render_handle_destroy_decl(handle) for handle in spec.handles.values()
    )
    handle_list_destroy_decls = "\n".join(
        _render_handle_list_destroy_decl(handle) for handle in handle_list_types
    )
    handle_list_list_destroy_decls = "\n".join(
        _render_handle_list_list_destroy_decl(handle) for handle in handle_list_types
    )
    variant_destroy_decls = _render_variant_destroy_decls(spec)
    result_struct_destroy_decls = _render_result_struct_destroy_decls(spec.abi)
    call_decls = "\n".join(_render_call_decl(call, spec) for call in spec.calls)
    sequence_kinds = _used_scalar_sequence_kinds(spec)
    common_type_decls = _render_common_type_decls(sequence_kinds)

    rendered = f"""#ifndef {guard}
#define {guard}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

/* Common types - guarded to allow multiple API headers to be included */
#ifndef IFCOPENSHELL_COMMON_TYPES_DEFINED
#define IFCOPENSHELL_COMMON_TYPES_DEFINED

{common_type_decls}

#endif /* IFCOPENSHELL_COMMON_TYPES_DEFINED */

{handle_forwards}

{handle_list_forwards}
{handle_list_list_forwards}

{option_struct_decls}

{result_struct_decls}

{optional_result_struct_decls}

{variant_decls}

typedef enum {{
    IFCOPENSHELL_ERROR_NONE = 0,
    IFCOPENSHELL_ERROR_RUNTIME = 1,
    IFCOPENSHELL_ERROR_VALUE = 2,
    IFCOPENSHELL_ERROR_TYPE = 3,
    IFCOPENSHELL_ERROR_NOT_IMPLEMENTED = 4,
    IFCOPENSHELL_ERROR_KEY = 5
}} ifcopenshell_error_kind_t;

void {spec.c_prefix}_clear_error(void);
const char* {spec.c_prefix}_last_error_message(void);
int {spec.c_prefix}_last_error_kind(void);

{destroy_decls}
{handle_list_destroy_decls}
{handle_list_list_destroy_decls}
{result_struct_destroy_decls}
{variant_destroy_decls}

{call_decls}

#ifdef __cplusplus
}}
#endif

#endif
"""
    debug_log(
        "c_backend.render_header.done", f"module={spec.module} bytes={len(rendered)}"
    )
    return rendered
