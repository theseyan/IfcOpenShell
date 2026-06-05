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
from ifcopenshell.api.owner import _capi


def assign_actor(
    file: ifcopenshell.file, relating_actor: ifcopenshell.entity_instance, related_object: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    """Assigns an actor to an object

    An actor may be assigned to objects which implies that the actor is
    responsible for. This is most commonly used in facility management for
    indicating the manufacturers, suppliers, and warrantors for product
    types.

    Here are a list of objects you may assign an actor to:

    * IfcControl: Indicates project directives issued by the actor.
    * IfcGroup: Indicates groups for which the actor is responsible.
    * IfcProduct: Indicates products for which the actor is responsible.
    * IfcProcess: Indicates processes for which the actor is responsible.
    * IfcResource: Indicates resources for which the actor is responsible to
        allocate, manage, or delegate. This is not the actor actually using
        the resource or performing the work. For that type of actor, see
        ifcopenshell.api.resource.assign_resource.

    :param relating_actor: The IfcActor who is responsible for the object.
    :param related_object: The object the actor is responsible for.
    :return: The newly created IfcRelAssignsToActor relationship.

    Example:

    .. code:: python

        # We need to procure and install 2 of this particular pump type in our facility.
        pump_type = ifcopenshell.api.root.create_entity(model, ifc_class="IfcPumpType")

        # Define who the manufacturer is
        manufacturer = ifcopenshell.api.owner.add_organisation(model,
            identification="PWP", name="Pumps With Power")
        ifcopenshell.api.owner.add_role(model, assigned_object=manufacturer, role="MANUFACTURER")

        # To help our facility manager, it's nice to provide contact details
        # of the manufacturer so they know how to call when the pump breaks.
        telecom = ifcopenshell.api.owner.add_address(model,
            assigned_object=organisation, ifc_class="IfcTelecomAddress")
        ifcopenshell.api.owner.edit_address(model, address=telecom,
            attributes={"Purpose": "OFFICE", "TelephoneNumbers": ["+61432466949"],
            "ElectronicMailAddresses": ["contact@example.com"],
            "WWWHomePageURL": "https://example.com"})

        # Make the manufacturer responsible for that pump type.
        ifcopenshell.api.owner.assign_actor(model,
            relating_actor=manufacturer, related_object=pump_type)
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        "owner_assign_actor",
        "Failed to assign actor",
        _capi.file_handle(file),
        _capi.instance_handle(relating_actor),
        _capi.instance_handle(related_object),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
