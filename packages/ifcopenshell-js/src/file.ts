
import type { IfcOpenshellFile } from '@ifcopenshell-js/wasm/api';
import { Entity } from './entity.js';
import { GeomIterator, type IteratorOptions } from './geom/iterator.js';
import { GeomSettings } from './geom/settings.js';
import { GeometryTree } from './geom/tree.js';
import { IfcOpenShellError, type IfcOpenShell } from './init.js';
import { HandleGuard } from './resource.js';
import { inspectEntity, type EntityInfo } from './util/inspect.js';

export interface OpenOptions {
  signal?: AbortSignal;
  readonly?: boolean;
}

export interface HeaderInfo {
  description: string[];
  implementationLevel: string;
  name: string;
  timeStamp: string;
  author: string[];
  organization: string[];
  preprocessorVersion: string;
  originatingSystem: string;
  authorization: string;
  schemas: string[];
}

export interface FileInfo {
  schema: string;
  ids: number[];
  types: string[];
  entityCount: number;
  maxId: number;
  good: number;
  storageMode: number;
  header: HeaderInfo | null;
}

export class IfcFile {
  private _raw: IfcOpenshellFile | null;
  private readonly guard: HandleGuard<IfcOpenshellFile>;

  private constructor(
    private readonly _shell: IfcOpenShell,
    raw: IfcOpenshellFile,
    owned = true,
  ) {
    this._raw = raw;
    this.guard = new HandleGuard(this, raw, owned);
  }

  static async open(
    shell: IfcOpenShell,
    bytes: Uint8Array | ArrayBuffer,
    filename?: string,
    options: OpenOptions = {},
  ): Promise<IfcFile> {
    if (options.signal?.aborted) {
      throw new IfcOpenShellError('Opening IFC file was aborted');
    }
    const raw = shell.raw.parse.openBytes(bytes, filename, options.readonly ?? false);
    if (!raw || raw.ptr === 0) throw new IfcOpenShellError('Failed to open IFC file');
    return new IfcFile(shell, raw);
  }

  static async createEmpty(shell: IfcOpenShell, schema: string): Promise<IfcFile> {
    return IfcFile.create(shell, schema);
  }

  static async create(shell: IfcOpenShell, schema: string): Promise<IfcFile> {
    const raw = shell.raw.parse.newFile(schema, 0, '');
    if (!raw || raw.ptr === 0) throw new IfcOpenShellError(`Failed to create ${schema} file`);
    return new IfcFile(shell, raw);
  }

  static wrap(shell: IfcOpenShell, raw: IfcOpenshellFile | null, owned = false): IfcFile | null {
    return raw && raw.ptr !== 0 ? new IfcFile(shell, raw, owned) : null;
  }

  get shell(): IfcOpenShell {
    return this._shell;
  }

  get raw(): IfcOpenshellFile {
    if (this._raw == null) throw new IfcOpenShellError('IfcFile has been disposed');
    return this._raw;
  }

  get schemaName(): string {
    return this.raw.schemaName();
  }

  get schema(): string {
    return this.schemaName;
  }

  get maxId(): number {
    return this.raw.getMaxId();
  }

  get ids(): number[] {
    return this.raw.entityNames();
  }

  get types(): string[] {
    return this.raw.types();
  }

  get entityCount(): number {
    return this.ids.length;
  }

  get isValid(): boolean {
    return this.raw.good() !== 0;
  }

  get(id: number): Entity | null {
    return Entity.wrap(this._shell, catchNull(() => this.raw.byId(id)));
  }

  find(guid: string): Entity | null {
    return Entity.wrap(this._shell, catchNull(() => this.raw.byGuid(guid)));
  }

  all(typeName: string, options: { includeSubtypes?: boolean } = {}): Entity[] {
    const list = options.includeSubtypes === false
      ? this.raw.byTypeExclSubtypes(typeName)
      : this.raw.byType(typeName);
    try {
      const out: Entity[] = [];
      for (let i = 0; i < list.size(); i++) {
        const item = Entity.wrap(this._shell, list.get(i));
        if (item) out.push(item);
      }
      return out;
    } finally {
      list.destroy();
    }
  }

  create(ifcClass: string, options: { predefinedType?: string | null; name?: string | null } = {}): Entity {
    return this._shell.api.root.createEntity(this, {
      ifcClass,
      predefinedType: options.predefinedType ?? undefined,
      name: options.name ?? undefined,
    });
  }

  text(): string {
    return this.raw.toString();
  }

  info(): FileInfo {
    const ids = this.ids;
    return {
      schema: this.schemaName,
      ids,
      types: this.types,
      entityCount: ids.length,
      maxId: this.raw.getMaxId(),
      good: this.raw.good(),
      storageMode: this.raw.storageMode(),
      header: this.header(),
    };
  }

  header(): HeaderInfo | null {
    const header = this.raw.header();
    if (!header || header.ptr === 0) return null;
    try {
      const description = header.fileDescription();
      const name = header.fileName();
      const schema = header.fileSchema();
      try {
        return {
          description: description.description(),
          implementationLevel: description.implementationLevel(),
          name: name.name(),
          timeStamp: name.timeStamp(),
          author: name.author(),
          organization: name.organization(),
          preprocessorVersion: name.preprocessorVersion(),
          originatingSystem: name.originatingSystem(),
          authorization: name.authorization(),
          schemas: schema.schemaIdentifiers(),
        };
      } finally {
        schema.destroy();
        name.destroy();
        description.destroy();
      }
    } finally {
      header.destroy();
    }
  }

  status(): number {
    return this.raw.good();
  }

  storageMode(): number {
    return this.raw.storageMode();
  }

  unit(unitType: string): number {
    return this.raw.getUnit(unitType);
  }

  totalInverses(entity: Entity): number {
    return this.raw.getTotalInverses(entity.raw);
  }

  inverses(entity: Entity): Entity[] {
    const list = this.raw.getInverse(entity.raw);
    try {
      const out: Entity[] = [];
      for (let i = 0; i < list.size(); i++) {
        const item = Entity.wrap(this._shell, list.get(i));
        if (item) out.push(item);
      }
      return out;
    } finally {
      list.destroy();
    }
  }

  inverseIndices(entity: Entity): number[] {
    return this.raw.getInverseIndices(entity.raw);
  }

  traverse(entity: Entity, options: { maxDepth?: number; breadthFirst?: boolean } = {}): Entity[] {
    const maxDepth = options.maxDepth ?? -1;
    const list = options.breadthFirst
      ? this.raw.traverseBreadthFirst(entity.raw, maxDepth)
      : this.raw.traverse(entity.raw, maxDepth);
    try {
      const out: Entity[] = [];
      for (let i = 0; i < list.size(); i++) {
        const item = Entity.wrap(this._shell, list.get(i));
        if (item) out.push(item);
      }
      return out;
    } finally {
      list.destroy();
    }
  }

  meshes(settings?: GeomSettings, options?: IteratorOptions): GeomIterator {
    const ownedSettings = settings === undefined;
    return new GeomIterator(this._shell, this, settings ?? new GeomSettings(this._shell), options, ownedSettings);
  }

  tree(settings?: GeomSettings): GeometryTree {
    return GeometryTree.fromFile(this._shell, this, settings);
  }

  async bounds(settings?: GeomSettings, options?: IteratorOptions): Promise<{
    min: [number, number, number] | null;
    max: [number, number, number] | null;
  }> {
    await using iterator = this.meshes(settings, options);
    await iterator.computeBounds(true);
    return iterator.bounds();
  }

  inspect(id: number): Promise<EntityInfo | null> {
    return inspectEntity(this, id);
  }

  dispose(): void {
    if (this._raw == null) return;
    this.guard.destroy();
    this._raw = null;
  }

  [Symbol.dispose](): void {
    this.dispose();
  }

  async [Symbol.asyncDispose](): Promise<void> {
    this.dispose();
  }
}

function catchNull<T>(fn: () => T): T | null {
  try {
    const value = fn();
    return value && typeof value === 'object' && 'ptr' in value && value.ptr === 0 ? null : value;
  } catch {
    return null;
  }
}
