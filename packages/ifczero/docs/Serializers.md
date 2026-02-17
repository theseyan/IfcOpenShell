# Serializers API

Import styles:

```ts
import * as ifc from "ifczero";
// or
import * as serializers from "ifczero/serializers";
```

## Types

- `SerializerFormat` - Output format union: `"obj" | "svg" | "ttl" | "step" | "iges" | "gltf" | "xml" | "json"`.
- `BufferSerializerFormat` - In-memory formats: `"obj" | "svg" | "ttl"`.
- `FileSerializerFormat` - Stream/file serializer formats: `"obj" | "svg" | "ttl"`.
- `ExportOptions` - Common run options `{ geometryLibrary?: string; numThreads?: number }`.

## Capability and Errors

- `hasFormat(format: SerializerFormat): boolean` - Checks format support in current wasm build.
- `lastError(): string` - Returns latest module-level serializer error.

## SerializerSettings

- `new SerializerSettings()` - Creates serializer settings object.
- `ptr: number` - Native settings pointer.
- `setBool(name: string, value: boolean): boolean` - Sets boolean setting.
- `setInt(name: string, value: number): boolean` - Sets integer setting.
- `setDouble(name: string, value: number): boolean` - Sets float setting.
- `setString(name: string, value: string): boolean` - Sets string setting.
- `getBool(name: string): boolean | null` - Reads boolean setting.
- `getInt(name: string): number | null` - Reads integer setting.
- `getDouble(name: string): number | null` - Reads float setting.
- `getString(name: string): string | null` - Reads string setting.
- `getType(name: string): string | null` - Reads setting type name.
- `settingNames(): string[]` - Lists available setting keys.
- `lastError: string` - Last settings-specific error.
- `destroy(): void` - Releases settings handle.

## One-shot Export Helpers

- `exportToFile(filePtr: number, geomSettingsPtr: number, serializerSettingsPtr: number, format: SerializerFormat, filename: string, opts?: ExportOptions): boolean` - Exports IFC content to file path.
- `exportToBuffer(filePtr: number, geomSettingsPtr: number, serializerSettingsPtr: number, format: BufferSerializerFormat, opts?: ExportOptions): { primary: string; secondary: string } | null` - Exports IFC content to in-memory buffers.

## FileSerializer

- `FileSerializer.create(geomSettingsPtr: number, serializerSettingsPtr: number, format: FileSerializerFormat, filename: string, options?: { mtlFilename?: string }): FileSerializer` - Creates a file-backed serializer instance.
- `ptr: number` - Native serializer pointer.
- `ready: boolean` - Serializer readiness state.
- `run(filePtr: number, geometryLibrary?: string, numThreads?: number): boolean` - Runs serializer.
- `lastError: string` - Last serializer instance error.
- `destroy(): void` - Releases serializer instance.

## BufferSerializer

- `BufferSerializer.create(geomSettingsPtr: number, serializerSettingsPtr: number, format: BufferSerializerFormat): BufferSerializer` - Creates in-memory serializer instance.
- `ptr: number` - Native serializer pointer.
- `ready: boolean` - Serializer readiness state.
- `run(filePtr: number, geometryLibrary?: string, numThreads?: number): boolean` - Runs serializer.
- `primaryBuffer: string` - Primary output buffer text.
- `secondaryBuffer: string` - Secondary output buffer text.
- `lastError: string` - Last serializer instance error.
- `destroy(): void` - Releases serializer instance.

## Raw Serializer Bindings

- serializers module export: `raw`.
- root module export: `serializersRaw`.

These are low-level direct bindings; use ergonomic APIs for most workflows.
