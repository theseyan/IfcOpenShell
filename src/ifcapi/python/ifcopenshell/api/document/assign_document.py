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

from typing import Union

import ifcopenshell
from ifcopenshell.api import _relationship_capi


def assign_document(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    document: ifcopenshell.entity_instance,
) -> Union[ifcopenshell.entity_instance, None]:
    """Assigns a document to a list of products

    An object may be assigned to zero, one, or multiple documents. Almost
    any object or property may be assigned to a document, though typically
    we'd only use it for spaces, types, physical products and schedules.
    Adding a new assignment is typically done using a document reference and
    an object.  IFC technically allows association with a document
    information and an object, but this is not encouraged because it is not
    consistent with other external relationships (such as classification
    systems or libraries).

    :param product: The list of objects to associate the document to. This could be
        almost any sensible object in IFC.
    :param document: The IfcDocumentReference to associate to, or
        alternatively an IfcDocumentInformation, though this is not
        recommended.
    :return: The IfcRelAssociatesDocument relationship
        or `None` if `products` was an empty list or all products were
        already assigned to the `document`.

    Example:

    .. code:: python

        document = ifcopenshell.api.document.add_information(model)
        ifcopenshell.api.document.edit_information(model,
            information=document,
            attributes={"Identification": "A-GA-6100", "Name": "Overall Plan",
            "Location": "A-GA-6100 - Overall Plan.pdf"})
        reference = ifcopenshell.api.document.add_reference(model, information=document)

        # Let's imagine storey represents an IfcBuildingStorey for the ground floor
        ifcopenshell.api.document.assign_document(model, products=[storey], document=reference)
    """
    user, application = _relationship_capi.owner_user_application(file)
    product_list = _relationship_capi.instance_list(products)
    return _relationship_capi.call_handle(
        file,
        "document_assign_document",
        _relationship_capi.file_handle(file),
        {
            "products": product_list,
            "document": _relationship_capi.instance_handle(document),
            "owner_history": None,
            "user": _relationship_capi.instance_handle(user),
            "application": _relationship_capi.instance_handle(application),
        },
    )
