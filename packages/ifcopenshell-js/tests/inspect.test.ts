
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkip, readLargeOffsetIfcBytes, readSampleIfcBytes } from './_helper.js';
import { IfcFile, type IfcOpenShell } from '../src/index.js';

describeOrSkip('inspectEntity', () => {
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

  it('returns null for missing ids', async () => {
    expect(await file.inspect(999_999_999)).toBeNull();
  });

  it('snapshots entity id, type, guid, and attributes', async () => {
    const info = await file.inspect(315);
    expect(info).not.toBeNull();
    expect(info!.id).toBe(315);
    expect(info!.type).toBe('IfcWall');
    expect(info!.guid).toBe('37cU2nGL100xirW00wMfmm');
    expect(info!.attributes.find((a) => a.name === 'GlobalId')?.value).toBe('37cU2nGL100xirW00wMfmm');
    expect(info!.attributes.find((a) => a.name === 'Name')?.value).toContain('Basic Wall');
  });

  it('snapshots Large Offset opening attributes with stable names', async () => {
    await using largeOffset = await IfcFile.open(shell, readLargeOffsetIfcBytes(), 'large_offset.ifc');
    const info = await largeOffset.inspect(2156);
    expect(info).not.toBeNull();
    expect(info!.type).toBe('IfcOpeningElement');
    expect(info!.guid).toBe('1PTz52_o9DlAOe36su2tDU');
    expect(info!.attributes.map((a) => a.name)).toEqual([
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
});
