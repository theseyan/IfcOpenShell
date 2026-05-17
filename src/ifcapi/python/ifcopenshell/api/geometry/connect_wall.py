# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2025 Dion Moult <dion@thinkmoult.com>
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

from . import _capi


def connect_wall(
    file: ifcopenshell.file,
    wall1: ifcopenshell.entity_instance,
    wall2: ifcopenshell.entity_instance,
    is_atpath: bool = False,
) -> Optional[ifcopenshell.entity_instance]:
    owner_history, user, application = _capi.owner_context(file)
    lib = _capi.get_lib()
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_geometry_connect_wall,
        _capi.file_handle(file),
        _capi.instance_handle(wall1),
        _capi.instance_handle(wall2),
        is_atpath,
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
        nullable=True,
    )
