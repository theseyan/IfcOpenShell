# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021-2022, Dion Moult <dion@thinkmoult.com>, Yassine Oualid <yassine@sigmadimensions.com>
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
from ifcopenshell.api.resource import _capi


def remove_resource(
    file: ifcopenshell.file, resource: ifcopenshell.entity_instance
) -> None:
    """Removes a resource and all relationships

    Example:

    .. code:: python

        # Add our own crew
        crew = ifcopenshell.api.resource.add_resource(model, ifc_class="IfcCrewResource")

        # Fire our crew
        ifcopenshell.api.resource.remove_resource(model, resource=crew)
    """
    owner_options, owner_refs = _capi.owner_options(file)
    options = {"resource": _capi.instance_handle(resource), **owner_options}
    _capi.call_status("resource_remove_resource", _capi.file_handle(file), options)
    _capi.invalidate(resource)
