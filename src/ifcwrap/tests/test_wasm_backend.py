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

from src.ifcwrap.binding_generator.binding_ir import BindingIR
from src.ifcwrap.binding_generator.binding_model import TypeSpec
from src.ifcwrap.binding_generator.host_metadata import (
    HostBindingMetadata,
    HostFunctionMetadata,
    HostParamMetadata,
    HostStructField,
    HostStructMetadata,
)
from src.ifcwrap.binding_generator.targets import TargetGenerationRequest, discover_targets, get_target
from src.ifcwrap.binding_generator.targets.wasm import WasmTargetBackend
from src.ifcwrap.binding_generator.targets.wasm.backend import render_export_list, render_wasm_bindings
from src.ifcwrap.binding_generator.targets.wasm.js_glue import render_js_glue
from src.ifcwrap.binding_generator.targets.wasm.typescript import render_typescript_declarations


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
        destroy_function=destroy_function or f"ifcopenshell_{c_type.removeprefix('ifcopenshell_').removesuffix('_t')}_destroy",
        layout="ptr_owned",
    )


def _make_function(
    *,
    c_name: str,
    params: tuple[HostParamMetadata, ...] = (),
    returns: TypeSpec | None = None,
    receiver: str | None = None,
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
    )


def _make_metadata(
    *,
    handles: dict[str, HostStructMetadata] | None = None,
    value_types: dict[str, HostStructMetadata] | None = None,
    functions: dict[str, HostFunctionMetadata] | None = None,
) -> HostBindingMetadata:
    return HostBindingMetadata(
        module="ifcopenshell_wrapper",
        c_prefix="ifcopenshell_demo",
        handles=handles or {},
        value_types=value_types or {},
        functions=functions or {},
        error_functions=_DEFAULT_ERROR_FUNCTIONS,
    )


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
            "openBytes(bytes: Uint8Array | ArrayBuffer | ArrayBufferView, "
            "filename?: string, readonly?: boolean): void;"
        ) in code
        assert "export interface IfcOpenshellGeomModule" in code
        assert "createSettings(): IfcOpenshellGeomSettings;" in code
        assert "export interface IfcOpenshellUnitModule" in code
        assert "addSiUnit(unit_type: string): void;" in code
        assert "parse: IfcOpenshellParseModule;" in code
        assert "geom: IfcOpenshellGeomModule;" in code
        assert "unit: IfcOpenshellUnitModule;" in code
        assert "    createSettings(): IfcOpenshellGeomSettings;" not in code.split(
            "export interface IfcOpenshellModule"
        )[1]


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
        assert "module.loadDynamicLibrary(path, { global: true, allowUndefined: true });" in code
        assert "module.FS.unlink(path);" in code
        assert "import initIfcOpenShellWasmModule" not in code

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
            "        geom: Object.freeze({"
            not in code
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
                    fields=(HostStructField("items", "int32_t*"), HostStructField("size", "size_t")),
                    destroy_function="ifcopenshell_int32_list_destroy",
                    element_type="int32_t",
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
                    params=(HostParamMetadata("values", "const ifcopenshell_int32_list_t*", "param", "int32"),),
                    returns=TypeSpec(kind="int32"),
                ),
            },
        )
        code = render_js_glue(metadata)
        assert "const low = module.HEAP32[index];" in code
        assert "const high = module.HEAP32[index + 1];" in code
        assert "_allocInputSequence(module, values" in code
        assert "_freeInputSequence(module, _valuesPtr" in code

    def test_generates_dependency_aware_plugin_loader(self):
        metadata = _make_metadata()
        code = render_js_glue(metadata)
        assert "const loadingPlugins = new Map();" in code
        assert "function pluginDependencies(kind, id)" in code
        assert "for (const dependency of pluginDependencies(kind, id))" in code
        assert "await loadPlugin(dependency.slice(0, separator), dependency.slice(separator + 1));" in code
        assert "async function loadPluginLibrary(kind, id, entry)" in code
        assert "module.loadDynamicLibrary(path, { global: true, allowUndefined: true });" in code
        assert "invoke_ifcopenshell_geom_plugin_registry_address" not in code


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

        assert {artifact.kind for artifact in artifacts.artifacts} == {"javascript", "typescript", "exports"}
