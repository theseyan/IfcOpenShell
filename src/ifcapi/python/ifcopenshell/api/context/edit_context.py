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

from typing import Any

import ifcopenshell
from ifcopenshell.api.context import _capi
from ifcopenshell.api.pset import _capi as pset_capi


def edit_context(file: ifcopenshell.file, context: ifcopenshell.entity_instance, attributes: dict[str, Any]) -> None:
    """Edits the attributes of an IfcGeometricRepresentationContext

    For more information about the attributes and data types of an
    IfcGeometricRepresentationContext, consult the IFC documentation.

    :param context: The IfcGeometricRepresentationContext entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        model = ifcopenshell.api.context.add_context(model, context_type="Model")
        # Revit had a bug where they incorrectly called the body representation a "Facetation"
        body = ifcopenshell.api.context.add_context(model,
            context_type="Model", context_identifier="Facetation", target_view="MODEL_VIEW", parent=model
        )

        # Let's fix it!
        ifcopenshell.api.context.edit_context(model,
            context=body, attributes={"ContextIdentifier": "Body"})
    """
    props = pset_capi.build_props(attributes)
    try:
        _capi.call_status(
            "context_edit_context",
            "context_edit_context failed",
            _capi.file_handle(file),
            _capi.instance_handle(context),
            props,
        )
    finally:
        pset_capi.free_props(props)
