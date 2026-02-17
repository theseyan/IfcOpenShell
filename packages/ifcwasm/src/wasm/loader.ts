import type { EmscriptenModule, CreateModuleFn } from "./types.js";

export type { EmscriptenModule, CreateModuleFn };

export async function loadModule(
  createModule: CreateModuleFn,
  overrides?: Partial<EmscriptenModule>,
): Promise<EmscriptenModule> {
  return createModule(overrides);
}
