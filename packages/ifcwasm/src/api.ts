import { loadModule } from "./wasm/loader.js";
import type { CreateModuleFn, EmscriptenModule } from "./wasm/types.js";

export let moduleInstance: EmscriptenModule | null = null;

export function setModule(M: EmscriptenModule): EmscriptenModule {
  moduleInstance = M;
  return M;
}

export function clearModule(): void {
  moduleInstance = null;
}

export function hasModule(): boolean {
  return moduleInstance !== null;
}

export function getModule(): EmscriptenModule {
  if (!moduleInstance) {
    throw new Error("IfcWasm module is not initialized. Call init() or setModule() first.");
  }
  return moduleInstance;
}

export async function init(
  createModule: CreateModuleFn,
  overrides?: Partial<EmscriptenModule>,
): Promise<EmscriptenModule> {
  const M = await loadModule(createModule, overrides);
  return setModule(M);
}
