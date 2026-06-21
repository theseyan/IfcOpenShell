<!-- This file was generated with the assistance of an AI coding tool. -->

# Native WASM Bootstrap Build System

Self-contained build system for IfcOpenShell WebAssembly that builds
Emscripten toolchain, third-party dependencies, and the WASM_BUILD target
**without Pyodide or SWIG**.

## Quick Start

```bash
# Check prerequisites
python tools/wasm_native/build.py doctor

# Full pipeline (installs emsdk, fetches sources, builds deps, configures, builds, tests, packages)
python tools/wasm_native/build.py all

# Or step by step:
python tools/wasm_native/build.py bootstrap-toolchain
python tools/wasm_native/build.py fetch
python tools/wasm_native/build.py build-deps
python tools/wasm_native/build.py configure
python tools/wasm_native/build.py build
python tools/wasm_native/build.py test
python tools/wasm_native/build.py package
```

## Profiles

Profiles control which geometry kernels and dependencies are built.

| Profile    | Kernels                        | Dependencies                              |
|------------|--------------------------------|-------------------------------------------|
| `minimal`  | passthrough                    | boost, eigen, nlohmann_json               |
| `manifold` | passthrough, manifold          | boost, eigen, nlohmann_json, manifold     |
| `cgal`     | passthrough, cgal, cgalsimple  | boost, eigen, nlohmann_json, gmp, mpfr, cgal |
| `occt`     | passthrough, opencascade       | boost, eigen, nlohmann_json, occt         |
| `full`     | all five kernels               | all dependencies                          |

Use `--profile` to select:

```bash
python tools/wasm_native/build.py --profile full all
python tools/wasm_native/build.py --profile manifold build-deps
```

## Commands

| Command                | Description                                      |
|------------------------|--------------------------------------------------|
| `doctor`               | Check system prerequisites (cmake, git, etc.)    |
| `bootstrap-toolchain`  | Install pinned Emscripten SDK                    |
| `fetch`                | Download + hash-verify all source archives       |
| `build-deps`           | Build dependencies for the selected profile      |
| `configure`            | Run `emcmake cmake` configure for IfcOpenShell   |
| `build`                | Build IfcOpenShell WASM target                   |
| `test`                 | Run profile-aware tests                          |
| `package`              | Write clean dist directory with artifacts        |
| `clean`                | Remove all build artifacts                       |
| `all`                  | Run full pipeline (default for CI)               |

## Build Layout

```
build/wasm-native/
  toolchain/          # emsdk installation
  downloads/          # cached source archives
  src/                # extracted sources
  build/              # per-dependency build trees
  prefix/<profile>/   # installed dependencies (CMAKE_PREFIX_PATH)
  ifcopenshell/<profile>/  # IfcOpenShell build tree
  dist/               # packaged output artifacts
```

## Output Artifacts

After `package`, the `dist/` directory contains:

- `ifcopenshell_wasm.wasm` — Main WASM module
- `ifcopenshell_wasm.mjs` — Emscripten runtime JS
- `ifcopenshell_api.mjs` — Generated JS API bindings
- `ifcopenshell_api.d.ts` — TypeScript declarations
- `ifcopenshell_plugins.json` — Plugin manifest
- `plugins/*.wasm` — Lazy-loaded side module plugins

## Pinned Sources

All dependency versions and URLs are pinned in `sources.lock.json`.
SHA-256 hashes are verified on download when available.

## Patches

Patches are stored in `patches/` and applied during `build-deps`:

- `occt/no_em_js.patch` — Strips EM_JS macros for SIDE_MODULE compatibility
- `manifold/install-metadata-for-emscripten.patch` — Enables cmake install for Emscripten

## Environment Overrides

- `WASM_NATIVE_TOOLCHAIN` — Path to an external emsdk installation (bypasses `bootstrap-toolchain`)
- `WASM_NATIVE_BUILD_ROOT` — Override the default `build/wasm-native/` build root

## Running Tests

```bash
# Run the bootstrap system's own unit tests
python -m pytest tools/wasm_native/tests/ -v

# Run WASM bindings integration tests (after build)
python tools/wasm_native/build.py test
```

## Requirements

- Python 3.8+
- cmake 3.21+
- git
- make
- patch
- tar, unzip
- Internet access (for initial download)

No Python package dependencies are required — the build system uses
only the Python standard library.
