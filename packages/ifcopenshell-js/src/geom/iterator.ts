
import type {
  IfcOpenshellFile,
  IfcOpenshellGeomElement,
  IfcOpenshellGeomIterator,
  IfcOpenshellGeomTaxonomyPoint3,
  IfcOpenshellGeomTransformation,
  IfcOpenshellGeomTriangulation,
  IfcOpenshellGeomTriangulationElement,
} from '@ifcopenshell-js/wasm/api';
import type { IfcFile } from '../file.js';
import { IfcOpenShellError, type IfcOpenShell } from '../init.js';
import { HandleGuard } from '../resource.js';
import { GeomSettings } from './settings.js';
import type { Mesh } from './mesh.js';

export type IteratorFilter =
  | { kind: 'types'; values: string[]; include?: boolean }
  | { kind: 'guids'; values: string[]; include?: boolean }
  | { kind: 'ids'; values: number[]; include?: boolean };

export interface OperationProgress {
  phase: string;
  message: string;
  ratio?: number;
  current?: number;
}

export interface IteratorMetadata {
  initialized: boolean;
  progress: number;
  hadError: boolean;
  unitName: string;
  unitMagnitude: number;
}

export interface IteratorOptions {
  kernel?: string;
  numThreads?: number;
  filter?: IteratorFilter;
}

export interface CollectOptions {
  limit?: number;
  progressInterval?: number;
  skipEmpty?: boolean;
  signal?: AbortSignal;
  onProgress?(progress: OperationProgress & { meshes: number }): void;
}

export interface CollectResult {
  meshes: Mesh[];
  truncated: boolean;
  metadata: IteratorMetadata;
}

export class GeomIterator implements AsyncIterable<Mesh> {
  private rawIter: IfcOpenshellGeomIterator | null = null;
  private guard: HandleGuard<IfcOpenshellGeomIterator> | null = null;
  private initialized = false;
  private exhausted = false;
  private disposed = false;
  private readonly ready: Promise<IfcOpenshellGeomIterator>;
  private readonly ownedSettings: GeomSettings | null;
  private settingsReleased = false;

  constructor(
    shell: IfcOpenShell,
    file: IfcFile,
    settings: GeomSettings,
    options: IteratorOptions = {},
    private readonly ownsSettings = false,
  ) {
    this.ownedSettings = ownsSettings ? settings : null;
    this.ready = createIterator(shell, file.raw, settings, options).then((raw) => {
      if (this.disposed) {
        raw.destroy();
        this.releaseSettings();
        return raw;
      }
      this.rawIter = raw;
      this.guard = new HandleGuard(this, raw, true);
      return raw;
    }, (error: unknown) => {
      this.releaseSettings();
      throw error;
    });
  }

  get raw(): IfcOpenshellGeomIterator {
    if (this.disposed || this.rawIter == null) throw new IfcOpenShellError('GeomIterator has been disposed');
    return this.rawIter;
  }

  async metadata(): Promise<IteratorMetadata> {
    const raw = await this.ready;
    if (this.disposed) throw new IfcOpenShellError('GeomIterator has been disposed');
    return {
      initialized: this.initialized,
      progress: raw.progress(),
      hadError: raw.hadErrorProcessingElements(),
      unitName: raw.unitName(),
      unitMagnitude: raw.unitMagnitude(),
    };
  }

  async initialize(): Promise<boolean> {
    if (this.disposed) throw new IfcOpenShellError('GeomIterator has been disposed');
    if (this.initialized) return true;
    const ok = (await this.ready).initialize();
    this.initialized = ok;
    return ok;
  }

  async computeBounds(withGeometry = true): Promise<void> {
    if (this.disposed) throw new IfcOpenShellError('GeomIterator has been disposed');
    (await this.ready).computeBounds(withGeometry);
  }

  async bounds(): Promise<{ min: [number, number, number] | null; max: [number, number, number] | null }> {
    const raw = await this.ready;
    if (this.disposed) throw new IfcOpenShellError('GeomIterator has been disposed');
    return { min: readPoint3(raw.boundsMin()), max: readPoint3(raw.boundsMax()) };
  }

  async boundsMin(): Promise<[number, number, number] | null> {
    return (await this.bounds()).min;
  }

  async boundsMax(): Promise<[number, number, number] | null> {
    return (await this.bounds()).max;
  }

  async nextMesh(): Promise<Mesh | null> {
    return this.nextWithOptions();
  }

  next(): Promise<Mesh | null> {
    return this.nextMesh();
  }

  async collect(options: CollectOptions = {}): Promise<CollectResult> {
    const meshes: Mesh[] = [];
    const limit = options.limit ?? Number.POSITIVE_INFINITY;
    const progressInterval = Math.max(1, options.progressInterval ?? 24);
    let seen = 0;

    while (meshes.length < limit) {
      throwIfAborted(options.signal);
      const mesh = await this.nextWithOptions(options);
      if (!mesh) break;
      seen++;
      if (!options.skipEmpty || (mesh.vertices.length > 0 && mesh.faces.length > 0)) meshes.push(mesh);
      if (seen % progressInterval === 0) {
        const metadata = await this.metadata();
        options.onProgress?.({
          phase: 'iterate',
          message: 'Iterating geometry',
          ratio: normalizeProgress(metadata.progress),
          current: meshes.length,
          meshes: meshes.length,
        });
      }
    }

    const metadata = await this.metadata();
    const truncated = meshes.length >= limit && !this.exhausted;
    options.onProgress?.({
      phase: 'done',
      message: 'Geometry iteration complete',
      ratio: truncated ? normalizeProgress(metadata.progress) : 1,
      current: meshes.length,
      meshes: meshes.length,
    });
    return { meshes, truncated, metadata };
  }

  async *[Symbol.asyncIterator](): AsyncIterableIterator<Mesh> {
    while (true) {
      const mesh = await this.nextMesh();
      if (!mesh) return;
      yield mesh;
    }
  }

  dispose(): void {
    if (this.disposed) return;
    this.disposed = true;
    if (this.rawIter == null) return;
    this.guard?.destroy();
    this.guard = null;
    this.rawIter = null;
    this.releaseSettings();
  }

  [Symbol.dispose](): void {
    this.dispose();
  }

  async [Symbol.asyncDispose](): Promise<void> {
    await this.ready.catch(() => undefined);
    this.dispose();
  }

  private releaseSettings(): void {
    if (!this.ownsSettings || this.settingsReleased) return;
    this.settingsReleased = true;
    this.ownedSettings?.dispose();
  }

  private async nextWithOptions(options: { signal?: AbortSignal } = {}): Promise<Mesh | null> {
    throwIfAborted(options.signal);
    if (this.disposed) throw new IfcOpenShellError('GeomIterator has been disposed');
    if (this.exhausted) return null;
    const raw = await this.ready;
    if (!await this.initialize()) {
      this.exhausted = true;
      return null;
    }
    const mesh = extractMesh(raw);
    this.exhausted = !raw.next();
    return mesh;
  }
}

async function createIterator(
  shell: IfcOpenShell,
  file: IfcOpenshellFile,
  settings: GeomSettings,
  options: IteratorOptions,
): Promise<IfcOpenshellGeomIterator> {
  const kernel = options.kernel ?? 'passthrough';
  await loadGeometry(shell, file, kernel);
  const raw = createFilteredIterator(shell, kernel, settings, file, options.numThreads ?? 1, options.filter);
  if (!raw || raw.ptr === 0) throw new IfcOpenShellError('Failed to create GeomIterator');
  return raw;
}

function createFilteredIterator(
  shell: IfcOpenShell,
  kernel: string,
  settings: GeomSettings,
  file: IfcOpenshellFile,
  threads: number,
  filter?: IteratorFilter,
): IfcOpenshellGeomIterator | null {
  const include = filter?.include ?? true;
  if (filter?.kind === 'types') {
    return shell.raw.geom.createIteratorWithIncludeExclude(kernel, settings.raw, file, filter.values, include, threads);
  }
  if (filter?.kind === 'guids') {
    return shell.raw.geom.createIteratorWithIncludeExcludeGlobalid(kernel, settings.raw, file, filter.values, include, threads);
  }
  if (filter?.kind === 'ids') {
    return shell.raw.geom.createIteratorWithIncludeExcludeId(kernel, settings.raw, file, filter.values, include, threads);
  }
  return shell.raw.geom.createIterator(kernel, settings.raw, file, threads);
}

export async function loadGeometry(shell: IfcOpenShell, file: IfcOpenshellFile, kernel: string): Promise<void> {
  await shell.loadPlugin('kernel', kernel);
  const schema = schemaPluginId(file.schemaName());
  if (schema) await shell.loadPlugin('mapping', schema);
}

export function schemaPluginId(schemaName: string): string | null {
  const normalized = schemaName.toLowerCase().replace(/[^a-z0-9]/g, '_');
  if (normalized.includes('ifc2x3')) return 'ifc2x3';
  if (normalized.includes('ifc4x3')) return 'ifc4x3_add2';
  if (normalized.includes('ifc4')) return 'ifc4';
  return null;
}

function extractMesh(iter: IfcOpenshellGeomIterator): Mesh | null {
  let tri: IfcOpenshellGeomTriangulationElement | null = null;
  let geom: IfcOpenshellGeomTriangulation | null = null;
  let element: IfcOpenshellGeomElement | null = null;
  let transform: IfcOpenshellGeomTransformation | null = null;
  try {
    tri = iter.getAsTriangulationElement();
    if (!tri || tri.ptr === 0) return null;
    geom = tri.geometry();
    if (!geom || geom.ptr === 0) return null;
    element = iter.get();
    if (!element || element.ptr === 0) return null;
    transform = element.transformation();
    const normals = geom.normals();
    return {
      id: element.id(),
      guid: element.guid(),
      type: element.type(),
      name: element.name(),
      vertices: new Float32Array(geom.verts() ?? []),
      faces: new Uint32Array(geom.faces() ?? []),
      normals: normals && normals.length > 0 ? new Float32Array(normals) : null,
      transform: toColumnMajorMatrix4(transform ? transform.matrix() : []),
      edges: new Uint32Array(geom.edges() ?? []),
      materialIds: new Int32Array(geom.materialIds() ?? []),
      itemIds: new Int32Array(geom.itemIds() ?? []),
      edgeItemIds: new Int32Array(geom.edgesItemIds() ?? []),
      uvs: new Float32Array(geom.uvs() ?? []),
      colors: new Float32Array(geom.colorsBuffer() ?? []),
    };
  } finally {
    release(transform);
    release(element);
    release(geom);
    release(tri);
  }
}

function readPoint3(point: IfcOpenshellGeomTaxonomyPoint3 | null): [number, number, number] | null {
  if (!point || point.ptr === 0) return null;
  try {
    const data = point.getData();
    return data && data.length >= 3 ? [data[0]!, data[1]!, data[2]!] : null;
  } finally {
    point.destroy();
  }
}

function toColumnMajorMatrix4(matrix: ArrayLike<number> | null | undefined): Float64Array {
  if (!matrix || matrix.length !== 16) return new Float64Array(matrix ?? []);
  return new Float64Array([
    matrix[0]!, matrix[4]!, matrix[8]!, matrix[12]!,
    matrix[1]!, matrix[5]!, matrix[9]!, matrix[13]!,
    matrix[2]!, matrix[6]!, matrix[10]!, matrix[14]!,
    matrix[3]!, matrix[7]!, matrix[11]!, matrix[15]!,
  ]);
}

function release(handle: { destroy(): void } | null | undefined): void {
  try {
    handle?.destroy();
  } catch {
    // ignore double destroy
  }
}

function throwIfAborted(signal: AbortSignal | undefined): void {
  if (signal?.aborted) throw new IfcOpenShellError('IfcOpenShell operation was cancelled');
}

function normalizeProgress(progress: number): number {
  if (!Number.isFinite(progress)) return 0;
  if (progress > 1) return Math.max(0, Math.min(1, progress / 100));
  return Math.max(0, Math.min(1, progress));
}
