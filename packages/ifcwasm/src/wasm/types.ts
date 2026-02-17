export interface EmscriptenModule {
  ccall(
    ident: string,
    returnType: string | null,
    argTypes: (string | null)[],
    args: unknown[],
  ): any;

  cwrap(
    ident: string,
    returnType: string | null,
    argTypes: (string | null)[],
  ): (...args: unknown[]) => any;

  getValue(ptr: number, type: string): number;
  setValue(ptr: number, value: number, type: string): void;

  UTF8ToString(ptr: number, maxBytesToRead?: number): string;
  stringToUTF8(str: string, outPtr: number, maxBytesToWrite: number): void;
  lengthBytesUTF8(str: string): number;

  stackAlloc(size: number): number;
  stackSave(): number;
  stackRestore(ptr: number): void;

  _malloc(size: number): number;
  _free(ptr: number): void;

  HEAPU8: Uint8Array;
  HEAPF64: Float64Array;
  HEAP32: Int32Array;
  HEAPU32: Uint32Array;

  FS: {
    mkdir(path: string): void;
    writeFile(path: string, data: string | Uint8Array): void;
    readFile(path: string, opts?: { encoding?: string }): Uint8Array | string;
    unlink(path: string): void;
  };
}

export type CreateModuleFn = (
  overrides?: Partial<EmscriptenModule>,
) => Promise<EmscriptenModule>;
