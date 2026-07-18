
import { afterAll, beforeAll, describe, expect, it, vi } from 'vitest';
import { createInstance, describeOrSkip, readSampleIfcBytes } from '../_helper.js';
import {
  GeomIterator,
  GeomSettings,
  IfcFile,
  IfcOpenShellError,
  IfcOpenShellErrorCode,
  isIfcOpenShellAbortError,
  transformPoint4,
  type IfcOpenShell,
} from '../../src/index.js';
import { schemaPluginId } from '../../src/geom/iterator.js';

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
    expect(metadata.progress).toBeLessThanOrEqual(1);
    expect(metadata.unitName.length).toBeGreaterThan(0);
    expect(metadata.unitMagnitude).toBeGreaterThan(0);
  });

  it('treats an unfiltered valid empty model as clean exhaustion with null bounds', async () => {
    await using emptyFile = await IfcFile.createEmpty(shell, 'IFC4');
    await using iterator = emptyFile.meshes(settings, { kernel: 'passthrough' });
    expect(await iterator.nextMesh()).toBeNull();
    const metadata = await iterator.metadata();
    expect(metadata.hadError).toBe(false);

    const bounds = await emptyFile.bounds(settings, { kernel: 'passthrough' });
    expect(bounds).toEqual({ min: null, max: null });
  });

  it('preserves native column-major translation and rotation after disposal', async () => {
    await using transformedFile = await IfcFile.createEmpty(shell, 'IFC4');
    using project = shell.api.root.createEntity(transformedFile, { ifcClass: 'IfcProject', name: 'Project' });
    using model = shell.api.context.addContext(transformedFile, {
      contextType: 'Model', contextIdentifier: '', targetView: '',
    });
    using body = shell.api.context.addContext(transformedFile, {
      contextType: 'Model', contextIdentifier: 'Body', targetView: 'MODEL_VIEW', parent: model,
    });
    using wall = shell.api.root.createEntity(transformedFile, { ifcClass: 'IfcWall', name: 'Transformed wall' });
    using representation = shell.api.geometry.addMeshRepresentation(transformedFile, body, {
      items: [{
        vertices: [[0, 0, 0], [2, 0, 0], [0, 3, 0]],
        faces: [{ outer: [0, 1, 2] }],
      }],
    });
    using relation = shell.api.geometry.assignRepresentation(transformedFile, wall, representation);
    using placement = shell.api.geometry.editObjectPlacement(transformedFile, {
      product: wall,
      matrix: [0, -1, 0, 10, 1, 0, 0, 20, 0, 0, 1, 30, 0, 0, 0, 1],
      isSi: true,
      shouldTransformChildren: false,
    });

    const iterator = transformedFile.meshes(settings, {
      kernel: 'passthrough',
      filter: { kind: 'ids', values: [wall.id] },
    });
    const mesh = await iterator.nextMesh();
    expect(mesh).not.toBeNull();
    const retained = mesh!.transform;
    const snapshot = retained.slice();
    expect([retained[12], retained[13], retained[14]]).toEqual([10, 20, 30]);
    expect([retained[3], retained[7], retained[11]]).toEqual([0, 0, 0]);
    expect(retained[0]).toBeCloseTo(0);
    expect(retained[1]).toBeCloseTo(1);
    expect(retained[4]).toBeCloseTo(-1);
    expect(transformPoint4(retained, [2, 3, 4])).toEqual([7, 22, 34]);
    await iterator[Symbol.asyncDispose]();
    expect(retained).toEqual(snapshot);
  });

  it('supports opt-in detached Float64 geometry snapshots', async () => {
    await using iterator = file.meshes(settings, { kernel: 'passthrough', precision: 'float64' });
    const mesh = await iterator.nextMesh();
    expect(mesh?.vertices).toBeInstanceOf(Float64Array);
    expect(mesh?.normals === null || mesh?.normals instanceof Float64Array).toBe(true);
    expect(mesh?.uvs).toBeInstanceOf(Float64Array);
    expect(mesh?.colors).toBeInstanceOf(Float64Array);
    const retained = mesh!.vertices;
    const snapshot = retained.slice();
    await iterator[Symbol.asyncDispose]();
    expect(retained).toEqual(snapshot);
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
    expect(mesh!.vertices.length).toBeGreaterThan(0);
    const retainedVertices = mesh!.vertices;
    const expectedVertices = retainedVertices.slice();
    await iterator.next();
    await iterator.dispose();
    expect(retainedVertices).toEqual(expectedVertices);
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
    const progress: Array<{ current: number; meshes: number; ratio: number }> = [];
    const result = await iterator.collect({
      limit: 1,
      progressInterval: 1,
      skipEmpty: true,
      onProgress: (event) => progress.push({ current: event.current!, meshes: event.meshes, ratio: event.ratio! }),
    });
    expect(result.meshes).toHaveLength(1);
    expect(result.truncated).toBe(true);
    expect(result.metadata.initialized).toBe(true);
    expect(progress.length).toBeGreaterThan(0);
    expect(progress.every((event) => event.current >= event.meshes)).toBe(true);
    expect(progress.every((event) => event.ratio >= 0 && event.ratio <= 1)).toBe(true);
  });

  it('rejects non-triangulated iterator output with a remedy', () => {
    using nativeSettings = new GeomSettings(shell);
    nativeSettings.setInt('iterator-output', 1);
    expect(() => file.meshes(nativeSettings)).toThrow(/requires triangulated geometry.*iterator-output.*0/i);
  });

  it('rejects unsupported geometry schemas descriptively', () => {
    expect(() => schemaPluginId('IFC5_EXPERIMENTAL')).toThrow(/Unsupported geometry mapping schema.*IFC5_EXPERIMENTAL/);
  });

  it('honours an already-aborted collect signal', async () => {
    await using iterator = file.meshes(settings);
    const controller = new AbortController();
    controller.abort('test cancellation reason');
    let cancellation: unknown;
    try {
      await iterator.collect({ signal: controller.signal });
    } catch (error) {
      cancellation = error;
    }
    expect(cancellation).toBeInstanceOf(IfcOpenShellError);
    expect(isIfcOpenShellAbortError(cancellation)).toBe(true);
    expect(cancellation).toMatchObject({ name: 'AbortError', code: IfcOpenShellErrorCode.OPERATION_CANCELLED });
    expect((cancellation as Error).cause).toBe('test cancellation reason');
  });

  it('dispose is idempotent and guards released handles', async () => {
    const iterator = new GeomIterator(shell, file, settings);
    await iterator.dispose();
    await iterator.dispose();
    await expect(iterator.metadata()).rejects.toBeInstanceOf(IfcOpenShellError);
  });
});
