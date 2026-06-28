# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shutil
import subprocess
import textwrap

import pytest


REPO_ROOT = Path(__file__).resolve().parents[3]
SOURCE_DIR = REPO_ROOT / "cmake"
DEFAULT_BUILD_DIR = REPO_ROOT / "build-wasm"
BOOST_ROOT = Path("/opt/homebrew/Cellar/boost@1.85/1.85.0_3")
BOOST_INCLUDE_DIR = BOOST_ROOT / "include"
EIGEN3_DIR = Path("/opt/homebrew/share/eigen3/cmake")
TEST_IFC = REPO_ROOT / "test" / "input" / "WallInstance_IFC4Add2.ifc"
PYTHON_EXECUTABLE = Path("/opt/homebrew/opt/python@3.11/bin/python3.11")


def _tool_path(preferred: str | Path, fallback: str) -> str | None:
    path = Path(preferred)
    if path.exists():
        return str(path)
    return shutil.which(fallback)


def _run(command: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(command, cwd=cwd, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
    return completed


def _ensure_configured(build_dir: Path) -> None:
    emcmake = _tool_path("/opt/homebrew/bin/emcmake", "emcmake")
    cmake = _tool_path("/opt/homebrew/bin/cmake", "cmake")
    if emcmake is None or cmake is None:
        pytest.skip("cmake/emcmake not installed")
    if not BOOST_INCLUDE_DIR.exists():
        pytest.skip(f"Boost headers not found at {BOOST_INCLUDE_DIR}")
    if not EIGEN3_DIR.exists():
        pytest.skip(f"Eigen3 CMake config not found at {EIGEN3_DIR}")
    if not PYTHON_EXECUTABLE.exists():
        pytest.skip(f"Python with PyYAML not found at {PYTHON_EXECUTABLE}")

    build_dir.mkdir(exist_ok=True)
    _run(
        [
            emcmake,
            cmake,
            "-S",
            str(SOURCE_DIR),
            "-B",
            str(build_dir),
            "-DWASM_BUILD=ON",
            "-DBUILD_IFCCAPI=ON",
            "-DBUILD_IFCAPI=ON",
            "-DBUILD_IFCGEOM=ON",
            "-DBUILD_IFCPYTHON=OFF",
            "-DBUILD_CONVERT=OFF",
            "-DBUILD_GEOMSERVER=OFF",
            "-DBUILD_EXAMPLES=OFF",
            "-DWITH_OPENCASCADE=OFF",
            "-DWITH_CGAL=OFF",
            "-DWITH_MANIFOLD=OFF",
            "-DCOLLADA_SUPPORT=OFF",
            "-DGLTF_SUPPORT=OFF",
            f"-DBOOST_ROOT={BOOST_ROOT}",
            f"-DBoost_INCLUDE_DIR={BOOST_INCLUDE_DIR}",
            f"-DEigen3_DIR={EIGEN3_DIR}",
            f"-DPYTHON_EXECUTABLE={PYTHON_EXECUTABLE}",
        ],
        cwd=REPO_ROOT,
    )


def _ensure_built(build_dir: Path) -> Path:
    cmake = _tool_path("/opt/homebrew/bin/cmake", "cmake")
    if cmake is None:
        pytest.skip("cmake not installed")
    _run([cmake, "--build", str(build_dir), "--target", "ifcopenshell_wasm", "--parallel", "10"], cwd=REPO_ROOT)
    output_dir = build_dir / "ifcwrap" / "wasm"
    assert (output_dir / "ifcopenshell_wasm.mjs").exists()
    assert (output_dir / "ifcopenshell_wasm.wasm").exists()
    assert (output_dir / "ifcopenshell_api.mjs").exists()
    assert (output_dir / "ifcopenshell_plugins.json").exists()
    assert (output_dir / "plugins").is_dir()
    return output_dir


def _render_node_test(ifc_path: Path) -> str:
    return textwrap.dedent(
        f"""
        import assert from 'node:assert/strict';
        import {{ readFileSync }} from 'node:fs';
        import initIfcOpenShellWasmModule from './ifcopenshell_wasm.mjs';
        import {{ createIfcOpenshellModule }} from './ifcopenshell_api.mjs';
        import {{ fileURLToPath }} from 'node:url';

        const wasmUrl = fileURLToPath(new URL('./ifcopenshell_wasm.wasm', import.meta.url));
        const pluginManifest = JSON.parse(readFileSync(new URL('./ifcopenshell_plugins.json', import.meta.url), 'utf8'));
        const pluginBaseUrl = new URL('./', import.meta.url).href;
        const api = await createIfcOpenshellModule(initIfcOpenShellWasmModule, wasmUrl, {{
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
        const rootWall = api.root.createEntity(newFile, 'IfcWall', null, 'Smoke Wall', null);
        assert.equal(rootWall.className(false), 'IfcWall');
        assert.match(rootWall.toString(false), /Smoke Wall/);

        step('pset-add');
        const pset = api.pset.addPset(newFile, rootWall, 'Pset_WasmSmoke', null, null, null, null);
        assert.equal(pset.className(false), 'IfcPropertySet');
        assert.match(pset.toString(false), /Pset_WasmSmoke/);

        step('pset-edit');
        const props = api.pset.propsNew();
        api.pset.propsSetString(props, 'Reference', 'ABC');
        api.pset.propsSetDouble(props, 'Height', 3.25);
        api.pset.propsSetBool(props, 'IsExternal', true);
        assert.equal(api.pset.editPset(newFile, pset, 'Pset_WasmSmokeEdited', props, null, true), true);
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


def test_generated_wasm_bindings_work_in_node():
    node = _tool_path("/Users/theseyan/.nvm/versions/node/v22.15.0/bin/node", "node")
    if node is None:
        pytest.skip("node not installed")
    if not TEST_IFC.exists():
        pytest.skip(f"Missing IFC fixture at {TEST_IFC}")

    build_dir = DEFAULT_BUILD_DIR
    _ensure_configured(build_dir)
    output_dir = _ensure_built(build_dir)

    node_script = output_dir / "test_wasm_bindings.mjs"
    node_script.write_text(_render_node_test(TEST_IFC), encoding="utf-8")

    completed = subprocess.run([node, str(node_script)], cwd=output_dir, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stderr or completed.stdout
