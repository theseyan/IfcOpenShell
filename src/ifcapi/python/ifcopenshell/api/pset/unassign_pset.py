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
from ifcopenshell import _ifcopenshell_capi as _capi


def unassign_pset(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    pset: ifcopenshell.entity_instance,
) -> None:
    """Unassign property set from the provided elements.

    :param products: Elements (or element types) to assign the pset from.
    :param pset: Property set.

    Example:

    .. code:: python

        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.pset.assign_pset(self.file, [element1, element2], pset)

        # Pset is now shared by 2 elements.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element1, element2}

        ifcopenshell.api.pset.unassign_pset(self.file, [element2], pset)
        # Pset was unassigned from element2.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element1}

    """
    product_list = [e._handle for e in products]
    _capi.ifcopenshell_ifcapi_pset_unassign_pset(file._handle, product_list, pset._handle)
