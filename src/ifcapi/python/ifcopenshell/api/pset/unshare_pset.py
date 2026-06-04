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


def unshare_pset(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    pset: ifcopenshell.entity_instance,
) -> list[ifcopenshell.entity_instance]:
    """Copy a shared pset as linked only to the provided elements.

    Note that method will create a copy of the pset for each element provided.

    :param products: Elements (or element types) to link the pset to.
    :param pset: Shared property set.
    :return: List of copied property sets.

    Example:

    .. code:: python

        element1 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        element2 = ifcopenshell.api.root.create_entity(self.file, ifc_class="IfcWall")
        ifcopenshell.api.pset.assign_pset(self.file, [element1, element2], pset)

        # Pset is now shared by 2 elements.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element1, element2}

        new_psets = ifcopenshell.api.pset.unshare_pset(self.file, [element2], pset)

        # element2 was unassigned from the original pset.
        assert ifcopenshell.util.element.get_elements_by_pset(pset) == {element1}
        new_pset = new_psets[0]

        # New pset was created and was assigned to element2.
        assert new_pset != pset
        assert ifcopenshell.util.element.get_elements_by_pset(new_pset) == {element2}
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    out = _capi.pset_unshare_pset(
        file._handle,
        product_list,
        pset._handle,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    return ifcopenshell._take_instance_list(file, out)
