
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkipFull, readSampleIfcBytes } from '../_helper.js';
import { GeomSettings, GeometryTree, IfcFile, type IfcOpenShell } from '../../src/index.js';

describeOrSkipFull('GeometryTree', () => {
  let shell: IfcOpenShell;
  let file: IfcFile;
  let settings: GeomSettings;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
    file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
    settings = new GeomSettings(shell);
  });

  afterAll(async () => {
    await settings.dispose();
    await file.dispose();
    await shell.dispose();
  });

  it('builds from a file and selects entities by bounding box', async () => {
    await using tree = file.tree(settings);
    const selected = await tree.selectBox({
      min: [-10, -10, -10],
      max: [10, 10, 10],
    });
    try {
      expect(Array.isArray(selected)).toBe(true);
    } finally {
      await Promise.all(selected.map((entity) => entity.dispose()));
    }
  });

  it('returns complete detached ray hits in native order', async () => {
    const bounds = await file.bounds(settings, { kernel: 'opencascade' });
    expect(bounds.min).not.toBeNull();
    expect(bounds.max).not.toBeNull();
    const min = bounds.min!;
    const max = bounds.max!;
    const tree = file.tree(settings);
    const hits = await tree.raycast({
      origin: [(min[0] + max[0]) / 2, (min[1] + max[1]) / 2, max[2] + 10],
      direction: [0, 0, -1],
      length: max[2] - min[2] + 20,
    });
    try {
      expect(hits.length).toBeGreaterThan(0);
      const rayDistances = hits.map((hit) => hit.rayDistance);
      expect(rayDistances).toEqual([...rayDistances].sort((a, b) => b - a));
      for (const hit of hits) {
        expect(hit.entity.id).toBeGreaterThan(0);
        expect(hit.position).toHaveLength(3);
        expect(hit.normal).toHaveLength(3);
        expect(hit.position.every(Number.isFinite)).toBe(true);
        expect(hit.normal.every(Number.isFinite)).toBe(true);
        expect(Number.isFinite(hit.distance)).toBe(true);
        expect(Number.isFinite(hit.rayDistance)).toBe(true);
        expect(Number.isFinite(hit.dotProduct)).toBe(true);
        expect(Number.isInteger(hit.styleIndex)).toBe(true);
      }
      const retainedPosition = [...hits[0]!.position];
      tree.dispose();
      expect(hits[0]!.position).toEqual(retainedPosition);
    } finally {
      tree.dispose();
      await Promise.all(hits.map((hit) => hit.entity.dispose()));
    }
  });

  it('builds from a successfully initialized iterator', async () => {
    await using iterator = file.meshes(settings, { kernel: 'opencascade' });
    const tree = GeometryTree.fromIterator(shell, iterator);
    await tree[Symbol.asyncDispose]();
    expect(() => tree.dispose()).not.toThrow();
  });
});
