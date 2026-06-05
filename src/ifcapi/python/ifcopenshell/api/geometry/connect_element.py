# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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
from ifcopenshell.api.geometry import _capi


def connect_element(
    file: ifcopenshell.file,
    relating_element: ifcopenshell.entity_instance,
    related_element: ifcopenshell.entity_instance,
    description: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    user, application = _capi.owner_user_application(file)
    return _capi.call_handle(
        file,
        "geometry_connect_element",
        _capi.file_handle(file),
        _capi.instance_handle(relating_element),
        _capi.instance_handle(related_element),
        _capi.string(description or ""),
        description is not None,
        None,
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
