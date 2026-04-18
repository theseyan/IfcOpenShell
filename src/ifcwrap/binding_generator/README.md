# IfcWrap Binding Generator

This directory contains the custom binding generator that is replacing the
legacy SWIG-only binding flow for `IfcParse`.

## Current State

- The generator is source-driven for direct bindings.
  It reads real C++ declarations through Clang AST data using
  `compile_commands.json`.
- The binding definition file is policy-oriented, not a full hand-written API
  inventory.
  For `IfcParse`, the main input is
  `specs/ifcparse.yml`; for `IfcGeom`, `specs/ifcgeom.yml`.
- Authored YAML is intentionally minimal.
  The spec names the exposed operation and any adapter implementation, while
  C ABI names are derived automatically from `expose_as` and `receiver`.
  Rare ABI-only naming exceptions use `c_expose_as` instead of a full manual
  `c_name`.
- The generated unified C API is checked in at:
  - `generated/ifcopenshell_api.h`
  - `generated/ifcopenshell_api.cpp`

The current `IfcParse` parity audit reports:

- `205` SWIG surface entries
- `205` covered entries
- `0` missing entries
- `0` unmapped receivers

That means the generated `IfcParse` C surface has functional parity with the
legacy SWIG-exposed `IfcParse` API, plus some generated-only helper surface for
typed C access.

## Main Files

- `specs/ifcparse.yml`
  Binding policy for the `IfcParse` slice.
- `clang_discovery.py`
  Clang AST discovery driven by `compile_commands.json`.
- `authored_spec.py`
  Spec loading, validation, and normalization into generator calls.
- `c_backend.py`
  C header and C++ glue generation.
- `audit_ifcparse_parity.py`
  SWIG-vs-generated parity audit for `IfcParse`.

## Build Integration

- `BUILD_IFCCAPI=ON`
  Builds the checked-in generated `IfcParse` C API.
- `IFCCAPI_REGENERATE_BINDINGS=ON`
  Developer-only regeneration mode.
  This regenerates the checked-in C API from `specs/ifcparse.yml` using the
  active build's `compile_commands.json`.

The generated files are intended to be checked in. Normal users of the library
do not need the generator dependencies.

## Test Coverage

The generated `IfcParse` C API is covered by:

- `tests/ifcparse_c_api_smoke.c`
  Main end-to-end smoke for parsed in-memory IFC data.
- `tests/ifcparse_c_api_surface_smoke.c`
  Wider generated-surface coverage for schema, metadata, mutators, traversal,
  and persistence-sensitive paths.
- `tests/ifcparse_c_api_rocksdb_smoke.c`
  Real RocksDB-backed smoke for create/add/mutate/reopen behavior.
- `tests/test_ifcparse_c_api_smoke_coverage.py`
  Guard that every generated C function is referenced by at least one C smoke
  source.

## Intended Direction

The next module slices should follow the same model:

1. source-driven discovery from real C++ via Clang AST
2. coarse binding policy in the spec
3. generated checked-in C ABI and glue
4. parity audit against legacy SWIG
5. full generated-surface C smoke coverage
