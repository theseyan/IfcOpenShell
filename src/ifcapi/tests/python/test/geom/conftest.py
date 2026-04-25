# This file was generated with the assistance of an AI coding tool.

from pathlib import Path

import pytest

import ifcopenshell


@pytest.fixture
def file(request):
    return ifcopenshell.open(Path(__file__).parents[1] / "fixtures" / request.param)
