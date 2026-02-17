import { loadModule } from "./wasm/loader.js";
import type { CreateModuleFn, EmscriptenModule, ModuleInitOverrides } from "./wasm/types.js";

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

async function resolveBundledModuleFactory(moduleJs: "ifcopenshell.js" | "ifcparse.js"): Promise<CreateModuleFn> {
  const url = new URL(`../wasm/${moduleJs}`, import.meta.url);
  const imported = await import(/* @vite-ignore */ url.href);
  const factory = (imported as { default?: unknown }).default ?? imported;
  if (typeof factory !== "function") {
    throw new Error(`Failed to load module factory from ${moduleJs}`);
  }
  return factory as CreateModuleFn;
}

export async function initWith(
  createModule: CreateModuleFn,
  overrides?: ModuleInitOverrides,
): Promise<EmscriptenModule> {
  const M = await loadModule(createModule, overrides);
  return setModule(M);
}

export async function init(overrides?: ModuleInitOverrides): Promise<EmscriptenModule> {
  const createModule = await resolveBundledModuleFactory("ifcopenshell.js");
  return initWith(createModule, overrides);
}

export async function initParse(overrides?: ModuleInitOverrides): Promise<EmscriptenModule> {
  const createModule = await resolveBundledModuleFactory("ifcparse.js");
  return initWith(createModule, overrides);
}
