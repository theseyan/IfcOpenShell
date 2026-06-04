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
from ifcopenshell import _ifcopenshell_capi as _capi


def remove_information(file: ifcopenshell.file, information: ifcopenshell.entity_instance) -> None:
    """Removes a document information

    All references and associations are also removed.

    :param information: The IfcDocumentInformation to remove
    :return: None

    Example:

    .. code:: python

        # Add a document
        document = ifcopenshell.api.document.add_information(model)
        # ... and remove it!
        ifcopenshell.api.document.remove_information(model, information=document)
    """
    _capi.ifcopenshell_ifcapi_document_remove_information(file._handle, information._handle)
