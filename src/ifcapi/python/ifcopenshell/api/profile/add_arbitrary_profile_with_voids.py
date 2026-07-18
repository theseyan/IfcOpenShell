# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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

from typing import Optional

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell.util.shape_builder import SequenceOfVectors


def add_arbitrary_profile_with_voids(
    file: ifcopenshell.file,
    outer_profile: SequenceOfVectors,
    inner_profiles: list[SequenceOfVectors],
    name: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    """Adds a new arbitrary polyline-based profile with voids

    The outer profile is represented as a polyline defined by a list of
    coordinates. Only straight segments are allowed. Coordinates must be
    provided in SI meters.

    To represent a closed curve, the first and last coordinate must be
    identical.

    The inner profiles are represented as a list of polylines.
    Every polyline in defined by a list of coordinates.
    Only straight segments are allowed. Coordinates must be
    provided in SI meters.

    :param outer_profile: A list of coordinates
    :param inner_profiles: A list of polylines
    :param name: If the profile is semantically significant (i.e. to be
        managed and reused by the user) then it must be named. Otherwise,
        this may be left as none.
    :return: The newly created IfcArbitraryProfileDefWithVoids

    Example:

    .. code:: python

        # A 400mm by 400mm square with a 200mm by 200mm hole in it.
        square_with_hole = ifcopenshell.api.profile.add_arbitrary_profile_with_voids(model,
            outer_profile=[(0., 0.), (.4, 0.), (.4, .4), (0., .4), (0., 0.)],
            inner_profiles=[[(0.1, 0.1), (0.3, 0.1), (0.3, 0.3), (0.1, 0.3), (0.1, 0.1)]],
            name="SK01 Hole Profile")
    """
    outer = [list(v) for v in outer_profile]
    inners = [[list(v) for v in inner] for inner in inner_profiles]
    try:
        handle = _capi.profile_add_arbitrary_profile_with_voids(
            file._handle,
            {
                "outer_profile": outer,
                "inner_profiles": inners,
                **({"name": name} if name is not None else {}),
            },
        )
    except TypeError as error:
        raise RuntimeError(str(error)) from error
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(
        _capi.last_error_message() or "Failed to add arbitrary profile with voids"
    )
