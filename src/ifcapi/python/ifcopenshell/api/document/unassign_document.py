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
from ifcopenshell import _generated_capi
from ifcopenshell.api.document import _capi


def unassign_document(
    file: ifcopenshell.file,
    products: list[ifcopenshell.entity_instance],
    document: ifcopenshell.entity_instance,
) -> None:
    """Unassigns a document and an association to the list of products

    :param product: The list of objects that the document reference or information is
        related to.
    :param document: The IfcDocumentReference (typically) or in rare cases
        the IfcDocumentInformation that is associated with the product
    :return: None

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

        # Now let's change our mind and remove the association
        ifcopenshell.api.document.unassign_document(model, products=[storey], document=reference)
    """

    lib = _capi.get_lib()
    _, user, application = _capi.owner_context(file)
    product_list = _capi.instance_list(products)
    _generated_capi.status_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_document_unassign_document(
            _capi.file_handle(file),
            product_list,
            _capi.instance_handle(document),
            _capi.instance_handle(user),
            _capi.instance_handle(application),
        ),
        "Failed to unassign document",
    )
