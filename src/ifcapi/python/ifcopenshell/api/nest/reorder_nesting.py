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


def reorder_nesting(
    file: ifcopenshell.file,
    item: ifcopenshell.entity_instance,
    old_index: int = 0,
    new_index: int = 0,
) -> None:
    """Reorder an item in its nesting set using Python list-compatible indexes."""
    user, application = _relationship_capi.owner_user_application(file)
    options = {"item": _relationship_capi.instance_handle(item), "new_index": new_index}
    if old_index:
        options["old_index"] = old_index
    if user is not None:
        options["user"] = _relationship_capi.instance_handle(user)
    if application is not None:
        options["application"] = _relationship_capi.instance_handle(application)
    _relationship_capi.call_status(
        "nest_reorder_nesting", _relationship_capi.file_handle(file), options
    )
