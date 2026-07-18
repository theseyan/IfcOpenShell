import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated native resource API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  async function newFile(): Promise<IfcFile> {
    const file = await IfcFile.createEmpty(shell, 'IFC4');
    await shell.api.root.createEntity(file, { ifcClass: 'IfcProject', name: 'Project' });
    return file;
  }

  function ids(entities: Entity[]): number[] {
    return entities.map((entity) => entity.id);
  }

  it('creates top-level and nested resources with semantic defaults', async () => {
    await using file = await newFile();
    const crew = shell.api.resource.addResource(file, {});
    const labor = shell.api.resource.addResource(file, {
      parentResource: crew,
      ifcClass: 'IfcLaborResource',
      name: '',
    });

    expect(crew.type).toBe('IfcCrewResource');
    expect(crew.get('Name')).toBe('Unnamed');
    expect(crew.get('PredefinedType')).toBe('NOTDEFINED');
    expect(shell.api.element.getNest(labor)?.id).toBe(crew.id);
    expect(file.all('IfcRelDeclares')).toHaveLength(1);
  });

  it('validates and replaces compatible quantities', async () => {
    await using file = await newFile();
    const crew = shell.api.resource.addResource(file, {});
    expect(() => shell.api.resource.addResourceQuantity(file, crew, 'IfcQuantityCount')).toThrow();
    expect(file.all('IfcPhysicalQuantity')).toHaveLength(0);

    const first = shell.api.resource.addResourceQuantity(file, crew, 'IfcQuantityTime');
    const second = shell.api.resource.addResourceQuantity(file, crew, 'IfcQuantityTime');
    expect(first.type).toBe('IfcQuantityTime');
    expect(second.id).not.toBe(first.id);
    expect((crew.get('BaseQuantity') as Entity).id).toBe(second.id);
    expect(file.get(first.id)).toBeNull();
  });

  it('keeps resource assignments idempotent and ordered, then cleans deletion', async () => {
    await using file = await newFile();
    const resource = shell.api.resource.addResource(file, { ifcClass: 'IfcLaborResource' });
    const first = shell.api.root.createEntity(file, { ifcClass: 'IfcWall', name: 'First' });
    const second = shell.api.root.createEntity(file, { ifcClass: 'IfcWall', name: 'Second' });

    const relation = shell.api.resource.assignResource(file, {
      relatingResource: resource,
      relatedObject: first,
    });
    const duplicate = shell.api.resource.assignResource(file, {
      relatingResource: resource,
      relatedObject: first,
    });
    shell.api.resource.assignResource(file, { relatingResource: resource, relatedObject: second });

    expect(duplicate.id).toBe(relation.id);
    expect(ids(relation.get('RelatedObjects') as Entity[])).toEqual([first.id, second.id]);
    shell.api.resource.unassignResource(file, { relatingResource: resource, relatedObject: first });
    expect(ids(relation.get('RelatedObjects') as Entity[])).toEqual([second.id]);

    const resourceId = resource.id;
    shell.api.resource.removeResource(file, { resource });
    expect(file.get(resourceId)).toBeNull();
    expect(file.all('IfcRelAssignsToResource')).toHaveLength(0);
    expect(file.all('IfcRelDeclares')).toHaveLength(0);
  });

  it('calculates work and usage through staged native APIs', async () => {
    await using file = await newFile();
    const resource = shell.api.resource.addResource(file, { ifcClass: 'IfcLaborResource' });
    const productivity = shell.api.pset.addPset(file, { product: resource, name: 'EPset_Productivity' });
    expect(shell.api.pset.editPset(file, {
      pset: productivity,
      properties: {
        BaseQuantityConsumed: 'PT1H',
        BaseQuantityProducedName: 'Count',
        BaseQuantityProducedValue: 2,
      },
    })).toBe(true);

    const task = shell.api.sequence.addTask(file, {});
    const taskTime = shell.api.sequence.addTaskTime(file, task, {});
    taskTime.set('ScheduleDuration', 'P1D');
    const first = shell.api.root.createEntity(file, { ifcClass: 'IfcWall' });
    const second = shell.api.root.createEntity(file, { ifcClass: 'IfcWall' });
    shell.api.sequence.assignProduct(file, first, task, {});
    shell.api.sequence.assignProduct(file, second, task, {});
    shell.api.sequence.assignProcess(file, task, resource, {});

    shell.api.resource.calculateResourceWork(file, resource);
    const usage = resource.get('Usage') as Entity;
    expect(usage.get('ScheduleWork')).toBe('PT1.0H');
    shell.api.resource.calculateResourceUsage(file, resource);
    expect(usage.get('ScheduleUsage')).toBeCloseTo(0.125);
  });
});
