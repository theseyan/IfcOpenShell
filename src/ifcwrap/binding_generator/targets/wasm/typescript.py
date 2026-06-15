# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re

try:
    from ...binding_model import TypeSpec
    from ...host_metadata import HostBindingMetadata, HostFunctionMetadata, HostParamMetadata, HostStructMetadata
    from .._shared import _INTERNAL_C_FUNCTIONS, _camel_name, _method_name, _public_name, _public_params, _snake_name, _type_name
except ImportError:  # pragma: no cover - script execution fallback
    from binding_model import TypeSpec
    from host_metadata import HostBindingMetadata, HostFunctionMetadata, HostParamMetadata, HostStructMetadata
    from targets._shared import _INTERNAL_C_FUNCTIONS, _camel_name, _method_name, _public_name, _public_params, _snake_name, _type_name

def _interface_name(name: str) -> str:
    base = name.removeprefix("ifcopenshell_").removesuffix("_t")
    parts = re.split(r"[_\s]+", base)
    return "IfcOpenshell" + "".join(part.capitalize() for part in parts if part)


def _ts_type_from_c_type(c_type: str, metadata: HostBindingMetadata) -> str:
    normalized = " ".join(c_type.replace(" *", "*").split())
    handle_name = next((name for name, handle in metadata.handles.items() if f"{handle.c_type}*" == normalized), None)
    if handle_name is not None:
        return _type_name(metadata.handles[handle_name].c_type)
    if normalized in {"bool", "ifcopenshell_logical_t"}:
        return "boolean"
    if normalized in {"int32_t", "uint32_t", "double", "size_t", "void*"}:
        return "number"
    if normalized == "int64_t":
        return "bigint"
    if normalized == "const char*":
        return "string"
    if normalized.endswith("**"):
        pointee = normalized[:-2].strip()
        handle_name = next((name for name, handle in metadata.handles.items() if handle.c_type == pointee), None)
        if handle_name is not None:
            return f"{_type_name(metadata.handles[handle_name].c_type)}[]"
    if normalized.startswith("const ifcopenshell_") and normalized.endswith("_list_t*"):
        if "double_list" in normalized:
            return "number[]"
        if "int32_list" in normalized or "uint32_list" in normalized:
            return "number[]"
        if "int64_list" in normalized:
            return "bigint[]"
        if "bool_list" in normalized:
            return "boolean[]"
        if "string_list" in normalized:
            return "string[]"
    return "unknown"


def _ts_type(type_spec: TypeSpec, metadata: HostBindingMetadata) -> str:
    if type_spec.sequence_depth > 0:
        inner = _ts_type(TypeSpec(kind=type_spec.kind, handle=type_spec.handle, struct=type_spec.struct), metadata)
        for _ in range(type_spec.sequence_depth):
            inner = f"{inner}[]"
        return inner
    if type_spec.kind in {"bool", "logical"}:
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
    else:
        result = "unknown"
    if type_spec.nullable and result != "void":
        return f"{result} | null"
    return result


def _render_struct_interfaces(metadata: HostBindingMetadata) -> str:
    chunks: list[str] = []
    for name, struct in sorted(metadata.value_types.items()):
        if struct.kind != "result_struct":
            continue
        fields = "\n".join(
            f"    {field.name}: {_ts_type_from_c_type(field.c_type, metadata)};" for field in struct.fields
        )
        chunks.append(f"  export interface {_interface_name(struct.c_type)} {{\n{fields}\n  }}")
    return "\n\n".join(chunks)


def _render_handle_classes(metadata: HostBindingMetadata) -> str:
    receiver_groups: dict[str, list[HostFunctionMetadata]] = {}
    for function in metadata.functions.values():
        if function.receiver is None:
            continue
        receiver_groups.setdefault(function.receiver, []).append(function)

    chunks: list[str] = []
    for handle_name, handle in sorted(metadata.handles.items()):
        methods = ["    readonly ptr: number;", "    destroy(): void;"]
        for function in sorted(receiver_groups.get(handle_name, []), key=lambda item: item.c_name):
            name = _public_name(function, metadata.c_prefix)
            params = ", ".join(
                f"{param.name}: {_ts_type_from_c_type(param.c_type, metadata)}"
                + (" | null" if param.nullable and "null" not in _ts_type_from_c_type(param.c_type, metadata) else "")
                for param in _public_params(function)
            )
            returns = _ts_type(function.returns, metadata)
            methods.append(f"    {name}({params}): {returns};")
        method_block = "\n".join(methods)
        chunks.append(f"  export class {_type_name(handle.c_type)} {{\n{method_block}\n  }}")
    return "\n\n".join(chunks)


def _render_module_interface(metadata: HostBindingMetadata) -> str:
    members: list[str] = []
    for handle in sorted(metadata.handles.values(), key=lambda item: item.c_type):
        type_name = _type_name(handle.c_type)
        members.append(f"    {type_name}: typeof {type_name};")
    for function in sorted(metadata.functions.values(), key=lambda item: item.c_name):
        if function.receiver is not None:
            continue
        if function.c_name in _INTERNAL_C_FUNCTIONS:
            continue
        name = _public_name(function, metadata.c_prefix)
        params = ", ".join(
            f"{param.name}: {_ts_type_from_c_type(param.c_type, metadata)}"
            + (" | null" if param.nullable and "null" not in _ts_type_from_c_type(param.c_type, metadata) else "")
            for param in _public_params(function)
        )
        members.append(f"    {name}({params}): {_ts_type(function.returns, metadata)};")
    members.extend(
        [
            "    loadPlugin(kind: 'schema' | 'kernel' | 'mapping' | 'tree' | 'document' | 'geometry_serializer', id: string): Promise<void>;",
            "    loadedPlugins(): string[];",
        ]
    )
    return "  export interface IfcOpenshellModule {\n" + "\n".join(members) + "\n  }"


def render_typescript_declarations(metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata] | None = None) -> str:
    del handles
    struct_interfaces = _render_struct_interfaces(metadata)
    handle_classes = _render_handle_classes(metadata)
    module_interface = _render_module_interface(metadata)
    sections = [
        "// This file was generated with the assistance of an AI coding tool.",
        "",
        "declare module 'ifcopenshell-api' {",
    ]
    if struct_interfaces:
        sections.extend([struct_interfaces, ""])
    if handle_classes:
        sections.extend([handle_classes, ""])
        sections.extend(
        [
            module_interface,
            "",
            "  export function createIfcOpenshellModule(",
            "    initModule: (options?: Record<string, unknown>) => Promise<unknown>,",
            "    wasmUrl?: string,",
            "    options?: {",
            "      pluginBaseUrl?: string;",
            "      pluginManifest?: Record<string, Record<string, { wasm: string }>>;",
            "    },",
            "  ): Promise<IfcOpenshellModule>;",
            "}",
            "",
        ]
    )
    return "\n".join(sections)


__all__ = ["render_typescript_declarations"]
