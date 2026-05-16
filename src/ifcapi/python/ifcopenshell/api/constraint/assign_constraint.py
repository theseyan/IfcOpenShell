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
from ifcopenshell import _generated_capi
from ifcopenshell.api.constraint import _capi


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
    lib = _capi.get_lib()
    owner_history, user, application = _capi.owner_context(file)
    product_list = _capi.instance_list(products)
    handle = _generated_capi.call_handle_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_constraint_assign_constraint,
        "Failed to assign constraint",
        _capi.file_handle(file),
        product_list,
        _capi.instance_handle(constraint),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return _capi.wrap_handle(file, handle)
