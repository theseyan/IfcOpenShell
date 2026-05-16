# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
import re

try:
    from .binding_ir import BindingIR, CallIR
    from .binding_model import HandleSpec, TypeSpec
except ImportError:  # pragma: no cover - script execution fallback
    from binding_ir import BindingIR, CallIR
    from binding_model import HandleSpec, TypeSpec


@dataclass(frozen=True)
class HostStructField:
    name: str
    c_type: str


@dataclass(frozen=True)
class HostStructMetadata:
    c_type: str
    kind: str
    fields: tuple[HostStructField, ...]
    destroy_function: str | None
    element_type: str | None = None
    sequence_depth: int = 0
    layout: str = "transparent"


@dataclass(frozen=True)
class HostParamMetadata:
    name: str
    c_type: str
    role: str
    type_kind: str
    nullable: bool = False


@dataclass(frozen=True)
class HostFunctionMetadata:
    c_name: str
    restype: str
    params: tuple[HostParamMetadata, ...]
    error_policy: str
    returns: TypeSpec
    receiver: str | None


@dataclass(frozen=True)
class HostBindingMetadata:
    module: str
    c_prefix: str
    handles: dict[str, HostStructMetadata]
    value_types: dict[str, HostStructMetadata]
    functions: dict[str, HostFunctionMetadata]
    error_functions: dict[str, str]


_SCALAR_PARAM_TYPES = {
    "bool": "bool",
    "logical": "ifcopenshell_logical_t",
    "int32": "int32_t",
    "int64": "int64_t",
    "double": "double",
    "uint32": "uint32_t",
    "size": "size_t",
}

_SCALAR_OUT_TYPES = {
    "bool": "bool*",
    "logical": "ifcopenshell_logical_t*",
    "int32": "int32_t*",
    "int64": "int64_t*",
    "double": "double*",
    "uint32": "uint32_t*",
    "size": "size_t*",
}

_BUFFER_TYPES = {
    "double_buffer": ("const double*", "const double**"),
    "int32_buffer": ("const int32_t*", "const int32_t**"),
}

_SEQUENCE_LEAF_C_TYPE = {
    "string": "ifcopenshell_string_t",
    "bool": "bool",
    "int32": "int32_t",
    "int64": "int64_t",
    "uint8": "uint8_t",
    "uint32": "uint32_t",
    "double": "double",
}


def _sequence_kind_parts(kind: str) -> tuple[str, int] | None:
    match = re.fullmatch(r"([a-z0-9]+)((?:_list)+)", kind)
    if match is None:
        return None
    leaf = match.group(1)
    depth = match.group(2).count("_list")
    if leaf not in _SEQUENCE_LEAF_C_TYPE:
        return None
    return leaf, depth


def _sequence_c_type(kind: str) -> str:
    return f"ifcopenshell_{kind}_t"


def _sequence_prev_kind(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    if depth <= 1:
        raise ValueError(f"{kind} has no previous sequence kind")
    return f"{leaf}{'_list' * (depth - 1)}"


def _sequence_items_c_type(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    if depth == 1:
        return _SEQUENCE_LEAF_C_TYPE[leaf]
    return _sequence_c_type(_sequence_prev_kind(kind))


def _snake_name(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def _handle_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_t"


def _handle_list_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_list_t"


def _handle_destroy_name(handle: HandleSpec) -> str:
    return f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"


def _handle_list_destroy_name(handle: HandleSpec) -> str:
    return f"ifcopenshell_{_snake_name(_handle_list_c_type(handle))}_destroy"


def _handle_list_list_destroy_name(handle: HandleSpec) -> str:
    return f"ifcopenshell_{_snake_name(_handle_list_list_c_type(handle))}_destroy"


def _sequence_destroy_name(kind: str) -> str:
    return f"ifcopenshell_{kind}_destroy"


def _type_spec_sequence_kind(type_spec: TypeSpec) -> str | None:
    if type_spec.sequence_depth <= 0 or type_spec.kind == "handle":
        return None
    return f"{type_spec.kind}{'_list' * type_spec.sequence_depth}"


def _used_scalar_sequence_kinds(ir: BindingIR) -> tuple[str, ...]:
    seen: set[str] = set()
    ordered: list[str] = []

    def add_type(type_spec: TypeSpec) -> None:
        kind = _type_spec_sequence_kind(type_spec)
        if kind is None:
            return
        parts = _sequence_kind_parts(kind)
        if parts is None:
            return
        leaf, depth = parts
        for current_depth in range(1, depth + 1):
            current = f"{leaf}{'_list' * current_depth}"
            if current not in seen:
                seen.add(current)
                ordered.append(current)

    for call in (*ir.functions, *ir.methods):
        add_type(call.returns)
        for param in call.params:
            add_type(param.type)
    for struct in ir.result_structs.values():
        for field in struct.fields:
            add_type(field.type)
    return tuple(ordered)


def _used_handle_list_handles(ir: BindingIR) -> tuple[HandleSpec, ...]:
    seen: set[str] = set()
    handles: list[HandleSpec] = []

    def add(type_spec: TypeSpec) -> None:
        if type_spec.kind != "handle" or type_spec.sequence_depth == 0 or type_spec.handle is None:
            return
        if type_spec.handle in seen:
            return
        seen.add(type_spec.handle)
        handles.append(ir.handles[type_spec.handle])

    for call in (*ir.functions, *ir.methods):
        add(call.returns)
        for param in call.params:
            add(param.type)
    for struct in ir.result_structs.values():
        for field in struct.fields:
            add(field.type)
    return tuple(handles)


def _param_c_type(type_spec: TypeSpec, ir: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return f"const {_sequence_c_type(sequence_kind)}*"
    if type_spec.kind in _SCALAR_PARAM_TYPES:
        return _SCALAR_PARAM_TYPES[type_spec.kind]
    if type_spec.kind == "string":
        return "const char*"
    if type_spec.kind in _BUFFER_TYPES:
        return _BUFFER_TYPES[type_spec.kind][0]
    if type_spec.kind == "handle":
        if type_spec.handle is None:
            raise ValueError("handle type is missing handle name")
        handle = ir.handles[type_spec.handle]
        if type_spec.sequence_depth == 1:
            return f"const {_handle_list_c_type(handle)}*"
        if type_spec.sequence_depth == 2:
            return f"const {_handle_list_list_c_type(handle)}*"
        return f"{handle.c_type}*"
    if type_spec.kind == "opaque_ptr":
        return "void*"
    if type_spec.kind == "struct":
        if type_spec.struct is None:
            raise ValueError("struct type is missing struct name")
        return ir.result_structs[type_spec.struct].c_type
    raise ValueError(f"Unsupported parameter kind: {type_spec.kind}")


def _out_c_type(type_spec: TypeSpec, ir: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return f"{_sequence_c_type(sequence_kind)}*"
    if type_spec.kind in _SCALAR_OUT_TYPES:
        return _SCALAR_OUT_TYPES[type_spec.kind]
    if type_spec.kind == "string":
        return "ifcopenshell_string_t*"
    if type_spec.kind in _BUFFER_TYPES:
        return _BUFFER_TYPES[type_spec.kind][1]
    if type_spec.kind == "handle":
        if type_spec.handle is None:
            raise ValueError("handle type is missing handle name")
        handle = ir.handles[type_spec.handle]
        if type_spec.sequence_depth == 1:
            return f"{_handle_list_c_type(handle)}*"
        if type_spec.sequence_depth == 2:
            return f"{_handle_list_list_c_type(handle)}*"
        return f"{handle.c_type}**"
    if type_spec.kind == "opaque_ptr":
        return "void**"
    if type_spec.kind == "struct":
        if type_spec.struct is None:
            raise ValueError("struct type is missing struct name")
        return f"{ir.result_structs[type_spec.struct].c_type}*"
    raise ValueError(f"Unsupported return kind: {type_spec.kind}")


def _field_c_type(type_spec: TypeSpec, ir: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return _sequence_c_type(sequence_kind)
    if type_spec.kind in _SCALAR_PARAM_TYPES:
        return _SCALAR_PARAM_TYPES[type_spec.kind]
    if type_spec.kind == "string":
        return "ifcopenshell_string_t"
    if type_spec.kind == "handle":
        if type_spec.handle is None:
            raise ValueError("handle type is missing handle name")
        handle = ir.handles[type_spec.handle]
        if type_spec.sequence_depth == 1:
            return _handle_list_c_type(handle)
        if type_spec.sequence_depth == 2:
            return _handle_list_list_c_type(handle)
        return f"{handle.c_type}*"
    if type_spec.kind == "opaque_ptr":
        return "void*"
    raise ValueError(f"Unsupported result struct field kind: {type_spec.kind}")


def _host_structs_for_handles(ir: BindingIR) -> dict[str, HostStructMetadata]:
    result: dict[str, HostStructMetadata] = {}
    for name, handle in sorted(ir.handles.items()):
        if handle.ptr_type == "shared_ptr":
            fields: tuple[HostStructField, ...] = ()
            layout = "opaque"
        else:
            fields = (HostStructField("ptr", "void*"), HostStructField("owned", "bool"))
            layout = "ptr_owned"
        result[name] = HostStructMetadata(
            c_type=handle.c_type,
            kind="handle",
            fields=fields,
            destroy_function=_handle_destroy_name(handle),
            element_type=handle.cpp_type,
            layout=layout,
        )
    return result


def _host_value_structs(ir: BindingIR) -> dict[str, HostStructMetadata]:
    result: dict[str, HostStructMetadata] = {
        "string": HostStructMetadata(
            c_type="ifcopenshell_string_t",
            kind="string",
            fields=(
                HostStructField("data", "char*"),
                HostStructField("size", "size_t"),
                HostStructField("owned", "bool"),
            ),
            destroy_function="ifcopenshell_string_destroy",
        )
    }
    for kind in _used_scalar_sequence_kinds(ir):
        result[kind] = HostStructMetadata(
            c_type=_sequence_c_type(kind),
            kind="sequence",
            fields=(HostStructField("items", f"{_sequence_items_c_type(kind)}*"), HostStructField("size", "size_t")),
            destroy_function=_sequence_destroy_name(kind),
            element_type=_sequence_items_c_type(kind),
            sequence_depth=(_sequence_kind_parts(kind) or ("", 0))[1],
        )
    for handle in _used_handle_list_handles(ir):
        list_type = _handle_list_c_type(handle)
        result[_snake_name(list_type)] = HostStructMetadata(
            c_type=list_type,
            kind="handle_sequence",
            fields=(HostStructField("items", f"{handle.c_type}**"), HostStructField("size", "size_t")),
            destroy_function=_handle_list_destroy_name(handle),
            element_type=handle.c_type,
            sequence_depth=1,
        )
        list_list_type = _handle_list_list_c_type(handle)
        result[_snake_name(list_list_type)] = HostStructMetadata(
            c_type=list_list_type,
            kind="handle_sequence",
            fields=(HostStructField("items", f"{list_type}*"), HostStructField("size", "size_t")),
            destroy_function=_handle_list_list_destroy_name(handle),
            element_type=list_type,
            sequence_depth=2,
        )
    for struct in ir.result_structs.values():
        result[struct.name] = HostStructMetadata(
            c_type=struct.c_type,
            kind="result_struct",
            fields=tuple(
                HostStructField(field.name, _field_c_type(field.type, ir))
                for field in struct.fields
            ),
            destroy_function=None,
            element_type=struct.cpp_type,
        )
    return result


def _function_metadata(call: CallIR, ir: BindingIR) -> HostFunctionMetadata:
    params: list[HostParamMetadata] = []
    if call.receiver is not None:
        receiver = ir.handles[call.receiver]
        params.append(
            HostParamMetadata(
                name="self",
                c_type=f"{receiver.c_type}*",
                role="receiver",
                type_kind="handle",
                nullable=False,
            )
        )
    for param in call.params:
        params.append(
            HostParamMetadata(
                name=param.name,
                c_type=_param_c_type(param.type, ir),
                role="param",
                type_kind=param.type.kind,
                nullable=param.type.nullable,
            )
        )
    if call.returns.kind != "void":
        params.append(
            HostParamMetadata(
                name="out_result",
                c_type=_out_c_type(call.returns, ir),
                role="out_result",
                type_kind=call.returns.kind,
                nullable=False,
            )
        )
    return HostFunctionMetadata(
        c_name=call.c_name,
        restype="bool",
        params=tuple(params),
        error_policy="bool_return_last_error",
        returns=call.returns,
        receiver=call.receiver,
    )


def build_host_metadata(ir: BindingIR) -> HostBindingMetadata:
    functions = {
        call.c_name: _function_metadata(call, ir)
        for call in sorted((*ir.functions, *ir.methods), key=lambda item: item.c_name)
    }
    return HostBindingMetadata(
        module=ir.module,
        c_prefix=ir.c_prefix,
        handles=_host_structs_for_handles(ir),
        value_types=_host_value_structs(ir),
        functions=functions,
        error_functions={
            "clear_error": f"{ir.c_prefix}_clear_error",
            "last_error_message": f"{ir.c_prefix}_last_error_message",
            "last_error_kind": f"{ir.c_prefix}_last_error_kind",
        },
    )
