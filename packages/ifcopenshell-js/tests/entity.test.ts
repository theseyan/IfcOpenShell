
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkip, readLargeOffsetIfcBytes, readSampleIfcBytes } from './_helper.js';
import { IfcFile, IfcOpenShellError, type IfcOpenShell } from '../src/index.js';

describeOrSkip('Entity', () => {
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

  it('exposes id, type and schema class checks', async () => {
    await using wall = await file.get(315);
    expect(wall?.id).toBe(315);
    expect(wall?.type).toBe('IfcWall');
    expect(await wall?.className()).toBe('IfcWall');
    expect(await wall?.className(true)).toContain('IfcWall');
    expect(await wall?.isA('IfcRoot')).toBe(true);
    expect(await wall?.isA('IfcSlab')).toBe(false);
  });

  it('reads attributes by index or name', async () => {
    await using wall = await file.get(315);
    await using guid = await wall!.attribute('GlobalId');
    expect(await guid.string()).toBe('37cU2nGL100xirW00wMfmm');

    const names = await wall!.attributes();
    expect(names).toContain('GlobalId');
    expect(names).toContain('Name');
    expect(await wall!.attributeIndex('GlobalId')).toBe(0);
    expect(await wall!.attributeName(0)).toBe('GlobalId');
    expect(await wall!.attributeType(0)).toBeTruthy();
    expect(await wall!.attributeCategory('GlobalId')).toBeGreaterThanOrEqual(0);

    await using first = await wall!.attribute(0);
    expect(first.type.length).toBeGreaterThan(0);
    expect(await wall!.get('GlobalId')).toBe('37cU2nGL100xirW00wMfmm');

    const info = wall!.info();
    expect(info).toMatchObject({
      id: 315,
      type: 'IfcWall',
      attributes: {
        GlobalId: '37cU2nGL100xirW00wMfmm',
      },
    });
    expect(wall!.entries().some(([name]) => name === 'Name')).toBe(true);
  });

  it('reads inverse attributes and mutates simple attribute values', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    await using wall = await editable.create('IfcWall', { name: 'Before' });
    await using nameBefore = await wall.attribute('Name');
    expect(await nameBefore.string()).toBe('Before');

    await wall.set('Name', 'After');
    expect(await wall.get('Name')).toBe('After');
    await using nameAfter = await wall.attribute('Name');
    expect(await nameAfter.string()).toBe('After');

    await wall.unset('Name');
    await using nameUnset = await wall.attribute('Name');
    expect(nameUnset.isNull).toBe(true);

    expect(await wall.inverseAttributes()).toContain('HasAssignments');
    expect(await wall.inverse('HasAssignments')).toEqual([]);
  });

  it('keeps Large Offset attribute names stable', async () => {
    await using largeOffset = await IfcFile.open(shell, readLargeOffsetIfcBytes(), 'large_offset.ifc');
    await using opening = await largeOffset.get(2156);
    expect(await opening!.attributes()).toEqual([
      'GlobalId',
      'OwnerHistory',
      'Name',
      'Description',
      'ObjectType',
      'ObjectPlacement',
      'Representation',
      'Tag',
      'PredefinedType',
    ]);
  });

  it('dispose is idempotent and guards released handles', async () => {
    const wall = await file.get(315);
    wall!.dispose();
    wall!.dispose();
    expect(() => wall!.raw).toThrow(IfcOpenShellError);
  });
});
