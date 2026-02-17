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

function asFactory(imported: unknown): CreateModuleFn | null {
  const factory = (imported as { default?: unknown }).default ?? imported;
  return typeof factory === "function" ? (factory as CreateModuleFn) : null;
}

async function tryImportFactory(specifier: string): Promise<CreateModuleFn | null> {
  try {
    const imported = await import(specifier);
    return asFactory(imported);
  } catch {
    return null;
  }
}

async function resolveBundledModuleFactory(moduleJs: "ifcopenshell.js" | "ifcparse.js"): Promise<CreateModuleFn> {
  if (moduleJs === "ifcopenshell.js") {
    return (
      (await tryImportFactory("ifczero/wasm/ifcopenshell.js")) ??
      (await tryImportFactory(new URL("../wasm/ifcopenshell.js", import.meta.url).href)) ??
      (await tryImportFactory(new URL("./wasm/ifcopenshell.js", import.meta.url).href)) ??
      (() => {
        throw new Error("Failed to load module factory: ifcopenshell.js");
      })()
    );
  }

  return (
    (await tryImportFactory("ifczero/wasm/ifcparse.js")) ??
    (await tryImportFactory(new URL("../wasm/ifcparse.js", import.meta.url).href)) ??
    (await tryImportFactory(new URL("./wasm/ifcparse.js", import.meta.url).href)) ??
    (() => {
      throw new Error("Failed to load module factory: ifcparse.js");
    })()
  );
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
