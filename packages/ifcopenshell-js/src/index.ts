
/**
 * Core `@ifcopenshell-js/web` API.
 *
 * @module Core
 */

import './disposable.js';

export {
  init,
  IfcOpenShellError,
  IfcOpenShellErrorCode,
  IfcOpenShellErrorKind,
  abortError,
  isIfcOpenShellAbortError,
} from './init.js';
export type { IfcOpenShell } from './init.js';
export type * from './generated/ifcopenshell_api.js';
export type { ApiData, ValueData, ValueInput } from './api.js';
export { Value } from './api.js';
export { IfcFile } from './file.js';
export type { FileInfo, HeaderInfo, OpenOptions } from './file.js';
export { Entity } from './entity.js';
export type { AttributeInput, EntityInfo } from './entity.js';
export { AttributeValue } from './attribute.js';
export type { IfcLogical, IfcValue, NestedEntityIds } from './attribute.js';
export {
  GeomIterator,
  GeomSettings,
  GeometryTree,
  columnMajorToRowMajor4,
  rowMajorToColumnMajor4,
  transformPoint4,
} from './geom/index.js';
export type {
  Box3,
  CollectOptions,
  CollectResult,
  IteratorFilter,
  IteratorMetadata,
  IteratorOptions,
  Mesh,
  MeshFloatArray,
  MeshPrecision,
  MatrixPoint3,
  OperationProgress,
  Point3,
  Ray,
  RayHit,
} from './geom/index.js';
export {
  SerializerSettings,
  exportToBuffer,
} from './serializers/index.js';
export { PsetProperties } from './pset.js';
export type { PsetInput, PsetScalar, PsetValue, TypedPsetValue } from './pset.js';
export type {
  ExportOptions,
  ExportResult,
  SerializerFormat,
} from './serializers/index.js';
export type {
  EmscriptenFS,
  EmscriptenOption,
  EmscriptenOptions,
  EmscriptenModuleFactory,
  IfcOpenshellApiFactory,
  IfcOpenshellModule,
  InitOptions,
  PluginEntry,
  PluginKind,
  PluginLoader,
  PluginManifest,
  Ptr,
  WasmAssets,
} from './types.js';
export * as util from './util/index.js';
