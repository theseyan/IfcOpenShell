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
import ifcopenshell.api.owner
from ifcopenshell import _ifcopenshell_capi as _capi


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
    owner_history = ifcopenshell.api.owner.create_owner_history(file)
    handle = _capi.system_add_system(
        file._handle,
        ifc_class,
        owner_history._handle if owner_history is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to add system")
