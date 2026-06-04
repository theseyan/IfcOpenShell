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
from ifcopenshell import _ifcopenshell_capi as _capi


def remove_unit(file: ifcopenshell.file, unit: ifcopenshell.entity_instance) -> None:
    """Remove a unit

    Be very careful when a unit is removed, as it may mean that previously
    defined quantities in the model completely lose their meaning.

    :param unit: The unit element to remove
    :return: None

    Example:

    .. code:: python

        # What?
        unit = ifcopenshell.api.unit.add_context_dependent_unit(model, name="HANDFULS")

        # Yeah maybe not.
        ifcopenshell.api.unit.remove_unit(model, unit=unit)
    """
    _capi.ifcopenshell_ifcapi_unit_remove_unit(file._handle, unit._handle)
