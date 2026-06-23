#!/usr/bin/env python3
# This file was generated with the assistance of an AI coding tool.
"""
Native WASM bootstrap build system for IfcOpenShell.

Builds Emscripten toolchain, third-party dependencies, and IfcOpenShell's
WASM_BUILD target without Pyodide or SWIG.

This is the consolidated entry point: shared utilities live in `nix/core.py`
and dependency build recipes live in `nix/deps.py`. Pinned source versions
and patches live in `nix/sources.lock.json`.

Usage:
    python nix/wasm_native.py <command> [--profile <name>]

Commands:
    doctor              Check system prerequisites
    bootstrap-toolchain Install pinned emsdk
    fetch               Download + hash-verify all sources
    build-deps          Build dependencies for a profile
    configure           Run emcmake cmake configure for IfcOpenShell
    build               Build IfcOpenShell WASM target
    test                Run profile-aware tests
    package             Write clean dist directory
    clean               Clean build artifacts
    all                 Full pipeline (default for CI)
"""

import argparse
import json
import logging
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

# Allow `python nix/wasm_native.py` direct invocation as well as
# `python -m nix.wasm_native`.
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from nix import core, deps  # noqa: E402

# ──────────────────────────────────────────────────────────────────────────────
# Constants
# ──────────────────────────────────────────────────────────────────────────────

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
BUILD_ROOT = Path(os.environ.get("WASM_NATIVE_BUILD_ROOT", REPO_ROOT / "build" / "wasm-native"))
CMAKE_DIR = REPO_ROOT / "cmake"

logger = logging.getLogger("wasm-native")


# ──────────────────────────────────────────────────────────────────────────────
# Profiles (inline — replaces tools/wasm_native/profiles.json)
# ──────────────────────────────────────────────────────────────────────────────

PROFILES: Dict[str, Dict[str, Any]] = {
    "minimal": {
        "description": "Passthrough kernel only (current working state)",
        "enabled_kernels": ["passthrough"],
        "dependencies": ["boost", "eigen", "nlohmann_json"],
        "cmake_flags": {
            "WITH_OPENCASCADE": "OFF",
            "WITH_CGAL": "OFF",
            "WITH_MANIFOLD": "OFF",
        },
        "test_expectations": {
            "schema_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "kernel_plugins": ["passthrough"],
            "mapping_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "geometry_serializer_plugins": ["obj"],
            "document_serializer_plugins": [],
        },
    },
    "manifold": {
        "description": "Manifold geometry kernel",
        "enabled_kernels": ["passthrough", "manifold"],
        "dependencies": ["boost", "eigen", "nlohmann_json", "manifold"],
        "cmake_flags": {
            "WITH_OPENCASCADE": "OFF",
            "WITH_CGAL": "OFF",
            "WITH_MANIFOLD": "ON",
        },
        "test_expectations": {
            "schema_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "kernel_plugins": ["passthrough", "manifold"],
            "mapping_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "geometry_serializer_plugins": ["obj"],
            "document_serializer_plugins": [],
        },
    },
    "cgal": {
        "description": "CGAL geometry kernel with GMP/MPFR",
        "enabled_kernels": ["passthrough", "cgal", "cgalsimple"],
        "dependencies": ["boost", "eigen", "nlohmann_json", "gmp", "mpfr", "cgal"],
        "cmake_flags": {
            "WITH_OPENCASCADE": "OFF",
            "WITH_CGAL": "ON",
            "WITH_MANIFOLD": "OFF",
            "CGAL_WITH_GMPXX": "Off",
        },
        "test_expectations": {
            "schema_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "kernel_plugins": ["passthrough", "cgal", "cgalsimple"],
            "mapping_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "geometry_serializer_plugins": ["obj"],
            "document_serializer_plugins": [],
        },
    },
    "occt": {
        "description": "OpenCASCADE geometry kernel",
        "enabled_kernels": ["passthrough", "opencascade"],
        "dependencies": ["boost", "eigen", "nlohmann_json", "occt"],
        "cmake_flags": {
            "WITH_OPENCASCADE": "ON",
            "WITH_CGAL": "OFF",
            "WITH_MANIFOLD": "OFF",
        },
        "test_expectations": {
            "schema_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "kernel_plugins": ["passthrough", "opencascade"],
            "tree_plugins": ["opencascade.brep", "opencascade.trianglebvh"],
            "mapping_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "geometry_serializer_plugins": ["obj"],
            "document_serializer_plugins": [],
        },
    },
    "full": {
        "description": "All three geometry kernels",
        "enabled_kernels": ["passthrough", "opencascade", "cgal", "cgalsimple", "manifold"],
        "dependencies": ["boost", "eigen", "nlohmann_json", "occt", "gmp", "mpfr", "cgal", "manifold"],
        "cmake_flags": {
            "WITH_OPENCASCADE": "ON",
            "WITH_CGAL": "ON",
            "WITH_MANIFOLD": "ON",
            "CGAL_WITH_GMPXX": "Off",
        },
        "test_expectations": {
            "schema_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "kernel_plugins": ["passthrough", "opencascade", "cgal", "cgalsimple", "manifold"],
            "tree_plugins": ["opencascade.brep", "opencascade.trianglebvh"],
            "mapping_plugins": ["ifc2x3", "ifc4", "ifc4x3_add2"],
            "geometry_serializer_plugins": ["obj"],
            "document_serializer_plugins": [],
        },
    },
}


def get_profile(name: str) -> Dict[str, Any]:
    """Get a specific profile by name, raising if not found."""
    if name not in PROFILES:
        available = ", ".join(sorted(PROFILES.keys()))
        raise ValueError(f"Unknown profile '{name}'. Available: {available}")
    return PROFILES[name]


def load_profiles() -> Dict[str, Dict[str, Any]]:
    """Return all profile definitions (inline)."""
    return PROFILES


# ──────────────────────────────────────────────────────────────────────────────
# Path helpers
# ──────────────────────────────────────────────────────────────────────────────


def toolchain_dir() -> Path:
    return BUILD_ROOT / "toolchain"


def downloads_dir() -> Path:
    return BUILD_ROOT / "downloads"


def src_dir() -> Path:
    return BUILD_ROOT / "src"


def build_subdir() -> Path:
    return BUILD_ROOT / "build"


def prefix_dir(profile: str) -> Path:
    return BUILD_ROOT / "prefix" / profile


def ifcopenshell_build_dir(profile: str) -> Path:
    return BUILD_ROOT / "ifcopenshell" / profile


def dist_dir() -> Path:
    return BUILD_ROOT / "dist"


def _resolved_toolchain_dir() -> Path:
    """Return the emsdk toolchain dir, honouring WASM_NATIVE_TOOLCHAIN."""
    external = os.environ.get("WASM_NATIVE_TOOLCHAIN")
    return Path(external) if external else toolchain_dir()


# ──────────────────────────────────────────────────────────────────────────────
# emsdk helpers
# ──────────────────────────────────────────────────────────────────────────────


def emsdk_env() -> Dict[str, str]:
    """Return environment variables for emsdk activation.

    Respects WASM_NATIVE_TOOLCHAIN env var to use an externally managed emsdk.
    """
    return core.emsdk_env(_resolved_toolchain_dir())


def emsdk_binaries() -> Dict[str, str]:
    """Return paths to key emsdk binaries."""
    return core.emsdk_binaries(_resolved_toolchain_dir())


# ──────────────────────────────────────────────────────────────────────────────
# Subcommands
# ──────────────────────────────────────────────────────────────────────────────


def cmd_doctor(args: argparse.Namespace) -> int:
    """Check system prerequisites."""
    checks = {
        "cmake": "cmake --version",
        "git": "git --version",
        "python3": f"{sys.executable} --version",
        "make": "make --version",
        "patch": "patch --version",
        "tar": "tar --version",
        "unzip": "unzip -v",
    }

    results = {}
    all_ok = True
    for name, cmd in checks.items():
        try:
            result = subprocess.run(
                cmd.split(),
                capture_output=True,
                text=True,
                timeout=10,
            )
            version = result.stdout.strip().split("\n")[0] if result.returncode == 0 else "FAILED"
            results[name] = (result.returncode == 0, version)
            if result.returncode != 0:
                all_ok = False
        except (FileNotFoundError, subprocess.TimeoutExpired):
            results[name] = (False, "NOT FOUND")
            all_ok = False

    print("System Prerequisites Check:")
    print("=" * 60)
    for name, (ok, version) in results.items():
        status = "✓" if ok else "✗"
        print(f"  {status} {name:12s} {version}")

    print()
    if all_ok:
        print("All prerequisites satisfied.")
        return 0
    else:
        print("Some prerequisites are missing. Install them before proceeding.")
        return 1


def cmd_bootstrap_toolchain(args: argparse.Namespace) -> int:
    """Install pinned emsdk."""
    external = os.environ.get("WASM_NATIVE_TOOLCHAIN")
    if external:
        logger.info("Using external emsdk from WASM_NATIVE_TOOLCHAIN=%s", external)
        emsdk_env_file = Path(external) / "emsdk_env.sh"
        if not emsdk_env_file.exists():
            logger.error("emsdk_env.sh not found at %s", external)
            return 1
        return 0

    lock = core.load_lockfile()
    return core.bootstrap_emsdk(lock, toolchain_dir(), force=args.force)


def cmd_fetch(args: argparse.Namespace) -> int:
    """Download + hash-verify all sources for the selected profile."""
    lock = core.load_lockfile()
    profile = get_profile(args.profile)
    dep_list = profile["dependencies"]

    results = core.fetch_sources(lock, dep_list, downloads_dir())

    print("\nFetch Results:")
    print("=" * 40)
    for name, status in results.items():
        print(f"  {name:20s} {status}")

    return 0


def cmd_build_deps(args: argparse.Namespace) -> int:
    """Build dependencies for a profile."""
    lock = core.load_lockfile()
    profile = get_profile(args.profile)
    dep_list = profile["dependencies"]

    # Ensure emsdk is available
    try:
        env = emsdk_env()
    except RuntimeError as e:
        logger.error("%s", e)
        return 1

    # Point shared deps module at our build root so intermediate build dirs
    # land inside build/wasm-native.
    deps.set_build_root(BUILD_ROOT)

    for dep_name in dep_list:
        if dep_name not in lock:
            logger.warning("Dependency '%s' not in lockfile, skipping.", dep_name)
            continue

        entry = lock[dep_name]
        logger.info("Building %s %s...", dep_name, entry["version"])

        # Fetch (no-op if already downloaded) and extract
        core.fetch_sources(lock, [dep_name], downloads_dir())
        src = core.extract_source(dep_name, lock, src_dir(), downloads_dir())

        profile_prefix = prefix_dir(args.profile)

        if dep_name == "boost":
            deps.build_boost(src, profile_prefix / "boost", env)
        elif dep_name == "eigen":
            deps.build_eigen(src, profile_prefix / "eigen", env)
        elif dep_name == "nlohmann_json":
            deps.build_json(src, profile_prefix / "nlohmann_json", env)
        elif dep_name == "gmp":
            deps.build_gmp(src, profile_prefix / "gmp", env)
        elif dep_name == "mpfr":
            gmp_prefix = profile_prefix / "gmp"
            deps.build_mpfr(src, profile_prefix / "mpfr", gmp_prefix, env)
        elif dep_name == "cgal":
            gmp_prefix = profile_prefix / "gmp"
            mpfr_prefix = profile_prefix / "mpfr"
            boost_prefix = profile_prefix / "boost"
            deps.build_cgal(src, profile_prefix / "cgal", gmp_prefix, mpfr_prefix, env,
                            boost_prefix=boost_prefix)
        elif dep_name == "occt":
            deps.build_occt(src, profile_prefix / "occt", env)
        elif dep_name == "manifold":
            deps.build_manifold(src, profile_prefix / "manifold", env)
        else:
            logger.warning("No build recipe for %s, skipping.", dep_name)

    logger.info("All dependencies built for profile '%s'.", args.profile)
    return 0


def generate_cmake_flags(profile: str) -> List[str]:
    """Generate CMake flags for IfcOpenShell WASM build."""
    p = get_profile(profile)
    pf = prefix_dir(profile)

    # Base flags
    flags = [
        "-DWASM_BUILD=ON",
        "-DBUILD_IFCCAPI=ON",
        "-DBUILD_IFCAPI=ON",
        "-DBUILD_IFCGEOM=ON",
        "-DBUILD_IFCPYTHON=OFF",
        "-DBUILD_CONVERT=OFF",
        "-DBUILD_GEOMSERVER=OFF",
        "-DBUILD_EXAMPLES=OFF",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DCOLLADA_SUPPORT=OFF",
        "-DBoost_NO_BOOST_CMAKE=On",
        "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
        f"-DPYTHON_EXECUTABLE={sys.executable}",
    ]

    # Profile-specific kernel flags
    for key, value in p["cmake_flags"].items():
        flags.append(f"-D{key}={value}")

    # Build CMAKE_FIND_ROOT_PATH from profile prefix
    prefix_paths = []
    for dep_name in p["dependencies"]:
        dep_prefix = pf / dep_name
        if dep_prefix.exists():
            prefix_paths.append(str(dep_prefix))

    if prefix_paths:
        root_path = ";".join(prefix_paths)
        flags.append(f"-DCMAKE_FIND_ROOT_PATH={root_path}")
        flags.append("-DCMAKE_PREFIX_PATH=//")

    # OCCT WASM dir
    occt_prefix = pf / "occt"
    if occt_prefix.exists() and p["cmake_flags"].get("WITH_OPENCASCADE") == "ON":
        flags.append(f"-DOPENCASCADE_WASM_DIR={occt_prefix}")

    return flags


def cmd_configure(args: argparse.Namespace) -> int:
    """Run emcmake cmake configure for IfcOpenShell."""
    profile = args.profile
    build_dir = ifcopenshell_build_dir(profile)
    build_dir.mkdir(parents=True, exist_ok=True)

    try:
        env = emsdk_env()
    except RuntimeError as e:
        logger.error("%s", e)
        return 1

    cmake_flags = generate_cmake_flags(profile)
    cmake_cmd = ["emcmake", "cmake", str(CMAKE_DIR)] + cmake_flags

    logger.info("Configuring IfcOpenShell for profile '%s'...", profile)
    core.run(cmake_cmd, cwd=build_dir, env=env)

    logger.info("Configuration complete. Build directory: %s", build_dir)
    return 0


def cmd_build(args: argparse.Namespace) -> int:
    """Build IfcOpenShell WASM target."""
    profile = args.profile
    build_dir = ifcopenshell_build_dir(profile)

    if not (build_dir / "CMakeCache.txt").exists():
        logger.error("Build not configured. Run 'configure' first.")
        return 1

    try:
        env = emsdk_env()
    except RuntimeError as e:
        logger.error("%s", e)
        return 1

    nproc = os.cpu_count() or 1
    logger.info("Building IfcOpenShell WASM (profile=%s, jobs=%d)...", profile, nproc)
    core.run(
        ["cmake", "--build", str(build_dir), "--parallel", str(nproc)],
        env=env,
    )

    logger.info("Build complete.")
    return 0


def cmd_test(args: argparse.Namespace) -> int:
    """Run profile-aware tests."""
    profile_name = args.profile
    profile = get_profile(profile_name)
    build_dir = ifcopenshell_build_dir(profile_name)

    wasm_dir = build_dir / "ifcwrap" / "wasm"
    plugins_json = wasm_dir / "ifcopenshell_plugins.json"

    if not plugins_json.exists():
        logger.error("Plugin manifest not found: %s", plugins_json)
        return 1

    with open(plugins_json) as f:
        manifest = json.load(f)

    expectations = profile.get("test_expectations", {})
    errors = []

    # Verify kernel plugins
    expected_kernels = expectations.get("kernel_plugins", [])
    actual_kernels = list(manifest.get("kernel", {}).keys())
    for k in expected_kernels:
        if k not in actual_kernels:
            errors.append(f"Missing kernel plugin: {k}")

    # Verify schema plugins
    expected_schemas = expectations.get("schema_plugins", [])
    actual_schemas = list(manifest.get("schema", {}).keys())
    for s in expected_schemas:
        if s not in actual_schemas:
            errors.append(f"Missing schema plugin: {s}")

    # Verify mapping plugins
    expected_mappings = expectations.get("mapping_plugins", [])
    actual_mappings = list(manifest.get("mapping", {}).keys())
    for m in expected_mappings:
        if m not in actual_mappings:
            errors.append(f"Missing mapping plugin: {m}")

    # Verify geometry serializer plugins
    expected_serializers = expectations.get("geometry_serializer_plugins", [])
    actual_serializers = list(manifest.get("geometry_serializer", {}).keys())
    for s in expected_serializers:
        if s not in actual_serializers:
            errors.append(f"Missing geometry_serializer plugin: {s}")

    # Verify tree plugins
    expected_trees = expectations.get("tree_plugins", [])
    actual_trees = list(manifest.get("tree", {}).keys())
    for t in expected_trees:
        if t not in actual_trees:
            errors.append(f"Missing tree plugin: {t}")

    if errors:
        logger.error("Test failures:")
        for e in errors:
            logger.error("  - %s", e)
        return 1

    # Verify output artifacts exist
    artifacts = [
        "ifcopenshell_wasm.wasm",
        "ifcopenshell_wasm.mjs",
        "ifcopenshell_api.mjs",
        "ifcopenshell_api.d.ts",
        "ifcopenshell_plugins.json",
    ]
    for name in artifacts:
        artifact_path = wasm_dir / name
        if not artifact_path.exists():
            errors.append(f"Missing artifact: {name}")

    # Verify plugin .wasm files exist
    plugins_dir = wasm_dir / "plugins"
    if plugins_dir.exists():
        plugin_files = list(plugins_dir.glob("*.wasm"))
        logger.info("Found %d plugin .wasm files", len(plugin_files))
    else:
        errors.append("plugins/ directory missing")

    if errors:
        logger.error("Artifact verification failures:")
        for e in errors:
            logger.error("  - %s", e)
        return 1

    logger.info("All tests passed for profile '%s'.", profile_name)
    return 0


def cmd_package(args: argparse.Namespace) -> int:
    """Write clean dist directory."""
    profile = args.profile
    build_dir = ifcopenshell_build_dir(profile)
    wasm_dir = build_dir / "ifcwrap" / "wasm"
    out = dist_dir()

    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True, exist_ok=True)

    # Copy artifacts
    artifacts = [
        "ifcopenshell_wasm.wasm",
        "ifcopenshell_wasm.mjs",
        "ifcopenshell_api.mjs",
        "ifcopenshell_api.d.ts",
        "ifcopenshell_plugins.json",
    ]

    for name in artifacts:
        src = wasm_dir / name
        if src.exists():
            shutil.copy2(src, out / name)
            logger.info("Copied: %s", name)
        else:
            logger.warning("Artifact not found: %s", src)

    # Copy plugins directory
    plugins_dir = wasm_dir / "plugins"
    if plugins_dir.exists():
        dest_plugins = out / "plugins"
        shutil.copytree(plugins_dir, dest_plugins)
        logger.info("Copied: plugins/")

    logger.info("Package written to %s", out)
    return 0


def cmd_clean(args: argparse.Namespace) -> int:
    """Clean build artifacts."""
    if BUILD_ROOT.exists():
        logger.info("Removing %s", BUILD_ROOT)
        shutil.rmtree(BUILD_ROOT)
        logger.info("Clean complete.")
    else:
        logger.info("Nothing to clean.")
    return 0


def cmd_all(args: argparse.Namespace) -> int:
    """Full pipeline (default for CI)."""
    steps = [
        ("doctor", cmd_doctor),
        ("bootstrap-toolchain", cmd_bootstrap_toolchain),
        ("fetch", cmd_fetch),
        ("build-deps", cmd_build_deps),
        ("configure", cmd_configure),
        ("build", cmd_build),
        ("test", cmd_test),
        ("package", cmd_package),
    ]

    for name, func in steps:
        logger.info("=" * 60)
        logger.info("Step: %s", name)
        logger.info("=" * 60)
        rc = func(args)
        if rc != 0:
            logger.error("Step '%s' failed with exit code %d", name, rc)
            return rc

    logger.info("Full pipeline completed successfully.")
    return 0


# ──────────────────────────────────────────────────────────────────────────────
# CLI
# ──────────────────────────────────────────────────────────────────────────────


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Native WASM bootstrap build system for IfcOpenShell",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--profile", "-p",
        default="minimal",
        help="Build profile name (default: minimal)",
    )
    parser.add_argument(
        "--force", "-f",
        action="store_true",
        help="Force reinstall/overwrite",
    )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose logging",
    )

    sub = parser.add_subparsers(dest="command", help="Available commands")

    sub.add_parser("doctor", help="Check system prerequisites")
    sub.add_parser("bootstrap-toolchain", help="Install pinned emsdk")
    sub.add_parser("fetch", help="Download + hash-verify all sources")
    sub.add_parser("build-deps", help="Build dependencies for a profile")
    sub.add_parser("configure", help="Run emcmake cmake configure")
    sub.add_parser("build", help="Build IfcOpenShell WASM target")
    sub.add_parser("test", help="Run profile-aware tests")
    sub.add_parser("package", help="Write clean dist directory")
    sub.add_parser("clean", help="Clean build artifacts")
    sub.add_parser("all", help="Full pipeline (default for CI)")

    return parser


def main(argv: Optional[List[str]] = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format="%(asctime)s [%(levelname)s] %(message)s",
        datefmt="%H:%M:%S",
    )

    commands = {
        "doctor": cmd_doctor,
        "bootstrap-toolchain": cmd_bootstrap_toolchain,
        "fetch": cmd_fetch,
        "build-deps": cmd_build_deps,
        "configure": cmd_configure,
        "build": cmd_build,
        "test": cmd_test,
        "package": cmd_package,
        "clean": cmd_clean,
        "all": cmd_all,
    }

    if not args.command:
        # Default to 'all' for CI
        args.command = "all"

    if args.command not in commands:
        parser.print_help()
        return 1

    return commands[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
