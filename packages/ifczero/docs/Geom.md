# Geom API

Import styles:

```ts
import * as ifc from "ifczero";
// or
import * as geom from "ifczero/geom";
```

## GeomSettings

- `new GeomSettings()` - Creates geometry settings object.
- `ptr: Ptr` - Native settings pointer.
- `setBool(name: string, value: boolean): boolean` - Sets boolean setting.
- `setInt(name: string, value: number): boolean` - Sets integer setting.
- `setDouble(name: string, value: number): boolean` - Sets floating setting.
- `setString(name: string, value: string): boolean` - Sets string setting.
- `setIntSet(name: string, values: number[]): boolean` - Sets integer set setting.
- `setStringSet(name: string, values: string[]): boolean` - Sets string set setting.
- `setDoubleVector(name: string, values: number[]): boolean` - Sets vector-of-double setting.
- `getBool(name: string): boolean | null` - Gets boolean setting value.
- `getInt(name: string): number | null` - Gets integer setting value.
- `getDouble(name: string): number | null` - Gets double setting value.
- `getString(name: string): string | null` - Gets string setting value.
- `getType(name: string): string | null` - Gets declared type name for a setting.
- `settingNames(): string[]` - Lists available setting keys.
- `lastError: string` - Last settings error string.
- `destroy(): void` - Releases settings handle.

## Iterator

Create:

- `Iterator.create(filePtr: Ptr, settingsPtr: Ptr, opts?: IteratorOptions): Iterator` - Creates geometry iterator.
- `Iterator.createWithTypeFilter(filePtr: Ptr, settingsPtr: Ptr, types: string[], include?: boolean, opts?: IteratorOptions): Iterator` - Creates iterator with type filter.
- `Iterator.createWithGuidFilter(filePtr: Ptr, settingsPtr: Ptr, guids: string[], include?: boolean, opts?: IteratorOptions): Iterator` - Creates iterator with GUID filter.
- `Iterator.createWithIdFilter(filePtr: Ptr, settingsPtr: Ptr, ids: number[], include?: boolean, opts?: IteratorOptions): Iterator` - Creates iterator with numeric id filter.

Lifecycle:

- `ptr: Ptr` - Native iterator pointer.
- `initialize(): boolean` - Initializes iteration state.
- `next(): boolean` - Advances iterator.
- `[Symbol.iterator](): IterableIterator<Mesh>` - Supports `for...of` mesh iteration.
- `destroy(): void` - Releases iterator handle.

State/query:

- `progress: number` - Progress percentage.
- `hadError: boolean` - Whether iterator encountered an error.
- `unitName: string` - Model unit name.
- `unitMagnitude: number` - Unit scale magnitude.
- `currentType: string` - Current element type name.
- `currentGuid: string` - Current element GlobalId.
- `currentKind: ElementKind` - Current element geometry kind.
- `currentId: number | null` - Current element id.
- `currentParentId: number | null` - Current parent id.
- `currentTransform: Float64Array | null` - Current 4x4 transform matrix.
- `computeBounds(withGeometry?: boolean): void` - Computes iterator bounds.
- `boundsMin(): [number, number, number] | null` - Returns minimum bounds.
- `boundsMax(): [number, number, number] | null` - Returns maximum bounds.
- `getMesh(): Mesh` - Returns current triangulated mesh.
- `getSerialized(): Serialized` - Returns current serialized geometry object.
- `log: string` - Iterator log output.
- `lastError: string` - Last iterator error string.

## Mesh

Metadata:

- `id: number` - IFC id.
- `parentId: number` - Parent IFC id.
- `name: string` - Element name.
- `type: string` - IFC type name.
- `guid: string` - IFC GlobalId.
- `context: string` - Representation context.
- `uniqueId: string` - Internal unique geometry id.

Geometry views (WASM-backed):

- `vertices: Float64Array` - Vertex positions.
- `faces: Int32Array` - Face indices.
- `normals: Float64Array` - Vertex normals.
- `edges: Int32Array` - Edge indices.
- `materialIds: Int32Array` - Material ids.
- `colors: Float64Array` - Color values.
- `transform: Float64Array` - 4x4 transform.
- `uvs: Float64Array` - UV data.
- `itemIds: Int32Array` - Item ids.
- `edgesItemIds: Int32Array` - Edge item ids.

Lifecycle:

- `destroy(): void` - Releases mesh handle.

## Serialized

Metadata:

- `id: number` - IFC id.
- `parentId: number` - Parent IFC id.
- `name: string` - Element name.
- `type: string` - IFC type.
- `guid: string` - IFC GlobalId.
- `context: string` - Representation context.
- `uniqueId: string` - Internal unique geometry id.
- `brepData: string` - Serialized BREP payload.

Data:

- `surfaceStyles: Float64Array` - Surface style values.
- `surfaceStyleIds: Int32Array` - Surface style ids.
- `transform: Float64Array` - 4x4 transform.
- `toData(): SerializedData` - Returns copied plain-data snapshot.
- `destroy(): void` - Releases serialized handle.

## Tree

- `new Tree()` - Creates spatial/clash tree.
- `ptr: Ptr` - Native tree pointer.
- `addFile(filePtr: Ptr, settingsPtr: Ptr): boolean` - Adds file geometry to tree.
- `selectById(filePtr: Ptr, id: number, completelyWithin?: boolean, extend?: number): number[]` - Spatial query by element id.
- `selectBox(min: [number, number, number], max: [number, number, number], completelyWithin?: boolean): number[]` - Box selection query.
- `selectPoint(point: [number, number, number], extend?: number): number[]` - Point selection query.
- `clashIntersection(filePtr: Ptr, setAIds: number[], setBIds: number[], tolerance?: number, checkAll?: boolean): ClashResult[]` - Intersection clash test.
- `clashCollision(filePtr: Ptr, setAIds: number[], setBIds: number[], allowTouching?: boolean): ClashResult[]` - Collision clash test.
- `clashClearance(filePtr: Ptr, setAIds: number[], setBIds: number[], clearance: number, checkAll?: boolean): ClashResult[]` - Clearance clash test.
- `lastError: string` - Last tree error string.
- `destroy(): void` - Releases tree handle.

## Geom Convenience Functions

- `createMeshForId(filePtr: Ptr, settingsPtr: Ptr, id: number, geometryLibrary?: string): Mesh | null` - Builds one mesh by entity id.
- `createShapeForId(filePtr: Ptr, settingsPtr: Ptr, id: number, geometryLibrary?: string): CreatedShape | null` - Builds one shape by entity id.
- `mapShapeReprForId(filePtr: Ptr, settingsPtr: Ptr, id: number): string | null` - Returns mapped representation descriptor.
- `lastError(): string` - Returns module-level geometry error string.

`CreatedShape`:

- `kind: CreatedShapeKind` - Shape payload kind.
- `mesh: Mesh | null` - Mesh payload when triangulated/brep.
- `serialized: Serialized | null` - Serialized payload when serialized.
- `transform: Float64Array | null` - Shape transform matrix.
- `destroy(): void` - Releases created shape handle.

## Geom Types

- `Ptr` - Native pointer alias.
- `IteratorOptions` - Iterator creation options.
- `ElementKind` - Iterator element kind enum.
- `CreatedShapeKind` - Created shape kind enum.
- `MeshData` - Plain mesh data shape type.
- `SerializedData` - Plain serialized data shape type.
- `ClashResult` - Clash result record.

## Raw Geom Bindings

- geom module export: `raw`.
- root module export: `geomRaw`.

These are low-level direct bindings; prefer ergonomic APIs unless you need full C ABI control.
