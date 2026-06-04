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


def remove_metric(file: ifcopenshell.file, metric: ifcopenshell.entity_instance) -> None:
    """Remove a metric benchmark

    Removes a metric benchmark and all of its associations to any products
    and objectives.

    :param metric: The IfcMetric you want to remove.
    :return: None

    Example:

    .. code:: python

        objective = ifcopenshell.api.constraint.add_objective(model)
        metric = ifcopenshell.api.constraint.add_metric(model,
            objective=objective)
        ifcopenshell.api.constraint.remove_metric(model,
            metric=metric)
    """
    _capi.constraint_remove_metric(file._handle, metric._handle)
