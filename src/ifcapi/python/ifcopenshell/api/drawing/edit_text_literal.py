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
from ifcopenshell.api.attribute.edit_attributes import _edit_named_attributes


def edit_text_literal(
    file: ifcopenshell.file, text_literal: ifcopenshell.entity_instance, attributes: dict[str, Any]
) -> None:
    """Edits the attributes of an IfcTextLiteral

    For more information about the attributes and data types of an
    IfcTextLiteral, consult the IFC documentation.

    :param reference: The IfcTextLiteral entity you want to edit
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        text = model.createIfcTextLiteral()
        ifcopenshell.api.drawing.edit_text_literal(model,
            text_literal=text, attributes={"Literal": "MY ANNOTATION"})
    """
    _edit_named_attributes(file, text_literal, attributes, "drawing_edit_text_literal")
