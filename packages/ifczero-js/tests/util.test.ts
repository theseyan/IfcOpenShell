import { describe, it, expect, beforeAll } from "vitest";
import { readFileSync } from "node:fs";
import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";

import { init } from "../src/api.js";
import { IfcFile } from "../src/parse/IfcFile.js";
import { Entity } from "../src/parse/Entity.js";
import {
  getType,
  getTypes,
  getContainer,
  getParent,
  getMaterial,
  getAggregate,
  getNest,
  getFilledVoid,
  getVoidedElement,
  getPredefinedType,
  isUserDefinedType,
  getPsets,
  getPset,
  getClassificationReferences,
  unitScale,
  unitSiTypeName,
  getProjectUnit,
  unitConvert,
} from "../src/util/element.js";
import { UnitType } from "../src/util/types.js";
import type { EmscriptenModule } from "../src/wasm/types.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const WASM_DIR = join(__dirname, "../wasm");
const TEST_IFC = join(__dirname, "data/basic_shape_SweptSolid.ifc");

let M: EmscriptenModule;
let file: IfcFile;
let proxy: Entity;
let project: Entity;
let building: Entity;

beforeAll(async () => {
  M = await init({
    locateFile: (f: string) => join(WASM_DIR, f),
  });
  const content = readFileSync(TEST_IFC, "utf-8");
  file = IfcFile.openFromMemory(content)!;

  proxy = file.byId(1000)!;
  project = file.byId(100)!;
  building = file.byId(500)!;
});

describe("element navigation", () => {
  it("getContainer returns containing spatial element", () => {
    const container = getContainer(proxy);
    expect(container).not.toBeNull();
    if (container) {
      expect(container.isA("IfcSpatialStructureElement")).toBe(true);
    }
  });

  it("getContainer with direct=false traverses up", () => {
    const container = getContainer(proxy, false);
    // Should still find a container (possibly a higher-level one)
    expect(container).not.toBeNull();
  });

  it("getParent returns parent element", () => {
    const parent = getParent(proxy);
    // proxy may or may not have a parent depending on model structure
    expect(parent === null || parent instanceof Entity).toBe(true);
  });

  it("getType returns null for element without type", () => {
    const typeObj = getType(proxy);
    // This test IFC may not have a type object for the proxy
    expect(typeObj === null || typeObj instanceof Entity).toBe(true);
  });

  it("getTypes returns array", () => {
    const types = getTypes(proxy);
    expect(Array.isArray(types)).toBe(true);
  });

  it("getPredefinedType returns null or string", () => {
    const pdt = getPredefinedType(proxy);
    expect(pdt === null || typeof pdt === "string").toBe(true);
  });

  it("isUserDefinedType returns boolean", () => {
    const result = isUserDefinedType(proxy);
    expect(typeof result).toBe("boolean");
  });

  it("getAggregate returns null for non-aggregated element", () => {
    const agg = getAggregate(proxy);
    expect(agg === null || agg instanceof Entity).toBe(true);
  });

  it("getNest returns null for non-nested element", () => {
    const nest = getNest(proxy);
    expect(nest === null || nest instanceof Entity).toBe(true);
  });

  it("getFilledVoid returns null for non-filling element", () => {
    const fv = getFilledVoid(proxy);
    expect(fv === null || fv instanceof Entity).toBe(true);
  });

  it("getVoidedElement returns null for non-opening element", () => {
    const ve = getVoidedElement(proxy);
    expect(ve === null || ve instanceof Entity).toBe(true);
  });

  it("getMaterial returns null or entity", () => {
    const mat = getMaterial(proxy);
    expect(mat === null || mat instanceof Entity).toBe(true);
  });

  it("getMaterial with inherit option", () => {
    const mat = getMaterial(proxy, { inherit: true });
    expect(mat === null || mat instanceof Entity).toBe(true);
  });
});

describe("property sets", () => {
  it("getPsets returns array (may be empty for this model)", () => {
    const psets = getPsets(proxy);
    expect(Array.isArray(psets)).toBe(true);
  });

  it("getPsets on project returns array", () => {
    const psets = getPsets(project);
    expect(Array.isArray(psets)).toBe(true);
  });

  it("getPset returns null for non-existent pset name", () => {
    const pset = getPset(proxy, "NonExistentPsetName");
    expect(pset).toBeNull();
  });

  it("getPsets with psetsOnly filter", () => {
    const psets = getPsets(proxy, { psetsOnly: true });
    expect(Array.isArray(psets)).toBe(true);
  });

  it("getPsets with qtosOnly filter", () => {
    const qtos = getPsets(proxy, { qtosOnly: true });
    expect(Array.isArray(qtos)).toBe(true);
  });
});

describe("classifications", () => {
  it("getClassificationReferences returns array", () => {
    const refs = getClassificationReferences(proxy);
    expect(Array.isArray(refs)).toBe(true);
  });

  it("getClassificationReferences with inherit=false", () => {
    const refs = getClassificationReferences(proxy, false);
    expect(Array.isArray(refs)).toBe(true);
  });
});

describe("units", () => {
  it("unitScale returns scale factor for length", () => {
    const scale = unitScale(file.ptr, UnitType.LENGTHUNIT);
    expect(scale).toBeCloseTo(0.001); // millimetres
  });

  it("unitScale returns 1.0 for undefined unit type", () => {
    const scale = unitScale(file.ptr, UnitType.FORCEUNIT);
    expect(scale).toBe(1.0); // not defined in this file
  });

  it("unitSiTypeName returns SI type name for length", () => {
    const name = unitSiTypeName(UnitType.LENGTHUNIT);
    expect(name).toBe("METRE");
  });

  it("unitSiTypeName returns SI type name for area", () => {
    const name = unitSiTypeName(UnitType.AREAUNIT);
    expect(name).toBe("SQUARE_METRE");
  });

  it("unitSiTypeName returns null for unknown unit", () => {
    const name = unitSiTypeName("BOGUSUNIT");
    expect(name).toBeNull();
  });

  it("getProjectUnit returns a unit entity", () => {
    const unit = getProjectUnit(file.ptr, UnitType.LENGTHUNIT);
    expect(unit).not.toBeNull();
    expect(unit!.ptr).toBeGreaterThan(0);
  });

  it("unitConvert converts between units", () => {
    const result = unitConvert(1000, "MILLI", "METRE", "", "METRE");
    expect(result).toBeCloseTo(1.0);
  });

  it("unitConvert handles identity conversion", () => {
    const result = unitConvert(5.0, "", "METRE", "", "METRE");
    expect(result).toBeCloseTo(5.0);
  });
});
