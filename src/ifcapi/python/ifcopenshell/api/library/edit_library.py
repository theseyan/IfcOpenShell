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
import datetime
from typing import Any

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell.api.attribute.edit_attributes import _edit_attributes


def edit_library(
    file: ifcopenshell.file,
    library: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
) -> None:
    """Edits the attributes of an IfcLibraryInformation

    For more information about the attributes and data types of an
    IfcLibraryInformation, consult the IFC documentation.

    :param library: The IfcLibraryInformation entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        library = ifcopenshell.api.library.add_library(model, name="Brickschema")
        ifcopenshell.api.library.edit_library(model, library=library,
            attributes={"Description": "A Brickschema TTL including only mechanical distribution systems."})
    """

    if "VersionDate" in attributes:
        dt = attributes["VersionDate"]
        if isinstance(dt, datetime.datetime):
            _capi.library_edit_version_date(
                file._handle, library._handle, dt.isoformat()
            )
            attributes = attributes.copy()
            del attributes["VersionDate"]

    _edit_attributes(file, library, attributes)
