#!/usr/bin/env python3
# This file was generated with the assistance of an AI coding tool.
"""
Native WASM bootstrap build system for IfcOpenShell.

Builds Emscripten toolchain, third-party dependencies, and IfcOpenShell's
WASM_BUILD target without Pyodide or SWIG.

Usage:
    python tools/wasm_native/build.py <command> [--profile <name>]

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
import hashlib
import json
import logging
import os
import platform
import shutil
import subprocess
import sys
import tarfile
import zipfile
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple
from urllib.request import urlretrieve

# ──────────────────────────────────────────────────────────────────────────────
# Constants
# ──────────────────────────────────────────────────────────────────────────────

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent.parent
BUILD_ROOT = Path(os.environ.get("WASM_NATIVE_BUILD_ROOT", REPO_ROOT / "build" / "wasm-native"))
PATCHES_DIR = SCRIPT_DIR / "patches"

LOCK_FILE = SCRIPT_DIR / "sources.lock.json"
PROFILES_FILE = SCRIPT_DIR / "profiles.json"

CMAKE_DIR = REPO_ROOT / "cmake"

logger = logging.getLogger("wasm-native")


# ──────────────────────────────────────────────────────────────────────────────
# Utilities
# ──────────────────────────────────────────────────────────────────────────────


def run(cmd: List[str], cwd: Optional[Path] = None, env: Optional[Dict[str, str]] = None,
        check: bool = True, capture: bool = False) -> subprocess.CompletedProcess:
    """Run a subprocess command with logging."""
    logger.info("$ %s", " ".join(cmd))
    merged_env = dict(os.environ)
    if env:
        merged_env.update(env)
    return subprocess.run(
        cmd,
        cwd=str(cwd) if cwd else None,
        env=merged_env,
        check=check,
        capture_output=capture,
        text=True,
    )


def sha256_file(path: Path) -> str:
    """Compute SHA-256 hex digest of a file."""
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def download_file(url: str, dest: Path) -> None:
    """Download a file from url to dest with progress."""
    logger.info("Downloading %s -> %s", url, dest)
    dest.parent.mkdir(parents=True, exist_ok=True)

    def _reporthook(block_num: int, block_size: int, total_size: int) -> None:
        downloaded = block_num * block_size
        if total_size > 0:
            pct = min(100.0, downloaded * 100.0 / total_size)
            mb = downloaded / (1 << 20)
            total_mb = total_size / (1 << 20)
            print(f"\r  {pct:5.1f}%  {mb:.1f}/{total_mb:.1f} MB", end="", flush=True)

    urlretrieve(url, str(dest), reporthook=_reporthook)
    print()  # newline after progress


def extract_archive(archive_path: Path, dest_dir: Path, archive_type: str) -> Path:
    """Extract an archive and return the top-level directory path."""
    dest_dir.mkdir(parents=True, exist_ok=True)
    logger.info("Extracting %s -> %s", archive_path, dest_dir)

    if archive_type in ("tar.gz", "tar.bz2", "tar.xz", "tar"):
        with tarfile.open(str(archive_path), "r:*") as tf:
            # Get top-level dir name
            members = tf.getmembers()
            top_dirs = set()
            for m in members:
                parts = m.name.split("/")
                if parts and parts[0]:
                    top_dirs.add(parts[0])
            tf.extractall(str(dest_dir), filter="data")
            if len(top_dirs) == 1:
                return dest_dir / top_dirs.pop()
            return dest_dir
    elif archive_type == "zip":
        with zipfile.ZipFile(str(archive_path), "r") as zf:
            zf.extractall(str(dest_dir))
            top_dirs = set()
            for name in zf.namelist():
                parts = name.split("/")
                if parts and parts[0]:
                    top_dirs.add(parts[0])
            if len(top_dirs) == 1:
                return dest_dir / top_dirs.pop()
            return dest_dir
    else:
        raise ValueError(f"Unsupported archive type: {archive_type}")


def apply_patch(patch_file: Path, target_dir: Path) -> None:
    """Apply a patch file using the patch command."""
    logger.info("Applying patch %s in %s", patch_file, target_dir)
    run(["patch", "-p1", "--forward", "-i", str(patch_file)], cwd=target_dir, check=False)


def which_or_error(name: str) -> str:
    """Find an executable or raise an error."""
    path = shutil.which(name)
    if not path:
        raise RuntimeError(f"Required tool not found: {name}")
    return path


# ──────────────────────────────────────────────────────────────────────────────
# Configuration Loading
# ──────────────────────────────────────────────────────────────────────────────


def load_lockfile() -> Dict[str, Any]:
    """Load and return the sources.lock.json contents."""
    with open(LOCK_FILE) as f:
        return json.load(f)


def load_profiles() -> Dict[str, Any]:
    """Load and return the profiles.json contents."""
    with open(PROFILES_FILE) as f:
        return json.load(f)


def get_profile(name: str) -> Dict[str, Any]:
    """Get a specific profile by name, raising if not found."""
    profiles = load_profiles()
    if name not in profiles:
        available = ", ".join(sorted(profiles.keys()))
        raise ValueError(f"Unknown profile '{name}'. Available: {available}")
    return profiles[name]


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


# ──────────────────────────────────────────────────────────────────────────────
# emsdk helpers
# ──────────────────────────────────────────────────────────────────────────────


def emsdk_env() -> Dict[str, str]:
    """Return environment variables for emsdk activation.

    Respects WASM_NATIVE_TOOLCHAIN env var to use an externally managed emsdk.
    """
    external = os.environ.get("WASM_NATIVE_TOOLCHAIN")
    td = Path(external) if external else toolchain_dir()
    emsdk_env_file = td / "emsdk_env.sh"
    if not emsdk_env_file.exists():
        raise RuntimeError(f"emsdk not found at {td}. Run 'bootstrap-toolchain' first.")

    result = run(
        ["bash", "-c", f"source {emsdk_env_file} && env"],
        capture=True,
        check=True,
    )
    env_vars = {}
    for line in result.stdout.splitlines():
        if "=" in line:
            key, _, value = line.partition("=")
            env_vars[key] = value
    return env_vars


def emsdk_binaries() -> Dict[str, str]:
    """Return paths to key emsdk binaries."""
    external = os.environ.get("WASM_NATIVE_TOOLCHAIN")
    td = Path(external) if external else toolchain_dir()
    emscripten = td / "upstream" / "emscripten"
    return {
        "emcc": str(emscripten / "emcc"),
        "em++": str(emscripten / "em++"),
        "emcmake": str(emscripten / "emcmake"),
        "emconfigure": str(emscripten / "emconfigure"),
        "emar": str(emscripten / "emar"),
        "emsdk": str(td / "emsdk"),
    }


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

    lock = load_lockfile()
    emsdk_entry = lock["emsdk"]
    version = emsdk_entry["version"]
    td = toolchain_dir()

    if td.exists() and (td / "emsdk_env.sh").exists():
        logger.info("emsdk already installed at %s", td)
        if not args.force:
            logger.info("Use --force to reinstall.")
            return 0
        logger.info("Force reinstalling emsdk...")
        shutil.rmtree(td)

    td.mkdir(parents=True, exist_ok=True)

    # Clone emsdk
    logger.info("Cloning emsdk %s...", version)
    run(["git", "clone", "--depth=1", "--branch", version, emsdk_entry["url"], str(td)])

    # Install and activate
    logger.info("Installing emsdk %s...", version)
    run([str(td / "emsdk"), "install", version], cwd=td)

    logger.info("Activating emsdk %s...", version)
    run([str(td / "emsdk"), "activate", version], cwd=td)

    logger.info("emsdk %s installed at %s", version, td)
    return 0


def cmd_fetch(args: argparse.Namespace) -> int:
    """Download + hash-verify all sources."""
    lock = load_lockfile()
    profile = get_profile(args.profile)
    deps = profile["dependencies"]

    dl_dir = downloads_dir()
    dl_dir.mkdir(parents=True, exist_ok=True)

    results = {}
    for dep_name in deps:
        if dep_name not in lock:
            logger.warning("Dependency '%s' not in lockfile, skipping.", dep_name)
            continue

        entry = lock[dep_name]
        url = entry["url"]
        version = entry["version"]
        archive_type = entry["archive_type"]
        expected_sha256 = entry.get("sha256")

        # Determine filename
        if archive_type == "git":
            logger.info("Skipping git-based dependency: %s", dep_name)
            results[dep_name] = "skipped (git)"
            continue

        filename = url.split("/")[-1]
        if not filename:
            filename = f"{dep_name}-{version}.{archive_type}"
        dest = dl_dir / filename

        # Download if needed
        if dest.exists():
            logger.info("Already downloaded: %s", dest)
        else:
            download_file(url, dest)

        # Verify hash
        if expected_sha256:
            actual_sha256 = sha256_file(dest)
            if actual_sha256 != expected_sha256:
                logger.error(
                    "SHA256 mismatch for %s:\n  expected: %s\n  actual:   %s",
                    dep_name, expected_sha256, actual_sha256,
                )
                results[dep_name] = "FAILED (hash mismatch)"
                continue
            logger.info("SHA256 verified: %s", dep_name)
        else:
            logger.warning("No SHA256 for %s, skipping verification.", dep_name)

        results[dep_name] = "ok"

    print("\nFetch Results:")
    print("=" * 40)
    for name, status in results.items():
        print(f"  {name:20s} {status}")

    return 0


def _extract_source(dep_name: str, entry: Dict[str, Any]) -> Optional[Path]:
    """Extract source for a dependency. Returns the source directory path."""
    if entry["archive_type"] == "git":
        return _clone_git_source(dep_name, entry)

    dl_dir = downloads_dir()
    url = entry["url"]
    filename = url.split("/")[-1]
    if not filename:
        filename = f"{dep_name}-{entry['version']}.{entry['archive_type']}"
    archive_path = dl_dir / filename

    if not archive_path.exists():
        raise RuntimeError(f"Archive not found: {archive_path}. Run 'fetch' first.")

    extract_dest = src_dir() / dep_name
    if extract_dest.exists():
        logger.info("Source already extracted: %s", extract_dest)
        # Find the top-level dir
        subdirs = [d for d in extract_dest.iterdir() if d.is_dir()]
        if len(subdirs) == 1:
            return subdirs[0]
        return extract_dest

    return extract_archive(archive_path, extract_dest, entry["archive_type"])


def _clone_git_source(dep_name: str, entry: Dict[str, Any]) -> Path:
    """Clone a git-based source."""
    target = src_dir() / dep_name
    if target.exists():
        logger.info("Git source already cloned: %s", target)
        return target

    target.mkdir(parents=True, exist_ok=True)
    ref = entry.get("ref", entry.get("version"))
    logger.info("Cloning %s @ %s...", entry["url"], ref)
    run(["git", "clone", "--depth=1", "--branch", ref, entry["url"], str(target)])
    return target


def _build_boost_wasm(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build Boost for WASM."""
    if (prefix / "lib").exists() and any((prefix / "lib").glob("*.a")):
        logger.info("Boost already built at %s", prefix)
        return

    prefix.mkdir(parents=True, exist_ok=True)

    # Bootstrap
    bootstrap = src / "bootstrap.sh"
    if bootstrap.exists():
        run([str(bootstrap)], cwd=src, env=env)

    # Build with emscripten toolset
    b2 = src / "b2"
    run(
        [
            str(b2),
            "toolset=emscripten",
            "--with-system",
            "--with-program_options",
            "--with-regex",
            "--with-thread",
            "--with-date_time",
            "--with-iostreams",
            "--with-filesystem",
            "link=static",
            "variant=release",
            "threading=single",
            f"--stagedir={prefix}",
            "stage",
            "-s",
            "NO_BZIP2=1",
        ],
        cwd=src,
        env=env,
    )

    # Convert .bc to .a
    lib_dir = prefix / "lib"
    if lib_dir.exists():
        for bc_file in lib_dir.glob("*.bc"):
            a_file = bc_file.with_suffix(".a")
            run(["emar", "q", str(a_file), str(bc_file)], cwd=src, env=env)

    # Copy headers to prefix (stage only installs libraries)
    src_boost = src / "boost"
    include_dir = prefix / "include"
    if src_boost.exists() and not (include_dir / "boost").exists():
        include_dir.mkdir(parents=True, exist_ok=True)
        shutil.copytree(str(src_boost), str(include_dir / "boost"))


def _build_eigen(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build/install Eigen (header-only)."""
    if (prefix / "include" / "eigen3").exists():
        logger.info("Eigen already installed at %s", prefix)
        return

    build_dir = build_subdir() / "eigen"
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        ["emcmake", "cmake", str(src),
         f"-DCMAKE_INSTALL_PREFIX={prefix}",
         "-DBUILD_TESTING=OFF"],
        cwd=build_dir,
        env=env,
    )
    run(["cmake", "--build", str(build_dir), "--target", "install"], env=env)


def _build_nlohmann_json(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build/install nlohmann_json (header-only)."""
    if (prefix / "include" / "nlohmann").exists():
        logger.info("nlohmann_json already installed at %s", prefix)
        return

    build_dir = build_subdir() / "nlohmann_json"
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        ["emcmake", "cmake", str(src),
         f"-DCMAKE_INSTALL_PREFIX={prefix}",
         "-DJSON_BuildTests=OFF"],
        cwd=build_dir,
        env=env,
    )
    run(["cmake", "--build", str(build_dir), "--target", "install"], env=env)


def _build_gmp(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build GMP for WASM."""
    if (prefix / "lib" / "libgmp.a").exists():
        logger.info("GMP already built at %s", prefix)
        return

    build_dir = build_subdir() / "gmp"
    build_dir.mkdir(parents=True, exist_ok=True)

    # Set HOST_CC for macOS
    build_env = dict(env)
    if platform.system() == "Darwin":
        build_env["HOST_CC"] = "clang"

    run(
        [
            "emconfigure", str(src / "configure"),
            "--host=wasm32",
            "--disable-assembly",
            "--enable-cxx",
            "--enable-static",
            "--disable-shared",
            "--with-pic",
            f"--prefix={prefix}",
        ],
        cwd=build_dir,
        env=build_env,
    )

    # Patch config.h
    config_h = build_dir / "config.h"
    if config_h.exists():
        content = config_h.read_text()
        content = content.replace("HAVE_OBSTACK_VPRINTF 1", "HAVE_OBSTACK_VPRINTF 0")
        config_h.write_text(content)

    run(["make", f"-j{os.cpu_count() or 1}"], cwd=build_dir, env=build_env)
    run(["make", "install"], cwd=build_dir, env=build_env)


def _build_mpfr(src: Path, prefix: Path, gmp_prefix: Path, env: Dict[str, str]) -> None:
    """Build MPFR for WASM."""
    if (prefix / "lib" / "libmpfr.a").exists():
        logger.info("MPFR already built at %s", prefix)
        return

    build_dir = build_subdir() / "mpfr"
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        [
            "emconfigure", str(src / "configure"),
            "--host=none",
            "--enable-static",
            "--disable-shared",
            "--with-pic",
            f"--with-gmp={gmp_prefix}",
            f"--prefix={prefix}",
        ],
        cwd=build_dir,
        env=env,
    )

    run(["make", f"-j{os.cpu_count() or 1}"], cwd=build_dir, env=env)
    run(["make", "install"], cwd=build_dir, env=env)


def _build_cgal(src: Path, prefix: Path, gmp_prefix: Path, mpfr_prefix: Path,
                env: Dict[str, str]) -> None:
    """Build CGAL for WASM (header-only with GMP/MPFR)."""
    if (prefix / "lib" / "cmake" / "CGAL").exists():
        logger.info("CGAL already built at %s", prefix)
        return

    build_dir = build_subdir() / "cgal"
    build_dir.mkdir(parents=True, exist_ok=True)

    lib_ext = "a"
    run(
        [
            "emcmake", "cmake", str(src),
            f"-DCMAKE_INSTALL_PREFIX={prefix}",
            "-DCGAL_HEADER_ONLY=On",
            "-DBUILD_SHARED_LIBS=Off",
            f"-DGMP_LIBRARIES={gmp_prefix}/lib/libgmp.{lib_ext}",
            f"-DGMP_INCLUDE_DIR={gmp_prefix}/include",
            f"-DMPFR_LIBRARIES={mpfr_prefix}/lib/libmpfr.{lib_ext}",
            f"-DMPFR_INCLUDE_DIR={mpfr_prefix}/include",
        ],
        cwd=build_dir,
        env=env,
    )

    run(["cmake", "--build", str(build_dir), "--target", "install"], env=env)


def _build_occt(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build OpenCASCADE for WASM."""
    if (prefix / "lib" / "cmake" / "opencascade").exists():
        logger.info("OCCT already built at %s", prefix)
        return

    # Apply patches
    lock = load_lockfile()
    entry = lock["occt"]
    for patch_rel in entry.get("patches", []):
        patch_file = PATCHES_DIR / patch_rel
        if patch_file.exists():
            apply_patch(patch_file, src)

    build_dir = build_subdir() / "occt"
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        [
            "emcmake", "cmake", str(src),
            f"-DINSTALL_DIR={prefix}",
            "-DCMAKE_BUILD_TYPE=MinSizeRel",
            "-DBUILD_LIBRARY_TYPE=Static",
            "-DBUILD_MODULE_Draw=0",
            "-DBUILD_MODULE_DETools=OFF",
            "-DBUILD_RELEASE_DISABLE_EXCEPTIONS=Off",
            "-DCMAKE_CXX_FLAGS=-fwasm-exceptions -sSUPPORT_LONGJMP=wasm",
            "-DUSE_XLIB=OFF",
            "-DUSE_FREETYPE=OFF",
            "-DUSE_OPENGL=OFF",
            "-DUSE_GLES2=OFF",
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
        ],
        cwd=build_dir,
        env=env,
    )

    run(["cmake", "--build", str(build_dir), "--parallel", "2"], env=env)
    run(["cmake", "--build", str(build_dir), "--target", "install"], env=env)


def _build_manifold(src: Path, prefix: Path, env: Dict[str, str]) -> None:
    """Build Manifold for WASM."""
    if (prefix / "lib" / "cmake" / "manifold").exists():
        logger.info("Manifold already built at %s", prefix)
        return

    # Apply patches
    lock = load_lockfile()
    entry = lock["manifold"]
    for patch_rel in entry.get("patches", []):
        patch_file = PATCHES_DIR / patch_rel
        if patch_file.exists():
            apply_patch(patch_file, src)

    build_dir = build_subdir() / "manifold"
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        [
            "emcmake", "cmake", str(src),
            f"-DCMAKE_INSTALL_PREFIX={prefix}",
            "-DCMAKE_BUILD_TYPE=MinSizeRel",
            "-DMANIFOLD_PAR=OFF",
            "-DMANIFOLD_CROSS_SECTION=OFF",
            "-DMANIFOLD_PYBIND=OFF",
            "-DMANIFOLD_JSBIND=OFF",
            "-DMANIFOLD_CBIND=OFF",
            "-DMANIFOLD_TEST=OFF",
            "-DMANIFOLD_EXPORT=OFF",
            "-DMANIFOLD_DOWNLOADS=OFF",
        ],
        cwd=build_dir,
        env=env,
    )

    run(["cmake", "--build", str(build_dir), "--parallel", "2"], env=env)
    run(["cmake", "--build", str(build_dir), "--target", "install"], env=env)


def cmd_build_deps(args: argparse.Namespace) -> int:
    """Build dependencies for a profile."""
    lock = load_lockfile()
    profile = get_profile(args.profile)
    deps = profile["dependencies"]

    # Ensure emsdk is available
    try:
        env = emsdk_env()
    except RuntimeError as e:
        logger.error("%s", e)
        return 1

    for dep_name in deps:
        if dep_name not in lock:
            logger.warning("Dependency '%s' not in lockfile, skipping.", dep_name)
            continue

        entry = lock[dep_name]
        logger.info("Building %s %s...", dep_name, entry["version"])

        # Extract source
        src = _extract_source(dep_name, entry)
        if src is None:
            logger.error("Failed to extract source for %s", dep_name)
            return 1

        profile_prefix = prefix_dir(args.profile)

        # Build based on dependency type
        if dep_name == "boost":
            _build_boost_wasm(src, profile_prefix / "boost", env)
        elif dep_name == "eigen":
            _build_eigen(src, profile_prefix / "eigen", env)
        elif dep_name == "nlohmann_json":
            _build_nlohmann_json(src, profile_prefix / "nlohmann_json", env)
        elif dep_name == "gmp":
            _build_gmp(src, profile_prefix / "gmp", env)
        elif dep_name == "mpfr":
            gmp_prefix = profile_prefix / "gmp"
            _build_mpfr(src, profile_prefix / "mpfr", gmp_prefix, env)
        elif dep_name == "cgal":
            gmp_prefix = profile_prefix / "gmp"
            mpfr_prefix = profile_prefix / "mpfr"
            _build_cgal(src, profile_prefix / "cgal", gmp_prefix, mpfr_prefix, env)
        elif dep_name == "occt":
            _build_occt(src, profile_prefix / "occt", env)
        elif dep_name == "manifold":
            _build_manifold(src, profile_prefix / "manifold", env)
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
        "-DCMAKE_BUILD_TYPE=MinSizeRel",
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
    run(cmake_cmd, cwd=build_dir, env=env)

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
    run(
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
    out.mkdir(parents=True)

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
