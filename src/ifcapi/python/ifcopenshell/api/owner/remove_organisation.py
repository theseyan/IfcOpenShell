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
from ifcopenshell.api.owner import _capi


def remove_organisation(file: ifcopenshell.file, organisation: ifcopenshell.entity_instance) -> None:
    """Remove an organisation

    All roles and addresses assigned to the organisation will also be
    removed.

    :param organisation: The IfcOrganization to remove
    :return: None

    Example:

    .. code:: python

        organisation = ifcopenshell.api.owner.add_organisation(model,
            identification="AWB", name="Architects Without Ballpens")
        ifcopenshell.api.owner.remove_organisation(model, organisation=organisation)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_owner_remove_organisation,
        "Failed to remove organisation",
        _capi.file_handle(file),
        _capi.instance_handle(organisation),
    )
