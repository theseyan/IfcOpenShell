# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re

from ...abi_ir import BindingABI, CFunctionIR, CParamIR, CTypeIR
from ...binding_model import TypeSpec
from .._shared import (
    _INTERNAL_C_FUNCTIONS,
    _camel_name,
    _method_name,
    _public_module_member,
    _public_name,
    _public_params,
    _snake_name,
    _type_name,
    _typed_buffer_element,
)

_HIDDEN_FUNCTIONS = frozenset({"ifcopenshell_project_append_asset_cache_free"})


def _interface_name(name: str) -> str:
    base = name.removeprefix("ifcopenshell_").removesuffix("_t")
    parts = re.split(r"[_\s]+", base)
    return "IfcOpenshell" + "".join(part.capitalize() for part in parts if part)


def _ts_type_from_c_type(c_type: str, metadata: BindingABI) -> str:
    normalized = " ".join(c_type.replace(" *", "*").split())
    normalized_base = normalized.removeprefix("const ").removesuffix("*").strip()
    option = next(
        (
            option
            for option in metadata.option_structs.values()
            if normalized in {f"{option.c_type}*", f"const {option.c_type}*"}
        ),
        None,
    )
    if option is not None:
        return _interface_name(option.c_type)
    handle_name = next(
        (
            name
            for name, handle in metadata.handles.items()
            if f"{handle.c_type}*" == normalized
        ),
        None,
    )
    if handle_name is not None:
        return _type_name(metadata.handles[handle_name].c_type)
    if normalized in {"bool", "ifcopenshell_logical_t"}:
        return "boolean"
    if normalized in {"int32_t", "uint32_t", "double", "size_t", "void*"}:
        return "number"
    if normalized == "int64_t":
        return "bigint"
    if normalized in {"const char*", "ifcopenshell_string_t"}:
        return "string"
    if normalized.endswith("**"):
        pointee = normalized[:-2].strip()
        handle_name = next(
            (
                name
                for name, handle in metadata.handles.items()
                if handle.c_type == pointee
            ),
            None,
        )
        if handle_name is not None:
            return f"{_type_name(metadata.handles[handle_name].c_type)}[]"
    sequence = next(
        (
            item
            for item in metadata.value_types.values()
            if item.c_type == normalized_base
            and item.kind
            in {
                "sequence",
                "handle_sequence",
                "input_record_sequence",
                "result_record_sequence",
            }
        ),
        None,
    )
    if sequence is not None:
        return _sequence_ts_type(sequence, metadata)
    return "IfcOpenshellRawValue"


def _sequence_ts_type(struct: CTypeIR, metadata: BindingABI) -> str:
    if struct.kind == "input_record_sequence":
        option = next(
            (
                item
                for item in metadata.option_structs.values()
                if item.c_type == struct.element_type
            ),
            None,
        )
        return (
            f"{_interface_name(option.c_type)}[]"
            if option is not None
            else "IfcOpenshellRawValue[]"
        )
    if struct.kind == "handle_sequence":
        item = next(
            (
                _type_name(handle.c_type)
                for handle in metadata.handles.values()
                if struct.element_type is not None
                and struct.element_type.removeprefix("const ").removesuffix("*").strip()
                == handle.c_type
            ),
            "IfcOpenshellRawValue",
        )
        return f"{item}[]"

    if struct.kind == "result_record_sequence":
        item = next(
            (
                value
                for value in metadata.value_types.values()
                if value.c_type == struct.element_type and value.kind == "result_struct"
            ),
            None,
        )
        return (
            f"{_interface_name(item.c_type)}[]"
            if item is not None
            else "IfcOpenshellRawValue[]"
        )

    elem = (struct.element_type or "").removeprefix("const ").removesuffix("*").strip()
    scalar = {
        "bool": "boolean",
        "int32_t": "number",
        "uint32_t": "number",
        "uint8_t": "number",
        "double": "number",
        "int64_t": "bigint",
        "ifcopenshell_string_t": "string",
    }.get(elem)
    if scalar is not None:
        return f"{scalar}[]"

    nested = next(
        (item for item in metadata.value_types.values() if item.c_type == elem), None
    )
    if nested is not None and nested.kind in {
        "sequence",
        "handle_sequence",
        "result_record_sequence",
    }:
        return f"{_sequence_ts_type(nested, metadata)}[]"
    return "IfcOpenshellRawValue[]"


def _native_typed_array(element_type: str) -> str:
    return {
        "double": "Float64Array",
        "int32_t": "Int32Array",
        "uint32_t": "Uint32Array",
        "uint8_t": "Uint8Array",
    }[element_type]


def _ts_type(type_spec: TypeSpec, metadata: BindingABI) -> str:
    if type_spec.sequence_depth > 0:
        inner = _ts_type(
            TypeSpec(
                kind=type_spec.kind,
                handle=type_spec.handle,
                struct=type_spec.struct,
                variants=type_spec.variants,
            ),
            metadata,
        )
        for _ in range(type_spec.sequence_depth):
            inner = f"{inner}[]"
        return inner
    if type_spec.kind == "bool":
        result = "boolean"
    elif type_spec.kind in {"double", "int32", "uint32", "size", "uint8", "opaque_ptr"}:
        result = "number"
    elif type_spec.kind == "int64":
        result = "bigint"
    elif type_spec.kind == "string":
        result = "string"
    elif type_spec.kind == "void":
        result = "void"
    elif type_spec.kind == "handle" and type_spec.handle is not None:
        result = _type_name(metadata.handles[type_spec.handle].c_type)
    elif type_spec.kind == "struct" and type_spec.struct is not None:
        struct = metadata.value_types[type_spec.struct]
        result = _interface_name(struct.c_type)
    elif type_spec.kind == "option" and type_spec.struct is not None:
        option = metadata.option_structs[type_spec.struct]
        result = _interface_name(option.c_type)
    elif type_spec.kind == "variant":
        result = " | ".join(
            _ts_type(alt, metadata).removesuffix(" | null")
            for alt in type_spec.variants
        )
    else:
        result = "IfcOpenshellRawValue"
    if type_spec.nullable and result != "void":
        return f"{result} | null"
    return result


def _render_struct_interfaces(metadata: BindingABI) -> str:
    chunks: list[str] = []
    for name, struct in sorted(metadata.value_types.items()):
        if struct.kind != "result_struct":
            continue
        fields = "\n".join(
            f"    {field.name}: {_ts_type_from_c_type(field.c_type, metadata)};"
            for field in struct.fields
        )
        chunks.append(
            f"  export interface {_interface_name(struct.c_type)} {{\n{fields}\n  }}"
        )
    return "\n\n".join(chunks)


def _render_option_struct_interfaces(metadata: BindingABI) -> str:
    chunks: list[str] = []
    for option in sorted(
        metadata.option_structs.values(), key=lambda item: item.c_type
    ):
        fields = "\n".join(
            f"    {field.name}{'?' if field.type.nullable else ''}: "
            f"{_ts_type(field.type, metadata).removesuffix(' | null')};"
            for field in option.fields
        )
        chunks.append(
            f"  export interface {_interface_name(option.c_type)} {{\n{fields}\n  }}"
        )
    return "\n\n".join(chunks)


def _render_handle_classes(metadata: BindingABI) -> str:
    receiver_groups: dict[str, list[CFunctionIR]] = {}
    for function in metadata.functions.values():
        if function.receiver is None:
            continue
        receiver_groups.setdefault(function.receiver, []).append(function)

    chunks: list[str] = []
    for handle_name, handle in sorted(metadata.handles.items()):
        methods = ["    readonly ptr: number;", "    destroy(): void;"]
        for function in sorted(
            receiver_groups.get(handle_name, []), key=lambda item: item.c_name
        ):
            name = _public_name(function, metadata.c_prefix)
            methods.append(_render_function_signature(name, function, metadata))
        method_block = "\n".join(methods)
        chunks.append(
            f"  export class {_type_name(handle.c_type)} {{\n{method_block}\n  }}"
        )
    return "\n\n".join(chunks)


def _render_function_signature(
    name: str, function: CFunctionIR, metadata: BindingABI
) -> str:
    params = ", ".join(
        f"{param.name}: {_ts_type_from_c_type(param.c_type, metadata)}"
        + (
            " | null"
            if param.nullable
            and "null" not in _ts_type_from_c_type(param.c_type, metadata)
            else ""
        )
        for param in _public_params(function)
    )
    buffer_element = _typed_buffer_element(function, metadata)
    if buffer_element is not None:
        generic_params = f"{params}, " if params else ""
        signature = (
            f"    {name}({params}): {_native_typed_array(buffer_element)};\n"
            f"    {name}<T extends IfcOpenshellNumericTypedArray>({generic_params}arrayType: "
            "IfcOpenshellNumericArrayConstructor<T>): T;"
        )
    else:
        signature = f"    {name}({params}): {_ts_type(function.returns, metadata)};"
    if not function.doc:
        return signature
    return _render_doc_comment(function.doc, "    ") + "\n" + signature


def _render_doc_comment(doc: str, indent: str) -> str:
    escaped = doc.replace("*/", "* /").strip()
    lines = escaped.splitlines()
    if len(lines) == 1:
        return f"{indent}/** {lines[0]} */"
    body = "\n".join(f"{indent} * {line}" if line else f"{indent} *" for line in lines)
    return f"{indent}/**\n{body}\n{indent} */"


def _module_interface_name(module_name: str) -> str:
    return (
        "IfcOpenshell"
        + "".join(part.capitalize() for part in module_name.split("_") if part)
        + "Module"
    )


def _collect_module_members(metadata: BindingABI) -> dict[str, list[str]]:
    module_members: dict[str, list[str]] = {}
    for function in sorted(metadata.functions.values(), key=lambda item: item.c_name):
        if (
            function.receiver is not None
            or function.c_name in _INTERNAL_C_FUNCTIONS
            or function.c_name in _HIDDEN_FUNCTIONS
        ):
            continue
        module_member = _public_module_member(function, metadata.c_prefix)
        if module_member is None:
            continue
        module_name, member_name = module_member
        module_members.setdefault(module_name, []).append(
            _render_function_signature(member_name, function, metadata)
        )
    parse_open = metadata.functions.get("ifcopenshell_parse_open")
    if parse_open is not None:
        returns = _ts_type(parse_open.returns, metadata)
        module_members.setdefault("parse", []).append(
            "    openBytes(bytes: Uint8Array | ArrayBuffer | ArrayBufferView, "
            f"filename?: string, readonly?: boolean): {returns};"
        )
    return module_members


def _render_nested_module_interfaces(metadata: BindingABI) -> str:
    chunks: list[str] = []
    for module_name, members in sorted(_collect_module_members(metadata).items()):
        chunks.append(
            f"  export interface {_module_interface_name(module_name)} {{\n"
            + "\n".join(members)
            + "\n  }"
        )
    return "\n\n".join(chunks)


def _render_module_interface(
    metadata: BindingABI, module_members: dict[str, list[str]]
) -> str:
    members: list[str] = []
    for handle in sorted(metadata.handles.values(), key=lambda item: item.c_type):
        type_name = _type_name(handle.c_type)
        members.append(f"    {type_name}: typeof {type_name};")
    for function in sorted(metadata.functions.values(), key=lambda item: item.c_name):
        if function.receiver is not None:
            continue
        if (
            function.c_name in _INTERNAL_C_FUNCTIONS
            or function.c_name in _HIDDEN_FUNCTIONS
        ):
            continue
        if _public_module_member(function, metadata.c_prefix) is not None:
            continue
        name = _public_name(function, metadata.c_prefix)
        members.append(_render_function_signature(name, function, metadata))
    for module_name in sorted(module_members):
        members.append(f"    {module_name}: {_module_interface_name(module_name)};")
    members.extend(
        [
            "    loadPlugin(kind: 'schema' | 'kernel' | 'mapping' | 'tree' | 'document' | 'geometry_serializer', id: string): Promise<void>;",
            "    loadedPlugins(): string[];",
        ]
    )
    return "  export interface IfcOpenshellModule {\n" + "\n".join(members) + "\n  }"


def render_typescript_declarations(
    metadata: BindingABI, handles: dict[str, CTypeIR] | None = None
) -> str:
    del handles
    struct_interfaces = _render_struct_interfaces(metadata)
    option_struct_interfaces = _render_option_struct_interfaces(metadata)
    handle_classes = _render_handle_classes(metadata)
    module_members = _collect_module_members(metadata)
    nested_module_interfaces = _render_nested_module_interfaces(metadata)
    module_interface = _render_module_interface(metadata, module_members)
    sections = [
        "// This file was generated with the assistance of an AI coding tool.",
        "",
        "declare module 'ifcopenshell-api' {",
        "  export type IfcOpenshellRawValue = null | boolean | number | bigint | string | object | IfcOpenshellRawValue[];",
        "  export type IfcOpenshellNumericTypedArray = Int8Array | Uint8Array | Uint8ClampedArray | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array;",
        "  export interface IfcOpenshellNumericArrayConstructor<T extends IfcOpenshellNumericTypedArray> {",
        "    readonly BYTES_PER_ELEMENT: number;",
        "    new (arrayLike: ArrayLike<number>): T;",
        "  }",
        "",
    ]
    if struct_interfaces:
        sections.extend([struct_interfaces, ""])
    if option_struct_interfaces:
        sections.extend([option_struct_interfaces, ""])
    if handle_classes:
        sections.extend([handle_classes, ""])
    if nested_module_interfaces:
        sections.extend([nested_module_interfaces, ""])
    sections.extend(
        [
            module_interface,
            "",
            "  export function createIfcOpenshellModule(",
            "    initModule: (options?: Record<string, IfcOpenshellRawValue>) => Promise<object>,",
            "    wasmUrl?: string,",
            "    options?: {",
            "      pluginBaseUrl?: string;",
            "      pluginManifest?: Record<string, Record<string, { wasm: string; depends?: string[] }>>;",
            "      pluginLoader?: (",
            "        url: string,",
            "        plugin: { kind: string; id: string; entry: { wasm: string; depends?: string[] } },",
            "      ) => Promise<Uint8Array | ArrayBuffer | ArrayBufferView> | Uint8Array | ArrayBuffer | ArrayBufferView;",
            "    },",
            "  ): Promise<IfcOpenshellModule>;",
            "}",
            "",
        ]
    )
    return "\n".join(sections)


__all__ = ["render_typescript_declarations"]
