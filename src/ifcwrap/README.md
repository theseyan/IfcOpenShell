# IfcWrap

`src/ifcwrap` now contains two binding paths:

- the legacy SWIG interface files used for the current Python bindings
- the custom source-driven binding generator for the new C API

## Current Status

The custom generator currently targets `IfcParse`.

- direct bindings are discovered from the real C++ source using Clang AST data
- policy stays in `binding_generator/specs/ifcparse.yml`
- generated output is checked in under `binding_generator/generated/`
- the generated `IfcParse` C API builds through `BUILD_IFCCAPI=ON`
- parity with the legacy SWIG-exposed `IfcParse` surface is tracked in
  `binding_generator/generated/ifcparse_parity.md`

At the current snapshot, `IfcParse` has full audited surface parity with the
legacy SWIG layer: `205/205` covered entries.

## Key Paths

- `IfcPython.i`, `IfcParseWrapper.i`, `IfcGeomWrapper.i`
  Legacy SWIG binding inputs.
- `binding_generator/specs/ifcparse.yml`
  Coarse binding policy for ifcparse.
- `binding_generator/specs/ifcgeom.yml`
  Coarse binding policy for ifcgeom.
- `binding_generator/generated/ifcopenshell_api.h`
  Generated unified public C header (ifcparse + ifcgeom).
- `binding_generator/generated/ifcopenshell_api.cpp`
  Generated C++ glue.
- `tests/ifcparse_c_api_smoke.c`
  Main generated-C smoke test.
- `tests/ifcparse_c_api_surface_smoke.c`
  Wider surface and mutator smoke coverage.
- `tests/ifcparse_c_api_rocksdb_smoke.c`
  RocksDB-backed generated-C smoke coverage.

## Build Flags

- `BUILD_IFCPYTHON=ON`
  Builds the legacy SWIG-based Python bindings.
- `BUILD_IFCCAPI=ON`
  Builds the generated `IfcParse` C API.
- `IFCCAPI_REGENERATE_BINDINGS=ON`
  Developer-only regeneration of the checked-in C bindings.

The intended next step after `IfcParse` is to extend the same generator model
to the next `ifcwrap` slice while keeping the binding definition files coarse
and maintainable.
