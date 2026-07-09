
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkip, readSampleIfcBytes } from './_helper.js';
import { IfcFile, IfcOpenShellError, type IfcOpenShell } from '../src/index.js';

describeOrSkip('IfcFile', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell.dispose();
  });

  it('opens an IFC model and exposes high-level queries', async () => {
    await using file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');

    expect(file.schema).toBe('IFC4');
    const info = await file.info();
    expect(info.schema).toBe('IFC4');
    expect(info.entityCount).toBeGreaterThan(0);
    expect(info.maxId).toBeGreaterThan(0);
    expect(info.header?.schemas).toContain('IFC4');
    expect(await file.header()).toMatchObject({ schemas: ['IFC4'] });
    expect(file.status()).toBeGreaterThanOrEqual(0);
    expect(await file.storageMode()).toBeGreaterThanOrEqual(0);
    expect(file.maxId).toBeGreaterThan(0);
    expect(file.ids).toContain(315);
    expect(file.types).toContain('IfcWall');
    expect(file.entityCount).toBeGreaterThan(0);

    const wall = await file.get(315);
    expect(wall?.id).toBe(315);
    expect(wall?.type).toBe('IfcWall');
    await wall?.dispose();

    const byGuid = await file.find('37cU2nGL100xirW00wMfmm');
    expect(await byGuid?.isA('IfcWall')).toBe(true);
    await byGuid?.dispose();

    const walls = await file.all('IfcWall');
    expect(walls).toHaveLength(1);
    await Promise.all(walls.map((entity) => entity.dispose()));

    expect(await file.get(9999999)).toBeNull();
    expect(await file.text()).toContain('ISO-10303-21');
  });

  it('exposes inverse and traversal queries', async () => {
    await using file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
    await using wall = await file.get(315);
    expect(await file.totalInverses(wall!)).toBeGreaterThan(0);
    expect((await file.inverseIndices(wall!)).length).toBeGreaterThan(0);
    const inverses = await file.inverses(wall!);
    try {
      expect(inverses.length).toBeGreaterThan(0);
    } finally {
      await Promise.all(inverses.map((entity) => entity.dispose()));
    }
    const traversal = await file.traverse(wall!, { maxDepth: 1 });
    try {
      expect(traversal.length).toBeGreaterThan(0);
    } finally {
      await Promise.all(traversal.map((entity) => entity.dispose()));
    }
  });

  it('honours an already-aborted open signal', async () => {
    const controller = new AbortController();
    controller.abort();
    await expect(
      IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc', { signal: controller.signal }),
    ).rejects.toBeInstanceOf(IfcOpenShellError);
  });

  it('creates an empty file and new entities', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    expect(file.schema).toBe('IFC4');
    expect(file.ids).toHaveLength(0);

    const wall = await file.create('IfcWall', { name: 'Created Wall' });
    try {
      expect(wall.type).toBe('IfcWall');
      const walls = await file.all('IfcWall');
      try {
        expect(walls.map((entity) => entity.id)).toEqual([wall.id]);
      } finally {
        await Promise.all(walls.map((entity) => entity.dispose()));
      }
    } finally {
      await wall.dispose();
    }
  });

  it('dispose is idempotent and guards released handles', async () => {
    const file = await IfcFile.createEmpty(shell, 'IFC4');
    file.dispose();
    file.dispose();
    expect(() => file.raw).toThrow(IfcOpenShellError);
  });
});
