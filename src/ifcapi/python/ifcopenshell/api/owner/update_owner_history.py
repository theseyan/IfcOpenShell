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

from typing import Union

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def update_owner_history(
    file: ifcopenshell.file, element: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Updates the owner that is assigned to an object

    This ensures that the owner is tracked to have modified the object last,
    including the time when the change occured. See
    ifcopenshell.api.owner.create_owner_history for details.

    :param element: The IfcRoot element to update the ownership details on
        when a change is made.
    :return: The updated IfcOwnerHistory element.

    Example:

    .. code:: python

        # See ifcopenshell.api.owner.create_owner_history for setup
        # [ ... example setup code ... ]

        # We've finished our ownership setup. Now let's start our script and
        # create a space. Notice we don't actually call
        # create_owner_history at all. This is already automatically handled
        # by the API when necessary. Under the hood, the API is actually
        # running this code on the IfcSpace element:
        # element.OwnerHistory = ifcopenshell.api.owner.create_owner_history(model)
        space = ifcopenshell.api.root.create_entity(model, ifc_class="IfcSpace")

        # Any edits we make will have ownership tracking automatically
        # applied. There is no need to run any owner.update_owner_history
        # API calls either.
        ifcopenshell.api.attribute.edit_attributes(model, product=space, attributes={"Name": "Lobby"})
    """
    if not element.is_a("IfcRoot"):
        return
    user = ifcopenshell.api.owner.settings.get_user(file)
    if not user:
        return
    application = ifcopenshell.api.owner.settings.get_application(file)
    if not application:
        return
    handle = _capi.ifcopenshell_ifcapi_owner_update_owner_history(
        file._handle,
        element._handle,
        user._handle,
        application._handle,
    )
    return ifcopenshell.entity_instance(file, handle) if handle else None
