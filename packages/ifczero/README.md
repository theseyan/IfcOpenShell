# ifczero

A complete IFC authoring, geometry and serialization library for the web, powered by [IfcOpenShell](https://github.com/ifcopenshell/ifcopenshell).

## Initialize

- `init(overrides?: ModuleInitOverrides): Promise<EmscriptenModule>`
  Loads the full bundled `ifcopenshell.js` and caches the module.
- `initParse(overrides?: ModuleInitOverrides): Promise<EmscriptenModule>`
  Loads bundled `ifcparse.js` and caches the module.

Runtime helpers:

- `getModule(): EmscriptenModule`
- `hasModule(): boolean`
- `clearModule(): void`
- `moduleInstance: EmscriptenModule | null`

Example using bundled full module:

```ts
import { init } from "ifczero";

await init({
  locateFile: (file) => new URL(`../wasm/${file}`, import.meta.url).pathname,
});
```

You can also import `ifczero/wasm/*.js` directly and use `ccall`/`cwrap` manually, but the TS APIs are the recommended layer.

## Module Guides

- [Parse.md](./docs/Parse.md)
- [Geom.md](./docs/Geom.md)
- [Serializers.md](./docs/Serializers.md)
- [Util.md](./docs/Util.md)

## Low-level Helpers

- `loadModule(createModule, overrides?): Promise<EmscriptenModule>`
- `readString(ptr): string | null`
- `readF64Array(ptr, count): Float64Array`
- `readI32Array(ptr, count): Int32Array`
- `writeBytes(data): number`

## Raw Bindings

- Root: `parseRaw`, `psetqtoRaw`, `ifcutilRaw`, `geomRaw`, `serializersRaw`
- Namespaced: `parse.raw`, `parse.rawPsetQto`, `parse.rawUtil`, `geom.raw`, `serializers.raw`, `util.raw`

These are low-level direct C ABI wrappers.
