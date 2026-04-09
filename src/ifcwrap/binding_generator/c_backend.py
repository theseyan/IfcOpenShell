# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from pathlib import Path
import re
from typing import Union

try:
    from .authored_spec import AuthoredBindingSpec, CallSpec, HandleSpec, MergedBindingSpec, ParamSpec, TypeSpec, load_authored_spec, load_merged_specs
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, CallSpec, HandleSpec, MergedBindingSpec, ParamSpec, TypeSpec, load_authored_spec, load_merged_specs

# Type alias for spec types
BindingSpec = Union[AuthoredBindingSpec, MergedBindingSpec]

AI_HEADER = "// This file was generated with the assistance of an AI coding tool.\n"

# Common type implementations - shared across all API modules
# Only the first module (ifcparse) should emit these; dependent modules skip them
COMMON_TYPE_IMPLS = """
void ifcopenshell_string_destroy(ifcopenshell_string_t* value) {
    if (value == nullptr) {
        return;
    }
    if (value->owned && value->data != nullptr) {
        delete[] value->data;
    }
    value->data = nullptr;
    value->size = 0;
    value->owned = false;
}

void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_string_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_int32_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}

void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value) {
    if (value == nullptr || value->items == nullptr) {
        return;
    }
    for (size_t i = 0; i < value->size; ++i) {
        ifcopenshell_double_list_destroy(&value->items[i]);
    }
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}
"""


def _snake_name(c_type: str) -> str:
    base = c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return base


def _handle_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_t"


def _handle_list_helper_name(handle: HandleSpec) -> str:
    return f"make_{_snake_name(_handle_list_c_type(handle))}"


def _render_handle_list_helpers(handle: HandleSpec) -> str:
    """Generate make_ and to_cpp_ helpers for a handle list type."""
    list_c = _handle_list_c_type(handle)
    helper_name = _handle_list_helper_name(handle)
    snake = _snake_name(list_c)
    if handle.ptr_type == "shared_ptr":
        return f"""static {list_c} {helper_name}(const std::vector<std::shared_ptr<{handle.cpp_type}>>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = new {handle.c_type}{{values[i]}};
    }}
    return {list_c}{{items, values.size()}};
}}

static std::vector<std::shared_ptr<{handle.cpp_type}>> to_cpp_{snake}(const {list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
    std::vector<std::shared_ptr<{handle.cpp_type}>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        auto* item = values->items[i];
        if (item == nullptr) {{
            throw std::runtime_error("handle_list contains an invalid handle");
        }}
        result.push_back(item->ptr);
    }}
    return result;
}}"""
    return f"""static {list_c} {helper_name}(const std::vector<{handle.cpp_type}*>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = new {handle.c_type}{{values[i], false}};
    }}
    return {list_c}{{items, values.size()}};
}}

static {list_c} {helper_name}(const std::vector<const {handle.cpp_type}*>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = new {handle.c_type}{{const_cast<{handle.cpp_type}*>(values[i]), false}};
    }}
    return {list_c}{{items, values.size()}};
}}

static std::vector<const {handle.cpp_type}*> to_cpp_{snake}(const {list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
    std::vector<const {handle.cpp_type}*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {{
            throw std::runtime_error("handle_list contains an invalid handle");
        }}
        result.push_back(item->ptr);
    }}
    return result;
}}"""


def _used_handle_list_handles(spec: BindingSpec) -> tuple[HandleSpec, ...]:
    seen: set[str] = set()
    handles: list[HandleSpec] = []
    for call in (*spec.functions, *spec.methods):
        if call.returns.kind == "handle_list":
            handle_name = call.returns.handle
            if handle_name not in seen:
                seen.add(handle_name)
                handles.append(spec.handles[handle_name])
        for param in call.params:
            if param.type.kind != "handle_list":
                continue
            handle_name = param.type.handle
            if handle_name in seen:
                continue
            seen.add(handle_name)
            handles.append(spec.handles[handle_name])
    return tuple(handles)


def _normalize_cpp_type(cpp_type: str | None) -> str:
    if not cpp_type:
        return ""
    cpp_type = re.sub(r"/\*.*?\*/", "", cpp_type)
    return " ".join(
        cpp_type.replace(" &", "&").replace(" *", "*").replace("< ", "<").replace(" >", ">").split()
    )


# Mapping from type kind to (param_type, out_type, result_template, needs_conversion).
# Entries with None require handle-specific logic handled separately.
_SCALAR_TYPE_MAP: dict[str, tuple[str, str, str]] = {
    "bool":            ("bool",     "bool*",     "*out_result = {expr};"),
    "int32":           ("int32_t",  "int32_t*",  "*out_result = static_cast<int32_t>({expr});"),
    "double":          ("double",   "double*",   "*out_result = static_cast<double>({expr});"),
    "uint32":          ("uint32_t", "uint32_t*", "*out_result = static_cast<uint32_t>({expr});"),
    "size":            ("size_t",   "size_t*",   "*out_result = static_cast<size_t>({expr});"),
}

_LIST_TYPE_MAP: dict[str, tuple[str, str, str, str]] = {
    # kind: (param_type, out_type, make_helper, to_cpp_helper)
    "bool_list":         ("const ifcopenshell_bool_list_t*",         "ifcopenshell_bool_list_t*",         "make_bool_list",         "to_cpp_bool_list"),
    "string_list":       ("const ifcopenshell_string_list_t*",       "ifcopenshell_string_list_t*",       "make_string_list",       "to_cpp_string_list"),
    "int32_list":        ("const ifcopenshell_int32_list_t*",        "ifcopenshell_int32_list_t*",        "make_int32_list",        "to_cpp_int32_list"),
    "uint32_list":       ("const ifcopenshell_uint32_list_t*",       "ifcopenshell_uint32_list_t*",       "make_uint32_list",       "to_cpp_uint32_list"),
    "int32_list_list":   ("const ifcopenshell_int32_list_list_t*",   "ifcopenshell_int32_list_list_t*",   "make_int32_list_list",   "to_cpp_int32_list_list"),
    "double_list":       ("const ifcopenshell_double_list_t*",       "ifcopenshell_double_list_t*",       "make_double_list",       "to_cpp_double_list"),
    "double_list_list":  ("const ifcopenshell_double_list_list_t*",  "ifcopenshell_double_list_list_t*",  "make_double_list_list",  "to_cpp_double_list_list"),
}

_BUFFER_TYPE_MAP: dict[str, tuple[str, str]] = {
    "double_buffer": ("const double*",   "const double**"),
    "int32_buffer":  ("const int32_t*",  "const int32_t**"),
}

# Kinds whose C parameter uses a _cpp-suffixed variable after conversion
_NEEDS_CONVERSION_KINDS = frozenset(
    {"string", "string_list", "bool_list", "int32_list", "uint32_list",
     "int32_list_list", "double_list", "double_list_list",
     "handle", "handle_list", "opaque_ptr"}
)


def _cpp_param_type(param: ParamSpec, spec: BindingSpec) -> str:
    kind = param.type.kind
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][0]
    if kind == "string":
        return "const char*"
    if kind in _LIST_TYPE_MAP:
        return _LIST_TYPE_MAP[kind][0]
    if kind in _BUFFER_TYPE_MAP:
        return _BUFFER_TYPE_MAP[kind][0]
    if kind == "handle":
        return f"{spec.handles[param.type.handle].c_type}*"
    if kind == "handle_list":
        return f"const {_handle_list_c_type(spec.handles[param.type.handle])}*"
    if kind == "opaque_ptr":
        return "void*"
    msg = f"Unsupported parameter kind: {kind}"
    raise ValueError(msg)


def _out_param_type(type_spec: TypeSpec, spec: BindingSpec) -> str:
    kind = type_spec.kind
    if kind == "void":
        msg = "void has no out parameter"
        raise ValueError(msg)
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][1]
    if kind == "string":
        return "ifcopenshell_string_t*"
    if kind in _LIST_TYPE_MAP:
        return _LIST_TYPE_MAP[kind][1]
    if kind in _BUFFER_TYPE_MAP:
        return _BUFFER_TYPE_MAP[kind][1]
    if kind == "handle":
        return f"{spec.handles[type_spec.handle].c_type}**"
    if kind == "handle_list":
        return f"{_handle_list_c_type(spec.handles[type_spec.handle])}*"
    if kind == "opaque_ptr":
        return "void**"
    msg = f"Unsupported return kind: {kind}"
    raise ValueError(msg)


def _render_call_decl(call: CallSpec, spec: BindingSpec) -> str:
    parts = []
    if call.receiver is not None:
        parts.append(f"{spec.handles[call.receiver].c_type}* self")
    parts.extend(f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params)
    if call.returns.kind != "void":
        parts.append(f"{_out_param_type(call.returns, spec)} out_result")
    params = ", ".join(parts) if parts else "void"
    return f"bool {call.c_name}({params});"


def _render_handle_destroy_decl(handle: HandleSpec) -> str:
    return f"void ifcopenshell_{_snake_name(handle.c_type)}_destroy({handle.c_type}* handle);"


def _render_handle_list_destroy_decl(handle: HandleSpec) -> str:
    list_c_type = _handle_list_c_type(handle)
    return f"void ifcopenshell_{_snake_name(list_c_type)}_destroy({list_c_type}* value);"


def _render_header(spec: BindingSpec) -> str:
    guard = f"{spec.c_prefix.upper()}_API_H"
    handle_forwards = "\n".join(f"typedef struct {handle.c_type} {handle.c_type};" for handle in spec.handles.values())
    handle_list_types = _used_handle_list_handles(spec)
    handle_list_forwards = "\n".join(
        f"typedef struct {_handle_list_c_type(handle)} {{\n"
        f"    {handle.c_type}** items;\n"
        f"    size_t size;\n"
        f"}} {_handle_list_c_type(handle)};"
        for handle in handle_list_types
    )
    destroy_decls = "\n".join(_render_handle_destroy_decl(handle) for handle in spec.handles.values())
    handle_list_destroy_decls = "\n".join(_render_handle_list_destroy_decl(handle) for handle in handle_list_types)
    call_decls = "\n".join(_render_call_decl(call, spec) for call in (*spec.functions, *spec.methods))

    return f"""{AI_HEADER}
#ifndef {guard}
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

typedef struct ifcopenshell_string_t {{
    char* data;
    size_t size;
    bool owned;
}} ifcopenshell_string_t;

typedef struct ifcopenshell_string_list_t {{
    ifcopenshell_string_t* items;
    size_t size;
}} ifcopenshell_string_list_t;

typedef struct ifcopenshell_bool_list_t {{
    bool* items;
    size_t size;
}} ifcopenshell_bool_list_t;

typedef struct ifcopenshell_int32_list_t {{
    int32_t* items;
    size_t size;
}} ifcopenshell_int32_list_t;

typedef struct ifcopenshell_uint32_list_t {{
    uint32_t* items;
    size_t size;
}} ifcopenshell_uint32_list_t;

typedef struct ifcopenshell_int32_list_list_t {{
    ifcopenshell_int32_list_t* items;
    size_t size;
}} ifcopenshell_int32_list_list_t;

typedef struct ifcopenshell_double_list_t {{
    double* items;
    size_t size;
}} ifcopenshell_double_list_t;

typedef struct ifcopenshell_double_list_list_t {{
    ifcopenshell_double_list_t* items;
    size_t size;
}} ifcopenshell_double_list_list_t;

void ifcopenshell_string_destroy(ifcopenshell_string_t* value);
void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value);
void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value);
void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value);
void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value);
void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value);
void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value);
void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value);

#endif /* IFCOPENSHELL_COMMON_TYPES_DEFINED */

{handle_forwards}

{handle_list_forwards}

void {spec.c_prefix}_clear_error(void);
const char* {spec.c_prefix}_last_error_message(void);

{destroy_decls}
{handle_list_destroy_decls}

{call_decls}

#ifdef __cplusplus
}}
#endif

#endif
"""


def _handle_storage_type(handle: HandleSpec) -> str:
    if handle.name == "attribute_value":
        return "AttributeValue"
    if handle.name == "instance_list":
        return "aggregate_of_instance::ptr"
    if handle.ptr_type == "shared_ptr":
        return f"std::shared_ptr<{handle.cpp_type}>"
    return f"{handle.cpp_type}*"


def _destroy_body(handle: HandleSpec) -> str:
    if handle.name in {"attribute_value", "instance_list"}:
        return "delete handle;"
    if handle.destructor == "shared_ptr":
        # For shared_ptr handles, the shared_ptr destructor handles the ref count
        return "handle->ptr.reset();\n    delete handle;"
    if handle.destructor == "delete":
        return "if (handle->owned && handle->ptr) { delete handle->ptr; }\n    delete handle;"
    return "delete handle;"


def _wrap_handle_expr(type_spec: TypeSpec, expr: str, spec: BindingSpec) -> str:
    handle = spec.handles[type_spec.handle]
    owned = "true" if type_spec.ownership == "owned" else "false"
    if handle.name in {"attribute_value", "instance_list"}:
        return f"new {handle.c_type}{{{expr}}}"
    if handle.ptr_type == "shared_ptr":
        # For shared_ptr types, we copy the shared_ptr
        return f"new {handle.c_type}{{{expr}}}"
    pointer_expr = expr
    if _normalize_cpp_type(type_spec.cpp_type).endswith("&"):
        pointer_expr = f"&({expr})"
    if _normalize_cpp_type(type_spec.cpp_type).startswith("const "):
        pointer_expr = f"const_cast<{handle.cpp_type}*>({pointer_expr})"
    return f"new {handle.c_type}{{{pointer_expr}, {owned}}}"


def _render_result_assignment(call: CallSpec, spec: BindingSpec, expr: str) -> str:
    type_spec = call.returns
    kind = type_spec.kind
    if kind == "void":
        return f"{expr};"
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][2].format(expr=expr)
    if kind == "string":
        helper = "make_static_string" if type_spec.ownership == "static" else "make_string"
        return f"*out_result = {helper}({expr});"
    if kind in _LIST_TYPE_MAP:
        return f"*out_result = {_LIST_TYPE_MAP[kind][2]}({expr});"
    if kind in _BUFFER_TYPE_MAP:
        return f"*out_result = ({expr}).data();"
    if kind == "handle":
        return f"*out_result = {_wrap_handle_expr(type_spec, expr, spec)};"
    if kind == "handle_list":
        helper = _handle_list_helper_name(spec.handles[type_spec.handle])
        return f"*out_result = {helper}({expr});"
    if kind == "opaque_ptr":
        return f"*out_result = static_cast<void*>({expr});"
    msg = f"Unsupported return kind: {kind}"
    raise ValueError(msg)


def _null_check(param_name: str, label: str) -> str:
    return f'    if ({param_name} == nullptr) {{ throw std::runtime_error("{label} \\"{param_name}\\" must not be null"); }}'


def _render_param_prelude(param: ParamSpec, spec: BindingSpec) -> str:
    type_spec = param.type
    kind = type_spec.kind
    if kind == "string":
        if type_spec.nullable:
            return f"    const char* {param.name}_str = {param.name};"
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    std::string {param.name}_cpp({param.name});"
        )
    if kind in _LIST_TYPE_MAP:
        to_cpp = _LIST_TYPE_MAP[kind][3]
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    auto {param.name}_cpp = {to_cpp}({param.name});"
        )
    if kind == "handle":
        handle = spec.handles[type_spec.handle]
        if handle.name in {"attribute_value", "instance_list"}:
            return (
                f'{_null_check(param.name, "Handle parameter")}\n'
                f"    auto {param.name}_cpp = {param.name}->value;"
            )
        cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        is_shared = handle.ptr_type == "shared_ptr"
        # Check if cpp_type refers to the shared_ptr itself (e.g., "const T::ptr &")
        refers_to_shared_ptr = is_shared and cpp_type is not None and (
            "::ptr" in cpp_type or "shared_ptr" in cpp_type
        )
        if type_spec.cpp_type is not None and cpp_type.endswith("&"):
            if refers_to_shared_ptr:
                # Reference to the shared_ptr: pass ptr member directly as reference
                value_expr = f"{param.name}->ptr"
                auto_kw = "const auto&"
            else:
                # Reference to underlying object: dereference pointer
                value_expr = f"*{param.name}->ptr"
                auto_kw = "auto&"
        elif type_spec.cpp_type is not None and not cpp_type.endswith("*") and not is_shared:
            # Value parameter (discovered, raw-ptr handle): dereference to copy
            value_expr = f"*{param.name}->ptr"
            auto_kw = "auto"
        else:
            # Pointer, shared_ptr, or unspecified (adapter): keep as-is
            value_expr = f"{param.name}->ptr"
            auto_kw = "auto"
        if type_spec.nullable:
            return (
                f"    auto {param.name}_cpp = ({param.name} != nullptr && {param.name}->ptr != nullptr) ? {value_expr} : nullptr;"
            )
        return (
            f'    if ({param.name} == nullptr || {param.name}->ptr == nullptr) {{ throw std::runtime_error("Handle parameter \\"{param.name}\\" is invalid"); }}\n'
            f"    {auto_kw} {param.name}_cpp = {value_expr};"
        )
    if kind == "handle_list":
        handle = spec.handles[type_spec.handle]
        helper_name = f"to_cpp_{_snake_name(_handle_list_c_type(handle))}"
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    auto {param.name}_cpp = {helper_name}({param.name});"
        )
    if kind == "opaque_ptr":
        cpp_type = type_spec.cpp_type
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    auto {param.name}_cpp = static_cast<{cpp_type}>({param.name});"
        )
    return ""


def _constructor_arg(p: ParamSpec) -> str:
    """Build the expression for a single constructor argument."""
    if p.type.kind == "handle":
        # Constructors take references; prelude gives us a pointer, so dereference
        return f"*{p.name}_cpp"
    if p.type.kind in _NEEDS_CONVERSION_KINDS:
        return f"{p.name}_cpp"
    return p.name


def _render_constructor(call: CallSpec, spec: BindingSpec) -> str:
    handle = spec.handles[call.returns.handle]
    cpp_class = call.cpp_class or handle.cpp_type
    arg_str = ", ".join(_constructor_arg(p) for p in call.params)
    new_expr = f"new {cpp_class}({arg_str})"

    if call.cpp_class and call.cpp_class != handle.cpp_type:
        new_expr = f"static_cast<{handle.cpp_type}*>({new_expr})"

    result_line = _render_result_assignment(call, spec, new_expr)

    if call.compile_guard:
        guard = call.compile_guard
        # Use #if defined() for all guards; compound guards (containing "defined(") are used as-is
        guard_expr = guard if "defined(" in guard else f"defined({guard})"
        return (
            f"#if {guard_expr}\n"
            f"        {result_line}\n"
            f"#else\n"
            f'        throw std::runtime_error("{call.c_name} requires {guard}");\n'
            f"#endif"
        )

    return result_line


def _call_expr_args(call: CallSpec) -> str:
    return ", ".join(
        f"{p.name}_cpp" if p.type.kind in _NEEDS_CONVERSION_KINDS else p.name
        for p in call.params
    )


def _render_call_impl(call: CallSpec, spec: BindingSpec) -> str:
    params = [f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params]
    if call.returns.kind != "void":
        params.append(f"{_out_param_type(call.returns, spec)} out_result")
    signature = f"bool {call.c_name}({', '.join(params) if params else 'void'})"

    prelude_lines = []
    if call.returns.kind != "void":
        prelude_lines.append('    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }')
    if call.receiver is not None:
        receiver_c_type = spec.handles[call.receiver].c_type
        receiver_name = "self"
        receiver_handle = spec.handles[call.receiver]
        if receiver_handle.name == "instance_list":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || !{receiver_name}->value) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto self_cpp = {receiver_name}->value;")
        elif receiver_handle.name == "attribute_value":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto& self_cpp = {receiver_name}->value;")
        elif receiver_handle.ptr_type == "shared_ptr":
            # For shared_ptr handles, check that the shared_ptr is not null and use .get()
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || {receiver_name}->ptr == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto* self_cpp = {receiver_name}->ptr.get();")
        else:
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || {receiver_name}->ptr == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto* self_cpp = {receiver_name}->ptr;")
        params = [f"{receiver_c_type}* self"] + [f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params]
        if call.returns.kind != "void":
            params.append(f"{_out_param_type(call.returns, spec)} out_result")
        signature = f"bool {call.c_name}({', '.join(params)})"

    for param in call.params:
        block = _render_param_prelude(param, spec)
        if block:
            prelude_lines.append(block)

    if call.kind in {"function", "method"}:
        call_target = f"self_cpp->{call.cpp_name}" if call.receiver is not None else call.cpp_name
        expr = f"{call_target}({_call_expr_args(call)})"
        body_line = _render_result_assignment(call, spec, expr)
    elif call.kind == "field":
        expr = f"self_cpp->{call.cpp_name}"
        # Convert std::array fields to std::vector for list returns
        cpp_type = call.returns.cpp_type or ""
        if "std::array<" in cpp_type:
            m = re.match(r".*std::array<\s*(\w+)", cpp_type)
            elem = m.group(1) if m else "double"
            expr = f"std::vector<{elem}>({expr}.begin(), {expr}.end())"
        # Add null guard for handle fields (shared_ptr can be null)
        if call.returns.kind == "handle":
            field_expr = f"self_cpp->{call.cpp_name}"
            null_guard = f'if (!{field_expr}) {{ throw std::runtime_error("{call.cpp_name} is not set"); }}\n        '
            body_line = null_guard + _render_result_assignment(call, spec, expr)
        else:
            body_line = _render_result_assignment(call, spec, expr)
    elif call.kind == "value_handle_field":
        target_handle = spec.handles[call.returns.handle]
        expr = f"std::make_shared<{target_handle.cpp_type}>(self_cpp->{call.cpp_name})"
        body_line = _render_result_assignment(call, spec, expr)
    elif call.kind == "has_field":
        body_line = f"*out_result = (self_cpp->{call.cpp_name} != nullptr);"
    elif call.kind == "children_count":
        field = call.cpp_name
        body_line = f"*out_result = self_cpp->{field}.size();"
    elif call.kind == "children_at":
        field = call.cpp_name
        body_line = (
            f'if (index >= self_cpp->{field}.size()) {{ throw std::runtime_error("Index out of bounds"); }}\n'
            f"        {_render_result_assignment(call, spec, f'self_cpp->{field}[index]')}"
        )
    elif call.kind == "children_add":
        cast_type = call.cpp_name  # stashed in cpp_name
        if cast_type:
            body_line = (
                f"auto cast_item = ifcopenshell::geometry::taxonomy::dcast<{cast_type}>(item_cpp);\n"
                f'        if (!cast_item) {{ throw std::runtime_error("Invalid item type"); }}\n'
                f"        self_cpp->children.push_back(cast_item);"
            )
        else:
            body_line = "self_cpp->children.push_back(item_cpp);"
    elif call.kind == "field_setter":
        body_line = f"self_cpp->{call.cpp_name} = value_cpp;"
    elif call.kind == "method_size":
        body_line = f"*out_result = self_cpp->{call.cpp_name}().size();"
    elif call.kind == "array_field":
        body_line = _render_result_assignment(call, spec, f"self_cpp->{call.cpp_name}")
    elif call.kind == "optional_has":
        body_line = f"*out_result = self_cpp->{call.cpp_name}.is_initialized();"
    elif call.kind == "optional_get":
        null_guard = (
            f'if (!self_cpp->{call.cpp_name}.is_initialized()) '
            f'{{ throw std::runtime_error("{call.cpp_name} is not set"); }}\n        '
        )
        body_line = null_guard + _render_result_assignment(call, spec, f"*self_cpp->{call.cpp_name}")
    elif call.kind == "as_item_cast":
        target_handle = spec.handles[call.returns.handle]
        body_line = _render_result_assignment(
            call, spec,
            f"std::static_pointer_cast<{target_handle.cpp_type}>(self->ptr)"
        )
    elif call.kind == "ccomponents_vector":
        access_via = call.cpp_name  # e.g. "ccomponents" or "data()->ccomponents"
        body_line = (
            f"const auto& v = self_cpp->{access_via}();\n"
            f"        {_render_result_assignment(call, spec, 'std::vector<double>{{v(0), v(1), v(2)}}')}"
        )
    elif call.kind == "ccomponents_matrix":
        access_via = call.cpp_name
        body_line = (
            f"std::vector<double> data(16);\n"
            f"        const auto& mat = self_cpp->{access_via}();\n"
            f"        for (int i = 0; i < 4; ++i) {{\n"
            f"            for (int j = 0; j < 4; ++j) {{\n"
            f"                data[i * 4 + j] = mat(i, j);\n"
            f"            }}\n"
            f"        }}\n"
            f"        {_render_result_assignment(call, spec, 'data')}"
        )
    elif call.kind == "variant_get":
        parts = call.cpp_name.split("|")  # method|cpp_type
        method, cpp_type = parts[0], parts[1]
        body_line = (
            f"auto val = self_cpp->{method}(name_cpp);\n"
            f"        auto* p = boost::get<{cpp_type}>(&val);\n"
            f'        if (!p) {{ throw std::runtime_error("Setting is not of expected type"); }}\n'
            f"        {_render_result_assignment(call, spec, '*p')}"
        )
    elif call.kind == "variant_set":
        parts = call.cpp_name.split("|")  # method|variant_type|cpp_type
        method, variant_type, cpp_type = parts[0], parts[1], parts[2]
        if cpp_type == "int64_t":
            value_expr = f"{variant_type}(static_cast<int64_t>(value))"
        elif cpp_type == "double":
            value_expr = f"{variant_type}(value)"
        elif cpp_type == "bool":
            value_expr = f"{variant_type}(value)"
        elif cpp_type == "std::string":
            value_expr = f"{variant_type}(value_cpp)"
        else:
            value_expr = f"{variant_type}({cpp_type}(value))"
        body_line = f"self_cpp->{method}(name_cpp, {value_expr});"
    elif call.kind == "constructor":
        body_line = _render_constructor(call, spec)
    else:
        body = call.implementation.body.rstrip()
        if call.returns.kind == "void":
            body_line = f"[&]() {{\n{body}\n        }}();"
        else:
            body_line = "auto generated_result = [&]() {\n" + body + "\n        }();\n        "
            body_line += _render_result_assignment(call, spec, "generated_result")

    prelude = "\n".join(prelude_lines)
    if prelude:
        prelude += "\n"

    return f"""{signature} {{
    try {{
        {spec.c_prefix}_clear_error();
{prelude}        {body_line}
        return true;
    }} catch (const std::exception& e) {{
        set_last_error(e.what());
        return false;
    }} catch (...) {{
        set_last_error("Unknown C++ exception");
        return false;
    }}
}}"""


def _render_cpp(spec: BindingSpec, header_name: str) -> str:
    handle_structs = []
    for handle in spec.handles.values():
        storage_type = _handle_storage_type(handle)
        if handle.name in {"attribute_value", "instance_list"}:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} value;\n}};")
        elif handle.ptr_type == "shared_ptr":
            # shared_ptr handles don't need owned field - ref counting handles ownership
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} ptr;\n}};")
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

    call_impls = "\n\n".join(_render_call_impl(call, spec) for call in (*spec.functions, *spec.methods))
    includes = "\n".join(
        f'#include {header}' if header.startswith('<') else f'#include "{header}"'
        for header in spec.public_headers
    )
    handle_structs_block = "\n\n".join(handle_structs)
    destroy_impls_block = "\n\n".join(destroy_impls)
    handle_list_types = _used_handle_list_handles(spec)
    handle_list_helpers = "\n\n".join(
        _render_handle_list_helpers(handle) for handle in handle_list_types
    )
    handle_list_destroy_impls = "\n\n".join(
        f"""void ifcopenshell_{_snake_name(_handle_list_c_type(handle))}_destroy({_handle_list_c_type(handle)}* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_{_snake_name(handle.c_type)}_destroy(value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}"""
        for handle in handle_list_types
    )

    # Only emit common type implementations if this is not a dependent module
    # MergedBindingSpec always emits common types (no depends_on_common attribute)
    depends_on_common = getattr(spec, 'depends_on_common', None)
    common_type_impls = "" if depends_on_common else COMMON_TYPE_IMPLS

    return f"""{AI_HEADER}
#include "{header_name}"

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

{includes}
#include "aggregate_of_instance.h"
#include "IfcEntityInstanceData.h"
#include "utils.h"

namespace {{
thread_local std::string g_last_error;
bool feature_use_attribute_value_derived = false;
std::stringstream ifcopenshell_log_stream;
bool g_log_stream_initialized = false;

void ensure_log_stream_initialized() {{
    if (!g_log_stream_initialized) {{
        Logger::SetOutput(nullptr, &ifcopenshell_log_stream);
        g_log_stream_initialized = true;
    }}
}}

IfcUtil::ArgumentType helper_fn_attribute_type(const IfcUtil::IfcBaseClass* inst, unsigned index) {{
    const IfcParse::parameter_type* parameter_type = nullptr;
    if (inst->declaration().as_entity()) {{
        parameter_type = inst->declaration().as_entity()->attribute_by_index(index)->type_of_attribute();
        if (inst->declaration().as_entity()->derived()[index]) {{
            return IfcUtil::Argument_DERIVED;
        }}
    }} else if (inst->declaration().as_type_declaration() && index == 0) {{
        parameter_type = inst->declaration().as_type_declaration()->declared_type();
    }} else if (inst->declaration().as_enumeration_type() && index == 0) {{
        return IfcUtil::Argument_STRING;
    }}

    if (parameter_type == nullptr) {{
        return IfcUtil::Argument_UNKNOWN;
    }}
    return IfcUtil::from_parameter_type(parameter_type);
}}

void set_last_error(const std::string& message) {{
    g_last_error = message;
}}

void validate_list_items(const char* name, const void* items, size_t size) {{
    if (size > 0 && items == nullptr) {{
        throw std::runtime_error(std::string("Parameter '") + name + "' has a null items pointer");
    }}
}}

template <typename T>
struct is_std_vector : std::false_type {{}};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {{}};

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

std::string json_escape_string(const std::string& value) {{
    std::ostringstream out;
    for (unsigned char ch : value) {{
        switch (ch) {{
        case '\\\\': out << "\\\\\\\\"; break;
        case '"': out << "\\\\\\""; break;
        case '\\b': out << "\\\\b"; break;
        case '\\f': out << "\\\\f"; break;
        case '\\n': out << "\\\\n"; break;
        case '\\r': out << "\\\\r"; break;
        case '\\t': out << "\\\\t"; break;
        default:
            if (ch < 0x20) {{
                out << "\\\\u"
                    << "00"
                    << "0123456789abcdef"[ch >> 4]
                    << "0123456789abcdef"[ch & 0x0f];
            }} else {{
                out << static_cast<char>(ch);
            }}
        }}
    }}
    return out.str();
}}

std::string json_quote(const std::string& value) {{
    return std::string(1, '"') + json_escape_string(value) + '"';
}}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier);

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier);

template <typename T>
std::string vector_to_json_string(const std::vector<T>& values, bool include_identifier) {{
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {{
        if (i != 0) {{
            out << ",";
        }}
        out << value_to_json_string(values[i], include_identifier);
    }}
    out << "]";
    return out.str();
}}

std::string reference_or_simple_type_to_json_string(const IfcParse::reference_or_simple_type& value, bool include_identifier) {{
    if (auto* instance = std::get_if<IfcUtil::IfcBaseClass*>(&value)) {{
        return *instance ? instance_to_info_json_string(*instance, include_identifier) : "null";
    }}
    auto reference = std::get<IfcParse::InstanceReference>(value);
    return std::string(R"({{"ref":)") + std::to_string(reference.v) + "}}";
}}

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier) {{
    if constexpr (is_std_vector_v<T>) {{
        return vector_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, std::string>) {{
        return json_quote(value);
    }} else if constexpr (std::is_same_v<T, const char*>) {{
        return value ? json_quote(value) : "null";
    }} else if constexpr (std::is_same_v<T, bool>) {{
        return value ? "true" : "false";
    }} else if constexpr (std::is_same_v<T, boost::logic::tribool>) {{
        if (boost::logic::indeterminate(value)) {{
            return "null";
        }}
        return value ? "true" : "false";
    }} else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {{
        std::ostringstream out;
        out << value;
        return out.str();
    }} else if constexpr (std::is_same_v<T, EnumerationReference>) {{
        return json_quote(std::string(value.value()));
    }} else if constexpr (std::is_same_v<T, IfcParse::reference_or_simple_type>) {{
        return reference_or_simple_type_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, IfcUtil::IfcBaseClass*>) {{
        return value ? instance_to_info_json_string(value, include_identifier) : "null";
    }} else if constexpr (std::is_same_v<T, aggregate_of_instance::ptr>) {{
        if (!value) {{
            return "null";
        }}
        std::ostringstream out;
        out << "[";
        for (size_t i = 0; i < value->size(); ++i) {{
            if (i != 0) {{
                out << ",";
            }}
            out << ((*value)[static_cast<int>(i)] ? instance_to_info_json_string((*value)[static_cast<int>(i)], include_identifier) : "null");
        }}
        out << "]";
        return out.str();
    }} else if constexpr (std::is_same_v<T, aggregate_of_aggregate_of_instance::ptr>) {{
        if (!value) {{
            return "null";
        }}
        std::ostringstream out;
        out << "[";
        size_t outer_index = 0;
        for (auto outer = value->begin(); outer != value->end(); ++outer, ++outer_index) {{
            if (outer_index != 0) {{
                out << ",";
            }}
            out << "[";
            for (size_t inner_index = 0; inner_index < outer->size(); ++inner_index) {{
                if (inner_index != 0) {{
                    out << ",";
                }}
                auto* instance = (*outer)[inner_index];
                out << (instance ? instance_to_info_json_string(instance, include_identifier) : "null");
            }}
            out << "]";
        }}
        out << "]";
        return out.str();
    }} else if constexpr (
        std::is_same_v<T, empty_aggregate_t> ||
        std::is_same_v<T, empty_aggregate_of_aggregate_t> ||
        std::is_same_v<T, Blank> ||
        std::is_same_v<T, Derived>) {{
        return "null";
    }} else {{
        return json_quote("<unsupported>");
    }}
}}

std::string attribute_value_to_json_string(const AttributeValue& value, bool include_identifier) {{
    return value.apply_visitor([include_identifier](const auto& inner) -> std::string {{
        return value_to_json_string(inner, include_identifier);
    }});
}}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier) {{
    if (instance == nullptr) {{
        return "null";
    }}
    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    if (instance->declaration().as_entity()) {{
        const auto attributes = instance->declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {{
            emit_field(attributes[i]->name(), attribute_value_to_json_string(instance->get_attribute_value(i), include_identifier));
        }}
        if (include_identifier) {{
            emit_field("id", std::to_string(instance->as<IfcUtil::IfcBaseEntity>()->id()));
        }}
    }} else {{
        emit_field("wrappedValue", attribute_value_to_json_string(instance->get_attribute_value(0), include_identifier));
    }}

    emit_field("type", json_quote(instance->declaration().name()));
    out << "}}";
    return out.str();
}}

std::string unresolved_reference_variant_to_json_string(
    const std::variant<
        IfcParse::reference_or_simple_type,
        std::vector<IfcParse::reference_or_simple_type>,
        std::vector<std::vector<IfcParse::reference_or_simple_type>>>& value,
    bool include_identifier) {{
    return std::visit(
        [include_identifier](const auto& inner) -> std::string {{
            return value_to_json_string(inner, include_identifier);
        }},
        value);
}}

std::string unresolved_references_to_json_string(
    const IfcParse::unresolved_references& references,
    const IfcParse::declaration* declaration) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : references) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        const auto& mutable_value = entry.first;
        out << "{{"
            << R"("entity_name":)" << mutable_value.name_
            << R"(,"attribute_index":)" << static_cast<int>(mutable_value.index_);
        if (declaration && declaration->as_entity() && mutable_value.index_ < declaration->as_entity()->attribute_count()) {{
            out << R"(,"attribute_name":)" << json_quote(declaration->as_entity()->attribute_by_index(mutable_value.index_)->name());
        }}
        out << R"(,"value":)" << unresolved_reference_variant_to_json_string(entry.second, true) << "}}";
    }}
    out << "]";
    return out.str();
}}

template <typename Inverses>
std::string inverses_to_json_string(const Inverses& inverses) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : inverses) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << "{{"
            << R"("instance_id":)" << std::get<0>(entry.first)
            << R"(,"instance_type":)" << std::get<1>(entry.first)
            << R"(,"attribute_index":)" << std::get<2>(entry.first)
            << R"(,"referencing_ids":)" << vector_to_json_string(entry.second, true)
            << "}}";
    }}
    out << "]";
    return out.str();
}}

std::string instance_stream_read_instance_json(IfcParse::InstanceStreamer* streamer, bool type_as_declaration_instance) {{
    if (!(*streamer)) {{
        return "null";
    }}
    auto inst = streamer->readInstance();
    if (!inst) {{
        return "null";
    }}

    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    emit_field("id", std::to_string(std::get<0>(*inst)));
    if (type_as_declaration_instance) {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }} else {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }}

    const auto* declaration = std::get<1>(*inst);
    const auto& data = std::get<2>(*inst);
    if (declaration->as_entity()) {{
        for (size_t i = 0; i < declaration->as_entity()->attribute_count(); ++i) {{
            emit_field(
                declaration->as_entity()->attribute_by_index(i)->name(),
                attribute_value_to_json_string(data.get_attribute_value(nullptr, declaration, 0, i), true));
        }}
    }}

    for (const auto& reference : streamer->references()) {{
        std::string key = std::to_string(reference.first.index_);
        if (declaration->as_entity() && reference.first.index_ < declaration->as_entity()->attribute_count()) {{
            key = declaration->as_entity()->attribute_by_index(reference.first.index_)->name();
        }}
        emit_field(key, unresolved_reference_variant_to_json_string(reference.second, true));
    }}

    streamer->references().clear();
    streamer->inverses().clear();
    out << "}}";
    return out.str();
}}

ifcopenshell_string_t make_string(const std::string& value) {{
    char* data = new char[value.size() + 1];
    std::memcpy(data, value.c_str(), value.size() + 1);
    return ifcopenshell_string_t{{data, value.size(), true}};
}}

ifcopenshell_string_t make_static_string(const char* value) {{
    if (value == nullptr) {{
        return ifcopenshell_string_t{{nullptr, 0, false}};
    }}
    return ifcopenshell_string_t{{const_cast<char*>(value), std::strlen(value), false}};
}}

ifcopenshell_string_list_t make_string_list(const std::vector<std::string>& values) {{
    auto* items = new ifcopenshell_string_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_string(values[i]);
    }}
    return ifcopenshell_string_list_t{{items, values.size()}};
}}

std::vector<std::string> to_cpp_string_list(const ifcopenshell_string_list_t* value) {{
    validate_list_items("string_list", value->items, value->size);
    std::vector<std::string> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        const auto& item = value->items[i];
        if (item.data == nullptr && item.size > 0) {{
            throw std::runtime_error("string_list contains a null string buffer");
        }}
        result.emplace_back(item.data == nullptr ? "" : item.data, item.size);
    }}
    return result;
}}

ifcopenshell_bool_list_t make_bool_list(const std::vector<bool>& values) {{
    bool* items = values.empty() ? nullptr : new bool[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = values[i];
    }}
    return ifcopenshell_bool_list_t{{items, values.size()}};
}}

std::vector<bool> to_cpp_bool_list(const ifcopenshell_bool_list_t* value) {{
    validate_list_items("bool_list", value->items, value->size);
    std::vector<bool> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(value->items[i]);
    }}
    return result;
}}

ifcopenshell_int32_list_t make_int32_list(const std::vector<int>& values) {{
    int32_t* items = values.empty() ? nullptr : new int32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = static_cast<int32_t>(values[i]);
    }}
    return ifcopenshell_int32_list_t{{items, values.size()}};
}}

std::vector<int> to_cpp_int32_list(const ifcopenshell_int32_list_t* value) {{
    validate_list_items("int32_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<int>(value->items, value->items + value->size);
}}

ifcopenshell_uint32_list_t make_uint32_list(const std::vector<unsigned int>& values) {{
    uint32_t* items = values.empty() ? nullptr : new uint32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = static_cast<uint32_t>(values[i]);
    }}
    return ifcopenshell_uint32_list_t{{items, values.size()}};
}}

std::vector<unsigned int> to_cpp_uint32_list(const ifcopenshell_uint32_list_t* value) {{
    validate_list_items("uint32_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<unsigned int>(value->items, value->items + value->size);
}}

ifcopenshell_int32_list_list_t make_int32_list_list(const std::vector<std::vector<int>>& values) {{
    auto* items = values.empty() ? nullptr : new ifcopenshell_int32_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_int32_list(values[i]);
    }}
    return ifcopenshell_int32_list_list_t{{items, values.size()}};
}}

std::vector<std::vector<int>> to_cpp_int32_list_list(const ifcopenshell_int32_list_list_t* value) {{
    validate_list_items("int32_list_list", value->items, value->size);
    std::vector<std::vector<int>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(to_cpp_int32_list(&value->items[i]));
    }}
    return result;
}}

ifcopenshell_double_list_t make_double_list(const std::vector<double>& values) {{
    double* items = values.empty() ? nullptr : new double[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = values[i];
    }}
    return ifcopenshell_double_list_t{{items, values.size()}};
}}

std::vector<double> to_cpp_double_list(const ifcopenshell_double_list_t* value) {{
    validate_list_items("double_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<double>(value->items, value->items + value->size);
}}

ifcopenshell_double_list_list_t make_double_list_list(const std::vector<std::vector<double>>& values) {{
    auto* items = values.empty() ? nullptr : new ifcopenshell_double_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_double_list(values[i]);
    }}
    return ifcopenshell_double_list_list_t{{items, values.size()}};
}}

std::vector<std::vector<double>> to_cpp_double_list_list(const ifcopenshell_double_list_list_t* value) {{
    validate_list_items("double_list_list", value->items, value->size);
    std::vector<std::vector<double>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(to_cpp_double_list(&value->items[i]));
    }}
    return result;
}}

template <typename T>
void set_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index, const T& value) {{
    instance->set_attribute_value(index, value);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {{
        entity->populate_derived();
    }}
}}

void unset_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index) {{
    instance->unset_attribute_value(index);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {{
        entity->populate_derived();
    }}
}}

void set_instance_attribute_from_attribute_value(IfcUtil::IfcBaseClass* instance, size_t index, const AttributeValue& value) {{
    value.apply_visitor([&](const auto& inner) -> void {{
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, Derived>) {{
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        }} else {{
            set_instance_argument(instance, index, inner);
        }}
    }});
}}
}} // namespace

{handle_structs_block}

{handle_list_helpers}

{common_type_impls}

void {spec.c_prefix}_clear_error(void) {{
    g_last_error.clear();
}}

const char* {spec.c_prefix}_last_error_message(void) {{
    return g_last_error.c_str();
}}

{destroy_impls_block}
{handle_list_destroy_impls}

{call_impls}
"""


def generate(spec_path: Path, header_out: Path, cpp_out: Path, compile_commands_path: Path | None = None) -> None:
    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands_path)
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")


def generate_merged(
    spec_paths: list[Path],
    module: str,
    c_prefix: str,
    header_out: Path,
    cpp_out: Path,
    compile_commands_path: Path | None = None,
) -> None:
    """Generate bindings from multiple specs merged together."""
    spec = load_merged_specs(spec_paths, module, c_prefix, compile_commands_path=compile_commands_path)
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate the first C backend skeleton from a handwritten binding spec.")
    parser.add_argument("--spec", type=Path, action="append", required=True, 
                        help="Path to a binding spec YAML. Can be specified multiple times for merged output.")
    parser.add_argument("--header-out", type=Path, required=True, help="Output path for the generated C header.")
    parser.add_argument("--cpp-out", type=Path, required=True, help="Output path for the generated C++ glue source.")
    parser.add_argument(
        "--compile-commands",
        type=Path,
        default=None,
        help="Optional path to compile_commands.json for AST-backed source discovery.",
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
        generate(args.spec[0], args.header_out, args.cpp_out, compile_commands_path=args.compile_commands)
    else:
        # Multiple specs - merge them
        generate_merged(
            args.spec,
            args.module,
            args.c_prefix,
            args.header_out,
            args.cpp_out,
            compile_commands_path=args.compile_commands,
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
