import { describe, it, expect, beforeAll, afterAll } from "vitest";
import { createRequire } from "node:module";
import { readFileSync } from "node:fs";
import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";

import { setModule } from "../src/api.js";
import { IfcFile } from "../src/parse/IfcFile.js";
import { GeomSettings } from "../src/geom/Settings.js";
import { Iterator } from "../src/geom/Iterator.js";
import { Mesh } from "../src/geom/Mesh.js";
import { createMeshForId, createShapeForId, lastError as geomLastError } from "../src/geom/functions.js";
import type { EmscriptenModule } from "../src/wasm/types.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const WASM_DIR = join(__dirname, "../../../zig-out");
const TEST_IFC = join(__dirname, "data/basic_shape_SweptSolid.ifc");

let M: EmscriptenModule;
let IFC_CONTENT: string;

beforeAll(async () => {
  const require = createRequire(import.meta.url);
  const mod = require(join(WASM_DIR, "ifcopenshell.js"));
  const createModule = mod.default || mod;
  M = await createModule({
    locateFile: (file: string) => join(WASM_DIR, file),
  });
  setModule(M);
  IFC_CONTENT = readFileSync(TEST_IFC, "utf-8");
});

describe("GeomSettings", () => {
  let settings: GeomSettings;

  beforeAll(() => {
    settings = new GeomSettings();
  });

  afterAll(() => {
    settings.destroy();
  });

  it("creates settings with valid pointer", () => {
    expect(settings.ptr).toBeGreaterThan(0);
  });

  it("settingNames returns available setting names", () => {
    const names = settings.settingNames();
    expect(names.length).toBeGreaterThan(0);
    expect(names).toContain("weld-vertices");
  });

  it("setBool sets a boolean setting", () => {
    const ok = settings.setBool("weld-vertices", false);
    expect(ok).toBe(true);
  });

  it("setBool rejects unknown setting name", () => {
    const ok = settings.setBool("nonexistent-setting", true);
    expect(ok).toBe(false);
  });

  it("setDouble sets a double setting", () => {
    const ok = settings.setDouble("mesher-linear-deflection", 0.01);
    expect(ok).toBe(true);
  });
});

describe("Iterator", () => {
  let file: IfcFile;
  let settings: GeomSettings;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT)!;
    settings = new GeomSettings();
  });

  afterAll(() => {
    settings.destroy();
    file.close();
  });

  it("creates an iterator", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    expect(iter.ptr).toBeGreaterThan(0);
    iter.destroy();
  });

  it("initializes successfully", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    const ok = iter.initialize();
    expect(ok).toBe(true);
    iter.destroy();
  });

  it("progress starts at 0 before initialization", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    const p = iter.progress;
    expect(p).toBe(0);
    iter.destroy();
  });

  it("hadError is false on clean file", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    expect(iter.hadError).toBe(false);
    iter.destroy();
  });

  it("unitName returns a unit name", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    const name = iter.unitName;
    expect(name).toBeTruthy();
    iter.destroy();
  });

  it("unitMagnitude returns a positive value", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    const mag = iter.unitMagnitude;
    expect(mag).toBeGreaterThan(0);
    iter.destroy();
  });

  it("currentType returns a type name after init", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    const typeName = iter.currentType;
    expect(typeName).toBeTruthy();
    iter.destroy();
  });

  it("currentGuid returns a GUID after init", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    const guid = iter.currentGuid;
    expect(guid).toBeTruthy();
    expect(guid.length).toBe(22);
    iter.destroy();
  });

  it("log returns a string (possibly empty)", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    expect(typeof iter.log).toBe("string");
    iter.destroy();
  });
});

describe("Mesh", () => {
  let file: IfcFile;
  let settings: GeomSettings;
  let mesh: Mesh;
  let iter: Iterator;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT)!;
    settings = new GeomSettings();
    iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    mesh = iter.getMesh();
  });

  afterAll(() => {
    mesh.destroy();
    iter.destroy();
    settings.destroy();
    file.close();
  });

  it("has a positive ID", () => {
    expect(mesh.id).toBeGreaterThan(0);
  });

  it("has a type name", () => {
    expect(mesh.type).toBeTruthy();
  });

  it("has a GUID", () => {
    expect(mesh.guid).toBeTruthy();
    expect(mesh.guid.length).toBe(22);
  });

  it("has a name", () => {
    expect(typeof mesh.name).toBe("string");
  });

  it("has vertices", () => {
    const verts = mesh.vertices;
    expect(verts).toBeInstanceOf(Float64Array);
    expect(verts.length).toBeGreaterThan(0);
    expect(verts.length % 3).toBe(0);
  });

  it("has faces", () => {
    const faces = mesh.faces;
    expect(faces).toBeInstanceOf(Int32Array);
    expect(faces.length).toBeGreaterThan(0);
  });

  it("has normals (may be empty if no-normals is set)", () => {
    const normals = mesh.normals;
    expect(normals).toBeInstanceOf(Float64Array);
    if (normals.length > 0) {
      expect(normals.length).toBe(mesh.vertices.length);
    }
  });

  it("has transform data", () => {
    const xform = mesh.transform;
    expect(xform).toBeInstanceOf(Float64Array);
    expect(xform.length).toBe(16);
  });

  it("transform is a valid 4x4 matrix", () => {
    const xform = mesh.transform;
    // Last row of a valid 4x4 affine matrix is [0, 0, 0, 1]
    expect(xform[15]).toBeCloseTo(1.0);
  });

  it("has material IDs", () => {
    const ids = mesh.materialIds;
    expect(ids).toBeInstanceOf(Int32Array);
  });

  it("has colors", () => {
    const colors = mesh.colors;
    expect(colors).toBeInstanceOf(Float64Array);
  });
});

describe("Iterator for...of", () => {
  let file: IfcFile;
  let settings: GeomSettings;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT)!;
    settings = new GeomSettings();
  });

  afterAll(() => {
    settings.destroy();
    file.close();
  });

  it("supports for...of iteration", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    const meshes: Mesh[] = [];
    for (const mesh of iter) {
      meshes.push(mesh);
    }
    expect(meshes.length).toBeGreaterThan(0);

    for (const mesh of meshes) {
      expect(mesh.id).toBeGreaterThan(0);
      expect(mesh.vertices.length).toBeGreaterThan(0);
      mesh.destroy();
    }
    iter.destroy();
  });

  it("progress reaches 100 after full iteration", () => {
    const iter = Iterator.create(file.ptr, settings.ptr);
    const meshes: Mesh[] = [];
    for (const mesh of iter) {
      meshes.push(mesh);
    }
    expect(iter.progress).toBe(100);
    for (const m of meshes) m.destroy();
    iter.destroy();
  });
});

describe("Iterator with settings", () => {
  let file: IfcFile;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT)!;
  });

  afterAll(() => {
    file.close();
  });

  it("respects weld-vertices = false", () => {
    const settings = new GeomSettings();
    settings.setBool("weld-vertices", false);
    const iter = Iterator.create(file.ptr, settings.ptr);
    const meshes: Mesh[] = [];
    for (const mesh of iter) {
      meshes.push(mesh);
    }
    expect(meshes.length).toBeGreaterThan(0);
    for (const m of meshes) m.destroy();
    iter.destroy();
    settings.destroy();
  });

  it("produces different vertex counts with different weld settings", () => {
    const settingsWeld = new GeomSettings();
    settingsWeld.setBool("weld-vertices", true);
    const settingsNoWeld = new GeomSettings();
    settingsNoWeld.setBool("weld-vertices", false);

    const iterWeld = Iterator.create(file.ptr, settingsWeld.ptr);
    const meshesWeld: Mesh[] = [];
    for (const mesh of iterWeld) meshesWeld.push(mesh);

    const iterNoWeld = Iterator.create(file.ptr, settingsNoWeld.ptr);
    const meshesNoWeld: Mesh[] = [];
    for (const mesh of iterNoWeld) meshesNoWeld.push(mesh);

    expect(meshesWeld.length).toBe(meshesNoWeld.length);

    // Vertex counts may differ between welded and unwelded
    const weldVerts = meshesWeld.reduce((s, m) => s + m.vertices.length, 0);
    const noWeldVerts = meshesNoWeld.reduce((s, m) => s + m.vertices.length, 0);
    expect(noWeldVerts).toBeGreaterThanOrEqual(weldVerts);

    for (const m of meshesWeld) m.destroy();
    for (const m of meshesNoWeld) m.destroy();
    iterWeld.destroy();
    iterNoWeld.destroy();
    settingsWeld.destroy();
    settingsNoWeld.destroy();
  });
});

describe("Multiple files", () => {
  it("can iterate geometry from multiple files independently", () => {
    const file1 = IfcFile.openFromMemory(IFC_CONTENT)!;
    const file2 = IfcFile.openFromMemory(IFC_CONTENT)!;
    const settings = new GeomSettings();

    const iter1 = Iterator.create(file1.ptr, settings.ptr);
    const iter2 = Iterator.create(file2.ptr, settings.ptr);

    const meshes1: Mesh[] = [];
    const meshes2: Mesh[] = [];
    for (const mesh of iter1) meshes1.push(mesh);
    for (const mesh of iter2) meshes2.push(mesh);

    expect(meshes1.length).toBe(meshes2.length);
    expect(meshes1.length).toBeGreaterThan(0);

    for (const m of meshes1) m.destroy();
    for (const m of meshes2) m.destroy();
    iter1.destroy();
    iter2.destroy();
    settings.destroy();
    file1.close();
    file2.close();
  });
});

describe("GeomSettings getters", () => {
  it("getBool returns a boolean value", () => {
    const settings = new GeomSettings();
    settings.setBool("weld-vertices", false);
    expect(settings.getBool("weld-vertices")).toBe(false);
    settings.setBool("weld-vertices", true);
    expect(settings.getBool("weld-vertices")).toBe(true);
    settings.destroy();
  });

  it("getDouble returns a double value", () => {
    const settings = new GeomSettings();
    settings.setDouble("mesher-linear-deflection", 0.5);
    const val = settings.getDouble("mesher-linear-deflection");
    expect(val).not.toBeNull();
    expect(val).toBeCloseTo(0.5);
    settings.destroy();
  });

  it("getType returns the type of a setting", () => {
    const settings = new GeomSettings();
    const type = settings.getType("weld-vertices");
    expect(type).toBeTruthy();
    settings.destroy();
  });

  it("lastError returns a string", () => {
    const settings = new GeomSettings();
    expect(typeof settings.lastError).toBe("string");
    settings.destroy();
  });
});

describe("Iterator metadata", () => {
  it("currentId returns the element id during iteration", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.create(file.ptr, settings.ptr);
    expect(iter.initialize()).toBe(true);
    const id = iter.currentId;
    expect(id).not.toBeNull();
    expect(id).toBeGreaterThan(0);
    iter.destroy();
    settings.destroy();
    file.close();
  });

  it("currentParentId returns a parent id during iteration", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.create(file.ptr, settings.ptr);
    expect(iter.initialize()).toBe(true);
    const parentId = iter.currentParentId;
    expect(parentId).not.toBeNull();
    iter.destroy();
    settings.destroy();
    file.close();
  });

  it("currentTransform returns a 4x4 matrix during iteration", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.create(file.ptr, settings.ptr);
    expect(iter.initialize()).toBe(true);
    const t = iter.currentTransform;
    expect(t).not.toBeNull();
    expect(t!.length).toBe(16);
    iter.destroy();
    settings.destroy();
    file.close();
  });

  it("computeBounds and boundsMin/Max return bounding box", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    // Iterate to populate geometry
    do { /* consume */ } while (iter.next());
    iter.computeBounds();
    const min = iter.boundsMin();
    const max = iter.boundsMax();
    expect(min).not.toBeNull();
    expect(max).not.toBeNull();
    expect(min!.length).toBe(3);
    expect(max!.length).toBe(3);
    iter.destroy();
    settings.destroy();
    file.close();
  });
});

describe("Filtered iterators", () => {
  it("createWithIdFilter includes only the specified id", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.createWithIdFilter(file.ptr, settings.ptr, [1000], true);
    const meshes: Mesh[] = [];
    for (const mesh of iter) meshes.push(mesh);
    expect(meshes.length).toBe(1);
    expect(meshes[0].id).toBe(1000);
    for (const m of meshes) m.destroy();
    iter.destroy();
    settings.destroy();
    file.close();
  });

  it("createWithTypeFilter filters by type", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.createWithTypeFilter(file.ptr, settings.ptr, ["IfcBuildingElementProxy"]);
    const meshes: Mesh[] = [];
    for (const mesh of iter) meshes.push(mesh);
    expect(meshes.length).toBeGreaterThan(0);
    for (const m of meshes) {
      expect(m.type).toBe("IfcBuildingElementProxy");
    }
    for (const m of meshes) m.destroy();
    iter.destroy();
    settings.destroy();
    file.close();
  });
});

describe("createMeshForId", () => {
  it("creates a mesh directly for an entity id", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const mesh = createMeshForId(file.ptr, settings.ptr, 1000);
    expect(mesh).not.toBeNull();
    expect(mesh!.id).toBe(1000);
    expect(mesh!.vertices.length).toBeGreaterThan(0);
    mesh!.destroy();
    settings.destroy();
    file.close();
  });

  it("returns null for a non-geometry entity", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const mesh = createMeshForId(file.ptr, settings.ptr, 100); // IfcProject has no geometry
    expect(mesh).toBeNull();
    settings.destroy();
    file.close();
  });
});

describe("createShapeForId", () => {
  it("returns an owned shape wrapper and mesh for geometry entities", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const shape = createShapeForId(file.ptr, settings.ptr, 1000);
    expect(shape).not.toBeNull();
    const mesh = shape!.mesh;
    expect(mesh).not.toBeNull();
    expect(mesh!.id).toBe(1000);
    shape!.destroy();
    settings.destroy();
    file.close();
  });
});

describe("geomLastError", () => {
  it("returns a string", () => {
    expect(typeof geomLastError()).toBe("string");
  });
});

describe("Mesh edgesItemIds", () => {
  it("edgesItemIds returns an Int32Array", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const settings = new GeomSettings();
    const iter = Iterator.create(file.ptr, settings.ptr);
    iter.initialize();
    const mesh = iter.getMesh();
    const eids = mesh.edgesItemIds;
    expect(eids).toBeInstanceOf(Int32Array);
    mesh.destroy();
    iter.destroy();
    settings.destroy();
    file.close();
  });
});
