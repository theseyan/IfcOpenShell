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


def remove_classification(file: ifcopenshell.file, classification: ifcopenshell.entity_instance) -> None:
    """Removes an IfcClassification from the project and all references

    The classification and all of its relationships, children references,
    and relationships between objects and child references are completely
    removed from a project.

    :param classification: The IfcClassification entity you want to remove
    :return: None

    Example:

    .. code:: python

        classification = model.by_type("IfcClassification")[0]
        ifcopenshell.api.classification.remove_classification(model,
            classification=classification)
    """
    lib = _relationship_capi.get_lib()
    _relationship_capi.call_status(
        lib.ifcopenshell_ifcapi_classification_remove_classification,
        _relationship_capi.file_handle(file),
        _relationship_capi.instance_handle(classification),
    )
