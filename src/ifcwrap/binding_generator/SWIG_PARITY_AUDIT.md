<!-- This file was generated with the assistance of an AI coding tool. -->

# SWIG Parity Audit

Current baseline for `bindgen-v2` against the public surfaces in:

- `src/ifcwrap/IfcParseWrapper.i`
- `src/ifcwrap/IfcGeomWrapper.i`

This audit is intentionally pragmatic. It tracks parity at the level that matters for the generated C API:

- callable API surface that should exist outside Python
- explicit SWIG `%extend` helpers that represent real library behavior
- exclusions for Python-only conveniences that should not be treated as C API gaps

## Current Status

The generator is now in a workable state for parity work:

- merged repo-backed generation succeeds
- `ifcopenshell_capi_smoke` rebuilds and passes
- discovery/memory regressions from low-signal clang AST probing were fixed
- IfcOpenShell-specific glue is mostly back in YAML/spec policy instead of parser internals

This is not yet a claim of complete SWIG parity.

After the current `%extend` audit pass:

- no active non-Python `%extend` method from `IfcGeomWrapper.i` is known to be missing from the generated C API
- no active non-Python `%extend` method from `IfcParseWrapper.i` is known to be missing from the generated C API
- the previously missing collection-valued settings overloads and name-based `IfcBaseClass::get_argument(...)` convenience are now covered
- commented-out SWIG helpers such as `IfcFile.types_with_super()` are not treated as parity targets

## Covered Surface

These SWIG-exposed areas are now represented in the generated C API and exercised at least partly by tests or smoke coverage.

### IfcGeom

- `IfcGeom::tree` selection helpers
  - `select(IfcBaseClass*)`
  - `select(gp_Pnt)`
  - `select(shape_serialization)`
  - `select(BRepElement*)`
  - `select_box(IfcBaseClass*)`
  - `select_box(gp_Pnt)`
  - `select_box(Bnd_Box)`
- `IfcGeom::tree` clash helpers
  - `clash_intersection_many`
  - `clash_collision_many`
  - `clash_clearance_many`
- taxonomy constructor surface
  - points, directions, curves, planes, solids, lofts, booleans, sweeps
- taxonomy access helpers
  - children access
  - matrix access
  - component extraction
  - `style.instance_id`
- `IfcGeom::ConversionResultShape` wrapper methods
  - `serialize`
  - `serialize_obj`
  - `convex_tag`
  - `area`
  - `volume`
  - `length`
- `IfcGeom::Representation::BRep`
  - `item`
  - `calculate_volume`
  - `calculate_surface_area`
  - `as_compound`
- opaque number operations
- function item evaluator helpers
- serializer creation and settings access for scalar, string, and collection-valued setting types

### IfcParse

- core file/schema traversal and lookup surface
- `IfcUtil::IfcBaseClass` utility methods used outside Python-only representation concerns
  - attribute category
  - attribute names
  - inverse names
  - `is_a`
  - argument name/type/index access
  - argument lookup by name
  - inverse lookup
  - string conversion
  - file pointer
- `IfcFile` RocksDB key-value helpers
- IFC header helpers
- schema/type/entity/attribute inspection surface
- argument mutation helpers for scalar, aggregate, entity, and nested aggregate cases

## Known Non-Gaps

These appear in SWIG but should not be treated as missing C API parity.

### Python-only conveniences

- `__repr__`, `__iter__`, `__getitem__`, `__len__`, `__eq__`
- typemaps and Python tuple/list/dict conversion helpers
- `readInstancePy`
- `get_info_cpp`
- `ShapeRTTI`
- SWIG pointer upcast helpers for taxonomy shared_ptr polymorphism

These belong to the Python wrapper layer, not the C API layer.

### Explicit glue that should remain explicit

- external OpenCascade/CGAL wrapper types that are not part of the generic parser model
- stream / `std::ostream` helpers
- command-line option definition helpers using `po::options_description`

## Real Remaining Gaps

No active non-Python `%extend` gap is currently known from `IfcParseWrapper.i` or `IfcGeomWrapper.i`.

The remaining work is validation rather than discovery:

- keep checking that newly added/generated APIs behave the same as their SWIG equivalents
- decide whether any non-`%extend` SWIG-exposed callable surface still needs explicit C API coverage
- tighten runtime parity tests so this status stays true as the branch evolves

## Production Readiness

Not production ready yet.

Reasons:

- parity has been audited at the active `%extend` surface, but not yet signed off with exhaustive runtime validation
- the generated code still builds with many warnings
- there is not yet exhaustive API-level validation against the intended SWIG surface
- the current confidence level is smoke/integration-level, not release-level

## Next Work

Recommended next steps, in order:

1. Add targeted runtime parity tests for the active non-Python wrapper conveniences that were added manually in YAML.
2. Audit non-`%extend` SWIG callable surface only where it still differs materially from the generated C API.
3. Only after the runtime parity matrix is closed should this branch be considered for production hardening.
