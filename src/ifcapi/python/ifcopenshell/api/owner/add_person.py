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
from ifcopenshell.api.owner import _capi


def add_person(
    file: ifcopenshell.file,
    identification: str = "HSeldon",
    family_name: str = "Seldon",
    given_name: str = "Hari",
) -> ifcopenshell.entity_instance:
    """Adds a new person

    Persons are used to identify a legal or liable representative of an
    organisation or point of contact.

    :param identification: The computer readable unique identification of
        the person. For example, their username in a CDE or alias.
    :param family_name: The family name
    :param given_name: The given name
    :return: The newly created IfcPerson

    Example:

    .. code:: python

        ifcopenshell.api.owner.add_person(model,
            identification="bobthebuilder", family_name="Thebuilder", given_name="Bob")
    """
    lib = _capi.get_lib()
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_owner_add_person,
        "Failed to add person",
        _capi.file_handle(file),
        _generated_capi.encode_string(identification),
        _generated_capi.encode_string(family_name),
        _generated_capi.encode_string(given_name),
    )
