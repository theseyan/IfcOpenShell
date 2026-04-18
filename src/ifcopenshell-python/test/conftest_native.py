# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""
conftest plugin that, when IFCOPENSHELL_USE_NATIVE=1 is set, replaces the
Python high-level API functions with their native C++ equivalents from ifcapi.

Usage:
    pytest test/api/root/  # runs pure-Python tests (default)
    IFCOPENSHELL_USE_NATIVE=1 pytest test/api/root/  # runs C++ backend tests
"""

import os
import sys


def pytest_configure(config):
    if os.environ.get("IFCOPENSHELL_USE_NATIVE") != "1":
        return

    # Ensure the ifcapi Python bridge is on the path
    bridge_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "ifcapi", "python"))
    if bridge_path not in sys.path:
        sys.path.insert(0, bridge_path)

    try:
        import ifcapi.root as native_root
    except Exception as e:
        import warnings
        warnings.warn(f"IFCOPENSHELL_USE_NATIVE=1 but cannot load ifcapi: {e}")
        return

    import ifcopenshell.api.root
    import ifcopenshell.api.root.create_entity as create_entity_module

    # Replace the module-level create_entity function
    original_fn = create_entity_module.create_entity
    create_entity_module.create_entity = native_root.create_entity

    # Also replace the reference in the root package
    ifcopenshell.api.root.create_entity = native_root.create_entity

    print(f"[ifcapi] Native backend enabled for root.create_entity")
