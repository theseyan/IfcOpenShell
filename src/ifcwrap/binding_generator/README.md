# IfcWrap Binding Generator

This directory contains the unified IfcOpenShell binding generator.

## Current State

- One invocation discovers the full IfcParse, IfcAPI, and IfcGeom surface,
  finalizes one ABI-aware `BindingIR`, and emits C, Python, and WASM/TypeScript.
- The generator is source-driven for direct bindings.
  It reads real C++ declarations through Clang AST data using
  `--discovery-include-dir` flags for include path resolution.
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
  Clang AST discovery driven by `--discovery-include-dir` flags.
- `authored_spec.py`
  Spec loading, validation, and normalization into generator calls.
- `pipeline.py`
  Full-project discovery, policy merge, and semantic lowering.
- `binding_ir.py` and `abi_ir.py`
  Finalized semantic and C ABI contract.
- `c_backend.py`
  Pure C header and C++ glue emission from finalized IR.
- `generate.py`
  The only production command and owner of atomic artifact writes.

## Build Integration

- `BUILD_IFCCAPI=ON`
  Builds the checked-in full IfcOpenShell C API.
- `IFCCAPI_REGENERATE_BINDINGS=ON`
  Developer-only regeneration mode.
  This exposes the single `ifcopenshell_bindings_codegen` target, which emits
  every C, Python, WASM, and TypeScript binding artifact in one process.
- `IFCWRAP_BINDGEN_JOBS=<N>`
  Optional generator-only concurrency for independent Clang discovery jobs.
  CMake's `--parallel` flag still only controls CMake build scheduling; it does
  not parallelise the Python generator itself. The default is
  `min(8, os.cpu_count())`; set `IFCWRAP_BINDGEN_JOBS=1` to force sequential
  discovery while debugging.

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
