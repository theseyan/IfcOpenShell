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
from ifcopenshell.api.sequence import _capi


def assign_product(
    file: ifcopenshell.file,
    relating_product: ifcopenshell.entity_instance,
    related_object: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Assigns a product to be produced as a result of a process

    A construction task may result in products (e.g. a wall) being
    constructed. These task "Outputs" are defined in IFC through product
    relationships.

    Not all tasks have Outputs. For example, maintenance tasks will
    typically not have any outputs.

    See ifcopenshell.api.sequence.assign_process for Inputs and other types
    of process relationships that can be described in manufacturing
    process modeling.

    :param relating_product: The IfcProduct that was constructed as a result
        of the task.
    :param related_object: The IfcProcess (typically IfcTask) of the
        construction task.
    :return: The newly created IfcRelAssignsToProduct relationship

    Example:

    .. code:: python

        # Let's imagine we are creating a construction schedule. All tasks
        # need to be part of a work schedule.
        schedule = ifcopenshell.api.sequence.add_work_schedule(model, name="Construction Schedule A")

        # Let's create a construction task. Note that the predefined type is
        # important to distinguish types of tasks.
        task = ifcopenshell.api.sequence.add_task(model,
            work_schedule=schedule, name="Build wall", identification="A", predefined_type="CONSTRUCTION")

        # Let's say we have a wall somewhere.
        wall = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # Let's construct that wall!
        ifcopenshell.api.sequence.assign_product(model, relating_product=wall, related_object=task)
    """
    owner_history, user, application = _capi.owner_context(file)
    return _capi.call_handle(
        file,
        _capi.get_lib().ifcopenshell_ifcapi_sequence_assign_product,
        _capi.file_handle(file),
        _capi.instance_handle(relating_product),
        _capi.instance_handle(related_object),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
        nullable=True,
    )
