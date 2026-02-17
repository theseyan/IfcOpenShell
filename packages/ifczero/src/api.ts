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

async function tryImportIfcOpenShellFactory(): Promise<CreateModuleFn | null> {
  try {
    const imported = await import("ifczero/wasm/ifcopenshell.js");
    return asFactory(imported);
  } catch {
    return null;
  }
}

async function tryImportIfcParseFactory(): Promise<CreateModuleFn | null> {
  try {
    const imported = await import("ifczero/wasm/ifcparse.js");
    return asFactory(imported);
  } catch {
    return null;
  }
}

async function tryImportFactory(specifier: string): Promise<CreateModuleFn | null> {
  try {
    const imported = await import(specifier);
    return asFactory(imported);
  } catch {
    return null;
  }
}

function moduleHref(path: string): string {
  return new URL(path, import.meta.url).href;
}

async function resolveIfcOpenShellFactory(): Promise<CreateModuleFn> {
  return (
    (await tryImportIfcOpenShellFactory()) ??
    (await tryImportFactory(moduleHref("../wasm/ifcopenshell.js"))) ??
    (await tryImportFactory(moduleHref("./wasm/ifcopenshell.js"))) ??
    (() => {
      throw new Error("Failed to load module factory: ifcopenshell.js");
    })()
  );
}

async function resolveIfcParseFactory(): Promise<CreateModuleFn> {
  return (
    (await tryImportIfcParseFactory()) ??
    (await tryImportFactory(moduleHref("../wasm/ifcparse.js"))) ??
    (await tryImportFactory(moduleHref("./wasm/ifcparse.js"))) ??
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
  const createModule = await resolveIfcOpenShellFactory();
  return initWith(createModule, overrides);
}

export async function initParse(overrides?: ModuleInitOverrides): Promise<EmscriptenModule> {
  const createModule = await resolveIfcParseFactory();
  return initWith(createModule, overrides);
}
