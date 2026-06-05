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
from ifcopenshell.api import _relationship_capi


def add_georeferencing(file: ifcopenshell.file, ifc_class: str = "IfcMapConversion", name: str = "EPSG:3857") -> None:
    """Add empty georeferencing entities to a model

    By default, models are not georeferenced. Georeferencing requires two
    entities: a definition of the projected coordinated reference system
    (CRS) used, and the transformation parameters between any local coordinate
    system and that projected CRS if any.

    This function will create the entities to store the projected CRS and
    map conversion transformation, but will leave all the parameters blank.
    It is this the users responsibility to specify the correct
    georeferencing parameters. See
    ifcopenshell.api.georeference.edit_georeferencing.

    :param ifc_class: A type of IfcCoordinateOperation. For IFC2X3, this has no
        impact and only uses ePSet_MapConversion.

    Example:

    .. code:: python

        ifcopenshell.api.georeference.add_georeferencing(model)
    """
    owner_history, user, application = _relationship_capi.owner_context(file)
    _relationship_capi.call_status(
        "georeference_add_georeferencing",
        _relationship_capi.file_handle(file),
        _relationship_capi.string(ifc_class),
        _relationship_capi.string(name),
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
