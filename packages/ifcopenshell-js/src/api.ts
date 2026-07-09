
import type {
  IfcOpenshellParseInstanceList,
  IfcOpenshellValue,
} from '@ifcopenshell-js/wasm/api';
import { Entity } from './entity.js';
import { IfcFile } from './file.js';
import type { IfcOpenShell } from './init.js';
import { HandleGuard } from './resource.js';

export interface DisposableHandle {
  destroy(): void;
}

export type ApiData =
  | null
  | boolean
  | number
  | bigint
  | string
  | IfcFile
  | Entity
  | Value
  | ApiData[]
  | { [key: string]: ApiData };

export type ValueData = null | boolean | number | bigint | string | Entity | ValueData[] | { [key: string]: ValueData };

export type ValueInput = ValueData | Value;

const enum ValueKind {
  None = 0,
  Bool = 1,
  Int = 2,
  Double = 3,
  String = 4,
  Entity = 5,
  List = 6,
  Dict = 7,
}

export class InstanceList {
  private _raw: IfcOpenshellParseInstanceList | null;
  private readonly guard: HandleGuard<IfcOpenshellParseInstanceList>;

  constructor(
    private readonly shell: IfcOpenShell,
    raw: IfcOpenshellParseInstanceList,
    owned = true,
  ) {
    this._raw = raw;
    this.guard = new HandleGuard(this, raw, owned);
  }

  get raw(): IfcOpenshellParseInstanceList {
    if (this._raw == null) throw new Error('InstanceList has been disposed');
    return this._raw;
  }

  get length(): number {
    return this.raw.size();
  }

  size(): number {
    return this.length;
  }

  get(index: number): Entity | null {
    return Entity.wrap(this.shell, this.raw.get(index));
  }

  items(): Entity[] {
    const out: Entity[] = [];
    for (let i = 0; i < this.length; i++) {
      const item = this.get(i);
      if (item) out.push(item);
    }
    return out;
  }

  *[Symbol.iterator](): IterableIterator<Entity> {
    for (const item of this.items()) yield item;
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

export class Value {
  private _raw: IfcOpenshellValue | null;
  private readonly guard: HandleGuard<IfcOpenshellValue>;

  constructor(
    private readonly shell: IfcOpenShell,
    raw: IfcOpenshellValue,
    owned = true,
  ) {
    this._raw = raw;
    this.guard = new HandleGuard(this, raw, owned);
  }

  get raw(): IfcOpenshellValue {
    if (this._raw == null) throw new Error('Value has been disposed');
    return this._raw;
  }

  get kind(): number {
    return this.shell.raw.value.kind(this.raw);
  }

  asBool(): boolean {
    return this.shell.raw.value.asBool(this.raw);
  }

  asNumber(): number {
    return this.shell.raw.value.asDouble(this.raw);
  }

  asInt(): bigint {
    return this.shell.raw.value.asInt64(this.raw);
  }

  asString(): string {
    return this.shell.raw.value.asString(this.raw);
  }

  asEntity(): Entity {
    return Entity.wrap(this.shell, this.shell.raw.value.asInstance(this.raw))!;
  }

  listSize(): number {
    return this.shell.raw.value.listSize(this.raw);
  }

  listAt(index: number): Value | null {
    return wrapValue(this.shell, this.shell.raw.value.listAt(this.raw, index));
  }

  dictSize(): number {
    return this.shell.raw.value.dictSize(this.raw);
  }

  dictKeyAt(index: number): string {
    return this.shell.raw.value.dictKeyAt(this.raw, index);
  }

  dictValueAt(index: number): Value | null {
    return wrapValue(this.shell, this.shell.raw.value.dictValueAt(this.raw, index));
  }

  value(): ValueData {
    switch (this.kind) {
      case ValueKind.None:
        return null;
      case ValueKind.Bool:
        return this.asBool();
      case ValueKind.Int:
        return intData(this.asInt());
      case ValueKind.Double:
        return this.asNumber();
      case ValueKind.String:
        return this.asString();
      case ValueKind.Entity:
        return this.asEntity();
      case ValueKind.List:
        return this.list();
      case ValueKind.Dict:
        return this.dict();
      default:
        return this.asString();
    }
  }

  list(): ValueData[] {
    const out: ValueData[] = [];
    for (let i = 0; i < this.listSize(); i++) {
      const item = this.listAt(i);
      try {
        out.push(item?.value() ?? null);
      } finally {
        item?.dispose();
      }
    }
    return out;
  }

  dict(): Record<string, ValueData> {
    const out: Record<string, ValueData> = {};
    for (let i = 0; i < this.dictSize(); i++) {
      const item = this.dictValueAt(i);
      try {
        out[this.dictKeyAt(i)] = item?.value() ?? null;
      } finally {
        item?.dispose();
      }
    }
    return out;
  }

  toJSON(): ValueData {
    return this.value();
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

export function wrapList(shell: IfcOpenShell, raw: IfcOpenshellParseInstanceList | null): InstanceList | null {
  return raw && raw.ptr !== 0 ? new InstanceList(shell, raw) : null;
}

export function wrapValue(shell: IfcOpenShell, raw: IfcOpenshellValue | null): Value | null {
  return raw && raw.ptr !== 0 ? new Value(shell, raw) : null;
}

export function toRawValue(shell: IfcOpenShell, value: ValueInput, temps: DisposableHandle[]): IfcOpenshellValue {
  if (value instanceof Value) return value.raw;

  const raw = createRawValue(shell, value, temps);
  if (!raw || raw.ptr === 0) throw new Error('Failed to create native value');
  temps.push(raw);
  return raw;
}

export function fromRawValue(shell: IfcOpenShell, raw: IfcOpenshellValue | null): ValueData | null {
  const value = wrapValue(shell, raw);
  if (!value) return null;
  try {
    return value.value();
  } finally {
    value.dispose();
  }
}

function createRawValue(shell: IfcOpenShell, value: ValueData, temps: DisposableHandle[]): IfcOpenshellValue | null {
  if (value === null) return shell.raw.value.newNone();
  if (value instanceof Entity) return shell.raw.value.newInstance(value.raw);

  switch (typeof value) {
    case 'boolean':
      return shell.raw.value.newBool(value);
    case 'bigint':
      return shell.raw.value.newInt(value);
    case 'number':
      return Number.isInteger(value) ? shell.raw.value.newInt(BigInt(value)) : shell.raw.value.newDouble(value);
    case 'string':
      return shell.raw.value.newString(value);
    case 'object':
      if (Array.isArray(value)) return createRawList(shell, value, temps);
      return createRawDict(shell, value, temps);
    default:
      throw new TypeError(`Unsupported selector value type: ${typeof value}`);
  }
}

function intData(value: bigint): number | bigint {
  return value >= BigInt(Number.MIN_SAFE_INTEGER) && value <= BigInt(Number.MAX_SAFE_INTEGER)
    ? Number(value)
    : value;
}

function createRawList(shell: IfcOpenShell, values: ValueData[], temps: DisposableHandle[]): IfcOpenshellValue | null {
  const list = shell.raw.value.newList();
  if (!list || list.ptr === 0) return list;
  temps.push(list);

  for (const item of values) {
    const rawItem = createRawValue(shell, item, temps);
    if (!rawItem || rawItem.ptr === 0) throw new Error('Failed to create native list item');
    temps.push(rawItem);
    if (!shell.raw.value.listAppend(list, rawItem)) throw new Error('Failed to append native list item');
  }

  return list;
}

function createRawDict(
  shell: IfcOpenShell,
  values: Record<string, ValueData>,
  temps: DisposableHandle[],
): IfcOpenshellValue | null {
  const dict = shell.raw.value.newDict();
  if (!dict || dict.ptr === 0) return dict;
  temps.push(dict);

  for (const [key, value] of Object.entries(values)) {
    const rawValue = createRawValue(shell, value, temps);
    if (!rawValue || rawValue.ptr === 0) throw new Error(`Failed to create native dict value for "${key}"`);
    temps.push(rawValue);
    if (!shell.raw.value.dictSet(dict, key, rawValue)) throw new Error(`Failed to set native dict value for "${key}"`);
  }

  return dict;
}
