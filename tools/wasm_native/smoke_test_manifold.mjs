// This file was generated with the assistance of an AI coding tool.
import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const distDir = resolve(process.argv[2] ?? "build/wasm-native/dist");
const ifcPath = resolve(process.argv[3] ?? "test/input/WallInstance_IFC4Add2.ifc");

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

  console.log("=== Manifold Triangulation Smoke Test ===");
  await api.loadPlugin("schema", "ifc4");
  console.log("1. Schema loaded");
  await api.loadPlugin("kernel", "manifold");
  console.log("2. Manifold kernel loaded");
  await api.loadPlugin("mapping", "ifc4");
  console.log("3. Mapping loaded");

  const model = api.open(ifcPath, false);
  console.log("4. IFC file opened");

  const settings = api.geomCreateSettings();
  settings.geomSettingsSetBool("weld-vertices", false);
  const iterator = api.geomCreateIterator("manifold", settings, model, 1);
  if (!iterator.geomIteratorInitialize()) {
    throw new Error("iteratorInitialize returned false");
  }
  console.log("5. Triangulation iterator initialized (manifold kernel)");

  let totalVerts = 0;
  let totalFaces = 0;
  let totalNormals = 0;
  let elementCount = 0;

  do {
    const triElem = iterator.geomIteratorGetAsTriangulationElement();
    if (!triElem) break;

    const geom = triElem.geomTriangulationElementGeometry();
    const verts = geom.geomTriangulationVerts();
    const faces = geom.geomTriangulationFaces();
    const normals = geom.geomTriangulationNormals();

    totalVerts += verts.length;
    totalFaces += faces.length;
    totalNormals += normals.length;
    elementCount++;

    if (elementCount <= 3) {
      console.log(`   Element ${elementCount}: ${verts.length / 3} verts, ${faces.length / 3} faces, ${normals.length / 3} normals`);
    }

    geom.destroy();
    triElem.destroy();
  } while (iterator.geomIteratorNext());

  console.log(`6. Processed ${elementCount} elements`);
  console.log(`   Total: ${totalVerts / 3} vertices, ${totalFaces / 3} triangles, ${totalNormals / 3} normals`);

  if (totalVerts === 0) throw new Error("No vertices generated");
  if (totalFaces === 0) throw new Error("No faces generated");
  if (totalNormals === 0) throw new Error("No normals generated");
  console.log(`7. Triangulation data validated (${totalVerts / 3} verts, ${totalFaces / 3} faces, ${totalNormals / 3} normals)`);

  iterator.destroy();
  settings.destroy();
  model.destroy();
  console.log("=== ALL MANIFOLD TRIANGULATION TESTS PASSED ===");
} catch (error) {
  console.error("Manifold triangulation failed:", error instanceof Error ? error.message : String(error));
  throw error;
}
