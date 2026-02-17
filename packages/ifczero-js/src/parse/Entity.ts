import type { EmscriptenModule } from "../wasm/types.js";
import * as bind from "./bindings.generated.js";
import { ArgumentType, type Ptr } from "./types.js";

interface FlattenedMatrix<T> {
  values: T[];
  rowOffsets: number[];
}

function flattenMatrix<T>(rows: T[][]): FlattenedMatrix<T> {
  const values: T[] = [];
  const rowOffsets: number[] = [0];
  for (const row of rows) {
    for (const value of row) values.push(value);
    rowOffsets.push(values.length);
  }
  return { values, rowOffsets };
}

export class Entity {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  static walk<T>(predicate: (v: unknown) => boolean, transform: (v: unknown) => T, value: unknown): unknown {
    if (Array.isArray(value)) {
      return value.map(item => Entity.walk(predicate, transform, item));
    } else if (predicate(value)) {
      return transform(value);
    }
    return value;
  }

  get id(): number {
    return bind.entity_id(this.M, this.ptr);
  }

  get typeName(): string {
    return bind.entity_type_name(this.M, this.ptr);
  }

  get typeNameWithSchema(): string {
    return bind.entity_type_name_with_schema(this.M, this.ptr);
  }

  isA(typeName: string): boolean {
    return bind.entity_is_a(this.M, this.ptr, typeName) !== 0;
  }

  toString(validSpf = 0): string {
    return bind.entity_to_string(this.M, this.ptr, validSpf);
  }

  get attributeCount(): number {
    return bind.entity_argument_count(this.M, this.ptr);
  }

  attributeIndex(name: string): number {
    return bind.entity_argument_index(this.M, this.ptr, name);
  }

  attributeName(index: number): string {
    return bind.entity_argument_name(this.M, this.ptr, index);
  }

  attributeNames(): string[] {
    const listPtr = bind.entity_attribute_names(this.M, this.ptr);
    if (!listPtr) return [];
    return this._readStringList(listPtr);
  }

  inverseAttributeNames(): string[] {
    const listPtr = bind.entity_inverse_attribute_names(this.M, this.ptr);
    if (!listPtr) return [];
    return this._readStringList(listPtr);
  }

  attributeCategory(name: string): number {
    return bind.entity_attribute_category(this.M, this.ptr, name);
  }

  argumentType(index: number): ArgumentType {
    return bind.entity_argument_value_type(this.M, this.ptr, index);
  }

  argumentDeclaredType(index: number): ArgumentType {
    return bind.entity_argument_declared_type(this.M, this.ptr, index);
  }

  isNull(index: number): boolean {
    return bind.entity_argument_is_null(this.M, this.ptr, index) !== 0;
  }

  getInt(index: number): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.entity_get_argument_as_int(this.M, this.ptr, index, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") : null;
    this.M.stackRestore(sp);
    return val;
  }

  getBool(index: number): boolean | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.entity_get_argument_as_bool(this.M, this.ptr, index, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") !== 0 : null;
    this.M.stackRestore(sp);
    return val;
  }

  getLogical(index: number): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(4);
    const ok = bind.entity_get_argument_as_logical(this.M, this.ptr, index, outPtr);
    const val = ok ? this.M.getValue(outPtr, "i32") : null;
    this.M.stackRestore(sp);
    return val;
  }

  getDouble(index: number): number | null {
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(8);
    const ok = bind.entity_get_argument_as_double(
      this.M,
      this.ptr,
      index,
      outPtr,
    );
    const val = ok ? this.M.getValue(outPtr, "double") : null;
    this.M.stackRestore(sp);
    return val;
  }

  getString(index: number): string | null {
    const result = bind.entity_get_argument_as_string(
      this.M,
      this.ptr,
      index,
    );
    return result || null;
  }

  getEntity(index: number): Entity | null {
    const ptr = bind.entity_get_argument_as_entity(this.M, this.ptr, index);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  getEntityList(index: number): Entity[] {
    const listPtr = bind.entity_get_argument_as_entity_list(
      this.M,
      this.ptr,
      index,
    );
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  getDoubleList(index: number): number[] {
    const listPtr = bind.entity_get_argument_as_double_list(
      this.M,
      this.ptr,
      index,
    );
    if (!listPtr) return [];
    const count = bind.double_list_count(this.M, listPtr);
    const result: number[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.double_list_get(this.M, listPtr, i));
    }
    bind.double_list_close(this.M, listPtr);
    return result;
  }

  getIntList(index: number): number[] {
    const listPtr = bind.entity_get_argument_as_int_list(
      this.M,
      this.ptr,
      index,
    );
    if (!listPtr) return [];
    const count = bind.int_list_count(this.M, listPtr);
    const result: number[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.int_list_get(this.M, listPtr, i));
    }
    bind.int_list_close(this.M, listPtr);
    return result;
  }

  getStringList(index: number): string[] {
    const listPtr = bind.entity_get_argument_as_string_list(
      this.M,
      this.ptr,
      index,
    );
    if (!listPtr) return [];
    return this._readStringList(listPtr);
  }

  getIntMatrix(index: number): number[][] {
    const matPtr = bind.entity_get_argument_as_int_matrix(this.M, this.ptr, index);
    if (!matPtr) return [];
    const rows = bind.int_matrix_row_count(this.M, matPtr);
    const result: number[][] = [];
    for (let r = 0; r < rows; r++) {
      const cols = bind.int_matrix_col_count(this.M, matPtr, r);
      const row: number[] = [];
      for (let c = 0; c < cols; c++) {
        row.push(bind.int_matrix_get(this.M, matPtr, r, c));
      }
      result.push(row);
    }
    bind.int_matrix_close(this.M, matPtr);
    return result;
  }

  getDoubleMatrix(index: number): number[][] {
    const matPtr = bind.entity_get_argument_as_double_matrix(this.M, this.ptr, index);
    if (!matPtr) return [];
    const rows = bind.double_matrix_row_count(this.M, matPtr);
    const result: number[][] = [];
    for (let r = 0; r < rows; r++) {
      const cols = bind.double_matrix_col_count(this.M, matPtr, r);
      const row: number[] = [];
      for (let c = 0; c < cols; c++) {
        row.push(bind.double_matrix_get(this.M, matPtr, r, c));
      }
      result.push(row);
    }
    bind.double_matrix_close(this.M, matPtr);
    return result;
  }

  getEntityMatrix(index: number): Entity[][] {
    const matPtr = bind.entity_get_argument_as_entity_matrix(this.M, this.ptr, index);
    if (!matPtr) return [];
    const rows = bind.entity_matrix_row_count(this.M, matPtr);
    const result: Entity[][] = [];
    for (let r = 0; r < rows; r++) {
      const cols = bind.entity_matrix_col_count(this.M, matPtr, r);
      const row: Entity[] = [];
      for (let c = 0; c < cols; c++) {
        const ePtr = bind.entity_matrix_get(this.M, matPtr, r, c);
        if (ePtr) row.push(new Entity(this.M, ePtr));
      }
      result.push(row);
    }
    bind.entity_matrix_close(this.M, matPtr);
    return result;
  }

  getInverse(inverseName: string): Entity[] {
    const listPtr = bind.entity_get_inverse(this.M, this.ptr, inverseName);
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  getInfo(options?: { recursive?: boolean; includeId?: boolean }): Record<string, unknown> {
    const recursive = options?.recursive ?? false;
    const includeId = options?.includeId ?? true;
    const seen = new Set<number>();
    return this._getInfoImpl(recursive, includeId, seen);
  }

  private _getInfoImpl(recursive: boolean, includeId: boolean, seen: Set<number>): Record<string, unknown> {
    const result: Record<string, unknown> = {};
    if (includeId) result.id = this.id;
    result.type = this.typeName;

    const count = this.attributeCount;
    for (let i = 0; i < count; i++) {
      const name = this.attributeName(i);
      if (this.isNull(i)) {
        result[name] = null;
        continue;
      }
      const argType = this.argumentType(i);
      result[name] = this._getValueByType(i, argType, recursive, includeId, seen);
    }
    return result;
  }

  private _getValueByType(
    index: number,
    argType: ArgumentType,
    recursive: boolean,
    includeId: boolean,
    seen: Set<number>,
  ): unknown {
    switch (argType) {
      case ArgumentType.INT:
        return this.getInt(index);
      case ArgumentType.BOOL:
        return this.getBool(index);
      case ArgumentType.LOGICAL:
        return this.getLogical(index);
      case ArgumentType.DOUBLE:
        return this.getDouble(index);
      case ArgumentType.STRING:
      case ArgumentType.BINARY:
      case ArgumentType.ENUMERATION:
        return this.getString(index);
      case ArgumentType.ENTITY_INSTANCE: {
        const e = this.getEntity(index);
        if (!e) return null;
        if (recursive && !seen.has(e.id)) {
          seen.add(e.id);
          return e._getInfoImpl(recursive, includeId, seen);
        }
        return e;
      }
      case ArgumentType.EMPTY_AGGREGATE:
      case ArgumentType.AGGREGATE_OF_EMPTY_AGGREGATE:
        return [];
      case ArgumentType.AGGREGATE_OF_INT:
        return this.getIntList(index);
      case ArgumentType.AGGREGATE_OF_DOUBLE:
        return this.getDoubleList(index);
      case ArgumentType.AGGREGATE_OF_STRING:
      case ArgumentType.AGGREGATE_OF_BINARY:
        return this.getStringList(index);
      case ArgumentType.AGGREGATE_OF_ENTITY_INSTANCE: {
        const entities = this.getEntityList(index);
        if (recursive) {
          return entities.map(e => {
            if (seen.has(e.id)) return e;
            seen.add(e.id);
            return e._getInfoImpl(recursive, includeId, seen);
          });
        }
        return entities;
      }
      case ArgumentType.AGGREGATE_OF_AGGREGATE_OF_INT:
        return this.getIntMatrix(index);
      case ArgumentType.AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
        return this.getDoubleMatrix(index);
      case ArgumentType.AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
        const mat = this.getEntityMatrix(index);
        if (recursive) {
          return mat.map(row => row.map(e => {
            if (seen.has(e.id)) return e;
            seen.add(e.id);
            return e._getInfoImpl(recursive, includeId, seen);
          }));
        }
        return mat;
      }
      default: return null;
    }
  }

  withAttributes(): this & Record<string, unknown> {
    return new Proxy(this, {
      get(target, prop, receiver) {
        if (typeof prop === "string" && !(prop in target)) {
          const FORWARD = 1;
          const INVERSE = 2;
          const cat = target.attributeCategory(prop);
          if (cat === FORWARD) {
            const idx = target.attributeIndex(prop);
            if (idx < 0) return undefined;
            const argType = target.argumentType(idx);
            if (target.isNull(idx)) return null;
            return target._getValueByType(idx, argType, false, true, new Set());
          } else if (cat === INVERSE) {
            return target.getInverse(prop);
          }
          return undefined;
        }
        return Reflect.get(target, prop, receiver);
      },
      set(target, prop, value, receiver) {
        if (typeof prop === "string" && !(prop in target)) {
          const FORWARD = 1;
          const cat = target.attributeCategory(prop);
          if (cat === FORWARD) {
            const idx = target.attributeIndex(prop);
            if (idx < 0) return false;
            if (value === null) return target.setNull(idx);
            if (typeof value === "number") {
              const argType = target.argumentType(idx);
              if (argType === ArgumentType.INT) return target.setInt(idx, value);
              if (argType === ArgumentType.LOGICAL) return target.setLogical(idx, value);
              if (argType === ArgumentType.DOUBLE) return target.setDouble(idx, value);
            }
            if (typeof value === "boolean") return target.setBool(idx, value);
            if (typeof value === "string") return target.setString(idx, value);
            if (value instanceof Entity) return target.setEntity(idx, value);
            return false;
          }
        }
        return Reflect.set(target, prop, value, receiver);
      },
    }) as this & Record<string, unknown>;
  }

  // Setters
  setInt(index: number, value: number): boolean {
    return bind.entity_set_argument_int(this.M, this.ptr, index, value) !== 0;
  }

  setBool(index: number, value: boolean): boolean {
    return (
      bind.entity_set_argument_bool(this.M, this.ptr, index, value ? 1 : 0) !==
      0
    );
  }

  setLogical(index: number, value: number): boolean {
    return bind.entity_set_argument_logical(this.M, this.ptr, index, value) !== 0;
  }

  setDouble(index: number, value: number): boolean {
    return (
      bind.entity_set_argument_double(this.M, this.ptr, index, value) !== 0
    );
  }

  setString(index: number, value: string): boolean {
    return (
      bind.entity_set_argument_string(this.M, this.ptr, index, value) !== 0
    );
  }

  setNull(index: number): boolean {
    return bind.entity_set_argument_null(this.M, this.ptr, index) !== 0;
  }

  unsetArgument(index: number): boolean {
    return bind.entity_unset_argument(this.M, this.ptr, index) !== 0;
  }

  setEntity(index: number, entity: Entity): boolean {
    return (
      bind.entity_set_argument_entity(this.M, this.ptr, index, entity.ptr) !== 0
    );
  }

  setIntList(index: number, values: number[]): boolean {
    const sp = this.M.stackSave();
    const buf = this.M.stackAlloc(values.length * 4);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(buf + i * 4, values[i], "i32");
    }
    const ok = bind.entity_set_argument_int_list(this.M, this.ptr, index, buf, values.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setDoubleList(index: number, values: number[]): boolean {
    const sp = this.M.stackSave();
    const buf = this.M.stackAlloc(values.length * 8);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(buf + i * 8, values[i], "double");
    }
    const ok = bind.entity_set_argument_double_list(this.M, this.ptr, index, buf, values.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setStringList(index: number, values: string[]): boolean {
    const ptrs: number[] = [];
    for (const s of values) {
      const len = this.M.lengthBytesUTF8(s) + 1;
      const p = this.M._malloc(len);
      this.M.stringToUTF8(s, p, len);
      ptrs.push(p);
    }
    const sp = this.M.stackSave();
    const buf = this.M.stackAlloc(ptrs.length * 4);
    for (let i = 0; i < ptrs.length; i++) {
      this.M.setValue(buf + i * 4, ptrs[i], "i32");
    }
    const ok = bind.entity_set_argument_string_list(this.M, this.ptr, index, buf, ptrs.length) !== 0;
    this.M.stackRestore(sp);
    for (const p of ptrs) this.M._free(p);
    return ok;
  }

  setEntityList(index: number, entities: Entity[]): boolean {
    const sp = this.M.stackSave();
    const buf = this.M.stackAlloc(entities.length * 4);
    for (let i = 0; i < entities.length; i++) {
      this.M.setValue(buf + i * 4, entities[i].ptr, "i32");
    }
    const ok = bind.entity_set_argument_entity_list(this.M, this.ptr, index, buf, entities.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setIntMatrix(index: number, rows: number[][]): boolean {
    const { values, rowOffsets } = flattenMatrix(rows);
    const sp = this.M.stackSave();
    const valuesPtr = this.M.stackAlloc(values.length * 4);
    const offsetsPtr = this.M.stackAlloc(rowOffsets.length * 4);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(valuesPtr + i * 4, values[i], "i32");
    }
    for (let i = 0; i < rowOffsets.length; i++) {
      this.M.setValue(offsetsPtr + i * 4, rowOffsets[i], "i32");
    }
    const ok = bind.entity_set_argument_int_matrix(
      this.M,
      this.ptr,
      index,
      valuesPtr,
      values.length,
      offsetsPtr,
      rows.length,
    ) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setDoubleMatrix(index: number, rows: number[][]): boolean {
    const { values, rowOffsets } = flattenMatrix(rows);
    const sp = this.M.stackSave();
    const valuesPtr = this.M.stackAlloc(values.length * 8);
    const offsetsPtr = this.M.stackAlloc(rowOffsets.length * 4);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(valuesPtr + i * 8, values[i], "double");
    }
    for (let i = 0; i < rowOffsets.length; i++) {
      this.M.setValue(offsetsPtr + i * 4, rowOffsets[i], "i32");
    }
    const ok = bind.entity_set_argument_double_matrix(
      this.M,
      this.ptr,
      index,
      valuesPtr,
      values.length,
      offsetsPtr,
      rows.length,
    ) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  setEntityMatrix(index: number, rows: Entity[][]): boolean {
    const { values, rowOffsets } = flattenMatrix(rows);
    const sp = this.M.stackSave();
    const valuesPtr = this.M.stackAlloc(values.length * 4);
    const offsetsPtr = this.M.stackAlloc(rowOffsets.length * 4);
    for (let i = 0; i < values.length; i++) {
      this.M.setValue(valuesPtr + i * 4, values[i].ptr, "i32");
    }
    for (let i = 0; i < rowOffsets.length; i++) {
      this.M.setValue(offsetsPtr + i * 4, rowOffsets[i], "i32");
    }
    const ok = bind.entity_set_argument_entity_matrix(
      this.M,
      this.ptr,
      index,
      valuesPtr,
      values.length,
      offsetsPtr,
      rows.length,
    ) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  private _readEntityList(listPtr: Ptr): Entity[] {
    const count = bind.entity_list_count(this.M, listPtr);
    const result: Entity[] = [];
    for (let i = 0; i < count; i++) {
      const ePtr = bind.entity_list_get(this.M, listPtr, i);
      if (ePtr) result.push(new Entity(this.M, ePtr));
    }
    bind.entity_list_close(this.M, listPtr);
    return result;
  }

  private _readStringList(listPtr: Ptr): string[] {
    const count = bind.string_list_count(this.M, listPtr);
    const result: string[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.string_list_get(this.M, listPtr, i));
    }
    bind.string_list_close(this.M, listPtr);
    return result;
  }
}
