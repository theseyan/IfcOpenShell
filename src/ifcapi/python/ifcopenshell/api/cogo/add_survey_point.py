# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2025 Thomas Krijnen <thomas@aecgeeks.com>
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

from typing import Union

import ifcopenshell
from ifcopenshell.api.cogo import _capi
from ifcopenshell import entity_instance


def add_survey_point(
    file: ifcopenshell.file, survey_point: entity_instance, site: Union[entity_instance, None] = None
) -> entity_instance:
    """
    Adds a single survey point to the model based on IFC Concept Template 4.1.7.1.2.5.
    Survey points are located relative to IfcRepresentationContext.WorldCoordinateSystem

    :param survey_point: The survey point
    :return: an IfcAnnotation entity

    Example:

    .. code:: python

        annotation = ifcopenshell.api.cogo.add_survey_point(file,file.createIfcCartesianPoint(4000.0,3500.0)))
    """
    lib = _capi.get_lib()
    user, application = _capi.owner_user_application(file)
    return _capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_cogo_add_survey_point,
        _capi.file_handle(file),
        _capi.instance_handle(survey_point),
        _capi.instance_handle(site),
        None,
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
