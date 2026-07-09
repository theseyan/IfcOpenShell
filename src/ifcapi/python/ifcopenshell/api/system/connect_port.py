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
import ifcopenshell.api.owner.settings
from ifcopenshell.api.system import _capi


def connect_port(
    file: ifcopenshell.file,
    port1: ifcopenshell.entity_instance,
    port2: ifcopenshell.entity_instance,
    direction: str = "NOTDEFINED",
    element: Optional[ifcopenshell.entity_instance] = None,
) -> None:
    """Connects two ports together

    A distribution element (e.g. a duct) may be connected to another
    distribution element (e.g. a fitting) by connecting a port at one of the
    duct to a port at the same end of the fitting.

    Ports may only have one connection, so you cannot have multiple things
    connected to the same port. Nor can you have incompatible port
    connections, such as an electrical port connected to an airflow port.

    Port connectivity may be explicit or implicit. Explicit connections are
    where the port connectivity is described for every single distribution
    element in detail. For example, a duct segment would have port
    connections to a duct fitting, which would have port connections to
    another duct segment, all the way from a fan to an air terminal exactly
    as constructed on site. Implicit connections only consider the key
    distribution control elements (e.g. the fan and the terminal) and ignore
    all of the details of the duct segments and fittings in between.
    Generally, explicit connectivity is preferred for later detailed design,
    and implicit connectivity is preferred for early phase design.

    :param port1: The port of the first distribution element to connect.
    :param port2: The port of the second distribution element to connect.
    :param direction: The directionality of distribution flow through the
        port connection. NOTDEFINED means that the direction has not yet
        been determined. This is useful during preliminary system design.
        SOURCE means that the flow is from the first element to the second
        element. SINK means that the flow is from the second element to the
        first element. SOURCEANDSINK means that flow is bi-directional
        between the first and second element.  SOURCEANDSINK is a relatively
        rare scenario.
    :param element: Optionally set an element through which the port
        connectivity is made, such as a segment or fitting. This is only to
        be used for implicit port connectivity where the segments and
        fittings are less important.

    Example:

    .. code:: python

        # A completely empty distribution system
        system = ifcopenshell.api.system.add_system(model)

        # Create a duct and a 90 degree bend fitting
        duct = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctSegment", predefined_type="RIGIDSEGMENT")
        fitting = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctFitting", predefined_type="BEND")

        # The duct and fitting is part of the system
        ifcopenshell.api.system.assign_system(model, products=[duct], system=system)
        ifcopenshell.api.system.assign_system(model, products=[fitting], system=system)

        # Create 2 ports, one for either end of both the duct and fitting.
        duct_port1 = ifcopenshell.api.system.add_port(model, element=duct)
        duct_port2 = ifcopenshell.api.system.add_port(model, element=duct)
        fitting_port1 = ifcopenshell.api.system.add_port(model, element=fitting)
        fitting_port2 = ifcopenshell.api.system.add_port(model, element=fitting)

        # Connect the duct and fitting together. At this point, we have not
        # yet determined the direction of the flow, so we leave direction as
        # NOTDEFINED.
        ifcopenshell.api.system.connect_port(model, port1=duct_port2, port2=fitting_port1)
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    _capi.call_status(
        "system_connect_port",
        "Failed to connect port",
        _capi.file_handle(file),
        {
            "port1": _capi.instance_handle(port1),
            "port2": _capi.instance_handle(port2),
            "direction": direction,
            "element": _capi.instance_handle(element),
            "owner_history": None,
            "user": _capi.instance_handle(user),
            "application": _capi.instance_handle(application),
        },
    )
