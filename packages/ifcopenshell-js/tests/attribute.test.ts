
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkip, readSampleIfcBytes } from './_helper.js';
import { Entity, IfcFile, IfcOpenShellError, type IfcOpenShell } from '../src/index.js';

describeOrSkip('AttributeValue', () => {
  let shell: IfcOpenShell;
  let file: IfcFile;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
    file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
  });

  afterAll(async () => {
    await file.dispose();
    await shell.dispose();
  });

  it('reads string and null attributes', async () => {
    await using wall = await file.get(315);
    await using guid = await wall!.attribute('GlobalId');
    expect(guid.isNull).toBe(false);
    expect(await guid.string()).toBe('37cU2nGL100xirW00wMfmm');
    expect(await guid.value()).toBe('37cU2nGL100xirW00wMfmm');

    await using rel = await file.get(623);
    await using desc = await rel!.attribute('Description');
    expect(desc.isNull).toBe(true);
  });

  it('reads entity references and aggregate references', async () => {
    await using wall = await file.get(315);
    await using ownerHistory = await wall!.attribute('OwnerHistory');
    await using owner = await ownerHistory.entity();
    expect(await owner?.isA('IfcOwnerHistory')).toBe(true);
    const ownerFromValue = await ownerHistory.value();
    expect(ownerFromValue).toBeInstanceOf(Object);
    expect(Array.isArray(ownerFromValue)).toBe(false);
    expect(ownerFromValue).toBeInstanceOf(Entity);
    await (ownerFromValue as Entity).dispose();

    await using rel = await file.get(623);
    await using related = await rel!.attribute(4);
    const entities = await related.entities();
    try {
      expect(entities).toHaveLength(1);
      expect(await entities[0]!.isA('IfcWall')).toBe(true);
    } finally {
      await Promise.all(entities.map((entity) => entity.dispose()));
    }
    const value = await related.value();
    try {
      expect(Array.isArray(value)).toBe(true);
      expect(value).toHaveLength(1);
    } finally {
      if (Array.isArray(value)) {
        await Promise.all(value.map((entity) => entity instanceof Entity ? entity.dispose() : undefined));
      }
    }
  });

  it('reads booleans and numeric lists', async () => {
    await using faceSet = await file.get(280);
    await using closed = await faceSet!.attribute(1);
    expect(await closed.boolean()).toBe(true);

    await using point = await file.get(6);
    await using coords = await point!.attribute(0);
    expect(await coords.size()).toBe(3);
    expect(await coords.numbers()).toHaveLength(3);
  });

  it('dispose is idempotent and guards released handles', async () => {
    await using wall = await file.get(315);
    const guid = await wall!.attribute(0);
    guid.dispose();
    guid.dispose();
    expect(() => guid.raw).toThrow(IfcOpenShellError);
  });
});
