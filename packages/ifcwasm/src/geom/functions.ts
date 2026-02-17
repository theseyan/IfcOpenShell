import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import { Mesh } from "./Mesh.js";
import { Serialized } from "./Serialized.js";
import { CreatedShapeKind, type Ptr } from "./types.js";

export function createMeshForId(
  filePtr: Ptr,
  settingsPtr: Ptr,
  id: number,
  geometryLibrary = "opencascade",
): Mesh | null {
  const M = getModule();
  const ptr = bind.create_mesh_for_id(M, filePtr, settingsPtr, geometryLibrary, id);
  return ptr ? new Mesh(M, ptr) : null;
}

export class CreatedShape {
  private destroyed = false;

  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  private assertAlive(): void {
    if (this.destroyed) throw new Error("CreatedShape has been destroyed");
  }

  get kind(): CreatedShapeKind {
    this.assertAlive();
    return bind.created_shape_kind(this.M, this.ptr) as CreatedShapeKind;
  }

  get mesh(): Mesh | null {
    this.assertAlive();
    const kind = this.kind;
    if (kind !== CreatedShapeKind.TRIANGULATION && kind !== CreatedShapeKind.BREP) return null;
    const meshPtr = bind.created_shape_mesh(this.M, this.ptr);
    return meshPtr ? new Mesh(this.M, meshPtr) : null;
  }

  get serialized(): Serialized | null {
    this.assertAlive();
    if (this.kind !== CreatedShapeKind.SERIALIZED) return null;
    const serializedPtr = bind.created_shape_serialized(this.M, this.ptr);
    return serializedPtr ? new Serialized(this.M, serializedPtr) : null;
  }

  get transform(): Float64Array | null {
    this.assertAlive();
    const sp = this.M.stackSave();
    const outPtr = this.M.stackAlloc(16 * 8);
    const hasTransform = bind.created_shape_transform(this.M, this.ptr, outPtr);
    if (!hasTransform) {
      this.M.stackRestore(sp);
      return null;
    }
    const transform = new Float64Array(16);
    for (let i = 0; i < 16; i++) {
      transform[i] = this.M.getValue(outPtr + i * 8, "double");
    }
    this.M.stackRestore(sp);
    return transform;
  }

  destroy(): void {
    if (this.destroyed) return;
    bind.created_shape_destroy(this.M, this.ptr);
    this.destroyed = true;
  }
}

export function createShapeForId(
  filePtr: Ptr,
  settingsPtr: Ptr,
  id: number,
  geometryLibrary = "opencascade",
): CreatedShape | null {
  const M = getModule();
  const shapePtr = bind.create_shape_for_id(M, filePtr, settingsPtr, geometryLibrary, id);
  if (!shapePtr) return null;
  return new CreatedShape(M, shapePtr);
}

export function lastError(): string {
  return bind.last_error(getModule());
}

export function mapShapeReprForId(
  filePtr: Ptr,
  settingsPtr: Ptr,
  id: number,
): string | null {
  const M = getModule();
  return bind.map_shape_repr_for_id(M, filePtr, settingsPtr, id) || null;
}
