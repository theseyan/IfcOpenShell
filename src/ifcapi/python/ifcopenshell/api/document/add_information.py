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

from typing import Optional

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def add_information(
    file: ifcopenshell.file, parent: Optional[ifcopenshell.entity_instance] = None
) -> ifcopenshell.entity_instance:
    """Adds a new document information to the project

    An IFC document information is a document associated with the project.
    It may be a drawing, specification, schedule, certificate, warranty
    guarantee, manual, contract, and so on. They are often used for drawings
    and facility management purposes.

    A document may also be a subdocument of a larger document, this is
    useful for superseding documents or tracking older versions. The parent
    is considered the latest version and the children are older revisions.

    :param parent: The parent document, if necessary.
    :return: The newly created IfcDocumentInformation entity

    Example:

    .. code:: python

        document = ifcopenshell.api.document.add_information(model)
        # A document typically has a unique drawing or document name (which
        # follows a coding system depending on the project), as well as a
        # title.  This should match what is shown on the titleblock or title
        # page of the document. At a minimum you'd also want to specify a
        # URI location. The location may be on local, or on a CDE, or any
        # other platform.
        ifcopenshell.api.document.edit_information(model,
            information=document,
            attributes={"Identification": "A-GA-6100", "Name": "Overall Plan",
            "Location": "A-GA-6100 - Overall Plan.pdf"})
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    handle = _capi.document_add_information(
        file._handle,
        parent._handle if parent is not None else None,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to add document information")
