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


_CLEAR_ERROR_NAME = "{clear_error}"
_LAST_ERROR_NAME = "{last_error}"
_STRING_DESTROY_NAME = "ifcopenshell_string_destroy"
_STRING_LIST_DESTROY_NAME = "ifcopenshell_string_list_destroy"
_BOOL_LIST_DESTROY_NAME = "ifcopenshell_bool_list_destroy"
_INT32_LIST_DESTROY_NAME = "ifcopenshell_int32_list_destroy"
_UINT32_LIST_DESTROY_NAME = "ifcopenshell_uint32_list_destroy"
_DOUBLE_LIST_DESTROY_NAME = "ifcopenshell_double_list_destroy"
_INT32_LIST_LIST_DESTROY_NAME = "ifcopenshell_int32_list_list_destroy"
_INT32_LIST_LIST_LIST_DESTROY_NAME = "ifcopenshell_int32_list_list_list_destroy"
_DOUBLE_LIST_LIST_DESTROY_NAME = "ifcopenshell_double_list_list_destroy"


def encode_string(value):
    if value is None:
        return None
    if isinstance(value, bytes):
        return value
    return str(value).encode("utf-8")


def make_string(value):
    data = encode_string(value) or b""
    buffer = ctypes.create_string_buffer(data)
    result = ifcopenshell_string_t()
    result.data = ctypes.cast(buffer, ctypes.c_void_p)
    result.size = len(data)
    result.owned = False
    result._keepalive = buffer  # type: ignore[attr-defined]
    return result


def make_string_list(values):
    encoded = [encode_string(value) or b"" for value in values]
    buffers = [ctypes.create_string_buffer(value) for value in encoded]
    items = (ifcopenshell_string_t * len(buffers))()
    for index, buffer in enumerate(buffers):
        items[index].data = ctypes.cast(buffer, ctypes.c_void_p)
        items[index].size = len(buffer.value)
        items[index].owned = False
    result = ifcopenshell_string_list_t()
    result.items = items
    result.size = len(buffers)
    result._keepalive = (items, buffers)  # type: ignore[attr-defined]
    return result


def make_bool_list(values):
    values = list(values)
    items = (ctypes.c_bool * len(values))(*[bool(value) for value in values])
    result = ifcopenshell_bool_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = items  # type: ignore[attr-defined]
    return result


def make_int32_list(values):
    values = list(values)
    items = (ctypes.c_int32 * len(values))(*[int(value) for value in values])
    result = ifcopenshell_int32_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = items  # type: ignore[attr-defined]
    return result


def make_int64_list(values):
    values = list(values)
    items = (ctypes.c_int64 * len(values))(*[int(value) for value in values])
    result = ifcopenshell_int64_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = items  # type: ignore[attr-defined]
    return result


def make_uint32_list(values):
    values = list(values)
    items = (ctypes.c_uint32 * len(values))(*[int(value) for value in values])
    result = ifcopenshell_uint32_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = items  # type: ignore[attr-defined]
    return result


def make_double_list(values):
    values = list(values)
    items = (ctypes.c_double * len(values))(*[float(value) for value in values])
    result = ifcopenshell_double_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = items  # type: ignore[attr-defined]
    return result


def make_int32_list_list(values):
    rows = [list(row) for row in values]
    row_buffers = [(ctypes.c_int32 * len(row))(*[int(value) for value in row]) for row in rows]
    items = (ifcopenshell_int32_list_t * len(row_buffers))()
    for index, buffer in enumerate(row_buffers):
        items[index].items = buffer
        items[index].size = len(buffer)
    result = ifcopenshell_int32_list_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, row_buffers)  # type: ignore[attr-defined]
    return result


def make_double_list_list(values):
    rows = [list(row) for row in values]
    row_buffers = [(ctypes.c_double * len(row))(*[float(value) for value in row]) for row in rows]
    items = (ifcopenshell_double_list_t * len(row_buffers))()
    for index, buffer in enumerate(row_buffers):
        items[index].items = buffer
        items[index].size = len(buffer)
    result = ifcopenshell_double_list_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, row_buffers)  # type: ignore[attr-defined]
    return result


def last_error(lib, default="Unknown error"):
    fn = getattr(lib, _LAST_ERROR_NAME, None)
    if fn is None:
        return default
    raw = fn()
    if not raw:
        return default
    return raw.decode("utf-8", errors="replace")


def status_or_raise(lib, status, fallback):
    if status:
        return True
    raise RuntimeError(last_error(lib, fallback))


def call_string(lib, fn, *args, decode=True, value_type=None):
    if value_type is None:
        value_type = ifcopenshell_string_t
    value = value_type()
    if not fn(*args, ctypes.byref(value)):
        return None
    return take_string(lib, value, decode=decode)


def call_string_list(lib, fn, *args, decode=True, value_type=None):
    if value_type is None:
        value_type = ifcopenshell_string_list_t
    value = value_type()
    if not fn(*args, ctypes.byref(value)):
        return None
    return take_string_list(lib, value, decode=decode)


def call_int32_list(lib, fn, *args):
    value = ifcopenshell_int32_list_t()
    if not fn(*args, ctypes.byref(value)):
        return None
    return take_int32_list(lib, value)


def call_uint32_list(lib, fn, *args):
    value = ifcopenshell_uint32_list_t()
    if not fn(*args, ctypes.byref(value)):
        return None
    return take_uint32_list(lib, value)


def call_double_list(lib, fn, *args):
    value = ifcopenshell_double_list_t()
    if not fn(*args, ctypes.byref(value)):
        return None
    return take_double_list(lib, value)


def call_string_or_raise(lib, fn, fallback, *args, decode=True, value_type=None):
    result = call_string(lib, fn, *args, decode=decode, value_type=value_type)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def call_string_list_or_raise(lib, fn, fallback, *args, decode=True, value_type=None):
    result = call_string_list(lib, fn, *args, decode=decode, value_type=value_type)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def call_int32_list_or_raise(lib, fn, fallback, *args):
    result = call_int32_list(lib, fn, *args)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def call_uint32_list_or_raise(lib, fn, fallback, *args):
    result = call_uint32_list(lib, fn, *args)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def call_double_list_or_raise(lib, fn, fallback, *args):
    result = call_double_list(lib, fn, *args)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def call_scalar(fn, c_type, *args):
    value = c_type()
    if not fn(*args, ctypes.byref(value)):
        return None
    return value.value


def call_scalar_or_raise(lib, fn, c_type, fallback, *args):
    result = call_scalar(fn, c_type, *args)
    if result is None:
        status_or_raise(lib, False, fallback)
    return result


def take_nullable_handle(lib, value, *, destroy=None):
    if not value:
        return None
    if not value.contents.ptr:
        if destroy is not None:
            destroy_fn = getattr(lib, destroy) if isinstance(destroy, str) else destroy
            destroy_fn(value)
        return None
    return ctypes.cast(value, ctypes.c_void_p).value


def call_handle(lib, fn, *args, destroy=None, handle_pointer_type=None):
    if handle_pointer_type is None:
        handle_pointer_type = ctypes.POINTER(_HandleStruct)
    out = handle_pointer_type()
    if not fn(*args, ctypes.byref(out)):
        return None
    return take_nullable_handle(lib, out, destroy=destroy)


def call_handle_or_raise(lib, fn, fallback, *args, destroy=None, handle_pointer_type=None):
    if handle_pointer_type is None:
        handle_pointer_type = ctypes.POINTER(_HandleStruct)
    out = handle_pointer_type()
    status_or_raise(lib, fn(*args, ctypes.byref(out)), fallback)
    return take_nullable_handle(lib, out, destroy=destroy)


def take_string(lib, value, *, decode=True):
    try:
        if value.data and value.size:
            raw = ctypes.string_at(value.data, value.size)
            return raw.decode("utf-8") if decode else raw
        return "" if decode else b""
    finally:
        getattr(lib, _STRING_DESTROY_NAME)(ctypes.byref(value))


def take_string_list(lib, value, *, decode=True):
    try:
        result = []
        for index in range(value.size):
            item = value.items[index]
            raw = ctypes.string_at(item.data, item.size) if item.data and item.size else b""
            result.append(raw.decode("utf-8") if decode else raw)
        return tuple(result)
    finally:
        getattr(lib, _STRING_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_bool_list(lib, value):
    try:
        return tuple(bool(value.items[index]) for index in range(value.size)) if value.items else tuple()
    finally:
        getattr(lib, _BOOL_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_int32_list(lib, value):
    try:
        return tuple(int(value.items[index]) for index in range(value.size)) if value.items else tuple()
    finally:
        getattr(lib, _INT32_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_uint32_list(lib, value):
    try:
        return tuple(int(value.items[index]) for index in range(value.size)) if value.items else tuple()
    finally:
        getattr(lib, _UINT32_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_double_list(lib, value):
    try:
        return tuple(float(value.items[index]) for index in range(value.size)) if value.items else tuple()
    finally:
        getattr(lib, _DOUBLE_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_int32_list_list(lib, value):
    try:
        return (
            tuple(tuple(int(value.items[i].items[j]) for j in range(value.items[i].size)) for i in range(value.size))
            if value.items
            else tuple()
        )
    finally:
        getattr(lib, _INT32_LIST_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_int32_list_list_list(lib, value):
    try:
        return (
            tuple(
                tuple(tuple(int(value.items[i].items[j].items[k]) for k in range(value.items[i].items[j].size)) for j in range(value.items[i].size))
                for i in range(value.size)
            )
            if value.items
            else tuple()
        )
    finally:
        getattr(lib, _INT32_LIST_LIST_LIST_DESTROY_NAME)(ctypes.byref(value))


def take_double_list_list(lib, value):
    try:
        return (
            tuple(tuple(float(value.items[i].items[j]) for j in range(value.items[i].size)) for i in range(value.size))
            if value.items
            else tuple()
        )
    finally:
        getattr(lib, _DOUBLE_LIST_LIST_DESTROY_NAME)(ctypes.byref(value))


def move_handle_list(lib, value, destroy, handle_pointer_type):
    destroy_fn = getattr(lib, destroy) if isinstance(destroy, str) else destroy
    try:
        moved_addrs = []
        null = handle_pointer_type()
        for index in range(value.size):
            slot = value.items[index]
            moved_addrs.append(ctypes.addressof(slot.contents) if slot else 0)
            value.items[index] = null
        return tuple(
            ctypes.cast(ctypes.c_void_p(addr), handle_pointer_type) if addr else handle_pointer_type()
            for addr in moved_addrs
        )
    finally:
        destroy_fn(ctypes.byref(value))


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
