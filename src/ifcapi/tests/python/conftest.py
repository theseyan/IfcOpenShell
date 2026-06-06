# SPDX-License-Identifier: LGPL-3.0-or-later

import sys
import os

import pytest

_ifcapi_python_path = os.path.join(os.path.dirname(__file__), "..", "..", "python")
sys.path.insert(0, _ifcapi_python_path)

# Ensure subprocesses spawned by tests can also import ifcopenshell.
_pythonpath = os.environ.get("PYTHONPATH", "")
_sep = os.pathsep if _pythonpath else ""
os.environ["PYTHONPATH"] = f"{_ifcapi_python_path}{_sep}{_pythonpath}"


_KNOWN_XFAILS: dict = {}


def pytest_collection_modifyitems(config, items):
    for item in items:
        for needle, reason in _KNOWN_XFAILS.items():
            if item.nodeid.endswith(needle):
                item.add_marker(pytest.mark.xfail(reason=reason, strict=False))

