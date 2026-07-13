# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import sys
from pathlib import Path

import pytest

# Ensure the repo root is on sys.path so `from src.ifcwrap...` works
# regardless of the working directory.
_REPO_ROOT = str(Path(__file__).resolve().parents[3])
if _REPO_ROOT not in sys.path:
    sys.path.insert(0, _REPO_ROOT)

from src.ifcwrap.binding_generator.binding_ir import BindingIR, CallIR, DirectCallOp
from src.ifcwrap.binding_generator.binding_model import ParamSpec, TypeSpec
from src.ifcwrap.binding_generator.host_metadata import (
    HostBindingMetadata,
    HostFunctionMetadata,
    HostOptionFieldMetadata,
    HostOptionStructMetadata,
    HostParamMetadata,
    HostStructField,
    HostStructMetadata,
    _function_metadata,
)
from src.ifcwrap.binding_generator.targets import (
    TargetGenerationRequest,
    discover_targets,
    get_target,
)
from src.ifcwrap.binding_generator.targets.wasm import WasmTargetBackend
from src.ifcwrap.binding_generator.targets.wasm.api_bridge import render_api_direct
from src.ifcwrap.binding_generator.targets.wasm.backend import (
    render_export_list,
    render_wasm_bindings,
)
from src.ifcwrap.binding_generator.targets.wasm.js_glue import render_js_glue
from src.ifcwrap.binding_generator.targets.wasm.typescript import (
    render_typescript_declarations,
)

_DEFAULT_ERROR_FUNCTIONS = {
    "clear_error": "ifcopenshell_demo_clear_error",
    "last_error_message": "ifcopenshell_demo_last_error_message",
    "last_error_kind": "ifcopenshell_demo_last_error_kind",
}


def _make_ir(module: str = "ifcopenshell_wrapper", c_prefix: str = "ifcopenshell_demo") -> BindingIR:
    return BindingIR(
        module=module,
        c_prefix=c_prefix,
        public_headers=(),
        handles={},
        result_structs={},
        functions=(),
        methods=(),
    )


def _make_handle(c_type: str, destroy_function: str | None = None) -> HostStructMetadata:
    return HostStructMetadata(
        c_type=c_type,
        kind="handle",
        fields=(HostStructField("ptr", "void*"), HostStructField("owned", "bool")),
        destroy_function=destroy_function
        or f"ifcopenshell_{c_type.removeprefix('ifcopenshell_').removesuffix('_t')}_destroy",
        layout="ptr_owned",
    )


def _make_function(
    *,
    c_name: str,
    params: tuple[HostParamMetadata, ...] = (),
    returns: TypeSpec | None = None,
    receiver: str | None = None,
    doc: str | None = None,
) -> HostFunctionMetadata:
    if returns is None:
        returns = TypeSpec(kind="void")
    full_params = list(params)
    if receiver is not None:
        full_params.insert(
            0,
            HostParamMetadata(
                name="self",
                c_type=f"ifcopenshell_demo_{receiver}_t*",
                role="receiver",
                type_kind="handle",
            ),
        )
    if returns.kind != "void":
        out_type = {
            "bool": "bool*",
            "double": "double*",
            "int32": "int32_t*",
            "int64": "int64_t*",
            "string": "ifcopenshell_string_t*",
            "handle": f"ifcopenshell_demo_{returns.handle}_t**",
            "struct": "void*",
            "variant": "void*",
        }[returns.kind]
        full_params.append(
            HostParamMetadata(
                name="out_result",
                c_type=out_type,
                role="out_result",
                type_kind=returns.kind,
            )
        )
    return HostFunctionMetadata(
        c_name=c_name,
        restype="bool",
        params=tuple(full_params),
        error_policy="bool_return_last_error",
        returns=returns,
        receiver=receiver,
        doc=doc,
    )


def _make_metadata(
    *,
    c_prefix: str = "ifcopenshell_demo",
    handles: dict[str, HostStructMetadata] | None = None,
    value_types: dict[str, HostStructMetadata] | None = None,
    option_structs: dict[str, HostOptionStructMetadata] | None = None,
    functions: dict[str, HostFunctionMetadata] | None = None,
) -> HostBindingMetadata:
    return HostBindingMetadata(
        module="ifcopenshell_wrapper",
        c_prefix=c_prefix,
        handles=handles or {},
        value_types=value_types or {},
        option_structs=option_structs or {},
        functions=functions or {},
        error_functions=_DEFAULT_ERROR_FUNCTIONS,
    )


class TestHostMetadata:
    def test_function_metadata_preserves_param_semantics(self):
        call = CallIR(
            expose_as="demo.use",
            c_name="ifcopenshell_demo_use",
            receiver=None,
            returns=TypeSpec(kind="void"),
            params=(
                ParamSpec(
                    "properties",
                    TypeSpec(
                        kind="opaque_ptr",
                        cpp_type="arbitrary_property_builder_t*",
                        semantic="property_map",
                    ),
                ),
                ParamSpec(
                    "opaque",
                    TypeSpec(kind="opaque_ptr", cpp_type="arbitrary_opaque_t*"),
                ),
            ),
            operation=DirectCallOp(cpp_name="demo_use"),
        )

        metadata = _function_metadata(call, _make_ir())
        params = {param.name: param for param in metadata.params}

        assert params["properties"].semantic == "property_map"
        assert params["opaque"].semantic is None


class TestWasmTypescript:
    def test_generates_handle_class_and_destroy_method(self):
        metadata = _make_metadata(handles={"file": _make_handle("ifcopenshell_demo_file_t")})
        code = render_typescript_declarations(metadata)
        assert "export class IfcOpenshellDemoFile" in code
        assert "destroy(): void;" in code
        assert "readonly ptr: number;" in code

    def test_maps_scalar_and_handle_types(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_open_file": _make_function(
                    c_name="ifcopenshell_demo_open_file",
                    params=(HostParamMetadata("path", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="file"),
                ),
                "ifcopenshell_demo_set_flag": _make_function(
                    c_name="ifcopenshell_demo_set_flag",
                    params=(HostParamMetadata("flag", "bool", "param", "bool"),),
                ),
            },
        )
        code = render_typescript_declarations(metadata)
        assert "openFile(path: string): IfcOpenshellDemoFile;" in code
        assert "setFlag(flag: boolean): void;" in code

    def test_generates_nested_module_interfaces(self):
        metadata = HostBindingMetadata(
            module="ifcopenshell_wrapper",
            c_prefix="ifcopenshell",
            handles={"settings": _make_handle("ifcopenshell_geom_settings_t")},
            value_types={},
            functions={
                "ifcopenshell_parse_open": _make_function(
                    c_name="ifcopenshell_parse_open",
                    params=(HostParamMetadata("path", "const char*", "param", "string"),),
                ),
                "ifcopenshell_geom_create_settings": _make_function(
                    c_name="ifcopenshell_geom_create_settings",
                    returns=TypeSpec(kind="handle", handle="settings"),
                ),
                "ifcopenshell_unit_add_si_unit": _make_function(
                    c_name="ifcopenshell_unit_add_si_unit",
                    params=(HostParamMetadata("unit_type", "const char*", "param", "string"),),
                ),
            },
            error_functions=_DEFAULT_ERROR_FUNCTIONS,
        )
        code = render_typescript_declarations(metadata)
        assert "export interface IfcOpenshellParseModule" in code
        assert "open(path: string): void;" in code
        assert (
            "openBytes(bytes: Uint8Array | ArrayBuffer | ArrayBufferView, filename?: string, readonly?: boolean): void;"
        ) in code
        assert "export interface IfcOpenshellGeomModule" in code
        assert "createSettings(): IfcOpenshellGeomSettings;" in code
        assert "export interface IfcOpenshellUnitModule" in code
        assert "addSiUnit(unit_type: string): void;" in code
        assert "parse: IfcOpenshellParseModule;" in code
        assert "geom: IfcOpenshellGeomModule;" in code
        assert "unit: IfcOpenshellUnitModule;" in code
        assert (
            "    createSettings(): IfcOpenshellGeomSettings;"
            not in code.split("export interface IfcOpenshellModule")[1]
        )

    def test_renders_source_docs_as_jsdoc(self):
        metadata = _make_metadata(
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(HostParamMetadata("ifc_class", "const char*", "param", "string"),),
                    doc="Create an IFC entity.\nInitializes identity and ownership metadata.",
                ),
            },
        )
        code = render_typescript_declarations(metadata)
        assert (
            "    /**\n"
            "     * Create an IFC entity.\n"
            "     * Initializes identity and ownership metadata.\n"
            "     */\n"
            "    rootCreateEntity(ifc_class: string): void;"
        ) in code

    def test_renders_option_struct_interfaces(self):
        metadata = _make_metadata(
            option_structs={
                "CreateEntityOptions": HostOptionStructMetadata(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        HostOptionFieldMetadata("ifc_class", TypeSpec(kind="string"), "const char*"),
                        HostOptionFieldMetadata(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                    ),
                )
            }
        )

        code = render_typescript_declarations(metadata)

        assert "export interface IfcOpenshellDemoCreateEntityOptions" in code
        assert "ifc_class: string;" in code
        assert "name?: string;" in code

    def test_preserves_native_option_field_casing(self):
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_demo_instance_t")},
            option_structs={
                "AssignObjectOptions": HostOptionStructMetadata(
                    name="AssignObjectOptions",
                    c_type="ifcopenshell_demo_assign_object_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_demo_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "relating_object",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_demo_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "sync_predefined_type",
                            TypeSpec(kind="bool"),
                            "bool",
                        ),
                    ),
                )
            },
        )

        code = render_typescript_declarations(metadata)

        assert "export interface IfcOpenshellDemoAssignObjectOptions" in code
        assert "owner_history?: IfcOpenshellDemoInstance;" in code
        assert "relating_object: IfcOpenshellDemoInstance;" in code
        assert "sync_predefined_type: boolean;" in code
        assert "ownerHistory" not in code
        assert "relatingObject" not in code
        assert "syncPredefinedType" not in code

    def test_renders_option_struct_parameters(self):
        metadata = _make_metadata(
            option_structs={
                "CreateEntityOptions": HostOptionStructMetadata(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(HostOptionFieldMetadata("ifc_class", TypeSpec(kind="string"), "const char*"),),
                )
            },
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_demo_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                )
            },
        )

        code = render_typescript_declarations(metadata)

        assert "rootCreateEntity(options: IfcOpenshellDemoCreateEntityOptions): void;" in code


class TestWasmJsGlue:
    def test_generates_handle_wrapper_and_destroy_logic(self):
        metadata = _make_metadata(handles={"file": _make_handle("ifcopenshell_demo_file_t")})
        code = render_js_glue(metadata)
        assert "export class IfcOpenshellDemoFile" in code
        assert "destroy()" in code
        assert "module._ifcopenshell_demo_file_destroy" in code

    def test_generates_module_factory_and_error_handling(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_open_file": _make_function(
                    c_name="ifcopenshell_demo_open_file",
                    params=(HostParamMetadata("path", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="file"),
                )
            },
        )
        code = render_js_glue(metadata)
        assert "export async function createIfcOpenshellModule" in code
        assert "module._ifcopenshell_demo_clear_error()" in code
        assert "throw new Error(_lastErrorMessage(module, 'ifcopenshell_demo_open_file failed'))" in code
        assert "openFile: (path) => invoke_ifcopenshell_demo_open_file(module, path)" in code
        assert "createIfcOpenshellModule(initModule, wasmUrl, options = {})" in code
        assert "const pluginBaseUrl = options.pluginBaseUrl ?? new URL('.', import.meta.url).href;" in code
        assert "const pluginLoader = options.pluginLoader ?? defaultPluginLoader;" in code
        assert "return new URL(entry.wasm, pluginBaseUrl).href;" in code
        assert "module.FS.writeFile(path, bytes);" in code
        assert "await module.loadDynamicLibrary(path, { global: true, allowUndefined: true, loadAsync: true });" in code
        assert "module.FS.unlink(path);" in code
        assert "import initIfcOpenShellWasmModule" not in code

    def test_wraps_owned_and_borrowed_handle_returns(self):
        metadata = _make_metadata(
            handles={"template": _make_handle("ifcopenshell_demo_template_t")},
            functions={
                "ifcopenshell_demo_template_create": _make_function(
                    c_name="ifcopenshell_demo_template_create",
                    returns=TypeSpec(kind="handle", handle="template", ownership="owned"),
                ),
                "ifcopenshell_demo_template_cached": _make_function(
                    c_name="ifcopenshell_demo_template_cached",
                    returns=TypeSpec(kind="handle", handle="template", ownership="borrowed"),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert "_wrapIfcOpenshellDemoTemplate(module.getValue(outResultPtr, '*'), true, module)" in code
        assert "_wrapIfcOpenshellDemoTemplate(module.getValue(outResultPtr, '*'), false, module)" in code

    def test_destroys_variant_results_after_transferring_handles(self):
        variant = TypeSpec(
            kind="variant",
            cpp_type="std::variant<Demo::Instance*, std::string>",
            variants=(
                TypeSpec(
                    kind="handle",
                    handle="instance",
                    cpp_type="Demo::Instance*",
                    ownership="owned",
                ),
                TypeSpec(kind="string", cpp_type="std::string"),
            ),
        )
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_demo_instance_t")},
            value_types={
                "instance_string_variant": HostStructMetadata(
                    c_type="ifcopenshell_demo_instance_string_variant_t",
                    kind="variant",
                    fields=(
                        HostStructField("kind", "int32_t"),
                        HostStructField("value_0", "ifcopenshell_demo_instance_t*"),
                        HostStructField("value_1", "ifcopenshell_string_t"),
                    ),
                    destroy_function="ifcopenshell_demo_instance_string_variant_destroy",
                    element_type=variant.cpp_type,
                )
            },
            functions={"ifcopenshell_demo_value": _make_function(c_name="ifcopenshell_demo_value", returns=variant)},
        )

        code = render_js_glue(metadata)

        assert "module._ifcopenshell_demo_instance_string_variant_destroy(outResultPtr);" in code
        assert "const ptr = module.getValue(fieldPtr, '*');" in code
        assert "module.setValue(fieldPtr, 0, '*');" in code

    def test_generates_nested_api_modules(self):
        metadata = HostBindingMetadata(
            module="ifcopenshell_wrapper",
            c_prefix="ifcopenshell",
            handles={"settings": _make_handle("ifcopenshell_geom_settings_t")},
            value_types={},
            functions={
                "ifcopenshell_parse_open": _make_function(
                    c_name="ifcopenshell_parse_open",
                    params=(HostParamMetadata("path", "const char*", "param", "string"),),
                ),
                "ifcopenshell_geom_create_settings": _make_function(
                    c_name="ifcopenshell_geom_create_settings",
                    returns=TypeSpec(kind="handle", handle="settings"),
                ),
                "ifcopenshell_unit_add_si_unit": _make_function(
                    c_name="ifcopenshell_unit_add_si_unit",
                    params=(HostParamMetadata("unit_type", "const char*", "param", "string"),),
                ),
            },
            error_functions=_DEFAULT_ERROR_FUNCTIONS,
        )
        code = render_js_glue(metadata)
        assert "parse: Object.freeze({" in code
        assert "open: (path) => invoke_ifcopenshell_parse_open(module, path)" in code
        assert "openBytes," in code
        assert "module.FS.writeFile(path, normalizeVirtualFileBytes(bytes));" in code
        assert "module.FS.unlink(path);" in code
        assert "geom: Object.freeze({" in code
        assert "createSettings: () => invoke_ifcopenshell_geom_create_settings(module)" in code
        assert "unit: Object.freeze({" in code
        assert "addSiUnit: (unit_type) => invoke_ifcopenshell_unit_add_si_unit(module, unit_type)" in code
        assert (
            "        createSettings: () => invoke_ifcopenshell_geom_create_settings(module),\n"
            "        geom: Object.freeze({" not in code
        )

    def test_generates_instance_methods_for_receiver_functions(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_file_set_name": _make_function(
                    c_name="ifcopenshell_demo_file_set_name",
                    receiver="file",
                    params=(HostParamMetadata("name", "const char*", "param", "string"),),
                )
            },
        )
        code = render_js_glue(metadata)
        assert "setName(name)" in code
        assert "invoke_ifcopenshell_demo_file_set_name(this.#module, this, name)" in code

    def test_reads_int64_from_heap32_and_marshals_scalar_sequences(self):
        metadata = _make_metadata(
            value_types={
                "string": HostStructMetadata(
                    c_type="ifcopenshell_string_t",
                    kind="string",
                    fields=(
                        HostStructField("data", "char*"),
                        HostStructField("size", "size_t"),
                        HostStructField("owned", "bool"),
                    ),
                    destroy_function="ifcopenshell_string_destroy",
                ),
                "int32_list": HostStructMetadata(
                    c_type="ifcopenshell_int32_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "int32_t*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_int32_list_destroy",
                    element_type="int32_t",
                    sequence_depth=1,
                ),
                "string_list": HostStructMetadata(
                    c_type="ifcopenshell_string_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_string_t*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_string_list_destroy",
                    element_type="ifcopenshell_string_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_demo_big": _make_function(
                    c_name="ifcopenshell_demo_big",
                    returns=TypeSpec(kind="int64"),
                ),
                "ifcopenshell_demo_sum_values": _make_function(
                    c_name="ifcopenshell_demo_sum_values",
                    params=(
                        HostParamMetadata(
                            "values",
                            "const ifcopenshell_int32_list_t*",
                            "param",
                            "int32",
                        ),
                    ),
                    returns=TypeSpec(kind="int32"),
                ),
                "ifcopenshell_demo_names": _make_function(
                    c_name="ifcopenshell_demo_names",
                    returns=TypeSpec(kind="string", sequence_depth=1),
                ),
            },
        )
        code = render_js_glue(metadata)
        assert "const low = module.HEAP32[index];" in code
        assert "const high = module.HEAP32[index + 1];" in code
        assert "_allocInputSequence(module, values" in code
        assert "_freeInputSequence(module, _valuesPtr" in code
        assert "outResultPtr = module._malloc(8);" in code
        assert '_readValueType(module, outResultPtr, _VALUE_TYPES["ifcopenshell_string_list_t"])' in code
        assert "module._ifcopenshell_string_list_destroy(outResultPtr);" in code

    def test_marshals_handle_sequence_parameters(self):
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_instance_t")},
            value_types={
                "instance_list": HostStructMetadata(
                    c_type="ifcopenshell_instance_list_t",
                    kind="handle_sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_instance_t**"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function=None,
                    element_type="ifcopenshell_instance_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_demo_from_handles": _make_function(
                    c_name="ifcopenshell_demo_from_handles",
                    params=(
                        HostParamMetadata(
                            "instances",
                            "const ifcopenshell_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                )
            },
        )

        code = render_js_glue(metadata)

        assert "_allocInputHandleSequence(module, instances" in code
        assert "_freeInputHandleSequence(module, _instancesPtr" in code
        assert "module.setValue(itemsPtr + index * POINTER_SIZE, item == null ? 0 : item.ptr, '*');" in code

    def test_generates_dependency_aware_plugin_loader(self):
        metadata = _make_metadata()
        code = render_js_glue(metadata)
        assert "const loadingPlugins = new Map();" in code
        assert "function pluginDependencies(kind, id)" in code
        assert "for (const dependency of pluginDependencies(kind, id))" in code
        assert "await loadPlugin(dependency.slice(0, separator), dependency.slice(separator + 1));" in code
        assert "async function loadPluginLibrary(kind, id, entry)" in code
        assert "await module.loadDynamicLibrary(path, { global: true, allowUndefined: true, loadAsync: true });" in code
        assert "invoke_ifcopenshell_geom_plugin_registry_address" not in code

    def test_marshals_option_struct_parameters(self):
        metadata = _make_metadata(
            value_types={
                "double_list": HostStructMetadata(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "double*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "double_list_list": HostStructMetadata(
                    c_type="ifcopenshell_double_list_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_double_list_t*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_list_destroy",
                    element_type="ifcopenshell_double_list_t",
                    sequence_depth=2,
                ),
            },
            option_structs={
                "CreateEntityOptions": HostOptionStructMetadata(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        HostOptionFieldMetadata("ifc_class", TypeSpec(kind="string"), "const char*"),
                        HostOptionFieldMetadata(
                            "axis",
                            TypeSpec(kind="double", sequence_depth=2),
                            "const ifcopenshell_double_list_list_t*",
                        ),
                        HostOptionFieldMetadata(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_demo_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                )
            },
        )

        code = render_js_glue(metadata)

        assert '"ifcopenshell_demo_create_entity_options_t": {' in code
        assert '"optionFields": [' in code
        assert '"name": "has_name"' in code
        assert (
            'var _optionsPtr = _allocInputOption(module, options, "ifcopenshell_demo_create_entity_options_t");' in code
        )
        assert (
            'if (_optionsPtr) _freeInputOption(module, _optionsPtr, "ifcopenshell_demo_create_entity_options_t");'
            in code
        )
        assert "function _allocInputOption(module, value, cType)" in code
        assert "function _zeroMemory(module, ptr, size)" in code
        assert "module.HEAPU32.fill(0, start, start + wordCount);" in code
        assert "module.HEAPU8.fill" not in code
        assert "    _zeroMemory(module, ptr, layout.size);" in code
        assert "function _freeInputOption(module, ptr, cType)" in code
        assert "function _writeInputSequence(module, structPtr, value, metadata)" in code
        assert "else _writeInputSequence(module, itemPtr, value[index], elementMetadata);" in code
        assert "function _freeInputSequenceItems(module, ptr, metadata)" in code


class TestWasmApiBridge:
    def test_generates_direct_api_facade(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "CreateEntityOptions": HostOptionStructMetadata(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_root_create_entity_options_t",
                    fields=(
                        HostOptionFieldMetadata("ifc_class", TypeSpec(kind="string"), "const char*"),
                        HostOptionFieldMetadata(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                        HostOptionFieldMetadata(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_root_create_entity": _make_function(
                    c_name="ifcopenshell_root_create_entity",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_root_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                    doc="Create an IFC entity.",
                )
            },
        )

        code = render_api_direct(metadata)

        assert "export interface IfcOpenShellRootCreateEntityOptions" in code
        assert "ownerHistory?: Entity;" in code
        assert "export interface RootApi" in code
        assert "export interface Api" in code
        assert "createEntity(file: IfcFile, options: IfcOpenShellRootCreateEntityOptions): Entity;" in code
        assert "/**\n * @internal\n */\nexport function createApi(shell: IfcOpenShell): Api {" in code
        assert "/**\n * @internal\n */\nexport interface Api" not in code
        assert "raw.root.createEntity(file.raw, encodeOptions(options" in code
        assert '"ifcClass": "ifc_class"' in code
        assert '"name": "name"' in code
        assert '"ownerHistory": "owner_history"' in code
        assert "type ApiInput = ApiData | PsetProperties | PsetInput;" in code
        assert "function toRaw(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue" in code
        assert "function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue" in code
        assert "disposeAll(temps);" in code
        assert "return wrapEntity(shell, result) as Entity;" in code

    def test_generates_exact_raw_api_members_and_signatures(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "AssignObjectOptions": HostOptionStructMetadata(
                    name="AssignObjectOptions",
                    c_type="ifcopenshell_aggregate_assign_object_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "relating_object",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "sync_predefined_type",
                            TypeSpec(kind="bool"),
                            "bool",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_aggregate_assign_object": _make_function(
                    c_name="ifcopenshell_aggregate_assign_object",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_aggregate_assign_object_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                ),
                "ifcopenshell_alpha_lookup": _make_function(
                    c_name="ifcopenshell_alpha_lookup",
                    params=(HostParamMetadata("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_beta_lookup": _make_function(
                    c_name="ifcopenshell_beta_lookup",
                    params=(HostParamMetadata("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_demo_use": _make_function(
                    c_name="ifcopenshell_demo_use",
                    params=(HostParamMetadata("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_guid_generate": _make_function(
                    c_name="ifcopenshell_guid_generate",
                    returns=TypeSpec(kind="string"),
                ),
            },
        )

        code = render_api_direct(metadata)
        raw_start = code.index("type RawApi = {")
        raw_end = code.index("export interface", raw_start)
        raw_api = code[raw_start:raw_end]

        assert "type RawFn" not in code
        assert "generate: () => string;" in raw_api
        assert "use: (value: string) => void;" in raw_api
        assert "assignObject: (file: RawValue, options: RawValue) => RawValue;" in raw_api
        assert "  alpha: {\n    lookup: (value: string) => void;\n  };" in raw_api
        assert "  beta: {\n    lookup: (value: string) => void;\n  };" in raw_api
        assert "[key: string]" not in raw_api
        assert "...args" not in raw_api
        assert "raw.guid.generate();" in code
        assert "raw.demo.use(value);" in code
        assert "raw.aggregate.assignObject(file.raw, encodeOptions(options" in code
        assert "raw.alpha.lookup(value);" in code
        assert "raw.beta.lookup(value);" in code
        low_level = render_typescript_declarations(metadata)
        assert "owner_history?: IfcOpenshellInstance;" in low_level
        assert "relating_object: IfcOpenshellInstance;" in low_level
        assert "sync_predefined_type: boolean;" in low_level
        assert "ownerHistory?: Entity;" in code
        assert "relatingObject: Entity;" in code
        assert "syncPredefinedType: boolean;" in code
        assert '"ownerHistory": "owner_history"' in code
        assert '"relatingObject": "relating_object"' in code
        assert '"syncPredefinedType": "sync_predefined_type"' in code

    def test_direct_api_facade_renders_field_documentation(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            option_structs={
                "DemoOptions": HostOptionStructMetadata(
                    name="DemoOptions",
                    c_type="ifcopenshell_demo_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "enabled",
                            TypeSpec(kind="bool", nullable=True),
                            "bool",
                            doc="Whether the feature is enabled.",
                        ),
                        HostOptionFieldMetadata("label", TypeSpec(kind="string"), "const char*"),
                    ),
                )
            },
            value_types={
                "DemoResult": HostStructMetadata(
                    c_type="ifcopenshell_demo_result_t",
                    kind="result_struct",
                    fields=(
                        HostStructField(
                            "value",
                            "double",
                            "Computed result value.\n\nMeasured in model units.",
                        ),
                        HostStructField("undocumented", "bool"),
                    ),
                    destroy_function=None,
                )
            },
        )

        code = render_api_direct(metadata)

        assert "/** Whether the feature is enabled. */\n  enabled?: boolean;" in code
        assert "/**\n   * Computed result value.\n   *\n   * Measured in model units.\n   */\n  value: number;" in code
        assert "  label: string;" in code
        assert "  undocumented: boolean;" in code
        assert "/** */" not in code

    def test_direct_api_facade_wraps_option_handles(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "CreateEntityOptions": HostOptionStructMetadata(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_root_create_entity_options_t",
                    fields=(
                        HostOptionFieldMetadata("ifc_class", TypeSpec(kind="string"), "const char*"),
                        HostOptionFieldMetadata(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_root_create_entity": _make_function(
                    c_name="ifcopenshell_root_create_entity",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_root_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                )
            },
        )

        code = render_api_direct(metadata)

        assert "ApiHandle" not in code
        assert "ApiTransport" not in code
        assert "apiWorkerDispatch" not in code
        assert "if (value instanceof Entity) return value.raw;" in code
        assert "raw.root.createEntity(file.raw, encodeOptions(options" in code

    def test_direct_api_facade_marshals_sequences_without_collapsing_instance_lists(
        self,
    ):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
                "parse_instance_list": _make_handle("ifcopenshell_parse_instance_list_t"),
            },
            value_types={
                "instance_list": HostStructMetadata(
                    c_type="ifcopenshell_instance_list_t",
                    kind="handle_sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_instance_t**"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_instance_list_destroy",
                    element_type="ifcopenshell_instance_t",
                    sequence_depth=1,
                ),
                "double_list": HostStructMetadata(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "double*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "double_list_list": HostStructMetadata(
                    c_type="ifcopenshell_double_list_list_t",
                    kind="sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_double_list_t*"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_list_destroy",
                    element_type="ifcopenshell_double_list_t",
                    sequence_depth=2,
                ),
            },
            option_structs={
                "CreateOptions": HostOptionStructMetadata(
                    name="CreateOptions",
                    c_type="ifcopenshell_demo_create_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "products",
                            TypeSpec(kind="handle", handle="instance", sequence_depth=1),
                            "ifcopenshell_instance_list_t",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_demo_assign": _make_function(
                    c_name="ifcopenshell_demo_assign",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "products",
                            "const ifcopenshell_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
                "ifcopenshell_demo_set_axis": _make_function(
                    c_name="ifcopenshell_demo_set_axis",
                    params=(
                        HostParamMetadata(
                            "axis",
                            "const ifcopenshell_double_list_list_t*",
                            "param",
                            "double",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_opaque": _make_function(
                    c_name="ifcopenshell_demo_use_opaque",
                    params=(
                        HostParamMetadata(
                            "items",
                            "ifcopenshell_parse_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
                "ifcopenshell_demo_create": _make_function(
                    c_name="ifcopenshell_demo_create",
                    params=(
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_demo_create_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "assign(file: IfcFile, products: Entity[]): void;" in code
        assert "setAxis(axis: number[][]): void;" in code
        assert "create(options: IfcOpenShellDemoCreateOptions): void;" in code
        assert "raw.demo.assign(file.raw, toRawSequence(products, shell, temps));" in code
        assert "raw.demo.setAxis(toRawSequence(axis, shell, temps));" in code
        assert "raw.demo.useOpaque(toRaw(items, shell, temps));" in code
        assert "raw.demo.create(encodeOptions(options" in code
        assert "function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue" in code
        assert "if (Array.isArray(value)) return value.map((item) => toRawSequence(item, shell, temps));" in code
        assert "if (isEntityArray(value)) {" in code
        assert "instanceListCreateFromHandles" in code
        assert "raw.demo.assign(file.raw, toRaw(products, shell, temps))" not in code

    def test_direct_api_facade_maps_pset_properties(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "PsetEditPsetOptions": HostOptionStructMetadata(
                    name="PsetEditPsetOptions",
                    c_type="ifcopenshell_pset_edit_pset_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "pset",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "properties",
                            TypeSpec(kind="opaque_ptr", semantic="property_map"),
                            "void*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_pset_edit_pset": _make_function(
                    c_name="ifcopenshell_pset_edit_pset",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_pset_edit_pset_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_pset_props_new": _make_function(
                    c_name="ifcopenshell_pset_props_new",
                    returns=TypeSpec(kind="int32"),
                ),
                "ifcopenshell_pset_props_free": _make_function(c_name="ifcopenshell_pset_props_free"),
                "ifcopenshell_pset_props_set_string": _make_function(c_name="ifcopenshell_pset_props_set_string"),
            },
        )

        code = render_api_direct(metadata)

        assert "properties: PsetProperties | PsetInput;" in code
        assert "propsNew" not in code
        assert "return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);" in code

    def test_direct_api_facade_wraps_pset_template_handles(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
                "pset_template_handle": _make_handle("ifcopenshell_pset_template_handle_t"),
            },
            value_types={
                "file_list": HostStructMetadata(
                    c_type="ifcopenshell_file_list_t",
                    kind="handle_sequence",
                    fields=(
                        HostStructField("items", "ifcopenshell_file_t**"),
                        HostStructField("size", "size_t"),
                    ),
                    destroy_function=None,
                    element_type="ifcopenshell_file_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_pset_template_create_from_files": _make_function(
                    c_name="ifcopenshell_pset_template_create_from_files",
                    params=(
                        HostParamMetadata("schema_identifier", "const char*", "param", "string"),
                        HostParamMetadata(
                            "template_files",
                            "const ifcopenshell_file_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                    returns=TypeSpec(
                        kind="handle",
                        handle="pset_template_handle",
                        ownership="owned",
                        nullable=True,
                    ),
                ),
                "ifcopenshell_pset_template_get_template": _make_function(
                    c_name="ifcopenshell_pset_template_get_template",
                    params=(HostParamMetadata("schema_identifier", "const char*", "param", "string"),),
                    returns=TypeSpec(
                        kind="handle",
                        handle="pset_template_handle",
                        ownership="borrowed",
                        nullable=True,
                    ),
                ),
                "ifcopenshell_pset_template_get_by_name": _make_function(
                    c_name="ifcopenshell_pset_template_get_by_name",
                    params=(
                        HostParamMetadata(
                            "pqt",
                            "ifcopenshell_pset_template_handle_t*",
                            "param",
                            "handle",
                        ),
                        HostParamMetadata("name", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance", nullable=True),
                ),
                "ifcopenshell_pset_template_free": _make_function(
                    c_name="ifcopenshell_pset_template_free",
                    params=(
                        HostParamMetadata(
                            "pqt",
                            "ifcopenshell_pset_template_handle_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "export interface PsetTemplate" in code
        assert (
            "templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null;"
            in code
        )
        assert "templateGetTemplate(schema_identifier: string): PsetTemplate | null;" in code
        assert "templateGetByName(pqt: PsetTemplate, name: string): Entity | null;" in code
        assert "templateFree" not in code
        assert "free(pqt" not in code
        assert (
            "const result = raw.pset.templateCreateFromFiles(schema_identifier, toRawSequence(template_files, shell, temps));"
            in code
        )
        assert "return result as PsetTemplate | null;" in code
        assert "const result = raw.pset.templateGetByName(pqt, name);" in code
        assert "pqt.raw" not in code
        assert "PsetTemplate | null" in code
        assert "number | null" not in code

    def test_direct_api_facade_maps_pset_props_by_c_type(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "AttributeEditAttributesOptions": HostOptionStructMetadata(
                    name="AttributeEditAttributesOptions",
                    c_type="ifcopenshell_attribute_edit_attributes_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "product",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        HostOptionFieldMetadata(
                            "attributes",
                            TypeSpec(kind="opaque_ptr", semantic="property_map"),
                            "void*",
                        ),
                    ),
                ),
                "GeoreferenceEditGeoreferencingOptions": HostOptionStructMetadata(
                    name="GeoreferenceEditGeoreferencingOptions",
                    c_type="ifcopenshell_georeference_edit_georeferencing_options_t",
                    fields=(
                        HostOptionFieldMetadata(
                            "coordinate_operation",
                            TypeSpec(
                                kind="opaque_ptr",
                                nullable=True,
                                semantic="property_map",
                            ),
                            "void*",
                        ),
                        HostOptionFieldMetadata(
                            "projected_crs",
                            TypeSpec(
                                kind="opaque_ptr",
                                nullable=True,
                                semantic="property_map",
                            ),
                            "void*",
                        ),
                    ),
                ),
            },
            functions={
                "ifcopenshell_attribute_edit_attributes": _make_function(
                    c_name="ifcopenshell_attribute_edit_attributes",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_attribute_edit_attributes_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_georeference_edit_georeferencing": _make_function(
                    c_name="ifcopenshell_georeference_edit_georeferencing",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "options",
                            "const ifcopenshell_georeference_edit_georeferencing_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "attributes: PsetProperties | PsetInput;" in code
        assert "coordinateOperation?: PsetProperties | PsetInput;" in code
        assert "projectedCrs?: PsetProperties | PsetInput;" in code
        assert "attributes: number" not in code
        assert "coordinateOperation?: number" not in code
        assert "projectedCrs?: number" not in code
        assert "return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);" in code
        assert "propsFree" not in code
        assert "propsSetString" not in code

    def test_direct_api_facade_maps_property_maps_by_semantic(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            functions={
                "ifcopenshell_resource_edit_resource_time": _make_function(
                    c_name="ifcopenshell_resource_edit_resource_time",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata(
                            "resource_time",
                            "ifcopenshell_instance_t*",
                            "param",
                            "handle",
                        ),
                        HostParamMetadata(
                            "attributes",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                            semantic="property_map",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_opaque": _make_function(
                    c_name="ifcopenshell_demo_use_opaque",
                    params=(
                        HostParamMetadata(
                            "value",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_dynamic": _make_function(
                    c_name="ifcopenshell_demo_use_dynamic",
                    params=(HostParamMetadata("value", "void*", "param", "opaque_ptr"),),
                ),
                "ifcopenshell_demo_use_nullable": _make_function(
                    c_name="ifcopenshell_demo_use_nullable",
                    params=(
                        HostParamMetadata(
                            "attributes",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                            nullable=True,
                            semantic="property_map",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert (
            "editResourceTime(file: IfcFile, resource_time: Entity, attributes: PsetProperties | PsetInput): void;"
            in code
        )
        assert (
            "raw.resource.editResourceTime(file.raw, resource_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));"
            in code
        )
        assert "useOpaque(value: ApiData): void;" in code
        assert "raw.demo.useOpaque(value);" in code
        assert "useDynamic(value: ApiData): void;" in code
        assert "raw.demo.useDynamic(value);" in code
        assert "useNullable(attributes: PsetProperties | PsetInput | null): void;" in code
        assert (
            "raw.demo.useNullable(attributes == null ? null : toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));"
            in code
        )
        assert "raw.demo.useOpaque(toRawPsetProperties" not in code

    def test_direct_api_facade_hides_selector_internals(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={"file": _make_handle("ifcopenshell_file_t")},
            functions={
                "ifcopenshell_selector_filter_all": _make_function(
                    c_name="ifcopenshell_selector_filter_all",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata("query", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_selector_parse_keys": _make_function(
                    c_name="ifcopenshell_selector_parse_keys",
                    params=(HostParamMetadata("query", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="int32", nullable=True),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "filterAll(file: IfcFile, query: string)" in code
        assert "parseKeys" not in code

    def test_direct_api_facade_excludes_value_module(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "value": _make_handle("ifcopenshell_value_t"),
            },
            functions={
                "ifcopenshell_value_new_string": _make_function(
                    c_name="ifcopenshell_value_new_string",
                    params=(HostParamMetadata("value", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_new_list": _make_function(
                    c_name="ifcopenshell_value_new_list",
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_new_dict": _make_function(
                    c_name="ifcopenshell_value_new_dict",
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_list_append": _make_function(
                    c_name="ifcopenshell_value_list_append",
                    params=(
                        HostParamMetadata("list", "ifcopenshell_value_t*", "param", "handle"),
                        HostParamMetadata("item", "ifcopenshell_value_t*", "param", "handle"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_value_dict_set": _make_function(
                    c_name="ifcopenshell_value_dict_set",
                    params=(
                        HostParamMetadata("dict", "ifcopenshell_value_t*", "param", "handle"),
                        HostParamMetadata("key", "const char*", "param", "string"),
                        HostParamMetadata("value", "ifcopenshell_value_t*", "param", "handle"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_selector_filter_all": _make_function(
                    c_name="ifcopenshell_selector_filter_all",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata("query", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "export interface ValueApi" not in code
        assert "value: ValueApi;" not in code
        assert "newString" not in code
        assert "newList" not in code
        assert "newDict" not in code
        assert "listAppend" not in code
        assert "dictSet" not in code
        assert "filterAll(file: IfcFile, query: string): ValueData | null;" in code

    def test_direct_api_facade_maps_selector_value_params_to_value_input(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "value": _make_handle("ifcopenshell_value_t"),
            },
            functions={
                "ifcopenshell_selector_set_element_value": _make_function(
                    c_name="ifcopenshell_selector_set_element_value",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata("element", "ifcopenshell_instance_t*", "param", "handle"),
                        HostParamMetadata("query", "const char*", "param", "string"),
                        HostParamMetadata("value", "ifcopenshell_value_t*", "param", "handle"),
                        HostParamMetadata("concat", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "value: ValueInput" in code
        assert "setElementValue(file: IfcFile" in code

    def test_direct_api_facade_maps_nullable_result_structs(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            value_types={
                "mep_result": HostStructMetadata(
                    c_type="ifcopenshell_shape_builder_mep_transition_shape_result_t",
                    kind="result_struct",
                    fields=(
                        HostStructField("representation", "ifcopenshell_instance_t*"),
                        HostStructField("start_length", "double"),
                    ),
                    destroy_function=None,
                ),
                "optional_mep_result": HostStructMetadata(
                    c_type="ifcopenshell_optional_shape_builder_mep_transition_shape_result_t",
                    kind="optional_result_struct",
                    fields=(
                        HostStructField("has_value", "bool"),
                        HostStructField(
                            "value",
                            "ifcopenshell_shape_builder_mep_transition_shape_result_t",
                        ),
                    ),
                    destroy_function=None,
                    element_type="mep_result",
                ),
            },
            functions={
                "ifcopenshell_shape_builder_mep_transition_shape": _make_function(
                    c_name="ifcopenshell_shape_builder_mep_transition_shape",
                    params=(HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),),
                    returns=TypeSpec(kind="struct", struct="mep_result", nullable=True),
                )
            },
        )

        code = render_api_direct(metadata)

        assert (
            "builderMepTransitionShape(file: IfcFile): IfcOpenShellShapeBuilderMepTransitionShapeResult | null;" in code
        )
        assert "if (result === null) return null;" in code
        assert "hasResult" not in code

    def test_direct_api_facade_maps_variants_to_unions(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            value_types={
                "entity_string_variant": HostStructMetadata(
                    c_type="ifcopenshell_instance_string_variant_t",
                    kind="variant",
                    fields=(
                        HostStructField("kind", "int32_t"),
                        HostStructField("value_0", "ifcopenshell_instance_t*"),
                        HostStructField("value_1", "ifcopenshell_string_t"),
                    ),
                    destroy_function=None,
                    element_type="std::variant<express::Base, std::string>",
                ),
            },
            functions={
                "ifcopenshell_sequence_add_date_time": _make_function(
                    c_name="ifcopenshell_sequence_add_date_time",
                    params=(
                        HostParamMetadata("file", "ifcopenshell_file_t*", "param", "handle"),
                        HostParamMetadata("date_time", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(
                        kind="variant",
                        variants=(
                            TypeSpec(
                                kind="handle",
                                handle="instance",
                                cpp_type="express::Base",
                            ),
                            TypeSpec(kind="string", cpp_type="std::string"),
                        ),
                        cpp_type="std::variant<express::Base, std::string>",
                    ),
                )
            },
        )

        code = render_api_direct(metadata)

        assert "addDateTime(file: IfcFile, date_time: string): Entity | string;" in code
        assert "return wrap(shell, result) as Entity | string;" in code
        assert "isEntity:" not in code
        assert "dateTimeString" not in code


class TestWasmBackend:
    def test_generate_writes_javascript_and_declaration_artifacts(self, tmp_path: Path):
        output_dir = tmp_path / "wasm-target"
        metadata = _make_metadata(handles={"file": _make_handle("ifcopenshell_demo_file_t")})

        artifacts = WasmTargetBackend().generate(
            TargetGenerationRequest(
                ir=_make_ir(module=metadata.module, c_prefix=metadata.c_prefix),
                metadata=metadata,
                api_header_path=None,
                options={},
                output_dir=output_dir,
            )
        )

        paths = {artifact.kind: artifact.path for artifact in artifacts.artifacts}
        assert paths["javascript"] == (output_dir / "ifcopenshell_api.js").resolve()
        assert paths["typescript"] == (output_dir / "ifcopenshell_api.d.ts").resolve()
        assert paths["exports"] == (output_dir / "ifcopenshell_exports.txt").resolve()
        assert paths["api"] == (output_dir / "ifcopenshell_api.ts").resolve()
        assert paths["javascript"].read_text(encoding="utf-8").startswith("// This file was generated")
        assert "declare module 'ifcopenshell-api'" in paths["typescript"].read_text(encoding="utf-8")
        assert "_ifcopenshell_demo_clear_error" in paths["exports"].read_text(encoding="utf-8")

    def test_generate_supports_exports_only(self, tmp_path: Path):
        output_dir = tmp_path / "wasm-target"
        metadata = _make_metadata(handles={"file": _make_handle("ifcopenshell_demo_file_t")})

        artifacts = WasmTargetBackend().generate(
            TargetGenerationRequest(
                ir=_make_ir(module=metadata.module, c_prefix=metadata.c_prefix),
                metadata=metadata,
                api_header_path=None,
                options={},
                output_dir=output_dir,
                exports_only=True,
            )
        )

        assert [(artifact.kind, artifact.path.name) for artifact in artifacts.artifacts] == [
            ("exports", "ifcopenshell_exports.txt")
        ]

    def test_registry_returns_wasm_backend(self):
        assert isinstance(get_target("wasm"), WasmTargetBackend)
        assert any(isinstance(target, WasmTargetBackend) for target in discover_targets())

    def test_render_wasm_bindings_returns_both_outputs(self):
        metadata = _make_metadata(handles={"file": _make_handle("ifcopenshell_demo_file_t")})
        javascript, declarations = render_wasm_bindings(metadata)
        assert "createIfcOpenshellModule" in javascript
        assert "IfcOpenshellModule" in declarations

    def test_render_export_list_includes_runtime_and_destroy_helpers(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            value_types={
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
            },
            functions={"ifcopenshell_demo_open": _make_function(c_name="ifcopenshell_demo_open")},
        )
        export_list = render_export_list(metadata)
        assert "_malloc" in export_list
        assert "_free" in export_list
        assert "_ifcopenshell_demo_open" in export_list
        assert "_ifcopenshell_demo_file_destroy" in export_list
        assert "_ifcopenshell_string_destroy" in export_list

    def test_generate_raises_when_api_header_is_missing_required_symbols(self, tmp_path: Path):
        output_dir = tmp_path / "wasm-target"
        header_path = tmp_path / "ifcopenshell_api.h"
        header_path.write_text(
            "bool ifcopenshell_demo_clear_error(void);\n"
            "void ifcopenshell_demo_file_destroy(ifcopenshell_demo_file_t* handle);\n",
            encoding="utf-8",
        )
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={"ifcopenshell_demo_open": _make_function(c_name="ifcopenshell_demo_open")},
        )

        with pytest.raises(ValueError, match="ifcopenshell_demo_open"):
            WasmTargetBackend().generate(
                TargetGenerationRequest(
                    ir=_make_ir(module=metadata.module, c_prefix=metadata.c_prefix),
                    metadata=metadata,
                    api_header_path=header_path,
                    options={},
                    output_dir=output_dir,
                )
            )

    def test_generate_accepts_non_bool_non_void_api_declarations(self, tmp_path: Path):
        output_dir = tmp_path / "wasm-target"
        header_path = tmp_path / "ifcopenshell_api.h"
        header_path.write_text(
            "void ifcopenshell_demo_clear_error(void);\n"
            "const char* ifcopenshell_demo_last_error_message(void);\n"
            "int ifcopenshell_demo_last_error_kind(void);\n"
            "void ifcopenshell_demo_file_destroy(ifcopenshell_demo_file_t* handle);\n"
            "bool ifcopenshell_demo_open(void);\n",
            encoding="utf-8",
        )
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={"ifcopenshell_demo_open": _make_function(c_name="ifcopenshell_demo_open")},
        )

        artifacts = WasmTargetBackend().generate(
            TargetGenerationRequest(
                ir=_make_ir(module=metadata.module, c_prefix=metadata.c_prefix),
                metadata=metadata,
                api_header_path=header_path,
                options={},
                output_dir=output_dir,
            )
        )

        assert {artifact.kind for artifact in artifacts.artifacts} == {
            "javascript",
            "typescript",
            "api",
            "exports",
        }
