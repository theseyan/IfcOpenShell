
import { Entity } from './entity.js';
import type { IfcOpenShell } from './init.js';

export type PsetScalar = null | boolean | number | bigint | string | Entity;

export interface TypedPsetValue {
  value: Exclude<PsetScalar, null | Entity>;
  type: string;
  unit?: Entity;
}

export type PsetValue =
  | PsetScalar
  | TypedPsetValue
  | PsetValue[]
  | { [key: string]: PsetValue };

export type PsetInput = Record<string, PsetValue>;

type RawPsetApi = {
  propsNew(): number;
  propsFree(ptr: number): void;
  propsSetNull(ptr: number, key: string): void;
  propsSetBool(ptr: number, key: string, value: boolean): void;
  propsSetInt(ptr: number, key: string, value: bigint): void;
  propsSetDouble(ptr: number, key: string, value: number): void;
  propsSetString(ptr: number, key: string, value: string): void;
  propsSetInstance(ptr: number, key: string, value: object): void;
  propsSetStringList(ptr: number, key: string, values: string[]): void;
  propsSetDoubleList(ptr: number, key: string, values: number[]): void;
  propsSetIntList(ptr: number, key: string, values: bigint[]): void;
  propsSetInstanceList(ptr: number, key: string, values: object): void;
  propsSetDict(ptr: number, key: string, child: number): void;
  propsSetTypedBool(ptr: number, key: string, value: boolean, ifcType: string): void;
  propsSetTypedDouble(ptr: number, key: string, value: number, ifcType: string): void;
  propsSetTypedInt(ptr: number, key: string, value: bigint, ifcType: string): void;
  propsSetTypedString(ptr: number, key: string, value: string, ifcType: string): void;
  propsSetUnitForLast(ptr: number, unit: object): void;
};

export class PsetProperties {
  readonly raw: number;
  readonly #shell: IfcOpenShell;
  #disposed = false;

  constructor(shell: IfcOpenShell, values: PsetInput = {}) {
    this.#shell = shell;
    this.raw = pset(shell).propsNew();
    finalizers?.register(this, { shell, raw: this.raw }, this);
    try {
      this.setMany(values);
    } catch (error) {
      this.dispose();
      throw error;
    }
  }

  static from(shell: IfcOpenShell, value: PsetProperties | PsetInput): PsetProperties {
    return value instanceof PsetProperties ? value : new PsetProperties(shell, value);
  }

  setMany(values: PsetInput): this {
    for (const [key, value] of Object.entries(values)) this.set(key, value);
    return this;
  }

  set(key: string, value: PsetValue): this {
    writeValue(this.#shell, this.raw, key, value);
    return this;
  }

  dispose(): void {
    if (this.#disposed) return;
    this.#disposed = true;
    finalizers?.unregister(this);
    pset(this.#shell).propsFree(this.raw);
  }

  [Symbol.dispose](): void {
    this.dispose();
  }

  async [Symbol.asyncDispose](): Promise<void> {
    this.dispose();
  }
}

export function toRawPsetProperties(
  shell: IfcOpenShell,
  value: PsetProperties | PsetInput,
  temps: Array<{ destroy(): void }>,
): number {
  if (value instanceof PsetProperties) return value.raw;
  const props = new PsetProperties(shell, value);
  temps.push({ destroy: () => props.dispose() });
  return props.raw;
}

function writeValue(shell: IfcOpenShell, ptr: number, key: string, value: PsetValue): void {
  const api = pset(shell);
  if (isTyped(value)) {
    writeTyped(api, ptr, key, value);
  } else if (value === null) {
    api.propsSetNull(ptr, key);
  } else if (value instanceof Entity) {
    api.propsSetInstance(ptr, key, value.raw);
  } else if (Array.isArray(value)) {
    writeList(shell, api, ptr, key, value);
  } else if (isPlainObject(value)) {
    using child = new PsetProperties(shell, value as PsetInput);
    api.propsSetDict(ptr, key, child.raw);
  } else if (typeof value === 'boolean') {
    api.propsSetBool(ptr, key, value);
  } else if (typeof value === 'bigint') {
    api.propsSetInt(ptr, key, value);
  } else if (typeof value === 'number') {
    api.propsSetDouble(ptr, key, value);
  } else {
    api.propsSetString(ptr, key, value);
  }
}

function writeTyped(api: RawPsetApi, ptr: number, key: string, value: TypedPsetValue): void {
  if (typeof value.value === 'boolean') api.propsSetTypedBool(ptr, key, value.value, value.type);
  else if (typeof value.value === 'bigint') api.propsSetTypedInt(ptr, key, value.value, value.type);
  else if (typeof value.value === 'number') api.propsSetTypedDouble(ptr, key, value.value, value.type);
  else api.propsSetTypedString(ptr, key, value.value, value.type);
  if (value.unit) api.propsSetUnitForLast(ptr, value.unit.raw);
}

function writeList(shell: IfcOpenShell, api: RawPsetApi, ptr: number, key: string, values: PsetValue[]): void {
  if (values.every((item): item is string => typeof item === 'string')) api.propsSetStringList(ptr, key, values);
  else if (values.every((item): item is number => typeof item === 'number')) api.propsSetDoubleList(ptr, key, values);
  else if (values.every((item): item is bigint => typeof item === 'bigint')) api.propsSetIntList(ptr, key, values);
  else if (values.every((item): item is Entity => item instanceof Entity)) {
    const list = shell.raw.parse.instanceListCreateFromHandles(values.map((item) => item.raw));
    try {
      api.propsSetInstanceList(ptr, key, list);
    } finally {
      list.destroy();
    }
  } else {
    throw new TypeError(`Unsupported property list for ${key}`);
  }
}

function isTyped(value: PsetValue): value is TypedPsetValue {
  return isPlainObject(value) && 'value' in value && typeof value.type === 'string';
}

function isPlainObject(value: PsetValue): value is Record<string, PsetValue> {
  return typeof value === 'object' && value !== null && Object.getPrototypeOf(value) === Object.prototype;
}

function pset(shell: IfcOpenShell): RawPsetApi {
  return shell.raw.pset as object as RawPsetApi;
}

const finalizers = typeof FinalizationRegistry === 'function'
  ? new FinalizationRegistry<{ shell: IfcOpenShell; raw: number }>(({ shell, raw }) => {
    try {
      pset(shell).propsFree(raw);
    } catch {
      // Finalizers may run during runtime teardown; explicit dispose reports errors earlier.
    }
  })
  : null;
