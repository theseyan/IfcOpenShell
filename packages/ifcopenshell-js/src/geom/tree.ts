
import type {
  IfcOpenshellGeomTree,
  IfcOpenshellGeomTreeRayIntersectionList,
  IfcOpenshellParseInstanceList,
} from '@ifcopenshell-js/wasm/api';
import { Entity } from '../entity.js';
import type { IfcFile } from '../file.js';
import { IfcOpenShellError, type IfcOpenShell } from '../init.js';
import { HandleGuard } from '../resource.js';
import type { GeomIterator } from './iterator.js';
import { loadGeometry } from './iterator.js';
import type { GeomSettings } from './settings.js';

/** Three-dimensional point or vector represented as `[x, y, z]`. */
export type Point3 = [number, number, number];

/** Axis-aligned box used for spatial selection. */
export interface Box3 {
  /** Minimum corner. */
  min: Point3;
  /** Maximum corner. */
  max: Point3;
}

/** Ray used for spatial intersection queries. */
export interface Ray {
  /** Ray origin. */
  origin: Point3;
  /** Ray direction. */
  direction: Point3;
  /** Maximum ray length. */
  length: number;
}

/** Entity returned by a ray intersection query. */
export interface RayHit {
  /** Intersected IFC entity. */
  entity: Entity;
}

/** Asynchronous spatial query tree built from IFC geometry. */
export class GeometryTree {
  private rawTree: IfcOpenshellGeomTree | null = null;
  private guard: HandleGuard<IfcOpenshellGeomTree> | null = null;
  private disposed = false;
  private readonly ready: Promise<IfcOpenshellGeomTree>;

  private constructor(
    private readonly shell: IfcOpenShell,
    ready: Promise<IfcOpenshellGeomTree>,
  ) {
    this.ready = ready.then((raw) => {
      if (this.disposed) {
        raw.destroy();
        return raw;
      }
      this.rawTree = raw;
      this.guard = new HandleGuard(this, raw, true);
      return raw;
    });
  }

  /** Build a tree directly from an IFC file. */
  static fromFile(shell: IfcOpenShell, file: IfcFile, settings?: GeomSettings): GeometryTree {
    return new GeometryTree(shell, createTreeFromFile(shell, file, settings));
  }

  /** Build a tree from an initialized geometry iterator. */
  static fromIterator(shell: IfcOpenShell, iterator: GeomIterator): GeometryTree {
    return new GeometryTree(shell, createTreeFromIterator(shell, iterator));
  }

  get raw(): IfcOpenshellGeomTree {
    if (this.disposed) throw new IfcOpenShellError('GeometryTree has been disposed');
    if (this.rawTree == null) throw new IfcOpenShellError('GeometryTree is not ready');
    return this.rawTree;
  }

  /** Select entities intersecting a point, with an optional tolerance extension. */
  async selectPoint(point: Point3, options: { extend?: number } = {}): Promise<Entity[]> {
    const tree = await this.handle();
    return wrapList(this.shell, tree.selectPoint(point[0], point[1], point[2], options.extend ?? 0));
  }

  /** Select entities intersecting or fully contained by an axis-aligned box. */
  async selectBox(box: Box3, options: { completelyWithin?: boolean } = {}): Promise<Entity[]> {
    const tree = await this.handle();
    return wrapList(this.shell, tree.selectBoxBounds(
      box.min[0],
      box.min[1],
      box.min[2],
      box.max[0],
      box.max[1],
      box.max[2],
      options.completelyWithin ?? false,
    ));
  }

  /** Select entities intersecting the bounds of another entity. */
  async selectEntity(entity: Entity, options: { completelyWithin?: boolean; extend?: number } = {}): Promise<Entity[]> {
    const tree = await this.handle();
    return wrapList(this.shell, tree.selectElement(
      entity.raw,
      options.completelyWithin ?? false,
      options.extend ?? 0,
    ));
  }

  /** Return entities intersected by a ray in distance order. */
  async raycast(ray: Ray): Promise<RayHit[]> {
    const tree = await this.handle();
    const hits = tree.selectRay(
      ray.origin[0],
      ray.origin[1],
      ray.origin[2],
      ray.direction[0],
      ray.direction[1],
      ray.direction[2],
      ray.length,
    );
    return rayHits(this.shell, tree, hits);
  }

  /** Release the native spatial tree. */
  dispose(): void {
    if (this.disposed) return;
    this.disposed = true;
    if (this.rawTree == null) return;
    this.guard?.destroy();
    this.guard = null;
    this.rawTree = null;
  }

  [Symbol.dispose](): void {
    this.dispose();
  }

  async [Symbol.asyncDispose](): Promise<void> {
    await this.ready.catch(() => undefined);
    this.dispose();
  }

  private async handle(): Promise<IfcOpenshellGeomTree> {
    if (this.disposed) throw new IfcOpenShellError('GeometryTree has been disposed');
    const tree = await this.ready;
    if (this.disposed) throw new IfcOpenShellError('GeometryTree has been disposed');
    return tree;
  }
}

async function createTreeFromFile(
  shell: IfcOpenShell,
  file: IfcFile,
  settings?: GeomSettings,
): Promise<IfcOpenshellGeomTree> {
  await loadGeometry(shell, file.raw, 'opencascade');
  await shell.loadPlugin('tree', 'opencascade.brep');
  await shell.loadPlugin('tree', 'opencascade.trianglebvh');
  return settings
    ? shell.raw.geom.createTreeFromFileWithSettings(file.raw, settings.raw)
    : shell.raw.geom.createTreeFromFile(file.raw);
}

async function createTreeFromIterator(shell: IfcOpenShell, iterator: GeomIterator): Promise<IfcOpenshellGeomTree> {
  await shell.loadPlugin('tree', 'opencascade.brep');
  await shell.loadPlugin('tree', 'opencascade.trianglebvh');
  await iterator.initialize();
  return shell.raw.geom.createTreeFromIterator(iterator.raw);
}

function wrapList(shell: IfcOpenShell, list: IfcOpenshellParseInstanceList | null): Entity[] {
  if (!list || list.ptr === 0) return [];
  try {
    const out: Entity[] = [];
    for (let i = 0; i < list.size(); i++) {
      const item = Entity.wrap(shell, list.get(i));
      if (item) out.push(item);
    }
    return out;
  } finally {
    list.destroy();
  }
}

function rayHits(
  shell: IfcOpenShell,
  tree: IfcOpenshellGeomTree,
  list: IfcOpenshellGeomTreeRayIntersectionList | null,
): RayHit[] {
  if (!list || list.ptr === 0) return [];
  try {
    const out: RayHit[] = [];
    const count = tree.rayIntersectionCount(list);
    for (let i = 0; i < count; i++) {
      const hit = tree.rayIntersectionAt(list, i);
      try {
        const entity = Entity.wrap(shell, hit.instance());
        if (entity) out.push({ entity });
      } finally {
        hit.destroy();
      }
    }
    return out;
  } finally {
    list.destroy();
  }
}
