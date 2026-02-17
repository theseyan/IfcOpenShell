import { getModule } from "../api.js";

export function readString(ptr: number): string | null {
  if (!ptr) return null;
  const M = getModule();
  return M.UTF8ToString(ptr);
}

export function allocString(str: string): number {
  const M = getModule();
  const len = M.lengthBytesUTF8(str) + 1;
  const ptr = M._malloc(len);
  if (!ptr) {
    throw new Error(`Out of WASM memory while allocating ${len} bytes for string`);
  }
  M.stringToUTF8(str, ptr, len);
  return ptr;
}

export function readF64Array(
  ptr: number,
  count: number,
): Float64Array {
  const M = getModule();
  return new Float64Array(M.HEAPF64.buffer, ptr, count);
}

export function readI32Array(
  ptr: number,
  count: number,
): Int32Array {
  const M = getModule();
  return new Int32Array(M.HEAP32.buffer, ptr, count);
}

export function writeBytes(
  data: Uint8Array,
): number {
  if (data.length === 0) return 0;
  const M = getModule();
  const ptr = M._malloc(data.length);
  if (!ptr) {
    throw new Error(`Out of WASM memory while allocating ${data.length} bytes`);
  }
  M.HEAPU8.set(data, ptr);
  return ptr;
}
