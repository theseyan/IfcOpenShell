import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import type { Ptr } from "./types.js";

export class GeomSettings {
  readonly ptr: Ptr;
  readonly M: EmscriptenModule;

  constructor() {
    this.M = getModule();
    this.ptr = bind.settings_create(this.M);
    if (!this.ptr) throw new Error("Failed to create GeomSettings");
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

  setIntSet(name: string, values: number[]): boolean {
    const sp = this.M.stackSave();
    const valuesPtr = this.M.stackAlloc(values.length * 4);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(valuesPtr + i * 4, values[i], "i32");
    }
    const ok = bind.settings_set_int_set(this.M, this.ptr, name, valuesPtr, values.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setStringSet(name: string, values: string[]): boolean {
    const stringPtrs: number[] = [];
    try {
      for (const value of values) {
        const len = this.M.lengthBytesUTF8(value) + 1;
        const ptr = this.M._malloc(len);
        if (!ptr) {
          throw new Error(`Out of WASM memory while allocating ${len} bytes for string setting`);
        }
        this.M.stringToUTF8(value, ptr, len);
        stringPtrs.push(ptr);
      }

      const sp = this.M.stackSave();
      try {
        const valuesPtr = this.M.stackAlloc(stringPtrs.length * 4);
        for (let i = 0; i < stringPtrs.length; i++) {
          this.M.setValue(valuesPtr + i * 4, stringPtrs[i], "i32");
        }
        return bind.settings_set_string_set(this.M, this.ptr, name, valuesPtr, stringPtrs.length) !== 0;
      } finally {
        this.M.stackRestore(sp);
      }
    } finally {
      for (const ptr of stringPtrs) this.M._free(ptr);
    }
  }

  setDoubleVector(name: string, values: number[]): boolean {
    const sp = this.M.stackSave();
    const valuesPtr = this.M.stackAlloc(values.length * 8);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(valuesPtr + i * 8, values[i], "double");
    }
    const ok =
      bind.settings_set_double_vector(this.M, this.ptr, name, valuesPtr, values.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
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
