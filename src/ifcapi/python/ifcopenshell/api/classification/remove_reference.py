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


def remove_reference(
    file: ifcopenshell.file,
    reference: ifcopenshell.entity_instance,
    products: list[ifcopenshell.entity_instance],
) -> None:
    """Removes a classification reference from the list of products

    If the classification reference is no longer associated to any products,
    the classification reference itself is also removed.

    :param reference: The IfcClassificationReference entity of the
        relationship you want to remove.
    :param product: The list fo object entities of the relationship you want to
        remove.

    :raises TypeError: If file is IFC2X3 and `products` has non-IfcRoot elements.

    :return: None

    Example:

    .. code:: python

        wall_type = model.by_type("IfcWallType")[0]
        classification = ifcopenshell.api.classification.add_classification(
            model, classification="MyCustomClassification")
        reference = ifcopenshell.api.classification.add_reference(model,
            products=[wall_type], classification=classification,
            identification="W_01", name="Interior Walls")
        ifcopenshell.api.classification.remove_reference(model,
            reference=reference, products=[wall_type])
    """
    product_list = _relationship_capi.instance_list(products)
    user, application = _relationship_capi.owner_user_application(file)
    _relationship_capi.call_status(
        "classification_remove_reference",
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_handle(reference),
        _relationship_capi.instance_list_ptr(product_list),
        _relationship_capi.instance_handle(user),
        _relationship_capi.instance_handle(application),
    )
