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
from ifcopenshell.api.system import _capi


def unassign_flow_control(
    file: ifcopenshell.file,
    relating_flow_element: ifcopenshell.entity_instance,
    related_flow_control: ifcopenshell.entity_instance,
) -> None:
    """Unassigns flow control element from the flow element.

    :param related_flow_control: IfcDistributionControlElement controling the
        flow element
    :param relating_flow_element: The IfcDistributionFlowElement that is being controlled
    :return: None

    Example:

    .. code:: python

        # assign control to the flow element
        flow_element = file.createIfcFlowSegment()
        flow_control = file.createIfcController()
        relation = ifcopenshell.api.system.assign_flow_control(
            file, relating_control=flow_control, related_object=flow_element
        )

        # und unassign it
        ifcopenshell.api.system.unassign_flow_control(file,
            relating_control=flow_control, related_object=flow_element
        )
    """
    _, user, application = _capi.owner_context(file)
    _capi.call_status(
        "system_unassign_flow_control",
        "Failed to unassign flow control",
        _capi.file_handle(file),
        _capi.instance_handle(relating_flow_element),
        _capi.instance_handle(related_flow_control),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
