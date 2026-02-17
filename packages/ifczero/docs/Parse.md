# Parse API

Import styles:

```ts
import * as ifc from "ifczero";
// or
import * as parse from "ifczero/parse";
```

## Parse Functions

- `version(): string` - Returns the IfcOpenShell version string.
- `createTimestamp(): string` - Returns an IFC-formatted timestamp string.
- `guessFileType(path: string): FileType` - Guesses IFC storage format from a path.
- `fileStatusMessage(status: FileStatus): string` - Converts `FileStatus` to a readable message.
- `lastError(): string` - Returns the last parser/global error string.

## IfcFile

Creation:

- `IfcFile.createEmpty(schema: string): IfcFile` - Creates an empty IFC model for a schema name.
- `IfcFile.open(path: string, fileType?: FileType, readonly?: boolean): IfcFile` - Opens an IFC file by path.
- `IfcFile.openFromMemory(data: Uint8Array | string): IfcFile` - Opens an IFC file from memory bytes/text.

Properties:

- `ptr: Ptr` - Native file handle pointer.
- `status: FileStatus` - Current parse status.
- `schema: string` - Active schema name.
- `schemaIdentifier: string` - Alias of `schema`.
- `entityCount: number` - Number of entities in the model.
- `maxId: number` - Current highest numeric IFC id.
- `lastError: string | null` - Last file-specific error, if any.

Header/entity lookup:

- `headerDescription(): Entity | null` - Returns `FILE_DESCRIPTION` header entity.
- `headerFileName(): Entity | null` - Returns `FILE_NAME` header entity.
- `headerSchema(): Entity | null` - Returns `FILE_SCHEMA` header entity.
- `entities(): Entity[]` - Returns all entities in the model.
- `entityIds(): number[]` - Returns all entity ids.
- `byId(id: number): Entity | null` - Looks up entity by numeric id.
- `byGuid(guid: string): Entity | null` - Looks up entity by GlobalId.
- `byType(typeName: string, includeSubtypes?: boolean): Entity[]` - Finds entities by IFC type.
- `byReference(id: number): Entity[]` - Finds entities referencing a given id.

Graph/inverse traversal:

- `getInverse(instanceId: number, typeName: string, attributeIndex: number): Entity[]` - Returns inverse matches for a filter.
- `getTotalInverses(instanceId: number): number` - Counts all inverse references.
- `inverseIndices(instanceId: number): number[]` - Returns inverse attribute indices.
- `traverse(entity: Entity, maxLevel?: number, breadthFirst?: boolean): Entity[]` - Traverses referenced graph from entity.
- `traverseById(instanceId: number, maxLevel?: number, breadthFirst?: boolean): Entity[]` - Traverses graph from numeric id.

Mutation:

- `createEntity(typeName: string): Entity` - Creates a new entity with generated id.
- `createEntityWithId(typeName: string, id: number): Entity` - Creates entity with explicit id.
- `addEntity(entity: Entity, id?: number): number` - Adds entity to file and returns assigned id.
- `addEntities(entities: Entity[]): boolean` - Adds many entities in one call.
- `removeEntity(entity: Entity): boolean` - Removes entity by handle.
- `removeById(id: number): boolean` - Removes entity by numeric id.
- `batchBegin(): void` - Starts batch mutation mode.
- `batchEnd(): void` - Ends batch mutation mode.
- `recalculateIdCounter(): void` - Recomputes internal id counter.

Serialization/units/lifecycle:

- `toSpf(): string` - Serializes the file to SPF text.
- `writeSpf(path: string): boolean` - Writes SPF to filesystem path.
- `unitScale(unitType: string): number` - Returns project scale factor for a unit kind.
- `unitEntity(unitType: string): Entity | null` - Returns unit definition entity for unit kind.
- `types(): TypeDeclaration[]` - Returns schema declarations used in file context.
- `close(): void` - Releases native file resources.

## Entity

Identity/type:

- `id: number` - Numeric IFC id.
- `typeName: string` - IFC type name.
- `typeNameWithSchema: string` - Qualified schema + type name.
- `isA(typeName: string): boolean` - Checks type or supertype membership.
- `toString(validSpf?: number): string` - Returns STEP-like textual representation.

Attribute metadata:

- `attributeCount: number` - Number of forward attributes.
- `attributeIndex(name: string): number` - Gets attribute index by name.
- `attributeName(index: number): string` - Gets attribute name by index.
- `attributeNames(): string[]` - Lists forward attribute names.
- `inverseAttributeNames(): string[]` - Lists inverse attribute names.
- `attributeCategory(name: string): number` - Returns category code for attribute name.
- `argumentType(index: number): ArgumentType` - Returns runtime value type code.
- `argumentDeclaredType(index: number): ArgumentType` - Returns declared type code.
- `isNull(index: number): boolean` - Checks whether argument is null.

Getters:

- `getInt(index: number): number | null` - Reads integer argument.
- `getBool(index: number): boolean | null` - Reads boolean argument.
- `getLogical(index: number): number | null` - Reads EXPRESS logical argument.
- `getDouble(index: number): number | null` - Reads floating-point argument.
- `getString(index: number): string | null` - Reads string-like argument.
- `getEntity(index: number): Entity | null` - Reads referenced entity.
- `getIntList(index: number): number[]` - Reads list of integers.
- `getDoubleList(index: number): number[]` - Reads list of doubles.
- `getStringList(index: number): string[]` - Reads list of strings.
- `getEntityList(index: number): Entity[]` - Reads list of entities.
- `getIntMatrix(index: number): number[][]` - Reads nested int aggregate.
- `getDoubleMatrix(index: number): number[][]` - Reads nested double aggregate.
- `getEntityMatrix(index: number): Entity[][]` - Reads nested entity aggregate.
- `getInverse(inverseName: string): Entity[]` - Reads inverse relation values.

Setters:

- `setInt(index: number, value: number): boolean` - Writes integer argument.
- `setBool(index: number, value: boolean): boolean` - Writes boolean argument.
- `setLogical(index: number, value: number): boolean` - Writes logical argument.
- `setDouble(index: number, value: number): boolean` - Writes double argument.
- `setString(index: number, value: string): boolean` - Writes string argument.
- `setEntity(index: number, entity: Entity): boolean` - Writes entity reference.
- `setNull(index: number): boolean` - Sets argument to null.
- `unsetArgument(index: number): boolean` - Unsets argument value.
- `setIntList(index: number, values: number[]): boolean` - Writes integer list.
- `setDoubleList(index: number, values: number[]): boolean` - Writes double list.
- `setStringList(index: number, values: string[]): boolean` - Writes string list.
- `setEntityList(index: number, entities: Entity[]): boolean` - Writes entity list.
- `setIntMatrix(index: number, rows: number[][]): boolean` - Writes nested int aggregate.
- `setDoubleMatrix(index: number, rows: number[][]): boolean` - Writes nested double aggregate.
- `setEntityMatrix(index: number, rows: Entity[][]): boolean` - Writes nested entity aggregate.

Helpers:

- `getInfo(options?: { recursive?: boolean; includeId?: boolean }): Record<string, unknown>` - Builds object snapshot of entity attributes.
- `withAttributes(): Entity & Record<string, unknown>` - Returns proxy with dynamic attribute accessors.
- `Entity.walk<T>(predicate, transform, value): unknown` - Recursively transforms matching values.

## Schema Introspection

`Schema`:

- `new Schema(name: string)` - Creates schema reflection handle.
- `declarationByName(typeName: string): TypeDeclaration | null` - Finds declaration by name.
- `entities(): TypeDeclaration[]` - Lists schema entity declarations.
- `Schema.availableSchemas(): string[]` - Lists schemas compiled in wasm.

`TypeDeclaration`:

- `name: string` - Declaration name.
- `schemaName: string` - Owning schema name.
- `index: number` - Internal declaration index.
- `kind: DeclarationKind` - Declaration kind enum.
- `isAbstract: boolean` - Whether declaration is abstract.
- `isA(typeName: string): boolean` - Checks declaration inheritance.
- `supertype: TypeDeclaration | null` - Super declaration.
- `subtypes: TypeDeclaration[]` - Direct sub declarations.
- `declaredType: TypeDeclaration | null` - Underlying declared type.
- `attributes(includeInherited?: boolean): AttributeDeclaration[]` - Attribute declarations.
- `inverseAttributes(includeInherited?: boolean): InverseAttributeDeclaration[]` - Inverse declarations.
- `attributeIndex(name: string): number` - Attribute index by name.
- `enumerationItems(): string[]` - Enumeration literals.
- `selectList(): TypeDeclaration[]` - SELECT alternatives.

`AttributeDeclaration`:

- `name: string` - Attribute name.
- `optional: boolean` - Optionality flag.
- `parameterType: ParameterTypeDeclaration | null` - Parameter type metadata.

`InverseAttributeDeclaration`:

- `name: string` - Inverse attribute name.
- `aggregationType: InverseAggregationType` - Inverse cardinality kind.
- `bound1: number` - Lower bound.
- `bound2: number` - Upper bound.
- `entityReference: TypeDeclaration | null` - Referenced entity type.
- `attributeReference: AttributeDeclaration | null` - Referenced forward attribute.

`ParameterTypeDeclaration`:

- `kind: ParameterTypeKind` - Parameter type category.
- `namedDeclaredType: TypeDeclaration | null` - Named declaration (for named kinds).
- `simpleDeclaredType: SimpleType` - Simple type enum.
- `aggregationType: AggregationType` - Aggregate kind.
- `aggregationBound1: number` - Aggregate lower bound.
- `aggregationBound2: number` - Aggregate upper bound.
- `aggregationElementType: ParameterTypeDeclaration | null` - Nested element parameter type.

## PsetQto

- `new PsetQto()` - Creates Pset/Qto template helper. By default it attempts to auto-load bundled schema templates (`IFC2X3`, `IFC4`, `IFC4X3`) in the background; failures are ignored.
- `bundledTemplateUrl(schema: string): URL | null` - Returns bundled template URL for supported schema, or `null`.
- `loadTemplateFromMemory(schema: string, data: Uint8Array | ArrayBuffer): boolean` - Loads one schema template from IFC bytes.
- `loadTemplateFromFile(schema: string, path: string): boolean` - Loads one schema template from an in-WASM filesystem path.
- `loadTemplateFromUrl(schema: string, url: string | URL, init?: RequestInit): Promise<boolean>` - Fetches IFC template from URL and loads it.
- `loadBundledTemplate(schema: string, init?: RequestInit): Promise<boolean>` - Loads packaged template for the schema.
- `unloadTemplate(schema: string): boolean` - Unloads one schema template from native cache.
- `isTemplateLoaded(schema: string): boolean` - Returns whether schema template is currently loaded.
- `getApplicable(schema: string, ifcClass: string, options?): PropertySetTemplate[]` - Gets templates applicable to class/predefined type.
- `getApplicableNames(schema: string, ifcClass: string, options?): string[]` - Gets applicable template names only.
- `getByName(schema: string, name: string): PropertySetTemplate | null` - Gets one template by name.
- `isTemplated(schema: string, name: string): boolean` - Checks if name exists in templates.
- `allTemplates(schema: string): PropertySetTemplate[]` - Lists all templates for schema.
- `getProperties(templatePtr: Ptr): PropertyTemplate[]` - Lists property templates of one template.
- `deinit(): void` - Releases native psetqto resources.

Notes:

- Autoload is best-effort and non-fatal. If bundled assets are not present, APIs continue to work once templates are loaded explicitly.
- Schema query methods perform a lazy bundled-load attempt for the requested schema if not loaded yet.

Template types:

- `PropertySetTemplate` - Template metadata + `ptr`.
- `PropertyTemplate` - Property template metadata.

## GUID Helpers

- `guidCompress(uuid: string): string` - Converts UUID hex to IFC compressed GUID.
- `guidExpand(guid: string): string` - Expands IFC GUID to UUID hex.
- `guidSplit(uuid: string): string` - Formats UUID hex with dashes.
- `newGuid(): string` - Creates random IFC compressed GUID.

## Parse Types

- `Ptr` - Native pointer alias (`number`).
- `FileType` - IFC file format enum.
- `FileStatus` - Parse status enum.
- `ArgumentType` - IFC argument value type enum.
- `DeclarationKind` - Schema declaration kind enum.
- `ParameterTypeKind` - Parameter kind enum.
- `SimpleType` - Simple EXPRESS type enum.
- `AggregationType` - Aggregate type enum.
- `InverseAggregationType` - Inverse aggregate type enum.

## Raw Parse Bindings

- parse module exports: `raw`, `rawPsetQto`, `rawUtil`.
- root module exports: `parseRaw`, `psetqtoRaw`, `ifcutilRaw`.

These are low-level direct bindings; prefer the ergonomic APIs above for normal usage.
