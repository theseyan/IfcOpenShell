import type { EmscriptenModule } from "../wasm/types.js";
import * as bind from "./bindings.generated.js";
import type { Ptr } from "./types.js";

export interface SerializedData {
  id: number;
  parentId: number;
  name: string;
  type: string;
  guid: string;
  context: string;
  uniqueId: string;
  brepData: string;
  surfaceStyles: Float64Array;
  surfaceStyleIds: Int32Array;
  transform: Float64Array;
}

export class Serialized {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get id(): number {
    return bind.serialized_id(this.M, this.ptr);
  }

  get parentId(): number {
    return bind.serialized_parent_id(this.M, this.ptr);
  }

  get name(): string {
    return bind.serialized_name(this.M, this.ptr);
  }

  get type(): string {
    return bind.serialized_type(this.M, this.ptr);
  }

  get guid(): string {
    return bind.serialized_guid(this.M, this.ptr);
  }

  get context(): string {
    return bind.serialized_context(this.M, this.ptr);
  }

  get uniqueId(): string {
    return bind.serialized_unique_id(this.M, this.ptr);
  }

  get brepData(): string {
    return bind.serialized_brep_data(this.M, this.ptr);
  }

  get surfaceStyles(): Float64Array {
    const count = bind.serialized_surface_styles_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.serialized_surface_styles_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get surfaceStyleIds(): Int32Array {
    const count = bind.serialized_surface_style_ids_count(this.M, this.ptr);
    if (count === 0) return new Int32Array(0);
    const dataPtr = bind.serialized_surface_style_ids_data(this.M, this.ptr);
    return new Int32Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  get transform(): Float64Array {
    const count = bind.serialized_transform_count(this.M, this.ptr);
    if (count === 0) return new Float64Array(0);
    const dataPtr = bind.serialized_transform_data(this.M, this.ptr);
    return new Float64Array(this.M.HEAPU8.buffer, dataPtr, count);
  }

  toData(): SerializedData {
    return {
      id: this.id,
      parentId: this.parentId,
      name: this.name,
      type: this.type,
      guid: this.guid,
      context: this.context,
      uniqueId: this.uniqueId,
      brepData: this.brepData,
      surfaceStyles: new Float64Array(this.surfaceStyles),
      surfaceStyleIds: new Int32Array(this.surfaceStyleIds),
      transform: new Float64Array(this.transform),
    };
  }

  destroy(): void {
    bind.serialized_destroy(this.M, this.ptr);
  }
}
