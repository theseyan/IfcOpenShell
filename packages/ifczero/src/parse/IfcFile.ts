import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import { Entity } from "./Entity.js";
import { TypeDeclaration } from "./Schema.js";
import { FileType, type FileStatus, type Ptr } from "./types.js";

export class IfcFile {
  private _ptr: Ptr;
  private _closed = false;

  private constructor(
    readonly M: EmscriptenModule,
    ptr: Ptr,
  ) {
    this._ptr = ptr;
  }

  get ptr(): Ptr {
    return this._ptr;
  }

  private static fromOpenedPtr(M: EmscriptenModule, ptr: Ptr, source: "memory" | "path"): IfcFile {
    if (!ptr) {
      if (source === "memory") {
        throw new Error("Failed to open IFC file from memory");
      }
      throw new Error("Failed to open IFC file");
    }

    const file = new IfcFile(M, ptr);
    const status = file.status;
    if (status !== 0) {
      const statusMessage = bind.file_status_message(M, status);
      const detail = file.lastError || bind.last_error(M) || "";
      file.close();
      if (detail && detail !== statusMessage) {
        throw new Error(`Failed to open IFC file (${source}): ${statusMessage} (${detail})`);
      }
      throw new Error(`Failed to open IFC file (${source}): ${statusMessage}`);
    }

    return file;
  }

  static createEmpty(schema: string): IfcFile {
    const M = getModule();
    const ptr = bind.file_create_empty(M, schema);
    if (!ptr) throw new Error("Failed to create empty IFC file");
    return new IfcFile(M, ptr);
  }

  static openFromMemory(data: Uint8Array | string): IfcFile {
    const M = getModule();
    const bytes = typeof data === "string" ? new TextEncoder().encode(data) : data;
    if (bytes.length === 0) {
      throw new Error("Cannot open IFC from an empty buffer");
    }
    const dataPtr = M._malloc(bytes.length);
    if (!dataPtr) {
      throw new Error("Failed to allocate IFC buffer");
    }

    M.HEAPU8.set(bytes, dataPtr);
    try {
      const ptr = M.ccall(
        "ifcopenshell_ifcparse_file_open_from_memory",
        "number",
        ["number", "number"],
        [dataPtr, bytes.length],
      );
      return IfcFile.fromOpenedPtr(M, ptr, "memory");
    } finally {
      M._free(dataPtr);
    }
  }

  static open(path: string, fileType: FileType = FileType.AUTODETECT, readonly = false): IfcFile {
    const M = getModule();
    const ptr = bind.file_open(M, path, fileType, readonly ? 1 : 0);
    return IfcFile.fromOpenedPtr(M, ptr, "path");
  }

  close(): void {
    if (this._closed) return;
    bind.file_close(this.M, this._ptr);
    this._closed = true;
  }

  get status(): FileStatus {
    return bind.file_status(this.M, this._ptr) as FileStatus;
  }

  get schema(): string {
    return bind.file_schema_name(this.M, this._ptr);
  }

  get schemaIdentifier(): string {
    return this.schema;
  }

  get entityCount(): number {
    return bind.file_entity_count(this.M, this._ptr);
  }

  get maxId(): number {
    return bind.file_max_id(this.M, this._ptr);
  }

  headerDescription(): Entity | null {
    const ptr = bind.file_header_file_description(this.M, this._ptr);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  headerFileName(): Entity | null {
    const ptr = bind.file_header_file_name(this.M, this._ptr);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  headerSchema(): Entity | null {
    const ptr = bind.file_header_file_schema(this.M, this._ptr);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  entities(): Entity[] {
    const listPtr = bind.file_entities(this.M, this._ptr);
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  entityIds(): number[] {
    const listPtr = bind.file_entity_ids(this.M, this._ptr);
    if (!listPtr) return [];
    const count = bind.int_list_count(this.M, listPtr);
    const result: number[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.int_list_get(this.M, listPtr, i));
    }
    bind.int_list_close(this.M, listPtr);
    return result;
  }

  byId(id: number): Entity | null {
    const ptr = bind.file_instance_by_id(this.M, this._ptr, id);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  byGuid(guid: string): Entity | null {
    const ptr = bind.file_instance_by_guid(this.M, this._ptr, guid);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  byType(typeName: string, includeSubtypes = true): Entity[] {
    const listPtr = bind.file_instances_by_type(
      this.M,
      this._ptr,
      typeName,
      includeSubtypes ? 1 : 0,
    );
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  byReference(id: number): Entity[] {
    const listPtr = bind.file_instances_by_reference(this.M, this._ptr, id);
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  getInverse(
    instanceId: number,
    typeName: string,
    attributeIndex: number,
  ): Entity[] {
    const listPtr = bind.file_get_inverse(
      this.M,
      this._ptr,
      instanceId,
      typeName,
      attributeIndex,
    );
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  getTotalInverses(instanceId: number): number {
    return bind.file_get_total_inverses(this.M, this._ptr, instanceId);
  }

  inverseIndices(instanceId: number): number[] {
    const listPtr = bind.file_get_inverse_indices(this.M, this._ptr, instanceId);
    if (!listPtr) return [];
    const count = bind.int_list_count(this.M, listPtr);
    const result: number[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.int_list_get(this.M, listPtr, i));
    }
    bind.int_list_close(this.M, listPtr);
    return result;
  }

  createEntity(typeName: string): Entity {
    const ptr = bind.file_create_entity_by_type(this.M, this._ptr, typeName);
    if (!ptr) throw new Error(`Failed to create entity of type ${typeName}`);
    return new Entity(this.M, ptr);
  }

  createEntityWithId(typeName: string, id: number): Entity {
    const ptr = bind.file_create_entity_by_type_with_id(this.M, this._ptr, typeName, id);
    if (!ptr) throw new Error(`Failed to create entity of type ${typeName} with id ${id}`);
    return new Entity(this.M, ptr);
  }

  addEntity(entity: Entity, id = 0): number {
    return bind.file_add_entity(this.M, this._ptr, entity.ptr, id);
  }

  addEntities(entities: Entity[]): boolean {
    if (entities.length === 0) return true;
    const sp = this.M.stackSave();
    const buf = this.M.stackAlloc(entities.length * 4);
    for (let i = 0; i < entities.length; i++) {
      this.M.setValue(buf + i * 4, entities[i].ptr, "i32");
    }
    const ok = bind.file_add_entities(this.M, this._ptr, buf, entities.length) !== 0;
    this.M.stackRestore(sp);
    return ok;
  }

  removeEntity(entity: Entity): boolean {
    return bind.file_remove_entity(this.M, this._ptr, entity.ptr) !== 0;
  }

  removeById(id: number): boolean {
    return bind.file_remove_entity_by_id(this.M, this._ptr, id) !== 0;
  }

  toSpf(): string {
    return bind.file_to_spf(this.M, this._ptr);
  }

  writeSpf(path: string): boolean {
    return bind.file_write_spf(this.M, this._ptr, path) !== 0;
  }

  unitScale(unitType: string): number {
    return bind.file_unit_scale(this.M, this._ptr, unitType);
  }

  unitEntity(unitType: string): Entity | null {
    const ptr = bind.file_unit_entity(this.M, this._ptr, unitType);
    return ptr ? new Entity(this.M, ptr) : null;
  }

  traverse(entity: Entity, maxLevel = -1, breadthFirst = false): Entity[] {
    const listPtr = bind.file_traverse(
      this.M,
      this._ptr,
      entity.ptr,
      maxLevel,
      breadthFirst ? 1 : 0,
    );
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  traverseById(instanceId: number, maxLevel = -1, breadthFirst = false): Entity[] {
    const listPtr = bind.file_traverse_by_id(
      this.M,
      this._ptr,
      instanceId,
      maxLevel,
      breadthFirst ? 1 : 0,
    );
    if (!listPtr) return [];
    return this._readEntityList(listPtr);
  }

  types(): TypeDeclaration[] {
    const listPtr = bind.file_types(this.M, this._ptr);
    if (!listPtr) return [];
    const count = bind.type_list_count(this.M, listPtr);
    const result: TypeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const tPtr = bind.type_list_get(this.M, listPtr, i);
      if (tPtr) result.push(new TypeDeclaration(this.M, tPtr));
    }
    bind.type_list_close(this.M, listPtr);
    return result;
  }

  batchBegin(): void {
    bind.file_batch_begin(this.M, this._ptr);
  }

  batchEnd(): void {
    bind.file_batch_end(this.M, this._ptr);
  }

  recalculateIdCounter(): void {
    bind.file_recalculate_id_counter(this.M, this._ptr);
  }

  get lastError(): string | null {
    return bind.file_last_error(this.M, this._ptr) || null;
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
}
