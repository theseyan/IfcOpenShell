# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2025 Thomas Krijnen <thomas@aecgeeks.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.


import ifcopenshell.api.cogo
import pytest
from ifcopenshell import _ifcopenshell_capi as _capi


def test_bearing2dd():
    assert 44.743888875 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 45 15 22.5 E")
    )
    assert 135.256111125 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 45 15 22.5 W")
    )
    assert 224.743888875 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("S 45 15 22.5 W")
    )
    assert 315.256111125 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("S 45 15 22.5 E")
    )

    assert 44.743888875 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("n 45 15 22.5 e")
    )
    assert 135.256111125 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("n 45 15 22.5 w")
    )
    assert 224.743888875 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("s 45 15 22.5 w")
    )
    assert 315.256111125 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("s 45 15 22.5 e")
    )

    assert 0.0 == pytest.approx(ifcopenshell.api.cogo.bearing2dd("N 90 E"))
    assert 0.0 == pytest.approx(ifcopenshell.api.cogo.bearing2dd("S 90 E"))

    assert 180.0 == pytest.approx(ifcopenshell.api.cogo.bearing2dd("N 90 W"))
    assert 180.0 == pytest.approx(ifcopenshell.api.cogo.bearing2dd("S 90 W"))

    assert 120.0 == pytest.approx(ifcopenshell.api.cogo.bearing2dd("N 30 W"))
    assert 120.16666666666667 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 30 10 W")
    )

    assert 89.999722222222228 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 00 00 1 E")
    )
    assert 89.99972222222222 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 0 0 1 E")
    )
    assert 89.99972222222222 == pytest.approx(
        ifcopenshell.api.cogo.bearing2dd("N 00 00 1.0 E")
    )

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("Bad String")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("Very Bad String")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N 100 15 22.5 E")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N -45 15 22.5 E")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N 45 -15 22.5 E")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N 45 88 22.5 E")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N 45 15 -22.5 E")

    with pytest.raises(ValueError, match="Invalid bearing string"):
        ifcopenshell.api.cogo.bearing2dd("N 45 15 99.5 E")


def test_bearing2dd_whitespace_boundaries_and_strict_numeric_tokens():
    assert ifcopenshell.api.cogo.bearing2dd("\tN\t0\tE  ") == pytest.approx(90.0)
    assert ifcopenshell.api.cogo.bearing2dd(" S   90   E ") == pytest.approx(0.0)
    assert ifcopenshell.api.cogo.bearing2dd("N 0 W") == pytest.approx(90.0)
    assert ifcopenshell.api.cogo.bearing2dd("N 89 59 59.999999 W") > 179.999

    for bearing in (
        "N 45.5 E",
        "N 45x E",
        "N E",
        "45 E",
        "N 45",
        "N nan E",
        "N inf E",
        "N 90 0 0.0001 E",
        "N 89 60 E",
    ):
        with pytest.raises(ValueError, match="^Invalid bearing string$"):
            ifcopenshell.api.cogo.bearing2dd(bearing)


def test_invalid_bearing_exposes_typed_error_contract():
    with pytest.raises(ValueError) as error:
        ifcopenshell.api.cogo.bearing2dd(
            "diagnostic wording is not a dispatch contract"
        )

    assert error.value.kind == _capi.IFCOPENSHELL_ERROR_VALUE
    assert error.value.code == _capi.IFCOPENSHELL_ERROR_CODE_INVALID_QUADRANT_BEARING


test_bearing2dd()
