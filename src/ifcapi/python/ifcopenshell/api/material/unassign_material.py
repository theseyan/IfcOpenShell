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


def unassign_material(file: ifcopenshell.file, products: list[ifcopenshell.entity_instance]) -> None:
    """Removes any material relationship with the list of products

    A product can only have one material assigned to it, which is why it is
    not necessary to specify the material to unassign. The material is not
    removed, only the relationship is removed.

    If the product does not have a material, nothing happens.

    Unassigning a LayerSet or ProfileSet from the product type will also
    remove all Usages of the set.

    :param products: The list IfcProducts that may or may not have a material
    :return: None

    Example:

    .. code:: python

        concrete = ifcopenshell.api.material.add_material(model, name="CON01", category="concrete")

        # Let's imagine a concrete bench made out of concrete.
        bench_type = ifcopenshell.api.root.create_entity(model, ifc_class="IfcFurnitureType")
        ifcopenshell.api.material.assign_material(model,
            products=[bench_type], type="IfcMaterial", material=concrete)

        # Let's change our mind and remove the concrete assignment. The
        # concrete material still exists, but the bench is no longer made
        # out of concrete now.
        ifcopenshell.api.material.unassign_material(model, products=[bench_type])
    """
    lib = _capi.get_lib()
    _, user, application = _capi.owner_context(file)
    product_list = _capi.instance_list(products)
    _capi.call_status(
        lib.ifcopenshell_ifcapi_material_unassign_material,
        "Failed to unassign material",
        _capi.file_handle(file),
        product_list,
        _capi.instance_handle(user),
        _capi.instance_handle(application),
    )
