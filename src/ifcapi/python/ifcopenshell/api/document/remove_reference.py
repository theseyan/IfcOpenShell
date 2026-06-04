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


def remove_reference(file: ifcopenshell.file, reference: ifcopenshell.entity_instance) -> None:
    """Remove a document reference

    All associations with objects are removed.

    :param reference: The IfcDocumentReference to remove
    :return: None

    Example:

    .. code:: python

        document = ifcopenshell.api.document.add_information(model)
        reference = ifcopenshell.api.document.add_reference(model, information=document)
        ifcopenshell.api.document.remove_reference(model, reference=reference)
    """
    _capi.ifcopenshell_ifcapi_document_remove_reference(file._handle, reference._handle)
