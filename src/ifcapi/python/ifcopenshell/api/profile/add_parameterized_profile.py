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
from ifcopenshell import _ifcopenshell_capi as _capi


def add_parameterized_profile(
    file: ifcopenshell.file, ifc_class: str, profile_type: str = "AREA"
) -> ifcopenshell.entity_instance:
    """Adds a new parameterised profile

    IFC offers parameterised profiles for common standardised hot roll
    steel sections and common concrete forms. A full list is available on
    the IFC documentation as subclasses of IfcParameterizedProfileDef.

    Currently, this API has no benefit over directly calling
    ifcopenshell.file.create_entity.

    :param ifc_class: The subclass of IfcParameterizedProfileDef that you'd
        like to create.
    :param profile_type:
    :return: The newly created element depending on the specified ifc_class.

    Example:

    .. code:: python

        circle = ifcopenshell.api.profile.add_parameterized_profile(model,
            ifc_class="IfcCircleProfileDef")
        circle.Radius = 1.
    """
    handle = _capi.ifcopenshell_ifcapi_profile_add_parameterized_profile(file._handle, ifc_class, profile_type)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to add parameterized profile")
