# @ifcopenshell-js/web

TypeScript and JavaScript APIs for reading, editing, inspecting, and generating
IFC models in the browser or Node.js, powered by [IfcOpenShell](https://ifcopenshell.org/).

## Install

```bash
npm install @ifcopenshell-js/web
```

## Quick start

The same API works in both browser and Node.js applications. Load the IFC bytes
with `fetch`, a file input, or your runtime's filesystem API:

```ts
import { IfcFile, init } from '@ifcopenshell-js/web';

const response = await fetch('/model.ifc');
const ifcBytes = new Uint8Array(await response.arrayBuffer());
const shell = await init();

await shell.loadPlugin('schema', 'ifc4');

const file = await IfcFile.open(shell, ifcBytes, 'model.ifc');
console.log(file.schema, file.entityCount);

const wall = file.get(315);
console.log(wall?.get('Name'));
console.log(file.all('IfcWall'));
```

## Working with IFC

Read and edit entities through the high-level wrappers:

```ts
const wall = file.get(315);
if (wall) {
  const name = wall.get('Name');
  wall.set('Description', 'Updated from TypeScript');
  console.log({ id: wall.id, type: wall.type, name });
}

const inspection = await file.inspect(315);
const walls = file.all('IfcWall');
```

Generated `api.*` modules are available from the initialized runtime:

```ts
const project = file.create('IfcProject', { name: 'Project' });
const site = file.create('IfcSite', { name: 'Site' });

shell.api.aggregate.assignObject(file, {
  relatingObject: project,
  products: [site],
});

const parts = shell.api.element.getParts(project);
```

## Geometry

Use the geometry subpath for settings, mesh iteration, and spatial queries:

```ts
import { GeomSettings } from '@ifcopenshell-js/web/geom';

const settings = new GeomSettings(shell);
const iterator = file.meshes(settings, { kernel: 'passthrough' });

const result = await iterator.collect({ limit: 100, skipEmpty: true });
for (const mesh of result.meshes) {
  console.log(mesh.type, mesh.vertices, mesh.faces);
}
```

Meshes expose typed-array buffers for vertices, faces, normals, transforms,
edges, UVs, material IDs, item IDs, and colors. `GeometryTree` provides point,
box, entity, and ray intersection queries.

Floating geometry buffers use detached `Float32Array` snapshots by default.
CPU-side analytical workflows can opt into detached `Float64Array` snapshots;
this does not increase GPU precision after a renderer uploads the data:

```ts
const precise = file.meshes(settings, { kernel: 'passthrough', precision: 'float64' });
const mesh64 = await precise.nextMesh();
```

`Mesh.transform` is a column-major `Float64Array`, so Three.js can consume it
directly with `object.matrix.fromArray(mesh.transform)`. Generated placement
utilities such as `shell.api.placement.getLocalPlacement()` return documented
row-major matrices. Convert those without a renderer dependency:

```ts
import { rowMajorToColumnMajor4 } from '@ifcopenshell-js/web/geom';

const rowMajor = shell.api.placement.getLocalPlacement(placement);
object.matrix.fromArray(rowMajorToColumnMajor4(rowMajor));
```

Progress ratios are normalized to `0..1`. Collection `current` counts native
items processed, while `meshes` counts retained meshes after `skipEmpty`.
`AbortSignal` is checked between synchronous native calls and cannot interrupt
one long native call already in progress.

## Serializers and utilities

The serializers subpath provides `exportToBuffer` and `SerializerSettings` for
OBJ, SVG, and TTL output. The utilities subpath provides `inspectEntity`,
`formatAttributeValue`, `hashColor`, and `meshColor`.

```ts
import { exportToBuffer } from '@ifcopenshell-js/web/serializers';

const result = await exportToBuffer(shell, file, settings, 'obj');
if (result) {
  console.log(result.primary);   // OBJ data
  console.log(result.secondary); // MTL data
}
```

## Package exports

| Import | Contents |
| --- | --- |
| `@ifcopenshell-js/web` | Runtime, files, entities, values, generated APIs, and core types |
| `@ifcopenshell-js/web/geom` | Geometry settings, mesh iteration, spatial trees, and geometry types |
| `@ifcopenshell-js/web/serializers` | Geometry serializers and serializer settings |
| `@ifcopenshell-js/web/util` | Entity inspection and display-color helpers |

## API documentation

From this package directory, generate the static API reference with:

```bash
npm run docs
```

The output is written to `docs/api/`. Run `npm run docs:check` to validate the
documentation without writing HTML. The reference is generated from public
TypeScript exports and JSDoc; documentation for generated `api.*` modules comes
from native binding comments.

## Repository development

When working from the IfcOpenShell repository, stage the WASM package before
running the package tests or demo if the artifacts are not already available:

```bash
cd packages/ifcopenshell-wasm && npm run stage
cd ../ifcopenshell-js
npm install
npm test
```

The React and Three.js demo is in the `demo/` directory.

## Bundler note

Vite handles the packaged WASM assets without extra configuration. Rollup and
esbuild need an asset-copy plugin or equivalent static-asset setup so the WASM
files are available at runtime.
