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
from ifcopenshell.api.constraint import _capi


def remove_constraint(file: ifcopenshell.file, constraint: ifcopenshell.entity_instance) -> None:
    """Remove a constraint (typically an objective)

    Removes a constraint definition and all of its associations to any
    products. Typically this would be an IfcObjective, although technically
    you can associate IfcMetrics ith products too, though the meaning may be
    unclear.

    :param constraint: The IfcObjective you want to remove.
    :return: None

    Example:

    .. code:: python

        objective = ifcopenshell.api.constraint.add_objective(model)
        ifcopenshell.api.constraint.remove_constraint(model,
            constraint=objective)
    """
    lib = _capi.get_lib()
    _generated_capi.status_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_constraint_remove_constraint(
            _capi.file_handle(file),
            _capi.instance_handle(constraint),
        ),
        "Failed to remove constraint",
    )
