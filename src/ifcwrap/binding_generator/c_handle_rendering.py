# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

try:
    from .authored_spec import HandleSpec, TypeSpec
    from .binding_ir import BindingIR
    from .c_type_rendering import _normalize_cpp_type, _qualify_handle_cpp_fragment
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import HandleSpec, TypeSpec
    from binding_ir import BindingIR
    from c_type_rendering import _normalize_cpp_type, _qualify_handle_cpp_fragment


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
    if handle.destructor.startswith("function:"):
        destructor = handle.destructor[len("function:") :].strip()
        return f"if (handle->owned && handle->ptr) {{ {destructor}(handle->ptr); }}\n    delete handle;"
    if handle.destructor == "shared_ptr":
        return "handle->ptr.reset();\n    delete handle;"
    if handle.destructor == "delete":
        return "if (handle->owned && handle->ptr) { delete handle->ptr; }\n    delete handle;"
    return "delete handle;"


def _wrap_handle_expr(type_spec: TypeSpec, expr: str, spec: BindingIR) -> str:
    handle = spec.handles[type_spec.handle]
    owned = "true" if type_spec.ownership == "owned" else "false"
    if handle.name in {"attribute_value", "instance_list"}:
        return f"new {handle.c_type}{{{expr}}}"
    if handle.ptr_type == "shared_ptr":
        return f"new {handle.c_type}{{{expr}}}"
    normalized_cpp_type = _normalize_cpp_type(type_spec.cpp_type)
    if normalized_cpp_type.startswith("std::unique_ptr<"):
        return f"new {handle.c_type}{{{expr}.release(), true}}"
    pointer_expr = expr
    if normalized_cpp_type.endswith("&"):
        pointer_expr = f"&({expr})"
    if normalized_cpp_type.startswith("const "):
        cast_target = normalized_cpp_type
        while cast_target.startswith("const "):
            cast_target = cast_target[len("const ") :].strip()
        while cast_target.endswith("&") or cast_target.endswith("*"):
            cast_target = cast_target[:-1].strip()
        cast_target = _qualify_handle_cpp_fragment(cast_target, handle.cpp_type)
        pointer_expr = f"const_cast<{cast_target}*>({pointer_expr})"
    return f"new {handle.c_type}{{{pointer_expr}, {owned}}}"
