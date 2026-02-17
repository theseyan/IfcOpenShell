import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import * as geomBind from "../geom/bindings.generated.js";

type Ptr = number;
const TTL_TRIANGULATION_SETTING = "triangulation-type";
const TTL_POLYHEDRON_WITH_HOLES = 2;

function ensureTtlSettings(M: EmscriptenModule, geomSettingsPtr: Ptr, format: SerializerFormat): void {
  if (format !== "ttl") return;
  geomBind.settings_set_int(M, geomSettingsPtr, TTL_TRIANGULATION_SETTING, TTL_POLYHEDRON_WITH_HOLES);
}

export type SerializerFormat =
  | "obj"
  | "svg"
  | "ttl"
  | "step"
  | "iges"
  | "gltf"
  | "xml"
  | "json";

export class SerializerSettings {
  readonly ptr: Ptr;
  readonly M: EmscriptenModule;

  constructor() {
    this.M = getModule();
    this.ptr = bind.settings_create(this.M);
    if (!this.ptr) throw new Error("Failed to create SerializerSettings");
  }

  setBool(name: string, value: boolean): boolean {
    return bind.settings_set_bool(this.M, this.ptr, name, value ? 1 : 0) !== 0;
  }

  setInt(name: string, value: number): boolean {
    return bind.settings_set_int(this.M, this.ptr, name, value) !== 0;
  }

  setDouble(name: string, value: number): boolean {
    return bind.settings_set_double(this.M, this.ptr, name, value) !== 0;
  }

  setString(name: string, value: string): boolean {
    return bind.settings_set_string(this.M, this.ptr, name, value) !== 0;
  }

  getBool(name: string): boolean | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.settings_get_bool(this.M, this.ptr, name, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") !== 0 : null;
    this.M.stackRestore(sp);
    return val;
  }

  getInt(name: string): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.settings_get_int(this.M, this.ptr, name, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") : null;
    this.M.stackRestore(sp);
    return val;
  }

  getDouble(name: string): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(8);
    const ok = bind.settings_get_double(this.M, this.ptr, name, outPtr);
    const val = ok ? this.M.getValue(outPtr, "double") : null;
    this.M.stackRestore(sp);
    return val;
  }

  getString(name: string): string | null {
    const result = bind.settings_get_string(this.M, this.ptr, name);
    return result || null;
  }

  getType(name: string): string | null {
    const result = bind.settings_get_type(this.M, this.ptr, name);
    return result || null;
  }

  settingNames(): string[] {
    const listPtr = bind.settings_setting_names(this.M, this.ptr);
    if (!listPtr) return [];
    const count = bind.string_list_count(this.M, listPtr);
    const result: string[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.string_list_get(this.M, listPtr, i));
    }
    bind.string_list_destroy(this.M, listPtr);
    return result;
  }

  get lastError(): string {
    return bind.settings_last_error(this.M, this.ptr);
  }

  destroy(): void {
    bind.settings_destroy(this.M, this.ptr);
  }
}

export function hasFormat(format: SerializerFormat): boolean {
  const M = getModule();
  switch (format) {
    case "gltf": return bind.has_gltf(M) !== 0;
    case "json": return bind.has_json(M) !== 0;
    case "svg":  return bind.has_svg(M) !== 0;
    case "ttl":  return bind.has_ttl(M) !== 0;
    case "step": return bind.has_step(M) !== 0;
    case "iges": return bind.has_iges(M) !== 0;
    case "obj":  return true;
    case "xml":  return true;
  }
}

export interface ExportOptions {
  geometryLibrary?: string;
  numThreads?: number;
}

function defaultMtlFilename(objFilename: string): string {
  return objFilename.toLowerCase().endsWith(".obj")
    ? `${objFilename.slice(0, -4)}.mtl`
    : `${objFilename}.mtl`;
}

export function exportToFile(
  filePtr: Ptr,
  geomSettingsPtr: Ptr,
  serializerSettingsPtr: Ptr,
  format: SerializerFormat,
  filename: string,
  opts: ExportOptions = {},
): boolean {
  const M = getModule();
  const lib = opts.geometryLibrary ?? "opencascade";
  const threads = opts.numThreads ?? 1;
  ensureTtlSettings(M, geomSettingsPtr, format);

  switch (format) {
    case "obj":
      return bind.export_obj(
        M,
        filePtr,
        geomSettingsPtr,
        serializerSettingsPtr,
        filename,
        defaultMtlFilename(filename),
        lib,
        threads,
      ) !== 0;
    case "svg":
      return bind.export_svg(M, filePtr, geomSettingsPtr, serializerSettingsPtr, filename, lib, threads) !== 0;
    case "step":
      return bind.export_step(M, filePtr, geomSettingsPtr, serializerSettingsPtr, filename, lib, threads) !== 0;
    case "iges":
      return bind.export_iges(M, filePtr, geomSettingsPtr, serializerSettingsPtr, filename, lib, threads) !== 0;
    case "ttl":
      return bind.export_ttl(M, filePtr, geomSettingsPtr, serializerSettingsPtr, filename, lib, threads) !== 0;
    case "gltf":
      return bind.export_gltf(M, filePtr, geomSettingsPtr, serializerSettingsPtr, filename, lib, threads) !== 0;
    case "xml":
      return bind.export_xml(M, filePtr, filename) !== 0;
    case "json":
      return bind.export_json(M, filePtr, filename) !== 0;
  }
}

export type BufferSerializerFormat = "obj" | "svg" | "ttl";
export type FileSerializerFormat = "obj" | "svg" | "ttl";

export class FileSerializer {
  readonly ptr: Ptr;

  private constructor(
    readonly M: EmscriptenModule,
    ptr: Ptr,
  ) {
    this.ptr = ptr;
  }

  static create(
    geomSettingsPtr: Ptr,
    serializerSettingsPtr: Ptr,
    format: FileSerializerFormat,
    filename: string,
    options: { mtlFilename?: string } = {},
  ): FileSerializer {
    const M = getModule();
    ensureTtlSettings(M, geomSettingsPtr, format);
    let ptr: Ptr;
    switch (format) {
      case "obj":
        ptr = bind.serializer_obj_create_file(
          M,
          geomSettingsPtr,
          serializerSettingsPtr,
          filename,
          options.mtlFilename ?? defaultMtlFilename(filename),
        );
        break;
      case "svg":
        ptr = bind.serializer_svg_create_file(M, geomSettingsPtr, serializerSettingsPtr, filename);
        break;
      case "ttl":
        ptr = bind.serializer_ttl_create_file(M, geomSettingsPtr, serializerSettingsPtr, filename);
        break;
    }
    if (!ptr) {
      const detail = bind.last_error(M);
      throw new Error(
        detail ? `Failed to create file serializer for ${format}: ${detail}` : `Failed to create file serializer for ${format}`,
      );
    }
    return new FileSerializer(M, ptr);
  }

  get ready(): boolean {
    return bind.serializer_ready(this.M, this.ptr) !== 0;
  }

  run(filePtr: Ptr, geometryLibrary = "opencascade", numThreads = 1): boolean {
    return bind.serializer_run(this.M, this.ptr, filePtr, geometryLibrary, numThreads) !== 0;
  }

  get lastError(): string {
    return bind.serializer_last_error(this.M, this.ptr);
  }

  destroy(): void {
    bind.serializer_destroy(this.M, this.ptr);
  }
}

export class BufferSerializer {
  readonly ptr: Ptr;

  private constructor(
    readonly M: EmscriptenModule,
    ptr: Ptr,
  ) {
    this.ptr = ptr;
  }

  static create(
    geomSettingsPtr: Ptr,
    serializerSettingsPtr: Ptr,
    format: BufferSerializerFormat,
  ): BufferSerializer {
    const M = getModule();
    ensureTtlSettings(M, geomSettingsPtr, format);
    let ptr: Ptr;
    switch (format) {
      case "obj":
        ptr = bind.serializer_obj_create_buffer(M, geomSettingsPtr, serializerSettingsPtr);
        break;
      case "svg":
        ptr = bind.serializer_svg_create_buffer(M, geomSettingsPtr, serializerSettingsPtr);
        break;
      case "ttl":
        ptr = bind.serializer_ttl_create_buffer(M, geomSettingsPtr, serializerSettingsPtr);
        break;
    }
    if (!ptr) {
      const detail = bind.last_error(M);
      throw new Error(
        detail ? `Failed to create buffer serializer for ${format}: ${detail}` : `Failed to create buffer serializer for ${format}`,
      );
    }
    return new BufferSerializer(M, ptr);
  }

  get ready(): boolean {
    return bind.serializer_ready(this.M, this.ptr) !== 0;
  }

  run(filePtr: Ptr, geometryLibrary = "opencascade", numThreads = 1): boolean {
    return bind.serializer_run(this.M, this.ptr, filePtr, geometryLibrary, numThreads) !== 0;
  }

  get primaryBuffer(): string {
    return bind.serializer_buffer_primary(this.M, this.ptr);
  }

  get secondaryBuffer(): string {
    return bind.serializer_buffer_secondary(this.M, this.ptr);
  }

  get lastError(): string {
    return bind.serializer_last_error(this.M, this.ptr);
  }

  destroy(): void {
    bind.serializer_destroy(this.M, this.ptr);
  }
}

export function exportToBuffer(
  filePtr: Ptr,
  geomSettingsPtr: Ptr,
  serializerSettingsPtr: Ptr,
  format: BufferSerializerFormat,
  opts: ExportOptions = {},
): { primary: string; secondary: string } | null {
  const ser = BufferSerializer.create(geomSettingsPtr, serializerSettingsPtr, format);
  const ok = ser.run(filePtr, opts.geometryLibrary, opts.numThreads);
  if (!ok) {
    ser.destroy();
    return null;
  }
  const result = { primary: ser.primaryBuffer, secondary: ser.secondaryBuffer };
  ser.destroy();
  return result;
}

export function lastError(): string {
  return bind.last_error(getModule());
}
