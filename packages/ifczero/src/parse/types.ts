export type Ptr = number;

export const enum FileType {
  IFCSPF = 0,
  IFCXML = 1,
  IFCZIP = 2,
  ROCKSDB = 3,
  UNKNOWN = 4,
  AUTODETECT = 5,
}

export const enum FileStatus {
  SUCCESS = 0,
  READ_ERROR = 1,
  NO_HEADER = 2,
  UNSUPPORTED_SCHEMA = 3,
  INVALID_SYNTAX = 4,
  UNKNOWN = 5,
}

export const enum ArgumentType {
  NULL = 0,
  DERIVED = 1,
  INT = 2,
  BOOL = 3,
  LOGICAL = 4,
  DOUBLE = 5,
  STRING = 6,
  BINARY = 7,
  ENUMERATION = 8,
  ENTITY_INSTANCE = 9,
  EMPTY_AGGREGATE = 10,
  AGGREGATE_OF_INT = 11,
  AGGREGATE_OF_DOUBLE = 12,
  AGGREGATE_OF_STRING = 13,
  AGGREGATE_OF_BINARY = 14,
  AGGREGATE_OF_ENTITY_INSTANCE = 15,
  AGGREGATE_OF_EMPTY_AGGREGATE = 16,
  AGGREGATE_OF_AGGREGATE_OF_INT = 17,
  AGGREGATE_OF_AGGREGATE_OF_DOUBLE = 18,
  AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE = 19,
  UNKNOWN = 20,
}

export const enum DeclarationKind {
  UNKNOWN = 0,
  TYPE_DECLARATION = 1,
  SELECT_TYPE = 2,
  ENUMERATION_TYPE = 3,
  ENTITY = 4,
}

export const enum ParameterTypeKind {
  UNKNOWN = 0,
  NAMED = 1,
  SIMPLE = 2,
  AGGREGATION = 3,
}

export const enum SimpleType {
  BINARY = 0,
  BOOLEAN = 1,
  INTEGER = 2,
  LOGICAL = 3,
  NUMBER = 4,
  REAL = 5,
  STRING = 6,
  INVALID = 7,
}

export const enum AggregationType {
  ARRAY = 0,
  BAG = 1,
  LIST = 2,
  SET = 3,
  INVALID = 4,
}

export const enum InverseAggregationType {
  BAG = 0,
  SET = 1,
  UNSPECIFIED = 2,
  INVALID = 3,
}
