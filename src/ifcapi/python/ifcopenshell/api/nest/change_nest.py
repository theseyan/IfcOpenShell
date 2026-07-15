# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021-2022 Dion Moult <dion@thinkmoult.com>, Yassine Oualid <yassine@sigmadimensions.com>
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


def change_nest(
    file: ifcopenshell.file,
    item: ifcopenshell.entity_instance,
    new_parent: ifcopenshell.entity_instance,
) -> None:
    """Move an already nested child to a new parent, appending it after existing children."""
    owner_history, user, application = _relationship_capi.owner_context(file)
    options = {
        "item": _relationship_capi.instance_handle(item),
        "new_parent": _relationship_capi.instance_handle(new_parent),
    }
    for key, value in (
        ("owner_history", owner_history),
        ("user", user),
        ("application", application),
    ):
        if value is not None:
            options[key] = _relationship_capi.instance_handle(value)
    _relationship_capi.call_status(
        "nest_change_nest", _relationship_capi.file_handle(file), options
    )
