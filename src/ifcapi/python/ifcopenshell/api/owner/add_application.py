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

from typing import Optional

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi


def add_application(
    file: ifcopenshell.file,
    application_developer: Optional[ifcopenshell.entity_instance] = None,
    version: Optional[str] = None,
    application_full_name: str = "IfcOpenShell",
    application_identifier: str = "IfcOpenShell",
) -> ifcopenshell.entity_instance:
    """Adds a new application

    IFC data may be associated with an authoring application to identify
    which application was responsible for editing or authoring the data. An
    application is defined by the developing organisation, as well as a full
    name and identifier. This is akin to how web browsers have an
    identification string.

    :param application_developer: The IfcOrganization responsible for
        creating the application. Defaults to generating an IfcOpenShell
        organisation if none is provided.
    :param version: The version of the application. Defaults to the
        ifcopenshell.version data if not specified.
    :param application_full_name: The name of the application
    :param application_identifier: An identification string for the
        application intended for computers to read.
    :return: The newly created IfcApplication

    Example:

    .. code:: python

        application = ifcopenshell.api.owner.add_application(model)
    """
    handle = _capi.owner_add_application(
        file._handle,
        application_developer._handle if application_developer is not None else None,
        version or ifcopenshell.version,
        application_full_name,
        application_identifier,
        None,
        None,
        None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to add application")
