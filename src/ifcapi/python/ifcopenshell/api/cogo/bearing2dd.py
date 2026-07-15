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

from ifcopenshell import _ifcopenshell_capi as _capi


def bearing2dd(bearing: str) -> float:
    """
    Converts a quadrant bearing string to decimal degrees

    The format of the string is "N|S dd (mm (ss.s)) E|W"
    where:
    N|S is N or S for North or South
    dd is degree (required)
    mm is minute (optional, but required if second is provided)
    ss.s is second (required)
    E|W is E or W for East or West

    :param str: the bearing string
    :return: Angle in radian
    """
    try:
        return _capi.cogo_bearing2dd(bearing)
    except RuntimeError as error:
        if str(error) == "Invalid bearing string":
            raise ValueError("Invalid bearing string") from None
        raise
