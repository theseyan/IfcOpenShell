
/**
 * Geometry APIs for settings, iteration, meshes, and spatial queries.
 *
 * @module Geometry
 */

import '../disposable.js';

export { GeomSettings } from './settings.js';
export { GeomIterator } from './iterator.js';
export { GeometryTree } from './tree.js';
export { columnMajorToRowMajor4, rowMajorToColumnMajor4, transformPoint4 } from './matrix.js';
export type {
  CollectOptions,
  CollectResult,
  IteratorFilter,
  IteratorMetadata,
  IteratorOptions,
  OperationProgress,
} from './iterator.js';
export type { Mesh, MeshFloatArray, MeshPrecision } from './mesh.js';
export type { MatrixPoint3 } from './matrix.js';
export type { Box3, Point3, Ray, RayHit } from './tree.js';
export type { SettingInput } from './settings.js';
