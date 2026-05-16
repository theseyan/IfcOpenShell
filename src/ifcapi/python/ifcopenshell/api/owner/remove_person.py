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


def remove_person(file: ifcopenshell.file, person: ifcopenshell.entity_instance) -> None:
    """Remove an person

    All roles and addresses assigned to the person will also be
    removed.
    In IFC2X3 will also remove related inventories if `person` was
    the only responsile person for them.

    :param person: The IfcPerson to remove
    :return: None

    Example:

    .. code:: python

        ifcopenshell.api.owner.add_person(model,
            identification="bobthebuilder", family_name="Thebuilder", given_name="Bob")
        ifcopenshell.api.owner.remove_person(model, person=person)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_owner_remove_person,
        "Failed to remove person",
        _capi.file_handle(file),
        _capi.instance_handle(person),
    )
