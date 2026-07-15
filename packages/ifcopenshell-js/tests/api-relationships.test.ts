
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated relationship API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  describe('aggregate', () => {
    it('assigns and unassigns product aggregation', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rel = await shell.api.aggregate.assignObject(file, {
        products: [wall],
        relatingObject: site,
      });
      expect(rel.type).toBe('IfcRelAggregates');

      const parent = await shell.api.element.getAggregate(wall);
      expect(parent?.id).toBe(site.id);

      shell.api.aggregate.unassignObject(file, { products: [wall] });
      const after = await shell.api.element.getAggregate(wall);
      expect(after).toBeNull();
    });
  });

  describe('classification', () => {
    it('creates classifications and references', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const classification = await shell.api.classification.addClassification(file, 'Uniclass');
      expect(classification).not.toBeNull();
      expect(classification.type).toBe('IfcClassification');

      const ref = await shell.api.classification.addReference(file, {
        products: [wall],
        identification: 'AB12',
        name: 'Item name',
      });
      expect(ref.type).toBe('IfcClassificationReference');

      const refs = await shell.api.classification.getReferences(wall, false);
      expect(refs.length).toBeGreaterThanOrEqual(1);

      shell.api.classification.removeReference(file, { reference: ref, products: [wall] });
      const after = await shell.api.classification.getReferences(wall, false);
      expect(after.length).toBe(0);

      shell.api.classification.removeClassification(file, classification);
    });
  });

  describe('constraint', () => {
    it('manages objectives and metrics', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });

      const objective = await shell.api.constraint.addObjective(file);
      expect(objective.type).toBe('IfcObjective');

      const metric = await shell.api.constraint.addMetric(file, objective);
      expect(metric.type).toBe('IfcMetric');

      const refs = await shell.api.constraint.addMetricReference(
        file, metric, 'Reference1',
      );
      expect(refs.length).toBeGreaterThanOrEqual(1);

      shell.api.constraint.removeMetric(file, metric);
      shell.api.constraint.removeConstraint(file, objective);
    });

    it('assigns constraints to products', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const objective = await shell.api.constraint.addObjective(file);

      const rel = await shell.api.constraint.assignConstraint(file, {
        products: [wall],
        constraint: objective,
      });
      expect(rel.type).toBeDefined();

      shell.api.constraint.unassignConstraint(file, {
        products: [wall],
        constraint: objective,
      });

      shell.api.constraint.removeConstraint(file, objective);
    });
  });

  describe('control', () => {
    it('assigns controls to elements', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const approval = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcApproval',
        name: 'Approval',
      });

      const rel = await shell.api.control.assignControl(file, {
        relatingControl: approval,
        relatedObjects: [wall],
      });
      expect(rel.type).toBeDefined();

      const controls = await shell.api.element.getControls(wall);
      expect(controls.length).toBeGreaterThanOrEqual(1);

      shell.api.control.unassignControl(file, {
        relatingControl: approval,
        relatedObjects: [wall],
      });
    });
  });

  describe('document', () => {
    it('manages document information and assignments', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const info = await shell.api.document.addInformation(file, {});
      expect(info.type).toBe('IfcDocumentInformation');

      const ref = await shell.api.document.addReference(file, info);
      expect(ref.type).toBe('IfcDocumentReference');

      const rel = await shell.api.document.assignDocument(file, {
        products: [wall],
        document: ref,
      });
      expect(rel.type).toBeDefined();

      shell.api.document.unassignDocument(file, {
        products: [wall],
        document: ref,
      });

      shell.api.document.removeReference(file, ref);
      shell.api.document.removeInformation(file, info);
    });
  });

  describe('group', () => {
    it('manages groups and product membership', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const group = await shell.api.group.addGroup(file, { name: 'Test Group' });
      expect(group.type).toBe('IfcGroup');

      const rel = await shell.api.group.assignGroup(file, {
        products: [wall],
        group,
      });
      expect(rel.type).toBeDefined();

      const groups = await shell.api.element.getGroups(wall);
      expect(groups.length).toBeGreaterThanOrEqual(1);

      shell.api.group.unassignGroup(file, { products: [wall], group });
      const after = await shell.api.element.getGroups(wall);
      expect(after.length).toBe(0);

      shell.api.group.removeGroup(file, group);
    });
  });

  describe('library', () => {
    it('manages library references', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const library = await shell.api.library.addLibrary(file, 'Test Library');
      expect(library.type).toBe('IfcLibraryInformation');

      const ref = await shell.api.library.addReference(file, library);
      expect(ref.type).toBe('IfcLibraryReference');

      const rel = await shell.api.library.assignReference(file, {
        products: [wall],
        reference: ref,
      });
      expect(rel.type).toBeDefined();

      shell.api.library.unassignReference(file, {
        products: [wall],
        reference: ref,
      });

      shell.api.library.removeReference(file, ref);
      shell.api.library.removeLibrary(file, library);
    });
  });

  describe('nest', () => {
    it('assigns and unassigns nested objects', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const host = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Host Wall',
      });
      const part = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcBeam',
        name: 'Nested Beam',
      });

      const rel = await shell.api.nest.assignObject(file, {
        products: [part],
        relatingObject: host,
      });
      expect(rel.type).toBe('IfcRelNests');

      const nest = await shell.api.element.getNest(part);
      expect(nest?.id).toBe(host.id);

      shell.api.nest.unassignObject(file, { products: [part] });
      const after = await shell.api.element.getNest(part);
      expect(after).toBeNull();
    });

    it('changes parents and reorders with optional and explicit indexes', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const firstParent = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcElementAssembly',
        name: 'First parent',
      });
      const secondParent = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcElementAssembly',
        name: 'Second parent',
      });
      const children = await Promise.all(['A', 'B', 'C'].map((name) =>
        shell.api.root.createEntity(file, { ifcClass: 'IfcBuildingElementProxy', name })));
      const relation = await shell.api.nest.assignObject(file, {
        products: children,
        relatingObject: firstParent,
      });

      shell.api.nest.reorderNesting(file, { item: children[2] });
      expect((relation.get('RelatedObjects') as { id: number }[]).map((item) => item.id))
        .toEqual([children[2].id, children[0].id, children[1].id]);
      shell.api.nest.reorderNesting(file, { item: children[2], oldIndex: -1, newIndex: -99 });
      expect((relation.get('RelatedObjects') as { id: number }[]).map((item) => item.id))
        .toEqual([children[1].id, children[2].id, children[0].id]);

      const before = (relation.get('RelatedObjects') as { id: number }[]).map((item) => item.id);
      expect(() => shell.api.nest.reorderNesting(file, {
        item: children[0],
        oldIndex: 99,
        newIndex: 0,
      })).toThrow();
      expect((relation.get('RelatedObjects') as { id: number }[]).map((item) => item.id)).toEqual(before);

      shell.api.nest.changeNest(file, { item: children[2], newParent: secondParent });
      expect(shell.api.element.getNest(children[2])?.id).toBe(secondParent.id);
      expect((relation.get('RelatedObjects') as { id: number }[]).map((item) => item.id))
        .toEqual([children[1].id, children[0].id]);
    });
  });

  describe('project', () => {
    it('assigns declarations to context', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const project = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const context = await shell.api.context.addContext(file, {
        contextType: 'Model',
        contextIdentifier: 'Body',
        targetView: 'MODEL_VIEW',
      });

      const rel = await shell.api.project.assignDeclaration(file, {
        definitions: [project],
        relatingContext: context,
      });
      expect(rel.type).toBe('IfcRelDeclares');

      shell.api.project.unassignDeclaration(file, {
        definitions: [project],
        relatingContext: context,
      });
    });
  });

  describe('spatial', () => {
    it('assigns and unassigns spatial containers', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rel = await shell.api.spatial.assignContainer(file, {
        products: [wall],
        relatingStructure: site,
      });
      expect(rel.type).toBe('IfcRelContainedInSpatialStructure');

      const container = await shell.api.element.getContainer(wall, {});
      expect(container?.id).toBe(site.id);

      shell.api.spatial.unassignContainer(file, { products: [wall] });
      const after = await shell.api.element.getContainer(wall, {});
      expect(after).toBeNull();
    });

    it('references and dereferences spatial structures', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site A',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rel = await shell.api.spatial.referenceStructure(file, {
        products: [wall],
        relatingStructure: site,
      });
      expect(rel.type).toBe('IfcRelReferencedInSpatialStructure');

      shell.api.spatial.dereferenceStructure(file, {
        products: [wall],
        relatingStructure: site,
      });
    });
  });

  describe('system', () => {
    it('manages systems and port connections', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const duct = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcDuctSegment',
        name: 'Duct',
      });

      const sys = await shell.api.system.addSystem(file, {
        ifcClass: 'IfcDistributionSystem',
      });
      expect(sys.type).toBe('IfcDistributionSystem');

      await shell.api.system.assignSystem(file, {
        products: [duct],
        system: sys,
      });

      const port1 = await shell.api.system.addPort(file, { element: duct });
      expect(port1.type).toBe('IfcDistributionPort');

      const port2 = await shell.api.system.addPort(file, {});
      expect(port2.type).toBe('IfcDistributionPort');

      shell.api.system.connectPort(file, {
        port1,
        port2,
        direction: 'SOURCE',
      });

      shell.api.system.disconnectPort(file, port1);
      shell.api.system.disconnectPort(file, port2);

      shell.api.system.unassignSystem(file, {
        products: [duct],
        system: sys,
      });

      shell.api.system.removeSystem(file, sys);
    });
  });

  describe('type', () => {
    it('assigns and unassigns element types', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const wallType = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWallType',
        name: 'Wall Type',
      });

      const rel = await shell.api.type.assignType(file, {
        objects: [wall],
        relatingType: wallType,
      });
      expect(rel.type).toBe('IfcRelDefinesByType');

      const type = await shell.api.element.getType(wall);
      expect(type?.id).toBe(wallType.id);

      shell.api.type.unassignType(file, { objects: [wall] });
      const after = await shell.api.element.getType(wall);
      expect(after).toBeNull();
    });
  });
});
