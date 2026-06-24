// This file was generated with the assistance of an AI coding tool.
import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

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
    fileURLToPath(new URL("ifcopenshell_wasm.wasm", distUrl)),
    {
      pluginBaseUrl: distUrl.href,
      pluginManifest: manifest,
      pluginLoader: (url) => readFileSync(fileURLToPath(url)),
    },
  );

  console.log("=== OCCT Smoke Test ===");
  await api.loadPlugin("schema", "ifc4");
  console.log("1. Schema loaded");
  await api.loadPlugin("kernel", "opencascade");
  console.log("2. OCCT kernel loaded");
  await api.loadPlugin("geometry_serializer", serializerId);
  console.log(`3. ${serializerId.toUpperCase()} serializer loaded`);
  await api.loadPlugin("mapping", "ifc4");
  console.log("4. Mapping loaded");
  await api.loadPlugin("tree", "opencascade.brep");
  console.log("5. OCCT BRep tree plugin loaded");
  await api.loadPlugin("tree", "opencascade.trianglebvh");
  console.log("6. OCCT triangle BVH tree plugin loaded");

  const model = api.parse.openBytes(readFileSync(ifcPath), "WallInstance_IFC4Add2.ifc", false);
  const settings = api.geom.createSettings();
  const iterator = api.geom.createIterator("opencascade", settings, model, 1);
  if (!iterator.initialize()) {
    throw new Error("iterator.initialize() returned false");
  }
  console.log("7. Iterator initialized");
  if (!iterator.get()) {
    throw new Error("iterator.get() returned null");
  }
  console.log("8. First geometry step succeeded");

  const treeFromFile = api.geom.createTreeFromFileWithSettings(model, settings);
  console.log("9. createTreeFromFileWithSettings succeeded");

  const treeFromIterator = api.geom.createTreeFromIterator(iterator);
  console.log("10. createTreeFromIterator succeeded");

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
