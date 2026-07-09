
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkipFull, readSampleIfcBytes } from '../_helper.js';
import { GeomSettings, IfcFile, type IfcOpenShell } from '../../src/index.js';

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

  it('supports raycasting with entity results', async () => {
    await using tree = file.tree(settings);
    const hits = await tree.raycast({
      origin: [0, 0, 10],
      direction: [0, 0, -1],
      length: 100,
    });
    try {
      expect(Array.isArray(hits)).toBe(true);
      for (const hit of hits) {
        expect(hit.entity.id).toBeGreaterThan(0);
      }
    } finally {
      await Promise.all(hits.map((hit) => hit.entity.dispose()));
    }
  });
});
