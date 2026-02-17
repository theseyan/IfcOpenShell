declare module "ifczero/wasm/ifcopenshell.js" {
  import type { CreateModuleFn } from "./types.js";
  const createModule: CreateModuleFn;
  export default createModule;
}

declare module "ifczero/wasm/ifcparse.js" {
  import type { CreateModuleFn } from "./types.js";
  const createModule: CreateModuleFn;
  export default createModule;
}
