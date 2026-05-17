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
from ifcopenshell.api.structural import _capi


def add_structural_analysis_model(file: ifcopenshell.file) -> ifcopenshell.entity_instance:
    """Add a new structural analysis model

    A structural analysis model is a group of all the loads, reactions,
    structural members, and structural connections required to describe a
    structural analysis model.

    A 3D analytical model is assumed.

    :return: The newly created IfcStructuralAnalysisModel

    Example:

    .. code:: python

        # Create a fresh blank structural analysis
        analysis = ifcopenshell.api.structural.add_structural_analysis_model(model)
    """
    lib = _capi.get_lib()
    owner_history = _capi.owner_history(file)
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_structural_add_structural_analysis_model,
        _capi.file_handle(file),
        _capi.instance_handle(owner_history),
    )
