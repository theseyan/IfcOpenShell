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
from ifcopenshell.api.resource import _capi


def add_resource(
    file: ifcopenshell.file,
    parent_resource: Optional[ifcopenshell.entity_instance] = None,
    ifc_class: str = "IfcCrewResource",
    name: Optional[str] = None,
    predefined_type: str = "NOTDEFINED",
) -> ifcopenshell.entity_instance:
    """Add a new construction resource

    Construction resources may be managed and connected to cost schedules
    and construction schedules. This allows calculations to be done on
    resource utilisation, cost optimisation (e.g. labour rates), and
    optioneering on build strategies.

    There are typically two types of resources. Crew resources are resources
    where you manage your own crew and you have full control over the
    equipment, labour, products, and materials used by your crew.
    Alternatively, there are subcontractor resources, where you simply
    delegate all the details to a subcontractor and it is not decomposed
    into further levels of detail.

    This means when adding resources, you'd first either add a crew or
    subcontract resource. If it is a crew resource, you'd then add child
    resources to that crew, such as equipment (cranes, excavators, hoists,
    etc), material (wood, concrete, etc), and labour (rigging crews,
    formworkers, etc).

    :param parent_resource: If this is a child resource (typically to a crew
        resource), then nominate the parent IfcConstructionResource here.
    :param ifc_class: The class of resource chosen from
        IfcConstructionEquipmentResource, IfcConstructionMaterialResource,
        IfcConstructionProductResource, IfcCrewResource, IfcLaborResource,
        or IfcSubContractResource.
    :param name: The name of the resource
    :param predefined_type: Consult the IFC documentation for the valid
        predefined types for each type of resource class.
    :return: The newly created resource depending on the nominated IFC
        class.

    Example:

    .. code:: python

        # Add our own crew
        crew = ifcopenshell.api.resource.add_resource(model, ifc_class="IfcCrewResource")

        # Add some labour to our crew.
        ifcopenshell.api.resource.add_resource(model, parent_resource=crew, ifc_class="IfcLaborResource")
    """

    owner_options, owner_refs = _capi.owner_options(file)
    options = {
        "ifc_class": ifc_class,
        "name": name or "Unnamed",
        "predefined_type": predefined_type,
        **owner_options,
    }
    if parent_resource is not None:
        options["parent_resource"] = _capi.instance_handle(parent_resource)
    return _capi.call_handle(
        file, "resource_add_resource", _capi.file_handle(file), options
    )
