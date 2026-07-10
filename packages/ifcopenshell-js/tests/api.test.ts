
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type IfcOpenShell } from '../src/index.js';
import { fromRawValue, toRawValue, type DisposableHandle } from '../src/api.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('direct api modules', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  it('exposes generated high-level api modules', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');

    const project = await shell.api.root.createEntity(
      file,
      { ifcClass: 'IfcProject', name: 'Project' },
    );

    expect(project.type).toBe('IfcProject');
    const entity = await file.get(1);
    expect(entity?.type).toBe('IfcProject');
    expect(await entity?.get('Name')).toBe('Project');
  });

  it('wraps generated instance lists', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const project = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcProject',
      name: 'Project',
    });

    const children = await shell.api.element.getParts(project);
    expect(children).toEqual([]);
  });

  it('accepts plain entity arrays for generated instance-list inputs', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const site = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcSite',
      name: 'Site',
    });

    expect(() => shell.api.aggregate.unassignObject(file, {
      products: [site],
    })).not.toThrow();
  });

  it('does not expose value module in public api', () => {
    expect((shell.api as Record<string, unknown>).value).toBeUndefined();
  });

  it('returns plain JS values from generated selector APIs', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcWall',
      name: 'Original',
    });

    expect(shell.api.selector.getElementValue(file, wall, 'Name')).toBe('Original');
  });

  it('sets selector values from query strings', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcWall',
      name: 'Original',
    });

    shell.api.selector.setElementValue(file, wall, 'Name', 'Updated', ', ');
    expect(shell.api.selector.getElementValue(file, wall, 'Name')).toBe('Updated');
  });

  it('encodes and decodes generated dynamic value data', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const project = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcProject',
      name: 'Project',
    });
    const temps: DisposableHandle[] = [];

    try {
      const raw = toRawValue(shell, [null, true, 42, 7n, 'text', project], temps);
      const data = fromRawValue(shell, raw);
      expect(Array.isArray(data)).toBe(true);
      const items = data as unknown[];
      expect(items.slice(0, 5)).toEqual([null, true, 42, 7, 'text']);
      expect(items[5]).toMatchObject({ id: project.id, type: project.type });
    } finally {
      for (let index = temps.length - 1; index >= 0; index -= 1) temps[index]?.destroy();
    }
  });

  it('encodes and decodes generated dynamic object values', async () => {
    const temps: DisposableHandle[] = [];

    try {
      const raw = toRawValue(shell, {
        name: 'Wall',
        meta: { reviewed: true, score: 4.5 },
        tags: ['A', null, 2n],
      }, temps);

      expect(fromRawValue(shell, raw)).toEqual({
        name: 'Wall',
        meta: { reviewed: true, score: 4.5 },
        tags: ['A', null, 2],
      });
    } finally {
      for (let index = temps.length - 1; index >= 0; index -= 1) temps[index]?.destroy();
    }
  });

  it('returns null for nullable element queries on bare wall', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcWall',
      name: 'Bare wall',
    });

    expect(shell.api.element.getType(wall)).toBeNull();
    expect(shell.api.element.getAggregate(wall)).toBeNull();
    expect(shell.api.element.getNest(wall)).toBeNull();
    expect(shell.api.element.getContainer(wall, {})).toBeNull();
    expect(shell.api.element.getParent(wall)).toBeNull();
    expect(shell.api.element.getMaterial(wall, {})).toBeNull();
    expect(shell.api.element.getFilledVoid(wall)).toBeNull();
    expect(shell.api.element.getVoidedElement(wall)).toBeNull();
  });

  it('accepts plain JS objects for pset property bag fields', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = await shell.api.root.createEntity(file, {
      ifcClass: 'IfcWall',
      name: 'Original',
    });

    shell.api.attribute.editAttributes(file, {
      product: wall,
      attributes: { Name: 'Updated' },
      syncPredefinedType: false,
      updateOwnerHistory: false,
    });

    expect(await wall.get('Name')).toBe('Updated');
  });

  it('adds SI units and resolves full unit names', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const metre = shell.api.unit.addSiUnit(file, 'LENGTHUNIT', null);
    expect(metre.type).toBe('IfcSIUnit');
    expect(shell.api.unit.getFullUnitName(metre)).toBe('METRE');
  });

  it('converts between unit prefixes', () => {
    expect(shell.api.unit.convert(1000, 'MILLI', 'METRE', '', 'METRE')).toBe(1);
  });

  it('creates rotation matrices', () => {
    const matrix = shell.api.placement.rotation(Math.PI / 2, 'Z');
    expect(matrix).toHaveLength(16);
    expect(Math.round(matrix[0])).toBe(0);
    expect(Math.round(matrix[1])).toBe(-1);
    expect(Math.round(matrix[4])).toBe(1);
    expect(Math.round(matrix[5])).toBe(0);
  });

  it('compares floating-point values with tolerance', () => {
    expect(shell.api.shape.isAlmostEqual(1.0000001, 1, 0.001)).toBe(true);
    expect(shell.api.shape.isAlmostEqual(1.5, 1, 0.001)).toBe(false);
  });
});
