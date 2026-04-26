# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
import re

try:
    from .binding_ir import BindingIR
    from .host_metadata import HostBindingMetadata, HostFunctionMetadata, HostStructMetadata, build_host_metadata
except ImportError:  # pragma: no cover - script execution fallback
    from binding_ir import BindingIR
    from host_metadata import HostBindingMetadata, HostFunctionMetadata, HostStructMetadata, build_host_metadata


_SCALAR_CTYPES = {
    "bool": "ctypes.c_bool",
    "char*": "ctypes.c_char_p",
    "const char*": "ctypes.c_char_p",
    "double": "ctypes.c_double",
    "int32_t": "ctypes.c_int32",
    "int64_t": "ctypes.c_int64",
    "size_t": "ctypes.c_size_t",
    "uint8_t": "ctypes.c_uint8",
    "uint32_t": "ctypes.c_uint32",
    "void": "None",
    "void*": "ctypes.c_void_p",
    "const void*": "ctypes.c_void_p",
}


def _normalize_c_type(c_type: str) -> str:
    return " ".join(c_type.replace(" *", "*").split())


def _base_pointer_type(c_type: str) -> tuple[str, int]:
    normalized = _normalize_c_type(c_type)
    pointer_depth = 0
    while normalized.endswith("*"):
        pointer_depth += 1
        normalized = normalized[:-1].strip()
    normalized = normalized.removeprefix("const ").strip()
    return normalized, pointer_depth


def _is_generated_struct_c_type(c_type: str) -> bool:
    return re.fullmatch(r"ifcopenshell_[A-Za-z0-9_]+_t", c_type) is not None


def _ctype_expr(c_type: str, *, generic_handle_types: set[str] | None = None) -> str:
    generic_handle_types = generic_handle_types or set()
    normalized = _normalize_c_type(c_type)
    if normalized in _SCALAR_CTYPES:
        return _SCALAR_CTYPES[normalized]
    base, pointer_depth = _base_pointer_type(normalized)
    if pointer_depth == 0:
        if base in _SCALAR_CTYPES:
            return _SCALAR_CTYPES[base]
        if _is_generated_struct_c_type(base):
            return "_HandleStruct" if base in generic_handle_types else base
        raise ValueError(f"Unsupported C type for ctypes backend: {c_type}")

    if base == "void":
        expr = "ctypes.c_void_p"
        pointer_depth -= 1
    elif base == "char":
        expr = "ctypes.c_char_p"
        pointer_depth -= 1
    elif base in _SCALAR_CTYPES:
        expr = _SCALAR_CTYPES[base]
    elif _is_generated_struct_c_type(base):
        expr = "_HandleStruct" if base in generic_handle_types else base
    else:
        raise ValueError(f"Unsupported pointer base for ctypes backend: {c_type}")

    for _ in range(pointer_depth):
        expr = f"ctypes.POINTER({expr})"
    return expr


def _field_ctype_expr(c_type: str, *, generic_handle_types: set[str]) -> str:
    normalized = _normalize_c_type(c_type)
    if normalized in {"char*", "const char*"}:
        return "ctypes.c_void_p"
    return _ctype_expr(c_type, generic_handle_types=generic_handle_types)


def _class_name(c_type: str) -> str:
    return c_type


def _render_struct(struct: HostStructMetadata, *, generic_handle_types: set[str]) -> str:
    name = _class_name(struct.c_type)
    if struct.c_type in generic_handle_types:
        return f"{name} = _HandleStruct\n"
    if not struct.fields:
        return f"class {name}(ctypes.Structure):\n    pass\n"
    fields = ",\n        ".join(
        f'("{field.name}", {_field_ctype_expr(field.c_type, generic_handle_types=generic_handle_types)})'
        for field in struct.fields
    )
    return f"class {name}(ctypes.Structure):\n    _fields_ = [\n        {fields},\n    ]\n"


def _render_function_signature(function: HostFunctionMetadata, *, generic_handle_types: set[str]) -> str:
    argtypes = ", ".join(_ctype_expr(param.c_type, generic_handle_types=generic_handle_types) for param in function.params)
    return (
        f'    "{function.c_name}": ('
        f"{_ctype_expr(function.restype, generic_handle_types=generic_handle_types)}, "
        f"[{argtypes}]"
        f"),"
    )


def _requires_generic_handle_alias(metadata: HostBindingMetadata) -> bool:
    return any(struct.kind == "handle" for struct in metadata.handles.values())


def _struct_sort_key(struct: HostStructMetadata) -> tuple[int, int, str]:
    if struct.kind == "handle":
        return (0, 0, struct.c_type)
    if struct.sequence_depth == 0:
        return (1, 0, struct.c_type)
    return (2, struct.sequence_depth, struct.c_type)


def render_python_ctypes(metadata: HostBindingMetadata, *, generic_handles: bool = False) -> str:
    structs = {struct.c_type: struct for struct in (*metadata.handles.values(), *metadata.value_types.values())}
    generic_handle_types = {struct.c_type for struct in metadata.handles.values()} if generic_handles else set()
    handle_base = ""
    if generic_handles and _requires_generic_handle_alias(metadata):
        handle_base = '''class _HandleStruct(ctypes.Structure):
    _fields_ = [
        ("ptr", ctypes.c_void_p),
        ("owned", ctypes.c_bool),
    ]
'''
    rendered_structs = "\n\n".join(
        _render_struct(struct, generic_handle_types=generic_handle_types)
        for struct in sorted(structs.values(), key=_struct_sort_key)
    )
    signatures = "\n".join(
        _render_function_signature(fn, generic_handle_types=generic_handle_types) for fn in metadata.functions.values()
    )
    destroy_signatures: list[str] = []
    for struct in structs.values():
        if struct.destroy_function is None:
            continue
        destroy_signatures.append(
            f'    "{struct.destroy_function}": (None, [{_ctype_expr(struct.c_type + "*", generic_handle_types=generic_handle_types)}]),'
        )
    destroy_block = "\n".join(sorted(set(destroy_signatures)))
    clear_error = metadata.error_functions["clear_error"]
    last_error = metadata.error_functions["last_error_message"]

    return f'''# This file was generated by IfcOpenShell's binding generator. Do not edit manually.

from __future__ import annotations

import ctypes


{handle_base}

{rendered_structs}


FUNCTION_SIGNATURES = {{
{signatures}
{destroy_block}
    "{clear_error}": (None, []),
    "{last_error}": (ctypes.c_char_p, []),
}}


def bind(lib, *, strict=True, names=None, prefixes=None):
    selected_names = set(names or ())
    selected_prefixes = tuple(prefixes or ())
    for name, (restype, argtypes) in FUNCTION_SIGNATURES.items():
        if selected_names or selected_prefixes:
            if name not in selected_names and not any(name.startswith(prefix) for prefix in selected_prefixes):
                continue
        try:
            fn = getattr(lib, name)
        except AttributeError:
            if strict:
                raise
            continue
        fn.restype = restype
        fn.argtypes = argtypes
    return lib
'''


def generate_python_ctypes(ir: BindingIR, output_path: Path, *, generic_handles: bool = False) -> None:
    output_path.write_text(render_python_ctypes(build_host_metadata(ir), generic_handles=generic_handles), encoding="utf-8")
