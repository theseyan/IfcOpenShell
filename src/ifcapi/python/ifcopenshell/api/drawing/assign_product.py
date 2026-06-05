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
from ifcopenshell.api import _relationship_capi


def assign_product(
    file: ifcopenshell.file,
    relating_product: ifcopenshell.entity_instance,
    related_object: ifcopenshell.entity_instance,
) -> ifcopenshell.entity_instance:
    """Associates a product and an object, typically for annotation

    Warning: this is an experimental API.

    When you want to draw attention to a feature or characteristic (such as
    a dimension, material, or name) or of a product (e.g. wall, slab,
    furniture, etc), an annotation object is created. This annotation is
    then associated with the product so that it can reference attributes,
    properties, and relationships.

    For example, an annotation of a line will be associated with a grid
    axis, such that when that grid axis moves, the annotation of that grid
    axis (which is typically truncated to the extents of a drawing) will
    also move.

    Another example might be a label of a furniture product, which might
    have some text of the name of the furniture to be shown on drawings or
    in 3D.

    :param relating_product: The IfcProduct the object is related to
    :param related_object: The object (typically IfcAnnotation) that the
        product is related to
    :return: The created IfcRelAssignsToProduct relationship

    Example:

    .. code:: python

        furniture = ifcopenshell.api.root.create_entity(model, ifc_class="IfcFurniture")
        annotation = ifcopenshell.api.root.create_entity(model, ifc_class="IfcAnnotation")
        ifcopenshell.api.drawing.assign_product(model,
            relating_product=furniture, related_object=annotation)
    """
    owner_history, user, application = _relationship_capi.owner_context(file)
    return _relationship_capi.call_handle(
        file,
        "drawing_assign_product",
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_handle(relating_product),
        _relationship_capi.instance_handle(related_object),
        _relationship_capi.instance_handle(owner_history),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
