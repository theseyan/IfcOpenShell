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
from ifcopenshell.api.context import _capi


def remove_context(file: ifcopenshell.file, context: ifcopenshell.entity_instance) -> None:
    """Removes an IfcGeometricRepresentationContext

    Any representation geometry that is assigned to the context is also
    removed. If a context is removed, then any subcontexts are also removed.

    :param context: The IfcGeometricRepresentationContext entity to remove
    :return: None

    Example:

    .. code:: python

        model = ifcopenshell.api.context.add_context(model, context_type="Model")
        # Revit had a bug where they incorrectly called the body representation a "Facetation"
        body = ifcopenshell.api.context.add_context(model,
            context_type="Model", context_identifier="Facetation", target_view="MODEL_VIEW", parent=model
        )

        # Let's just get rid of it completely
        ifcopenshell.api.context.remove_context(model, context=body)
    """
    _capi.call_status(
        "context_remove_context",
        "Failed to remove context",
        _capi.file_handle(file),
        _capi.instance_handle(context),
    )
