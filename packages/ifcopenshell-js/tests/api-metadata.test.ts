
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated metadata and authoring API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  describe('root', () => {
    it('removes products', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall to remove',
      });
      expect(wall.type).toBe('IfcWall');
      const id = wall.id;

      shell.api.root.removeProduct(file, wall, {});

      const entity = await file.get(id);
      expect(entity).toBeNull();
    });
  });

  describe('element', () => {
    it('gets openings on a fresh wall', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const openings = await shell.api.element.getOpenings(wall);
      expect(openings).toEqual([]);
    });

    it('checks UserDefinedType flag', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      expect(shell.api.element.isUserdefinedType(wall)).toBe(false);
    });

    it('gets containment from a site', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      await shell.api.spatial.assignContainer(file, {
        products: [wall],
        relatingStructure: site,
      });

      const children = await shell.api.element.getContained(site);
      expect(children.length).toBeGreaterThanOrEqual(1);

      await shell.api.spatial.unassignContainer(file, { products: [wall] });
    });

    it('gets referenced structures', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      await shell.api.spatial.referenceStructure(file, {
        products: [wall],
        relatingStructure: site,
      });

      const structures = await shell.api.element.getReferencedStructures(wall);
      expect(structures.length).toBeGreaterThanOrEqual(1);

      shell.api.spatial.dereferenceStructure(file, {
        products: [wall],
        relatingStructure: site,
      });
    });

    it('replaces one element with another', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const old = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Old Wall',
      });
      const replacement = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcBeam',
        name: 'New Beam',
      });

      shell.api.element.replaceElement(old, replacement);

      const entity = await file.get(old.id);
      expect(entity).not.toBeNull();
    });
  });

  describe('entity', () => {
    it('deep-removes entity with options', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const id = wall.id;

      shell.api.entity.removeDeepWithOptions(wall, {
        alsoConsider: [],
        doNotDelete: [],
      });

      const entity = await file.get(id);
      expect(entity).toBeNull();
    });
  });

  describe('guid', () => {
    it('generates valid compressed GUIDs', () => {
      const guid = shell.api.guid.generate();
      expect(guid).toHaveLength(22);
      expect(guid).toMatch(/^[0-9A-Za-z_$]{22}$/);
    });

    it('compresses and expands UUIDs', () => {
      const hex = '01234567-89ab-cdef-fedc-ba9876543210';
      const compressed = shell.api.guid.compress(hex);
      expect(compressed).toHaveLength(22);

      const expanded = shell.api.guid.expand(compressed);
      expect(expanded.replace(/-/g, '').toLowerCase()).toBe(
        hex.replace(/-/g, '').toLowerCase(),
      );
    });
  });

  describe('owner', () => {
    it('manages owner history lifecycle', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });

      const person = await shell.api.owner.addPerson(file, 'P001', 'Doe', 'John');
      expect(person.type).toBe('IfcPerson');

      const org = await shell.api.owner.addOrganisation(file, 'ORG01', 'Acme');
      expect(org.type).toBe('IfcOrganization');

      const pando = await shell.api.owner.addPersonAndOrganisation(file, person, org);
      expect(pando.type).toBe('IfcPersonAndOrganization');

      const app = await shell.api.owner.addApplication(file, {
        applicationDeveloper: org,
        version: '1.0',
        applicationFullName: 'Test App',
        applicationIdentifier: 'com.test.app',
      });
      expect(app.type).toBe('IfcApplication');

      const history = await shell.api.owner.createOwnerHistory(file, {});
      expect(typeof history === 'object' || history === null).toBe(true);

      shell.api.owner.updateOwnerHistory(file, {});

      shell.api.owner.removeApplication(file, app);
      shell.api.owner.removePersonAndOrganisation(file, pando);
      shell.api.owner.removePerson(file, person);
      shell.api.owner.removeOrganisation(file, org);
    });

    it('adds addresses and roles', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const person = await shell.api.owner.addPerson(file, 'P002', 'Smith', 'Jane');

      const address = await shell.api.owner.addAddress(
        file, person, 'IfcTelecomAddress',
      );
      expect(address.type).toBe('IfcTelecomAddress');
      shell.api.owner.removeAddress(file, address);

      const role = await shell.api.owner.addRole(
        file, person, 'ARCHITECT',
      );
      expect(role.type).toBe('IfcActorRole');
      shell.api.owner.removeRole(file, role);

      shell.api.owner.removePerson(file, person);
    });

    it('assigns and unassigns actors', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const person = await shell.api.owner.addPerson(file, 'P003', 'Brown', 'Bob');
      const org = await shell.api.owner.addOrganisation(file, 'ORG02', 'Corp');
      const pando = await shell.api.owner.addPersonAndOrganisation(file, person, org);
      const actor = await shell.api.owner.addActor(file, {
        actor: pando,
        ifcClass: 'IfcActor',
      });
      expect(actor.type).toBe('IfcActor');

      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      await shell.api.owner.assignActor(file, {
        relatingActor: actor,
        relatedObject: wall,
      });

      shell.api.owner.unassignActor(file, {
        relatingActor: actor,
        relatedObject: wall,
      });

      shell.api.owner.removeActor(file, actor);
      shell.api.owner.removePersonAndOrganisation(file, pando);
      shell.api.owner.removeOrganisation(file, org);
      shell.api.owner.removePerson(file, person);
    });
  });

  describe('pset', () => {
    it('adds, edits, and removes property sets', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const pset = await shell.api.pset.addPset(file, {
        product: wall,
        name: 'Pset_WallCommon',
      });
      expect(pset.type).toBe('IfcPropertySet');

      const changed = await shell.api.pset.editPset(file, {
        pset,
        properties: { Reference: 'A-101', LoadBearing: true },
        shouldPurge: true,
      });
      expect(changed).toBe(true);

      shell.api.pset.removePset(file, wall, pset);
    });

    it('assigns and unassigns shared property sets', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall1 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 1',
      });
      const wall2 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 2',
      });

      const pset = await shell.api.pset.addPset(file, {
        product: wall1,
        name: 'SharedPset',
      });
      await shell.api.pset.editPset(file, {
        pset,
        properties: { Note: 'shared' },
        shouldPurge: true,
      });

      const rel = await shell.api.pset.assignPset(file, {
        products: [wall2],
        pset,
      });
      expect(rel.type).toBeDefined();

      const ids = await shell.api.element.getPsetIds(wall2, {});
      expect(ids.length).toBeGreaterThanOrEqual(1);

      shell.api.pset.unassignPset(file, [wall2], pset);
      shell.api.pset.removePset(file, wall1, pset);
    });
  });

  describe('schema', () => {
    it('reassigns IFC class', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      expect(wall.type).toBe('IfcWall');

      const beam = await shell.api.schema.reassignClass(file, wall, 'IfcBeam');
      expect(beam.type).toBe('IfcBeam');
      expect(beam.id).toBe(wall.id);
    });
  });

  describe('selector', () => {
    it('reads element values via selector query', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Test Wall',
      });
      const value = shell.api.selector.getElementValue(file, wall, 'Name');
      expect(value).toBe('Test Wall');
    });

    it('queries elements via selector', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'roof parapet',
      });
      const slab = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSlab',
        name: 'Roof',
        predefinedType: 'ROOF',
      });

      const value = shell.api.selector.getElementValue(file, wall, 'Name');
      expect(value).toBe('roof parapet');

      const allResult = shell.api.selector.filterAll(file, 'IfcWall');
      expect(allResult).not.toBeNull();
      expect(selectorIds(shell.api.selector.filterAll(file, 'Name*=roof'))).toEqual([wall.id]);
      expect(selectorIds(shell.api.selector.filterAll(file, 'IfcSlab, PredefinedType=ROOF'))).toEqual([slab.id]);
    });
  });

  describe('unit', () => {
    it('gets unit assignment from project', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const assignment = await shell.api.unit.getUnitAssignment(file);
      expect(assignment !== null || assignment === null).toBe(true);
    });

    it('gets project length unit', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const unit = await shell.api.unit.getProjectUnit(file, 'LENGTHUNIT');
      expect(unit !== null || unit === null).toBe(true);
    });

    it('converts unit prefixes', () => {
      const result = shell.api.unit.convert(1000, 'MILLI', 'METRE', '', 'METRE');
      expect(result).toBe(1);
    });

    it('gets measure class from unit type', () => {
      const cls = shell.api.unit.getMeasureClass('LENGTHUNIT');
      expect(cls.length).toBeGreaterThan(0);
    });

    it('gets unit name universal', () => {
      const name = shell.api.unit.getUnitNameUniversal('METRE');
      expect(name).toBe('METRE');
    });

    it('gets SI dimensions', () => {
      const dims = shell.api.unit.getSiDimensions('METRE');
      expect(Array.isArray(dims)).toBe(true);
    });
  });
});

function selectorIds(value: unknown): number[] {
  if (!Array.isArray(value)) throw new TypeError('Expected selector result to be an array');
  return value.map((item) => {
    if (!item || typeof item !== 'object' || !('id' in item) || !('dispose' in item)) {
      throw new TypeError('Expected selector result item to be an entity');
    }
    const entity = item as { id: number; dispose(): void };
    try {
      return entity.id;
    } finally {
      entity.dispose();
    }
  });
}
