
import { afterAll, beforeAll, describe, expect, it, vi } from 'vitest';
import { createInstance, describeOrSkip, readSampleIfcBytes } from '../_helper.js';
import { GeomIterator, GeomSettings, IfcFile, IfcOpenShellError, type IfcOpenShell } from '../../src/index.js';

describeOrSkip('GeomIterator', () => {
  let shell: IfcOpenShell;
  let file: IfcFile;
  let settings: GeomSettings;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
    await shell.loadPlugin('kernel', 'passthrough');
    await shell.loadPlugin('mapping', 'ifc4');
    file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
    settings = new GeomSettings(shell);
  });

  afterAll(async () => {
    await settings.dispose();
    await file.dispose();
    await shell.dispose();
  });

  it('initializes and reports metadata', async () => {
    await using iterator = new GeomIterator(shell, file, settings, { kernel: 'passthrough', numThreads: 1 });
    expect(await iterator.initialize()).toBe(true);
    const metadata = await iterator.metadata();
    expect(metadata.initialized).toBe(true);
    expect(metadata.progress).toBeGreaterThanOrEqual(0);
    expect(metadata.unitName.length).toBeGreaterThan(0);
    expect(metadata.unitMagnitude).toBeGreaterThan(0);
  });

  it('yields meshes as transferable typed arrays', async () => {
    await using iterator = file.meshes(settings, { kernel: 'passthrough', numThreads: 1 });
    const mesh = await iterator.next();
    expect(mesh).not.toBeNull();
    expect(typeof mesh!.id).toBe('number');
    expect(typeof mesh!.guid).toBe('string');
    expect(typeof mesh!.type).toBe('string');
    expect(mesh!.vertices).toBeInstanceOf(Float32Array);
    expect(mesh!.faces).toBeInstanceOf(Uint32Array);
    expect(mesh!.transform).toBeInstanceOf(Float64Array);
    expect(mesh!.edges).toBeInstanceOf(Uint32Array);
    expect(mesh!.materialIds).toBeInstanceOf(Int32Array);
    expect(mesh!.itemIds).toBeInstanceOf(Int32Array);
    expect(mesh!.edgeItemIds).toBeInstanceOf(Int32Array);
    expect(mesh!.uvs).toBeInstanceOf(Float32Array);
    expect(mesh!.colors).toBeInstanceOf(Float32Array);
    expect(mesh!.transform.length).toBe(16);
    expect(mesh!.transform[3]).toBe(0);
    expect(mesh!.transform[7]).toBe(0);
    expect(mesh!.transform[11]).toBe(0);
    expect(mesh!.transform[15]).toBe(1);
  });

  it('disposes settings it creates for file.meshes()', async () => {
    const dispose = vi.spyOn(GeomSettings.prototype, 'dispose');
    try {
      const iterator = file.meshes(undefined, { kernel: 'passthrough' });
      await iterator.initialize();
      await iterator[Symbol.asyncDispose]();
      expect(dispose).toHaveBeenCalledTimes(1);
    } finally {
      dispose.mockRestore();
    }
  });

  it('supports filtered iteration by type, guid, and numeric id', async () => {
    await using byType = file.meshes(settings, {
      kernel: 'passthrough',
      filter: { kind: 'types', values: ['IfcWall'], include: true },
    });
    const typeResult = await byType.collect({ limit: 4, skipEmpty: true });
    expect(typeResult.meshes.every((mesh) => mesh.type === 'IfcWall')).toBe(true);

    await using byGuid = file.meshes(settings, {
      kernel: 'passthrough',
      filter: { kind: 'guids', values: ['37cU2nGL100xirW00wMfmm'], include: true },
    });
    const guidResult = await byGuid.collect({ limit: 4, skipEmpty: true });
    expect(guidResult.meshes.every((mesh) => mesh.guid === '37cU2nGL100xirW00wMfmm')).toBe(true);

    await using byId = file.meshes(settings, {
      kernel: 'passthrough',
      filter: { kind: 'ids', values: [315], include: true },
    });
    const idResult = await byId.collect({ limit: 4, skipEmpty: true });
    expect(idResult.meshes.every((mesh) => mesh.id === 315)).toBe(true);
  });

  it('supports async iteration and bounds', async () => {
    await using iterator = file.meshes(settings);
    let count = 0;
    for await (const _mesh of iterator) {
      count++;
      if (count >= 2) break;
    }
    expect(count).toBeGreaterThan(0);

    await using boundsIterator = file.meshes(settings);
    await boundsIterator.initialize();
    await boundsIterator.computeBounds(true);
    const bounds = await boundsIterator.bounds();
    expect(bounds.min === null || Array.isArray(bounds.min)).toBe(true);
    expect(bounds.max === null || Array.isArray(bounds.max)).toBe(true);
  });

  it('collects meshes with progress and limit support', async () => {
    await using iterator = file.meshes(settings);
    const progress: number[] = [];
    const result = await iterator.collect({
      limit: 1,
      progressInterval: 1,
      skipEmpty: true,
      onProgress: (event) => progress.push(event.meshes),
    });
    expect(result.meshes).toHaveLength(1);
    expect(result.truncated).toBe(true);
    expect(result.metadata.initialized).toBe(true);
    expect(progress.length).toBeGreaterThan(0);
  });

  it('honours an already-aborted collect signal', async () => {
    await using iterator = file.meshes(settings);
    const controller = new AbortController();
    controller.abort();
    await expect(iterator.collect({ signal: controller.signal })).rejects.toBeInstanceOf(IfcOpenShellError);
  });

  it('dispose is idempotent and guards released handles', async () => {
    const iterator = new GeomIterator(shell, file, settings);
    await iterator.dispose();
    await iterator.dispose();
    await expect(iterator.metadata()).rejects.toBeInstanceOf(IfcOpenShellError);
  });
});
