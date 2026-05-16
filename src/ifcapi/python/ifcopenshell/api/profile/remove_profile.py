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
from ifcopenshell import _generated_capi
from ifcopenshell.api.profile import _capi


def remove_profile(file: ifcopenshell.file, profile: ifcopenshell.entity_instance) -> None:
    """Removes a profile

    :param profile: The IfcProfileDef to remove.

    Example:

    .. code:: python

        circle = ifcopenshell.api.profile.add_parameterized_profile(model,
            ifc_class="IfcCircleProfileDef")
        circle = 1.
        ifcopenshell.api.profile.remove_profile(model, profile=circle)
    """
    lib = _capi.get_lib()
    _generated_capi.status_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_profile_remove_profile(_capi.file_handle(file), _capi.instance_handle(profile)),
        "Failed to remove profile",
    )
