import type { EmscriptenModule } from "../wasm/types.js";
import * as bind from "./bindings.generated.js";
import type { Ptr } from "./types.js";

export class Mesh {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get id(): number {
    return bind.mesh_id(this.M, this.ptr);
  }

  get parentId(): number {
    return bind.mesh_parent_id(this.M, this.ptr);
  }

  get name(): string {
    return bind.mesh_name(this.M, this.ptr);
  }

  get type(): string {
    return bind.mesh_type(this.M, this.ptr);
  }

  get guid(): string {
    return bind.mesh_guid(this.M, this.ptr);
  }

  get context(): string {
    return bind.mesh_context(this.M, this.ptr);
  }

  get uniqueId(): string {
    return bind.mesh_unique_id(this.M, this.ptr);
  }

  get vertices(): Float64Array {
    const count = bind.mesh_verts_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.mesh_verts_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get faces(): Int32Array {
    const count = bind.mesh_faces_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.mesh_faces_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get normals(): Float64Array {
    const count = bind.mesh_normals_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.mesh_normals_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get edges(): Int32Array {
    const count = bind.mesh_edges_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.mesh_edges_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get materialIds(): Int32Array {
    const count = bind.mesh_material_ids_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.mesh_material_ids_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get colors(): Float64Array {
    const count = bind.mesh_colors_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.mesh_colors_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get transform(): Float64Array {
    const count = bind.mesh_transform_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.mesh_transform_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get uvs(): Float64Array {
    const count = bind.mesh_uvs_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.mesh_uvs_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get itemIds(): Int32Array {
    const count = bind.mesh_item_ids_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.mesh_item_ids_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get edgesItemIds(): Int32Array {
    const count = bind.mesh_edges_item_ids_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.mesh_edges_item_ids_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  destroy(): void {
    bind.mesh_destroy(this.M, this.ptr);
  }
}
