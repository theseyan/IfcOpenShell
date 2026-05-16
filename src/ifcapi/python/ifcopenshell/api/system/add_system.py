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
from ifcopenshell import _generated_capi
from ifcopenshell.api.system import _capi


def add_system(file: ifcopenshell.file, ifc_class: str = "IfcDistributionSystem") -> ifcopenshell.entity_instance:
    """Add a new distribution system

    A distribution system is a group of distribution elements, like ducts,
    pipes, pumps, filters, fans, and so on that distribute a medium (air,
    liquid, or electricity) throughout a facility. Systems may be
    hierarchical, with larger systems composed of smaller subsystems.

    :param ifc_class: The type of system, chosen from IfcDistributionSystem
        for mechanical, electrical, communications, plumbing, fire, or
        security systems. Alternatively you may choose IfcBuildingSystem for
        specialised building facade systems or similar. For IFC2X3, choose
        IfcSystem.
    :return: The newly created IfcSystem.

    Example:

    .. code:: python

        # A completely empty distribution system
        system = ifcopenshell.api.system.add_system(model)
    """
    lib = _capi.get_lib()
    owner_history = _capi.create_owner_history(file)
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_system_add_system,
        "Failed to add system",
        _capi.file_handle(file),
        _generated_capi.encode_string(ifc_class),
        _capi.instance_handle(owner_history),
    )
