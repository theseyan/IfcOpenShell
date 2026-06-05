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


def remove_person_and_organisation(
    file: ifcopenshell.file, person_and_organisation: ifcopenshell.entity_instance
) -> None:
    """Removes a person and organisation

    Note that the underlying person and organisation is not removed, only
    the "person and organisation" group.

    :param person_and_organisation: The IfcPersonAndOrganization to remove.
    :return: None

    Example:

    .. code:: python

        person = ifcopenshell.api.owner.add_person(model,
            identification="lecorbycorbycorb", family_name="Curbosiar", given_name="Le")
        organisation = ifcopenshell.api.owner.add_organisation(model,
            identification="AWB", name="Architects Without Ballpens")

        user = ifcopenshell.api.owner.add_person_and_organisation(model,
            person=person, organisation=organisation)

        ifcopenshell.api.owner.remove_person_and_organisation(model, person_and_organisation=user)
    """
    _capi.call_status(
        "owner_remove_person_and_organisation",
        "Failed to remove person and organisation",
        _capi.file_handle(file),
        _capi.instance_handle(person_and_organisation),
    )
