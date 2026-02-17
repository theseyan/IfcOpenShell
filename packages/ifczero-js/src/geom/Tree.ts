import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import type { Ptr } from "./types.js";

export interface ClashResult {
  clashType: number;
  aId: number;
  bId: number;
  distance: number;
  p1: [number, number, number];
  p2: [number, number, number];
}

export class Tree {
  readonly ptr: Ptr;
  readonly M: EmscriptenModule;

  constructor() {
    this.M = getModule();
    this.ptr = bind.tree_create(this.M);
    if (!this.ptr) throw new Error("Failed to create Tree");
  }

  addFile(filePtr: Ptr, settingsPtr: Ptr): boolean {
    return bind.tree_add_file(this.M, this.ptr, filePtr, settingsPtr) !== 0;
  }

  selectById(filePtr: Ptr, id: number, completelyWithin = false, extend = 0): number[] {
    const listPtr = bind.tree_select_by_id(
      this.M,
      this.ptr,
      filePtr,
      id,
      completelyWithin ? 1 : 0,
      extend,
    );
    return this._readIdList(listPtr);
  }

  selectBox(min: [number, number, number], max: [number, number, number], completelyWithin = false): number[] {
    const sp = this.M.stackSave();
    const minPtr = this.M.stackAlloc(3 * 8);
    const maxPtr = this.M.stackAlloc(3 * 8);
    for (let i = 0; i < 3; i++) {
      this.M.setValue(minPtr + i * 8, min[i], "double");
      this.M.setValue(maxPtr + i * 8, max[i], "double");
    }
    const listPtr = bind.tree_select_box(this.M, this.ptr, minPtr, maxPtr, completelyWithin ? 1 : 0);
    this.M.stackRestore(sp);
    return this._readIdList(listPtr);
  }

  selectPoint(point: [number, number, number], extend = 0): number[] {
    const sp = this.M.stackSave();
    const ptr = this.M.stackAlloc(3 * 8);
    for (let i = 0; i < 3; i++) {
      this.M.setValue(ptr + i * 8, point[i], "double");
    }
    const listPtr = bind.tree_select_point(this.M, this.ptr, ptr, extend);
    this.M.stackRestore(sp);
    return this._readIdList(listPtr);
  }

  clashIntersection(
    filePtr: Ptr,
    setAIds: number[],
    setBIds: number[],
    tolerance = 0,
    checkAll = false,
  ): ClashResult[] {
    const sp = this.M.stackSave();
    const bufA = this.M.stackAlloc(setAIds.length * 4);
    const bufB = this.M.stackAlloc(setBIds.length * 4);
    for (let i = 0; i < setAIds.length; i++) this.M.setValue(bufA + i * 4, setAIds[i], "i32");
    for (let i = 0; i < setBIds.length; i++) this.M.setValue(bufB + i * 4, setBIds[i], "i32");
    const listPtr = bind.tree_clash_intersection_many(
      this.M, this.ptr, filePtr,
      bufA, setAIds.length,
      bufB, setBIds.length,
      tolerance, checkAll ? 1 : 0,
    );
    this.M.stackRestore(sp);
    return this._readClashList(listPtr);
  }

  clashCollision(
    filePtr: Ptr,
    setAIds: number[],
    setBIds: number[],
    allowTouching = false,
  ): ClashResult[] {
    const sp = this.M.stackSave();
    const bufA = this.M.stackAlloc(setAIds.length * 4);
    const bufB = this.M.stackAlloc(setBIds.length * 4);
    for (let i = 0; i < setAIds.length; i++) this.M.setValue(bufA + i * 4, setAIds[i], "i32");
    for (let i = 0; i < setBIds.length; i++) this.M.setValue(bufB + i * 4, setBIds[i], "i32");
    const listPtr = bind.tree_clash_collision_many(
      this.M, this.ptr, filePtr,
      bufA, setAIds.length,
      bufB, setBIds.length,
      allowTouching ? 1 : 0,
    );
    this.M.stackRestore(sp);
    return this._readClashList(listPtr);
  }

  clashClearance(
    filePtr: Ptr,
    setAIds: number[],
    setBIds: number[],
    clearance: number,
    checkAll = false,
  ): ClashResult[] {
    const sp = this.M.stackSave();
    const bufA = this.M.stackAlloc(setAIds.length * 4);
    const bufB = this.M.stackAlloc(setBIds.length * 4);
    for (let i = 0; i < setAIds.length; i++) this.M.setValue(bufA + i * 4, setAIds[i], "i32");
    for (let i = 0; i < setBIds.length; i++) this.M.setValue(bufB + i * 4, setBIds[i], "i32");
    const listPtr = bind.tree_clash_clearance_many(
      this.M, this.ptr, filePtr,
      bufA, setAIds.length,
      bufB, setBIds.length,
      clearance, checkAll ? 1 : 0,
    );
    this.M.stackRestore(sp);
    return this._readClashList(listPtr);
  }

  get lastError(): string {
    return bind.tree_last_error(this.M, this.ptr);
  }

  destroy(): void {
    bind.tree_destroy(this.M, this.ptr);
  }

  private _readIdList(listPtr: Ptr): number[] {
    if (!listPtr) return [];
    const count = bind.id_list_count(this.M, listPtr);
    const result: number[] = [];
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    for (let i = 0; i < count; i++) {
      const ok = bind.id_list_get(this.M, listPtr, i, outPtr);
      if (ok) result.push(this.M.getValue(outPtr, "i32"));
    }
    this.M.stackRestore(sp);
    bind.id_list_destroy(this.M, listPtr);
    return result;
  }

  private _readClashList(listPtr: Ptr): ClashResult[] {
    if (!listPtr) return [];
    const count = bind.clash_list_count(this.M, listPtr);
    const result: ClashResult[] = [];
    for (let i = 0; i < count; i++) {
      const cPtr = bind.clash_list_get(this.M, listPtr, i);
      if (cPtr) {
        result.push({
          clashType: this.M.getValue(cPtr, "i32"),
          aId: this.M.getValue(cPtr + 4, "i32"),
          bId: this.M.getValue(cPtr + 8, "i32"),
          distance: this.M.getValue(cPtr + 16, "double"),
          p1: [
            this.M.getValue(cPtr + 24, "double"),
            this.M.getValue(cPtr + 32, "double"),
            this.M.getValue(cPtr + 40, "double"),
          ],
          p2: [
            this.M.getValue(cPtr + 48, "double"),
            this.M.getValue(cPtr + 56, "double"),
            this.M.getValue(cPtr + 64, "double"),
          ],
        });
      }
    }
    bind.clash_list_destroy(this.M, listPtr);
    return result;
  }
}
