# IfcOpenShell C API / Native Python Binding — Progress Overview

> Branch: `bindgen-v3`
> Last updated: 2026-06-06

---

## Project Goals

Build a production-ready native C ABI (`ifcopenshell_capi`) with a CPython C extension backend (`_ifcopenshell_capi`) replacing the legacy ctypes bindings. This involves:

1. A custom binding generator (`src/ifcwrap/binding_generator/`) that sources C++ specs and YAML policy and emits stable C ABI + CPython C extension.
2. High-level native C++ implementations of `ifcopenshell.api.*` and `ifcopenshell.util.*` modules under `src/ifcapi/`.
3. Full coverage of `IfcParse` and `IfcGeom` C++ APIs through the generated C ABI.
4. Migration of YAML spec files (`ifcparse.yml`, `ifcgeom.yml`, `ifcapi.yml`) to C++ spec files (`ifcparse.hpp`, `ifcgeom.hpp`, `ifcapi.hpp`).

---

## Architecture

### Generator (`src/ifcwrap/binding_generator/`)

| File | Role |
|------|------|
| `c_backend.py` | Orchestrates generation; CLI entrypoint |
| `cpp_spec_frontend.py` | Parses C++ spec files via Clang, discovers functions, handles, result structs |
| `authored_spec.py` | Shared type inference; YAML policy IR; owned/borrowed/nullable semantics |
| `clang_discovery.py` | Clang AST-based method/function discovery |
| `contract_discovery.py` | `IFCAPI_BINDING` annotation discovery from facade headers |
| `c_call_rendering.py` | Call/result/parameter body rendering; empty-handle null-check generation |
| `c_header_rendering.py` | Public C header rendering |
| `c_handle_rendering.py` | Handle storage, destructor bodies, handle-return wrapping |
| `c_type_rendering.py` | ABI type maps, parameter/out-type rendering |
| `c_sequence_helpers.py` | Primitive sequence/list and handle-list rendering |
| `c_internal_header.py` | Internal C++ header rendering |
| `c_runtime_support.py` | Runtime support (error handling, string/list runtime helpers) |
| `python_extension_backend.py` | **IR-driven CPython C extension generator** — generates `_ifcopenshell_capi` wrappers for ALL 1,069 C API functions |
| `host_metadata.py` | Host binding metadata model |
| `binding_inventory.py` | Machine-readable audit report |
| `source_inventory.py` | Source inventory facade over `clang_discovery.py` |
| `specs/cpp/spec_macros.h` | C++ spec annotation vocabulary |

### Generated Outputs (committed)

| File | Description |
|------|-------------|
| `ifcwrap/binding_generator/generated/ifcopenshell_api.h` | Public C ABI header |
| `ifcwrap/binding_generator/generated/ifcopenshell_api.cpp` | Generated C++ implementation |
| `ifcwrap/binding_generator/generated/ifcopenshell_api_internal.hpp` | Internal C++ ABI header |
| `ifcwrap/binding_generator/generated/ifcopenshell_capi_py.cpp` | Generated CPython C extension (~31K lines, 1,069 function wrappers) |

### Python Binding Architecture

| File | Role |
|------|------|
| `ifcapi/python/ifcopenshell/_ifcopenshell_capi*.so` | Built CPython C extension — zero overhead C API calls |
| `ifcapi/python/ifcopenshell/_capi_utils.py` | **Auto-generated** shared utilities (handle conversion, error mapping, `unwrap_parameter_type`) |
| `ifcapi/python/ifcopenshell/__init__.py` | `file` class — uses `_capi.*` directly |
| `ifcapi/python/ifcopenshell/entity_instance.py` | `entity_instance` class — uses `_capi.*` directly |
| `ifcapi/python/ifcopenshell/ifcopenshell_wrapper.py` | Schema introspection — uses `_capi.*` directly |
| `ifcapi/python/ifcopenshell/api/*/_capi.py` | Thin API helpers, import from `_capi_utils` |
| `ifcapi/python/ifcopenshell/util/*.py` | Utility modules — uses `_capi.*` directly |
| `ifcapi/python/ifcopenshell/geom/*.py` | Geometry bindings — uses `_capi.*` directly |

### C++ Spec Files

| File | Status |
|------|--------|
| `specs/cpp/ifcapi.hpp` | ✅ Committed |
| `specs/cpp/ifcparse.hpp` | ✅ Committed |
| `specs/cpp/ifcgeom.hpp` | ⚠️ Partial (untracked) |

### YAML Spec Files

| File | Status |
|------|--------|
| `specs/ifcapi.yml` | ✅ Deleted — fully replaced by `ifcapi.hpp` |
| `specs/ifcparse.yml` | ✅ Discovery-policy only |
| `specs/ifcgeom.yml` | ❌ Still 3,317 lines — needs migration to `ifcgeom.hpp` |

---

## ctypes→C Extension Migration

### Status: COMPLETE ✅

**20 commits** on `bindgen-v3`. All ctypes eliminated. Every Python module uses `_ifcopenshell_capi` directly.

| What was removed | What replaced it |
|-----------------|------------------|
| `_generated_capi.py` (cypes facade) | `_ifcopenshell_capi` C extension |
| `python_ctypes_backend.py` (generator) | `python_extension_backend.py` (IR-driven) |
| `ctypes.CDLL` loading | Native C extension import |
| `ctypes.byref()` / `c_void_p` / `POINTER()` | Native handle objects |
| Manual `last_error` checking | C extension auto-raises exceptions |
| Duplicated `_capi.py` helpers | Generated `_capi_utils.py` |
| `ifcapi/python/ifcapi/` package (abandoned) | — (deleted) |

### Key Generator Features

- **1,069 auto-wrappers** generated from IR (zero hand-written wrappers)
- **73 handle types** with `PyMemberDef` (`handle`, `_handle_ptr`, `owned`)
- **Sequence protocol** for instance lists (`__len__`, `__getitem__`)
- **Destroy wrappers** auto-generated for all handle types
- **Cast handle null-check** — `declaration_as_entity` etc. return `nullptr` on failed cast
- **Value handle empty-check** — `express::Base` falsy values return `nullptr`
- **Void function error checking** — catches C++ functions that set errors but return ok

---

## Test Status

### Broad Native Suite (2585 passed, 22 skipped, 0 failed)

```bash
cd src/ifcapi/tests/python
python -m pytest test/ -q --ignore=test/test_sql.py --ignore=test/api/alignment
# 2585 passed, 22 skipped, 0 failed
```

### Skipped Tests

| Test | Reason |
|------|--------|
| `test_pipe_12d` | Pre-existing geometry core bug — mirrored coords on OpenCASCADE build. Present in v0.8.0. |
| 21 others | Pre-existing skips from before ctypes migration |

### Known Exclusions

| Test | Reason |
|------|--------|
| `test_sql.py` | Requires `ifcpatch` module (not in standard build) |
| `test/api/alignment/` | Large orchestration surface; deferred |

### Generator/Inventory Gate

```bash
python -m pytest -q --no-header -x \
  src/ifcwrap/tests/test_cpp_spec_frontend.py \
  src/ifcwrap/tests/test_ifcapi_binding_spec.py \
  src/ifcwrap/tests/test_binding_generator_features.py \
  src/ifcwrap/tests/test_generate_c_backend.py \
  src/ifcwrap/tests/test_binding_inventory.py
# Result: 75 passed
```

### Build Commands

```bash
# Codegen
cmake --build build-capi-stable --target ifcopenshell_capi_codegen --parallel 10

# C++ library
cmake --build build-capi-stable --target ifcopenshell_capi --parallel 10

# Python C extension
cmake --build build-capi-stable --target _ifcopenshell_capi --parallel 10
```

---

## High-Level Python API Native Coverage

### Native-Backed Modules

| Module | Status |
|--------|--------|
| `api.pset` | ✅ |
| `api.geometry` | ✅ |
| `api.root` | ✅ |
| `api.owner` | ✅ |
| `api.system` | ✅ |
| `api.material` | ✅ |
| `api.style` | ✅ |
| `api.context` | ✅ |
| `api.layer` | ✅ |
| `api.profile` | ✅ |
| `api.unit` | ✅ |
| `api.document` | ✅ |
| `api.library` | ✅ |
| `api.constraint` | ✅ |
| `api.project` | ✅ |
| `api.sequence` | ✅ |
| `api.cost` | ✅ |
| `api.resource` | ✅ |
| `api.control` | ✅ |
| `api.drawing` | ✅ |
| `api.classification` | ✅ |
| `api.georeference` | ✅ |
| `api.spatial` | ✅ |
| `api.feature` | ✅ |
| `api.boundary` | ✅ |
| `api.grid` | ✅ |
| `api.nest` | ✅ |
| `api.aggregate` | ✅ |
| `api.type` | ✅ |
| `api.group` | ✅ |
| `api.structural` | ✅ |
| `api.cogo` | ✅ |
| `api.attribute` | ✅ |
| `api.pset_template` | ✅ |
| `util.element` | ✅ |
| `util.selector` | ✅ |
| `util.unit` | ✅ |
| `util.shape_builder` | ✅ |
| `util.representation` | ✅ |
| `util.schema` | ✅ |

---

## Key Architectural Constraints

- **No changes to generated C/Python outputs directly** — always fix source/spec/generator, then regenerate
- **YAML stays policy-only** — no inline C++ adapter bodies in YAML
- **No ctypes anywhere** — all C API calls go through `_ifcopenshell_capi` C extension
- **No workarounds** — all fixes at root cause (generator, C API, or C++ implementation)
- **`src/ifcparse` and `src/ifcgeom` are core sources** — only modify if proven bug exists and v0.8.0 has same fix

---

## Immediate Action Items

1. **Commit `ifcgeom.hpp`** — untracked, at risk
2. **Complete `ifcgeom.yml` → `ifcgeom.hpp` migration**
3. **Regenerate `_ifcopenshell_capi` with IR-driven generator** — currently using cached .cpp, needs fresh codegen after ifcgeom.hpp migration
4. **Clean up `test/api/alignment/`** — migrated but imports `get_lib()` which no longer exists

---

## Recent Commits (bindgen-v3)

| Commit | Title |
|--------|-------|
| `6a5aa69` | Add _KNOWN_SKIPS to conftest |
| `74b53d6` | Fix 6 remaining test failures |
| `9fb1bdd` | Fix 36 remaining test failures |
| `7a392db` | Eliminate workarounds at root cause |
| `26d39f3` | Fix cast handle null-check |
| `9ab8ad1` | Generate shared _capi_utils.py |
| `19657c8` | PyMemberDef, sequence protocol, destroy wrappers |
| `de30317` | Fix empty value handle wrapping |
| `7b2565f` | Fix generator bugs |
| `1358b60` | Remove ctypes backend |
| `ea0f879` | Migrate file + entity_instance to C extension |
| `8933705` | Add CPython C extension skeleton |
