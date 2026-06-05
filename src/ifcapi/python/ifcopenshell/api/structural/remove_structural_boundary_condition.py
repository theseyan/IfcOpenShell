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
from ifcopenshell.api.structural import _capi


def remove_structural_boundary_condition(
    file: ifcopenshell.file,
    connection: Optional[ifcopenshell.entity_instance] = None,
    boundary_condition: Optional[ifcopenshell.entity_instance] = None,
) -> None:
    """Removes a condition from a connection, or an orphaned boundary condition

    :param connection: The IfcStructuralConnection to remove the condition
        from. If omitted, it is assumed to be an orphaned condition.
    :param boundary_condition: The IfcBoundaryCondition to remove.
    :return: None
    """
    if connection is None:
        assert boundary_condition, "Either connection or boundary_condition must be provided."
    _capi.call_status(
        "structural_remove_structural_boundary_condition",
        _capi.file_handle(file),
        _capi.instance_handle(connection),
        _capi.instance_handle(boundary_condition),
    )
