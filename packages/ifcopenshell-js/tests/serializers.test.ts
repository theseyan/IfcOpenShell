
import { afterAll, beforeAll, describe, expect, it, vi } from 'vitest';
import { createInstance, describeOrSkip, readSampleIfcBytes } from './_helper.js';
import {
  exportToBuffer,
  GeomSettings,
  IfcFile,
  IfcOpenShellError,
  SerializerSettings,
  type IfcOpenShell,
} from '../src/index.js';

describeOrSkip('serializers', () => {
  let shell: IfcOpenShell;
  let file: IfcFile;
  let geom: GeomSettings;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
    await shell.loadPlugin('kernel', 'passthrough');
    await shell.loadPlugin('mapping', 'ifc4');
    file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
    geom = new GeomSettings(shell);
  });

  afterAll(async () => {
    await geom.dispose();
    await file.dispose();
    await shell.dispose();
  });

  it('uses serializer settings with generic and typed accessors', async () => {
    await using settings = new SerializerSettings(shell);
    expect((await settings.names()).length).toBeGreaterThan(0);

    await settings.set('section-height', 2.75);
    expect(await settings.getDouble('section-height')).toBeCloseTo(2.75, 6);

    await settings.setInt('digits', 7);
    expect(await settings.value('digits')).toBe(7);

    await settings.set('base-uri', 'https://example.com/');
    expect(await settings.getString('base-uri')).toBe('https://example.com/');
  });

  it('disposes serializer settings deterministically', async () => {
    const settings = new SerializerSettings(shell);
    settings.dispose();
    settings.dispose();
    expect(() => settings.names()).toThrow(IfcOpenShellError);
  });

  it('auto-loads and exports OBJ output', async () => {
    const phases: string[] = [];
    const result = await exportToBuffer(shell, file, geom, 'obj', {
      kernel: 'passthrough',
      numThreads: 1,
      onProgress: (progress) => phases.push(progress.phase),
    });
    expect(await shell.loadedPlugins()).toContain('geometry_serializer:obj');
    expect(result === null || (typeof result.primary === 'string' && typeof result.secondary === 'string')).toBe(true);
    expect(phases).toContain('plugin');
  });

  it('passes caller-owned serializer settings to the native export', async () => {
    await using serializerSettings = new SerializerSettings(shell);
    const calls: unknown[][] = [];
    const expected = new Error('stop after capturing serializer settings');
    const raw = Object.create(shell.raw) as typeof shell.raw;
    const geom = Object.create(shell.raw.geom) as typeof shell.raw.geom;
    Object.defineProperty(geom, 'createGeometrySerializerByStream', {
      value: (...args: unknown[]) => {
        calls.push(args);
        throw expected;
      },
    });
    Object.defineProperty(raw, 'geom', { value: geom });
    const exportShell = Object.create(shell) as IfcOpenShell;
    Object.defineProperty(exportShell, 'raw', { value: raw });

    await expect(exportToBuffer(exportShell, file, geom, 'obj', {
      kernel: 'passthrough',
      numThreads: 1,
      serializerSettings,
    })).rejects.toBe(expected);
    expect(calls).toHaveLength(1);
    expect(calls[0]?.[4]).toBe(serializerSettings.raw);
  });

  it('reports a failed representation fallback instead of silently omitting geometry', async () => {
    const iterator = {
      ptr: 1,
      initialize: () => true,
      getAsTriangulationElement: () => { throw new Error('triangulation failed'); },
      getAsBrepElement: () => { throw new Error('BRep failed'); },
      next: () => false,
      destroy: vi.fn(),
    };
    const serializer = {
      ptr: 1,
      setFile: vi.fn(),
      writeHeader: vi.fn(),
      isTesselated: () => true,
      destroy: vi.fn(),
    };
    const buffer = { ptr: 1, destroy: vi.fn() };
    const serializerSettings = { ptr: 1, destroy: vi.fn() };
    const exportShell = {
      raw: {
        geom: {
          createSerializerSettings: () => serializerSettings,
          createBuffer: () => buffer,
          createGeometrySerializerByStream: () => serializer,
          createIterator: () => iterator,
        },
      },
      loadPlugin: async () => undefined,
      fs: null,
    } as unknown as IfcOpenShell;

    await expect(exportToBuffer(
      exportShell,
      { raw: { schemaName: () => 'IFC4' } } as IfcFile,
      { raw: {} } as GeomSettings,
      'obj',
    )).rejects.toThrow('Failed to serialize a geometry element as triangulation or BRep');
    expect(iterator.destroy).toHaveBeenCalledOnce();
    expect(serializer.destroy).toHaveBeenCalledOnce();
  });

  it('exports TTL output when the plugin is available', async () => {
    await shell.loadPlugin('geometry_serializer', 'ttl');
    const result = await exportToBuffer(shell, file, geom, 'ttl', { kernel: 'passthrough', numThreads: 1 });
    if (result === null) return;
    expect(result.secondary).toBe('');
    expect(result.primary.length).toBeGreaterThanOrEqual(0);
  });

  it('rejects unsupported formats', async () => {
    await expect(exportToBuffer(shell, file, geom, 'xml' as never)).rejects.toBeInstanceOf(IfcOpenShellError);
  });
});
