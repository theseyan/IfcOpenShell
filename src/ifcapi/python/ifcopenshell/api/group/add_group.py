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
from ifcopenshell import _generated_capi
from ifcopenshell.api import _relationship_capi


def add_group(
    file: ifcopenshell.file, name: str = "Unnamed", description: Optional[str] = None
) -> ifcopenshell.entity_instance:
    """Adds a new group

    An IFC group is an arbitrary collection of products, which are typically
    physical. It may be used when there is no other more specific group
    which may be used. Other types of groups include distribution systems,
    which group together products that are connected and circulate a medium
    (such as fluid or electricity), or zones, which group together spaces,
    or structural load groups, which group together loads for structural
    analysis, or inventories, which are groups of assets.

    :param name: The name of the group. Defaults to "Unnamed"
    :param description: The description of the purpose of the group.
    :return: The newly created IfcGroup

    Example:

    .. code:: python

        ifcopenshell.api.group.add_group(model, name="Unit 1A")
    """

    lib = _relationship_capi.get_lib()
    owner_history, user, application = _relationship_capi.owner_context(file)
    return _relationship_capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_group_add_group,
        _relationship_capi.file_handle(file),
        _generated_capi.encode_string(name),
        _generated_capi.encode_string(description) if description is not None else None,
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
