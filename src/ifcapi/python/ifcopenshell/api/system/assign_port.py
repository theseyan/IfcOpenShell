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


import ifcopenshell
from ifcopenshell.api.system import _capi


def assign_port(
    file: ifcopenshell.file, element: ifcopenshell.entity_instance, port: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    """Assigns a port to an element

    If you have an orphaned port, you may assign it to a distribution
    element using this function. Ports should typically not be orphaned, but
    it may be useful when patching up models.

    :param element: The IfcDistributionElement to assign the port to.
    :param port: The IfcDistributionPort you want to assign.
    :return: The IfcRelNests relationship, or the
        IfcRelConnectsPortToElement for IFC2X3.

    Example:

    .. code:: python

        # Create a duct
        duct = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcDuctSegment", predefined_type="RIGIDSEGMENT")

        # Create 2 ports, one for either end.
        port1 = ifcopenshell.api.system.add_port(model, element=duct)
        port2 = ifcopenshell.api.system.add_port(model, element=duct)

        # Unassign one port for some weird reason.
        ifcopenshell.api.system.unassign_port(model, element=duct, port=port1)

        # Reassign it back
        ifcopenshell.api.system.assign_port(model, element=duct, port=port1)
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "system_assign_port",
        "Failed to assign port",
        _capi.file_handle(file),
        _capi.instance_handle(element),
        _capi.instance_handle(port),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
