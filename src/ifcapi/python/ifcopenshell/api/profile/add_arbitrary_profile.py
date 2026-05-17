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
from ifcopenshell import _generated_capi
from ifcopenshell.api.profile import _capi
from ifcopenshell.util.shape_builder import SequenceOfVectors


def add_arbitrary_profile(
    file: ifcopenshell.file, profile: SequenceOfVectors, name: Optional[str] = None
) -> ifcopenshell.entity_instance:
    """Adds a new arbitrary polyline-based profile

    The profile is represented as a polyline defined by a list of
    coordinates. Only straight segments are allowed. Coordinates must be
    provided in SI meters.

    To represent a closed curve, the first and last coordinate must be
    identical.

    :param profile: A list of coordinates
    :param name: If the profile is semantically significant (i.e. to be
        managed and reused by the user) then it must be named. Otherwise,
        this may be left as none.
    :return: The newly created IfcArbitraryClosedProfileDef

    Example:

    .. code:: python

        # A 10mm by 100mm rectangle, such that might be used as a wooden
        # skirting board or kick plate.
        square = ifcopenshell.api.profile.add_arbitrary_profile(model,
            profile=[(0., 0.), (.01, 0.), (.01, .1), (0., .1), (0., 0.)],
            name="SK01 Profile")
    """
    lib = _capi.get_lib()
    handle = _generated_capi.call_handle_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_profile_add_arbitrary_profile,
        "Failed to add arbitrary profile",
        _capi.file_handle(file),
        _capi.double_list_list(profile),
        _capi.string(name) if name is not None else None,
        name is not None,
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return _capi.wrap_handle(file, handle)
