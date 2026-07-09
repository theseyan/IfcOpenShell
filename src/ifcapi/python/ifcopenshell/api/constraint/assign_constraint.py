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

from typing import Union

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def assign_constraint(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    constraint: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns a constraint to a list of products

    This assigns a relationship between a product and a constraint, so that
    when a product's properties and quantities do not match the requirements
    of the constraint's metrics, results can be flagged.

    It is assumed (but not explicit in the IFC documentation) that
    constraints are inherited from the type. This way, it is not necessary
    to create lots of constraint assignments.

    :param products: The list of products the constraint applies to. This is anything
        which can have properties or quantities.
    :param constraint: The IfcObjective constraint
    :return: The new or updated IfcRelAssociatesConstraint relationship
        or `None` if `products` was an empty list.
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    handle = _capi.constraint_assign_constraint(
        file._handle,
        {
            "products": product_list,
            "constraint": constraint._handle,
            "owner_history": None,
            "user": user._handle if user is not None else None,
            "application": application._handle if application is not None else None,
        },
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to assign constraint")
