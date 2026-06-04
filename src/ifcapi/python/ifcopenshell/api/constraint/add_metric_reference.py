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


def add_metric_reference(
    file: ifcopenshell.file, metric: ifcopenshell.entity_instance, reference_path: str
) -> list[ifcopenshell.entity_instance]:
    """
    Adds a chain of references to a metric. The reference path is a string of the form "attribute.attribute.attribute"
    Used to reference a value of an attribute of an instance through a metric objective entity.
    """
    out = _capi.ifcopenshell_ifcapi_constraint_add_metric_reference(
        file._handle, metric._handle, reference_path
    )
    return ifcopenshell._take_instance_list(file, out)
