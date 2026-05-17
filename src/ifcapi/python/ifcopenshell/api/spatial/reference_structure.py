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

from typing import Union

import ifcopenshell
from ifcopenshell.api import _relationship_capi


def reference_structure(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    relating_structure: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Denote that a list products is related to a list of spatial structures

    This is similar to ifcopenshell.api.spatial.assign_container, except
    that containment can only occur between a product and a single spatial
    structure element. This is fine if a wall is on level 1, but not
    appropriate if you have a multistorey column on multiple levels, or a
    door with a to and from space, or a stair going from one floor to
    another floor. This is where spatial referencing is used.

    Typically, the product will be contained in the lowermost, constructed
    first, or primarily accessible space. For a multistorey column or stair,
    the column or stair will therefore be contained in the lowermost storey.
    Then, any other storeys will be referenced.

    Referencing is non-hierarchical, so a door may be referenced in multiple
    spaces simultaneously.

    :param products: The list of physical IfcElements that exists in the space.
    :param relating_structure: The IfcSpatialStructureElement element, such
        as IfcBuilding, IfcBuildingStorey, or IfcSpace that the element
        exists in.
    :return: The IfcRelReferencedInSpatialStructure relationship instance
        or `None` if `products` was an empty list.

    Example:

    .. code:: python

        project = ifcopenshell.api.root.create_entity(model, ifc_class="IfcProject")
        site = ifcopenshell.api.root.create_entity(model, ifc_class="IfcSite")
        building = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuilding")
        storey1 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")
        storey2 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")
        storey3 = ifcopenshell.api.root.create_entity(model, ifc_class="IfcBuildingStorey")
        space = ifcopenshell.api.root.create_entity(model, ifc_class="IfcSpace")

        # The project contains a site (note that project aggregation is a special case in IFC)
        ifcopenshell.api.aggregate.assign_object(model, products=[site], relating_object=project)

        # The site has a building, the building has a storey, and the storey has a space
        ifcopenshell.api.aggregate.assign_object(model, products=[building], relating_object=site)
        ifcopenshell.api.aggregate.assign_object(model, products=[storey1,storey2,storey3], relating_object=building)
        ifcopenshell.api.aggregate.assign_object(model, products=[space], relating_object=storey1)

        # Create a column, this column spans 3 storeys
        column = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")

        # The column is contained in the lowermost storey
        ifcopenshell.api.spatial.assign_container(model, products=[column], relating_structure=storey1)

        # And referenced in the others
        ifcopenshell.api.spatial.reference_structure(
            model, products=[column], relating_structure=storey2
        )

        ifcopenshell.api.spatial.reference_structure(
            model, products=[column], relating_structure=storey3
        )
    """

    lib = _relationship_capi.get_lib()
    owner_history, user, application = _relationship_capi.owner_context(file)
    product_list = _relationship_capi.instance_list(products)
    return _relationship_capi.call_handle(
        file,
        lib.ifcopenshell_ifcapi_spatial_reference_structure,
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_list_ptr(product_list),
        _relationship_capi.instance_handle(relating_structure),
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
