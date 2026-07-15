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
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell.api.pset import _capi as pset_capi


def edit_prop_template(
    file: ifcopenshell.file,
    prop_template: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
) -> None:
    """Edits the attributes of an IfcSimplePropertyTemplate

    For more information about the attributes and data types of an
    IfcSimplePropertyTemplate, consult the IFC documentation.

    :param prop_template: The IfcSimplePropertyTemplate entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        template = ifcopenshell.api.pset_template.add_pset_template(model, name="ABC_RiskFactors")

        # Here's a property with just default values.
        prop = ifcopenshell.api.pset_template.add_prop_template(model, pset_template=template)

        # Let's edit it to give the actual values we need.
        ifcopenshell.api.pset_template.edit_prop_template(model,
            prop_template=prop, attributes={"Name": "DemoA", "PrimaryMeasureType": "IfcLengthMeasure"})
    """
    props = pset_capi.build_props(attributes)
    try:
        _capi.pset_template_edit_prop_template(
            file._handle,
            {"prop_template": prop_template._handle, "attributes": props},
        )
    finally:
        pset_capi.free_props(props)
