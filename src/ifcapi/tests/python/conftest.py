# SPDX-License-Identifier: LGPL-3.0-or-later

import sys
import os

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))


_KNOWN_XFAILS: dict = {}


def pytest_collection_modifyitems(config, items):
    for item in items:
        for needle, reason in _KNOWN_XFAILS.items():
            if item.nodeid.endswith(needle):
                item.add_marker(pytest.mark.xfail(reason=reason, strict=False))

