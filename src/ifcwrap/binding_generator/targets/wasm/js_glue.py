# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json

try:
    from ...binding_model import TypeSpec
    from ...host_metadata import HostBindingMetadata, HostFunctionMetadata, HostParamMetadata, HostStructMetadata
    from .._shared import (
        _INTERNAL_C_FUNCTIONS,
        _camel_name,
        _method_name,
        _public_module_member,
        _public_name,
        _public_params,
        _snake_name,
        _type_name,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from binding_model import TypeSpec
    from host_metadata import HostBindingMetadata, HostFunctionMetadata, HostParamMetadata, HostStructMetadata
    from targets._shared import (
        _INTERNAL_C_FUNCTIONS,
        _camel_name,
        _method_name,
        _public_module_member,
        _public_name,
        _public_params,
        _snake_name,
        _type_name,
    )

_POINTER_SIZE = 4

_SCALAR_LAYOUTS = {
    "bool": (1, 1),
    "logical": (4, 4),
    "int32": (4, 4),
    "uint8": (1, 1),
    "uint32": (4, 4),
    "size": (4, 4),
    "double": (8, 8),
    "int64": (8, 8),
}


def _normalize_c_type(c_type: str) -> str:
    return " ".join(c_type.replace(" *", "*").split())


def _handle_for_c_type(c_type: str, metadata: HostBindingMetadata) -> HostStructMetadata | None:
    normalized = _normalize_c_type(c_type)
    for handle in metadata.handles.values():
        if normalized in {f"{handle.c_type}*", f"const {handle.c_type}*"}:
            return handle
    return None


def _value_type_by_c_type(c_type: str, metadata: HostBindingMetadata) -> HostStructMetadata | None:
    normalized = _normalize_c_type(c_type).removeprefix("const ").removesuffix("*").strip()
    return next((value for value in metadata.value_types.values() if value.c_type == normalized), None)


def _type_layout(c_type: str, metadata: HostBindingMetadata) -> tuple[int, int]:
    normalized = _normalize_c_type(c_type)
    if normalized.endswith("*"):
        return _POINTER_SIZE, _POINTER_SIZE
    if normalized in {"bool", "uint8_t"}:
        return 1, 1
    if normalized in {"ifcopenshell_logical_t", "int32_t", "uint32_t", "size_t"}:
        return 4, 4
    if normalized in {"int64_t", "double"}:
        return 8, 8
    struct = _value_type_by_c_type(normalized, metadata)
    if struct is not None:
        return _struct_layout(struct, metadata)
    return _POINTER_SIZE, _POINTER_SIZE


def _align_to(offset: int, alignment: int) -> int:
    remainder = offset % alignment
    return offset if remainder == 0 else offset + alignment - remainder


def _struct_layout(struct: HostStructMetadata, metadata: HostBindingMetadata) -> tuple[int, int]:
    offset = 0
    max_alignment = 1
    for field in struct.fields:
        size, alignment = _type_layout(field.c_type, metadata)
        offset = _align_to(offset, alignment)
        offset += size
        max_alignment = max(max_alignment, alignment)
    return _align_to(offset, max_alignment), max_alignment


def _sequence_kind(type_spec: TypeSpec) -> str:
    return f"{type_spec.kind}{'_list' * type_spec.sequence_depth}"


def _sequence_value_type(type_spec: TypeSpec, metadata: HostBindingMetadata) -> HostStructMetadata:
    if type_spec.kind == "handle" and type_spec.handle is not None:
        handle = metadata.handles[type_spec.handle]
        suffix = "_list" * type_spec.sequence_depth
        key = _snake_name(f"{handle.c_type.removesuffix('_t')}{suffix}_t")
        return metadata.value_types[key]
    return metadata.value_types[_sequence_kind(type_spec)]


def _out_allocation(function: HostFunctionMetadata, metadata: HostBindingMetadata) -> str | None:
    returns = function.returns
    if returns.kind == "void":
        return None
    if returns.kind == "handle" or returns.kind == "opaque_ptr":
        return "POINTER_SIZE"
    if returns.kind in _SCALAR_LAYOUTS:
        return str(_SCALAR_LAYOUTS[returns.kind][0])
    if returns.sequence_depth > 0:
        return str(_struct_layout(_sequence_value_type(returns, metadata), metadata)[0])
    if returns.kind == "string":
        return str(_struct_layout(metadata.value_types["string"], metadata)[0])
    if returns.kind == "struct" and returns.struct is not None:
        return str(_struct_layout(metadata.value_types[returns.struct], metadata)[0])
    return "POINTER_SIZE"


def _read_value_type_expr(type_spec: TypeSpec, metadata: HostBindingMetadata, ptr_expr: str) -> str:
    if type_spec.sequence_depth > 0:
        sequence = _sequence_value_type(type_spec, metadata)
        return f"_readValueType(module, {ptr_expr}, _VALUE_TYPES[{json.dumps(sequence.c_type)}])"
    if type_spec.kind == "string":
        return f"_readValueType(module, {ptr_expr}, _VALUE_TYPES['ifcopenshell_string_t'])"
    if type_spec.kind == "struct" and type_spec.struct is not None:
        struct = metadata.value_types[type_spec.struct]
        return f"_readValueType(module, {ptr_expr}, _VALUE_TYPES[{json.dumps(struct.c_type)}])"
    return "module.getValue(outResultPtr, '*')"


def _return_expr(function: HostFunctionMetadata, metadata: HostBindingMetadata) -> str:
    returns = function.returns
    if returns.kind == "void":
        return "undefined"
    if returns.kind in {"string", "struct"} or returns.sequence_depth > 0:
        return _read_value_type_expr(returns, metadata, "outResultPtr")
    if returns.kind == "bool":
        return "module.getValue(outResultPtr, 'i8') !== 0"
    if returns.kind == "logical":
        return "module.getValue(outResultPtr, 'i32') !== 0"
    if returns.kind in {"int32", "uint32", "size"}:
        return "module.getValue(outResultPtr, 'i32')"
    if returns.kind == "double":
        return "module.getValue(outResultPtr, 'double')"
    if returns.kind == "int64":
        return "_readInt64(module, outResultPtr)"
    if returns.kind == "handle" and returns.handle is not None:
        handle = metadata.handles[returns.handle]
        return f"_wrap{_type_name(handle.c_type)}(module.getValue(outResultPtr, '*'), true, module)"
    return "module.getValue(outResultPtr, '*')"


def _destroy_out_result(function: HostFunctionMetadata, metadata: HostBindingMetadata) -> str | None:
    returns = function.returns
    if returns.sequence_depth > 0:
        destroy = _sequence_value_type(returns, metadata).destroy_function
    elif returns.kind == "string":
        destroy = metadata.value_types["string"].destroy_function
    else:
        destroy = None
    if destroy is None:
        return None
    return f"    if (outResultPtr) module._{destroy}(outResultPtr);"


def _js_arg_expr(param: HostParamMetadata, metadata: HostBindingMetadata) -> tuple[str, str | None, str | None]:
    name = param.name
    if param.type_kind in {"bool", "logical"}:
        return f"{name} ? 1 : 0", None, None
    handle = _handle_for_c_type(param.c_type, metadata)
    if handle is not None:
        return f"{name} == null ? 0 : {name}.ptr", None, None
    sequence = _value_type_by_c_type(param.c_type, metadata)
    if sequence is not None and sequence.kind == "sequence" and sequence.sequence_depth == 1:
        ptr_name = f"_{name}Ptr"
        if sequence.element_type == "ifcopenshell_string_t":
            alloc = (
                f"    var {ptr_name} = {name} == null ? 0 : _allocInputStringList(module, {name});"
                if param.nullable
                else f"    var {ptr_name} = _allocInputStringList(module, {name});"
            )
            cleanup = f"    if ({ptr_name}) _freeInputStringList(module, {ptr_name});"
        else:
            alloc = (
                f"    var {ptr_name} = {name} == null ? 0 : _allocInputSequence(module, {name}, {json.dumps(sequence.c_type)});"
                if param.nullable
                else f"    var {ptr_name} = _allocInputSequence(module, {name}, {json.dumps(sequence.c_type)});"
            )
            cleanup = f"    if ({ptr_name}) _freeInputSequence(module, {ptr_name}, {json.dumps(sequence.c_type)});"
        return ptr_name, alloc, cleanup
    if param.type_kind == "string":
        ptr_name = f"_{name}Ptr"
        alloc = (
            f"    var {ptr_name} = {name} == null ? 0 : _allocString(module, {name});"
            if param.nullable
            else f"    var {ptr_name} = _allocString(module, {name});"
        )
        cleanup = f"    if ({ptr_name}) module._free({ptr_name});"
        return ptr_name, alloc, cleanup
    if param.type_kind in {"int32", "uint32", "size", "double", "int64"}:
        return name, None, None
    return name, None, None


def _render_handle_classes(metadata: HostBindingMetadata) -> str:
    receiver_groups: dict[str, list[HostFunctionMetadata]] = {}
    for function in metadata.functions.values():
        if function.receiver is not None:
            receiver_groups.setdefault(function.receiver, []).append(function)

    chunks: list[str] = []
    for handle_name, handle in sorted(metadata.handles.items()):
        type_name = _type_name(handle.c_type)
        methods = []
        for function in sorted(receiver_groups.get(handle_name, []), key=lambda item: item.c_name):
            if function.c_name == handle.destroy_function:
                continue
            method = _public_name(function, metadata.c_prefix)
            params = ", ".join(param.name for param in _public_params(function))
            methods.append(
                f"    {method}({params}) {{\n"
                f"        return invoke_{function.c_name}(this.#module, this{', ' if params else ''}{params});\n"
                "    }"
            )
        method_block = "\n\n".join(methods)
        destroy = handle.destroy_function or f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
        chunks.append(
            f"export class {type_name} {{\n"
            "    #ptr;\n"
            "    #owned;\n"
            "    #module;\n\n"
            "    constructor(ptr, owned, module) {\n"
            "        this.#ptr = ptr;\n"
            "        this.#owned = owned;\n"
            "        this.#module = module;\n"
            "    }\n\n"
            "    get ptr() {\n"
            "        return this.#ptr;\n"
            "    }\n\n"
            "    destroy() {\n"
            f"        if (this.#ptr && this.#owned && this.#module._{destroy}) {{\n"
            f"            this.#module._{destroy}(this.#ptr);\n"
            "            this.#ptr = 0;\n"
            "            this.#owned = false;\n"
            "        }\n"
            "    }"
            + ("\n\n" + method_block if method_block else "")
            + "\n}\n\n"
            f"function _wrap{type_name}(ptr, owned, module) {{\n"
            f"    return ptr ? new {type_name}(ptr, owned, module) : null;\n"
            "}"
        )
    return "\n\n".join(chunks)


def _render_handle_wrapper_switch(metadata: HostBindingMetadata) -> str:
    cases = []
    for handle in sorted(metadata.handles.values(), key=lambda item: item.c_type):
        type_name = _type_name(handle.c_type)
        cases.append(f"        case {json.dumps(handle.c_type)}: return _wrap{type_name}(ptr, owned, module);")
    body = "\n".join(cases) or "        default: return ptr || null;"
    if cases:
        body += "\n        default: return ptr || null;"
    return (
        "function _wrapHandleByType(module, handleCType, ptr, owned) {\n"
        "    switch (handleCType) {\n"
        f"{body}\n"
        "    }\n"
        "}"
    )


def _render_wrapper(function: HostFunctionMetadata, metadata: HostBindingMetadata) -> str:
    public_params = _public_params(function)
    signature_names = ["module"]
    if function.receiver is not None:
        signature_names.append("self")
    signature_names.extend(param.name for param in public_params)

    marshalling_lines: list[str] = []
    cleanup_lines: list[str] = []
    arg_exprs: list[str] = []
    if function.receiver is not None:
        arg_exprs.append("self.ptr")
    for param in public_params:
        expr, alloc, cleanup = _js_arg_expr(param, metadata)
        if alloc:
            marshalling_lines.append(alloc)
        if cleanup:
            cleanup_lines.append(cleanup)
        arg_exprs.append(expr)

    allocation_size = _out_allocation(function, metadata)
    out_alloc = ""
    out_free = ""
    out_arg = ""
    if allocation_size is not None:
        out_alloc = f"    outResultPtr = module._malloc({allocation_size});\n"
        out_free = "        if (outResultPtr) module._free(outResultPtr);\n"
        out_arg = ", outResultPtr"

    call_args = ", ".join(arg_exprs)
    if call_args:
        call_args += out_arg
    else:
        call_args = out_arg.removeprefix(", ")

    error_name = function.c_name
    destroy_line = _destroy_out_result(function, metadata)
    return_body = []
    if function.returns.kind != "void":
        return_body.append("    const result = " + _return_expr(function, metadata) + ";")
        if destroy_line:
            return_body.append(destroy_line)
        return_body.append("    return result;")
    else:
        return_body.append("    return undefined;")
    return_statement = "\n".join(return_body) + "\n"

    cleanup_block = "\n".join(cleanup_lines + ([out_free.rstrip()] if out_free else []))
    if cleanup_block:
        cleanup_block = cleanup_block + "\n"

    if function.restype == "void":
        call_block = f"    module._{function.c_name}({call_args});\n"
    else:
        call_block = (
            f"    const ok = module._{function.c_name}({call_args});\n"
            + f"    if (!ok) throw new Error(_lastErrorMessage(module, '{error_name} failed'));\n"
        )

    return (
        f"function invoke_{function.c_name}({', '.join(signature_names)}) {{\n"
        f"    module._{metadata.error_functions['clear_error']}();\n"
        "    let outResultPtr = 0;\n"
        "    try {\n"
        + ("\n".join(marshalling_lines) + ("\n" if marshalling_lines else ""))
        + out_alloc
        + call_block
        + return_statement
        + "    } finally {\n"
        + cleanup_block
        + "    }\n"
        + "}"
    )


def _render_module_factory(metadata: HostBindingMetadata) -> str:
    members: list[str] = []
    module_members: dict[str, list[str]] = {}
    for handle in sorted(metadata.handles.values(), key=lambda item: item.c_type):
        type_name = _type_name(handle.c_type)
        members.append(f"        {type_name},")
    for function in sorted(metadata.functions.values(), key=lambda item: item.c_name):
        if function.receiver is not None:
            continue
        if function.c_name in _INTERNAL_C_FUNCTIONS:
            continue
        name = _public_name(function, metadata.c_prefix)
        params = ", ".join(param.name for param in _public_params(function))
        call = f"({params}) => invoke_{function.c_name}(module{', ' if params else ''}{params})"
        module_member = _public_module_member(function, metadata.c_prefix)
        if module_member is not None:
            module_name, member_name = module_member
            module_members.setdefault(module_name, []).append(f"            {member_name}: {call},")
        else:
            members.append(f"        {name}: {call},")
    if "ifcopenshell_parse_open" in metadata.functions:
        module_members.setdefault("parse", []).append("            openBytes,")
    for module_name, nested_members in sorted(module_members.items()):
        members.append(
            f"        {module_name}: Object.freeze({{\n"
            + "\n".join(nested_members)
            + "\n"
            "        }),"
        )
    joined = "\n".join(members)
    return (
        "export async function createIfcOpenshellModule(initModule, wasmUrl, options = {}) {\n"
        "    const module = await initModule({\n"
        "        locateFile(path) {\n"
        "            if (path.endsWith('.wasm') && wasmUrl) return wasmUrl;\n"
        "            return path;\n"
        "        },\n"
        "    });\n"
        "\n"
        "    const pluginBaseUrl = options.pluginBaseUrl ?? new URL('.', import.meta.url).href;\n"
        "    const pluginManifest = options.pluginManifest ?? {};\n"
        "    const pluginLoader = options.pluginLoader ?? defaultPluginLoader;\n"
        "    const loadedPlugins = new Set();\n"
        "    const loadingPlugins = new Map();\n"
        "\n"
        "    function pluginKey(kind, id) {\n"
        "        return `${kind}:${id}`;\n"
        "    }\n"
        "\n"
        "    function pluginEntry(kind, id) {\n"
        "        return pluginManifest?.[kind]?.[id] ?? null;\n"
        "    }\n"
        "\n"
        "    function pluginDependencies(kind, id) {\n"
        "        const entry = pluginEntry(kind, id);\n"
        "        if (!entry) return [];\n"
        "        return Array.isArray(entry.depends) ? entry.depends : [];\n"
        "    }\n"
        "\n"
        "    function pluginUrl(entry) {\n"
        "        return new URL(entry.wasm, pluginBaseUrl).href;\n"
        "    }\n"
        "\n"
        "    async function defaultPluginLoader(url) {\n"
        "        const response = await fetch(url);\n"
        "        if (!response.ok) {\n"
        "            throw new Error(`Failed to fetch WASM plugin ${url}: ${response.status}`);\n"
        "        }\n"
        "        return new Uint8Array(await response.arrayBuffer());\n"
        "    }\n"
        "\n"
        "    async function loadPluginLibrary(kind, id, entry) {\n"
        "        const url = pluginUrl(entry);\n"
        "        const bytes = normalizeVirtualFileBytes(await pluginLoader(url, { kind, id, entry }));\n"
        "        const path = virtualFilePath(entry.wasm);\n"
        "        module.FS.writeFile(path, bytes);\n"
        "        try {\n"
        "            await module.loadDynamicLibrary(path, { global: true, allowUndefined: true, loadAsync: true });\n"
        "        } finally {\n"
        "            module.FS.unlink(path);\n"
        "        }\n"
        "    }\n"
        "\n"
        "    async function loadPlugin(kind, id) {\n"
        "        const key = pluginKey(kind, id);\n"
        "        if (loadedPlugins.has(key)) return;\n"
        "        if (loadingPlugins.has(key)) {\n"
        "            return loadingPlugins.get(key);\n"
        "        }\n"
        "        const promise = (async () => {\n"
        "            for (const dependency of pluginDependencies(kind, id)) {\n"
        "                const separator = dependency.indexOf(':');\n"
        "                if (separator <= 0 || separator === dependency.length - 1) {\n"
        "                    throw new Error(`Invalid WASM plugin dependency ${dependency} for ${key}`);\n"
        "                }\n"
        "                await loadPlugin(dependency.slice(0, separator), dependency.slice(separator + 1));\n"
        "            }\n"
        "            if (kind === 'schema') {\n"
        "                const entry = pluginEntry('schema', id);\n"
        "                if (!entry?.wasm) {\n"
        "                    throw new Error(`Unknown WASM schema plugin ${id}`);\n"
        "                }\n"
        "                await loadPluginLibrary(kind, id, entry);\n"
        "                const schema = invoke_ifcopenshell_parse_schema_by_name(module, id.toUpperCase());\n"
        "                if (schema) schema.destroy();\n"
        "                loadedPlugins.add(key);\n"
        "                return;\n"
        "            }\n"
        "            const entry = pluginEntry(kind, id);\n"
        "            if (!entry?.wasm) {\n"
        "                throw new Error(`Unknown WASM plugin ${kind}/${id}`);\n"
        "            }\n"
        "            await loadPluginLibrary(kind, id, entry);\n"
        "            if (!invoke_ifcopenshell_geom_plugin_load(module, kind, id)) {\n"
        "                throw new Error(`Plugin ${kind}/${id} loaded but failed to register`);\n"
        "            }\n"
        "            if (!pluginIsLoaded(kind, id)) {\n"
        "                throw new Error(`Plugin ${kind}/${id} loaded but failed to register`);\n"
        "            }\n"
        "            loadedPlugins.add(key);\n"
        "        })();\n"
        "        loadingPlugins.set(key, promise);\n"
        "        try {\n"
        "            await promise;\n"
        "        } finally {\n"
        "            loadingPlugins.delete(key);\n"
        "        }\n"
        "    }\n"
        "\n"
        "    function pluginIsLoaded(kind, id) {\n"
        "        return invoke_ifcopenshell_geom_plugin_is_loaded(module, kind, id);\n"
        "    }\n"
        "\n"
        "    let nextVirtualFileId = 0;\n"
        "\n"
        "    function normalizeVirtualFileBytes(bytes) {\n"
        "        if (bytes instanceof Uint8Array) return bytes;\n"
        "        if (ArrayBuffer.isView(bytes)) {\n"
        "            return new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength);\n"
        "        }\n"
        "        if (bytes instanceof ArrayBuffer) return new Uint8Array(bytes);\n"
        "        throw new TypeError('Expected IFC input as Uint8Array, ArrayBuffer, or ArrayBufferView');\n"
        "    }\n"
        "\n"
        "    function virtualFilePath(filename) {\n"
        "        const basename = String(filename || 'model.ifc').split(/[\\\\/]/).pop() || 'model.ifc';\n"
        "        const safeName = basename.replace(/[^A-Za-z0-9._-]/g, '_');\n"
        "        nextVirtualFileId += 1;\n"
        "        return `/ifcopenshell-${nextVirtualFileId}-${safeName}`;\n"
        "    }\n"
        "\n"
        "    function openBytes(bytes, filename = 'model.ifc', readonly = false) {\n"
        "        const path = virtualFilePath(filename);\n"
        "        module.FS.writeFile(path, normalizeVirtualFileBytes(bytes));\n"
        "        try {\n"
        "            return invoke_ifcopenshell_parse_open(module, path, readonly);\n"
        "        } finally {\n"
        "            module.FS.unlink(path);\n"
        "        }\n"
        "    }\n"
        "\n"
        "    return Object.freeze({\n"
        + joined
        + "\n"
        "        loadPlugin,\n"
        "        loadedPlugins: () => Array.from(loadedPlugins),\n"
        "    });\n"
        "}"
    )


def _render_value_type_metadata(metadata: HostBindingMetadata) -> str:
    payload = {
        value.c_type: {
            "cType": value.c_type,
            "kind": value.kind,
            "destroyFunction": value.destroy_function,
            "elementType": value.element_type,
            "sequenceDepth": value.sequence_depth,
            "fields": [{"name": field.name, "cType": field.c_type} for field in value.fields],
        }
        for value in sorted(metadata.value_types.values(), key=lambda item: item.c_type)
    }
    return json.dumps(payload, indent=4, sort_keys=True)


def render_js_glue(metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata] | None = None) -> str:
    del handles
    wrappers = "\n\n".join(
        _render_wrapper(function, metadata) for function in sorted(metadata.functions.values(), key=lambda item: item.c_name)
    )
    classes = _render_handle_classes(metadata)
    handle_switch = _render_handle_wrapper_switch(metadata)
    factory = _render_module_factory(metadata)
    value_types = _render_value_type_metadata(metadata)
    return "\n".join(
        [
            "// This file was generated with the assistance of an AI coding tool.",
            "",
            "const POINTER_SIZE = 4;",
            f"const _VALUE_TYPES = {value_types};",
            "const _STRUCT_LAYOUT_CACHE = new Map();",
            "",
            "function _normalizeCType(cType) {",
            "    return cType.replace(/\\s+\\*/g, '*').replace(/\\s+/g, ' ').trim();",
            "}",
            "",
            "function _alignTo(offset, alignment) {",
            "    const remainder = offset % alignment;",
            "    return remainder === 0 ? offset : offset + alignment - remainder;",
            "}",
            "",
            "function _readInt64(module, ptr) {",
            "    const index = ptr >> 2;",
            "    const low = module.HEAP32[index];",
            "    const high = module.HEAP32[index + 1];",
            "    return (BigInt(high) << 32n) | BigInt(low >>> 0);",
            "}",
            "",
            "function _writeInt64(module, ptr, value) {",
            "    const normalized = BigInt(value);",
            "    const index = ptr >> 2;",
            "    module.HEAP32[index] = Number(BigInt.asIntN(32, normalized));",
            "    module.HEAP32[index + 1] = Number(BigInt.asIntN(32, normalized >> 32n));",
            "}",
            "",
            "function _nativeTypeInfo(cType) {",
            "    const normalized = _normalizeCType(cType);",
            "    if (normalized.endsWith('*')) return { size: POINTER_SIZE, align: POINTER_SIZE, getter: '*' };",
            "    switch (normalized) {",
            "        case 'bool':",
            "        case 'uint8_t': return { size: 1, align: 1, getter: 'i8' };",
            "        case 'ifcopenshell_logical_t':",
            "        case 'int32_t':",
            "        case 'uint32_t':",
            "        case 'size_t': return { size: 4, align: 4, getter: 'i32' };",
            "        case 'double': return { size: 8, align: 8, getter: 'double' };",
            "        case 'int64_t': return { size: 8, align: 8, getter: 'i64' };",
            "        default: {",
            "            const valueType = _VALUE_TYPES[normalized];",
            "            if (!valueType) return { size: POINTER_SIZE, align: POINTER_SIZE, getter: '*' };",
            "            const layout = _getStructLayout(valueType);",
            "            return { size: layout.size, align: layout.align, getter: 'struct', valueType };",
            "        }",
            "    }",
            "}",
            "",
            "function _getStructLayout(structMetadata) {",
            "    const cached = _STRUCT_LAYOUT_CACHE.get(structMetadata.cType);",
            "    if (cached) return cached;",
            "    let offset = 0;",
            "    let align = 1;",
            "    const fields = structMetadata.fields.map((field) => {",
            "        const info = _nativeTypeInfo(field.cType);",
            "        offset = _alignTo(offset, info.align);",
            "        const result = { ...field, offset, info };",
            "        offset += info.size;",
            "        align = Math.max(align, info.align);",
            "        return result;",
            "    });",
            "    const layout = { size: _alignTo(offset, align), align, fields };",
            "    _STRUCT_LAYOUT_CACHE.set(structMetadata.cType, layout);",
            "    return layout;",
            "}",
            "",
            "function _allocString(module, value) {",
            "    const size = module.lengthBytesUTF8(value) + 1;",
            "    const ptr = module._malloc(size);",
            "    module.stringToUTF8(value, ptr, size);",
            "    return ptr;",
            "}",
            "",
            "function _lastErrorMessage(module, fallbackMessage) {",
            f"    const errorPtr = module._{metadata.error_functions['last_error_message']}();",
            "    return errorPtr ? module.UTF8ToString(errorPtr) : fallbackMessage;",
            "}",
            "",
            "function _getValue(module, ptr, cType) {",
            "    const info = _nativeTypeInfo(cType);",
            "    if (info.getter === 'i64') return _readInt64(module, ptr);",
            "    if (info.getter === 'struct') return _readValueType(module, ptr, info.valueType);",
            "    return module.getValue(ptr, info.getter);",
            "}",
            "",
            "function _setValue(module, ptr, cType, value) {",
            "    const info = _nativeTypeInfo(cType);",
            "    if (info.getter === 'i64') {",
            "        _writeInt64(module, ptr, value);",
            "        return;",
            "    }",
            "    if (info.getter === 'i8') {",
            "        module.setValue(ptr, value ? 1 : 0, 'i8');",
            "        return;",
            "    }",
            "    module.setValue(ptr, value, info.getter);",
            "}",
            "",
            handle_switch,
            "",
            "function _readStringValue(module, ptr) {",
            "    const layout = _getStructLayout(_VALUE_TYPES['ifcopenshell_string_t']);",
            "    const dataField = layout.fields.find((field) => field.name === 'data');",
            "    const dataPtr = module.getValue(ptr + dataField.offset, '*');",
            "    return dataPtr ? module.UTF8ToString(dataPtr) : null;",
            "}",
            "",
            "function _readSequenceValue(module, ptr, metadata) {",
            "    const layout = _getStructLayout(metadata);",
            "    const itemsField = layout.fields.find((field) => field.name === 'items');",
            "    const sizeField = layout.fields.find((field) => field.name === 'size');",
            "    const itemsPtr = module.getValue(ptr + itemsField.offset, '*');",
            "    const size = module.getValue(ptr + sizeField.offset, 'i32');",
            "    if (!itemsPtr || size === 0) return [];",
            "    const elementType = metadata.elementType;",
            "    if (!elementType) return [];",
            "    const elementInfo = _nativeTypeInfo(elementType);",
            "    const result = [];",
            "    for (let index = 0; index < size; index += 1) {",
            "        const elementPtr = itemsPtr + index * elementInfo.size;",
            "        if (elementInfo.getter === 'struct') {",
            "            result.push(_readValueType(module, elementPtr, elementInfo.valueType));",
            "        } else if (elementType in _VALUE_TYPES) {",
            "            result.push(_readValueType(module, elementPtr, _VALUE_TYPES[elementType]));",
            "        } else if (_normalizeCType(elementType).endsWith('_t')) {",
            "            result.push(_wrapHandleByType(module, elementType, module.getValue(elementPtr, '*'), true));",
            "        } else {",
            "            const value = _getValue(module, elementPtr, elementType);",
            "            result.push(_normalizeCType(elementType) === 'bool' ? value !== 0 : value);",
            "        }",
            "    }",
            "    return result;",
            "}",
            "",
            "function _readStructValue(module, ptr, metadata) {",
            "    const layout = _getStructLayout(metadata);",
            "    const result = {};",
            "    for (const field of layout.fields) {",
            "        const fieldPtr = ptr + field.offset;",
            "        if (field.info.getter === 'struct') {",
            "            result[field.name] = _readValueType(module, fieldPtr, field.info.valueType);",
            "        } else if (_normalizeCType(field.cType).endsWith('_t*')) {",
            "            result[field.name] = _wrapHandleByType(module, _normalizeCType(field.cType).slice(0, -1), module.getValue(fieldPtr, '*'), true);",
            "        } else {",
            "            const value = _getValue(module, fieldPtr, field.cType);",
            "            result[field.name] = _normalizeCType(field.cType) === 'bool' ? value !== 0 : value;",
            "        }",
            "    }",
            "    return result;",
            "}",
            "",
            "function _readValueType(module, ptr, metadata) {",
            "    switch (metadata.kind) {",
            "        case 'string': return _readStringValue(module, ptr);",
            "        case 'sequence':",
            "        case 'handle_sequence': return _readSequenceValue(module, ptr, metadata);",
            "        case 'result_struct': return _readStructValue(module, ptr, metadata);",
            "        default: return _readStructValue(module, ptr, metadata);",
            "    }",
            "}",
            "",
            "",
            "function _allocInputStringList(module, value) {",
            "    if (!Array.isArray(value)) throw new Error('Expected a string array.');",
            "    const listMetadata = _VALUE_TYPES['ifcopenshell_string_list_t'];",
            "    const stringMetadata = _VALUE_TYPES['ifcopenshell_string_t'];",
            "    const listLayout = _getStructLayout(listMetadata);",
            "    const stringLayout = _getStructLayout(stringMetadata);",
            "    const itemsField = listLayout.fields.find((field) => field.name === 'items');",
            "    const sizeField = listLayout.fields.find((field) => field.name === 'size');",
            "    const dataField = stringLayout.fields.find((field) => field.name === 'data');",
            "    const stringSizeField = stringLayout.fields.find((field) => field.name === 'size');",
            "    const ownedField = stringLayout.fields.find((field) => field.name === 'owned');",
            "    const structPtr = module._malloc(listLayout.size);",
            "    const itemsPtr = value.length ? module._malloc(stringLayout.size * value.length) : 0;",
            "    const dataPtrs = [];",
            "    try {",
            "        for (let index = 0; index < value.length; index += 1) {",
            "            const itemPtr = itemsPtr + index * stringLayout.size;",
            "            const dataPtr = _allocString(module, value[index]);",
            "            dataPtrs.push(dataPtr);",
            "            module.setValue(itemPtr + dataField.offset, dataPtr, '*');",
            "            module.setValue(itemPtr + stringSizeField.offset, module.lengthBytesUTF8(value[index]), 'i32');",
            "            module.setValue(itemPtr + ownedField.offset, 1, 'i8');",
            "        }",
            "        module.setValue(structPtr + itemsField.offset, itemsPtr, '*');",
            "        module.setValue(structPtr + sizeField.offset, value.length, 'i32');",
            "        return structPtr;",
            "    } catch (error) {",
            "        for (const dataPtr of dataPtrs) module._free(dataPtr);",
            "        if (itemsPtr) module._free(itemsPtr);",
            "        module._free(structPtr);",
            "        throw error;",
            "    }",
            "}",
            "",
            "function _freeInputStringList(module, ptr) {",
            "    if (!ptr) return;",
            "    const listMetadata = _VALUE_TYPES['ifcopenshell_string_list_t'];",
            "    const stringMetadata = _VALUE_TYPES['ifcopenshell_string_t'];",
            "    const listLayout = _getStructLayout(listMetadata);",
            "    const stringLayout = _getStructLayout(stringMetadata);",
            "    const itemsField = listLayout.fields.find((field) => field.name === 'items');",
            "    const sizeField = listLayout.fields.find((field) => field.name === 'size');",
            "    const dataField = stringLayout.fields.find((field) => field.name === 'data');",
            "    const itemsPtr = module.getValue(ptr + itemsField.offset, '*');",
            "    const count = module.getValue(ptr + sizeField.offset, 'i32');",
            "    if (itemsPtr) {",
            "        for (let index = 0; index < count; index += 1) {",
            "            const itemPtr = itemsPtr + index * stringLayout.size;",
            "            const dataPtr = module.getValue(itemPtr + dataField.offset, '*');",
            "            if (dataPtr) module._free(dataPtr);",
            "        }",
            "        module._free(itemsPtr);",
            "    }",
            "    module._free(ptr);",
            "}",
            "",
            "function _allocInputSequence(module, value, cType) {",
            "    if (!Array.isArray(value)) throw new Error(`Expected an array for ${cType}.`);",
            "    const metadata = _VALUE_TYPES[cType];",
            "    if (!metadata || metadata.kind !== 'sequence' || metadata.sequenceDepth !== 1) {",
            "        throw new Error(`Sequence marshalling for ${cType} is not implemented.`);",
            "    }",
            "    const elementType = _normalizeCType(metadata.elementType || '');",
            "    if (!['bool', 'ifcopenshell_logical_t', 'int32_t', 'uint32_t', 'size_t', 'double', 'int64_t', 'uint8_t'].includes(elementType)) {",
            "        throw new Error(`Sequence marshalling for ${cType} is only supported for scalar element types.`);",
            "    }",
            "    const layout = _getStructLayout(metadata);",
            "    const itemsField = layout.fields.find((field) => field.name === 'items');",
            "    const sizeField = layout.fields.find((field) => field.name === 'size');",
            "    const elementInfo = _nativeTypeInfo(elementType);",
            "    const structPtr = module._malloc(layout.size);",
            "    const itemsPtr = value.length ? module._malloc(elementInfo.size * value.length) : 0;",
            "    try {",
            "        for (let index = 0; index < value.length; index += 1) {",
            "            _setValue(module, itemsPtr + index * elementInfo.size, elementType, value[index]);",
            "        }",
            "        module.setValue(structPtr + itemsField.offset, itemsPtr, '*');",
            "        module.setValue(structPtr + sizeField.offset, value.length, 'i32');",
            "        return structPtr;",
            "    } catch (error) {",
            "        if (itemsPtr) module._free(itemsPtr);",
            "        module._free(structPtr);",
            "        throw error;",
            "    }",
            "}",
            "",
            "function _freeInputSequence(module, ptr, cType) {",
            "    const metadata = _VALUE_TYPES[cType];",
            "    if (!metadata) {",
            "        module._free(ptr);",
            "        return;",
            "    }",
            "    const layout = _getStructLayout(metadata);",
            "    const itemsField = layout.fields.find((field) => field.name === 'items');",
            "    const itemsPtr = module.getValue(ptr + itemsField.offset, '*');",
            "    if (itemsPtr) module._free(itemsPtr);",
            "    module._free(ptr);",
            "}",
            "",
            classes,
            "",
            wrappers,
            "",
            factory,
            "",
        ]
    )


__all__ = ["render_js_glue"]
