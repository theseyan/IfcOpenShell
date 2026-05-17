# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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
from ifcopenshell.api import _relationship_capi


def dereference_structure(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    relating_structure: ifcopenshell.entity_instance,
) -> None:
    """Dereferences a list of products and space

    :param products: The list of physical IfcElements that exists in the space.
    :param relating_structure: The IfcSpatialStructureElement element, such
        as IfcBuilding, IfcBuildingStorey, or IfcSpace that the element
        exists in.
    :return: None

    Example:

    .. code:: python

        project = ifcopenshell.api.root.create_entity(model, ifc_class="IfcProject")
        site = ifcopenshell.api.root.create_entity(model, ifc_class="IfcSite")
        building = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuilding")
        storey1 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")
        storey2 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")
        storey3 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")

        # The project contains a site (note that project aggregation is a special case in IFC)
        ifcopenshell.api.aggregate.assign_object(model, products=[site], relating_object=project)

        # The site has a building, the building has a storey, and the storey has a space
        ifcopenshell.api.aggregate.assign_object(model, products=[building], relating_object=site)
        ifcopenshell.api.aggregate.assign_object(model, products=[storey], relating_object=building)
        ifcopenshell.api.aggregate.assign_object(model, products=[space], relating_object=storey)

        # Create a column, this column spans 3 storeys
        column = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # The column is contained in the lowermost storey
        ifcopenshell.api.spatial.assign_container(model, products=[column], relating_structure=storey1)

        # And referenced in the others
        ifcopenshell.api.spatial.reference_structure(model, products=[column], relating_structure=storey2)
        ifcopenshell.api.spatial.reference_structure(model, products=[column], relating_structure=storey3)

        # Actually, it only goes up to storey 2.
        ifcopenshell.api.spatial.dereference_structure(model, products=[column], relating_structure=storey3)
    """
    product_list = _relationship_capi.instance_list(products)
    user, application = _relationship_capi.owner_user_application(file)
    lib = _relationship_capi.get_lib()
    _relationship_capi.call_status(
        lib.ifcopenshell_ifcapi_spatial_dereference_structure,
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_list_ptr(product_list),
        _relationship_capi.instance_handle(relating_structure),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
