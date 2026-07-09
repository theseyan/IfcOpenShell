# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
import os
from pathlib import Path
import shutil
import subprocess
import textwrap

import pytest


REPO_ROOT = Path(__file__).resolve().parents[3]
DEFAULT_WASM_DIR = REPO_ROOT / "build" / "wasm-native" / "ifcopenshell" / "full" / "ifcwrap" / "wasm"
TEST_IFC = REPO_ROOT / "test" / "input" / "WallInstance_IFC4Add2.ifc"
WASM_DIR_ENV = "IFCOPENSHELL_WASM_DIR"


def _tool_path(preferred: str | Path, fallback: str) -> str | None:
    path = Path(preferred)
    if path.exists():
        return str(path)
    return shutil.which(fallback)


def _wasm_dir() -> Path:
    configured = os.environ.get(WASM_DIR_ENV)
    if configured:
        path = Path(configured).expanduser().resolve()
        assert path.is_dir(), f"{WASM_DIR_ENV} does not point to a directory: {path}"
        return path
    if DEFAULT_WASM_DIR.is_dir():
        return DEFAULT_WASM_DIR
    pytest.skip(
        f"prebuilt WASM artifacts not found at {DEFAULT_WASM_DIR}; "
        f"set {WASM_DIR_ENV} to a prebuilt ifcwrap/wasm directory"
    )


def _require_artifacts(wasm_dir: Path) -> None:
    required_files = [
        "ifcopenshell_wasm.node.mjs",
        "ifcopenshell_wasm.wasm",
        "ifcopenshell_api.mjs",
        "ifcopenshell_plugins.json",
    ]
    for name in required_files:
        assert (wasm_dir / name).exists(), f"missing WASM smoke artifact: {wasm_dir / name}"
    assert (wasm_dir / "plugins").is_dir(), f"missing WASM plugin directory: {wasm_dir / 'plugins'}"


def _render_node_test(wasm_dir: Path, ifc_path: Path) -> str:
    return textwrap.dedent(
        f"""
        import assert from 'node:assert/strict';
        import {{ readFileSync }} from 'node:fs';
        import {{ fileURLToPath, pathToFileURL }} from 'node:url';

        const wasmRoot = {json.dumps(str(wasm_dir))};
        const moduleUrl = (name) => pathToFileURL(`${{wasmRoot}}/${{name}}`).href;
        const wasmUrl = `${{wasmRoot}}/ifcopenshell_wasm.wasm`;
        const initFactory = (await import(moduleUrl('ifcopenshell_wasm.node.mjs'))).default;
        const {{ createIfcOpenshellModule }} = await import(moduleUrl('ifcopenshell_api.mjs'));
        const initModule = (options = {{}}) => initFactory({{
            ...options,
            wasmBinary: readFileSync(wasmUrl),
        }});
        const pluginManifest = JSON.parse(readFileSync(`${{wasmRoot}}/ifcopenshell_plugins.json`, 'utf8'));
        const pluginBaseUrl = pathToFileURL(`${{wasmRoot}}/`).href;
        const api = await createIfcOpenshellModule(initModule, wasmUrl, {{
            pluginBaseUrl,
            pluginManifest,
            pluginLoader: (url) => readFileSync(fileURLToPath(url)),
        }});
        const step = (name) => console.error(`wasm-smoke:${{name}}`);

        step('load-schemas');
        await api.loadPlugin('schema', 'ifc4');
        await api.loadPlugin('schema', 'ifc2x3');

        step('new-files');
        const newFile = api.parse.newFile('IFC4', 0, '');
        assert.equal(newFile.schemaName(), 'IFC4');
        const ifc2x3File = api.parse.newFile('IFC2X3', 0, '');
        assert.equal(ifc2x3File.schemaName(), 'IFC2X3');

        step('root-create-wall');
        const rootWall = api.root.createEntity(newFile, {{
            ifc_class: 'IfcWall',
            name: 'Smoke Wall',
        }});
        assert.equal(rootWall.className(false), 'IfcWall');
        assert.match(rootWall.toString(false), /Smoke Wall/);

        step('pset-add');
        const pset = api.pset.addPset(newFile, {{
            product: rootWall,
            name: 'Pset_WasmSmoke',
        }});
        assert.equal(pset.className(false), 'IfcPropertySet');
        assert.match(pset.toString(false), /Pset_WasmSmoke/);

        step('pset-edit');
        const props = api.pset.propsNew();
        assert.ok(props);
        api.pset.propsSetString(props, 'Reference', 'ABC');
        api.pset.propsSetDouble(props, 'Height', 3.25);
        api.pset.propsSetBool(props, 'IsExternal', true);
        assert.equal(api.pset.editPset(newFile, {{
            pset,
            name: 'Pset_WasmSmokeEdited',
            properties: props,
            should_purge: true,
        }}), true);
        api.pset.propsFree(props);
        assert.match(pset.toString(false), /Pset_WasmSmokeEdited/);
        const hasPropertiesAttr = pset.getArgument(4);
        const editedProps = hasPropertiesAttr.asInstanceList();
        assert.equal(editedProps.size(), 3);
        const findEditedProp = (name) => {{
            for (let index = 0; index < editedProps.size(); index += 1) {{
                const prop = editedProps.get(index);
                const nameValue = prop.getArgument(0);
                const propName = nameValue.asString();
                nameValue.destroy();
                if (propName === name) return prop;
                prop.destroy();
            }}
            return null;
        }};
        const referenceProp = findEditedProp('Reference');
        assert.ok(referenceProp);
        const referenceNominal = referenceProp.getArgument(2);
        const referenceValue = referenceNominal.asInstance();
        const referenceWrappedValue = referenceValue.getArgument(0);
        assert.equal(referenceWrappedValue.asString(), 'ABC');
        referenceWrappedValue.destroy();
        referenceValue.destroy();
        referenceNominal.destroy();
        referenceProp.destroy();
        const heightProp = findEditedProp('Height');
        assert.ok(heightProp);
        const heightNominal = heightProp.getArgument(2);
        const heightValue = heightNominal.asInstance();
        const heightWrappedValue = heightValue.getArgument(0);
        assert.equal(heightWrappedValue.asDouble(), 3.25);
        heightWrappedValue.destroy();
        heightValue.destroy();
        heightNominal.destroy();
        heightProp.destroy();
        const isExternalProp = findEditedProp('IsExternal');
        assert.ok(isExternalProp);
        const isExternalNominal = isExternalProp.getArgument(2);
        const isExternalValue = isExternalNominal.asInstance();
        const isExternalWrappedValue = isExternalValue.getArgument(0);
        assert.equal(isExternalWrappedValue.asBool(), true);
        isExternalWrappedValue.destroy();
        isExternalValue.destroy();
        isExternalNominal.destroy();
        isExternalProp.destroy();
        editedProps.destroy();
        hasPropertiesAttr.destroy();

        step('file-create-project');
        const project = newFile.createEntityByName('IfcProject');
        assert.equal(project.className(false), 'IfcProject');
        step('file-create-wall');
        const wall = newFile.createEntityByName('IfcWall');
        assert.equal(wall.className(false), 'IfcWall');
        step('string-list-returns');
        assert.ok(newFile.types().includes('IfcWall'));
        const wallAttributeNames = rootWall.getAttributeNames();
        assert.ok(wallAttributeNames.includes('GlobalId'));
        assert.ok(wallAttributeNames.includes('Name'));

        step('unit-add-si');
        const metre = api.unit.addSiUnit(newFile, 'LENGTHUNIT', null);
        assert.equal(metre.className(false), 'IfcSIUnit');
        step('unit-full-name');
        assert.equal(api.unit.getFullUnitName(metre), 'METRE');
        step('unit-convert');
        assert.equal(api.unit.convert(1000, 'MILLI', 'METRE', '', 'METRE'), 1);

        step('placement-rotation');
        assert.deepEqual(api.placement.rotation(Math.PI / 2, 'Z').slice(0, 4).map((v) => Math.round(v)), [0, -1, 0, 0]);
        step('shape-is-x');
        assert.equal(api.shape.isX(1.0000001, 1, 0.001), true);
        step('schema-checks');
        const ifc4Schema = api.parse.schemaByName('IFC4');
        assert.equal(ifc4Schema.name(), 'IFC4');
        const ifc2x3Schema = api.parse.schemaByName('IFC2X3');
        assert.equal(ifc2x3Schema.name(), 'IFC2X3');

        await api.loadPlugin('schema', 'ifc4');
        assert.ok(api.loadedPlugins().includes('schema:ifc4'));
        assert.ok(api.loadedPlugins().includes('schema:ifc2x3'));

        await api.loadPlugin('kernel', 'passthrough');
        await api.loadPlugin('mapping', 'ifc4');
        await api.loadPlugin('geometry_serializer', 'obj');
        assert.ok(api.loadedPlugins().includes('kernel:passthrough'));
        assert.ok(api.loadedPlugins().includes('mapping:ifc4'));
        assert.ok(api.loadedPlugins().includes('geometry_serializer:obj'));

        const settings = api.geom.createSettings();
        const iterator = api.geom.createIterator('passthrough', settings, newFile, 1);
        assert.ok(iterator);

        const opened = api.parse.openBytes(
            readFileSync({json.dumps(str(ifc_path))}),
            {json.dumps(ifc_path.name)},
            false,
        );
        assert.equal(opened.schemaName(), 'IFC4');

        iterator.destroy();
        settings.destroy();
        metre.destroy();
        wall.destroy();
        pset.destroy();
        rootWall.destroy();
        project.destroy();
        ifc2x3Schema.destroy();
        ifc4Schema.destroy();
        opened.destroy();
        ifc2x3File.destroy();
        newFile.destroy();
        """
    ).strip() + "\n"


def test_generated_wasm_bindings_work_in_node(tmp_path: Path):
    node = _tool_path("/Users/theseyan/.nvm/versions/node/v22.15.0/bin/node", "node")
    if node is None:
        pytest.skip("node not installed")
    if not TEST_IFC.exists():
        pytest.skip(f"Missing IFC fixture at {TEST_IFC}")

    wasm_dir = _wasm_dir()
    _require_artifacts(wasm_dir)

    node_script = tmp_path / "test_wasm_bindings.mjs"
    node_script.write_text(_render_node_test(wasm_dir, TEST_IFC), encoding="utf-8")

    completed = subprocess.run([node, str(node_script)], cwd=wasm_dir, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
