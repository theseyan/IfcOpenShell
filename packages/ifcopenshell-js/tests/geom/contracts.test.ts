import { describe, expect, it, vi } from 'vitest';
import { GeomIterator, GeometryTree, IfcOpenShellError } from '../../src/index.js';
import type { IfcFile, IfcOpenShell } from '../../src/index.js';
import type { GeomSettings } from '../../src/geom/settings.js';

describe('geometry failure contracts', () => {
  it('reports failed iterator initialization instead of silent exhaustion', async () => {
    const rawIterator = {
      ptr: 1,
      destroy: vi.fn(),
      initialize: vi.fn(() => false),
      hadErrorProcessingElements: vi.fn(() => true),
    };
    const shell = fakeShell({ createIterator: () => rawIterator });
    const iterator = new GeomIterator(shell, fakeFile(), fakeSettings());
    await expect(iterator.nextMesh()).rejects.toThrow(/Failed to initialize GeomIterator.*selected kernel/);
    await iterator[Symbol.asyncDispose]();
    expect(rawIterator.destroy).toHaveBeenCalledOnce();
  });

  it('rejects null geometry-tree factory handles', async () => {
    const shell = fakeShell({ createTreeFromFile: () => null });
    const tree = GeometryTree.fromFile(shell, fakeFile());
    await expect(tree.selectBox({ min: [0, 0, 0], max: [1, 1, 1] })).rejects.toThrow(
      /Failed to create GeometryTree from file.*no handle/,
    );
    await tree[Symbol.asyncDispose]();
  });

  it('requires iterator initialization before building a tree', async () => {
    const shell = fakeShell({ createTreeFromIterator: vi.fn() });
    const iterator = { initialize: vi.fn(async () => false) } as unknown as GeomIterator;
    const tree = GeometryTree.fromIterator(shell, iterator);
    await expect(tree.selectBox({ min: [0, 0, 0], max: [1, 1, 1] })).rejects.toBeInstanceOf(IfcOpenShellError);
    await expect(tree[Symbol.asyncDispose]()).resolves.toBeUndefined();
  });
});

function fakeShell(geom: Record<string, unknown>): IfcOpenShell {
  return {
    loadPlugin: vi.fn(async () => undefined),
    raw: { geom },
  } as unknown as IfcOpenShell;
}

function fakeFile(): IfcFile {
  return { raw: { schemaName: () => 'IFC4' } } as unknown as IfcFile;
}

function fakeSettings(): GeomSettings {
  return { raw: {}, getInt: () => 0 } as unknown as GeomSettings;
}
