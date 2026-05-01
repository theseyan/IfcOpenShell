# SPDX-License-Identifier: LGPL-3.0-or-later

"""
Python bridge for the native ifcapi library (libIfcApi).

Loads the shared library via ctypes and exposes high-level Python wrappers
that accept ifcopenshell.file objects and return ifcopenshell.entity_instance.
"""

import ctypes
import ctypes.util
import os
import sys

from ifcopenshell import _generated_capi

_lib = None


def _find_library():
    """Locate libIfcApi shared library."""
    # Check explicit env var first
    explicit = os.environ.get("IFCAPI_LIB_PATH")
    if explicit and os.path.isfile(explicit):
        return explicit

    # Check common build locations relative to this file
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = []

    # Relative to repo root (build-capi-stable/ifcapi/)
    repo_root = os.path.abspath(os.path.join(here, "..", "..", "..", ".."))
    for build_dir in ("build-capi-stable", "build", "build-release", "build-debug"):
        for name in ("libIfcApi.dylib", "libIfcApi.so", "IfcApi.dll"):
            candidates.append(os.path.join(repo_root, build_dir, "ifcapi", name))

    for c in candidates:
        if os.path.isfile(c):
            return c

    # Fallback: system search
    found = ctypes.util.find_library("IfcApi")
    return found


def _get_lib():
    global _lib
    if _lib is not None:
        return _lib

    path = _find_library()
    if not path:
        raise RuntimeError(
            "Cannot find libIfcApi shared library. "
            "Set IFCAPI_LIB_PATH or build with -DBUILD_IFCAPI=ON."
        )

    _lib = ctypes.CDLL(path)

    # Error handling
    _lib.ifcopenshell_last_error_message.restype = ctypes.c_char_p
    _lib.ifcopenshell_last_error_message.argtypes = []
    _lib.ifcopenshell_clear_error.restype = None
    _lib.ifcopenshell_clear_error.argtypes = []

    _generated_capi.bind(_lib, names=("ifcopenshell_ifcapi_root_create_entity",))

    return _lib


def _check_error():
    lib = _get_lib()
    err = lib.ifcopenshell_last_error_message()
    if err and len(err) > 0:
        msg = err.decode("utf-8", errors="replace")
        raise RuntimeError(f"ifcapi error: {msg}")


def _file_pointer(ifc_file):
    """Extract the raw C++ IfcParse::IfcFile* pointer from an ifcopenshell.file."""
    return ifc_file.wrapped_data.file_pointer()
