# @ifcopenshell-js/web

High-level TypeScript/JavaScript IFC library for browser and Node, built on [IfcOpenShell](https://ifcopenshell.org/) WASM.

This is the production-facing API surface (comparable in intent to `ifcopenshell-python`), not a thin ABI wrapper. The package initializes the WASM runtime directly; applications that need off-thread execution can run the library inside their own worker.

## Install

```bash
npm install @ifcopenshell-js/web
```

`@ifcopenshell-js/wasm` is a runtime dependency of `@ifcopenshell-js/web`; installing
the library also installs the packaged WASM binaries and generated API glue.

For local repository development, stage WASM artifacts into
`packages/ifcopenshell-wasm` before running tests or the demo:

```bash
cd packages/ifcopenshell-wasm && npm run stage
```

## Browser quick start

```ts
import { init, IfcFile } from '@ifcopenshell-js/web';

const api = await init();
await api.loadPlugin('schema', 'ifc4');

await using file = await IfcFile.open(api, ifcBytes, 'model.ifc');
const info = file.info();
const wall = file.get(315);
const wallName = wall?.get('Name');
const walls = file.all('IfcWall');
const ids = file.ids;
const elementInfo = await file.inspect(315);
```

Browser bundlers such as Vite discover the packaged WASM assets through static
`new URL(..., import.meta.url)` references in `@ifcopenshell-js/wasm`, then copy
and rewrite those URLs into the application build output. No app-specific
`/wasm` copy plugin is required.

The generated asset manifest references all packaged plugins so applications
can load additional plugins dynamically at runtime.

## Browser Bundlers

| Bundler | Default behavior |
|---------|------------------|
| Vite | Works without extra config. The build emits the main WASM and all plugin WASM files referenced by the asset manifest. |
| webpack 5 | Works without extra config in a browser build. Asset modules emit the main WASM and all plugin WASM files. |
| Rollup | Core Rollup plus `@rollup/plugin-node-resolve` resolves the package, but does not copy `new URL(..., import.meta.url)` package assets by itself. Use a copy/static-assets plugin, or serve the packaged `wasm/` directory yourself and pass `resolveUrls(baseUrl)`. |
| esbuild | Core esbuild bundles the JS, but does not copy these package assets from `new URL(..., import.meta.url)`. Use an asset-copy plugin or serve the packaged `wasm/` directory yourself and pass `resolveUrls(baseUrl)`. |

## Node quick start

```ts
import { init } from '@ifcopenshell-js/web';

const api = await init();
await api.loadPlugin('schema', 'ifc4');
```

`init()` runs the WASM runtime directly in browser and Node. Owned wrapper
objects release native handles automatically when they are garbage collected.
Use `dispose()` / `await using` when you want deterministic cleanup at a known
point, especially while iterating large models or geometry.

The parse/geometry foundation is intentionally high-level:
files expose header/status/inverse/traversal helpers plus property-style
summary data (`schema`, `ids`, `types`, `entityCount`, `maxId`), entities expose
attribute metadata plus `get()` / `set()` / `unset()` and plain `info()` objects,
attribute and selector values provide typed conversions and decoded `value()`, geometry
iteration supports include/exclude filters by type, GlobalId, or entity id, and
`GeometryTree` provides spatial selection and raycasting without exposing native
tree/vector handles.

Generated high-level `api.*` modules use JS-friendly values at the public
boundary. Native instance-list handles are represented as `Entity[]` in
arguments and return values:

```ts
const project = file.create('IfcProject', { name: 'Project' });
const site = file.create('IfcSite', { name: 'Site' });

shell.api.aggregate.assignObject(file, {
  relatingObject: project,
  products: [site],
});

const parts = shell.api.element.getParts(project); // Entity[]
```

Meshes returned from geometry iteration use typed-array buffers for vertices,
faces, normals, transforms, edges, UVs, material IDs, item IDs, edge item IDs,
and colors.

## Subpath exports

| Import | Contents |
|--------|----------|
| `@ifcopenshell-js/web` | `init`, `IfcFile`, `Entity`, core types |
| `@ifcopenshell-js/web/geom` | `GeomIterator`, `GeomSettings`, `Mesh` |
| `@ifcopenshell-js/web/serializers` | `exportToBuffer`, `SerializerSettings` |
| `@ifcopenshell-js/web/util` | `inspectEntity`, `meshColor`, helpers |

## Development

```bash
# Stage WASM assets (full profile recommended)
python nix/wasm_native.py --profile full build
cd ../ifcopenshell-wasm && npm run stage

cd ../ifcopenshell-js
npm install
npm test
```

## Demo

See [`demo/`](demo/) for the React + Three.js browser app.
