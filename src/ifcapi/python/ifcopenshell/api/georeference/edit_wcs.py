# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
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

import ifcopenshell
from ifcopenshell.api import _relationship_capi


def edit_wcs(
    file: ifcopenshell.file,
    x: float = 0.0,
    y: float = 0.0,
    z: float = 0.0,
    rotation: float = 0.0,
    is_si: bool = True,
) -> None:
    """Edits the WCS for all geometric contexts to a translation and rotation

    Typically, a project's local engineering origin (0, 0, 0) has a coordinate
    operation (e.g. map conversion) to a projected CRS. If a WCS is provided,
    the coordinate operation is relative to the WCS, not the local engineering
    origin.

    For example, if I have an IfcSite with a placement at (10, 0, 0) and a map
    conversion of (50, 0, 0), my IfcSite's local XYZ is at (10, 0, 0) with an
    ENH (Easting, Northing, Height) of (60, 0, 0).

    If I then define by WCS at (15, 0, 0), my IfcSite's local XYZ is still at
    (10, 0, 0) but its ENH is now at (45, 0, 0).

    It's recommended to leave the WCS at 0,0,0. Please :)

    :param x: The X translation of the WCS
    :param y: The Y translation of the WCS
    :param z: The Z translation of the WCS
    :param rotation: The rotation around the Z axis (i.e. top down plan view)
        in decimal degrees of the WCS. Anticlockwise is positive.

    Example:

    .. code:: python

        # This is the simplest scenario, resetting the WCS to 0,0,0 with no rotation (recommended)
        ifcopenshell.api.georeference.edit_wcs(model)
    """
    _relationship_capi.call_status(
        "georeference_edit_wcs",
        _relationship_capi.file_handle(file),
        x,
        y,
        z,
        rotation,
        is_si,
    )
