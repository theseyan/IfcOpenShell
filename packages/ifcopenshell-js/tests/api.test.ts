
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

  it('copies and reassigns classes through the generated root API', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = shell.api.root.createEntity(file, {
      ifcClass: 'IfcWall',
      name: 'Native wall',
    });
    const copy = shell.api.root.copyClass(file, wall);

    expect(copy.type).toBe('IfcWall');
    expect(copy.raw.identity()).not.toBe(wall.raw.identity());
    expect(await copy.get('Name')).toBe('Native wall');

    const slab = shell.api.root.reassignClass(file, {
      product: copy,
      ifcClass: 'IfcSlab',
      predefinedType: 'FLOOR',
    });
    expect(slab.type).toBe('IfcSlab');
    expect(await slab.get('PredefinedType')).toBe('FLOOR');

    const defaultWall = shell.api.root.createEntity(file, { ifcClass: 'IfcWall' });
    const proxy = shell.api.root.reassignClass(file, { product: defaultWall });
    expect(proxy.type).toBe('IfcBuildingElementProxy');
  });

  it('appends an asset through the generated project API', async () => {
    await using target = await IfcFile.createEmpty(shell, 'IFC4');
    await using library = await IfcFile.createEmpty(shell, 'IFC4');
    const wall = await shell.api.root.createEntity(library, {
      ifcClass: 'IfcWall',
      name: 'Library wall',
    });
    const appended = shell.api.project.appendAsset(target, { library, element: wall });
    expect(appended?.type).toBe('IfcWall');

    const unsupported = await shell.api.root.createEntity(library, {
      ifcClass: 'IfcProject',
      name: 'Unsupported project',
    });
    expect(shell.api.project.appendAsset(target, {
      library,
      element: unsupported,
    })).toBeNull();

    const cachedWall = await shell.api.root.createEntity(library, {
      ifcClass: 'IfcWall',
      name: 'Cached library wall',
    });
    const cache = shell.api.project.appendAssetCacheNew();
    expect(cache).not.toBeNull();
    try {
      const cached = shell.api.project.appendAsset(target, {
        library,
        element: cachedWall,
        cache: cache!,
      });
      expect(cached?.type).toBe('IfcWall');

      const entries = shell.api.project.appendAssetCacheEntries(cache!);
      expect(entries.sourceIdentities).toContain(cachedWall.raw.identity());
      expect(entries.targets.some((targetEntity) => targetEntity.type === 'IfcWall')).toBe(true);
    } finally {
      cache?.destroy();
    }
  });

  it('maps snake-case fields from an existing multi-field result', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const segment = file.create('IfcFlowSegment');
    const profile = file.create('IfcRectangleProfileDef');
    profile.set('ProfileType', 'AREA');
    profile.set('XDim', 2);
    profile.set('YDim', 1);
    const material = file.create('IfcMaterial', { name: 'Material' });
    const materialProfile = file.create('IfcMaterialProfile');
    materialProfile.set('Material', material);
    materialProfile.set('Profile', profile);
    const materialSet = file.create('IfcMaterialProfileSet');
    materialSet.set('MaterialProfiles', [materialProfile]);
    const association = file.create('IfcRelAssociatesMaterial');
    association.set('RelatedObjects', [segment]);
    association.set('RelatingMaterial', materialSet);

    const result = shell.api.shape.builderMepBendShape(file, {
      segment,
      startLength: 1,
      endLength: 1,
      angle: 1,
      radius: 1,
      bendVector: [1, 0],
      flipZAxis: false,
    });

    expect(result.representation.type).toBe('IfcShapeRepresentation');
    expect(result.startLength).toBe(1);
    expect(result.endLength).toBe(1);
    expect(result.lateralAxis).toBe(0);
    expect(result.mainProfileDimension).toBe(1);
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

  it('authors and edits units through plain option objects', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    await shell.api.root.createEntity(file, { ifcClass: 'IfcProject' });

    const assignment = shell.api.unit.assignUnit(file, {});
    expect(assignment.type).toBe('IfcUnitAssignment');
    const siUnits = file.all('IfcSIUnit');
    expect(siUnits).toHaveLength(3);
    const length = siUnits.find((unit) => unit.get('UnitType') === 'LENGTHUNIT');
    expect(length?.get('Prefix')).toBe('MILLI');

    const foot = shell.api.unit.addConversionBasedUnit(file, {});
    expect(foot.type).toBe('IfcConversionBasedUnit');
    expect(foot.get('Name')).toBe('foot');
    shell.api.unit.editNamedUnit(file, {
      unit: foot,
      attributes: { Name: 'custom foot' },
    });
    expect(foot.get('Name')).toBe('custom foot');

    const fahrenheit = shell.api.unit.addConversionBasedUnit(file, {
      name: 'fahrenheit',
      conversionOffset: 0,
    });
    expect(fahrenheit.type).toBe('IfcConversionBasedUnitWithOffset');
    expect(fahrenheit.get('ConversionOffset')).toBe(-459.67);
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
