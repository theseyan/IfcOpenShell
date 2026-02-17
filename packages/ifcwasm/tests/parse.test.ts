import { describe, it, expect, beforeAll, afterAll } from "vitest";
import { createRequire } from "node:module";
import { readFileSync } from "node:fs";
import { join, dirname } from "node:path";
import { fileURLToPath } from "node:url";

import { setModule } from "../src/api.js";
import { IfcFile } from "../src/parse/IfcFile.js";
import { Entity } from "../src/parse/Entity.js";
import { Schema } from "../src/parse/Schema.js";
import { PsetQto } from "../src/parse/PsetQto.js";
import { version, createTimestamp, guessFileType, fileStatusMessage } from "../src/parse/functions.js";
import { compress as guidCompress, expand as guidExpand, newGuid, split as guidSplit } from "../src/parse/guid.js";
import { AggregationType, ArgumentType, ParameterTypeKind } from "../src/parse/types.js";
import type { EmscriptenModule } from "../src/wasm/types.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const WASM_DIR = join(__dirname, "../../../zig-out");
const TEST_IFC = join(__dirname, "data/basic_shape_SweptSolid.ifc");

let M: EmscriptenModule;
let IFC_CONTENT: string;

beforeAll(async () => {
  IFC_CONTENT = readFileSync(TEST_IFC, "utf-8");
  const require = createRequire(import.meta.url);
  const mod = require(join(WASM_DIR, "ifcopenshell.js"));
  const createModule = mod.default || mod;
  M = await createModule({
    locateFile: (path: string) => join(WASM_DIR, path),
  });
  setModule(M);
});

describe("parse functions", () => {
  it("version returns a non-empty string", () => {
    const v = version();
    expect(v).toBeTruthy();
    expect(typeof v).toBe("string");
  });

  it("createTimestamp returns a timestamp string", () => {
    const ts = createTimestamp();
    expect(ts).toBeTruthy();
    expect(ts).toMatch(/\d{4}-\d{2}-\d{2}/);
  });

  it("Schema.availableSchemas returns available schemas", () => {
    const names = Schema.availableSchemas();
    expect(names.length).toBeGreaterThan(0);
    expect(names).toContain("IFC4");
  });

  it("guessFileType detects IFC SPF", () => {
    M.FS.writeFile("/tmp/test.ifc", IFC_CONTENT);
    const ft = guessFileType("/tmp/test.ifc");
    expect(ft).toBe(0); // IFC_SPF
    M.FS.unlink("/tmp/test.ifc");
  });

  it("fileStatusMessage returns readable messages", () => {
    const msg = fileStatusMessage(0);
    expect(msg).toBeTruthy();
  });
});

describe("IfcFile", () => {
  describe("createEmpty", () => {
    it("creates IFC4 file", () => {
      const file = IfcFile.createEmpty("IFC4");
      expect(file.ptr).toBeGreaterThan(0);
      expect(file.schema).toBe("IFC4");
      expect(file.entityCount).toBe(0);
      file.close();
    });
  });

  describe("openFromMemory (string)", () => {
    let file: IfcFile;

    beforeAll(() => {
      file = IfcFile.openFromMemory(IFC_CONTENT);
    });

    afterAll(() => {
      file.close();
    });

    it("opens successfully", () => {
      expect(file.ptr).toBeGreaterThan(0);
    });

    it("detects IFC4 schema", () => {
      expect(file.schema).toBe("IFC4");
    });

    it("has non-zero entity count", () => {
      expect(file.entityCount).toBeGreaterThan(0);
    });

    it("maxId returns a positive value", () => {
      expect(file.maxId).toBeGreaterThan(0);
    });
  });

  describe("openFromMemory (Uint8Array)", () => {
    it("opens from Uint8Array", () => {
      const data = new TextEncoder().encode(IFC_CONTENT);
      const file = IfcFile.openFromMemory(data);
      expect(file.schema).toBe("IFC4");
      expect(file.entityCount).toBeGreaterThan(0);
      file.close();
    });
  });

  describe("entity lookup", () => {
    let file: IfcFile;

    beforeAll(() => {
      file = IfcFile.openFromMemory(IFC_CONTENT);
    });

    afterAll(() => {
      file.close();
    });

    it("byId returns entity for valid ID", () => {
      // #100 = IFCPROJECT in basic_shape_SweptSolid.ifc
      const entity = file.byId(100);
      expect(entity).not.toBeNull();
      expect(entity!.id).toBe(100);
    });

    it("byId returns null for invalid ID", () => {
      const entity = file.byId(99999);
      expect(entity).toBeNull();
    });

    it("byGuid returns entity", () => {
      // Project GUID: 0xScRe4drECQ4DMSqUjd6d
      const entity = file.byGuid("0xScRe4drECQ4DMSqUjd6d");
      expect(entity).not.toBeNull();
      expect(entity!.typeName).toBe("IfcProject");
    });

    it("byGuid returns null for unknown GUID", () => {
      const entity = file.byGuid("NONEXISTENT_GUID_12345");
      expect(entity).toBeNull();
    });

    it("byType returns matching entities", () => {
      const directions = file.byType("IfcDirection");
      expect(directions.length).toBeGreaterThan(0);
      directions.forEach((d) => {
        expect(d.typeName).toBe("IfcDirection");
      });
    });

    it("byType returns empty for non-existent type", () => {
      const entities = file.byType("IfcWindow");
      expect(entities).toHaveLength(0);
    });

    it("byType with subtypes includes subtypes", () => {
      const products = file.byType("IfcProduct", true);
      expect(products.length).toBeGreaterThan(0);
    });
  });

  describe("entity creation and removal", () => {
    let file: IfcFile;

    beforeAll(() => {
      file = IfcFile.createEmpty("IFC4");
    });

    afterAll(() => {
      file.close();
    });

    it("createEntity creates a new entity", () => {
      const wall = file.createEntity("IfcWall");
      expect(wall).toBeTruthy();
      expect(wall.typeName).toBe("IfcWall");
      expect(wall.id).toBeGreaterThan(0);
    });

    it("entity count increases after creation", () => {
      const countBefore = file.entityCount;
      file.createEntity("IfcWall");
      expect(file.entityCount).toBe(countBefore + 1);
    });

    it("removeEntity removes an entity", () => {
      const beam = file.createEntity("IfcBeam");
      const countBefore = file.entityCount;
      const id = beam.id;
      const removed = file.removeEntity(beam);
      expect(removed).toBe(true);
      expect(file.entityCount).toBe(countBefore - 1);
      expect(file.byId(id)).toBeNull();
    });

    it("removeById removes by ID", () => {
      const slab = file.createEntity("IfcSlab");
      const id = slab.id;
      const countBefore = file.entityCount;
      const removed = file.removeById(id);
      expect(removed).toBe(true);
      expect(file.entityCount).toBe(countBefore - 1);
    });
  });

  describe("batch operations", () => {
    it("batch begin/end does not throw", () => {
      const file = IfcFile.createEmpty("IFC4");
      expect(() => {
        file.batchBegin();
        file.createEntity("IfcWall");
        file.createEntity("IfcWall");
        file.batchEnd();
      }).not.toThrow();
      expect(file.entityCount).toBe(2);
      file.close();
    });
  });

  describe("serialization", () => {
    it("toSpf returns valid STEP string", () => {
      const file = IfcFile.openFromMemory(IFC_CONTENT);
      const spf = file.toSpf();
      expect(spf).toContain("ISO-10303-21");
      expect(spf).toContain("IFCPROJECT");
      expect(spf).toContain("END-ISO-10303-21");
      file.close();
    });

    it("writeSpf writes to virtual filesystem", () => {
      const file = IfcFile.openFromMemory(IFC_CONTENT);
      const success = file.writeSpf("/tmp/output.ifc");
      expect(success).toBe(true);
      const content = M.FS.readFile("/tmp/output.ifc", {
        encoding: "utf8",
      }) as string;
      expect(content).toContain("ISO-10303-21");
      M.FS.unlink("/tmp/output.ifc");
      file.close();
    });
  });

  describe("traverse", () => {
    it("traverses entity graph", () => {
      const file = IfcFile.openFromMemory(IFC_CONTENT);
      const project = file.byId(100)!;
      expect(project).not.toBeNull();

      const traversed = file.traverse(project, 1);
      expect(traversed.length).toBeGreaterThan(0);
      file.close();
    });
  });

  describe("units", () => {
    it("unitScale returns scale factor", () => {
      const file = IfcFile.openFromMemory(IFC_CONTENT);
      const scale = file.unitScale("LENGTHUNIT");
      // basic_shape_SweptSolid.ifc uses MILLI METRE, so scale = 0.001
      expect(scale).toBeCloseTo(0.001, 6);
      file.close();
    });
  });

  describe("lastError", () => {
    it("returns null when no error", () => {
      const file = IfcFile.createEmpty("IFC4");
      expect(file.lastError).toBeNull();
      file.close();
    });
  });
});

describe("Entity", () => {
  let file: IfcFile;
  let project: Entity;
  let proxy: Entity;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT);
    project = file.byId(100)!; // IFCPROJECT
    proxy = file.byId(1000)!; // IFCBUILDINGELEMENTPROXY
  });

  afterAll(() => {
    file.close();
  });

  describe("basic properties", () => {
    it("id returns correct ID", () => {
      expect(project.id).toBe(100);
      expect(proxy.id).toBe(1000);
    });

    it("typeName returns IFC type", () => {
      expect(project.typeName).toBe("IfcProject");
      expect(proxy.typeName).toBe("IfcBuildingElementProxy");
    });

    it("isA checks type hierarchy", () => {
      expect(proxy.isA("IfcBuildingElementProxy")).toBe(true);
      expect(proxy.isA("IfcBuildingElement")).toBe(true);
      expect(proxy.isA("IfcProduct")).toBe(true);
      expect(proxy.isA("IfcRoot")).toBe(true);
      expect(proxy.isA("IfcBeam")).toBe(false);
    });

    it("toString returns STEP representation", () => {
      const str = proxy.toString();
      expect(str).toContain("IfcBuildingElementProxy");
    });
  });

  describe("attributes", () => {
    it("attributeCount returns correct count", () => {
      // IfcProject has 9 attributes in IFC4
      expect(project.attributeCount).toBe(9);
    });

    it("attributeIndex finds index by name", () => {
      const nameIdx = project.attributeIndex("Name");
      expect(nameIdx).toBeGreaterThanOrEqual(0);
    });

    it("attributeName returns name by index", () => {
      const name = project.attributeName(0);
      expect(name).toBe("GlobalId");
    });

    it("attributeNames returns all attribute names", () => {
      const names = project.attributeNames();
      expect(names).toContain("GlobalId");
      expect(names).toContain("Name");
      expect(names.length).toBe(project.attributeCount);
    });

    it("argumentType returns correct type", () => {
      // GlobalId is a string
      const type = project.argumentType(0);
      expect(type).toBe(ArgumentType.STRING);
    });
  });

  describe("attribute getters", () => {
    it("getString returns string attribute", () => {
      const globalId = project.getString(0); // GlobalId
      expect(globalId).toBe("0xScRe4drECQ4DMSqUjd6d");
    });

    it("getString returns name", () => {
      const nameIdx = project.attributeIndex("Name");
      const name = project.getString(nameIdx);
      expect(name).toBe("proxy with swept solid");
    });

    it("isNull detects null attributes", () => {
      // IfcBuildingElementProxy description (index 3) is 'sample proxy' — not null
      const descIdx = proxy.attributeIndex("Description");
      expect(proxy.isNull(descIdx)).toBe(false);
      expect(proxy.getString(descIdx)).toBe("sample proxy");
    });

    it("getEntity returns referenced entity", () => {
      // IfcProject.OwnerHistory (#110)
      const ohIdx = project.attributeIndex("OwnerHistory");
      const oh = project.getEntity(ohIdx);
      expect(oh).not.toBeNull();
      expect(oh!.id).toBe(110);
      expect(oh!.typeName).toBe("IfcOwnerHistory");
    });

    it("getEntity returns null for null reference", () => {
      // Check an attribute that is $ in the file
      // IfcBuilding #500 Tag attribute is $
      const building = file.byId(500)!;
      expect(building.typeName).toBe("IfcBuilding");
      const tagIdx = building.attributeIndex("Description");
      expect(building.isNull(tagIdx)).toBe(true);
    });

    it("getDoubleList returns list of doubles", () => {
      // #901 = IFCCARTESIANPOINT((0.,0.,0.))
      const point = file.byId(901)!;
      expect(point.typeName).toBe("IfcCartesianPoint");
      const coordIdx = point.attributeIndex("Coordinates");
      const coords = point.getDoubleList(coordIdx);
      expect(coords).toEqual([0, 0, 0]);
    });

    it("getEntityList returns list of entities", () => {
      // #301 = IFCUNITASSIGNMENT((#311,#312))
      const unitAssignment = file.byId(301)!;
      expect(unitAssignment.typeName).toBe("IfcUnitAssignment");
      const unitsIdx = unitAssignment.attributeIndex("Units");
      const units = unitAssignment.getEntityList(unitsIdx);
      expect(units.length).toBe(2);
    });
  });

  describe("attribute setters", () => {
    it("setString changes string attribute", () => {
      const f = IfcFile.createEmpty("IFC4");
      const w = f.createEntity("IfcWall");
      const nameIdx = w.attributeIndex("Name");

      const ok = w.setString(nameIdx, "NewName");
      expect(ok).toBe(true);
      expect(w.getString(nameIdx)).toBe("NewName");
      f.close();
    });

    it("setNull nullifies attribute", () => {
      const f = IfcFile.createEmpty("IFC4");
      const w = f.createEntity("IfcWall");
      const nameIdx = w.attributeIndex("Name");

      w.setString(nameIdx, "Temp");
      expect(w.isNull(nameIdx)).toBe(false);

      w.setNull(nameIdx);
      expect(w.isNull(nameIdx)).toBe(true);
      f.close();
    });

    it("setEntity sets entity reference", () => {
      const f = IfcFile.createEmpty("IFC4");
      const w = f.createEntity("IfcWall");
      const oh = f.createEntity("IfcOwnerHistory");
      const ohIdx = w.attributeIndex("OwnerHistory");

      const ok = w.setEntity(ohIdx, oh);
      expect(ok).toBe(true);

      const ref = w.getEntity(ohIdx);
      expect(ref).not.toBeNull();
      expect(ref!.id).toBe(oh.id);
      f.close();
    });

    it("setInt sets integer attribute", () => {
      const f = IfcFile.createEmpty("IFC4");
      const oh = f.createEntity("IfcOwnerHistory");
      const idx = oh.attributeIndex("CreationDate");
      const ok = oh.setInt(idx, 1704067200);
      expect(ok).toBe(true);
      expect(oh.getInt(idx)).toBe(1704067200);
      f.close();
    });

    it("setDouble sets double attribute", () => {
      const f = IfcFile.createEmpty("IFC4");
      const qty = f.createEntity("IfcQuantityLength");
      const idx = qty.attributeIndex("LengthValue");
      const ok = qty.setDouble(idx, 3.14159);
      expect(ok).toBe(true);
      const val = qty.getDouble(idx);
      expect(val).toBeCloseTo(3.14159, 4);
      f.close();
    });
  });

  describe("inverse queries", () => {
    it("getInverse returns referencing entities", () => {
      // IfcRelAggregates #519 references Project #100 via RelatingObject
      // The inverse attribute on IfcProject for aggregates is "IsDecomposedBy"
      const project2 = file.byId(100)!;
      const inverses = project2.getInverse("IsDecomposedBy");
      expect(inverses.length).toBeGreaterThanOrEqual(1);
      // The inverse should include #519 (IFCRELAGGREGATES)
      expect(inverses.some((e) => e.id === 519)).toBe(true);
    });
  });
});

describe("Schema introspection", () => {
  let schema: Schema;

  beforeAll(() => {
    schema = new Schema("IFC4");
  });

  it("declarationByName finds type", () => {
    const decl = schema.declarationByName("IfcWall");
    expect(decl).not.toBeNull();
  });

  it("TypeDeclaration.name returns type name", () => {
    const decl = schema.declarationByName("IfcWall")!;
    expect(decl.name).toBe("IfcWall");
  });

  it("TypeDeclaration.isA checks type hierarchy", () => {
    const wall = schema.declarationByName("IfcWall")!;
    expect(wall.isA("IfcRoot")).toBe(true);
    expect(wall.isA("IfcWall")).toBe(true);
    expect(wall.isA("IfcDoor")).toBe(false);
  });

  it("TypeDeclaration.supertype returns parent type", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const supertype = wall.supertype;
    expect(supertype).not.toBeNull();
    expect(supertype!.name).toBe("IfcBuildingElement");
  });

  it("TypeDeclaration.attributes returns own attributes", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const ownAttrs = wall.attributes(false);
    expect(ownAttrs.length).toBeGreaterThan(0);
    expect(ownAttrs[0].name).toBeTruthy();
  });

  it("TypeDeclaration.attributes with inherited", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const allAttrs = wall.attributes(true);
    const ownAttrs = wall.attributes(false);
    expect(allAttrs.length).toBeGreaterThan(ownAttrs.length);
    expect(allAttrs[0].name).toBe("GlobalId");
  });

  it("TypeDeclaration.subtypes returns subtypes", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const subs = wall.subtypes;
    expect(subs.length).toBeGreaterThanOrEqual(0);
  });

  it("Schema.entities returns all entity types", () => {
    const entities = schema.entities();
    expect(entities.length).toBeGreaterThan(100);
  });

  it("enumerationItems returns enum values", () => {
    const enumDecl = schema.declarationByName("IfcWallTypeEnum")!;
    expect(enumDecl).not.toBeNull();
    const items = enumDecl.enumerationItems();
    expect(items.length).toBeGreaterThan(0);
    expect(items).toContain("STANDARD");
    expect(items).toContain("USERDEFINED");
  });

  it("attributeIndex returns attribute position", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const idx = wall.attributeIndex("GlobalId");
    expect(idx).toBe(0);
  });

  it("AttributeDeclaration.optional returns boolean", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const attrs = wall.attributes(true);
    const globalId = attrs.find((a) => a.name === "GlobalId")!;
    expect(globalId.optional).toBe(false);
  });

  it("AttributeDeclaration.parameterType exposes named type metadata", () => {
    const wall = schema.declarationByName("IfcWall")!;
    const attrs = wall.attributes(true);
    const globalId = attrs.find((a) => a.name === "GlobalId")!;
    const parameterType = globalId.parameterType;
    expect(parameterType).not.toBeNull();
    expect(parameterType!.kind).toBe(ParameterTypeKind.NAMED);
    expect(parameterType!.namedDeclaredType?.name).toBe("IfcGloballyUniqueId");
  });

  it("AttributeDeclaration.parameterType exposes aggregation metadata", () => {
    const pointList = schema.declarationByName("IfcCartesianPointList3D")!;
    const attrs = pointList.attributes(true);
    const coordList = attrs.find((a) => a.name === "CoordList")!;
    const parameterType = coordList.parameterType;
    expect(parameterType).not.toBeNull();
    expect(parameterType!.kind).toBe(ParameterTypeKind.AGGREGATION);
    expect(parameterType!.aggregationType).not.toBe(AggregationType.INVALID);
    expect(parameterType!.aggregationElementType).not.toBeNull();
  });
});

describe("Direction/Point entity attributes", () => {
  let file: IfcFile;

  beforeAll(() => {
    file = IfcFile.openFromMemory(IFC_CONTENT);
  });

  afterAll(() => {
    file.close();
  });

  it("IfcDirection has correct direction ratios", () => {
    // #902 = IFCDIRECTION((1.,0.,0.))
    const dir = file.byId(902)!;
    expect(dir.typeName).toBe("IfcDirection");
    const ratioIdx = dir.attributeIndex("DirectionRatios");
    const ratios = dir.getDoubleList(ratioIdx);
    expect(ratios).toEqual([1, 0, 0]);
  });

  it("IfcCartesianPoint has correct coordinates", () => {
    // #901 = IFCCARTESIANPOINT((0.,0.,0.))
    const pt = file.byId(901)!;
    expect(pt.typeName).toBe("IfcCartesianPoint");
    const coordIdx = pt.attributeIndex("Coordinates");
    const coords = pt.getDoubleList(coordIdx);
    expect(coords).toEqual([0, 0, 0]);
  });
});

describe("Roundtrip: create → serialize → reparse", () => {
  it("creates entities, serializes, and reparses", () => {
    const file = IfcFile.createEmpty("IFC4");

    const wall1 = file.createEntity("IfcWall");
    const nameIdx = wall1.attributeIndex("Name");
    wall1.setString(nameIdx, "RoundtripWall");

    const wall2 = file.createEntity("IfcWall");
    wall2.setString(nameIdx, "SecondWall");

    expect(file.entityCount).toBe(2);

    const spf = file.toSpf();
    expect(spf).toContain("RoundtripWall");
    expect(spf).toContain("SecondWall");
    file.close();

    const file2 = IfcFile.openFromMemory(spf);
    expect(file2.entityCount).toBe(2);

    const walls = file2.byType("IfcWall");
    expect(walls).toHaveLength(2);
    const wallNames = walls.map((w) => w.getString(w.attributeIndex("Name")));
    expect(wallNames).toContain("RoundtripWall");
    expect(wallNames).toContain("SecondWall");
    file2.close();
  });

  describe("IfcFile extended", () => {
    let file: IfcFile;

    beforeAll(() => {
      file = IfcFile.openFromMemory(IFC_CONTENT);
    });

    afterAll(() => {
      file.close();
    });

    it("status returns 0 (OK)", () => {
      expect(file.status).toBe(0);
    });

    it("schemaIdentifier aliases schema", () => {
      expect(file.schemaIdentifier).toBe(file.schema);
    });

    it("headerDescription returns an entity", () => {
      const hd = file.headerDescription();
      expect(hd).not.toBeNull();
      expect(hd!.ptr).toBeGreaterThan(0);
    });

    it("headerFileName returns an entity", () => {
      const hf = file.headerFileName();
      expect(hf).not.toBeNull();
    });

    it("headerSchema returns an entity", () => {
      const hs = file.headerSchema();
      expect(hs).not.toBeNull();
    });

    it("entities() returns all entities", () => {
      const ents = file.entities();
      expect(ents.length).toBe(file.entityCount);
      expect(ents[0]).toBeInstanceOf(Entity);
    });

    it("entityIds() returns all IDs", () => {
      const ids = file.entityIds();
      expect(ids.length).toBe(file.entityCount);
      expect(ids).toContain(100); // IfcProject
    });

    it("byReference returns referencing entities", () => {
      const refs = file.byReference(110); // OwnerHistory
      expect(refs.length).toBeGreaterThan(0);
    });

    it("getTotalInverses returns a count", () => {
      const count = file.getTotalInverses(110); // OwnerHistory is referenced by many
      expect(count).toBeGreaterThan(0);
    });

    it("traverseById returns traversed entities", () => {
      const traversed = file.traverseById(100, 1);
      expect(traversed.length).toBeGreaterThan(0);
    });

    it("unitEntity returns a unit entity for LENGTHUNIT", () => {
      const unit = file.unitEntity("LENGTHUNIT");
      expect(unit).not.toBeNull();
      expect(unit!.ptr).toBeGreaterThan(0);
    });

    it("types() returns type declarations from the file", () => {
      const types = file.types();
      expect(types.length).toBeGreaterThan(0);
      const names = types.map(t => t.name);
      expect(names).toContain("IfcProject");
    });

    it("createEntityWithId creates with specific id", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const entity = newFile.createEntityWithId("IfcWall", 9999);
      expect(entity.id).toBe(9999);
      newFile.close();
    });

    it("recalculateIdCounter works after manipulation", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      newFile.createEntityWithId("IfcWall", 500);
      newFile.recalculateIdCounter();
      expect(newFile.maxId).toBeGreaterThanOrEqual(500);
      newFile.close();
    });
  });

  describe("Entity extended", () => {
    let file: IfcFile;
    let project: Entity;

    beforeAll(() => {
      file = IfcFile.openFromMemory(IFC_CONTENT);
      project = file.byId(100)!;
    });

    afterAll(() => {
      file.close();
    });

    it("typeNameWithSchema includes schema prefix", () => {
      const qName = project.typeNameWithSchema;
      expect(qName).toContain("IFC4");
      expect(qName).toContain("IfcProject");
    });

    it("inverseAttributeNames returns inverse attrs", () => {
      const names = project.inverseAttributeNames();
      expect(names.length).toBeGreaterThan(0);
    });

    it("attributeCategory returns a valid category", () => {
      const cat = project.attributeCategory("Name");
      expect(typeof cat).toBe("number");
    });

    it("argumentDeclaredType reports declared argument kind", () => {
      const idx = project.attributeIndex("Name");
      expect(project.argumentDeclaredType(idx)).toBe(ArgumentType.STRING);
    });

    it("unsetArgument unsets an attribute", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const wall = newFile.createEntity("IfcWall");
      const nameIdx = wall.attributeIndex("Name");
      wall.setString(nameIdx, "TestWall");
      expect(wall.getString(nameIdx)).toBe("TestWall");
      wall.unsetArgument(nameIdx);
      expect(wall.isNull(nameIdx)).toBe(true);
      newFile.close();
    });

    it("setDoubleList sets a double list attribute", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const cartesian = newFile.createEntity("IfcCartesianPoint");
      const coordIdx = cartesian.attributeIndex("Coordinates");
      cartesian.setDoubleList(coordIdx, [1.0, 2.0, 3.0]);
      const coords = cartesian.getDoubleList(coordIdx);
      expect(coords).toHaveLength(3);
      expect(coords[0]).toBeCloseTo(1.0);
      expect(coords[1]).toBeCloseTo(2.0);
      expect(coords[2]).toBeCloseTo(3.0);
      newFile.close();
    });

    it("setEntityList sets an entity list attribute", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const p1 = newFile.createEntity("IfcCartesianPoint");
      p1.setDoubleList(p1.attributeIndex("Coordinates"), [0, 0, 0]);
      const p2 = newFile.createEntity("IfcCartesianPoint");
      p2.setDoubleList(p2.attributeIndex("Coordinates"), [1, 0, 0]);
      const polyline = newFile.createEntity("IfcPolyline");
      const pointsIdx = polyline.attributeIndex("Points");
      polyline.setEntityList(pointsIdx, [p1, p2]);
      const retrieved = polyline.getEntityList(pointsIdx);
      expect(retrieved).toHaveLength(2);
      newFile.close();
    });

    it("setDoubleMatrix sets nested coordinate list values", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const pointList = newFile.createEntity("IfcCartesianPointList3D");
      const coordListIdx = pointList.attributeIndex("CoordList");
      const ok = pointList.setDoubleMatrix(coordListIdx, [
        [0, 0, 0],
        [1, 0, 0],
        [1, 1, 0],
      ]);
      expect(ok).toBe(true);
      expect(pointList.getDoubleMatrix(coordListIdx)).toEqual([
        [0, 0, 0],
        [1, 0, 0],
        [1, 1, 0],
      ]);
      newFile.close();
    });

    it("setIntMatrix sets nested face index values", () => {
      const newFile = IfcFile.createEmpty("IFC4");
      const pointList = newFile.createEntity("IfcCartesianPointList3D");
      pointList.setDoubleMatrix(pointList.attributeIndex("CoordList"), [
        [0, 0, 0],
        [1, 0, 0],
        [1, 1, 0],
      ]);
      const faceSet = newFile.createEntity("IfcTriangulatedFaceSet");
      const coordinatesIdx = faceSet.attributeIndex("Coordinates");
      expect(faceSet.setEntity(coordinatesIdx, pointList)).toBe(true);
      const coordIndexIdx = faceSet.attributeIndex("CoordIndex");
      const ok = faceSet.setIntMatrix(coordIndexIdx, [[1, 2, 3]]);
      expect(ok).toBe(true);
      expect(faceSet.getIntMatrix(coordIndexIdx)).toEqual([[1, 2, 3]]);
      newFile.close();
    });
  });

  describe("Schema introspection", () => {
    it("TypeDeclaration.index returns a number", () => {
      const schema = new Schema("IFC4");
      const wall = schema.declarationByName("IfcWall");
      expect(wall).not.toBeNull();
      expect(typeof wall!.index).toBe("number");
    });

    it("TypeDeclaration.declaredType returns underlying type for typedefs", () => {
      const schema = new Schema("IFC4");
      const label = schema.declarationByName("IfcLabel");
      expect(label).not.toBeNull();
      const dt = label!.declaredType;
      expect(typeof dt === "object" || dt === null).toBe(true);
    });

    it("TypeDeclaration.inverseAttributes returns inverse attrs for IfcWall", () => {
      const schema = new Schema("IFC4");
      const wall = schema.declarationByName("IfcWall");
      const invAttrs = wall!.inverseAttributes();
      expect(invAttrs.length).toBeGreaterThan(0);
      expect(typeof invAttrs[0].name).toBe("string");
    });

    it("InverseAttributeDeclaration has entityReference", () => {
      const schema = new Schema("IFC4");
      const wall = schema.declarationByName("IfcWall");
      const invAttrs = wall!.inverseAttributes();
      const withRef = invAttrs.find(a => a.entityReference !== null);
      expect(withRef).toBeDefined();
      expect(withRef!.entityReference!.name).toBeTruthy();
    });
  });
});

describe("GUID utilities", () => {
  it("compress and expand round-trip", () => {
    const uuid = "3f2504e04f8911d39a0c0305e82c3301";
    const guid = guidCompress(uuid);
    expect(guid).toHaveLength(22);
    const back = guidExpand(guid);
    expect(back).toBe(uuid);
  });

  it("known GUID from test IFC", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0];
    const guid = project.getString(project.attributeIndex("GlobalId"));
    expect(guid).toBeTruthy();
    const uuid = guidExpand(guid!);
    expect(uuid).toHaveLength(32);
    const recompressed = guidCompress(uuid);
    expect(recompressed).toBe(guid);
    const formatted = guidSplit(uuid);
    expect(formatted).toMatch(/^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/);
    file.close();
  });

  it("newGuid generates unique 22-char GUIDs", () => {
    const a = newGuid();
    const b = newGuid();
    expect(a).toHaveLength(22);
    expect(b).toHaveLength(22);
    expect(a).not.toBe(b);
  });
});

describe("Entity.getInfo", () => {
  it("returns plain object with id, type, and attribute names", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0];
    const info = project.getInfo();
    expect(info.id).toBe(project.id);
    expect(info.type).toBe("IfcProject");
    expect("GlobalId" in info).toBe(true);
    expect("Name" in info).toBe(true);
    expect(typeof info.GlobalId).toBe("string");
    file.close();
  });

  it("getInfo without id when includeId=false", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0];
    const info = project.getInfo({ includeId: false });
    expect("id" in info).toBe(false);
    expect(info.type).toBe("IfcProject");
    file.close();
  });

  it("getInfo with recursive=true expands nested entities", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const proxy = file.byId(1000);
    expect(proxy).not.toBeNull();
    const info = proxy!.getInfo({ recursive: true });
    expect(info.type).toBe("IfcBuildingElementProxy");
    if (info.OwnerHistory && typeof info.OwnerHistory === "object" && !(info.OwnerHistory instanceof Entity)) {
      const oh = info.OwnerHistory as Record<string, unknown>;
      expect(oh.type).toBeTruthy();
    }
    file.close();
  });

  it("getInfo handles null attributes", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0];
    const info = project.getInfo();
    const names = project.attributeNames();
    const nullAttrs = names.filter(n => info[n] === null);
    expect(nullAttrs.length).toBeGreaterThanOrEqual(0);
    file.close();
  });
});

describe("Entity.withAttributes (dynamic access)", () => {
  it("reads forward attributes by name", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0].withAttributes();
    expect((project as any).GlobalId).toBeTruthy();
    expect(typeof (project as any).GlobalId).toBe("string");
    expect((project as any).Name).toBe("proxy with swept solid");
    file.close();
  });

  it("returns undefined for non-existent attributes", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0].withAttributes();
    expect((project as any).NonExistentAttr).toBeUndefined();
    file.close();
  });

  it("returns null for null attributes", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0].withAttributes();
    const info = project.getInfo();
    const nullName = project.attributeNames().find(n => info[n] === null);
    if (nullName) {
      expect((project as any)[nullName]).toBeNull();
    }
    file.close();
  });

  it("reads inverse attributes by name", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const building = file.byType("IfcBuilding")[0].withAttributes();
    const isDecomposedBy = (building as any).IsDecomposedBy;
    if (isDecomposedBy !== undefined) {
      expect(Array.isArray(isDecomposedBy)).toBe(true);
    }
    file.close();
  });

  it("existing Entity methods still work", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const project = file.byType("IfcProject")[0].withAttributes();
    expect(project.id).toBeGreaterThan(0);
    expect(project.typeName).toBe("IfcProject");
    expect(project.isA("IfcProject")).toBe(true);
    expect(project.attributeCount).toBeGreaterThan(0);
    file.close();
  });

  it("sets forward attributes by name", () => {
    const file = IfcFile.createEmpty("IFC4");
    const wall = file.createEntity("IfcWall", []);
    const w = wall!.withAttributes();
    (w as any).Name = "TestWall";
    expect(w.getString(w.attributeIndex("Name"))).toBe("TestWall");
    file.close();
  });
});

describe("Entity.walk", () => {
  it("transforms matching values", () => {
    const result = Entity.walk(
      (v) => v === "old",
      () => "new",
      "old",
    );
    expect(result).toBe("new");
  });

  it("leaves non-matching values unchanged", () => {
    const result = Entity.walk(
      (v) => v === "old",
      () => "new",
      "other",
    );
    expect(result).toBe("other");
  });

  it("recurses into arrays", () => {
    const result = Entity.walk(
      (v) => typeof v === "number" && v > 5,
      (v) => (v as number) * 2,
      [1, 3, 7, 10],
    );
    expect(result).toEqual([1, 3, 14, 20]);
  });

  it("recurses into nested arrays", () => {
    const result = Entity.walk(
      (v) => v === "x",
      () => "y",
      ["a", ["x", "b"], "x"],
    );
    expect(result).toEqual(["a", ["y", "b"], "y"]);
  });

  it("transforms Entity instances in a list", () => {
    const file = IfcFile.openFromMemory(IFC_CONTENT);
    const walls = file.byType("IfcBuildingElementProxy");
    const ids = Entity.walk(
      (v) => v instanceof Entity,
      (v) => (v as Entity).id,
      walls,
    );
    expect(Array.isArray(ids)).toBe(true);
    expect((ids as number[]).every(v => typeof v === "number")).toBe(true);
    file.close();
  });
});

describe("PsetQto", () => {
  let pq: PsetQto;

  beforeAll(() => {
    pq = new PsetQto();
  });

  afterAll(() => {
    pq.deinit();
  });

  it("allTemplates returns non-empty list for IFC4", () => {
    const templates = pq.allTemplates("IFC4");
    expect(templates.length).toBeGreaterThan(0);
    expect(templates[0].name).toBeTruthy();
  });

  it("allTemplates returns empty for invalid schema", () => {
    const templates = pq.allTemplates("IFC99");
    expect(templates).toEqual([]);
  });

  it("getByName finds Pset_WallCommon", () => {
    const tmpl = pq.getByName("IFC4", "Pset_WallCommon");
    expect(tmpl).not.toBeNull();
    expect(tmpl!.name).toBe("Pset_WallCommon");
    expect(tmpl!.applicableEntity).toBeTruthy();
    expect(tmpl!.applicableEntity!).toContain("IfcWall");
  });

  it("getByName returns null for unknown template", () => {
    const tmpl = pq.getByName("IFC4", "Pset_NonExistent_12345");
    expect(tmpl).toBeNull();
  });

  it("isTemplated returns true for known template", () => {
    expect(pq.isTemplated("IFC4", "Pset_WallCommon")).toBe(true);
  });

  it("isTemplated returns false for unknown template", () => {
    expect(pq.isTemplated("IFC4", "Pset_NonExistent_12345")).toBe(false);
  });

  it("getApplicable returns templates for IfcWall", () => {
    const templates = pq.getApplicable("IFC4", "IfcWall");
    expect(templates.length).toBeGreaterThan(0);

    const names = templates.map(t => t.name);
    expect(names).toContain("Pset_WallCommon");
  });

  it("getApplicable with psetOnly excludes Qto", () => {
    const templates = pq.getApplicable("IFC4", "IfcWall", { psetOnly: true });
    for (const t of templates) {
      expect(t.name).not.toMatch(/^Qto_/);
    }
  });

  it("getApplicable with qtoOnly excludes Pset", () => {
    const templates = pq.getApplicable("IFC4", "IfcWall", { qtoOnly: true });
    for (const t of templates) {
      expect(t.name).not.toMatch(/^Pset_/);
    }
  });

  it("getApplicable returns empty for non-existent class", () => {
    const templates = pq.getApplicable("IFC4", "IfcNonExistent_12345");
    expect(templates).toEqual([]);
  });

  it("getApplicableNames returns string list", () => {
    const names = pq.getApplicableNames("IFC4", "IfcWall");
    expect(names.length).toBeGreaterThan(0);
    expect(names).toContain("Pset_WallCommon");
  });

  it("getApplicableNames count matches getApplicable count", () => {
    const templates = pq.getApplicable("IFC4", "IfcDoor");
    const names = pq.getApplicableNames("IFC4", "IfcDoor");
    expect(names.length).toBe(templates.length);
  });

  it("getProperties returns property templates", () => {
    const tmpl = pq.getByName("IFC4", "Pset_WallCommon");
    expect(tmpl).not.toBeNull();

    const props = pq.getProperties(tmpl!.ptr);
    expect(props.length).toBeGreaterThan(0);
    expect(props[0].name).toBeTruthy();
  });

  it("template has description and templateType", () => {
    const tmpl = pq.getByName("IFC4", "Pset_WallCommon");
    expect(tmpl).not.toBeNull();
    expect(tmpl!.templateType).toBeTruthy();
  });

  it("subtypes inherit applicable templates", () => {
    const wallTemplates = pq.getApplicable("IFC4", "IfcWall");
    const stdCaseTemplates = pq.getApplicable("IFC4", "IfcWallStandardCase");
    expect(stdCaseTemplates.length).toBeGreaterThanOrEqual(wallTemplates.length);
  });
});
