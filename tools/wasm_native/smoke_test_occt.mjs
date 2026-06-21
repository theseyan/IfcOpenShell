// This file was generated with the assistance of an AI coding tool.
import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const distDir = resolve(process.argv[2] ?? "build/wasm-native/dist");
const ifcPath = resolve(process.argv[3] ?? "test/input/WallInstance_IFC4Add2.ifc");
const serializerId = process.argv[4] ?? "svg";

const distUrl = pathToFileURL(`${distDir}/`);
const initIfcOpenShellWasm = (await import(new URL("ifcopenshell_wasm.mjs", distUrl))).default;
const { createIfcOpenshellModule } = await import(new URL("ifcopenshell_api.mjs", distUrl));

const manifest = JSON.parse(readFileSync(new URL("ifcopenshell_plugins.json", distUrl), "utf8"));

try {
  const api = await createIfcOpenshellModule(
    initIfcOpenShellWasm,
    new URL("ifcopenshell_wasm.wasm", distUrl).pathname,
    {
      pluginBaseUrl: distUrl.pathname,
      pluginManifest: manifest,
    },
  );

  // Critical path: load serializer BEFORE iterator use.
  // This was the crash path — loading an OCCT-backed serializer plugin before
  // the first OCCT geometry iteration caused geomIteratorInitialize() to crash
  // with "memory access out of bounds" because the serializer side module
  // duplicated OCCT runtime state. The fix moves heavy OCCT serializer
  // implementation code into the kernel provider module.
  console.log("=== OCCT Smoke Test ===");
  await api.loadPlugin("schema", "ifc4");
  console.log("1. Schema loaded");
  await api.loadPlugin("kernel", "opencascade");
  console.log("2. OCCT kernel loaded (provider)");
  await api.loadPlugin("geometry_serializer", serializerId);
  console.log(`3. ${serializerId.toUpperCase()} serializer loaded (local registrar)`);
  await api.loadPlugin("mapping", "ifc4");
  console.log("4. Mapping loaded");
  await api.loadPlugin("tree", "opencascade.brep");
  console.log("5. OCCT tree plugin loaded");
  await api.loadPlugin("tree", "opencascade.trianglebvh");
  console.log("6. OCCT triangle BVH tree plugin loaded");

  const model = api.open(ifcPath, false);
  const settings = api.geomCreateSettings();
  const iterator = api.geomCreateIterator("opencascade", settings, model, 1);
  if (!iterator.geomIteratorInitialize()) {
    throw new Error("iteratorInitialize returned false");
  }
  console.log("7. Iterator initialized");
  if (!iterator.geomIteratorGet()) {
    throw new Error("iteratorGet returned null");
  }
  console.log("8. First geometry step succeeded");

  const treeFromFile = api.geomCreateTree();
  const addFile = treeFromFile.addFile ?? treeFromFile.geomTreeAddFile;
  if (typeof addFile !== "function") {
    throw new Error("tree add_file binding is missing");
  }
  addFile.call(treeFromFile, model, settings);
  console.log(`9. tree ${typeof treeFromFile.addFile === "function" ? "addFile" : "geomTreeAddFile"} succeeded`);

  const treeFromIterator = api.geomCreateTree();
  const addIterator = treeFromIterator.addIterator ?? treeFromIterator.geomTreeAddIterator;
  if (typeof addIterator !== "function") {
    throw new Error("tree add_iterator binding is missing");
  }
  addIterator.call(treeFromIterator, iterator);
  console.log(`10. tree ${typeof treeFromIterator.addIterator === "function" ? "addIterator" : "geomTreeAddIterator"} succeeded`);

  treeFromIterator.destroy();
  treeFromFile.destroy();
  iterator.destroy();
  settings.destroy();
  model.destroy();
  console.log("=== ALL OCCT SMOKE TESTS PASSED ===");
} catch (error) {
  console.error("OCCT smoke failed:", error instanceof Error ? error.message : String(error));
  throw error;
}
