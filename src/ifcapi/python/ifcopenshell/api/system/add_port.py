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
from ifcopenshell.api.system import _capi


def add_port(
    file: ifcopenshell.file, element: Optional[ifcopenshell.entity_instance] = None
) -> ifcopenshell.entity_instance:
    """Adds a new distribution port to an element

    A distribution port represents a connection point on an element, where
    a distribution element may be connected to another distribution element.
    For example, a duct segment will typically have two ports, one at either
    end, because you can attach another segment or fitting to either end of
    the duct segment.

    This will both add a distribution port and automatically assign it to a
    distribution element.

    :param element: The IfcDistributionElement you want to add a
        distribution port to.
    :return: The newly created IfcDistributionPort

    Example:

    .. code:: python

        # Create a duct
        duct = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctSegment", predefined_type="RIGIDSEGMENT")

        # Create 2 ports, one for either end.
        port1 = ifcopenshell.api.system.add_port(model, element=duct)
        port2 = ifcopenshell.api.system.add_port(model, element=duct)
    """
    lib = _capi.get_lib()
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_system_add_port,
        "Failed to add port",
        _capi.file_handle(file),
        _capi.instance_handle(element),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
