import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import { Mesh } from "./Mesh.js";
import { Serialized } from "./Serialized.js";
import type { ElementKind } from "./types.js";
import type { Ptr } from "./types.js";

export interface IteratorOptions {
  geometryLibrary?: string;
  numThreads?: number;
}

function withCStringArray<T>(
  M: EmscriptenModule,
  values: string[],
  fn: (bufferPtr: number, count: number) => T,
): T {
  const ptrs: number[] = [];
  for (const value of values) {
    const len = M.lengthBytesUTF8(value) + 1;
    const ptr = M._malloc(len);
    M.stringToUTF8(value, ptr, len);
    ptrs.push(ptr);
  }

  const sp = M.stackSave();
  const buf = M.stackAlloc(ptrs.length * 4);
  for (let i = 0; i < ptrs.length; i++) {
    M.setValue(buf + i * 4, ptrs[i], "i32");
  }

  try {
    return fn(buf, ptrs.length);
  } finally {
    M.stackRestore(sp);
    for (const ptr of ptrs) M._free(ptr);
  }
}

export class Iterator {
  readonly ptr: Ptr;

  private constructor(
    readonly M: EmscriptenModule,
    ptr: Ptr,
  ) {
    this.ptr = ptr;
  }

  static create(
    filePtr: Ptr,
    settingsPtr: Ptr,
    opts: IteratorOptions = {},
  ): Iterator {
    const M = getModule();
    const ptr = bind.iterator_create(
      M,
      filePtr,
      settingsPtr,
      opts.geometryLibrary ?? "opencascade",
      opts.numThreads ?? 1,
    );
    if (!ptr) throw new Error("Failed to create geometry iterator");
    return new Iterator(M, ptr);
  }

  static createWithTypeFilter(
    filePtr: Ptr,
    settingsPtr: Ptr,
    types: string[],
    include = true,
    opts: IteratorOptions = {},
  ): Iterator {
    const M = getModule();

    const ptr = withCStringArray(M, types, (bufferPtr, count) =>
      bind.iterator_create_with_type_filter(
        M,
        filePtr,
        settingsPtr,
        opts.geometryLibrary ?? "opencascade",
        bufferPtr,
        count,
        include ? 1 : 0,
        opts.numThreads ?? 1,
      ));
    if (!ptr) throw new Error("Failed to create filtered iterator");
    return new Iterator(M, ptr);
  }

  static createWithGuidFilter(
    filePtr: Ptr,
    settingsPtr: Ptr,
    guids: string[],
    include = true,
    opts: IteratorOptions = {},
  ): Iterator {
    const M = getModule();

    const ptr = withCStringArray(M, guids, (bufferPtr, count) =>
      bind.iterator_create_with_globalid_filter(
        M,
        filePtr,
        settingsPtr,
        opts.geometryLibrary ?? "opencascade",
        bufferPtr,
        count,
        include ? 1 : 0,
        opts.numThreads ?? 1,
      ));
    if (!ptr) throw new Error("Failed to create GUID-filtered iterator");
    return new Iterator(M, ptr);
  }

  static createWithIdFilter(
    filePtr: Ptr,
    settingsPtr: Ptr,
    ids: number[],
    include = true,
    opts: IteratorOptions = {},
  ): Iterator {
    const M = getModule();

    const sp = M.stackSave();
    const buf = M.stackAlloc(ids.length * 4);
    for (let i = 0; i < ids.length; i++) {
      M.setValue(buf + i * 4, ids[i], "i32");
    }
    const ptr = bind.iterator_create_with_id_filter(
      M,
      filePtr,
      settingsPtr,
      opts.geometryLibrary ?? "opencascade",
      buf,
      ids.length,
      include ? 1 : 0,
      opts.numThreads ?? 1,
    );
    M.stackRestore(sp);
    if (!ptr) throw new Error("Failed to create ID-filtered iterator");
    return new Iterator(M, ptr);
  }

  initialize(): boolean {
    return bind.iterator_initialize(this.M, this.ptr) !== 0;
  }

  next(): boolean {
    return bind.iterator_next(this.M, this.ptr) !== 0;
  }

  get progress(): number {
    return bind.iterator_progress(this.M, this.ptr);
  }

  get hadError(): boolean {
    return bind.iterator_had_error(this.M, this.ptr) !== 0;
  }

  get unitName(): string {
    return bind.iterator_unit_name(this.M, this.ptr);
  }

  get unitMagnitude(): number {
    return bind.iterator_unit_magnitude(this.M, this.ptr);
  }

  get currentType(): string {
    return bind.iterator_current_type(this.M, this.ptr);
  }

  get currentGuid(): string {
    return bind.iterator_current_guid(this.M, this.ptr);
  }

  get currentKind(): ElementKind {
    return bind.iterator_current_kind(this.M, this.ptr) as ElementKind;
  }

  get currentId(): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.iterator_current_id(this.M, this.ptr, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") : null;
    this.M.stackRestore(sp);
    return val;
  }

  get currentParentId(): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.iterator_current_parent_id(this.M, this.ptr, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") : null;
    this.M.stackRestore(sp);
    return val;
  }

  get currentTransform(): Float64Array | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(16 * 8);
    const ok = bind.iterator_current_transform(this.M, this.ptr, outPtr);
    if (!ok) {
      this.M.stackRestore(sp);
      return null;
    }
    const result = new Float64Array(16);
    for (let i = 0; i < 16; i++) {
      result[i] = this.M.getValue(outPtr + i * 8, "double");
    }
    this.M.stackRestore(sp);
    return result;
  }

  computeBounds(withGeometry = true): void {
    bind.iterator_compute_bounds(this.M, this.ptr, withGeometry ? 1 : 0);
  }

  boundsMin(): [number, number, number] | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(3 * 8);
    const ok = bind.iterator_bounds_min(this.M, this.ptr, outPtr);
    if (!ok) {
      this.M.stackRestore(sp);
      return null;
    }
    const result: [number, number, number] = [
      this.M.getValue(outPtr, "double"),
      this.M.getValue(outPtr + 8, "double"),
      this.M.getValue(outPtr + 16, "double"),
    ];
    this.M.stackRestore(sp);
    return result;
  }

  boundsMax(): [number, number, number] | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(3 * 8);
    const ok = bind.iterator_bounds_max(this.M, this.ptr, outPtr);
    if (!ok) {
      this.M.stackRestore(sp);
      return null;
    }
    const result: [number, number, number] = [
      this.M.getValue(outPtr, "double"),
      this.M.getValue(outPtr + 8, "double"),
      this.M.getValue(outPtr + 16, "double"),
    ];
    this.M.stackRestore(sp);
    return result;
  }

  getMesh(): Mesh {
    const meshPtr = bind.iterator_get_mesh(this.M, this.ptr);
    if (!meshPtr) throw new Error("Failed to get mesh from iterator");
    return new Mesh(this.M, meshPtr);
  }

  getSerialized(): Serialized {
    const serializedPtr = bind.iterator_get_serialized(this.M, this.ptr);
    if (!serializedPtr) throw new Error("Failed to get serialized shape from iterator");
    return new Serialized(this.M, serializedPtr);
  }

  get log(): string {
    return bind.iterator_log(this.M, this.ptr);
  }

  get lastError(): string {
    return bind.iterator_last_error(this.M, this.ptr);
  }

  *[Symbol.iterator](): IterableIterator<Mesh> {
    if (!this.initialize()) return;
    do {
      yield this.getMesh();
    } while (this.next());
  }

  destroy(): void {
    bind.iterator_destroy(this.M, this.ptr);
  }
}
