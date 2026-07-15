# SPDX-License-Identifier: LGPL-3.0-or-later

"""Run the current upstream railing tests unchanged against the native package."""

from pathlib import Path

_UPSTREAM_TEST = (
    Path(__file__).resolve().parents[7]
    / "src/ifcopenshell-python/test/api/geometry/test_add_railing_representation.py"
)
exec(compile(_UPSTREAM_TEST.read_text(), str(_UPSTREAM_TEST), "exec"))
