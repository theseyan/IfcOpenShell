
import type { IfcOpenshellInstance } from '@ifcopenshell-js/wasm/api';
import { AttributeValue, type IfcValue } from './attribute.js';
import { IfcOpenShellError, type IfcOpenShell } from './init.js';
import { HandleGuard } from './resource.js';

/** Values accepted by {@link Entity.set}. */
export type AttributeInput =
  | null
  | boolean
  | number
  | string
  | Entity
  | Entity[]
  | number[]
  | string[];

/** Plain-object snapshot returned by {@link Entity.info}. */
export interface EntityInfo {
  id: number;
  type: string;
  attributes: Record<string, IfcValue>;
}

/** High-level wrapper for one IFC entity instance. */
export class Entity {
  private _raw: IfcOpenshellInstance | null;
  private readonly guard: HandleGuard<IfcOpenshellInstance>;
  readonly id: number;
  readonly type: string;

  private constructor(
    private readonly shell: IfcOpenShell,
    raw: IfcOpenshellInstance,
    owned = true,
  ) {
    this._raw = raw;
    this.guard = new HandleGuard(this, raw, owned);
    this.id = raw.id();
    this.type = raw.className(false);
  }

  static wrap(shell: IfcOpenShell, raw: IfcOpenshellInstance | null, owned = true): Entity | null {
    return raw && raw.ptr !== 0 ? new Entity(shell, raw, owned) : null;
  }

  get raw(): IfcOpenshellInstance {
    if (this._raw == null) throw new IfcOpenShellError('Entity has been disposed');
    return this._raw;
  }

  get typeName(): string {
    return this.type;
  }

  className(withSchema = false): string {
    return this.raw.className(withSchema);
  }

  isA(className: string): boolean {
    return this.raw.isA(className);
  }

  attribute(nameOrIndex: string | number): AttributeValue {
    const raw = typeof nameOrIndex === 'string'
      ? this.raw.getArgumentByName(nameOrIndex)
      : this.raw.getArgument(nameOrIndex);
    return new AttributeValue(this.shell, raw);
  }

  /** Read and decode an attribute by name or zero-based index. */
  get(nameOrIndex: string | number): IfcValue {
    using attribute = this.attribute(nameOrIndex);
    return attribute.value();
  }

  attributes(): string[] {
    return this.raw.getAttributeNames();
  }

  entries(): [string, IfcValue][] {
    return this.attributes().map((name) => [name, this.get(name)]);
  }

  /** Return the entity id, type, and decoded forward attributes. */
  info(): EntityInfo {
    return {
      id: this.id,
      type: this.type,
      attributes: Object.fromEntries(this.entries()),
    };
  }

  toJSON(): EntityInfo {
    return this.info();
  }

  inverseAttributes(): string[] {
    return this.raw.getInverseAttributeNames();
  }

  /** Return entities referenced by the named inverse attribute. */
  inverse(name: string): Entity[] {
    const list = this.raw.getInverse(name);
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

  attributeIndex(name: string): number {
    return this.raw.getArgumentIndex(name);
  }

  attributeName(index: number): string {
    return this.raw.getArgumentName(index);
  }

  attributeType(nameOrIndex: string | number): string {
    const index = typeof nameOrIndex === 'number' ? nameOrIndex : this.attributeIndex(nameOrIndex);
    return this.raw.getArgumentType(index);
  }

  attributeCategory(name: string): number {
    return this.raw.getAttributeCategory(name);
  }

  /** Set an attribute, inferring the native value kind from its IFC type. */
  set(nameOrIndex: string | number, value: AttributeInput, options: { type?: string } = {}): void {
    const index = typeof nameOrIndex === 'number' ? nameOrIndex : this.attributeIndex(nameOrIndex);
    setArgument(this.shell, this.raw, index, value, options.type ?? this.raw.getArgumentType(index));
  }

  /** Clear an attribute by name or zero-based index. */
  unset(nameOrIndex: string | number): void {
    if (typeof nameOrIndex === 'string') {
      this.raw.unsetAttributeValue(nameOrIndex);
      return;
    }
    this.raw.unsetArgument(nameOrIndex);
  }

  /** Serialize the entity as STEP text. */
  text(validSpf = false): string {
    return this.raw.toString(validSpf);
  }

  /** Release the native entity handle. Safe to call more than once. */
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

function setArgument(
  shell: IfcOpenShell,
  entity: IfcOpenshellInstance,
  index: number,
  value: AttributeInput,
  typeName: string,
): void {
  if (value === null) {
    entity.unsetArgument(index);
    return;
  }

  const type = typeName.toLowerCase();
  if (value instanceof Entity) {
    entity.setArgumentInstance(index, value.raw);
  } else if (typeof value === 'boolean') {
    entity.setArgumentBool(index, value);
  } else if (typeof value === 'string') {
    if (type.includes('enum') || type.includes('logical')) {
      entity.setArgumentEnumerationByName(index, value);
    } else {
      entity.setArgumentString(index, value);
    }
  } else if (typeof value === 'number') {
    if (type.includes('int') || type.includes('logical')) {
      entity.setArgumentInt32(index, value);
    } else {
      entity.setArgumentDouble(index, value);
    }
  } else if (Array.isArray(value)) {
    if (value.every((item): item is Entity => item instanceof Entity)) {
      const list = shell.raw.parse.instanceListCreateFromHandles(value.map((item) => item.raw));
      try {
        entity.setArgumentInstanceList(index, list);
      } finally {
        list.destroy();
      }
    } else if (value.every((item): item is string => typeof item === 'string')) {
      entity.setArgumentStringList(index, value);
    } else if (type.includes('int')) {
      entity.setArgumentInt32List(index, value);
    } else {
      entity.setArgumentDoubleList(index, value);
    }
  }
}
