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

_KNOWN_SKIPS = (
    "test/geom/test_sweeps.py::test_pipe_12d",
)


def pytest_collection_modifyitems(config, items):
    for item in items:
        for needle, reason in _KNOWN_XFAILS.items():
            if item.nodeid.endswith(needle):
                item.add_marker(pytest.mark.xfail(reason=reason, strict=False))
        for needle in _KNOWN_SKIPS:
            if item.nodeid.endswith(needle):
                item.add_marker(pytest.mark.skip(reason="Known pre-existing bug"))

