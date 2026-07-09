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


def unassign_structural_analysis_model(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    structural_analysis_model: ifcopenshell.entity_instance,
) -> None:
    """Removes a relationship between a structural element and the analysis model

    :param products: The structural elements that is part of the analysis.
    :param structural_analysis_model: The IfcStructuralAnalysisModel that
        the structural element is related to.
    :return: None
    """
    user, application = _capi.owner_user_application(file)
    _capi.call_status(
        "structural_unassign_structural_analysis_model",
        _capi.file_handle(file),
        _capi.instance_list(products),
        _capi.instance_handle(structural_analysis_model),
        {
            "user": _capi.instance_handle(user),
            "application": _capi.instance_handle(application),
        },
    )
