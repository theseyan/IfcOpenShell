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


def copy_material(
    file: ifcopenshell.file, material: ifcopenshell.entity_instance
) -> ifcopenshell.entity_instance:
    """Copies a material or material set

    All material psets and styles are copied. The copied material is not
    associated to any elements.

    If a material set is copied, the set items are also copied. However the
    underlying materials (and profiles) used within the set items are reused.

    If a material is associated with a presentation style, that presentation
    style is reused.

    :param material: The IfcMaterialDefinition to copy
    :return: The new copy of the material

    Example:

    .. code:: python

        concrete = ifcopenshell.api.material.add_material(model, name="CON01", category="concrete")

        # Let's duplicate the concrete material
        concrete_copy = ifcopenshell.api.material.copy_material(model, material=concrete)
    """
    return _capi.call_handle(
        file,
        "material_copy_material",
        "Failed to copy material",
        _capi.file_handle(file),
        _capi.instance_handle(material),
    )
