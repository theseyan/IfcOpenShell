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


def assign_pset(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    pset: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Assign property set to provided elements.

    This method can be used to make psets shared by multiple elements.

    :param products: Elements (or element types) to assign the pset to.
    :param pset: Property set.
    :return: None if `products` is empty or has only type elements.
        IfcRelDefinesByProperties if `products` contains occurrences.

    Example:

    .. code:: python

        element = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")
        ifcopenshell.api.pset.assign_pset(model, [element], pset)
        # Pset is now assigned.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element}

        element1 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")
        ifcopenshell.api.pset.assign_pset(model, [element1, element2], pset)
        # Pset is now shared by multiple elements.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element, element1, element2}

        # Same for element types.
        element_type = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWallType")
        ifcopenshell.api.pset.assign_pset(model, [element_type], type_pset)
        # Pset is now assigned to the type.
        assert ifcopenshell.util.element.get_elements_by_pset(type_pset) == {element_type}
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    handle = _capi.pset_assign_pset(
        file._handle,
        {
            "products": product_list,
            "pset": pset._handle,
            "owner_history": None,
            "user": user._handle if user is not None else None,
            "application": application._handle if application is not None else None,
        },
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    return None
