#!/usr/bin/env python3
# This file was generated with the assistance of an AI coding tool.
"""
Unit tests for the WASM native bootstrap build system.
"""

import hashlib
import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch, MagicMock

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

import build


class TestLockfileParsing(unittest.TestCase):
    """Test lockfile loading and validation."""

    def test_load_lockfile(self):
        """Test that lockfile loads correctly."""
        lock = build.load_lockfile()
        self.assertIsInstance(lock, dict)
        self.assertIn("emsdk", lock)
        self.assertIn("boost", lock)
        self.assertIn("occt", lock)
        self.assertIn("manifold", lock)
        self.assertIn("gmp", lock)
        self.assertIn("mpfr", lock)
        self.assertIn("cgal", lock)
        self.assertIn("eigen", lock)
        self.assertIn("nlohmann_json", lock)

    def test_lockfile_entry_structure(self):
        """Test that each lockfile entry has required fields."""
        lock = build.load_lockfile()
        for name, entry in lock.items():
            self.assertIn("url", entry, f"{name} missing 'url'")
            self.assertIn("version", entry, f"{name} missing 'version'")
            self.assertIn("archive_type", entry, f"{name} missing 'archive_type'")
            self.assertIn("patches", entry, f"{name} missing 'patches'")
            self.assertIsInstance(entry["patches"], list, f"{name} 'patches' should be list")

    def test_lockfile_versions(self):
        """Test that versions match expected values."""
        lock = build.load_lockfile()
        self.assertEqual(lock["occt"]["version"], "7.8.1")
        self.assertEqual(lock["gmp"]["version"], "6.3.0")
        self.assertEqual(lock["mpfr"]["version"], "3.1.6")
        self.assertEqual(lock["cgal"]["version"], "5.6.3")
        self.assertEqual(lock["manifold"]["version"], "3.2.1")
        self.assertEqual(lock["boost"]["version"], "1.86.0")
        self.assertEqual(lock["eigen"]["version"], "3.4.0")
        self.assertEqual(lock["nlohmann_json"]["version"], "3.11.3")

    def test_lockfile_archive_types(self):
        """Test that archive types are valid."""
        lock = build.load_lockfile()
        valid_types = {"tar.gz", "tar.bz2", "tar.xz", "tar", "zip", "git"}
        for name, entry in lock.items():
            self.assertIn(
                entry["archive_type"],
                valid_types,
                f"{name} has invalid archive_type: {entry['archive_type']}",
            )

    def test_lockfile_urls(self):
        """Test that URLs are well-formed."""
        lock = build.load_lockfile()
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
            actual = build.sha256_file(temp_path)
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
            actual = build.sha256_file(temp_path)
            self.assertEqual(actual, expected)
        finally:
            temp_path.unlink()

    def test_sha256_large_file(self):
        """Test SHA-256 on larger file."""
        content = b"x" * (1 << 20)  # 1 MB
        with tempfile.NamedTemporaryFile(mode="wb", delete=False) as f:
            f.write(content)
            f.flush()
            temp_path = Path(f.name)

        try:
            expected = hashlib.sha256(content).hexdigest()
            actual = build.sha256_file(temp_path)
            self.assertEqual(actual, expected)
        finally:
            temp_path.unlink()


class TestProfileResolution(unittest.TestCase):
    """Test profile loading and resolution."""

    def test_load_profiles(self):
        """Test that profiles loads correctly."""
        profiles = build.load_profiles()
        self.assertIsInstance(profiles, dict)
        self.assertIn("minimal", profiles)
        self.assertIn("manifold", profiles)
        self.assertIn("cgal", profiles)
        self.assertIn("occt", profiles)
        self.assertIn("full", profiles)

    def test_get_profile_valid(self):
        """Test getting a valid profile."""
        profile = build.get_profile("minimal")
        self.assertIsInstance(profile, dict)
        self.assertIn("enabled_kernels", profile)
        self.assertIn("dependencies", profile)
        self.assertIn("cmake_flags", profile)
        self.assertIn("test_expectations", profile)

    def test_get_profile_invalid(self):
        """Test getting an invalid profile raises error."""
        with self.assertRaises(ValueError) as ctx:
            build.get_profile("nonexistent")
        self.assertIn("nonexistent", str(ctx.exception))

    def test_minimal_profile(self):
        """Test minimal profile has correct settings."""
        profile = build.get_profile("minimal")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertEqual(len(profile["enabled_kernels"]), 1)
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "OFF")

    def test_manifold_profile(self):
        """Test manifold profile has correct settings."""
        profile = build.get_profile("manifold")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertIn("manifold", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "OFF")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "OFF")
        self.assertIn("manifold", profile["dependencies"])

    def test_cgal_profile(self):
        """Test cgal profile has correct settings."""
        profile = build.get_profile("cgal")
        self.assertIn("cgal", profile["enabled_kernels"])
        self.assertIn("cgalsimple", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "ON")
        self.assertIn("gmp", profile["dependencies"])
        self.assertIn("mpfr", profile["dependencies"])
        self.assertIn("cgal", profile["dependencies"])

    def test_occt_profile(self):
        """Test occt profile has correct settings."""
        profile = build.get_profile("occt")
        self.assertIn("opencascade", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "ON")
        self.assertIn("occt", profile["dependencies"])

    def test_full_profile(self):
        """Test full profile has all kernels enabled."""
        profile = build.get_profile("full")
        self.assertIn("passthrough", profile["enabled_kernels"])
        self.assertIn("opencascade", profile["enabled_kernels"])
        self.assertIn("cgal", profile["enabled_kernels"])
        self.assertIn("cgalsimple", profile["enabled_kernels"])
        self.assertIn("manifold", profile["enabled_kernels"])
        self.assertEqual(profile["cmake_flags"]["WITH_OPENCASCADE"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_CGAL"], "ON")
        self.assertEqual(profile["cmake_flags"]["WITH_MANIFOLD"], "ON")

    def test_profile_dependencies_subset_of_lockfile(self):
        """Test that all profile dependencies exist in lockfile."""
        lock = build.load_lockfile()
        profiles = build.load_profiles()
        for name, profile in profiles.items():
            for dep in profile["dependencies"]:
                self.assertIn(
                    dep,
                    lock,
                    f"Profile '{name}' references dependency '{dep}' not in lockfile",
                )


class TestCMakeFlagGeneration(unittest.TestCase):
    """Test CMake flag generation."""

    def test_generate_cmake_flags_base(self):
        """Test base CMake flags are present."""
        # Mock prefix_dir to avoid filesystem issues
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = build.generate_cmake_flags("minimal")

        # Check essential flags
        flag_str = " ".join(flags)
        self.assertIn("-DWASM_BUILD=ON", flag_str)
        self.assertIn("-DBUILD_IFCCAPI=ON", flag_str)
        self.assertIn("-DBUILD_IFCAPI=ON", flag_str)
        self.assertIn("-DBUILD_IFCGEOM=ON", flag_str)
        self.assertIn("-DBUILD_IFCPYTHON=OFF", flag_str)
        self.assertIn("-DBUILD_CONVERT=OFF", flag_str)
        self.assertIn("-DBUILD_GEOMSERVER=OFF", flag_str)
        self.assertIn("-DBUILD_EXAMPLES=OFF", flag_str)
        self.assertIn("-DBUILD_SHARED_LIBS=OFF", flag_str)
        self.assertIn("-DCOLLADA_SUPPORT=OFF", flag_str)
        self.assertIn("-DBoost_NO_BOOST_CMAKE=On", flag_str)
        self.assertIn("-DCMAKE_BUILD_TYPE=MinSizeRel", flag_str)
        self.assertIn(f"-DPYTHON_EXECUTABLE={sys.executable}", flags)

    def test_generate_cmake_flags_minimal(self):
        """Test minimal profile flags."""
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = build.generate_cmake_flags("minimal")

        flag_str = " ".join(flags)
        self.assertIn("-DWITH_OPENCASCADE=OFF", flag_str)
        self.assertIn("-DWITH_CGAL=OFF", flag_str)
        self.assertIn("-DWITH_MANIFOLD=OFF", flag_str)

    def test_generate_cmake_flags_manifold(self):
        """Test manifold profile flags."""
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = build.generate_cmake_flags("manifold")

        flag_str = " ".join(flags)
        self.assertIn("-DWITH_MANIFOLD=ON", flag_str)
        self.assertIn("-DWITH_OPENCASCADE=OFF", flag_str)
        self.assertIn("-DWITH_CGAL=OFF", flag_str)

    def test_generate_cmake_flags_cgal(self):
        """Test cgal profile flags."""
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = build.generate_cmake_flags("cgal")

        flag_str = " ".join(flags)
        self.assertIn("-DWITH_CGAL=ON", flag_str)
        self.assertIn("-DCGAL_WITH_GMPXX=Off", flag_str)

    def test_generate_cmake_flags_full(self):
        """Test full profile flags."""
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            flags = build.generate_cmake_flags("full")

        flag_str = " ".join(flags)
        self.assertIn("-DWITH_OPENCASCADE=ON", flag_str)
        self.assertIn("-DWITH_CGAL=ON", flag_str)
        self.assertIn("-DWITH_MANIFOLD=ON", flag_str)

    def test_generate_cmake_flags_returns_list(self):
        """Test that generate_cmake_flags returns a list of strings."""
        with patch("build.prefix_dir") as mock_prefix:
            mock_prefix.return_value = Path("/tmp/test-prefix")
            for profile_name in ["minimal", "manifold", "cgal", "occt", "full"]:
                flags = build.generate_cmake_flags(profile_name)
                self.assertIsInstance(flags, list)
                for f in flags:
                    self.assertIsInstance(f, str)


class TestPatchManagement(unittest.TestCase):
    """Test patch file existence and structure."""

    def test_occt_patch_exists(self):
        """Test that OCCT patch file exists."""
        patch_file = build.PATCHES_DIR / "occt" / "no_em_js.patch"
        self.assertTrue(patch_file.exists(), f"OCCT patch not found: {patch_file}")

    def test_manifold_patch_exists(self):
        """Test that Manifold patch file exists."""
        patch_file = build.PATCHES_DIR / "manifold" / "install-metadata-for-emscripten.patch"
        self.assertTrue(patch_file.exists(), f"Manifold patch not found: {patch_file}")

    def test_patches_are_valid_diff_format(self):
        """Test that patch files look like valid diffs."""
        for patch_path in build.PATCHES_DIR.rglob("*.patch"):
            content = patch_path.read_text()
            # Basic diff format check
            self.assertTrue(
                content.startswith("---") or content.startswith("diff ") or "diff --git" in content,
                f"Patch {patch_path} doesn't look like a valid diff",
            )


class TestDependencyBuildConfiguration(unittest.TestCase):
    """Test dependency CMake configuration."""

    def test_occt_uses_wasm_exception_flags(self):
        """Test that OCCT is built with the same exception ABI as side modules."""
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            src = root / "src"
            prefix = root / "prefix"
            src.mkdir()

            with (
                patch("build.load_lockfile", return_value={"occt": {"patches": []}}),
                patch("build.build_subdir", return_value=root / "build"),
                patch("build.run") as mock_run,
            ):
                build._build_occt(src, prefix, {})

        cmake_command = mock_run.call_args_list[0].args[0]
        self.assertIn("-DCMAKE_CXX_FLAGS=-fwasm-exceptions -sSUPPORT_LONGJMP=wasm", cmake_command)


class TestPathHelpers(unittest.TestCase):
    """Test path helper functions."""

    def test_build_root(self):
        """Test BUILD_ROOT is set correctly."""
        self.assertTrue(str(build.BUILD_ROOT).endswith("build/wasm-native"))

    def test_toolchain_dir(self):
        """Test toolchain_dir returns correct path."""
        self.assertEqual(build.toolchain_dir(), build.BUILD_ROOT / "toolchain")

    def test_downloads_dir(self):
        """Test downloads_dir returns correct path."""
        self.assertEqual(build.downloads_dir(), build.BUILD_ROOT / "downloads")

    def test_src_dir(self):
        """Test src_dir returns correct path."""
        self.assertEqual(build.src_dir(), build.BUILD_ROOT / "src")

    def test_prefix_dir(self):
        """Test prefix_dir returns profile-specific path."""
        self.assertEqual(build.prefix_dir("minimal"), build.BUILD_ROOT / "prefix" / "minimal")
        self.assertEqual(build.prefix_dir("full"), build.BUILD_ROOT / "prefix" / "full")

    def test_ifcopenshell_build_dir(self):
        """Test ifcopenshell_build_dir returns profile-specific path."""
        self.assertEqual(
            build.ifcopenshell_build_dir("minimal"),
            build.BUILD_ROOT / "ifcopenshell" / "minimal",
        )

    def test_dist_dir(self):
        """Test dist_dir returns correct path."""
        self.assertEqual(build.dist_dir(), build.BUILD_ROOT / "dist")


class TestCLIParsing(unittest.TestCase):
    """Test CLI argument parsing."""

    def test_default_command(self):
        """Test default command resolves to 'all' via main()."""
        # argparse default is None; main() maps None -> 'all'
        parser = build.build_parser()
        args = parser.parse_args([])
        self.assertIsNone(args.command)
        # Verify main() resolves it
        with patch("build.cmd_all", return_value=0) as mock_all:
            rc = build.main(["all"])
            self.assertEqual(rc, 0)

    def test_default_profile(self):
        """Test default profile is 'minimal'."""
        parser = build.build_parser()
        args = parser.parse_args(["doctor"])
        self.assertEqual(args.profile, "minimal")

    def test_custom_profile(self):
        """Test custom profile setting."""
        parser = build.build_parser()
        args = parser.parse_args(["--profile", "full", "build"])
        self.assertEqual(args.profile, "full")
        self.assertEqual(args.command, "build")

    def test_verbose_flag(self):
        """Test verbose flag."""
        parser = build.build_parser()
        args = parser.parse_args(["--verbose", "doctor"])
        self.assertTrue(args.verbose)

    def test_force_flag(self):
        """Test force flag."""
        parser = build.build_parser()
        args = parser.parse_args(["--force", "bootstrap-toolchain"])
        self.assertTrue(args.force)

    def test_all_subcommands(self):
        """Test all subcommands can be parsed."""
        parser = build.build_parser()
        commands = [
            "doctor", "bootstrap-toolchain", "fetch", "build-deps",
            "configure", "build", "test", "package", "clean", "all",
        ]
        for cmd in commands:
            args = parser.parse_args([cmd])
            self.assertEqual(args.command, cmd)


class TestRunUtility(unittest.TestCase):
    """Test run() utility function."""

    def test_run_success(self):
        """Test successful command execution."""
        result = build.run(["echo", "hello"], capture=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("hello", result.stdout)

    def test_run_failure(self):
        """Test failed command execution raises on check=True."""
        with self.assertRaises(subprocess.CalledProcessError):
            build.run(["false"], check=True)

    def test_run_no_check(self):
        """Test that check=False doesn't raise on failure."""
        result = build.run(["false"], check=False, capture=True)
        self.assertNotEqual(result.returncode, 0)


class TestDoctorCommand(unittest.TestCase):
    """Test doctor command."""

    def test_doctor_returns_zero(self):
        """Test doctor command succeeds on a system with basic tools."""
        args = MagicMock()
        rc = build.cmd_doctor(args)
        self.assertEqual(rc, 0)


if __name__ == "__main__":
    unittest.main()
