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
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def unassign_constraint(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    constraint: ifcopenshell.entity_instance,
) -> None:
    """Unassigns a constraint from a list of products

    The constraint will not be deleted and is available to be assigned to
    other products.

    :param products: The list of products the constraint applies to.
    :param constraint: The IfcObjective constraint
    :return: None
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    _capi.constraint_unassign_constraint(
        file._handle,
        product_list,
        constraint._handle,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
