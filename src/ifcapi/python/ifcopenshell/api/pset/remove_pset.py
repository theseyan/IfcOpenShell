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


def remove_pset(
    file: ifcopenshell.file, product: ifcopenshell.entity_instance, pset: ifcopenshell.entity_instance
) -> None:
    """Removes a property set from a product

    All properties that are part of this property set are also removed.

    :param product: The IfcObject to remove the property set from.
    :param pset: The IfcPropertySet or IfcElementQuantity to remove.
    :return: None

    Example:

    .. code:: python

        # Let's imagine we have a new wall type with a property set.
        wall_type = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWallType")
        pset = ifcopenshell.api.pset.add_pset(model, product=wall_type, name="Pset_WallCommon")

        # Remove it!
        ifcopenshell.api.pset.remove_pset(model, product=wall_type, pset=pset)
    """
    _capi.pset_remove_pset(file._handle, product._handle, pset._handle)
