import type { EmscriptenModule, CreateModuleFn, ModuleInitOverrides } from "./types.js";

export type { EmscriptenModule, CreateModuleFn, ModuleInitOverrides };

export async function loadModule(
  createModule: CreateModuleFn,
  overrides?: ModuleInitOverrides,
): Promise<EmscriptenModule> {
  return createModule(overrides);
}
