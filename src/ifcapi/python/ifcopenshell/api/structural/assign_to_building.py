# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2026 Dion Moult <dion@thinkmoult.com>
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
# This file was generated with the assistance of an AI coding tool.

import ifcopenshell
from ifcopenshell.api.structural import _capi


def assign_to_building(
    file: ifcopenshell.file,
    structural_analysis_model: ifcopenshell.entity_instance,
    building: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Associates a structural analysis model with a building via IfcRelServicesBuildings

    The existing :func:`assign_structural_analysis_model` handles
    IfcRelAssignsToGroup (linking structural members to the analysis model).
    This function handles the separate model-to-building relationship, which
    records which building the structural analysis model serves.

    :param structural_analysis_model: The IfcStructuralAnalysisModel to
        associate with the building.
    :param building: The IfcBuilding (or other IfcSpatialStructureElement)
        that the structural analysis model serves.
    :return: The IfcRelServicesBuildings relationship.

    Example:

    .. code:: python

        building = ifcopenshell.util.selector.filter_elements(model, "IfcBuilding")[0]
        model_ = ifcopenshell.api.structural.add_structural_analysis_model(model)
        ifcopenshell.api.structural.assign_to_building(model,
            structural_analysis_model=model_, building=building)
    """
    lib = _capi.get_lib()
    owner_history = _capi.owner_history(file)
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_structural_assign_to_building,
        _capi.file_handle(file),
        _capi.instance_handle(structural_analysis_model),
        _capi.instance_handle(building),
        _capi.instance_handle(owner_history),
    )
