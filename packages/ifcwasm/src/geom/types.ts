export type Ptr = number;

export const enum ElementKind {
  NONE = 0,
  TRIANGULATION = 1,
  TRIANGULATED = 1,
  BREP = 2,
  SERIALIZED = 3,
}

export const enum CreatedShapeKind {
  NONE = 0,
  TRIANGULATION = 1,
  TRIANGULATED = 1,
  BREP = 2,
  SERIALIZED = 3,
  TRANSFORM = 4,
}

export interface MeshData {
  id: number;
  parentId: number;
  name: string;
  type: string;
  guid: string;
  vertices: Float64Array;
  faces: Int32Array;
  normals: Float64Array;
  edges: Int32Array;
  materialIds: Int32Array;
  colors: Float64Array;
  transform: Float64Array;
}
