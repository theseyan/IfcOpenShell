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
from ifcopenshell.api.material import _capi


def remove_material(file: ifcopenshell.file, material: ifcopenshell.entity_instance) -> None:
    """Removes a material

    If the material is used in a material set, the corresponding layer,
    profile, or constituent is also removed. Note that this may result in a
    material set with zero items in it, which is invalid, so the user must
    take care of this situation themselves.

    :param material: The IfcMaterial entity you want to remove
    :return: None

    Example:

    .. code:: python

        # Create a material
        aluminium = ifcopenshell.api.material.add_material(model, name="AL01", category="aluminium")

        # ... and remove it
        ifcopenshell.api.material.remove_material(model, material=aluminium)
    """
    lib = _capi.get_lib()
    _capi.call_status(
        lib.ifcopenshell_ifcapi_material_remove_material,
        "Failed to remove material",
        _capi.file_handle(file),
        _capi.instance_handle(material),
    )
