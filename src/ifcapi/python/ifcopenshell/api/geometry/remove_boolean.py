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
from ifcopenshell.api.geometry import _capi


def remove_boolean(file: ifcopenshell.file, item: ifcopenshell.entity_instance) -> None:
    """Removes a boolean operation without deleting the operands

    The first operand will replace the boolean result itself, and the second
    operand will be reset as a top level representation item.

    This may affect the Items of IfcShapeRepresentation, so it is recommended
    to run :func:`ifcopenshell.api.geometry.validate_type` after all boolean
    modifications are complete.

    :param item: This may either be an IfcBooleanResult or an
        IfcRepresentationItem that is participating in one or more boolean
        results (in which case all are removed).
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_geometry_remove_boolean,
        _capi.file_handle(file),
        _capi.instance_handle(item),
    )
