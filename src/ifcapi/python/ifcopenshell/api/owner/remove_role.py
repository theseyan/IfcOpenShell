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


def remove_role(file: ifcopenshell.file, role: ifcopenshell.entity_instance) -> None:
    """Removes a role

    People and organisations using the role will be untouched. This may
    leave some of them without roles.

    :param role: The IfcActorRole to remove.
    :return: None

    Example:

    .. code:: python

        organisation = ifcopenshell.api.owner.add_organisation(model,
            identification="AWB", name="Architects Without Ballpens")
        role = ifcopenshell.api.owner.add_role(model, assigned_object=organisation, role="ARCHITECT")

        # After running this, the organisation will have no role again
        ifcopenshell.api.owner.remove_role(model, role=role)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_owner_remove_role,
        "Failed to remove role",
        _capi.file_handle(file),
        _capi.instance_handle(role),
    )
