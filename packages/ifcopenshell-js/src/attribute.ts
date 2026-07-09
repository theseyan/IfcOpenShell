
import type { IfcOpenshellParseAttributeValue } from '@ifcopenshell-js/wasm/api';
import { Entity } from './entity.js';
import { IfcOpenShellError, type IfcOpenShell } from './init.js';
import { HandleGuard } from './resource.js';

export type IfcValue =
  | null
  | boolean
  | number
  | string
  | Entity
  | Entity[]
  | number[]
  | string[];

export class AttributeValue {
  private _raw: IfcOpenshellParseAttributeValue | null;
  private readonly guard: HandleGuard<IfcOpenshellParseAttributeValue>;
  readonly type: string;
  readonly isNull: boolean;

  constructor(
    private readonly shell: IfcOpenShell,
    raw: IfcOpenshellParseAttributeValue,
    owned = true,
  ) {
    this._raw = raw;
    this.guard = new HandleGuard(this, raw, owned);
    this.type = raw.type();
    this.isNull = raw.isNull();
  }

  get raw(): IfcOpenshellParseAttributeValue {
    if (this._raw == null) throw new IfcOpenShellError('AttributeValue has been disposed');
    return this._raw;
  }

  string(): string {
    return this.raw.asString();
  }

  number(): number {
    return this.raw.asDouble();
  }

  integer(): number {
    return this.raw.asInt32();
  }

  boolean(): boolean {
    return this.raw.asBool();
  }

  entity(): Entity | null {
    return Entity.wrap(this.shell, this.raw.asInstance());
  }

  entities(): Entity[] {
    const list = this.raw.asInstanceList();
    try {
      const out: Entity[] = [];
      for (let i = 0; i < list.size(); i++) {
        const item = Entity.wrap(this.shell, list.get(i));
        if (item) out.push(item);
      }
      return out;
    } finally {
      list.destroy();
    }
  }

  strings(): string[] {
    return this.raw.asStringList();
  }

  numbers(): number[] {
    return this.raw.asDoubleList();
  }

  integers(): number[] {
    return this.raw.asInt32List();
  }

  enumeration(): string {
    return this.raw.asEnumerationValue();
  }

  enumerationIndex(): number {
    return this.raw.asEnumerationIndex();
  }

  value(): IfcValue {
    if (this.raw.isNull()) return null;
    const type = this.type.toLowerCase().replace(/[\s_-]+/g, '');
    const aggregate = type.includes('list') || type.includes('aggregate') || type.includes('vector');
    if (type.includes('instance') && aggregate) return this.entities();
    if (type.includes('instance') || type.includes('entity')) return this.entity();
    if (type.includes('enum')) return this.enumeration();
    if (type.includes('bool') || type.includes('logical')) return this.boolean();
    if (type.includes('string') && aggregate) return this.strings();
    if (type.includes('int') && aggregate) return this.integers();
    if ((type.includes('double') || type.includes('float') || type.includes('real')) && aggregate) return this.numbers();
    if (type.includes('int')) return this.integer();
    if (type.includes('double') || type.includes('float') || type.includes('real') || type.includes('number')) return this.number();
    try {
      return this.string();
    } catch {
      return this.type;
    }
  }

  size(): number {
    return this.raw.size();
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
