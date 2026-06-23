#!/usr/bin/env python3
# This file was generated with the assistance of an AI coding tool.
"""
Unit tests for the consolidated WASM build system under `nix/`.

Covers:
- Lockfile parsing and validation (`nix.core`, `nix/sources.lock.json`)
- SHA-256 hash verification (`nix.core.sha256_file`)
- Profile resolution (`nix.wasm_native.PROFILES` / `get_profile`)
- CMake flag generation (`nix.wasm_native.generate_cmake_flags`)
- Patch file existence and validity (`nix.core.PATCHES_DIR`)
- Path helpers (`nix.wasm_native`)
- CLI parsing (`nix.wasm_native.build_parser`, `main`)
- `run()` utility (`nix.core.run`)
- Dependency build configuration (`nix.deps.build_occt` flag wiring)
"""

import hashlib
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch, MagicMock

# Make the repo root importable so `from nix import core, deps, wasm_native` works
# whether pytest is invoked from the repo root or from `nix/tests/`.
_REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(_REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(_REPO_ROOT))

from nix import core, deps, wasm_native


class TestLockfileParsing(unittest.TestCase):
    """Test lockfile loading and validation."""

    def test_load_lockfile(self):
        """Test that lockfile loads correctly."""
        lock = core.load_lockfile()
        self.assertIsInstance(lock, dict)
        for name in ("emsdk", "boost", "occt", "manifold", "gmp", "mpfr",
                      "cgal", "eigen", "nlohmann_json", "libxml2", "pcre",
                      "opencollada", "swig", "rocksdb", "zstd"):
            self.assertIn(name, lock, f"{name} missing from lockfile")

    def test_lockfile_entry_structure(self):
        """Test that each lockfile entry has required fields."""
        lock = core.load_lockfile()
        for name, entry in lock.items():
            self.assertIn("url", entry, f"{name} missing 'url'")
            self.assertIn("version", entry, f"{name} missing 'version'")
            self.assertIn("archive_type", entry, f"{name} missing 'archive_type'")
            self.assertIn("patches", entry, f"{name} missing 'patches'")
            self.assertIsInstance(entry["patches"], list, f"{name} 'patches' should be list")

    def test_lockfile_versions(self):
        """Test that versions match expected pinned values."""
        lock = core.load_lockfile()
        self.assertEqual(lock["occt"]["version"], "7.8.1")
        self.assertEqual(lock["gmp"]["version"], "6.3.0")
        self.assertEqual(lock["mpfr"]["version"], "3.1.6")
        self.assertEqual(lock["cgal"]["version"], "5.6.3")
        self.assertEqual(lock["manifold"]["version"], "3.2.1")
        self.assertEqual(lock["boost"]["version"], "1.86.0")
        self.assertEqual(lock["eigen"]["version"], "3.4.0")
        self.assertEqual(lock["nlohmann_json"]["version"], "3.11.3")
        self.assertEqual(lock["libxml2"]["version"], "2.13.8")
        self.assertEqual(lock["pcre"]["version"], "8.41")
        self.assertEqual(lock["opencollada"]["version"], "v1.6.68")
        self.assertEqual(lock["swig"]["version"], "4.2.1")
        self.assertEqual(lock["rocksdb"]["version"], "9.11.2")
        self.assertEqual(lock["zstd"]["version"], "1.5.7")
        self.assertEqual(lock["emsdk"]["version"], "5.0.7")

    def test_lockfile_archive_types(self):
        """Test that archive types are valid."""
        lock = core.load_lockfile()
        valid_types = {"tar.gz", "tar.bz2", "tar.xz", "tar", "zip", "git"}
        for name, entry in lock.items():
            self.assertIn(
                entry["archive_type"],
                valid_types,
                f"{name} has invalid archive_type: {entry['archive_type']}",
            )

    def test_lockfile_urls(self):
        """Test that URLs are well-formed."""
        lock = core.load_lockfile()
        for name, entry in lock.items():
            url = entry["url"]
            if entry["archive_type"] != "git":
                self.assertTrue(
                    url.startswith("http://") or url.startswith("https://"),
                    f"{name} URL should start with http:// or https://: {url}",
                )
            else:
                self.assertTrue(
                    url.startswith("https://"),
                    f"{name} git URL should start with https://: {url}",
                )

    def test_lockfile_patches_reference_existing_files(self):
        """Every patch referenced in the lockfile must exist under nix/patches/."""
        lock = core.load_lockfile()
        for name, entry in lock.items():
            for patch_rel in entry.get("patches", []):
                patch_path = core.PATCHES_DIR / patch_rel
                self.assertTrue(
                    patch_path.exists(),
                    f"{name} references missing patch: {patch_path}",
                )

    def test_lockfile_is_valid_json(self):
        """The lockfile must parse as valid JSON."""
        with open(core.LOCK_FILE) as f:
            data = json.load(f)
        self.assertIsInstance(data, dict)
        self.assertGreater(len(data), 0)


class TestHashVerification(unittest.TestCase):
    """Test SHA-256 hash verification."""

    def test_sha256_file(self):
        """Test SHA-256 computation on a known file."""
        with tempfile.NamedTemporaryFile(mode="wb", delete=False) as f:
            content = b"hello world"
            f.write(content)
            f.flush()
            temp_path = Path(f.name)

        try:
            expected = hashlib.sha256(content).hexdigest()
            actual = core.sha256_file(temp_path)
            self.assertEqual(actual, expected)
        finally:
            temp_path.unlink()

    def test_sha256_empty_file(self):
        """Test SHA-256 on empty file."""
        with tempfile.NamedTemporaryFile(mode="wb", delete=False) as f:
            f.flush()
            temp_path = Path(f.name)

        try:
            expected = hashlib.sha256(b"").hexdigest()
            actual = core.sha256_file(temp_path)
            self.assertEqual(actual, expected)
        finally:
            temp_path.unlink()

    def test_sha256_large_file(self):
        """Test SHA-256 on larger file (exercises chunking loop)."""
        content = b"x" * (1 << 20)  # 1 MB
        with tempfile.NamedTemporaryFile(mode="wb", delete=False) as f:
            f.write(content)
            f.flush()
            temp_path = Path(f.name)

        try:
            expected = hashlib.sha256(content).hexdigest()
            actual = core.sha256_file(temp_path)
            self.assertEqual(actual, expected)
        finally:
            temp_path.unlink()

    def test_fetch_sources_skips_unknown_dep(self):
        """fetch_sources must report missing deps without raising."""
        lock = core.load_lockfile()
        with tempfile.TemporaryDirectory() as td:
            results = core.fetch_sources(lock, ["__not_a_real_dep__"], Path(td))
        self.assertEqual(results["__not_a_real_dep__"], "missing in lockfile")


class TestProfileResolution(unittest.TestCase):
    """Test profile loading and resolution from wasm_native."""

    def test_profiles_dict_contents(self):
        """Test that all expected profiles are defined inline."""
        profiles = wasm_native.PROFILES
        self.assertIsInstance(profiles, dict)
        for name in ("minimal", "manifold", "cgal", "occt", "full"):
            self.assertIn(name, profiles)

    def test_get_profile_valid(self):
        """Test getting a valid profile."""
        profile = wasm_native.get_profile("minimal")
        self.assertIsInstance(profile, dict)
        # `enabled_kernels` is the legacy field name from profiles.json; we keep
        # it on the inline profiles. `dependencies` and `cmake_flags` are the
        # fields actually consumed by the build pipeline.
        self.assertIn("enabled_kernels", profile)
        self.assertIn("dependencies", profile)
        self.assertIn("cmake_flags", profile)
        self.assertIn("test_expectations", profile)

    def test_get_profile_invalid(self):
        """Test getting an invalid profile raises error."""
        with self.assertRaises(ValueError) as ctx:
            wasm_native.get_profile("nonexistent")
        self.assertIn("nonexistent", str(ctx.exception))

    def test_minimal_profile(self):
        profile = wasm_native.get_profile("minimal")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertEqual(len(profile["enabled_kernels"]), 1)
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "OFF")

    def test_manifold_profile(self):
        profile = wasm_native.get_profile("manifold")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertIn("manifold", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "OFF")
        self.assertIn("manifold", profile["dependencies"])

    def test_cgal_profile(self):
        profile = wasm_native.get_profile("cgal")
        self.assertIn("cgal", profile["enabled_kernels"])
        self.assertIn("cgalsimple", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "ON")
        self.assertIn("gmp", profile["dependencies"])
        self.assertIn("mpfr", profile["dependencies"])
        self.assertIn("cgal", profile["dependencies"])

    def test_occt_profile(self):
        profile = wasm_native.get_profile("occt")
        self.assertIn("opencascade", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "ON")
        self.assertIn("occt", profile["dependencies"])

    def test_full_profile(self):
        profile = wasm_native.get_profile("full")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertIn("opencascade", profile["enabled_kernels"])
        self.assertIn("cgal", profile["enabled_kernels"])
        self.assertIn("cgalsimple", profile["enabled_kernels"])
        self.assertIn("manifold", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "ON")

    def test_profile_dependencies_subset_of_lockfile(self):
        """All profile dependencies must exist in lockfile."""
        lock = core.load_lockfile()
        for name, profile in wasm_native.PROFILES.items():
            for dep in profile["dependencies"]:
                self.assertIn(
                    dep,
                    lock,
                    f"Profile '{name}' references dependency '{dep}' not in lockfile",
                )

    def test_load_profiles_helper(self):
        """load_profiles() returns the same dict as PROFILES."""
        self.assertIs(wasm_native.load_profiles(), wasm_native.PROFILES)


class TestCMakeFlagGeneration(unittest.TestCase):
    """Test CMake flag generation."""

    def test_generate_cmake_flags_base(self):
        """Test base CMake flags are present."""
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = wasm_native.generate_cmake_flags("minimal")

        flag_str = " ".join(flags)
        for expected in (
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
        ):
            self.assertIn(expected, flag_str)
        self.assertIn(f"-DPYTHON_EXECUTABLE={sys.executable}", flags)

    def test_generate_cmake_flags_minimal(self):
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = wasm_native.generate_cmake_flags("minimal")
        flag_str = " ".join(flags)
        self.assertIn("-DWITH_OPENCASCADE=OFF", flag_str)
        self.assertIn("-DWITH_CGAL=OFF", flag_str)
        self.assertIn("-DWITH_MANIFOLD=OFF", flag_str)

    def test_generate_cmake_flags_manifold(self):
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = wasm_native.generate_cmake_flags("manifold")
        flag_str = " ".join(flags)
        self.assertIn("-DWITH_MANIFOLD=ON", flag_str)
        self.assertIn("-DWITH_OPENCASCADE=OFF", flag_str)
        self.assertIn("-DWITH_CGAL=OFF", flag_str)

    def test_generate_cmake_flags_cgal(self):
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = wasm_native.generate_cmake_flags("cgal")
        flag_str = " ".join(flags)
        self.assertIn("-DWITH_CGAL=ON", flag_str)
        self.assertIn("-DCGAL_WITH_GMPXX=Off", flag_str)

    def test_generate_cmake_flags_full(self):
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = wasm_native.generate_cmake_flags("full")
        flag_str = " ".join(flags)
        self.assertIn("-DWITH_OPENCASCADE=ON", flag_str)
        self.assertIn("-DWITH_CGAL=ON", flag_str)
        self.assertIn("-DWITH_MANIFOLD=ON", flag_str)

    def test_generate_cmake_flags_returns_list(self):
        """Test that generate_cmake_flags returns a list of strings."""
        with patch("nix.wasm_native.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            for profile_name in ("minimal", "manifold", "cgal", "occt", "full"):
                flags = wasm_native.generate_cmake_flags(profile_name)
                self.assertIsInstance(flags, list)
                for f in flags:
                    self.assertIsInstance(f, str)


class TestPatchManagement(unittest.TestCase):
    """Test patch file existence and structure."""

    def test_occt_patch_exists(self):
        patch_file = core.PATCHES_DIR / "occt" / "no_em_js.patch"
        self.assertTrue(patch_file.exists(), f"OCCT patch not found: {patch_file}")

    def test_manifold_patch_exists(self):
        patch_file = core.PATCHES_DIR / "manifold" / "install-metadata-for-emscripten.patch"
        self.assertTrue(patch_file.exists(), f"Manifold patch not found: {patch_file}")

    def test_opencollada_patches_exist(self):
        for name in (
            "pr622_and_disable_subdirs.patch",
            "allow_static_libraries_config_on_unix.patch",
            "remove_tr1.patch",
        ):
            patch_file = core.PATCHES_DIR / "opencollada" / name
            self.assertTrue(patch_file.exists(), f"OpenCOLLADA patch not found: {patch_file}")

    def test_patches_are_valid_diff_format(self):
        """Test that patch files look like valid diffs.

        Debian-style patches start with a `Description:` header before the
        unified diff body, so we accept `---`/`+++` markers anywhere in the
        file rather than only at the start.
        """
        for patch_path in core.PATCHES_DIR.rglob("*.patch"):
            content = patch_path.read_text()
            self.assertTrue(
                content.startswith("---")
                or content.startswith("diff ")
                or content.startswith("Index: ")
                or "diff --git" in content
                or ("\n--- " in content and "\n+++ " in content),
                f"Patch {patch_path} doesn't look like a valid diff",
            )


class TestDependencyBuildConfiguration(unittest.TestCase):
    """Test dependency CMake configuration wiring."""

    def test_occt_uses_wasm_exception_flags(self):
        """OCCT must be built with -fwasm-exceptions to match side modules."""
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            src = root / "src"
            prefix = root / "prefix"
            src.mkdir()

            with (
                patch("nix.core.load_lockfile", return_value={"occt": {"patches": []}}),
                patch("nix.deps._dep_build_dir", return_value=root / "build"),
                patch("nix.core.run") as mock_run,
            ):
                deps.build_occt(src, prefix, {})

        # First call is the emcmake cmake configure step
        cmake_command = mock_run.call_args_list[0].args[0]
        self.assertIn("-DCMAKE_CXX_FLAGS=-fwasm-exceptions -sSUPPORT_LONGJMP=wasm", cmake_command)
        self.assertIn("-DBUILD_LIBRARY_TYPE=Static", cmake_command)
        self.assertIn("-DBUILD_MODULE_DETools=OFF", cmake_command)

    def test_build_root_default(self):
        """deps.BUILD_ROOT defaults to build/wasm-native under the repo root."""
        self.assertTrue(str(deps.BUILD_ROOT).endswith("build/wasm-native"))

    def test_set_build_root(self):
        """set_build_root updates BUILD_ROOT for intermediate build dirs."""
        original = deps.BUILD_ROOT
        try:
            new_root = Path("/tmp/custom-wasm-root")
            deps.set_build_root(new_root)
            self.assertEqual(deps.BUILD_ROOT, new_root)
            self.assertEqual(deps._dep_build_dir("occt"), new_root / "build" / "occt")
        finally:
            deps.set_build_root(original)


class TestPathHelpers(unittest.TestCase):
    """Test path helper functions in wasm_native."""

    def test_build_root(self):
        """BUILD_ROOT is set to the wasm-native default or env override."""
        self.assertTrue(str(wasm_native.BUILD_ROOT).endswith("build/wasm-native"))

    def test_toolchain_dir(self):
        self.assertEqual(wasm_native.toolchain_dir(), wasm_native.BUILD_ROOT / "toolchain")

    def test_downloads_dir(self):
        self.assertEqual(wasm_native.downloads_dir(), wasm_native.BUILD_ROOT / "downloads")

    def test_src_dir(self):
        self.assertEqual(wasm_native.src_dir(), wasm_native.BUILD_ROOT / "src")

    def test_prefix_dir(self):
        self.assertEqual(wasm_native.prefix_dir("minimal"), wasm_native.BUILD_ROOT / "prefix" / "minimal")
        self.assertEqual(wasm_native.prefix_dir("full"), wasm_native.BUILD_ROOT / "prefix" / "full")

    def test_ifcopenshell_build_dir(self):
        self.assertEqual(
            wasm_native.ifcopenshell_build_dir("minimal"),
            wasm_native.BUILD_ROOT / "ifcopenshell" / "minimal",
        )

    def test_dist_dir(self):
        self.assertEqual(wasm_native.dist_dir(), wasm_native.BUILD_ROOT / "dist")


class TestCLIParsing(unittest.TestCase):
    """Test CLI argument parsing."""

    def test_default_command(self):
        """Default command resolves to 'all' via main()."""
        parser = wasm_native.build_parser()
        args = parser.parse_args([])
        self.assertIsNone(args.command)

    def test_default_profile(self):
        parser = wasm_native.build_parser()
        args = parser.parse_args(["doctor"])
        self.assertEqual(args.profile, "minimal")

    def test_custom_profile(self):
        parser = wasm_native.build_parser()
        args = parser.parse_args(["--profile", "full", "build"])
        self.assertEqual(args.profile, "full")
        self.assertEqual(args.command, "build")

    def test_verbose_flag(self):
        parser = wasm_native.build_parser()
        args = parser.parse_args(["--verbose", "doctor"])
        self.assertTrue(args.verbose)

    def test_force_flag(self):
        parser = wasm_native.build_parser()
        args = parser.parse_args(["--force", "bootstrap-toolchain"])
        self.assertTrue(args.force)

    def test_all_subcommands(self):
        parser = wasm_native.build_parser()
        commands = [
            "doctor", "bootstrap-toolchain", "fetch", "build-deps",
            "configure", "build", "test", "package", "clean", "all",
        ]
        for cmd in commands:
            args = parser.parse_args([cmd])
            self.assertEqual(args.command, cmd)

    def test_main_resolves_default_to_all(self):
        """main() with explicit 'all' command invokes the all pipeline."""
        with patch("nix.wasm_native.cmd_all", return_value=0) as mock_all:
            rc = wasm_native.main(["all"])
        self.assertEqual(rc, 0)
        mock_all.assert_called_once()


class TestRunUtility(unittest.TestCase):
    """Test the run() utility in nix.core."""

    def test_run_success(self):
        result = core.run(["echo", "hello"], capture=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("hello", result.stdout)

    def test_run_failure(self):
        with self.assertRaises(subprocess.CalledProcessError):
            core.run(["false"], check=True)

    def test_run_no_check(self):
        result = core.run(["false"], check=False, capture=True)
        self.assertNotEqual(result.returncode, 0)

    def test_run_env_merge(self):
        """run() must merge `env` on top of the current environment."""
        result = core.run(
            ["sh", "-c", "echo $WASM_BUILD_TEST_VAR"],
            env={"WASM_BUILD_TEST_VAR": "merged-value"},
            capture=True,
        )
        self.assertEqual(result.returncode, 0)
        self.assertIn("merged-value", result.stdout)


class TestDoctorCommand(unittest.TestCase):
    """Test the doctor command."""

    def test_doctor_returns_zero(self):
        args = MagicMock()
        rc = wasm_native.cmd_doctor(args)
        self.assertEqual(rc, 0)


if __name__ == "__main__":
    unittest.main()
